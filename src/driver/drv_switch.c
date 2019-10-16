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

/* define switch port and pin */
#define STEP_DRV_SW1_PORT			GPIOD
#define STEP_DRV_SW1_PIN			GPIO_PIN_4
#define STEP_DRV_SW2_PORT			GPIOA
#define STEP_DRV_SW2_PIN			GPIO_PIN_1
#define STEP_DRV_SW3_PORT			GPIOA
#define STEP_DRV_SW3_PIN			GPIO_PIN_2

uint8_t ra_get_switch(void)
{
	uint8_t switch_val = 0;

	if ((STEP_DRV_SW1_PORT->IDR & (uint8_t)STEP_DRV_SW1_PIN) == RESET)
		switch_val |= 0x01;
	
	if ((STEP_DRV_SW2_PORT->IDR & (uint8_t)STEP_DRV_SW2_PIN) == RESET)
		switch_val |= 0x02;

	if ((STEP_DRV_SW3_PORT->IDR & (uint8_t)STEP_DRV_SW3_PIN) == RESET)
		switch_val |= 0x04;

	return switch_val;
}

/**
 * This function will configure switch pin.
 */

void ra_hw_switch_init(void)
{
	/* reset corresponding bit to GPIO in CR2 register */
	STEP_DRV_SW1_PORT->CR2 &= (uint8_t)(~(STEP_DRV_SW1_PIN));
	/* configure pin mode is output */
	STEP_DRV_SW1_PORT->DDR &= (uint8_t)(~(STEP_DRV_SW1_PIN));
	/* configure pin mode is pull-pull */
	STEP_DRV_SW1_PORT->CR1 |= (uint8_t)STEP_DRV_SW1_PIN;

	/* reset corresponding bit to GPIO in CR2 register */
	STEP_DRV_SW2_PORT->CR2 &= (uint8_t)(~(STEP_DRV_SW2_PIN));
	/* configure pin mode is output */
	STEP_DRV_SW2_PORT->DDR &= (uint8_t)(~(STEP_DRV_SW2_PIN));
	/* configure pin mode is pull-pull */
	STEP_DRV_SW2_PORT->CR1 |= (uint8_t)STEP_DRV_SW2_PIN;

	/* reset corresponding bit to GPIO in CR2 register */
	STEP_DRV_SW3_PORT->CR2 &= (uint8_t)(~(STEP_DRV_SW3_PIN));
	/* configure pin mode is output */
	STEP_DRV_SW3_PORT->DDR &= (uint8_t)(~(STEP_DRV_SW3_PIN));
	/* configure pin mode is pull-pull */
	STEP_DRV_SW3_PORT->CR1 |= (uint8_t)STEP_DRV_SW3_PIN;
}

