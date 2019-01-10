#ifndef __USART_H
#define __USART_H
#include "stdio.h"
#include "sys.h"

#define FP_SEND_SIZE   255
#define FP_RCV_SIZE    255

#define LC12S_SEND_SIZE   255
#define LC12S_RCV_SIZE    255

extern uint8_t fp_uart_send_buf[FP_SEND_SIZE];
extern uint8_t fp_uart_rcv_buf[FP_RCV_SIZE];

extern uint8_t lc12s_send_buf[LC12S_SEND_SIZE];
extern uint8_t lc12s_uart_rcv_buf[LC12S_RCV_SIZE];

void uart2_dma_init(u32 bound);
int uart2_send(uint8_t *data, uint16_t len);

void uart1_dma_init(u32 bound);
int uart1_send(uint8_t *data, uint16_t len);
#endif

