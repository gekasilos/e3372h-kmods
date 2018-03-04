/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2013-2015. All rights reserved.
 *
 * mobile@huawei.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef _wlan_if_h_
#define _wlan_if_h_

#include <linux/signal.h>
#include "wlan_utils.h"


#define    WLAN_TRACE_INFO(fmt, ...)    printk(fmt, ##__VA_ARGS__)
#define    WLAN_TRACE_ERROR(fmt, ...)    printk(fmt, ##__VA_ARGS__)


/* signal report */
typedef enum
{
    WIFI_SIGNAL_UPDATE_STA_LIST    = SIGIO,   /* WiFi STA�б���� */
    WIFI_SIGNAL_AUTO_SHUTDOWN      = SIGUSR2,   /* WiFi�Զ��ر��ź� */
}WLAN_SIGNAL_EVENT_ENUM;

/* event report */
typedef enum _WLAN_EVENT_TYPE
{
    USER_WIFI_TIMEOUT_EVENT         = 1,         /* WiFi�Զ��ر���Ϣ */
    USER_WIFI_UPDATE_STA_LIST       = 2,         /* STA������������¼� */
    USER_WIFI_DATA_DEAD_EVENT       = 32,        /* ���Ƶ���FW�쳣�ϱ� */
    USER_WIFI_CTRL_DEAD_EVENT       = 33,        /* ��������FW�쳣�ϱ� */
    USER_WIFI_NULL_EVENT = ((unsigned int)-1),   /* ���¼� */
}WLAN_EVENT_TYPE;

typedef struct _wlan_user_event
{
    WLAN_EVENT_TYPE eventId;
    unsigned int eventVal;
} WLAN_USER_EVENT;

 /* WiFiоƬ״̬���ṹ��,DHD�л�����ͬ�ṹ��Ķ���*/
 typedef struct
 {
     unsigned int rxerror;
     unsigned int txerror;
     unsigned int cmderror;
 }WLAN_STATUS_STU;


 /*===========================================================================
 
                         ������������
 
 ===========================================================================*/

/*****************************************************************************
 ��������  : WLAN_RETURN_TYPE wlan_signal_report(WiFi_SIGNAL_EVENT_ENUM signal)
 ��������  : ��Ӧ�ò㷢���ź�
 �������  : NA
 �������  : NA
 �� �� ֵ  : ��ǰ����WiFi����ֵ
*****************************************************************************/
WLAN_RETURN_TYPE wlan_signal_report(WLAN_SIGNAL_EVENT_ENUM signal);

/*****************************************************************************
 ��������  : WLAN_RETURN_TYPE wlan_event_report(WLAN_USER_EVENT *event)
 ��������  : ��Ӧ�ò㷢���¼�
 �������  : NA
 �������  : NA
 �� �� ֵ  : ��ǰ����WiFi����ֵ
*****************************************************************************/
WLAN_RETURN_TYPE wlan_event_report(WLAN_USER_EVENT *event);


#endif


