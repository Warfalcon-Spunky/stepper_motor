/*
 * Copyright (c) 2017-2019, Radiation Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date 		  Author	   Notes
 * 2019-03-29	  Warfalcon	   first implementation
 */

#ifndef __DRV_MOTOR_H
#define __DRV_MOTOR_H

extern void ra_hw_stepper_motor_init(void);
extern void ra_motor_period_update(void);
extern void ra_motor_ctrl(bool_t b_run, bool_t b_swing, bool_t b_clockwise, uint32_t reload);
extern bool_t ra_motor_is_running(void);

#endif /* __DRV_MOTOR_H */

