/*
 * Copyright (c) 2017-2019, Radiation Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date 		  Author	   Notes
 * 2019-03-29	  Warfalcon	   first implementation
 */

#ifndef __DRV_UART1_H
#define __DRV_UART1_H

/*
 * Serial FIFO mode 
 */
struct ra_uart_rx_fifo
{
    /* software fifo */
    uint8_t *buffer;
	uint16_t bufsz;

    uint16_t put_index, get_index;

	bool_t is_full;
};

extern void ra_hw_uart1_init(void)     ;
extern void ra_uart1_timer_update(void);
extern uint16_t ra_uart1_send_bytes(uint8_t *tx_buf, uint16_t tx_buff_len);
extern uint16_t ra_uart1_recv_bytes(uint8_t *rx_buf, uint16_t rx_buff_len);

#endif /* __DRV_UART1_H */

