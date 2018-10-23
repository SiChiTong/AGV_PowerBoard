/*
 *  Author: Adam Huang
 *  Date:2016/12/13
 */
#include "protocol.h"
#include "app_platform.h"
#include <stdlib.h>
#include "serial_leds.h"
#include "voltage_detect.h"
#include "stringUtils.h"
//#include "nbosDriverFlash.h"
#include "upgrade_flash.h"
#include "tps611xx_bl.h"
#include "battery.h"

#define protocol_log(M, ...) custom_log("Protocol", M, ##__VA_ARGS__)
#define protocol_log_trace() custom_log_trace("Protocol")


static  uint8_t    rx_buf[UART_RX_BUFFER_LENGTH];
static  uint8_t    tx_buf[UART_TX_BUFFER_LENGTH];

rcv_buf_t                rxInfoInRam;

serial_t                serialDataInRam = {
    .rx_buf = {
        .p_buffer = rx_buf,
        .offset = rx_buf + 3, //header,length,ctype
        .buffer_size = UART_RX_BUFFER_LENGTH,
    },
    .tx_buf = {
        .p_buffer = tx_buf,
        .offset = tx_buf + 2, //header,length
        .buffer_size = UART_TX_BUFFER_LENGTH,
    },
};

uart_serial_t           uartSerialInRam;

serial_t * const serial = &serialDataInRam;
//UART_HandleTypeDef      ComUartHandle;

static OSStatus uart_frame_send(serial_t *serial, const uint8_t *p_data, uint32_t size);
static OSStatus RcvSerialLedsFrameProcess(serial_t *serial);
OSStatus AckSerialLedsFrameProcess(serial_t *serial, uint8_t mode, color_t *color, uint8_t period);
//static OSStatus recReadSysStatusVbatFrameProcess(serial_t *serial);
//static OSStatus ackReadSysStatusVbatFrameProcess(serial_t *serial, uint8_t cmd_num);
static OSStatus RcvReadModuleStatusFrameProcess(serial_t *serial);
static OSStatus AckReadModuleStatusFrameProcess(serial_t *serial, uint8_t cmd_num);
//static OSStatus recReadFaultStatusFrameProcess(serial_t *serial);
//static OSStatus ackReadFaultStatusFrameProcess(serial_t *serial, uint8_t cmd_num);
static OSStatus RcvModuleControlFrameProcess(serial_t *serial);
static OSStatus ackModuleControlFrameProcess(serial_t *serial, uint32_t ack);
static OSStatus ackNotSupportFrameProcess(serial_t *serial, uint8_t ctype);
static OSStatus recTestCurrentCmdFrame(serial_t *serial);
static OSStatus ackReadErrChannelFrameProcess(serial_t *serial);
static OSStatus RcvIRLedControlFrameProcess(serial_t *serial);
static OSStatus ackIRLedControlFrameProcess(serial_t *serial);


OSStatus protocol_init(void)//(serial_t *serial)
{
    OSStatus err = kNoErr;

    //serial = &serialDataInRam;
    //require_action(serial , exit, err = kGeneralErr);
    if(serial->is_serial_initialized)
    {

        {
            goto exit;
        }
        //MicoUartFinalize(COMM_UART);
        //serial->is_serial_initialized = 0;

    }


    serial->rx_info = &rxInfoInRam;
    require_action(serial->rx_info , exit, err = kGeneralErr);
    serial->uart_serial = &uartSerialInRam;
    require_action(serial->uart_serial , exit, err = kGeneralErr);

    serial->rx_info->p_data = 0;
    serial->rx_info->rx_buffer = rx_buf;
    serial->rx_info->rx_count = 0;

    // serial->uart_serial->uartHandle = &ComUartHandle;
    // require_action(serial->uart_serial->uartHandle, exit, err = kGeneralErr);

    //  serial->uart_serial->UARTx = USART_COM;

    err = uart_ser_open(serial->uart_serial, 115200);
    require_action(err == kNoErr, exit,  protocol_log("open uart err"));

    require_action(serial->rx_buf.p_buffer , exit, err = kGeneralErr);
    memset((uint8_t *)serial->rx_buf.p_buffer, 0x00, serial->rx_buf.buffer_size);
#ifdef COMM_DMA_USE_INT
    startDmaRecive(serial->uart_serial->uartHandle, (uint8_t *)serial->rx_buf.p_buffer);
#else
    startDmaRecive(COMM_UART, (uint8_t *)serial->rx_buf.p_buffer);
#endif
    serial->is_start_dma_receive = 1;
    serial->is_serial_initialized = 1;
    protocol_log("protocol initialize success");

exit:
    return err;
}

static OSStatus RcvSerialLedsFrameProcess(serial_t *serial)
{
    OSStatus err = kNoErr;
    uint8_t       ligth_mode;
    //uint16_t      lightEffect;
    rcv_serial_leds_frame_t *serial_leds_frame;
    color_t *color;
    uint8_t period;
    //ack_serial_leds_frame_t *ack_serial_leds_frame;

    require_action(serial , exit, err = kGeneralErr);
    require_action(serial->uart_serial , exit, err = kGeneralErr);

    serial_leds_frame = (rcv_serial_leds_frame_t *)(serial->rx_buf.offset);
    ligth_mode = serial_leds_frame->cur_light_mode;
    color = &serial_leds_frame->color;
    period = serial_leds_frame->period;


    //ack_serial_leds_frame->ctype = FRAME_TYPE_LEDS_CONTROL;
    //ack_serial_leds_frame->cur_light_mode = ligth_mode;
    //memcpy(ack_serial_leds_frame->color, color, sizeof(color_t));

    if(STATE_IS_POWER_OFF != (boardStatus->system_status & STATE_RUN_BITS))
    {
        set_serial_leds_effect((light_mode_t)ligth_mode, color , period);
    }

    //ack_serial_leds_frame = (ack_serial_leds_frame_t *)serial->tx_buf.offset;
    //require_action(ack_serial_leds_frame , exit, err = kGeneralErr);


    //ack_serial_leds_frame->color = color;

    //err = AckSerialLedsFrameProcess(serial, ligth_mode, color, period);

exit:
    return err;
}

