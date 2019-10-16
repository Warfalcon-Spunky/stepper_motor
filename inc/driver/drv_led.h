/*
 * Copyright (c) 2017-2019, Radiation Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date 		  Author	   Notes
 * 2019-03-29	  Warfalcon	   first implementation
 */


#ifndef __DRV_LED_H
#define __DRV_LED_H

#define LED_SLOW_MODE				0x80
#define LED_FAST_MODE				0x40
#define LED_ON_MODE					0x20
#define LED_OFF_MODE				0x10


extern void ra_hw_led_init(void);
extern void ra_led_ctrl(uint8_t mode);
extern void ra_led_period_update(void);

#endif /* __DRV_LED_H */
