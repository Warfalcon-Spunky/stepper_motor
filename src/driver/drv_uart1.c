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

/* define director of rs485 port and pin */
#define COM_DIR_PORT				GPIOA
#define COM_DIR_PIN					GPIO_PIN_3

static uint8_t uart_status = 0x00;

/* define uart rx buffer and fifo */
static struct ra_uart_rx_fifo rx_fifo;
static uint8_t  rx_buffer[128];

static void ra_uart_dir_delay(void)
{
	uint32_t dly = 2000;
	while (dly)
	{
		dly--;
	}
}

static void ra_uart1_init(void)
{
    uint32_t BaudRate_Mantissa = 0, BaudRate_Mantissa100 = 0;

    /* Clear the word length bit */
    UART1->CR1 &= (uint8_t)(~UART1_CR1_M);  
    /* Set the word length bit according to UART1_WordLength value */
//	UART1->CR1 |= UART1_WORDLENGTH_8D

    /* Clear the STOP bits */
    UART1->CR3 &= (uint8_t)(~UART1_CR3_STOP);  
    /* Set the STOP bits number according to UART1_StopBits value  */ 
//	UART1->CR3 |= UART1_STOPBITS_1;

    /* Clear the Parity Control bit */
    UART1->CR1 &= (uint8_t)(~(UART1_CR1_PCEN | UART1_CR1_PS));  
    /* Set the Parity Control bit to UART1_Parity value */
//	UART1->CR1 |= UART1_PARITY_NO;

    /* Clear the LSB mantissa of UART1DIV  */
    UART1->BRR1 &= (uint8_t)(~UART1_BRR1_DIVM);  
    /* Clear the MSB mantissa of UART1DIV  */
    UART1->BRR2 &= (uint8_t)(~UART1_BRR2_DIVM);  
    /* Clear the Fraction bits of UART1DIV */
    UART1->BRR2 &= (uint8_t)(~UART1_BRR2_DIVF);  

    /* Set the UART1 BaudRates in BRR1 and BRR2 registers according to UART1_BaudRate value */
    BaudRate_Mantissa    = (HSI_VALUE / ((uint32_t)(UART1_BAUDRATE) << 4));
    BaudRate_Mantissa100 = ((HSI_VALUE * 100) / ((uint32_t)(UART1_BAUDRATE) << 4));
    /* Set the fraction of UART1DIV  */
    UART1->BRR2 |= (uint8_t)((uint8_t)(((BaudRate_Mantissa100 - (BaudRate_Mantissa * 100)) << 4) / 100) & (uint8_t)0x0F); 
    /* Set the MSB mantissa of UART1DIV  */
    UART1->BRR2 |= (uint8_t)((BaudRate_Mantissa >> 4) & (uint8_t)0xF0); 
    /* Set the LSB mantissa of UART1DIV  */
    UART1->BRR1 |= (uint8_t)BaudRate_Mantissa;           

    /* Disable the Transmitter and Receiver before seting the LBCL, CPOL and CPHA bits */
    UART1->CR2 &= (uint8_t)~(UART1_CR2_TEN | UART1_CR2_REN); 
    /* Clear the Clock Polarity, lock Phase, Last Bit Clock pulse */
    UART1->CR3 &= (uint8_t)~(UART1_CR3_CPOL | UART1_CR3_CPHA | UART1_CR3_LBCL); 
    /* Set the Clock Polarity, lock Phase, Last Bit Clock pulse */
    UART1->CR3 |= (uint8_t)((uint8_t)UART1_SYNCMODE_CLOCK_DISABLE & (uint8_t)(UART1_CR3_CPOL | UART1_CR3_CPHA | UART1_CR3_LBCL));  

	UART1->CR2 |= (uint8_t)UART1_CR2_TEN;
	UART1->CR2 |= (uint8_t)UART1_CR2_REN;

    /* uart1 sync mode disable */
    UART1->CR3 &= (uint8_t)(~UART1_CR3_CKEN); 
}

