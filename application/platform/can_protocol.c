/*
 *  Author: Kaka Xie
 *  brief: CAN protocol process
 */

#include "can_protocol.h"
#include "protocol.h"
#include "platform.h"
#include "app_platform.h"
#include "stm32f1xx.h"
//#include "serial_leds.h"
#include "stm32f1xx_powerboard.h"
#include "Debug.h"


#include "can_fifo.h"
#include "battery.h"
#include "tps611xx_bl.h"

#define CanProtocolLog(format, ...)  custom_log("can protocol", format, ##__VA_ARGS__)

__IO uint32_t flag = 0xff;
//CanTxMsgTypeDef TxMessage;
CanRxMsgTypeDef RxMessage;
CanTxMsgTypeDef TxMessage;

extern platform_can_driver_t  platform_can_drivers[];
uint8_t CanTxdataBuff[CAN_LONG_FRAME_LENTH_MAX] = {0};

uint8_t swVersion[] = SW_VERSION;
//uint8_t hwVersion[] = HW_VERSION;

can_tx_data_t  CommandProcessing(uint32_t func_ID, uint8_t* dstPtr, uint8_t* pdata, uint32_t len);

can_fifo_t can_fifo_ram;
can_fifo_t *can_fifo = &can_fifo_ram;

can_pkg_t can_pkg[CAN_FIFO_SIZE] = {0};


typedef void (*CallBackFunc)(void);

typedef struct
{
    uint32_t val;
    CallBackFunc callback;
}CALLBACK_T;

const CALLBACK_T ID_table[]=
{
    {0,NULL},
    {1,NULL},
};

CALLBACK_T FuncId_table[] = {
    {0x00,NULL},
};


/**
 * @brief  rx msg handle
 * @param
 * @retval
 * @RevisionHistory
 */
CALLBACK_T* FuncIdHandle(uint32_t funcid)
{
    int i;
    int func_num = sizeof(FuncId_table)/sizeof(FuncId_table[0]);
    for(i = 0;i <func_num;i++)
    {
        if(FuncId_table[i].val == funcid)
        {
            return (&FuncId_table[i]);
        }
    }
    return NULL;
}

#define ONLYONCE       0x00
#define BEGIN         0x01
#define TRANSING       0x02
#define END            0x03


void CanTX(mico_can_t can_type, uint32_t canx_id,uint8_t* pdata,uint16_t len)
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
    TxMessage.IDE   = CAN_ID_EXT;   //扩展模式
    TxMessage.RTR   = CAN_RTR_DATA; //发送的是数据
    //if(roundCount <= 1)
    if(t_len <= 7)
    {
        TxMsg.can_data_t.seg_polo = ONLYONCE;
        TxMessage.DLC = t_len+1;


        memcpy(&TxMessage.Data[1],pdata,t_len);
        TxMessage.Data[0] = TxMsg.can_data[0];

        if((CAN_USED->TSR&0x1C000000))
        {
            can_message_send(MICO_CAN1, &TxMessage);//
        }
        return ;
    }

    {
        int num;
        {
            for(num = 0; num < roundCount; num++)
            {
                //SET SEGPOLO
                if(num == 0)
                {
                    TxMsg.can_data_t.seg_polo = BEGIN;
                }
                else
                {
                    TxMsg.can_data_t.seg_polo = TRANSING;
                }

                if(modCount == 0 && num == roundCount-1)
                {
                    TxMsg.can_data_t.seg_polo = END;
                }

                TxMsg.can_data_t.seg_num = num;
                memcpy(TxMsg.can_data_t.data, &pdata[num*7], 7);
                memcpy(TxMessage.Data, TxMsg.can_data, 8);
                TxMessage.DLC = 8;
                if((CAN_USED->TSR&0x1C000000))
                {
                    can_message_send(MICO_CAN1, &TxMessage);//发送报文
                }

                //TRANSMIT LAST MSG
                if(modCount !=0 && num == roundCount-1)
                {
                    num++;
                    TxMsg.can_data_t.seg_polo = END;
                    TxMsg.can_data_t.seg_num = num;
                    memcpy(TxMsg.can_data_t.data,&pdata[num*7],modCount);
                    memcpy(TxMessage.Data,TxMsg.can_data,modCount+1);
                    TxMessage.DLC = modCount+1;
                    if((CAN_USED->TSR&0x1C000000))
                    {
                        can_message_send(MICO_CAN1, &TxMessage);//
                    }
                }
            }

        }

    }
}

/*******************************ID HANDLE END*************************************************/



#define CAN_READ_DATA               0x80