OSStatus AckSerialLedsFrameProcess(serial_t *serial, uint8_t mode, color_t *color, uint8_t period)
{
    OSStatus err = kNoErr;
    uint8_t  length = sizeof(ack_serial_leds_frame_t);
    ack_serial_leds_frame_t *ack_serial_leds_frame;

    require_action(serial , exit, err = kGeneralErr);
    require_action(serial->uart_serial , exit, err = kGeneralErr);
    ack_serial_leds_frame = (ack_serial_leds_frame_t *)serial->tx_buf.offset;
    ack_serial_leds_frame->ctype = FRAME_TYPE_LEDS_CONTROL;

    memcpy(&ack_serial_leds_frame->color, color, sizeof(color_t));
    ack_serial_leds_frame->period = period;
    ack_serial_leds_frame->cur_light_mode = mode;

    err = uart_frame_send(serial, (uint8_t *)ack_serial_leds_frame/*serial->tx_buf.offset*/, length);

exit:
    return err;
}

    static OSStatus
uart_frame_send(serial_t *serial, const uint8_t *p_data, uint32_t size)
{
    OSStatus err = kNoErr;
    uint8_t  checkSum;
    ram_buff_t *tx_buff = NULL;

    require_action(serial , exit, err = kGeneralErr);
    require_action(serial->uart_serial , exit, err = kGeneralErr);

    tx_buff = &serial->tx_buf;
    require_action(tx_buff->p_buffer , exit, err = kGeneralErr);
    require_action(size <= tx_buff->buffer_size - 4, exit, err = kParamErr);

    *(uint8_t *)tx_buff->p_buffer = FRAME_HEADER;
    *(uint8_t *)(tx_buff->p_buffer + 1) = size + 4;//header,length,footer,checksum

    checkSum = 0;
    tx_buff->p_data = (uint8_t *)tx_buff->p_buffer;
    if(p_data != tx_buff->offset)
    {
        memmove((void *)tx_buff->offset, (void *)p_data, size);
    }
    for(uint8_t i = 0; i < size + 2; i++)
    {
        checkSum += *tx_buff->p_data ++;
    }
    *tx_buff->p_data ++ = checkSum;
    *tx_buff->p_data ++ = FRAME_FOOTER;

    err = uart_ser_write(serial->uart_serial, \
            (uint8_t *)tx_buff->p_buffer, tx_buff->p_data - tx_buff->p_buffer);

    memset((uint8_t *)tx_buff->p_buffer, 0x00, tx_buff->buffer_size);

exit:
    return err;
}

#if 0
static OSStatus recReadSysStatusVbatFrameProcess(serial_t *serial)
{
    OSStatus err = kNoErr;
    uint8_t  cmd_num;
    require_action(serial , exit, err = kGeneralErr);
    require_action(serial->uart_serial , exit, err = kGeneralErr);

    cmd_num = *(uint8_t *)serial->rx_buf.offset;

    err = ackReadSysStatusVbatFrameProcess(serial, cmd_num);

exit:
    return err;
}
#endif


OSStatus AckReadSysStatusFrameProcess(serial_t *serial, uint8_t cmd_num)
{
    OSStatus err = kNoErr;
    uint8_t  length = sizeof(ack_sys_status_t);
    ack_sys_status_t *ack_sys_status_frame;

    require_action(serial , exit, err = kGeneralErr);
    require_action(serial->uart_serial , exit, err = kGeneralErr);
    ack_sys_status_frame = (ack_sys_status_t*)serial->tx_buf.offset;
    ack_sys_status_frame->cmd_num = cmd_num;
    ack_sys_status_frame->ctype = FRAME_TYPE_SYS_STATUS;
    ack_sys_status_frame->sys_status = boardStatus->system_status;
    err = uart_frame_send(serial, (uint8_t *)ack_sys_status_frame, length);
exit:
    return err;
}
static OSStatus RcvReadSysStatusFrameProcess(serial_t *serial)
{
    OSStatus err = kNoErr;
    uint8_t  cmd_num;
    require_action(serial , exit, err = kGeneralErr);
    require_action(serial->uart_serial , exit, err = kGeneralErr);
    cmd_num = *(uint8_t *)serial->rx_buf.offset;
    err = AckReadSysStatusFrameProcess(serial , cmd_num);
exit:
    return err;
}


static OSStatus AckReadVbatFrameProcess(serial_t *serial, uint8_t cmd_num)
{
    OSStatus err = kNoErr;
    uint8_t  length = sizeof(ack_sys_status_t);
    ack_bat_status_t *bat_status_frame;
    require_action(serial , exit, err = kGeneralErr);
    require_action(serial->uart_serial , exit, err = kGeneralErr);
    bat_status_frame = (ack_bat_status_t*)serial->tx_buf.offset;
    bat_status_frame->cmd_num = cmd_num;
    bat_status_frame->ctype = FRAME_TYPE_BAT_STATUS;
    if(cmd_num == 1)
    {
        bat_status_frame->bat_status = voltageConvert->bat_voltage;
    }
    else if(cmd_num == 2)
    {
        bat_status_frame->bat_status = battery_pack.percentage;  //test code
    }

    err = uart_frame_send(serial, (uint8_t *)bat_status_frame, length);
exit:
    return err;
}
static OSStatus RcvReadBatStatus(serial_t *serial)
{
    OSStatus err = kNoErr;
    uint8_t  cmd_num;
    require_action(serial , exit, err = kGeneralErr);
    require_action(serial->uart_serial , exit, err = kGeneralErr);
    cmd_num = *(uint8_t *)serial->rx_buf.offset;
    err = AckReadVbatFrameProcess(serial , cmd_num);
exit:
    return err;
}

