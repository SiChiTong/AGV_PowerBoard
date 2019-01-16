/**
******************************************************************************
* @file    can.c
* @author  Kaka.Xie
* @brief   This file provides CAN protocol functions.
******************************************************************************

******************************************************************************
*/
#include "can_protocol_task.h"
#include "delay.h"
#include "platform.h"
#include <string.h>
#include <stdio.h>
#include "can.h"
#include "can_fifo.h"


//#define CanProtocolLog(format, ...)  custom_log("can protocol", format, ##__VA_ARGS__)

CanRxMsg RxMessage;
CanTxMsg TxMessage;
uint8_t CanTxdataBuff[CAN_LONG_FRAME_LENTH_MAX] = {0};


#define HW_VERSION_V0_2                  "NOAH_PM_V0.2"
#define HW_VERSION_V0_3                  "NOAH_PM_V0.3"
#define SW_VERSION                      "NOAHC001M08B109"
#define PROTOCOL_VERSION                "20170619P0001"

#define CMD_NOT_FOUND   0

uint16_t CmdProcessing(can_id_union *id, uint8_t *data_in, uint16_t data_len, uint8_t *data_out)
{
    id->can_id_t.ack = 1;
    switch(id->can_id_t.func_id)
    {
//        case CAN_FUN_ID_RESET:
//            //          platform_mcu_reset();
//            break;
//        case CAN_FUN_ID_WRITE:
//        case CAN_FUN_ID_READ:
//            switch(id->can_id_t.source_id)
//            {
//                case CAN_SOURCE_ID_READ_VERSION:
//                    //SW_VERSION
//                    break;
//                case CAN_READ_DATA:

//                    return 1;
//                default :
//                    break;
//            }
//        default:
//            break;

                case CAN_FUN_ID_RESET:
//            platform_mcu_reset();
            break;
        case CAN_FUN_ID_WRITE:
        case CAN_FUN_ID_READ:
            switch(id->can_id_t.source_id)
            {
                case CAN_SOURCE_ID_READ_VERSION:
                    data_out[0] = data_in[0];
                    if(data_in[0] == 1)//read software version
                    {
                        memcpy(&data_out[2], SW_VERSION, sizeof(SW_VERSION));
                        //return strlen(SW_VERSION) + 1;
                        data_out[1] = strlen(SW_VERSION);
                        return sizeof(SW_VERSION) + 2;
                    }
                    else if(data_in[0] == 2)//protocol version
                    {
                        memcpy(&data_out[2], PROTOCOL_VERSION, sizeof(PROTOCOL_VERSION));
                        data_out[1] = strlen(PROTOCOL_VERSION);
                        return sizeof(PROTOCOL_VERSION) + 2;

                    }
                    else if(data_in[0] == 3)//hardware version
                    {
//                        memcpy(&data_out[2], boardStatus->hw_version, strlen(boardStatus->hw_version));
//                        data_out[1] = strlen(boardStatus->hw_version);
//                        return strlen(boardStatus->hw_version) + 2;
                    }
                    return CMD_NOT_FOUND;
                    break;
//                case CAN_READ_DATA:

//                    break;
                case CAN_SOURCE_ID_SET_MODULE_STATE:

                    data_out[0] = data_in[0];

                    {
                        if(data_in[1] == 1)//group num = 1
                        {
//                            //uint32_t module = (data_in[5]) | (data_in[4]<<8) | (data_in[3] << 16) | (data_in[2] << 24);
//                            uint32_t module = *(uint32_t *)&data_in[2];
//                            power_on_off_type_def on_off;
//                            if((data_in[6] != POWER_OFF) && (data_in[6] != POWER_ON))
//                            {
//                                return 0;
//                            }
//                            on_off = (power_on_off_type_def)data_in[6];
//                            module &= 0xffffffff;
//                            power_ctrl((power_enable_type_def)module, on_off);
//                            uint32_t tmp = get_module_power_state(POWER_ALL);
//                            data_out[1] = data_in[1];
//                            *(uint32_t*)&data_out[2] = module;
//                            *(uint32_t*)&data_out[6] = tmp;
//                            data_out[10] = on_off;
//                            return 11;
                        }
#if 0
                        if(data_in[1] == 2)//group num = 2
                        {

                            uint32_t module = (data_in[5]) | (data_in[4] << 8) | (data_in[3] << 16) | (data_in[2] << 24);
                            module &= 0xffffffff;
                            BSP_Power_OnOffEx((PowerEnable_2_TypeDef)module,(power_on_off_type_def)data_in[6]);
                            uint32_t tmp = GetModulePowerStateEx(POWER_ALL_2);
                            data_out[1] = data_in[1];
                            memcpy(&data_out[2],(uint8_t *)(&tmp) , 4);
                            return 6;
                        }
#endif
                    }
                    return CMD_NOT_FOUND;

                case CAN_SOURCE_ID_GET_MODULE_STATE:

                    {

                        if(data_in[1] == 1)//group num = 1
                        {
//                            uint32_t tmp = get_module_power_state(POWER_ALL);
//                            data_out[1] = 1;
//                            *(uint32_t*)&data_out[2] = tmp;


                            return 6;
                        }
                    }
                    return CMD_NOT_FOUND;

                case CAN_SOURCE_ID_GET_BAT_STATE:
//                    if(battery_pack.com_status == false)
//                    {
//                        *(uint16_t*)&data_out[1] = voltageConvert->bat_voltage;
//                        data_out[3] = 0;
//                    }
//                    else
//                    {
//                        *(uint16_t*)&data_out[1] = battery_pack.pack_voltage;
//                        data_out[3] = battery_pack.percentage;
//                    }

                    return 4;

                    break;
                case CAN_SOURCE_ID_GET_SYS_STATE:
//                    *(uint16_t*)&data_out[1] = boardStatus->system_status;
                    return 3;
                    break;

                case CAN_SOURCE_ID_GET_ADC_DATA:
//                    memcpy(data_out, (uint8_t *)voltageConvert, sizeof(voltage_data_t));
//                    return sizeof(voltage_data_t);
                    break;
                case CAN_SOURCE_ID_SET_IR_LED_LIGHTNESS:
                    {
#if 1
//                        uint8_t duty = data_in[0];
//                        if(duty > 100)
//                        {
//                            duty = 100;
//                        }
//                        brightness_dimming(50000,  duty);
//                        boardStatus->irled_duty = duty;
//                        data_out[0] = data_in[0];
//                        data_out[1] = duty;
                        return 2;
#endif
                        break;
                    }

                case CAN_SOURCE_ID_SET_LED_EFFECT:
                    if(data_in[0] == 0)
                    {
//                        light_mode_t mode;
//                        uint8_t period = data_in[5];
//                        color_t *color;
//                        mode =  (light_mode_t)data_in[1];
//                        color = (color_t*)&data_in[2];
//                        set_serial_leds_effect(mode, color, period);
                        return 0;
                    }
                    return 0;
                    break;



#if 0
                case CAN_SOURCE_ID_ERROR_STATE:
                    memcpy(data_out, voltageConvertData->faultBitTemp, 5);
                    return 5;

                case CAN_SOURCE_ID_READ_ADC_DATA:
                    voltageDebug.isNeedUpload = YES;
                    voltageDebug.uploadRate = data_in[0];
                    return 0;
                case CAN_SOURCE_ID_READ_RK_STATE:

                    break;
                case CAN_SOURCE_ID_PWM_LED:

                    break;
#endif

                case CAN_SOURCE_ID_REMOTE_POWRER_CTRL:
                    {
                        data_out[0] = data_in[0];
//                        if((YES == boardStatus->is_power_off_finished) && (YES == boardStatus->is_power_on_finished) && (boardStatus->system_status & STATE_RUN_BITS))
//                        {
//                            if((data_in[0] == 1) || (data_in[0] == 2))
//                            {
//                                boardStatus->remote_device_power_ctrl = data_in[0];
//                                data_out[1] = 0; //succeed
//                            }
//                            else
//                            {
//                                data_out[1] = 1; //parameter error
//                            }
//                        }
//                        else
//                        {
//                            data_out[1] = 2;    //device is not power on yet
//                        }
//                        return 2;
//                        break;
                    }
                case CAN_SOURCE_ID_GET_SERIALS_LEDS_VERSION:
//                    get_serials_leds_version();
                    break;
                default :
                    break;
            }


        default:
            break;
    }
    return 0;
}


