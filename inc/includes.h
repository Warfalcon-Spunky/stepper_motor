/*
 * Copyright (c) 2017-2019, Radiation Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date 		  Author	   Notes
 * 2019-03-29	  Warfalcon	   first implementation
 */

#ifndef __INCLUDES_H__ 
#define __INCLUDES_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

// #include <math.h>
// #include <stdio.h>

#include "stm8s_conf.h"

#include "def.h"
#include "stm8s_it.h"
#include "config.h"

#include "bsp.h"
#include "drv_iwdg.h"
#include "drv_led.h"
#include "drv_switch.h"
#include "drv_tim.h"
#include "drv_motor.h"
#include "drv_uart1.h"
#include "infrared.h"
#include "ctrl.h"
#include "decode.h"

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __INCLUDES_H__ */