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
#define LED_PORT					GPIOC					
#define LED_PIN						GPIO_PIN_3

#define LED_SLOW_FREQ				63
#define LED_FAST_FREQ				25

#define LED_SLOW_MODE_MASK			0x80
#define LED_FAST_MODE_MASK			0x40
#define LED_COUNTER_MASK			0x3f

static uint8_t led_mode;

void ra_led_ctrl(uint8_t mode)
{
	switch (mode)
	{
	case LED_SLOW_MODE: {led_mode = 0x00; led_mode |= LED_SLOW_MODE_MASK; break;}
	case LED_FAST_MODE: {led_mode = 0x00; led_mode |= LED_FAST_MODE_MASK; break;}
	case LED_ON_MODE:   {led_mode = 0x00; LED_PORT->ODR &= (uint8_t)(~LED_PIN);   break;}
	case LED_OFF_MODE:  {led_mode = 0x00; LED_PORT->ODR |= (uint8_t)LED_PIN;      break;}
	default: {led_mode = 0x00; break;}
	}
}

/*
 * execute period is 8ms
 */
void ra_led_period_update(void)
{
	if (led_mode & LED_SLOW_MODE_MASK)
	{	
		if (led_mode & LED_COUNTER_MASK)
			led_mode--;
		else
		{
			LED_PORT->ODR ^= (uint8_t)(LED_PIN);
			led_mode |=  (uint8_t)(LED_SLOW_FREQ);
		}
	}
	else if (led_mode & LED_FAST_MODE_MASK)
	{
		if (led_mode & LED_COUNTER_MASK)
			led_mode--;
		else
		{
			LED_PORT->ODR ^= (uint8_t)(LED_PIN);
			led_mode |=  (uint8_t)(LED_FAST_FREQ);
		}
	}
#ifdef RA_TEST_FOR_LED
	else
	{
		LED_PORT->ODR ^= (uint8_t)(LED_PIN);
	}
#endif
}

/**
 * This function will configure LED pin.
 */
void ra_hw_led_init(void)
{
	/* reset corresponding bit to GPIO in CR2 register */
	LED_PORT->CR2 &= (uint8_t)(~(LED_PIN));
	/* configure output high level */
	LED_PORT->ODR |= (uint8_t)LED_PIN;
	/* configure pin mode is output */
	LED_PORT->DDR |= (uint8_t)LED_PIN;
	/* configure pin mode is push-pull */
	LED_PORT->CR1 |= (uint8_t)LED_PIN;

	led_mode = 0x00;
}