void upload_adc_data(void)
{
    can_id_union id;
    id.canx_id_t.ack = 0;
    id.canx_id_t.dest_mac_id = 0;////
    id.canx_id_t.func_id = CAN_FUN_ID_TRIGGER;
    id.canx_id_t.source_id = CAN_SOURCE_ID_GET_ADC_DATA;
    id.canx_id_t.src_mac_id = CAN_NOAH_PB_ID;////

    CanTX(MICO_CAN1, id.canx_id, (uint8_t *)voltageConvert, sizeof(voltage_data_t));
}

void upload_sys_state(void)
{
    can_id_union id;
    uint8_t tx_buf[3];
    id.canx_id_t.ack = 0;
    id.canx_id_t.dest_mac_id = 0;////
    id.canx_id_t.func_id = CAN_FUN_ID_TRIGGER;
    id.canx_id_t.source_id = CAN_SOURCE_ID_GET_SYS_STATE;
    id.canx_id_t.src_mac_id = CAN_NOAH_PB_ID;////
    tx_buf[0] = 0;
    *(uint16_t*)&tx_buf[1] = boardStatus->system_status;
    CanTX(MICO_CAN1, id.canx_id, tx_buf, sizeof(tx_buf));
}

void upload_power_off_signal(uint32_t second)
{
    can_id_union id;
    uint8_t tx_buf[3];
    id.canx_id_t.ack = 0;
    id.canx_id_t.dest_mac_id = 0;////
    id.canx_id_t.func_id = CAN_FUN_ID_TRIGGER;
    id.canx_id_t.source_id = CAN_SOURCE_ID_POWER_OFF_SIGNAL;
    id.canx_id_t.src_mac_id = CAN_NOAH_PB_ID;////
    tx_buf[0] = second;

    CanTX(MICO_CAN1, id.canx_id, tx_buf, sizeof(tx_buf));
}

void upload_bat_info(void)
{
    can_id_union id;
    uint8_t tx_buf[4];
    id.canx_id_t.ack = 0;
    id.canx_id_t.dest_mac_id = 0;////
    id.canx_id_t.func_id = CAN_FUN_ID_TRIGGER;
    id.canx_id_t.source_id = CAN_SOURCE_ID_GET_BAT_STATE;
    id.canx_id_t.src_mac_id = CAN_NOAH_PB_ID;////
    tx_buf[0] = 0;
    //tx_buf[1] = 0;
    if(battery_pack.com_status == false)
    {
        *(uint16_t*)&tx_buf[1] = voltageConvert->bat_voltage;
        tx_buf[3] = 0;
    }
    else
    {
        *(uint16_t*)&tx_buf[1] = battery_pack.pack_voltage;
        tx_buf[3] = battery_pack.percentage;
    }

    CanTX(MICO_CAN1, id.canx_id, tx_buf, sizeof(tx_buf));
}

void ack_leds_effect(light_mode_t light_mode, color_t *cur_color, uint8_t period)
{
    can_id_union id;
    uint8_t tx_buf[7];
    id.canx_id_t.ack = 1;
    id.canx_id_t.dest_mac_id = 0;////
    id.canx_id_t.func_id = CAN_FUN_ID_TRIGGER;
    id.canx_id_t.source_id = CAN_SOURCE_ID_SET_LED_EFFECT;
    id.canx_id_t.src_mac_id = CAN_NOAH_PB_ID;////

    tx_buf[0] = 0;
    tx_buf[1] = 0;
    tx_buf[2] = light_mode;
    *(color_t *)&tx_buf[3] = *cur_color;
    tx_buf[6] = period;

    CanTX(MICO_CAN1, id.canx_id, tx_buf, sizeof(tx_buf));
}

void ack_serials_leds_version(uint8_t *data, uint8_t len)
{
    can_id_union id;
    id.canx_id_t.ack = 1;
    id.canx_id_t.dest_mac_id = 0;////
    id.canx_id_t.func_id = CAN_FUN_ID_TRIGGER;
    id.canx_id_t.source_id = CAN_SOURCE_ID_GET_SERIALS_LEDS_VERSION;
    id.canx_id_t.src_mac_id = CAN_NOAH_PB_ID;////

    CanTX(MICO_CAN1, id.canx_id, data, len);
}

