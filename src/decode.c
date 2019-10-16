/*
 * Copyright (c) 2017-2019, Radiation Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date 		  Author	   Notes
 * 2019-03-29	  Warfalcon	   first implementation
 */


#include "includes.h"

#define RA_GET_BUFFER_BIT(buffer, bit_idx)				(buffer[bit_idx / 8] & (1 << (bit_idx % 8)))

static uint8_t reception_buffer[INFRARED_BUFFER_SIZE + 2];
static uint8_t power_swing = 0x00;

static void ra_decode_send_by_serial(uint8_t *buffer, uint16_t buffer_len)
{
	uint8_t i;
	uint8_t chk;

	/* total length of frame */
	buffer[0] = (buffer_len + 2) & 0x00ff;
	/* calcuate checksum */
	for (chk = 0, i = 0; i < (buffer_len + 1); i++)
		chk ^= buffer[i];
	buffer[buffer_len + 1] = chk;

	ra_uart1_send_bytes(buffer, buffer_len + 2);
}

static void ra_infrared_decode(uint8_t *buffer, uint16_t buffer_len)
{
	uint8_t switch_val;
	static uint8_t keep_val;
	
	switch_val = ra_get_switch();
	/******************************/
	/*         Gree               */
	/******************************/
	if (switch_val == 0x00)			
	{
		/* indicate power on */
		if (buffer[0] & 0x10) 
		{
			/* device in power-off status */
			if ((power_swing & 0xf0) == 0x00)
			{
				power_swing = 0xf0;
				ra_ctrl_cmd(MOTOR_POWER_CMD);
			}
			/* indicate swing enable */
			else if (buffer[0] & 0x02)
			{
				if (((power_swing & 0xf0) != 0x00) && ((power_swing & 0x0f) == 0x00))
				{
					power_swing |= 0x0f;
					ra_ctrl_cmd(MOTOR_SWING_CMD);
				}
			}
			/* indicate swing disable */
			else if (!(buffer[0] & 0x02))
			{
				if (((power_swing & 0xf0) != 0x00) && ((power_swing & 0x0f) != 0x00))
				{
					power_swing &= 0xf0;
					ra_ctrl_cmd(MOTOR_SWING_CMD);
				}
			}
		}
		/* indicate power off */
		else if ((!(buffer[0] & 0x10)) && ((power_swing & 0xf0) != 0x00))
		{
			power_swing = 0x00;
			ra_ctrl_cmd(MOTOR_POWER_CMD);
		}
	}
	/******************************/
	/*         Midea              */
	/******************************/
	else if (switch_val == 0x01)
	{
		if ((buffer[0] == 0xb2) && (buffer[1] == (uint8_t)(~buffer[0])) && (buffer[2] == (uint8_t)(~buffer[3])) && (buffer[4] == (uint8_t)(~buffer[5])))
		{			
			if ((buffer[2] == 0x6b) && (buffer[4] == 0xe0))
			{
				if (((power_swing & 0x0f) == 0x00) && ((power_swing & 0xf0) != 0x00))
				{
					power_swing |= 0x0f;
					ra_ctrl_cmd(MOTOR_SWING_CMD);
				}
				else if (((power_swing & 0x0f) != 0x00) && ((power_swing & 0xf0) != 0x00))
				{
					power_swing &= 0xf0;
					ra_ctrl_cmd(MOTOR_SWING_CMD);
				}
			}
			else if ((buffer[2] == 0x7b) && (buffer[4] == 0xe0) && ((power_swing & 0xf0) != 0x00))
			{
				power_swing = 0x00;
				ra_ctrl_cmd(MOTOR_POWER_CMD);
			}
			else if ((power_swing & 0xf0) == 0x00)
			{
				power_swing = 0xf0;
				ra_ctrl_cmd(MOTOR_POWER_CMD);
			}
		}
	}
	/******************************/
	/*         CHIGO              */
	/******************************/
	else if (switch_val == 0x02)
	{
		if (buffer_len == 12)
		{
			if ((buffer[0] == (uint8_t)(~buffer[1])) && (buffer[2] == (uint8_t)(~buffer[3])) 
				&& (buffer[4] == (uint8_t)(~buffer[5])) && (buffer[6] == (uint8_t)(~buffer[7])) 
				&& (buffer[8] == (uint8_t)(~buffer[9])) && (buffer[10] == (uint8_t)(~buffer[11])))
			{
				if (buffer[6] & 0x40)
				{
					
					if ((power_swing & 0xf0) != 0x00)
					{
						power_swing = 0x00;
						ra_ctrl_cmd(MOTOR_POWER_CMD);
					}					
				}
				else
				{
					if ((power_swing & 0xf0) == 0x00)
					{
						power_swing = 0xf0;
						ra_ctrl_cmd(MOTOR_POWER_CMD);
					}
					else if (buffer[6] & 0x20)
					{
						if ((power_swing & 0x0f) == 0x00)
						{
							power_swing |= 0x0f;
							ra_ctrl_cmd(MOTOR_SWING_CMD);
						}						
					}
					else
					{
						if ((power_swing & 0x0f) != 0x00)
						{
							power_swing &= 0xf0;
							ra_ctrl_cmd(MOTOR_SWING_CMD);
						}
					}
				}
			}
		}
	}
	/******************************/
	/*         海信专用               */
	/******************************/
	else if (switch_val == 0x03)
	{
		if ((buffer_len == 17) && (buffer[0] == 0x02))
		{
			if (buffer[8] & 0x04)
			{
				if ((power_swing & 0xf0) == 0x00)
				{
					power_swing = 0xf0;
					ra_ctrl_cmd(MOTOR_POWER_CMD);
					/* save current value and detect change */
					keep_val = buffer[13];			
				}
				else if (buffer[13] != keep_val)
				{
					/* save current value and detect change */
					keep_val = buffer[13];	

					if ((power_swing & 0x0f) == 0x00)
					{
						power_swing |= 0x0f;
						ra_ctrl_cmd(MOTOR_SWING_CMD);
					}
					else
					{
						power_swing &= 0xf0;
						ra_ctrl_cmd(MOTOR_SWING_CMD);
					}
				}
			}
			else
			{
				if ((power_swing & 0xf0) != 0x00)
				{
					power_swing = 0x00;
					ra_ctrl_cmd(MOTOR_POWER_CMD);
				}
			}
		}
	}
	/******************************/
	/*         KELON              */
	/******************************/
	else if (switch_val == 0x04)
	{
		if ((buffer_len == 21) && (buffer[0] == (uint8_t)(~buffer[1])))
		{
			if (buffer[2] & 0x20)
			{
				if ((power_swing & 0xf0) != 0x00)
				{
					power_swing = 0x00;
					ra_ctrl_cmd(MOTOR_POWER_CMD);
				}
				else
				{
					power_swing = 0xf0;
					ra_ctrl_cmd(MOTOR_POWER_CMD);
				}
			}
			else if ((buffer[8] & 0x03) && ((power_swing & 0xf0) != 0x00))
			{
				if ((power_swing & 0x0f) == 0x00)
				{
					power_swing |= 0x0f;
					ra_ctrl_cmd(MOTOR_SWING_CMD);
				}
				if ((power_swing & 0x0f) != 0x00)
				{
					power_swing &= 0xf0;
					ra_ctrl_cmd(MOTOR_SWING_CMD);
				}
			}
		}
	}
	/******************************/
	/*         Haier              */
	/******************************/
	else if (switch_val == 0x05)
	{
		if ((buffer_len == 14) && (buffer[0] == 0xa6))
		{
			if (buffer[4] & 0x40)
			{
				if ((power_swing & 0xf0) == 0x00)
				{
					power_swing = 0xf0;
					ra_ctrl_cmd(MOTOR_POWER_CMD);
				}
				else if (buffer[12] & 0x03)
				{
					if ((power_swing & 0x0f) == 0x00)
					{
						power_swing |= 0x0f;
						ra_ctrl_cmd(MOTOR_SWING_CMD);
					}
					else if ((power_swing & 0x0f) != 0x00)
					{
						power_swing &= 0xf0;
						ra_ctrl_cmd(MOTOR_SWING_CMD);
					}
				}
			}
			else
			{
				if ((power_swing & 0xf0) != 0x00)
				{
					power_swing = 0x00;
					ra_ctrl_cmd(MOTOR_POWER_CMD);
				}
			}
		}
	}
	/******************************/
	/*         qunda              */
	/******************************/
	else if (switch_val == 0x06)
	{
		if ((buffer_len == 4) && (buffer[0] == 0x03) && (buffer[1] & 0x01))
		{
			if ((power_swing & 0xf0) != 0x00)
			{
				power_swing = 0x00;
				ra_ctrl_cmd(MOTOR_POWER_CMD);
			}
			else
			{
				power_swing = 0xf0;
				ra_ctrl_cmd(MOTOR_POWER_CMD);
			}
		}
		else if ((buffer_len == 2) && (buffer[0] == (uint8_t)(~buffer[1])) && (buffer[0] & 0x0b))
		{
			if (((power_swing & 0x0f) == 0x00) && ((power_swing & 0xf0) != 0x00))
			{
				power_swing |= 0x0f;
				ra_ctrl_cmd(MOTOR_SWING_CMD);
			}
			else if (((power_swing & 0x0f) != 0x00) && ((power_swing & 0xf0) != 0x00))
			{
				power_swing &= 0xf0;
				ra_ctrl_cmd(MOTOR_SWING_CMD);
			}
		}
	}
	/******************************/
	/*         HITACHI            */
	/******************************/
	else if (switch_val == 0x07)
	{
		if (buffer_len == 23)
		{
			if (buffer[21] == (uint8_t)(~buffer[22]))
			{
				if ((buffer[21] == 0x09) && ((power_swing & 0xf0) == 0x00))
				{
					power_swing = 0xf0;
					ra_ctrl_cmd(MOTOR_POWER_CMD);
				}
				else if ((buffer[21] == 0x05) && ((power_swing & 0xf0) != 0x00))
				{
					power_swing = 0x00;
					ra_ctrl_cmd(MOTOR_POWER_CMD);
				}
			}
		}
		else if (buffer_len == 17)
		{
			if (buffer[11] == (uint8_t)(~buffer[12]))
			{
				if ((buffer[11] == 0x70) || (buffer[11] == 0x4e))
				{
					if (((power_swing & 0x0f) == 0x00) && ((power_swing & 0xf0) != 0x00))
					{
						power_swing |= 0x0f;
						ra_ctrl_cmd(MOTOR_SWING_CMD);
					}
					else if (((power_swing & 0x0f) != 0x00) && ((power_swing & 0xf0) != 0x00))
					{
						power_swing &= 0xf0;
						ra_ctrl_cmd(MOTOR_SWING_CMD);
					}
				}
			}
		}
	}
}

void ra_decode_period_update(void)
{
	bool_t  motor_is_busy;
	uint8_t i;
	uint8_t checksum;
	uint8_t reception_len;

	/* if motor is in ON or OFF processing, can not be control */
	motor_is_busy = ra_ctrl_status();
	if (motor_is_busy == RA_FALSE)
	{
		reception_len = infrared_scan(&reception_buffer[1]);
		if (reception_len)
		{
			/* decode and execute */
			ra_infrared_decode(&reception_buffer[1], reception_len);		
			/* send the data to rs485 */
			ra_decode_send_by_serial(reception_buffer, reception_len);
		}	
	}
	
	reception_len = ra_uart1_recv_bytes(reception_buffer, sizeof(reception_buffer));
	if ((reception_len > 3) && (motor_is_busy == RA_FALSE))
	{
		for (checksum = 0, i = 0; i < reception_len; i++)
			checksum ^= reception_buffer[i];
		
		if (checksum == 0x00)
			ra_infrared_decode(&reception_buffer[1], reception_len - 2);
	}

}