#if 0
static OSStatus ackReadSysStatusVbatFrameProcess(serial_t *serial, uint8_t cmd_num)
{
    OSStatus err = kNoErr;
    uint8_t  length = sizeof(ack_sys_status_vbat_frame_t);
    ack_sys_status_vbat_frame_t *ackSysStatusVbatFrame;

    require_action(serial , exit, err = kGeneralErr);
    require_action(serial->uart_serial , exit, err = kGeneralErr);


    ackSysStatusVbatFrame = (ack_sys_status_vbat_frame_t *)serial->tx_buf.offset;
    require_action(ackSysStatusVbatFrame , exit, err = kGeneralErr);

    ackSysStatusVbatFrame->ctype = FRAME_TYPE_S_SYS_V_BAT;
    ackSysStatusVbatFrame->cmd_num = cmd_num;
    switch(ackSysStatusVbatFrame->cmd_num)
    {
        case SYS_VBAT_READ_CMD:
            ackSysStatusVbatFrame->system_status = boardStatus->system_status;
            ackSysStatusVbatFrame->voltage = boardStatus->bat_level;
            break;
        case SYS_VCHARGE_READ_CMD:
            ackSysStatusVbatFrame->system_status = boardStatus->system_status;
            ackSysStatusVbatFrame->voltage = boardStatus->charger_level;
            break;
        default:
            ackSysStatusVbatFrame->system_status = boardStatus->system_status;
            ackSysStatusVbatFrame->voltage = 0xFFFF;
            break;
    }
    err = uart_frame_send(serial, (uint8_t *)ackSysStatusVbatFrame, length);

exit:
    return err;
}
#endif
static OSStatus RcvReadModuleStatusFrameProcess(serial_t *serial)
{
    OSStatus err = kNoErr;
    uint8_t  cmd_num;
    require_action(serial , exit, err = kGeneralErr);
    require_action(serial->uart_serial , exit, err = kGeneralErr);

    cmd_num = *(uint8_t *)serial->rx_buf.offset;

    err = AckReadModuleStatusFrameProcess(serial, cmd_num);

exit:
    return err;
}

static OSStatus AckReadModuleStatusFrameProcess(serial_t *serial, uint8_t cmd_num)
{
    OSStatus err = kNoErr;
    uint8_t  length = sizeof(ack_module_status_frame_t);
    ack_module_status_frame_t *ackModuleStatusFrame;
    uint32_t moduleState;

    require_action(serial , exit, err = kGeneralErr);
    require_action(serial->uart_serial , exit, err = kGeneralErr);


    ackModuleStatusFrame = (ack_module_status_frame_t *)serial->tx_buf.offset;
    require_action(ackModuleStatusFrame , exit, err = kGeneralErr);

    ackModuleStatusFrame->ctype = FRAME_TYPE_MODULE_STATE;
    ackModuleStatusFrame->cmd_num = cmd_num;

    switch(ackModuleStatusFrame->cmd_num)
    {
        case MODULE_STATUS_CMD_NUM:

            moduleState = get_modules_states();
            memcpy(ackModuleStatusFrame->module_status, (uint8_t *)&moduleState, sizeof(uint32_t));

            break;
        default:
            memset(ackModuleStatusFrame->module_status, 0xFF, 4);
            break;
    }
    err = uart_frame_send(serial, (uint8_t *)ackModuleStatusFrame, length);

exit:
    return err;
}

#if 0
static OSStatus recReadFaultStatusFrameProcess(serial_t *serial)
{
    OSStatus err = kNoErr;
    uint8_t  cmd_num;
    require_action(serial, exit, err = kGeneralErr);
    require_action(serial->uart_serial , exit, err = kGeneralErr);

    cmd_num = *(uint8_t *)serial->rx_buf.offset;

    err = ackReadFaultStatusFrameProcess(serial, cmd_num);

exit:
    return err;
}
#endif

#if 0
static OSStatus ackReadFaultStatusFrameProcess(serial_t *serial, uint8_t cmd_num)
{
    OSStatus err = kNoErr;
    uint8_t  length = sizeof(ackFaultStatusFrame_t);
    ackFaultStatusFrame_t *ackFaultStatusFrame;

    require_action(serial, exit, err = kGeneralErr);
    require_action(serial->uart_serial , exit, err = kGeneralErr);


    ackFaultStatusFrame = (ackFaultStatusFrame_t *)serial->tx_buf.offset;
    require_action(ackFaultStatusFrame , exit, err = kGeneralErr);

    ackFaultStatusFrame->ctype = FRAME_TYPE_FAULT_BIT;
    ackFaultStatusFrame->cmd_num = cmd_num;

    switch(ackFaultStatusFrame->cmd_num)
    {
        case FAULT_STATUS_CMD_NUM:
            memcpy(ackFaultStatusFrame->fault_bit, voltageConvertData->faultBitTemp, 4);
            break;
        default:
            memset(ackFaultStatusFrame->fault_bit, 0xFF, 4);
            break;
    }
    err = uart_frame_send(serial, (uint8_t *)ackFaultStatusFrame, length);

exit:
    return err;
}
#endif

