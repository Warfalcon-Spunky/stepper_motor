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

static uint32_t systick_cnt = 0;
static uint8_t  systick_flag = 0;

#ifdef RA_ENABLE_IWDG
static uint8_t  wdt_cnt = 0;
#endif

/* system tick is 100us */
INTERRUPT_HANDLER(TIM4_UPD_OVF_IRQHandler, 23)
{
	systick_interrupt_clear();

	systick_cnt++;

	/* 1ms interrupt */
	if ((systick_cnt % 10) == 0)
		systick_flag |= 0x01;
	
	/* 4ms interrupt */
	if ((systick_cnt % 40) == 0)
	{
		ra_motor_period_update();
		systick_flag |= 0x02;
	}
	
	/* 8ms interrupt */
	if ((systick_cnt % 80) == 0)
	{
#ifdef RA_ENABLE_IWDG
		if (wdt_cnt)
		{
			IWDG->KR = IWDG_KEY_REFRESH;
			wdt_cnt--;
		}
#endif
		systick_flag |= 0x04;
	}
}

/**
  * @brief  Main program.
  */
void main(void)
{
	ra_hw_board_init();
  
  	while (1)
  	{
  		/* 1ms interrupt */
  		if (systick_flag & 0x01)
  		{
  			systick_flag &= 0xfe;
			ra_uart1_timer_update();
  		}
		/* 4ms interrupt */
		if (systick_flag & 0x02)
		{
			systick_flag &= 0xfd;			
		}
		/* 8ms interrupt */
		if (systick_flag & 0x04)
		{
			systick_flag &= 0xfb;
			ra_led_period_update();
			ra_decode_period_update();
			ra_ctrl_period_update();
		}

#ifdef RA_ENABLE_IWDG
		ra_iwdg_reload();
#endif
  	}
}



