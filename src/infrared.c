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

typedef struct protocal_stru
{
	/* start plus time scale */	
	uint16_t start_plus_dn;
	uint16_t start_plus_up;
	/* connect plus time scale */
	uint16_t connt_plus_dn;
	uint16_t connt_plus_up;
	/* logic 1 time scale */
	uint16_t logic_1_plus_dn;
	uint16_t logic_1_plus_up;
	/* logic 0 time scale */
	uint16_t logic_0_plus_dn;
	uint16_t logic_0_plus_up;
} ra_ir_protocal;

/* bit7: received boot flag */
/* bit6: decode a key value */
/* bit5: reserval */
/* bit4: capture rising edge */
static uint8_t infrared_status = 0x00;
static uint8_t infrared_recv_len = 0x00;
static uint8_t b_is_rising = RA_TRUE;
static uint16_t infrared_index = 0;

static uint8_t infrared_buffer[INFRARED_BUFFER_SIZE];


const static ra_ir_protocal protocal[10] = {
	/* S: 4477     C: 20059      1: 1646     0: 573     Gree */
	{4000, 5000, 15000, 30000, 1500, 1800, 300, 800},
	/* S: 4395     C: 5278      1: 1651     0: 550     Midea */
	{4000, 5000,  4500,  6000, 1500, 1800, 300, 800},
	/* S: 7394     C: 7456      1: 1635     0: 597     CHIGO */
	{6800, 7800,  6500,  8500, 1500, 1800, 300, 800},
	/* S: 1627     C: 77006      1: 1242     0: 434     海信通用: 有3段, 使用第1段            */
	{1550, 1800,  60000,  65000, 1000, 1500, 300, 800},
	/* S: 4502     C: 8024      1: 1672     0: 572     KELON */
	{4000, 5000,  7500,  8500, 1500, 1800, 300, 800},
	/* S: 4447     C: 3068      1: 1662     0: 569     Haier */
	{4000, 5000,  2800,  3300, 1500, 1800, 300, 800},
	/* S: 4497     C: 82565     1: 1696     0: 636     qunda: 有4段, 使用第1段 */
	{4000, 5000,  60000,  65000, 1500, 1800, 300, 800},
	/* S: 1670     C: 1670      1: 1288     0: 433     HITACHI */
	{1550, 1800,  1550,  1800, 1000, 1500, 300, 800},
	/* S: 7354     C: 7354      1: 1267     0: 446     节能新型 */
	{7000, 7500,  7000,  7500, 1000, 1500, 300, 800},
	/* S: 2592   C: 5192~20977   1: 1019     0: 443     McQuay */
	{2400, 2800,  4500, 21000, 950, 1150, 300, 800}
};

/* timer over time is 10ms */
INTERRUPT_HANDLER(TIM2_UPD_OVF_BRK_IRQHandler, 13) 
{
	if (TIM2->SR1 & (uint8_t)(TIM2_FLAG_UPDATE))
	{
		TIM2->SR1 &= (uint8_t)(~TIM2_FLAG_UPDATE);
		
		if (infrared_status & 0x80)
		{
			/* clear the rising flag */
			infrared_status &= 0xef;
			/* a valid key value received */
			infrared_status |= 0x40;
			/* clear boot flag */
			infrared_status &= 0x7f;
			/* get valid bytes number */
			infrared_recv_len = (infrared_index / 8) & 0x00ff;
			if (infrared_index % 8)
				infrared_recv_len++;
		}
	}
}