#ifdef NOT_USE_TMP
static void setModulePowerOnOff(uint8_t module, uint8_t onoff)
{
    switch(module)
    {
        case MOTOR_MODULE:
            if(onoff == 0x01) {
                power_ctrl(POWER_MOTOR, POWER_ON);
            }
            else {
                power_ctrl(POWER_MOTOR, POWER_OFF);
            }
            break;
        case SENSOR_MODULE:
            if(onoff == 0x01) {
                power_ctrl(POWER_SENSOR, POWER_ON);
            }
            else {
                power_ctrl(POWER_SENSOR, POWER_OFF);
            }
            break;
        case LEDS_MODULE:
            if(onoff == 0x01) {
                power_ctrl(POWER_LEDS, POWER_ON);
            }
            else {
                power_ctrl(POWER_LEDS, POWER_OFF);
            }
            break;
        case _5VRESERVE_MODULE:
            if(onoff == 0x01) {
                power_ctrl(POWER_5V_RES, POWER_ON);
            }
            else {
                power_ctrl(POWER_5V_RES, POWER_OFF);
            }
            break;
        case _12V_ROUTER_MODULE:
            if(onoff == 0x01) {
                power_ctrl(POWER_5V_RES, POWER_ON);
            }
            else {
                power_ctrl(POWER_5V_RES, POWER_OFF);
            }
            break;
        case _2_1_PA_MODULE:
            if(onoff == 0x01) {
                power_ctrl(POWER_2_1_PA, POWER_ON);
            }
            else {
                power_ctrl(POWER_2_1_PA, POWER_OFF);
            }
            break;
        case DYP_MODULE:
            if(onoff == 0x01) {
                power_ctrl(POWER_DYP, POWER_ON);
            }
            else {
                power_ctrl(POWER_DYP, POWER_OFF);
            }
            break;
        case _12V_RESERVE_MODULE:
            if(onoff == 0x01) {
                power_ctrl(POWER_12V_RES, POWER_ON);
            }
            else {
                power_ctrl(POWER_12V_RES, POWER_OFF);
            }
            break;
        case PRINTER_MODULE:
            if(onoff == 0x01) {
                power_ctrl(POWER_PRINTER, POWER_ON);
            }
            else {
                power_ctrl(POWER_PRINTER, POWER_OFF);
            }
            break;
        case _24V_RESERVE_MODULE:
            if(onoff == 0x01) {
                power_ctrl(POWER_24V_RES, POWER_ON);
            }
            else {
                power_ctrl(POWER_24V_RES, POWER_OFF);
            }
            break;
        case BAT_NV_MODULE:
            if(onoff == 0x01) {
                power_ctrl(POWER_BAT_NV, POWER_ON);
            }
            else {
                power_ctrl(POWER_BAT_NV, POWER_OFF);
            }
            break;
        case _5V_ALL_MODULE:
            if(onoff == 0x01) {
                power_ctrl(POWER_5V_EN, POWER_ON);
            }
            else {
                power_ctrl(POWER_5V_EN, POWER_OFF);
            }
            break;
        case _12V_ALL_MODULE:
            if(onoff == 0x01) {
                power_ctrl(POWER_12V_EN, POWER_ON);
            }
            else {
                power_ctrl(POWER_12V_EN, POWER_OFF);
            }
            break;
        case _24V_ALL_MODULE:
            if(onoff == 0x01) {
                power_ctrl(POWER_24V_EN, POWER_ON);
            }
            else {
                power_ctrl(POWER_24V_EN, POWER_OFF);
            }
            break;
        case AIUI_MODULE:
            if(onoff == 0x01) {
                power_ctrl(POWER_AIUI, POWER_ON);
            }
            else {
                power_ctrl(POWER_AIUI, POWER_OFF);
            }
            break;
        case _5V_ROUTER_MODULE:
            if(onoff == 0x01) {
                power_ctrl(POWER_5V_ROUTOR, POWER_ON);
            }
            else {
                power_ctrl(POWER_5V_ROUTOR, POWER_OFF);
            }
            break;
    }
}
#endif
static OSStatus RcvModuleControlFrameProcess(serial_t *serial)
{
    OSStatus err = kNoErr;
    rcv_module_control_frame_t *rcv_module_control_frame;
    uint32_t state = 0;
    uint8_t on_off;
    uint32_t module;


    require_action(serial, exit, err = kGeneralErr);
    require_action(serial->uart_serial , exit, err = kGeneralErr);

    rcv_module_control_frame = (rcv_module_control_frame_t *)serial->rx_buf.offset;
    require_action(rcv_module_control_frame , exit, err = kGeneralErr);

    on_off = rcv_module_control_frame->control;
    module = rcv_module_control_frame->module;
    if((rcv_module_control_frame->module < POWER_ALL) && (rcv_module_control_frame->module > 0))
    {
        power_ctrl((power_enable_type_def)module, (power_on_off_type_def)on_off);
    }
    else
    {
        err = ackModuleControlFrameProcess(serial, HW_NO_SUPPORT);
        goto exit;
    }
#if 0
    switch(rcv_module_control_frame->module)
    {
        case SYSTEM_MODULE:
            if(rcv_module_control_frame->control == 0x00)//shutdown system
            {
                power_off_devices();
            }
            else if(rcv_module_control_frame->control == 0x01)// reboot system
            {
                flashTable.is_need_auto_reboot = 'Y';
                MICOBootConfiguration(&flashTable);
                power_off_devices();
                boardStatus->reboot_flag = REBOOT_YES;
            }
            break;
#if 0   // do not delete
        case POWER_5V_MOTOR:
        case POWER_5V_RECHARGE:
        case POWER_5V_SENSOR_BOARD:
        case POWER_5V_SWITCH:
        case POWER_5V_ROUTER:
        case POWER_5V_EN:

        case POWER_12V_PAD:
        case POWER_12V_2_1_PA:
        case POWER_12V_EXTEND:
        case POWER_12V_X86 :
        case POWER_12V_NV:
        case POWER_12V_EN:


        case POWER_24V_EN:
        case POWER_24V_PRINTER:
        case POWER_24V_EXTEND:
        case POWER_VSYS_24V_NV:

        case POWER_485:
        case POWER_SYS_LED:
        case POWER_RECHARGE_LED:
        case POWER_SLAM:
            power_ctrl((power_enable_type_def)rcv_module_control_frame->module, (power_on_off_type_def)rcv_module_control_frame->control);
            break;
#endif
#ifdef NOT_USE_TMP
            setModulePowerOnOff(recModuleControlFrame->module, recModuleControlFrame->control);
#endif
            break;
        case PAD_MODULE:
        case X86_MODULE:
        case NV_MODULE:
        case DLP_MODULE:
            set_module_power_signal(rcv_module_control_frame->module, rcv_module_control_frame->control);
            break;
        default:
            err = ackModuleControlFrameProcess(serial, HW_NO_SUPPORT);
            goto exit;
            break;
    }
#endif


    state =  get_module_power_state(POWER_ALL);
#if 0
    if(on_off == POWER_ON)
    {
        state |= module;
    }
    if(on_off == POWER_OFF)
    {
        state &= ~module;
    }
#endif
    err = ackModuleControlFrameProcess(serial, state);

exit:
    return err;
}

static OSStatus ackModuleControlFrameProcess(serial_t *serial, uint32_t ack)
{
    OSStatus err = kNoErr;
    uint8_t  length = sizeof(ack_general_frame_t);
    ack_general_frame_t *ackGeneralFrame;

    require_action(serial, exit, err = kGeneralErr);
    require_action(serial->uart_serial , exit, err = kGeneralErr);


    ackGeneralFrame = (ack_general_frame_t *)serial->tx_buf.offset;
    require_action(ackGeneralFrame , exit, err = kGeneralErr);

    ackGeneralFrame->ctype = FRAME_TYPE_MODULE_CONTROL;
    ackGeneralFrame->ack = ack;

    err = uart_frame_send(serial, (uint8_t *)ackGeneralFrame, length);

exit:
    return err;
}

