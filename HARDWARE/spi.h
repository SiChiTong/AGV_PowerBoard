#ifndef __HARDWARE_SPI_H__
#define __HARDWARE_SPI_H__
#include "stm32f10x.h"
#include "sys.h"

void spi2_init(void);               //初始化SPI口
void spi2_set_speed(u8 SpeedSet); //设置SPI速度
u8 spi2_read_write_byte(u8 TxData);//SPI总线读写一个字节

#endif
