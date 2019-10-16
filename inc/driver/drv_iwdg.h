/*
 * Copyright (c) 2017-2019, Radiation Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date 		  Author	   Notes
 * 2019-03-29	  Warfalcon	   first implementation
 */


#ifndef __BSP_IWDG_H
#define __BSP_IWDG_H

#define ra_iwdg_reload()		wdt_cnt = 0x80

extern void ra_hw_iwdg_init(void);

#endif/* end of ifndef __BSP_IWDG_H */