static OSStatus RcvIRLedControlFrameProcess(serial_t *serial)
{
    OSStatus err = kNoErr;
    rcv_irled_control_frame_t *rcv_irled_control_frame;

    require_action(serial, exit, err = kGeneralErr);
    require_action(serial->uart_serial , exit, err = kGeneralErr);

    rcv_irled_control_frame = (rcv_irled_control_frame_t *)serial->rx_buf.offset;
    require_action(rcv_irled_control_frame , exit, err = kGeneralErr);

    if(rcv_irled_control_frame->type_wr == 0x00)//read
    {
    }
    else if(rcv_irled_control_frame->type_wr == 0x01)//write
    {
        brightness_dimming(50000, rcv_irled_control_frame->brightness);
        boardStatus->irled_duty = rcv_irled_control_frame->brightness;
    }
    err = ackIRLedControlFrameProcess(serial);

exit:
    return err;
}

static OSStatus ackIRLedControlFrameProcess(serial_t *serial)
{
    OSStatus err = kNoErr;
    uint8_t  length = sizeof(ack_general_frame_t);
    ack_general_frame_t *ackGeneralFrame;

    require_action(serial, exit, err = kGeneralErr);
    require_action(serial->uart_serial , exit, err = kGeneralErr);


    ackGeneralFrame = (ack_general_frame_t *)serial->tx_buf.offset;
    require_action(ackGeneralFrame , exit, err = kGeneralErr);

    ackGeneralFrame->ctype = FRAME_TYPE_IRLED_CONTROL;
    ackGeneralFrame->ack = (uint8_t) boardStatus->irled_duty;

    err = uart_frame_send(serial, (uint8_t *)ackGeneralFrame, length);

exit:
    return err;
}

static OSStatus ackNotSupportFrameProcess(serial_t *serial, uint8_t ctype)
{
    OSStatus err = kNoErr;
    uint8_t  length = sizeof(ack_general_frame_t);
    ack_general_frame_t *ackGeneralFrame;

    require_action(serial, exit, err = kGeneralErr);
    require_action(serial->uart_serial, exit, err = kGeneralErr);


    ackGeneralFrame = (ack_general_frame_t *)serial->tx_buf.offset;
    require_action(ackGeneralFrame, exit, err = kGeneralErr);

    ackGeneralFrame->ctype = ctype;
    ackGeneralFrame->ack = HW_NO_SUPPORT;

    err = uart_frame_send(serial, (uint8_t *)ackGeneralFrame, length);

exit:
    return err;
}

static OSStatus ackFwVersionInfoFrameProcess(serial_t *serial);
static OSStatus ackProtocolVersionInfoFrameProcess(serial_t *serial);

static OSStatus recVersionInfoFrameProcess(serial_t *serial)
{
    OSStatus err = kNoErr;
    uint8_t readCommand;

    require_action(serial, exit, err = kGeneralErr);
    require_action(serial->uart_serial, exit, err = kGeneralErr);

    readCommand = *serial->rx_buf.offset;
    switch(readCommand)
    {
        case READ_FW_VERSION_COMMAND:
            err = ackFwVersionInfoFrameProcess(serial);
            break;
        case READ_PROTOCOL_VERSION:
            err = ackProtocolVersionInfoFrameProcess(serial);
            break;
        default:
            break;
    }

exit:
    return err;
}

static OSStatus ackFwVersionInfoFrameProcess(serial_t *serial)
{
    OSStatus err = kNoErr;
    uint8_t length = sizeof(ack_version_info_frame_t);
    uint8_t swVersion[16] = SW_VERSION;
    //uint8_t hwVersion[13] = HW_VERSION;
    //hwVersion[2] = 0;
    ack_version_info_frame_t   *ackVersionInfoFrame;

    require_action(serial, exit, err = kGeneralErr);
    require_action(serial->uart_serial, exit, err = kGeneralErr);

    ackVersionInfoFrame = (ack_version_info_frame_t *)serial->tx_buf.offset;
    require_action(ackVersionInfoFrame, exit, err = kGeneralErr);

    ackVersionInfoFrame->ctype = FRAME_TYPE_VERSION_INFO;
    ackVersionInfoFrame->ver_type = VERSION_TYPE_FARMWARE;
    //memcpy(ackVersionInfoFrame->hw, (void const*)hwVersion, 3);
    memcpy(ackVersionInfoFrame->sw, (void const*)swVersion, 16);

    err = uart_frame_send(serial, (uint8_t *)ackVersionInfoFrame, length);

exit:
    return err;
}

static OSStatus ackProtocolVersionInfoFrameProcess(serial_t *serial)
{
    OSStatus err = kNoErr;
    uint8_t length = sizeof(ack_version_info_frame_t);
    int8_t swVersion[13] = PROTOCOL_VERSION;
    ack_version_info_frame_t   *ackVersionInfoFrame;

    require_action(serial, exit, err = kGeneralErr);
    require_action(serial->uart_serial, exit, err = kGeneralErr);

    ackVersionInfoFrame = (ack_version_info_frame_t *)serial->tx_buf.offset;
    require_action(ackVersionInfoFrame, exit, err = kGeneralErr);

    ackVersionInfoFrame->ctype = FRAME_TYPE_VERSION_INFO;
    ackVersionInfoFrame->ver_type = VERSION_TYPE_PROTOCOL;
    memcpy(&ackVersionInfoFrame->hw, (void const*)swVersion, 13);

    err = uart_frame_send(serial, (uint8_t *)ackVersionInfoFrame, length);

exit:
    return err;
}

static OSStatus recFirmwareUpgradeProcess(serial_t *serial);
static OSStatus generalAckUpgradeProcess(serial_t *serial, uint8_t upgradeType, uint8_t ack_state);
static OSStatus recPrepareUpgradeFrameProcess(serial_t *serial);
static OSStatus recFirmwarePackageFrameProcess(serial_t *serial);
static OSStatus recUpgradeFinishCheckFrameProcess(serial_t *serial);

