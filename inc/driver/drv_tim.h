/*
 * Copyright (c) 2017-2019, Radiation Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date 		  Author	   Notes
 * 2019-03-29	  Warfalcon	   first implementation
 */

#ifndef __DRV_TIM_H
#define __DRV_TIM_H

/* Clear the IT pending Bit */
#define systick_interrupt_clear()		{TIM4->SR1 = (uint8_t)(~TIM4_IT_UPDATE);}

extern void ra_hw_tim2_init(void);
extern void ra_hw_tim4_init(void);

#endif /* end of #ifndef __DRV_TIM_H */


