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


/**
 * This function will initial IWDG hardware
 */
void ra_hw_iwdg_init(void)
{          
	/* IWDG timeout equal to 1.02s (the timeout may varies due to LSI frequency dispersion) */
	
  	/* Enable write access to IWDG_PR and IWDG_RLR registers */	
	IWDG->KR = (uint8_t)IWDG_WriteAccess_Enable;
  	/* IWDG counter clock: LSI/256 */
	IWDG->PR = (uint8_t)IWDG_Prescaler_256;
  	/* Set counter reload value to obtain 250ms IWDG TimeOut. Counter Reload Value = 1.02s */
	IWDG->RLR = 0xff;
	/* Disable write access to IWDG_PR and IWDG_RLR registers */	
	IWDG->KR = (uint8_t)IWDG_WriteAccess_Disable;
  	/* Enable IWDG (the LSI oscillator will be enabled by hardware) */
	IWDG->KR = IWDG_KEY_ENABLE;

  	/* clear all reset flag */
	RST->SR = 0x1f;
}

