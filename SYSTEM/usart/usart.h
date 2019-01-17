#ifndef __USART_H
#define __USART_H
#include "stdio.h"
#include "sys.h"

void uart_4_init(u32 bound);
int uart_4_send(uint8_t *data, uint16_t len);
#endif

