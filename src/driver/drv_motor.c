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

/* define stepper-motor driver port and pin */
#define STEP_DRV_A_PROT				GPIOD
#define STEP_DRV_A_PIN				GPIO_PIN_2
#define STEP_DRV_B_PROT				GPIOC
#define STEP_DRV_B_PIN				GPIO_PIN_7
#define STEP_DRV_C_PROT				GPIOC
#define STEP_DRV_C_PIN				GPIO_PIN_6
#define STEP_DRV_D_PROT				GPIOC
#define STEP_DRV_D_PIN				GPIO_PIN_5

#define STEP_ZERO()		{ 															\
							STEP_DRV_A_PROT->ODR &= (uint8_t)~((STEP_DRV_A_PIN));	\
							STEP_DRV_B_PROT->ODR &= (uint8_t)~((STEP_DRV_B_PIN));	\
							STEP_DRV_C_PROT->ODR &= (uint8_t)~((STEP_DRV_C_PIN));	\
							STEP_DRV_D_PROT->ODR &= (uint8_t)~((STEP_DRV_D_PIN));	\
						}

#define STEP_ONE()		{ 															\
							STEP_DRV_A_PROT->ODR |= (uint8_t)STEP_DRV_A_PIN;		\
							STEP_DRV_B_PROT->ODR |= (uint8_t)STEP_DRV_B_PIN;		\
							STEP_DRV_C_PROT->ODR &= (uint8_t)~((STEP_DRV_C_PIN));	\
							STEP_DRV_D_PROT->ODR &= (uint8_t)~((STEP_DRV_D_PIN));	\
						}

#define STEP_TWO()		{ 															\
							STEP_DRV_A_PROT->ODR &= (uint8_t)~((STEP_DRV_A_PIN));	\
							STEP_DRV_B_PROT->ODR |= (uint8_t)STEP_DRV_B_PIN;		\
							STEP_DRV_C_PROT->ODR |= (uint8_t)STEP_DRV_C_PIN;		\
							STEP_DRV_D_PROT->ODR &= (uint8_t)~((STEP_DRV_D_PIN));	\
						}

#define STEP_THR()		{ 															\
							STEP_DRV_A_PROT->ODR &= (uint8_t)~((STEP_DRV_A_PIN));	\
							STEP_DRV_B_PROT->ODR &= (uint8_t)~((STEP_DRV_B_PIN));	\
							STEP_DRV_C_PROT->ODR |= (uint8_t)STEP_DRV_C_PIN;		\
							STEP_DRV_D_PROT->ODR |= (uint8_t)STEP_DRV_D_PIN;		\
						}

#define STEP_FOU()		{ 															\
							STEP_DRV_A_PROT->ODR |= (uint8_t)STEP_DRV_A_PIN;		\
							STEP_DRV_B_PROT->ODR &= (uint8_t)~((STEP_DRV_B_PIN));	\
							STEP_DRV_C_PROT->ODR &= (uint8_t)~((STEP_DRV_C_PIN));	\
							STEP_DRV_D_PROT->ODR |= (uint8_t)STEP_DRV_D_PIN;		\
						}
						

#define MOTOR_RUNNING_MASK				0x80000000
#define MOTOR_SWING_MASK				0x40000000
#define MOTOR_DIRECTION_MASK			0x20000000
#define MOTOR_STEPIDX_MASK				0x18000000
#define MOTOR_RESEVAL_MASK				0x04000000
#define MOTOR_RELOAD_MASK				0x03ffe000
#define MOTOR_RUNCNT_MASK				0x00001fff
/* bit31=motor running, bit30=motor swing, bit29=motor direction, 
   bit28~bit27=step index, bit26=reseval, bit25~bit13=reload value bit12~bit0=counter value */
static uint32_t motor_ctrl_para;

bool_t ra_motor_is_running(void)
{
	if (motor_ctrl_para & MOTOR_RUNNING_MASK)
		return RA_TRUE;
	else
		return RA_FALSE;
}

void ra_motor_ctrl(bool_t b_run, bool_t b_swing, bool_t b_clockwise, uint32_t reload)
{
	uint32_t ctrl_para;

	ctrl_para = motor_ctrl_para;
	
	if (b_run == RA_TRUE)
		ctrl_para |= (uint32_t)(MOTOR_RUNNING_MASK);
	else
		ctrl_para &= (uint32_t)(~MOTOR_RUNNING_MASK);

	if (b_swing == RA_TRUE)
		ctrl_para |= (uint32_t)(MOTOR_SWING_MASK);
	else
		ctrl_para &= (uint32_t)(~MOTOR_SWING_MASK);

	/* when the mode is swing, keep the last direction */
	if (b_swing == RA_FALSE)
	{
		if (b_clockwise == RA_TRUE)
			ctrl_para |= (uint32_t)(MOTOR_DIRECTION_MASK);
		else
			ctrl_para &= (uint32_t)(~MOTOR_DIRECTION_MASK);
	}

	ctrl_para &= (uint32_t)(~MOTOR_STEPIDX_MASK);
	
	ctrl_para &= (uint32_t)(~MOTOR_RELOAD_MASK);
	ctrl_para |= (uint32_t)((reload << 13) & MOTOR_RELOAD_MASK);
		
	if (b_swing == RA_FALSE)
	{
		ctrl_para &= (uint32_t)(~MOTOR_RUNCNT_MASK);
		ctrl_para |= (uint32_t)(reload & MOTOR_RUNCNT_MASK);
	}

	disableInterrupts();
	motor_ctrl_para = ctrl_para;
	enableInterrupts();
}

