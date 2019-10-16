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

void ra_autoswitch_clock(CLK_Source_TypeDef new_clock)
{
	CLK_Source_TypeDef clock_master;
    uint16_t down_counter = CLK_TIMEOUT;

    /* Current clock master saving */
    clock_master = (CLK_Source_TypeDef)CLK->CMSR;
	/* Enables Clock switch */
    CLK->SWCR |= CLK_SWCR_SWEN;
    /* Disables Switch interrupt */
	CLK->SWCR &= (uint8_t)(~CLK_SWCR_SWIEN);
    /* Selection of the target clock source */
    CLK->SWR = (uint8_t)new_clock;
	/* Wait clock stable */
    while ((((CLK->SWCR & CLK_SWCR_SWBSY) != 0 )&& (down_counter != 0)))
    {
        down_counter--;
    }

	/* Switch OFF current clock if required */
    if (clock_master == CLK_SOURCE_HSI)
        CLK->ICKR &= (uint8_t)(~CLK_ICKR_HSIEN);
    else if (clock_master == CLK_SOURCE_LSI)
        CLK->ICKR &= (uint8_t)(~CLK_ICKR_LSIEN);
    else if (clock_master == CLK_SOURCE_HSE)
        CLK->ECKR &= (uint8_t)(~CLK_ECKR_HSEEN);
}

/**
 * This function will configure and initial clock source.
 */
static void ra_hw_sysclk_init(void)
{
#if (SYSCLK_SOURCE == HSI_2M)
	/* auto switch to HSI source and disable interrupt and disable clock */
	ra_autoswitch_clock(CLK_SOURCE_HSI);
	/* configure HSI div 8 to system clock */
	/* Clear and set High speed internal clock prescaler */
    CLK->CKDIVR &= (uint8_t)(~CLK_CKDIVR_HSIDIV);
    CLK->CKDIVR |= CLK_PRESCALER_HSIDIV8;
	/* wait HSI eastable */
	while ((CLK->ICKR & (uint8_t)CLK_FLAG_HSIRDY) == RESET);
#endif

#if (SYSCLK_SOURCE == HSI_4M)
	/* auto switch to HSI source and disable interrupt and disable clock */
	ra_autoswitch_clock(CLK_SOURCE_HSI);
	/* configure HSI div 4 to system clock */						  	
	/* Clear and set High speed internal clock prescaler */
    CLK->CKDIVR &= (uint8_t)(~CLK_CKDIVR_HSIDIV);
    CLK->CKDIVR |= CLK_PRESCALER_HSIDIV4;
	/* wait HSI eastable */
	while ((CLK->ICKR & (uint8_t)CLK_FLAG_HSIRDY) == RESET);
#endif

#if (SYSCLK_SOURCE == HSI_8M)
	/* auto switch to HSI source and disable interrupt and disable clock */
	ra_autoswitch_clock(CLK_SOURCE_HSI);
	/* configure HSI div 2 to system clock */	
	/* Clear and set High speed internal clock prescaler */
    CLK->CKDIVR &= (uint8_t)(~CLK_CKDIVR_HSIDIV);
    CLK->CKDIVR |= CLK_PRESCALER_HSIDIV2;
	/* wait HSI eastable */
	while ((CLK->ICKR & (uint8_t)CLK_FLAG_HSIRDY) == RESET);
#endif

#if (SYSCLK_SOURCE == HSI_16M)
	/* auto switch to HSI source and disable interrupt and disable clock */
	ra_autoswitch_clock(CLK_SOURCE_HSI);
	/* configure HSI div 1 to system clock */
	/* Clear and set High speed internal clock prescaler */
    CLK->CKDIVR &= (uint8_t)(~CLK_CKDIVR_HSIDIV);
    CLK->CKDIVR |= CLK_PRESCALER_HSIDIV1;
	/* wait HSI eastable */
	while ((CLK->ICKR & (uint8_t)CLK_FLAG_HSIRDY) == RESET);
#endif

#if (SYSCLK_SOURCE == LSI_128K)
	/* auto switch to LSI source and disable interrupt and disable clock */
	ra_autoswitch_clock(CLK_SOURCE_LSI);
	/* wait LSI eastable */
	while ((CLK->ICKR & (uint8_t)CLK_FLAG_LSIRDY) == RESET);
#endif

#if (SYSCLK_SOURCE == HSE_XTAL)
	/* auto switch to HSE source and disable interrupt and disable clock */
	ra_autoswitch_clock(CLK_SOURCE_HSE);
	/* wait HSE eastable */
	while ((CLK->ECKR & (uint8_t)CLK_FLAG_HSERDY) == RESET);
#endif
}


/**
 * This function will initial target board
 */
void ra_hw_board_init(void)
{
	ra_hw_sysclk_init();
	/* tim4 is systick */
	ra_hw_tim4_init();
	/* tim2_icc2 is input capture */
	ra_hw_tim2_init();

	ra_hw_led_init();

	ra_hw_switch_init();

	ra_hw_uart1_init();

	ra_hw_stepper_motor_init();
	
#ifdef RA_ENABLE_IWDG
	ra_hw_iwdg_init();
#endif
	
	enableInterrupts();
}