#define CMD_NOT_FOUND   0
uint16_t process_can_protocol(can_id_union *id, const uint8_t *data_in, const uint16_t data_in_len, uint8_t *data_out)
{
    //uint8_t data_out_len;
    id->canx_id_t.ack = 1;
    id->canx_id_t.dest_mac_id = id->canx_id_t.src_mac_id;
    id->canx_id_t.src_mac_id = CAN_NOAH_PB_ID;
    id->canx_id_t.res = 0;
    //id->canx_id_t.func_id =
    switch(id->canx_id_t.func_id)
    {
        case CAN_FUN_ID_RESET:
            platform_mcu_reset();
            break;
        case CAN_FUN_ID_WRITE:
        case CAN_FUN_ID_READ:
            switch(id->canx_id_t.source_id)
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
                        memcpy(&data_out[2], boardStatus->hw_version, strlen(boardStatus->hw_version));
                        data_out[1] = strlen(boardStatus->hw_version);
                        return strlen(boardStatus->hw_version) + 2;
                    }
                    return CMD_NOT_FOUND;
                    break;
                case CAN_READ_DATA:

                    break;
                case CAN_SOURCE_ID_SET_MODULE_STATE:

                    data_out[0] = data_in[0];

                    {
                        if(data_in[1] == 1)//group num = 1
                        {
                            //uint32_t module = (data_in[5]) | (data_in[4]<<8) | (data_in[3] << 16) | (data_in[2] << 24);
                            uint32_t module = *(uint32_t *)&data_in[2];
                            power_on_off_type_def on_off;
                            if((data_in[6] != POWER_OFF) && (data_in[6] != POWER_ON))
                            {
                                return 0;
                            }
                            on_off = (power_on_off_type_def)data_in[6];
                            module &= 0xffffffff;
                            power_ctrl((power_enable_type_def)module, on_off);
                            uint32_t tmp = get_module_power_state(POWER_ALL);
                            data_out[1] = data_in[1];
                            *(uint32_t*)&data_out[2] = module;
                            *(uint32_t*)&data_out[6] = tmp;
                            data_out[10] = on_off;
                            return 11;
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
                            uint32_t tmp = get_module_power_state(POWER_ALL);
                            data_out[1] = 1;
                            *(uint32_t*)&data_out[2] = tmp;
                            //memcpy(&data_out[2],(uint8_t *)(&tmp) , 4);

                            return 6;
                        }
                    }
                    return CMD_NOT_FOUND;

                case CAN_SOURCE_ID_GET_BAT_STATE:
                    if(battery_pack.com_status == false)
                    {
                        *(uint16_t*)&data_out[1] = voltageConvert->bat_voltage;
                        data_out[3] = 0;
                    }
                    else
                    {
                        *(uint16_t*)&data_out[1] = battery_pack.pack_voltage;
                        data_out[3] = battery_pack.percentage;
                    }

                    return 4;

                    break;
                case CAN_SOURCE_ID_GET_SYS_STATE:
                    *(uint16_t*)&data_out[1] = boardStatus->system_status;
                    return 3;
                    break;

                case CAN_SOURCE_ID_GET_ADC_DATA:
                    memcpy(data_out, (uint8_t *)voltageConvert, sizeof(voltage_data_t));
                    return sizeof(voltage_data_t);
                    break;
                case CAN_SOURCE_ID_SET_IR_LED_LIGHTNESS:
                    {
#if 1
                        uint8_t duty = data_in[0];
                        if(duty > 100)
                        {
                            duty = 100;
                        }
                        brightness_dimming(50000,  duty);
                        boardStatus->irled_duty = duty;
                        data_out[0] = data_in[0];
                        data_out[1] = duty;
                        return 2;
#endif
                        break;
                    }

                case CAN_SOURCE_ID_SET_LED_EFFECT:
                    if(data_in[0] == 0)
                    {
                        light_mode_t mode;
                        uint8_t period = data_in[5];
                        color_t *color;
                        mode =  (light_mode_t)data_in[1];
                        color = (color_t*)&data_in[2];
                        set_serial_leds_effect(mode, color, period);
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
                        if((YES == boardStatus->is_power_off_finished) && (YES == boardStatus->is_power_on_finished) && (boardStatus->system_status & STATE_RUN_BITS))
                        {
                            if((data_in[0] == 1) || (data_in[0] == 2))
                            {
                                boardStatus->remote_device_power_ctrl = data_in[0];
                                data_out[1] = 0; //succeed
                            }
                            else
                            {
                                data_out[1] = 1; //parameter error
                            }
                        }
                        else
                        {
                            data_out[1] = 2;    //device is not power on yet
                        }
                        return 2;
                        break;
                    }
                case CAN_SOURCE_ID_GET_SERIALS_LEDS_VERSION:
                    get_serials_leds_version();
                    break;
                default :
                    break;
            }


        default:
            break;
    }

    return CMD_NOT_FOUND;
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

#define CAN_LONG_FRAME_TIME_OUT     5000/SYSTICK_PERIOD