void ra_hw_uart1_init(void)
{
	rx_fifo.bufsz = sizeof(rx_buffer);
	rx_fifo.buffer = rx_buffer;
	rx_fifo.is_full = RA_FALSE;
	rx_fifo.get_index = 0;
	rx_fifo.put_index = 0;	
	
	/* reset corresponding bit to GPIO in CR2 register */
	COM_DIR_PORT->CR2 &= (uint8_t)(~(COM_DIR_PIN));
	/* configure output low level */
	COM_DIR_PORT->ODR &= (uint8_t)(~(COM_DIR_PIN));
	/* configure pin mode is output */
	COM_DIR_PORT->DDR |= (uint8_t)COM_DIR_PIN;
	/* configure pin mode is push-pull */
	COM_DIR_PORT->CR1 |= (uint8_t)COM_DIR_PIN;
	/* configure no external interrupt or no slope control */
	COM_DIR_PORT->CR2 &= (uint8_t)(~(COM_DIR_PIN));

	/* UART1_BAUDRATE, UART1_WORDLENGTH_8D, UART1_STOPBITS_1, UART1_PARITY_NO */
	ra_uart1_init();
	/* uart1 enable UART1_IT_RXNE interrupt */
	UART1->CR2 |= UART1_CR2_RIEN;
	/* uart1 enable */
    UART1->CR1 &= (uint8_t)(~UART1_CR1_UARTD); 
}

uint16_t ra_uart1_send_bytes(uint8_t *tx_buf, uint16_t tx_buff_len)
{
	uint16_t tx_idx = 0;

	/* set rs485 to tx status */
	COM_DIR_PORT->ODR |= (uint8_t)COM_DIR_PIN;
	ra_uart_dir_delay();

	while (tx_idx < tx_buff_len)
	{
		while ((UART1->SR & (uint8_t)UART1_FLAG_TXE) == RESET);
		UART1->DR = tx_buf[tx_idx++];
	}
	
	/* set rs485 to rx status */
	ra_uart_dir_delay();
	COM_DIR_PORT->ODR &= (uint8_t)(~(COM_DIR_PIN));	
	
	return (tx_buff_len - tx_idx);
}

uint16_t ra_uart1_recv_bytes(uint8_t *rx_buf, uint16_t rx_buff_len)
{
	uint8_t ch;
	uint16_t size = rx_buff_len;

	if (uart_status & 0x40)
	{
		while (rx_buff_len)
		{		
			/* there's no data: */
			if ((rx_fifo.get_index == rx_fifo.put_index) && (rx_fifo.is_full == RA_FALSE))
				break;
		
			/* otherwise there's the data: */
			ch = rx_fifo.buffer[rx_fifo.get_index++];
		
			if (rx_fifo.get_index >= rx_fifo.bufsz) 
				rx_fifo.get_index = 0;
		
			if (rx_fifo.is_full == RA_TRUE)
				rx_fifo.is_full = RA_FALSE;
		
			*rx_buf = ch;
			rx_buf++; rx_buff_len--;
		}

		uart_status = 0x00;
	}

	return size - rx_buff_len;
}

void ra_uart1_timer_update(void)
{
	if (uart_status & 0x80)
	{
		uart_status++;
		if ((uart_status & 0x3f) > UART1_TIMEOUT)
			uart_status |= 0x40;
	}
}

/* ISR for uart1 interrupt */
INTERRUPT_HANDLER(UART1_RX_IRQHandler, 18)
{
	uint8_t ch;

	if ((UART1->SR & UART1_SR_RXNE) && (UART1->CR2 & UART1_CR2_RIEN))
	{
		UART1->SR = (uint8_t)(~(UART1_SR_RXNE));
		
		ch = (uint8_t)UART1->DR;

		if ((uart_status & 0x40) == 0x00)
		{
			rx_fifo.buffer[rx_fifo.put_index++] = ch;
		
			if (rx_fifo.put_index >= rx_fifo.bufsz)
				rx_fifo.put_index = 0;

			/* if the next position is read index, discard this 'read char' */
	        if (rx_fifo.put_index == rx_fifo.get_index)
	        {
	            rx_fifo.get_index += 1;
	            rx_fifo.is_full = RA_TRUE;
				
	            if (rx_fifo.get_index >= rx_fifo.bufsz) 
					rx_fifo.get_index = 0;
	        }

			uart_status |= 0x80;
			uart_status &= 0xc0;
		}
	}

	if (UART1->SR & UART1_FLAG_OR)
		ch = (uint8_t)UART1->DR;
}