#define ONLYONCE       0x00
#define BEGIN         0x01
#define TRANSING       0x02
#define END            0x03


void Can1_TX(uint32_t canx_id,uint8_t* pdata,uint16_t len)
{
    //return ;
    uint16_t t_len;
    uint16_t roundCount;
    uint8_t modCount;
    can_data_union TxMsg = {0};
    //CanTxMsgTypeDef *TxMessage = platform_can_drivers[can_type].handle->pTxMsg;

    t_len = len;
    roundCount = t_len/7;
    modCount = t_len%7;

    TxMessage.ExtId = canx_id;
    TxMessage.IDE   = CAN_ID_EXT;
    TxMessage.RTR   = CAN_RTR_DATA;
    //if(roundCount <= 1)
    if(t_len <= 7)
    {
        TxMsg.can_data_t.seg_polo = ONLYONCE;
        TxMessage.DLC = t_len+1;

        memcpy(&TxMessage.Data[1],pdata,t_len);
        TxMessage.Data[0] = TxMsg.can_data[0];

        if((CAN_USED->TSR&0x1C000000))
        {
            CAN_Transmit(CAN1, &TxMessage);//
        }
        else
        {
            printf("TX busy ! \r\n");
        }
        return ;
    }

    {
        int num;
        {
            for(num = 0; num < roundCount; num++)
            {
                //SET SEGPOLO
                if( num == 0)
                {
                    TxMsg.can_data_t.seg_polo = BEGIN;
                }
                else
                {
                    TxMsg.can_data_t.seg_polo = TRANSING;
                }

                if( modCount == 0 && num == roundCount-1)
                {
                    TxMsg.can_data_t.seg_polo = END;
                }

                TxMsg.can_data_t.seg_num = num;
                memcpy(TxMsg.can_data_t.data, &pdata[num*7], 7);
                memcpy(TxMessage.Data, TxMsg.can_data, 8);
                TxMessage.DLC = 8;
                if((CAN_USED->TSR&0x1C000000))
                {
                    CAN_Transmit(CAN1, &TxMessage);//
                }
                else
                {
                    printf("TX busy ! \r\n");
                }

                //TRANSMIT LAST MSG
                if( modCount !=0 && num == roundCount-1 )
                {
                    num++;
                    TxMsg.can_data_t.seg_polo = END;
                    TxMsg.can_data_t.seg_num = num;
                    memcpy(TxMsg.can_data_t.data,&pdata[num*7],modCount);
                    memcpy(TxMessage.Data,TxMsg.can_data,modCount+1);
                    TxMessage.DLC = modCount+1;
                    if((CAN_USED->TSR&0x1C000000))
                    {
                        CAN_Transmit(CAN1, &TxMessage);//
                    }
                    else
                    {
                        printf("TX busy ! \r\n");
                    }
                }
            }
        }
    }
}