#define CAN_COMM_TIME_OUT           5000
uint32_t can_comm_start_time;


void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan)
{
    CanProtocolLog("can err callback");
}



#define CAN_LONG_FRAME_TIME_OUT     5000/SYSTICK_PERIOD

#define CAN_COMM_TIME_OUT           5000
uint32_t can_comm_start_time;

#define CAN_COM_TIME_OUT    10000/SYSTICK_PERIOD
static uint32_t can_com_start_time = 0;
void can_protocol_period(void)
{
    static can_id_union id;
    uint16_t tx_len;
    uint8_t rx_len;
    static can_data_union rx_buf;
    can_pkg_t can_pkg_tmp;
    uint8_t buf_index;
    uint8_t seg_polo;
    uint8_t seg_num;
#if 0
    static uint8_t set_led_flag = 0;

    if(boardStatus->system_status & STATE_POWER_ON)
    {

        if(os_get_time() - can_com_start_time >= CAN_COM_TIME_OUT)
        {
            set_serial_leds_effect(LIGHTS_MODE_COM_ERROR, NULL, 0);
            set_led_flag = 0;
        }
        else
        {
            if(set_led_flag == 0)
            {
                set_serial_leds_effect(LIGHTS_MODE_NOMAL, NULL, 0);
                set_led_flag = 1;
            }
        }
    }
#endif

    while(is_can_fifo_empty(can_fifo) == FALSE)
    {
        get_can_pkg_from_fifo(can_fifo, &can_pkg_tmp);

        memcpy(rx_buf.can_data,  can_pkg_tmp.data.can_data, can_pkg_tmp.len);
        id.canx_id = can_pkg_tmp.id.canx_id;
        rx_len = can_pkg_tmp.len;
        seg_polo = rx_buf.can_data_t.seg_polo;
        seg_num = rx_buf.can_data_t.seg_num;

        if(id.canx_id_t.dest_mac_id == CAN_NOAH_PB_ID)
        {
            can_com_start_time = os_get_time();
            if(rx_buf.can_data_t.seg_polo == ONLYONCE)
            {

                //if((id.canx_id_t.source_id < SOURCE_ID_PREPARE_UPDATE) && (id.canx_id_t.source_id > SOURCE_ID_CHECK_TRANSMIT))
                {
                    //process the data here//
                    tx_len = process_can_protocol(&id, rx_buf.can_data_t.data, rx_len - 1, CanTxdataBuff);
                    //process the data here//
                    if(tx_len > 0)
                    {
                        CanTX(MICO_CAN1, id.canx_id, CanTxdataBuff, tx_len);
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
                        if(os_get_time() - can_long_frame_buf->can_rcv_buf[i].start_time > CAN_LONG_FRAME_TIME_OUT)
                        {
                            can_long_frame_buf->free_buf(i);
                            CanProtocolLog("LONG FRAME RCV TIMEOUT! ! ! !\r\n");
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
                        CanProtocolLog("LONG FRAME RCV BUF IS FULL! ! ! !\r\n");
                        goto exit;
                    }
                    memcpy(&can_long_frame_buf->can_rcv_buf[buf_index].rcv_buf[0], rx_buf.can_data_t.data, CAN_ONE_FRAME_DATA_LENTH);
                    can_long_frame_buf->can_rcv_buf[buf_index].used_len = CAN_ONE_FRAME_DATA_LENTH;
                    can_long_frame_buf->can_rcv_buf[buf_index].can_id = id.canx_id;
                    can_long_frame_buf->can_rcv_buf[buf_index].start_time = os_get_time();
                }
                else if((seg_polo == TRANSING) || (seg_polo == END))
                {
                    buf_index = can_long_frame_buf->get_the_buf_by_id(id.canx_id);
                    if((buf_index == CAN_BUF_NO_THIS_ID) || (buf_index >= CAN_LONG_BUF_NUM))
                    {
                        CanProtocolLog("ERROR ! !\r\n");
                        goto exit;
                    }
                    can_long_frame_buf->can_rcv_buf[buf_index].start_time = os_get_time();
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

                        CanTX(MICO_CAN1, id.canx_id, can_long_frame_buf->can_rcv_buf[buf_index].rcv_buf, can_long_frame_buf->can_rcv_buf[buf_index].used_len);  // test :send the data back;
                        can_long_frame_buf->free_buf(buf_index);
                    }
                }
            }

        }



    }
    //can_message_read(MICO_CAN1, &RxMessage);


exit:
    //platform_can_drivers[MICO_CAN1].rx_complete = 0;

    return;
}







