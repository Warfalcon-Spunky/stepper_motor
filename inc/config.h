/*
 * Copyright (c) 2017-2019, Radiation Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date 		  Author	   Notes
 * 2019-03-29	  Warfalcon	   first implementation
 */
 
#ifndef __CONFIG_H__
#define __CONFIG_H__    

/* system clock source defintion */
#define HSI_2M						0
#define HSI_4M						1
#define HSI_8M						2
#define HSI_16M						3
#define LSI_128K					4
#define HSE_XTAL					5
#define SYSCLK_SOURCE				HSI_16M

#define RA_ENABLE_IWDG
// #define RA_TEST_FOR_LED

/* cofigure the system ticks value */
#define SYSTICK_PERIOD				100				/* uint: us */
/* cofigure infrared OVF value */
#define INFRARED_PERIOD				60000			/* uint: us */
/* cofigure infrared receive buffer size */
#define INFRARED_BUFFER_SIZE		64				
/* uart1 baudrate */
#define UART1_BAUDRATE				19200
/* uart1 timeout */
#define UART1_TIMEOUT				10				/* uint: ms */
	

#endif /* __CONFIG_H__ */
