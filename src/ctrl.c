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

#define MOTOR_CLOCKWISE				RA_TRUE
#define MOTOR_ANTI_CLOCKWISE		RA_FALSE

#define MOTOR_MAX_OPEN_TIME			950		/* 4ms tick =  */
#define MOTOR_MAX_CLOSE_TIME		950
#define MOTOR_MAX_SWING_TIME		400

#define MOTOR_BUSY_STATUS_MASK		0x80
#define MOTOR_POWER_STATUS_MASK		0x40
#define MOTOR_SWING_STATUS_MASK		0x20
#define MOTOR_CMD_MASK				0x0f

/* bit7=motor busy status, bit6=power status, bit5=swing status, bit4=reseval, bit3~bit0=command */
/* this variable initial to 0x41, because it must execuate close process when device power-on */
static uint8_t motor_ctrl_status = MOTOR_POWER_STATUS_MASK | MOTOR_POWER_CMD;

/*
 * send control command, but it must execute last command completly.
 */
bool_t ra_ctrl_cmd(uint8_t cmd)
{
	if (motor_ctrl_status & (MOTOR_BUSY_STATUS_MASK | MOTOR_CMD_MASK))
		return RA_FALSE;

	motor_ctrl_status |= (cmd & MOTOR_CMD_MASK);
	return RA_TRUE;
}

/*
 * get current motor is busy, normally only execute POWER-ON or POWER-OFF if it is return busy.
 */
bool_t ra_ctrl_status(void)
{
	if (motor_ctrl_status & MOTOR_BUSY_STATUS_MASK)
		return RA_TRUE;
	else
		return RA_FALSE;
}

/*
 * execute period is 8ms
 */
void ra_ctrl_period_update(void)
{
	if (motor_ctrl_status & MOTOR_BUSY_STATUS_MASK)
	{
		ra_led_ctrl(LED_ON_MODE);
		
		if (ra_motor_is_running() == RA_FALSE)
			motor_ctrl_status &= (uint8_t)(~MOTOR_BUSY_STATUS_MASK); 
	}
	else
	{
		ra_led_ctrl(LED_OFF_MODE);
		
		if ((motor_ctrl_status & MOTOR_CMD_MASK) == MOTOR_POWER_CMD)
		{
			if (motor_ctrl_status & MOTOR_POWER_STATUS_MASK)	
			{
				/* indicate device power-off */
				motor_ctrl_status &= (uint8_t)(~MOTOR_POWER_STATUS_MASK);
				ra_motor_ctrl(RA_TRUE, RA_FALSE, MOTOR_ANTI_CLOCKWISE, MOTOR_MAX_CLOSE_TIME);
			}
			else
			{
				/* indicate device power-on */
				motor_ctrl_status |= (uint8_t)MOTOR_POWER_STATUS_MASK;
				ra_motor_ctrl(RA_TRUE, RA_FALSE, MOTOR_CLOCKWISE, MOTOR_MAX_OPEN_TIME);
			}

			/* sign refuse operate until finsh current process */
			motor_ctrl_status |= (uint8_t)MOTOR_BUSY_STATUS_MASK;
		}
		else if ((motor_ctrl_status & MOTOR_CMD_MASK) == MOTOR_SWING_CMD)
		{
			if (motor_ctrl_status & MOTOR_POWER_STATUS_MASK)
			{
				if (motor_ctrl_status & MOTOR_SWING_STATUS_MASK)
				{
					/* indicate stop swing */
					motor_ctrl_status &= (uint8_t)(~MOTOR_SWING_STATUS_MASK);
					ra_motor_ctrl(RA_FALSE, RA_TRUE, MOTOR_ANTI_CLOCKWISE, MOTOR_MAX_SWING_TIME);
				}
				else
				{
					/* indicate start swing */
					motor_ctrl_status |= (uint8_t)MOTOR_SWING_STATUS_MASK;
					ra_motor_ctrl(RA_TRUE, RA_TRUE, MOTOR_ANTI_CLOCKWISE, MOTOR_MAX_SWING_TIME);
				}
			}
		}
	}

	motor_ctrl_status &= (uint8_t)(~MOTOR_CMD_MASK);
}