INTERRUPT_HANDLER(TIM2_CAP_COM_IRQHandler, 14)
{
	uint8_t byte_idx;
	uint8_t prot_idx;
	uint8_t capture_val_h, capture_val_l;
	uint16_t capture_val;
	
	if ((TIM2->SR1 & (uint8_t)(TIM2_FLAG_CC2)) || (TIM2->SR2 & (uint8_t)(TIM2_FLAG_CC2)))
	{
		if (b_is_rising == RA_TRUE)
		{
			b_is_rising = RA_FALSE;
			
			/* clear input capture flag */
			TIM2->SR1 = (uint8_t)(~(uint8_t)(TIM2_FLAG_CC2));
	    	TIM2->SR2 = (uint8_t)(~(uint8_t)(TIM2_FLAG_CC2)) & 0x1e;
			/* change to polarity to falling */
			TIM2->CCER1 |= TIM2_CCER1_CC2P;
			/* clear counter register */
			TIM2->CNTRH = 0x00;
			TIM2->CNTRL = 0x00;
			if ((infrared_status & 0x40) == 0)
			{
				/* sign the rising plus has capture */
				infrared_status |= 0x10;
				/* flash led indicate receive infrared data */
				ra_led_ctrl(LED_ON_MODE);
			}
		}
		else
		{
			b_is_rising = TRUE;
			
			/* Get the Capture 2 Register value */
			capture_val_h = TIM2->CCR2H;
	 		capture_val_l = TIM2->CCR2L;
			capture_val = ((uint16_t)(capture_val_h) << 8) | (uint16_t)(capture_val_l);
			/* clear input capture flag */
	    	TIM2->SR1 = (uint8_t)(~(uint8_t)(TIM2_FLAG_CC2));
	    	TIM2->SR2 = (uint8_t)(~(uint8_t)(TIM2_FLAG_CC2)) & 0x1e;
			/* change to polarity to rising */
			TIM2->CCER1 &= (uint8_t)(~TIM2_CCER1_CC2P);			
			/* clear counter register */
			TIM2->CNTRH = 0x00;
			TIM2->CNTRL = 0x00;
			
			prot_idx = ra_get_switch();
			
			if (infrared_status & 0x10)
			{
				if (infrared_status & 0x80)
				{	
					/* receive one low bit: standard is 560us */
					if ((capture_val > protocal[prot_idx].logic_0_plus_dn) && (capture_val < protocal[prot_idx].logic_0_plus_up))
					{
						if (infrared_index < (sizeof(infrared_buffer) * 8))
						{
							infrared_buffer[infrared_index / 8] <<= 1;
							infrared_buffer[infrared_index / 8] |= 0x00;
							infrared_index++;
						}
					}
					/* receive one high bit */
					else if ((capture_val > protocal[prot_idx].logic_1_plus_dn) && (capture_val < protocal[prot_idx].logic_1_plus_up))
					{
						if (infrared_index < (sizeof(infrared_buffer) * 8))
						{
							infrared_buffer[infrared_index / 8] <<= 1;
							infrared_buffer[infrared_index / 8] |= 0x01;
							infrared_index++;
						}
					}
					/* receive a connect code */
					else if ((capture_val > protocal[prot_idx].connt_plus_dn) && (capture_val < protocal[prot_idx].connt_plus_up))
					{
						/* we not support mutiply keys */
					}
				}
				/* capture boot puls */
				else if ((capture_val > protocal[prot_idx].start_plus_dn) && (capture_val < protocal[prot_idx].start_plus_up))
				{
					infrared_index = 0x00;
					infrared_status &= 0xf0;
					infrared_recv_len = 0x00;
					infrared_status |= 0x80;
				}
			}
			/* clear the rising flag */
			infrared_status &= 0xef;
			/* flash led indicate receive infrared data */
			ra_led_ctrl(LED_OFF_MODE);
		}
	}
}

uint8_t infrared_scan(uint8_t *recv_buffer)
{
	uint8_t infrared_len;
	uint8_t recv_len = 0;

	if (infrared_status & 0x40)
	{		
		infrared_len = infrared_recv_len;
		while (infrared_len)
		{
			recv_buffer[recv_len] = infrared_buffer[recv_len];
			recv_len++; infrared_len--;
		}

		/* insure the led is off status after receive a frame data */
		ra_led_ctrl(LED_OFF_MODE);
		
		infrared_recv_len = 0x00;

		disableInterrupts();
		infrared_status &= 0xb0;
		enableInterrupts();
	}
	
	return recv_len;
}

