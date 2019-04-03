/**
  ******************************************************************************
  * @file    GPIO/IOToggle/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "includes.h"
#include "usart.h"


void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}


void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

void SVC_Handler(void)
{
}

void DebugMon_Handler(void)
{
}

//void PendSV_Handler(void)
//{
//}

 //void SysTick_Handler(void)
//{
//}


void DMA1_Channel4_IRQHandler(void)     //USART1-TX
{
    OSIntEnter();
    if (DMA_GetITStatus(DMA1_IT_TC4) != RESET)
    {
        DMA_Cmd(DMA1_Channel4, DISABLE);
        DMA_ClearFlag(DMA1_FLAG_TC4);
        DMA_ClearITPendingBit(DMA1_IT_TC4); // 清除中断标志位
//        while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);    //等待一包数据发送完成
    }
    OSIntExit();
}

void DMA1_Channel5_IRQHandler(void)     //USART1-TX
{
    OSIntEnter();
    if (DMA_GetITStatus(DMA1_IT_TC5) != RESET)
    {
        DMA_Cmd(DMA1_Channel5, DISABLE);
        DMA_ClearFlag(DMA1_FLAG_TC5);
        DMA_ClearITPendingBit(DMA1_IT_TC5); // 清除中断标志位
//        while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);    //等待一包数据发送完成
    }
    OSIntExit();
}


uint32_t uart1_rcv_test_cnt = 0;


void USART1_IRQHandler(void)
{
    OSIntEnter();
    volatile unsigned char temper=0;

    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        temper = USART1->SR;
        temper = USART1->DR;    //清USART_IT_IDLE
        uart1_rcv_test_cnt++;
    }
    OSIntExit();
}


#include "battery.h"
void UART4_IRQHandler(void)
{
    volatile unsigned char temper=0;

    OSIntEnter();

    if (USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
    {
        temper = UART4->SR;
        temper = UART4->DR;    //清USART_IT_IDLE
        battery_data_recieved(temper);
        uart1_rcv_test_cnt++;
    }
//    else
//    {
//        USART_ClearITPendingBit(UART4, USART_IT_TC);
////        USART_ClearITPendingBit(UART4, USART_IT_RXNE);
////        USART_ClearITPendingBit(UART4, USART_IT_ERR);
//    }
    OSIntExit();
}

void DMA1_Channel7_IRQHandler(void)     //USART2-TX
{
    OSIntEnter();
    if (DMA_GetITStatus(DMA1_IT_TC7) != RESET)
    {
        DMA_Cmd(DMA1_Channel7, DISABLE);
        DMA_ClearFlag(DMA1_FLAG_TC7);
        DMA_ClearITPendingBit(DMA1_IT_TC7); // 清除中断标志位
//        while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);    //等待一包数据发送完成
    }
    OSIntExit();
}

void DMA1_Channel6_IRQHandler(void)     //USART2-TX
{
    OSIntEnter();
    if (DMA_GetITStatus(DMA1_IT_TC6) != RESET)
    {
        DMA_Cmd(DMA1_Channel6, DISABLE);
        DMA_ClearFlag(DMA1_FLAG_TC6);
        DMA_ClearITPendingBit(DMA1_IT_TC6); // 清除中断标志位
//        while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);    //等待一包数据发送完成
    }
    OSIntExit();
}

/*    USART2 IDLE interrupt    */
void USART2_IRQHandler(void)
{
    OSIntEnter();
    volatile unsigned char temper=0;

    if (USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
    {
        temper = USART2->SR;
        temper = USART2->DR;    //清USART_IT_IDLE

        DMA_Cmd(DMA1_Channel6, DISABLE);
        DMA_Cmd(DMA1_Channel6,ENABLE);
    }
    OSIntExit();
}


#include "platform.h"
#include "charge_task.h"
uint32_t charge_state_tmp = 0;
void EXTI9_5_IRQHandler(void)
{
    OSIntEnter();
    uint8_t value = 0;
    if(EXTI_GetITStatus(EXTI_Line6) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line6);
        value = get_recharge_gpio_value();
        charge_state_tmp &= 0xffffff00;
        charge_state_tmp += value;
    }
    if(EXTI_GetITStatus(EXTI_Line7) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line7);
        value = get_charge_gpio_value();
        charge_state_tmp &= 0xffff00ff;
        charge_state_tmp += value << 8;
    }
    OSMboxPost(charge_state_mailbox, (void*)charge_state_tmp);// 此处的邮箱可以更改为信号量
    OSIntExit();
}


#include "can_protocol_task.h"
extern CanRxMsg RxMessage;
void USB_LP_CAN1_RX0_IRQHandler(void)
{
#if 0
    OSIntEnter();
    can_pkg_t can_pkg_tmp;
    CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
    can_pkg_tmp.id.canx_id = RxMessage.ExtId;
    can_pkg_tmp.len = RxMessage.DLC;
    memcpy(can_pkg_tmp.data.can_data, RxMessage.Data, can_pkg_tmp.len);
    put_can_pkg_to_fifo(can_fifo, can_pkg_tmp);
    OSIntExit();
#else
    OSIntEnter();
    can_pkg_t *can_buf;
    uint8_t err = 0;
    while((CAN1->RF0R & 0x03) > 0)
    {
        CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
        can_buf = (can_pkg_t *)OSMemGet(can_rcv_buf_mem_handle, &err);
        CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0 | CAN_IT_FOV0);   //清除中断

        if((can_buf != 0) && (err == OS_ERR_NONE))
        {
            can_buf->id.canx_id = RxMessage.ExtId;
            can_buf->len = RxMessage.DLC;
            memcpy(can_buf->data.can_data, RxMessage.Data, can_buf->len);
            OSQPost(can_rcv_buf_queue_handle, (void *)can_buf);
        }
//        else
//        {
//            can_rcv_buf_mem_handle = OSMemCreate((void *)&can_rcv_buf_mem[0][0], sizeof(can_rcv_buf_mem) / sizeof(can_rcv_buf_mem[0]), sizeof(can_pkg_t), &err);
//            if(can_rcv_buf_mem_handle == 0)
//            {
//                /*
//                todo: err process
//                */
//            }
//        }
    }

    OSIntExit();
#endif
}


/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/
