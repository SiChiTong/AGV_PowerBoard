/*
 *  Author: Kaka Xie
 *  brief: usart operation
 */

#include "sys.h"
#include "usart.h"

#if SYSTEM_SUPPORT_UCOS
#include "includes.h"           //ucos 使用
#endif


#if 1
#pragma import(__use_no_semihosting)
//标准库需要的支持函数
struct __FILE
{
    int handle;

};

FILE __stdout;
//定义_sys_exit()以避免使用半主机模式
void _sys_exit(int x)
{
    x = x;
}
//重定义fputc函数
int fputc(int ch, FILE *f)
{
    //while((USART1->SR&0X40)==0);//循环发送,直到发送完毕
    //USART1->DR = (u8) ch;
    return ch;
}
#endif


uint8_t fp_uart_send_buf[FP_SEND_SIZE];
uint8_t fp_uart_rcv_buf[FP_RCV_SIZE];
void uart2_dma_init(u32 bound)
{
    USART_InitTypeDef USART_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;
    GPIO_InitTypeDef	GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    {
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    }

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);



    USART_InitStructure.USART_BaudRate = bound;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART2, &USART_InitStructure);

    USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
    USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);  //
    USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);
    //USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART2, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);


    //DMA configuration,  USART2 TX
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART2->DR);    // 外设地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&fp_uart_send_buf[0];    // 内存地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                  // 内存 to 外设
    DMA_InitStructure.DMA_BufferSize = FP_SEND_SIZE;             // 缓存大小
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;    // 外设地址不增加
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;             // 内存地址增加
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // 数据宽度8位
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; // 数据宽度
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                     // 循环缓存模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;                 // DMA优先级
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                        // MEM到MEM传输禁止
    DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, ENABLE);                     // 允许DMA传输完中断
    DMA_Init(DMA1_Channel7, &DMA_InitStructure);
    DMA_Cmd(DMA1_Channel7, DISABLE);

    //USART2 RX
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART2->DR);    // 外设地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&fp_uart_rcv_buf[0];    // 内存地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                  // 内存 to 外设
    DMA_InitStructure.DMA_BufferSize = FP_RCV_SIZE;             // 缓存大小
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;    // 外设地址不增加
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;             // 内存地址增加
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // 数据宽度8位
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; // 内存数据宽度
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                     // 循环缓存模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                 // DMA优先级
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                        // MEM到MEM传输禁止
    //DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, ENABLE);                     // 允许DMA传输完中断
    DMA_Init(DMA1_Channel6, &DMA_InitStructure);
    DMA_Cmd(DMA1_Channel6, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel7_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);


}


int uart2_send(uint8_t *data, uint16_t len)
{
    memcpy(fp_uart_send_buf, data, len);
    while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
    DMA1_Channel7->CNDTR = len;
    DMA_Cmd(DMA1_Channel7, ENABLE);

    return 0;
}




uint8_t lc12s_send_buf[LC12S_SEND_SIZE];
uint8_t lc12s_uart_rcv_buf[LC12S_RCV_SIZE];
void uart1_dma_init(u32 bound)
{
    USART_InitTypeDef USART_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;
    GPIO_InitTypeDef	GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    {
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    }

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);



    USART_InitStructure.USART_BaudRate = bound;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART1, &USART_InitStructure);

    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
    USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);  //
    USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
    //USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART1, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);


    //DMA configuration,  COM1-TX
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART1->DR);    // 外设地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&lc12s_send_buf[0];    // 内存地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                  // 内存 to 外设
    DMA_InitStructure.DMA_BufferSize = LC12S_SEND_SIZE;             // 缓存大小
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;    // 外设地址不增加
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;             // 内存地址增加
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // 数据宽度8位
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; // 内存数据宽度
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                     // 循环缓存模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;                 // DMA优先级
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                        // MEM到MEM传输禁止
    DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);                     // 允许DMA传输完中断
    DMA_Init(DMA1_Channel4, &DMA_InitStructure);
    DMA_Cmd(DMA1_Channel4, DISABLE);

    //COM1-RX
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART1->DR);    // 外设地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&lc12s_uart_rcv_buf[0];    // 内存地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                  // 内存 to 外设
    DMA_InitStructure.DMA_BufferSize = LC12S_RCV_SIZE;             // 缓存大小
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;    // 外设地址不增加
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;             // 内存地址增加
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // 数据宽度8位
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; // 数据宽度
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                     // 循环缓存模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                 // DMA优先级
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                        // MEM到MEM传输禁止
    //DMA_ITConfig(DMA1_Channel5, DMA_IT_TC, ENABLE);                     // 允许DMA传输完中断
    DMA_Init(DMA1_Channel5, &DMA_InitStructure);
    DMA_Cmd(DMA1_Channel5, ENABLE);


    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);


}


