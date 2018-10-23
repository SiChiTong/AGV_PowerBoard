/**
 ******************************************************************************
 * @file    MicoDriverCan.h
 * @author  Adam Huang
 * @version V1.0.0
 * @date    13-Jan-2017
 * @brief   This file provides all the headers of Can operation functions.
 ******************************************************************************
 **/

#ifndef __MICODRIVERCAN_H__
#define __MICODRIVERCAN_H__

#pragma once

#include "common.h"
#include "platform.h"
#include "platform_peripheral.h"

OSStatus can_init( mico_can_t can );

OSStatus can_message_send( mico_can_t can, const CanTxMsgTypeDef *msg);

OSStatus can_message_read( mico_can_t can, const void *msg );
//OSStatus can_message_read( mico_can_t can, const CanRxMsgTypeDef *msg );

#endif