static OSStatus recFirmwareUpgradeProcess(serial_t *serial)
{
    OSStatus err = kNoErr;
    uint8_t upgradeType;

    require_action(serial, exit, err = kGeneralErr);
    require_action(serial->uart_serial, exit, err = kGeneralErr);

    upgradeType = *(uint8_t *)serial->rx_buf.offset;

    switch(upgradeType)
    {
        case UPGRADE_FRAME_TYPE_PREPARE:
            boardStatus->is_upgrading = 1;
            err = recPrepareUpgradeFrameProcess(serial);
            break;
        case UPGRADE_FRAME_TYPE_GOING:
            err = recFirmwarePackageFrameProcess(serial);
            break;
        case UPGRADE_FRAME_TYPE_FINISH:
            err = recUpgradeFinishCheckFrameProcess(serial);
            boardStatus->is_upgrading = 0;
            break;
        default:
            err = generalAckUpgradeProcess(serial, 0xFF, 0xFF);//no support
            break;
    }

exit:
    return err;
}

static OSStatus recPrepareUpgradeFrameProcess(serial_t *serial)
{
    OSStatus err = kNoErr;
    uint32_t firmware_size;
    rec_prepare_upgrade_frame_t *recPrepareUpgradeFrame;
    mico_logic_partition_t *ota_partition_info;

    ota_partition_info = MicoFlashGetInfo(MICO_PARTITION_OTA_TEMP);
    require_action(ota_partition_info->partition_owner != MICO_FLASH_NONE, exit, err = kUnsupportedErr);

    recPrepareUpgradeFrame = (rec_prepare_upgrade_frame_t *)serial->rx_buf.offset;
    require_action(recPrepareUpgradeFrame , exit, err = kGeneralErr);

    firmware_size = ReadBig32(recPrepareUpgradeFrame->firmware_size);
    if(firmware_size > ota_partition_info->partition_length)
    {
        err = generalAckUpgradeProcess(serial, UPGRADE_FRAME_TYPE_PREPARE, MCU_STORAGE_NOT_ENOUGH);
        protocol_log("not enough storage");
        goto exit;
    }
    protocol_log("firmware_size is:%d", firmware_size);
    if(!upgradePrepareFlash(recPrepareUpgradeFrame->md5, firmware_size))
    {
        err = generalAckUpgradeProcess(serial, UPGRADE_FRAME_TYPE_PREPARE, MCU_PREPARE_OK);
        protocol_log("mcu prepare ok");
    }
    else
    {
        err = generalAckUpgradeProcess(serial, UPGRADE_FRAME_TYPE_PREPARE, MCU_RETRY_LATER);
        protocol_log("mcu retry later");
    }

exit:
    return err;
}

static OSStatus generalAckUpgradeProcess(serial_t *serial, uint8_t upgradeType, uint8_t ack_state)
{
    OSStatus err = kNoErr;
    uint8_t length = sizeof(upgrade_general_ack_frame_t);
    upgrade_general_ack_frame_t *upgradeGeneralAckFrame;

    require_action(serial, exit, err = kGeneralErr);
    require_action(serial->uart_serial, exit, err = kGeneralErr);

    upgradeGeneralAckFrame = (upgrade_general_ack_frame_t *)serial->tx_buf.offset;
    require_action(upgradeGeneralAckFrame, exit, err = kGeneralErr);

    upgradeGeneralAckFrame->ctype = FRAME_TYPE_FW_UPGRADE;
    upgradeGeneralAckFrame->upgrade_frame_type = upgradeType;
    upgradeGeneralAckFrame->ack_state = ack_state;

    err = uart_frame_send(serial, (uint8_t *)upgradeGeneralAckFrame, length);

exit:
    return err;
}

static OSStatus recFirmwarePackageFrameProcess(serial_t *serial)
{
    OSStatus err = kNoErr;
    rec_firmware_package_frame_t  recFirmwarePackageFrame;
    uint32_t package_data_length;

    require_action(serial, exit, err = kGeneralErr);
    require_action(serial->uart_serial, exit, err = kGeneralErr);

    package_data_length = *(uint8_t *)(serial->rx_buf.offset - 2) - 6;
    require_action(package_data_length, exit, err = kGeneralErr);

    recFirmwarePackageFrame.upgrade_frame_type = *(uint8_t *)(serial->rx_buf.offset);
    recFirmwarePackageFrame.package_data = (uint8_t *)(serial->rx_buf.offset + 1);

    if(!upgradeWriteFlashData((uint32_t *)recFirmwarePackageFrame.package_data, package_data_length))
    {
        err = generalAckUpgradeProcess(serial, UPGRADE_FRAME_TYPE_GOING, PACKGE_PROCESS_OK);
    }
    else
    {
        err = generalAckUpgradeProcess(serial, UPGRADE_FRAME_TYPE_GOING, PACKGE_PROCESS_FAIL);
    }
exit:
    return err;
}

static OSStatus recUpgradeFinishCheckFrameProcess(serial_t *serial)
{
    OSStatus err = kNoErr;
    rec_upgrade_finish_check_frame_t *recUpgradeFinishCheckFrame;

    require_action(serial, exit, err = kGeneralErr);
    require_action(serial->uart_serial, exit, err = kGeneralErr);

    recUpgradeFinishCheckFrame = (rec_upgrade_finish_check_frame_t *)serial->rx_buf.offset;
    require_action(recUpgradeFinishCheckFrame, exit, err = kGeneralErr);

    if(recUpgradeFinishCheckFrame->finish_flag == 0x00)
    {
        if(!upgradeCheckFlash())
        {
            err = generalAckUpgradeProcess(serial, UPGRADE_FRAME_TYPE_FINISH, PACKGE_PROCESS_OK);
            protocol_log("MD5 success,sent right ack");
        }
        else
        {
            err = generalAckUpgradeProcess(serial, UPGRADE_FRAME_TYPE_FINISH, FIRMWARE_CHECKSUM_ERR);
            protocol_log("MD5 err,sent err ack");
        }
    }
    else
    {
        err = generalAckUpgradeProcess(serial, UPGRADE_FRAME_TYPE_FINISH, 0xFF);
    }

exit:
    return err;
}