/*
 * execute period is 2ms
 */
void ra_motor_period_update(void)
{
	uint8_t step_idx;
	uint32_t run_cnt;
	
	if (motor_ctrl_para & MOTOR_RUNNING_MASK)
	{
		step_idx = (uint8_t)((motor_ctrl_para & MOTOR_STEPIDX_MASK) >> 27);
		switch (step_idx)
		{
		case 0: STEP_ONE();	break;
		case 1: STEP_TWO();	break;
		case 2: STEP_THR();	break;
		case 3: STEP_FOU();	break;
		default: step_idx = 0; break;
		}
			
		if (motor_ctrl_para & MOTOR_DIRECTION_MASK)
			step_idx = (uint8_t)(step_idx + 1) % 4;
		else
			step_idx = (uint8_t)(step_idx - 1) % 4;
		
		motor_ctrl_para &= (uint32_t)(~MOTOR_STEPIDX_MASK);
		motor_ctrl_para |= ((uint32_t)(step_idx)) << 27;

		run_cnt = motor_ctrl_para & MOTOR_RUNCNT_MASK;
		if (run_cnt)
		{
			run_cnt--;
			motor_ctrl_para &= (uint32_t)(~MOTOR_RUNCNT_MASK);
			motor_ctrl_para |= run_cnt;
		}
		else
		{
			if (motor_ctrl_para & MOTOR_SWING_MASK)
			{
				motor_ctrl_para &= (uint32_t)(~MOTOR_STEPIDX_MASK);
				motor_ctrl_para |= (motor_ctrl_para & MOTOR_RELOAD_MASK) >> 13;
				if (motor_ctrl_para & MOTOR_DIRECTION_MASK)
					motor_ctrl_para &= (uint32_t)(~MOTOR_DIRECTION_MASK);
				else
					motor_ctrl_para |= MOTOR_DIRECTION_MASK;
			}
			else
				motor_ctrl_para &= (uint32_t)(~MOTOR_RUNNING_MASK);
		}
	}
	else
		STEP_ZERO();
}

void ra_hw_stepper_motor_init(void)
{
	/* stepper motor A pin configure: out pull-up initial low */
	STEP_DRV_A_PROT->CR2 &= (uint8_t)(~(STEP_DRV_A_PIN));
	STEP_DRV_A_PROT->ODR &= (uint8_t)(~(STEP_DRV_A_PIN));
	STEP_DRV_A_PROT->DDR |= (uint8_t)(STEP_DRV_A_PIN);
	STEP_DRV_A_PROT->CR1 |= (uint8_t)(STEP_DRV_A_PIN);

	/* stepper motor A pin configure: out pull-up initial low */
	STEP_DRV_B_PROT->CR2 &= (uint8_t)(~(STEP_DRV_B_PIN));
	STEP_DRV_B_PROT->ODR &= (uint8_t)(~(STEP_DRV_B_PIN));
	STEP_DRV_B_PROT->DDR |= (uint8_t)(STEP_DRV_B_PIN);
	STEP_DRV_B_PROT->CR1 |= (uint8_t)(STEP_DRV_B_PIN);

	/* stepper motor A pin configure: out pull-up initial low */
	STEP_DRV_C_PROT->CR2 &= (uint8_t)(~(STEP_DRV_C_PIN));
	STEP_DRV_C_PROT->ODR &= (uint8_t)(~(STEP_DRV_C_PIN));
	STEP_DRV_C_PROT->DDR |= (uint8_t)(STEP_DRV_C_PIN);
	STEP_DRV_C_PROT->CR1 |= (uint8_t)(STEP_DRV_C_PIN);

	/* stepper motor A pin configure: out pull-up initial low */
	STEP_DRV_D_PROT->CR2 &= (uint8_t)(~(STEP_DRV_D_PIN));
	STEP_DRV_D_PROT->ODR &= (uint8_t)(~(STEP_DRV_D_PIN));
	STEP_DRV_D_PROT->DDR |= (uint8_t)(STEP_DRV_D_PIN);
	STEP_DRV_D_PROT->CR1 |= (uint8_t)(STEP_DRV_D_PIN);

	motor_ctrl_para = 0;
}