can_long_buf_t can_long_frame_buf_ram;
can_long_buf_t *can_long_frame_buf = &can_long_frame_buf_ram;

#define CAN_LONG_BUF_FULL   0xff
static uint8_t get_one_free_buf(void)
{
    for(uint8_t i = 0; i < CAN_LONG_BUF_NUM; i++)
    {
        if(can_long_frame_buf->can_rcv_buf[i].used_len == 0)
        {
            return i;
        }
    }
    return CAN_LONG_BUF_FULL;
}
static void free_buf(uint8_t index)
{
    can_long_frame_buf->can_rcv_buf[index].can_id = 0;
    can_long_frame_buf->can_rcv_buf[index].used_len = 0;
}
#define CAN_BUF_NO_THIS_ID      0xfe
static uint8_t get_the_buf_by_id(uint32_t id)
{
    for(uint8_t i = 0; i < CAN_LONG_BUF_NUM; i++)
    {
        if(id == can_long_frame_buf->can_rcv_buf[i].can_id)
        {
            return i;
        }
    }
    return CAN_BUF_NO_THIS_ID;
}
void can_long_buf_init(void)
{
    can_long_frame_buf->get_one_free_buf = get_one_free_buf;
    can_long_frame_buf->get_the_buf_by_id = get_the_buf_by_id;
    can_long_frame_buf->free_buf = free_buf;
    can_fifo_init(can_fifo, can_pkg, CAN_FIFO_SIZE);
}

#define CAN_LONG_FRAME_TIME_OUT     5 * OS_TICKS_PER_SEC
#define CAN_COMM_TIME_OUT           5 * OS_TICKS_PER_SEC
uint32_t can_comm_start_time;

#define CAN_COM_TIME_OUT    5 * OS_TICKS_PER_SEC
static uint32_t can_com_start_time = 0;


OS_STK can_protocol_task_stk[CAN_PROTOCOL_TASK_STK_SIZE];