void uart_1_init(u32 bound)
{
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
    USART_DeInit(USART1);  //复位串口1
    //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA9

    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PA10

    //Usart1 NVIC 配置

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

    //USART 初始化设置

    USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

    USART_Init(USART1, &USART_InitStructure); //初始化串口
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启中断
    USART_Cmd(USART1, ENABLE);                    //使能串口
}




//初始化IO 串口2
//bound:波特率
void uart_2_init(u32 bound)
{
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//使能USART2，GPIOA时钟
    USART_DeInit(USART2);  //复位串口2
    //USART2_TX   PA.2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA2

    //USART2_RX	  PA.3
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PA3

    //USART2 NVIC 配置

    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

    //USART 初始化设置

    USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

    USART_Init(USART2, &USART_InitStructure); //初始化串口
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启中断
    USART_Cmd(USART2, ENABLE);                    //使能串口
}



//初始化IO 串口3
//bound:波特率
void uart_3_init(u32 bound)
{
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//使能USART3，GPIOB时钟
    USART_DeInit(USART3);  //复位串口1
    //USART3_TX   PB.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB.10
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOB, &GPIO_InitStructure); //初始化PB10

    //USART3_RX	  PB.11
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);  //初始化PB11

    //USART3 NVIC 配置

    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

    //USART 初始化设置

    USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

    USART_Init(USART3, &USART_InitStructure); //初始化串口
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启中断
    USART_Cmd(USART3, ENABLE);                    //使能串口
}



//初始化IO 串口3
//bound:波特率
void uart_4_init(u32 bound)
{
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	//使能UART4，GPIOB时钟
    USART_DeInit(UART4);  //复位串口1
    //USART4_TX   PC.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB.10
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOC, &GPIO_InitStructure); //初始化PB10

    //UART4_RX	  PC.11
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOC, &GPIO_InitStructure);  //初始化PB11

    //UART4 NVIC 配置

    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3 ;//抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;		//子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

    //USART 初始化设置

    USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

    USART_Init(UART4, &USART_InitStructure); //初始化串口
    USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//开启中断
    USART_Cmd(UART4, ENABLE);                    //使能串口
}
#if 0
int uart1_send(uint8_t *data, uint16_t len)
{
    memcpy(lc12s_send_buf, data, len);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    DMA1_Channel4->CNDTR = len;
    DMA_Cmd(DMA1_Channel4, ENABLE);

    return 0;
}
#endif

int uart_3_send(uint8_t *data, uint16_t len)
{
    uint8_t i = 0;
    while(len--)
    {
        while((USART3->SR & 0X40)==0);//循环发送,直到发送完毕
        USART3->DR = (u8) data[i++];
    }
    return 0;
}


int uart_4_send(uint8_t *data, uint16_t len)
{
    uint8_t i = 0;
    while(len--)
    {
        while((UART4->SR & 0X40)==0);//循环发送,直到发送完毕
        UART4->DR = (u8) data[i++];
    }
    return 0;
}