static OSStatus recTestCurrentCmdFrame(serial_t *serial)
{
    OSStatus err = kNoErr;
    rec_test_current_cmd_frame_t *recTestCurrentCmdFrame;

    require_action(serial, exit, err = kGeneralErr);
    require_action(serial->uart_serial, exit, err = kGeneralErr);

    recTestCurrentCmdFrame = (rec_test_current_cmd_frame_t *)serial->rx_buf.offset;
    require_action(recTestCurrentCmdFrame, exit, err = kGeneralErr);

    switch(recTestCurrentCmdFrame->send_rate)
    {
        case SEND_RATE_SINGLE:
            upload_current_info(serial, voltageConvert);
            break;
        case SEND_RATE_1HZ:
        case SEND_RATE_2HZ:
        case SEND_RATE_5HZ:
        case SEND_RATE_10HZ:
        case SEND_RATE_50HZ:
        case SEND_RATE_100HZ:
        case SEND_RATE_0_5HZ:
        case SEND_RATE_0_2HZ:
        case SEND_RATE_0_1HZ:
            voltageDebug.uploadRate = recTestCurrentCmdFrame->send_rate;
            break;
        default:
            //    ackTestCurrentCmdFrame(0x01);
            goto exit;
    }

    //  ackTestCurrentCmdFrame(0x00);

    voltageDebug.uploadFlagTime = os_get_time();
    switch(recTestCurrentCmdFrame->command)
    {
        case 0x00:
            voltageDebug.isNeedUpload = NO;
            break;
        case 0x01:
            voltageDebug.isNeedUpload = YES;
            break;
        default:
            break;
    }

exit:
    return err;
}

OSStatus upload_current_info(serial_t *serial, voltage_data_t *voltage_info)
{
    OSStatus err = kNoErr;
    upload_current_info_frame_t *uploadCurrentInfoFrame;
    uint8_t length = sizeof(upload_current_info_frame_t);

    require_action(serial, exit, err = kGeneralErr);
    require_action(serial->uart_serial, exit, err = kGeneralErr);
    require_action(voltage_info, exit, err = kGeneralErr);


    uploadCurrentInfoFrame = (upload_current_info_frame_t *)serial->tx_buf.offset;
    require_action(uploadCurrentInfoFrame, exit, err = kGeneralErr);

    uploadCurrentInfoFrame->ctype = FRAME_TYPE_TEST_CURRENT;
    uploadCurrentInfoFrame->cmd_type = 0x00;

    memcpy(&uploadCurrentInfoFrame->current_info, voltage_info, sizeof(upload_current_info_frame_t));
    uploadCurrentInfoFrame->send_rate = voltageDebug.uploadRate;
    memcpy(uploadCurrentInfoFrame->fault_bit, voltageConvertData->faultBitTemp, 4);

    err = uart_frame_send(serial, (uint8_t *)uploadCurrentInfoFrame, length);

exit:
    return err;
}

uint32_t map_time_by_rate(uint8_t send_rate)
{
    if(!IS_SEND_RATE_DATA(send_rate))
    {
        return 0;
    }
    switch(send_rate)
    {
        case SEND_RATE_1HZ:
            return 1000/SYSTICK_PERIOD;
        case SEND_RATE_2HZ:
            return 500/SYSTICK_PERIOD;
        case SEND_RATE_5HZ:
            return 200/SYSTICK_PERIOD;
        case SEND_RATE_10HZ:
            return 100/SYSTICK_PERIOD;
        case SEND_RATE_50HZ:
            return 20/SYSTICK_PERIOD;
        case SEND_RATE_100HZ:
            return 10/SYSTICK_PERIOD;
        case SEND_RATE_0_5HZ:
            return 2000/SYSTICK_PERIOD;
        case SEND_RATE_0_2HZ:
            return 5000/SYSTICK_PERIOD;
        case SEND_RATE_0_1HZ:
            return 10000/SYSTICK_PERIOD;
        default:
            return 0;
    }
}

static OSStatus recReadErrChannelFrameProcess(serial_t *serial)
{
    OSStatus err = kNoErr;
    uint8_t readLatestErrCmd;

    require_action(serial, exit, err = kGeneralErr);
    require_action(serial->uart_serial, exit, err = kGeneralErr);

    readLatestErrCmd = *(uint8_t *)serial->rx_buf.offset;

    if(readLatestErrCmd == 0x00)
    {
        err = ackReadErrChannelFrameProcess(serial);
    }
    else
    {
        err = ackNotSupportFrameProcess(serial, *(uint8_t *)(serial->rx_buf.offset - 1));
    }

exit:
    return err;
}

static OSStatus ackReadErrChannelFrameProcess(serial_t *serial)
{
    OSStatus err = kNoErr;
    uint8_t length = sizeof(err_info_frame_t);
    err_info_frame_t *errInfoFrame;

    require_action(serial, exit, err = kGeneralErr);
    require_action(serial->uart_serial, exit, err = kGeneralErr);

    errInfoFrame = (err_info_frame_t *)(uint8_t *)serial->tx_buf.offset;
    require_action(errInfoFrame, exit, err = kGeneralErr);
    errInfoFrame->ctype = FRAME_TYPE_READ_ERR_CURRENT;
    errInfoFrame->err_channel = boardStatus->err_channel;
    errInfoFrame->err_data = boardStatus->err_value;
    err = uart_frame_send(serial, (uint8_t *)errInfoFrame, length);

exit:
    return err;
}

static int is_receive_right_frame(void);
static int is_receive_right_frame(void)
{
    if(serial->rx_info->rx_count > 0 && *serial->rx_buf.p_buffer == FRAME_HEADER)
    {
        if(*(serial->rx_buf.p_buffer + 1) != 0)
        {
            if(*(serial->rx_buf.p_buffer + *(serial->rx_buf.p_buffer + 1) - 1) == FRAME_FOOTER)
            {
                return 0;
            }
        }
    }
    return -1;
}

