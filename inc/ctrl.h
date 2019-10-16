/*
 * Copyright (c) 2017-2019, Radiation Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date 		  Author	   Notes
 * 2019-03-29	  Warfalcon	   first implementation
 */
 
#ifndef __CTRL_H__
#define __CTRL_H__    

#define MOTOR_NONE_CMD				0x00
#define MOTOR_POWER_CMD				0x01
#define MOTOR_SWING_CMD				0x02

extern bool_t ra_ctrl_cmd(uint8_t cmd);
extern bool_t ra_ctrl_status(void);
extern void ra_ctrl_period_update(void);

#endif /* __CTRL_H__ */