void can_protocol_task(void *pdata)
{
    static can_id_union id = {0};
    static can_data_union rx_buf = {0};
    delay_ms(500);
    while(1)
    {
        can_pkg_t can_pkg_tmp = {0};
        uint8_t buf_index = 0;
        uint8_t seg_polo = 0;
        uint8_t seg_num = 0;
        uint16_t tx_len = 0;
        uint8_t rx_len = 0;

        if(is_can_fifo_empty(can_fifo) == FALSE)
        {
            get_can_pkg_from_fifo(can_fifo, &can_pkg_tmp);

            memcpy(rx_buf.can_data,  can_pkg_tmp.data.can_data, can_pkg_tmp.len);
            id.canx_id = can_pkg_tmp.id.canx_id;
            rx_len = can_pkg_tmp.len;
            seg_polo = rx_buf.can_data_t.seg_polo;
            seg_num = rx_buf.can_data_t.seg_num;

            if(id.can_id_t.dest_mac_id == POWERBOARD_CAN_MAC_SRC_ID)
            {
                can_com_start_time = get_tick();
                if(rx_buf.can_data_t.seg_polo == ONLYONCE)
                {
                    //if((id.canx_id_t.source_id < SOURCE_ID_PREPARE_UPDATE) && (id.canx_id_t.source_id > SOURCE_ID_CHECK_TRANSMIT))
                    {
                        //process the data here//
                        tx_len = CmdProcessing(&id, rx_buf.can_data_t.data, rx_len - 1, CanTxdataBuff);
                        //process the data here//
                        if(tx_len > 0)
                        {
                            Can1_TX(id.canx_id, CanTxdataBuff, tx_len);
                        }

                        //CanTX(MICO_CAN1, id.canx_id, test_data, sizeof(test_data));
                    }
                }
                else //long frame
                {
                    for(uint8_t i = 0; i < CAN_LONG_BUF_NUM; i++)
                    {
                        if(can_long_frame_buf->can_rcv_buf[i].used_len > 0)
                        {
                            if(get_tick() - can_long_frame_buf->can_rcv_buf[i].start_time > CAN_LONG_FRAME_TIME_OUT)
                            {
                                can_long_frame_buf->free_buf(i);
                                printf("LONG FRAME RCV TIMEOUT! ! ! !\r\n");
                            }
                        }
                    }

                    if(seg_polo == BEGIN)
                    {
                        buf_index = can_long_frame_buf->get_the_buf_by_id(id.canx_id);
                        if(buf_index == CAN_BUF_NO_THIS_ID)
                        {
                            buf_index = can_long_frame_buf->get_one_free_buf();
                        }
                        else
                        {
                            //
                        }

                        if((buf_index == CAN_LONG_BUF_FULL) || (buf_index >= CAN_LONG_BUF_NUM))
                        {
                            printf("LONG FRAME RCV BUF IS FULL! ! ! !\r\n");
                            goto exit;
                        }
                        memcpy(&can_long_frame_buf->can_rcv_buf[buf_index].rcv_buf[0], rx_buf.can_data_t.data, CAN_ONE_FRAME_DATA_LENTH);
                        can_long_frame_buf->can_rcv_buf[buf_index].used_len = CAN_ONE_FRAME_DATA_LENTH;
                        can_long_frame_buf->can_rcv_buf[buf_index].can_id = id.canx_id;
                        can_long_frame_buf->can_rcv_buf[buf_index].start_time = get_tick();
                    }
                    else if((seg_polo == TRANSING) || (seg_polo == END))
                    {
                        buf_index = can_long_frame_buf->get_the_buf_by_id(id.canx_id);
                        if((buf_index == CAN_BUF_NO_THIS_ID) || (buf_index >= CAN_LONG_BUF_NUM))
                        {
                            printf("ERROR ! !\r\n");
                            goto exit;
                        }
                        can_long_frame_buf->can_rcv_buf[buf_index].start_time = get_tick();
                        if(seg_polo == TRANSING)
                        {
                            memcpy(&can_long_frame_buf->can_rcv_buf[buf_index].rcv_buf[seg_num * CAN_ONE_FRAME_DATA_LENTH], rx_buf.can_data_t.data, CAN_ONE_FRAME_DATA_LENTH);
                            can_long_frame_buf->can_rcv_buf[buf_index].used_len += CAN_ONE_FRAME_DATA_LENTH;
                        }
                        if(seg_polo == END)
                        {
                            memcpy(&can_long_frame_buf->can_rcv_buf[buf_index].rcv_buf[seg_num * CAN_ONE_FRAME_DATA_LENTH], rx_buf.can_data_t.data, rx_len - 1);
                            can_long_frame_buf->can_rcv_buf[buf_index].used_len += rx_len - 1;

                            printf("long frame receive complete\r\n");
                            for(uint8_t j = 0; j < can_long_frame_buf->can_rcv_buf[buf_index].used_len; j++)
                            {
                                printf("data[%d]: %d\r\n", j, can_long_frame_buf->can_rcv_buf[buf_index].rcv_buf[j]);
                            }
                            //process the data here//
                            /**********************/
                            //process the data here//

                            Can1_TX(id.canx_id, can_long_frame_buf->can_rcv_buf[buf_index].rcv_buf, can_long_frame_buf->can_rcv_buf[buf_index].used_len);  // test :send the data back;
                            can_long_frame_buf->free_buf(buf_index);
                        }
                    }
                }
            }
        }
        else
        {
            delay_ms(20);
        }
    }

exit:
    return;
}