void protocol_period(void)
{
    uint8_t checksum;
    uint8_t detect_type;

    if(!serial->is_serial_initialized)
    {
        protocol_init();
    }
#ifdef COMM_DMA_USE_INT
    serial->rx_info->rx_count = receviedDmaDataLength(serial->uart_serial->uartHandle);
#else
    serial->rx_info->rx_count = receviedDmaDataLength(COMM_UART);
#endif

    if(!is_receive_right_frame())//serial->rx_info->rx_count > 0 && serial->rx_buf.buffer)//&& rx_buf.rxBuffer[rxBuf[1]-1] == FRAME_FOOTER)//(rx_buf.rx_count > 0 && rx_buf.rxBuffer[rx_buf.rx_count-1] == 0xA5)
    {
#ifdef PROTOCOL_DEBUG
        if(OPEN_SHOW_LOG == serial->rx_info->show_log_flag)
        {
            char *debugString;
            debugString = DataToHexStringWithSpaces(\
                    (const uint8_t *)serial->rx_info->rx_buffer, serial->rx_info->rx_count);
            protocol_log("rxBuf: %s", debugString);
            free(debugString);
        }
#endif
        serial->rx_info->p_data = 0;
        if(serial->rx_info->start_time == 0)
        {
            if(STATE_POWER_ON == (boardStatus->system_status & STATE_RUN_BITS))
            {
                set_serial_leds_effect(LIGHTS_MODE_NOMAL, NULL, 0);
            }
            protocol_log("start communicating");
        }
        serial->rx_info->start_time = os_get_time();
    }
    else
    {
        if((serial->rx_info->start_time != 0) && \
                ((os_get_time() - serial->rx_info->start_time) >= \
                 COMMUNICATION_TIMEOUT_TIME_MS/SYSTICK_PERIOD))
        {
            serial->rx_info->start_time = 0;
            protocol_log("communicate timeout");
            if(STATE_POWER_ON == (boardStatus->system_status & STATE_RUN_BITS))
            {
                set_serial_leds_effect(LIGHTS_MODE_ERROR, NULL, 0);
            }
#ifdef COMM_DMA_USE_INT
            stopDmaRecive(serial->uart_serial->uartHandle);
#else
            stopDmaRecive(COMM_UART);
#endif
            serial->is_start_dma_receive = 0;
            //      serial->rx_info->startTime = os_get_time();
            protocol_init();
        }
        goto exit;
    }
#ifdef COMM_DMA_USE_INT
    stopDmaRecive(serial->uart_serial->uartHandle);
#else
    stopDmaRecive(COMM_UART);
#endif
    serial->is_start_dma_receive = 0;
    serial->rx_buf.receive_start_time = 0;

    checksum = 0;

    serial->rx_buf.p_data = (uint8_t *)serial->rx_buf.p_buffer;

    checksum += *serial->rx_buf.p_data;

    serial->rx_info->buffer_header.detect_length = *++serial->rx_buf.p_data;
    if(*(serial->rx_buf.p_data + serial->rx_info->buffer_header.detect_length - 2) != FRAME_FOOTER)
    {
        protocol_log("not known cmd");
        goto exit;
    }
    checksum += *serial->rx_buf.p_data;

    serial->rx_info->buffer_header.detect_type = *++serial->rx_buf.p_data;
    checksum += *serial->rx_buf.p_data;

    for(uint8_t i = 0; i < (serial->rx_info->buffer_header.detect_length - 5); i++)
    {
        checksum += *++serial->rx_buf.p_data;
    }
    require_action_quiet(checksum == *++serial->rx_buf.p_data, exit, \
            protocol_log("check sum not match"));

    detect_type = serial->rx_info->buffer_header.detect_type;
#if 1
    switch(detect_type)
    {
        case FRAME_TYPE_LEDS_CONTROL:     //done
            boardStatus->is_upgrading = 0;
            RcvSerialLedsFrameProcess(serial);
            break;

        case FRAME_TYPE_SYS_STATUS:       //done
            //recReadSysStatusVbatFrameProcess(serial);
            RcvReadSysStatusFrameProcess(serial);
            break;

        case FRAME_TYPE_BAT_STATUS:       //done
            RcvReadBatStatus(serial);
            break;


        case FRAME_TYPE_MODULE_STATE:
            RcvReadModuleStatusFrameProcess(serial);  //done
            break;
            //case FRAME_TYPE_FAULT_BIT:
            // recReadFaultStatusFrameProcess(serial);
            //break;
        case FRAME_TYPE_MODULE_CONTROL:                   //done
            RcvModuleControlFrameProcess(serial);
            break;

        case FRAME_TYPE_VERSION_INFO:
            recVersionInfoFrameProcess(serial);
            break;
        case FRAME_TYPE_FW_UPGRADE:
            recFirmwareUpgradeProcess(serial);
            break;
        case FRAME_TYPE_TEST_CURRENT:
            recTestCurrentCmdFrame(serial);
            break;
        case FRAME_TYPE_READ_ERR_CURRENT:
            recReadErrChannelFrameProcess(serial);
            break;
        case FRAME_TYPE_IRLED_CONTROL:            // done ??
            RcvIRLedControlFrameProcess(serial);
            break;
        default:
            ackNotSupportFrameProcess(serial, detect_type);
            break;
    }
#endif
#if 1
    if(serial->rx_info->rx_count)
    {
        memset((uint8_t *)serial->rx_buf.p_buffer, 0x00, serial->rx_buf.buffer_size);
    }
#ifdef COMM_DMA_USE_INT
    startDmaRecive(serial->uart_serial->uartHandle, (uint8_t *)serial->rx_buf.p_buffer);
#else
    startDmaRecive(COMM_UART, (uint8_t *)serial->rx_buf.p_buffer);
#endif
    serial->is_start_dma_receive = 1;
#endif
exit:
    if(serial->rx_info->rx_count == 0xFF)
    {
        memset((uint8_t *)serial->rx_buf.p_buffer, 0x00, serial->rx_buf.buffer_size);
#ifdef COMM_DMA_USE_INT
        startDmaRecive(serial->uart_serial->uartHandle, (uint8_t *)serial->rx_buf.p_buffer);
        stopDmaRecive(serial->uart_serial->uartHandle);
#else
        startDmaRecive(COMM_UART, (uint8_t *)serial->rx_buf.p_buffer);
        stopDmaRecive(COMM_UART);
#endif
        serial->is_start_dma_receive = 0;
    }
    if(!serial->is_start_dma_receive)
    {
        serial->rx_info->rx_count = 0;
        memset((uint8_t *)serial->rx_buf.p_buffer, 0x00, serial->rx_buf.buffer_size);
#ifdef COMM_DMA_USE_INT
        startDmaRecive(serial->uart_serial->uartHandle, (uint8_t *)serial->rx_buf.p_buffer);
#else
        startDmaRecive(COMM_UART, (uint8_t *)serial->rx_buf.p_buffer);
#endif
        serial->is_start_dma_receive = 1;
    }
    return;
}
