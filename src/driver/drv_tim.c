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

/* define LED indicator port and pin */
#define CAPTURE_PORT				GPIOD					
#define CAPTURE_PIN					GPIO_PIN_3


void ra_hw_tim4_init(void)
{          
	/* enable pre-load */
	TIM4->CR1 |= TIM4_CR1_ARPE;
	
	/* set the Prescaler value */
    TIM4->PSCR = (uint8_t)(TIM4_PRESCALER_128);
    /* set the autoreload value */
    TIM4->ARR = (uint8_t)((16000000u / (1000000 / SYSTICK_PERIOD)) / 128);

	/* Clear TIM4 update flag */	
	TIM4->SR1 &= (uint8_t)(~TIM4_FLAG_UPDATE);
	/* enable TIM4 interrupt */
	TIM4->IER |= (uint8_t)(TIM4_IT_UPDATE);

	/* enable TIM4 */
	TIM4->CR1 |= TIM4_CR1_CEN;				
}

void ra_hw_tim2_init(void)
{
	/* 
	 * configure timer of TIM2 
	 */
	/* enable pre-load */
	TIM2->CR1 |= TIM2_CR1_ARPE;
	/* set the Prescaler value = 16 div = 1us */
	TIM2->PSCR = 0x04;
	TIM2->EGR = TIM2_PSCRELOADMODE_IMMEDIATE;
    /* set the autoreload value = 60000us/60ms */
	TIM2->ARRH = (uint8_t)(INFRARED_PERIOD >> 8);
	TIM2->ARRL = (uint8_t)(INFRARED_PERIOD & 0x00ff);
	/* Clear TIM2 update flag */	
	TIM2->SR1 &= (uint8_t)(~TIM2_FLAG_UPDATE);
	/* enable TIM2 interrupt */
	TIM2->IER |= (uint8_t)(TIM2_IT_UPDATE);

	
	/* 
	 * configure capture mode of TIM2 
	 */
	/* reset corresponding bit to GPIO in CR2 register */
	CAPTURE_PORT->CR2 &= (uint8_t)(~(CAPTURE_PIN));
	/* configure pin mode is input */
	CAPTURE_PORT->DDR &= (uint8_t)(~(CAPTURE_PIN));
	/* configure pin mode is float */
	CAPTURE_PORT->CR1 &= (uint8_t)(~(CAPTURE_PIN)); 
	
	/* Disable the Channel 2: Reset the CCE Bit */
    TIM2->CCER1 &= (uint8_t)(~TIM2_CCER1_CC2E);
	/* Select the input=TI2FP2 prescale=0 and set the filter=8 period fliter */
//	TIM2->CCMR2 = 00110001b;
	TIM2->CCMR2 = 0x31;
    /* the polarity is rising */
    TIM2->CCER1 &= (uint8_t)(~TIM2_CCER1_CC2P);
    /* Set the CCE Bit */
    TIM2->CCER1 |= TIM2_CCER1_CC2E;
	/* Clear the IT pending Bit */
    TIM2->SR1 = (uint8_t)(~TIM2_IT_CC2);
	/* Enable the Interrupt sources */
    TIM2->IER |= (uint8_t)TIM2_IT_CC2;
	
	/* Enable TIM2: it will be enable by hardware when the timer mode configure trig mode */
 	TIM2->CR1 |= TIM2_CR1_CEN;
}

