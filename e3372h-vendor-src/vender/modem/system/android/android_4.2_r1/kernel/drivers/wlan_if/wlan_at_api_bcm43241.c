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
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/string.h>
#include <product_config.h>

#include "wlan_at.h"
#include "wlan_utils.h"
#include "drv_version.h"
#include <bsp_shared_ddr.h>

#define WLAN_AT_SSID_SUPPORT            2                  /*֧�ֵ�SSID����*/
#define WLAN_AT_KEY_SUPPORT             5                  /*֧�ֵķ�����*/

#ifdef BSP_CONFIG_BOARD_E5_E5578
#define WLAN_AT_MODE_SUPPORT            "2,3,4"          /*֧�ֵ�ģʽ(b/g/n)*/
#else
#define WLAN_AT_MODE_SUPPORT            "1,2,3,4"          /*֧�ֵ�ģʽ(a/b/g/n)*/
#endif

#define WLAN_AT_BAND_SUPPORT            "0,1"              /*֧�ֵĴ���(20M/40M/80M/160M)*/

#ifdef BSP_CONFIG_BOARD_E5_E5578
#define WLAN_AT_TSELRF_SUPPORT          "0,1"          /*֧�ֵ�������������*/
#else
#define WLAN_AT_TSELRF_SUPPORT          "0,1,2,3"          /*֧�ֵ�������������*/
#endif

#define WLAN_AT_GROUP_MAX               4                  /*֧�ֵ������������*/
#define WLAN_AT_TYPE_MAX                2                  /*֧�ֻ�ȡ�������Ϣ����*/

/*WIFI���ʵ�������*/
#define WLAN_AT_POWER_MIN               (-15)
#define WLAN_AT_POWER_MAX               (30)

/*WIFI��������ջ�ģʽ*/
#define WLAN_AT_WIFI_TX_MODE            (17)
#define WLAN_AT_WIFI_RX_MODE            (18)

/*WiFi����ģʽ*/
#define AT_WIFI_MODE_ONLY_PA            0x00                /*WIFIֻ֧��PAģʽ*/
#define AT_WIFI_MODE_ONLY_NOPA          0x01                /*WIFIֻ֧��NO PAģʽ*/
#define AT_WIFI_MODE_PA_NOPA            0x02                /*WIFIͬʱ֧��PAģʽ��NO PAģʽ*/

#define RX_PACKET_SIZE                  1000                /*װ��ÿ�η�����*/

#define WLAN_CHANNEL_2G_MIN             1                   /*2.4G�ŵ���Сֵ*/
#define WLAN_CHANNEL_5G_MIN             36                  /*5G�ŵ���Сֵ*/
#define WLAN_CHANNEL_2G_MAX             14                  /*2.4G�ŵ����*/
#define WLAN_CHANNEL_2G_MIDDLE          6
#define WLAN_CHANNEL_5G_MAX             165                 /*5G�ŵ����*/

#define WLAN_CHANNEL_5G_W52_START       36
#define WLAN_CHANNEL_5G_W52_END         48
#define WLAN_CHANNEL_5G_W53_START       52
#define WLAN_CHANNEL_5G_W53_END         64
#define WLAN_CHANNEL_5G_W57_START       149
#define WLAN_CHANNEL_5G_W57_END         161

#define WLAN_CHANNEL_5G_INTERVAL        4                     /*5G�ŵ����*/
#define WLAN_CHANNEL_5G_40M_INTERVAL    8                     /*5G 40M�ŵ����*/

#define WLAN_FREQ_2G_MAX                2484                  /*2.4G���Ƶ��*/

#define WLAN_FREQ_5G_W52_MIN            5180                  /*W52��СƵ��*/
#define WLAN_FREQ_5G_W53_MAX            5320                  /*W53���Ƶ��*/

#define WLAN_FREQ_5G_W52_40M_MIN        5190                  /*W52 40M��СƵ��*/
#define WLAN_FREQ_5G_W53_40M_MAX        5310                  /*W53 40M���Ƶ��*/

#define WLAN_FREQ_5G_W56_MIN            5500                  /*W56��СƵ��*/
#define WLAN_FREQ_5G_W56_MAX            5700                  /*W56���Ƶ��*/

#define WLAN_FREQ_5G_W56_40M_MIN        5510                  /*W56 40M��СƵ��*/
#define WLAN_FREQ_5G_W56_40M_MAX        5670                  /*W56 40M���Ƶ��*/

#define WLAN_FREQ_5G_W57_MIN            5745                  /*W57��СƵ��*/
#define WLAN_FREQ_5G_W57_MAX            5825                  /*W57���Ƶ��*/

#define WLAN_FREQ_5G_W57_40M_MIN        5755                  /*W57��СƵ��*/
#define WLAN_FREQ_5G_W57_40M_MAX        5795                  /*W57���Ƶ��*/

#define WIFI_CMD_MAX_SIZE               256                   /*cmd�ַ���256����*/
#define WIFI_CMD_8_SIZE                 8                     /*cmd�ַ���8����*/
#define HUNDRED                         100

#define WIFI_MIMO_MODE                  4                     /*MIMO״̬ʱ��WIFI�����߹���ģʽ��4*/
#define WIFI_SISO_MODE                  3                     /*SISO״̬ʱ��WIFI�����߹���ģʽС��3*/

#define DALEY_100_TIME  100
#define DALEY_500_TIME  500
#define DALEY_1000_TIME 1000
#define DALEY_5000_TIME 5000
#define DALEY_2000_TIME 2000

/*��WiFiоƬ�·���������*/
#define WIFI_TEST_CMD(cmd) do {\
    char temp_cmd[WIFI_CMD_MAX_SIZE] = {0};\
    int cmd_ret = 0;\
    snprintf(temp_cmd, WIFI_CMD_MAX_SIZE, "/system/bin/wifi_brcm/exe/%s", cmd);\
    PLAT_WLAN_INFO("[ret=%d]%s\n", cmd_ret, temp_cmd);\
    cmd_ret = wlan_run_cmd(temp_cmd);\
    if (0 != cmd_ret)\
    {\
        PLAT_WLAN_ERR("Run CMD Error");\ 
        return AT_RETURN_FAILURE;\
    }\
    msleep(DALEY_100_TIME);\
}while(0)


/*WiFiоƬʹ�ܶ��Լ��*/
#define ASSERT_WiFi_OFF(ret)                    \
if (AT_WIENABLE_OFF == g_wlan_at_data.wifiStatus) \
{                                               \
    PLAT_WLAN_INFO("Exit on WiFi OFF\n");        \
    return ret;                                 \
}

/*WiFiȫ�ֱ����ṹ�� */
typedef struct tagWlanATGlobal
{
    WLAN_AT_WIENABLE_TYPE   wifiStatus;    /*Ĭ�ϼ��ز���ģʽ*/
    WLAN_AT_WIMODE_TYPE     wifiMode;      /*wifiЭ��ģʽ*/
    WLAN_AT_WIBAND_TYPE     wifiBand;      /*wifiЭ����ʽ*/
    WLAN_AT_WIFREQ_STRU     wifiFreq;      /*wifiƵ����Ϣ*/
    uint32                  wifiChannel;   /*wifi�ŵ���Ϣ*/
    uint32                  wifiRate;      /*wifi��������*/
    int32                   wifiPower;     /*wifi���书��*/
    WLAN_AT_FEATURE_TYPE    wifiTX;        /*wifi�����״̬*/
    WLAN_AT_WIRX_STRU       wifiRX;        /*wifi���ջ�״̬*/
    WLAN_AT_WIRPCKG_STRU    wifiPckg;      /*wifi�����*/  
    uint32                  wifiGroup;     /*wifi����ģʽ*/
}WLAN_AT_GLOBAL_ST;

typedef struct tagWlanATPacketREP
{
    unsigned int TotalRxPkts;
    unsigned int BadRxPkts;
    unsigned int UcastRxPkts;
    unsigned int McastRxPkts;
}WLAN_AT_PACK_REP_ST;

typedef struct
{
    uint brate;
    int8 rate_str[WIFI_CMD_8_SIZE];
}BRATE_ST;

/*��¼��ǰ��WiFiģʽ������Ƶ�ʣ����ʵȲ���*/
STATIC WLAN_AT_GLOBAL_ST g_wlan_at_data = {AT_WIENABLE_TEST, AT_WIMODE_80211n, AT_WIBAND_20M
         , {2412, 0}, 1, 6500, 3175, AT_FEATURE_DISABLE, {AT_FEATURE_DISABLE, {0}, {0}}, AT_WiPARANGE_HIGH};/*����WiFi��Ĭ�ϲ���*/

/*WiFi���ݰ�ͳ����Ϣ*/
STATIC WLAN_AT_PACK_REP_ST g_wifi_packet_report = {0};
STATIC WLAN_AT_PACK_REP_ST g_wifi_packet_new_rep = {0};/*ʵʱ��Ϣ*/
STATIC int g_rx = 0;

/*WiFi��Ч��У��*/
STATIC int32 check_wifi_valid()
{        
    char *pValStr = "wl ver";    
    
    WIFI_TEST_CMD(pValStr);
   
    return AT_RETURN_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////
/*(1)^WIENABLE ����WiFiģ��ʹ�� */
//////////////////////////////////////////////////////////////////////////
/*****************************************************************************
 ��������  : int32 WlanATSetWifiEnable(WLAN_AT_WIENABLE_TYPE onoff)
 ��������  : ����wifi �������ģʽ������ģʽ���ر�wifi
 �������  :  0  �ر�
              1  ������ģʽ
              2  �򿪲���ģʽ
 �������  : NA
 �� �� ֵ  : 0 �ɹ�
             1 ʧ��
 ����˵��  : 
*****************************************************************************/
STATIC int32 ATSetWifiEnable(WLAN_AT_WIENABLE_TYPE onoff)
{
    int32 ret = AT_RETURN_SUCCESS;

    ret = check_wifi_valid();
    if (AT_RETURN_SUCCESS != ret)
    {       
        PLAT_WLAN_INFO("check_wifi_valid failed!\n");
        return ret;
    }
    
    switch (onoff)
    {
        case AT_WIENABLE_OFF:
            {                
                PLAT_WLAN_INFO("Set wifi to off mode\n");
                
                WIFI_TEST_CMD("wl down");
                msleep(DALEY_2000_TIME);
                g_wlan_at_data.wifiStatus = AT_WIENABLE_OFF;
            }            
            break;
        /* ����ʱ�������ѯ��wienable����Ϊ2�����ʾ�������س�����
         * ���ǵ�һ���ϵ��쳣(�����ϲ��ᷢ��)�����µ磬�ϵ�ָ���
        */    
        case AT_WIENABLE_ON:
            {
                PLAT_WLAN_INFO("Set wifi to normal mode\n");
                
                WIFI_TEST_CMD("wifi_poweroff_43241.sh");
                WIFI_TEST_CMD("wifi_poweron_factory_43241.sh");
                WIFI_TEST_CMD("wl down");

                g_wlan_at_data.wifiStatus = AT_WIENABLE_ON;
            }
            break;
        case AT_WIENABLE_TEST:
            {                
                PLAT_WLAN_INFO("Set wifi to test mode\n");    
                
                WIFI_TEST_CMD("wifi_poweroff_43241.sh");
                WIFI_TEST_CMD("wifi_poweron_factory_43241.sh");
                WIFI_TEST_CMD("wl down");
                
                g_wlan_at_data.wifiStatus = AT_WIENABLE_TEST;
            }         
            break;
        default: 
            ret = AT_RETURN_FAILURE;
            break;
    }   
    
    return ret;
}
/*****************************************************************************
 ��������  : WLAN_AT_WIENABLE_TYPE WlanATGetWifiEnable()
 ��������  : ��ȡ��ǰ��WiFiģ��ʹ��״̬
 �������  : NA
 �������  : NA
 �� �� ֵ  : 0  �ر�
             1  ����ģʽ(����ģʽ)
             2  ����ģʽ(������ģʽ)
 ����˵��  : 
*****************************************************************************/
STATIC WLAN_AT_WIENABLE_TYPE ATGetWifiEnable(void)
{    
    int32 ret = AT_RETURN_SUCCESS;
    
    ret = check_wifi_valid();
    if (AT_RETURN_SUCCESS != ret)
    {       
        PLAT_WLAN_INFO("check_wifi_valid failed!\n");        
        return AT_WIENABLE_OFF;
    }

    WIFI_TEST_CMD("wl down");
    
    return g_wlan_at_data.wifiStatus;
}

//////////////////////////////////////////////////////////////////////////
/*(2)^WIMODE ����WiFiģʽ���� Ŀǰ��Ϊ��ģʽ����*/
//////////////////////////////////////////////////////////////////////////
/*****************************************************************************
 ��������  : int32 WlanATSetWifiMode(WLAN_AT_WIMODE_TYPE mode)
 ��������  : ����WiFi AP֧�ֵ���ʽ
 �������  : 0,  CWģʽ
             1,  802.11a��ʽ
             2,  802.11b��ʽ
             3,  802.11g��ʽ
             4,  802.11n��ʽ
             5,  802.11ac��ʽ
 �������  : NA
 �� �� ֵ  : 0 �ɹ�
             1 ʧ��
 ����˵��  : 
*****************************************************************************/
STATIC int32 ATSetWifiMode(WLAN_AT_WIMODE_TYPE mode)
{
    PLAT_WLAN_INFO("enter,mode = %d",mode);
    if (mode > AT_WIMODE_80211ac)
    {
        return (AT_RETURN_FAILURE);
    }   

    if (AT_WIMODE_80211a == mode)
    {
        WIFI_TEST_CMD("wl band a");
        WIFI_TEST_CMD("wl nmode 0");  
    }
    else if (AT_WIMODE_80211b == mode)
    {
        WIFI_TEST_CMD("wl nmode 0");
        WIFI_TEST_CMD("wl gmode 0");
        
    }
    else if (AT_WIMODE_80211g == mode)
    {
        WIFI_TEST_CMD("wl nmode 0");
        WIFI_TEST_CMD("wl gmode 2");        
    }
    else
    {
        WIFI_TEST_CMD("wl nmode 1");
        WIFI_TEST_CMD("wl gmode 1");        
    }
    
    g_wlan_at_data.wifiMode = mode;
    return (AT_RETURN_SUCCESS);
}

/*****************************************************************************
 ��������  : int32 WlanATGetWifiMode(WLAN_AT_BUFFER_STRU *strBuf)
 ��������  : ��ȡ��ǰWiFi֧�ֵ���ʽ
             ��ǰģʽ�����ַ�����ʽ����eg: 2
 �������  : NA
 �������  : NA
 �� �� ֵ  : NA
 ����˵��  : 
*****************************************************************************/
STATIC int32 ATGetWifiMode(WLAN_AT_BUFFER_STRU *strBuf)
{
    if (NULL == strBuf)
    {
        return (AT_RETURN_FAILURE);
    }

    OSA_SNPRINTF(strBuf->buf, WLAN_AT_BUFFER_SIZE, "%d", g_wlan_at_data.wifiMode);
    return (AT_RETURN_SUCCESS);
}

/*****************************************************************************
 ��������  : int32 WlanATGetWifiModeSupport(WLAN_AT_BUFFER_STRU *strBuf)
 ��������  : ��ȡWiFiоƬ֧�ֵ�����Э��ģʽ
             ֧�ֵ�����ģʽ�����ַ�����ʽ����eg: 2,3,4
 �������  : NA
 �������  : NA
 �� �� ֵ  : 0 �ɹ�
             1 ʧ��
 ����˵��  : 
*****************************************************************************/
STATIC int32 ATGetWifiModeSupport(WLAN_AT_BUFFER_STRU *strBuf)
{
    if (NULL == strBuf)
    {
        return (AT_RETURN_FAILURE);
    }
    /*begin add by yangzhiyong for E5573S-607 antenna*/
    uint32  *pu32HwId = (u32 *)SHM_MEM_HW_VER_ADDR;
    printk("the hardid is %x",*pu32HwId);

    if(HW_VER_PRODUCT_E5573S_607  ==  *pu32HwId)
    {
        OSA_SNPRINTF(strBuf->buf, WLAN_AT_BUFFER_SIZE, "%s", "2,3,4");
        return (AT_RETURN_SUCCESS);
    }
    /*end add by yangzhiyong for E5573S-607 antenna*/
    OSA_SNPRINTF(strBuf->buf, WLAN_AT_BUFFER_SIZE, "%s", WLAN_AT_MODE_SUPPORT);
    return (AT_RETURN_SUCCESS);
}

//////////////////////////////////////////////////////////////////////////
/*(3)^WIBAND ����WiFi������� */
//////////////////////////////////////////////////////////////////////////
/*****************************************************************************
 ��������  : int32 WlanATSetWifiBand(WLAN_AT_WIBAND_TYPE width)
 ��������  : ��������wifi����
 �������  : 0 20M
             1 40M
 �������  : NA
 �� �� ֵ  : 0 �ɹ�
             1 ʧ��
 ����˵��  : ֻ����nģʽ�²ſ�������40M����
*****************************************************************************/
STATIC int32 ATSetWifiBand(WLAN_AT_WIBAND_TYPE band)
{
    int32 ret = AT_RETURN_SUCCESS;
    
    PLAT_WLAN_INFO("enter,band = %d\n",band);
    
    switch(band)
    {
        case AT_WIBAND_20M: 
            {
                g_wlan_at_data.wifiBand = AT_WIBAND_20M;
                break;
            }         
        case AT_WIBAND_40M:
            {                
                if(AT_WIMODE_80211n == g_wlan_at_data.wifiMode)
                {
                    g_wlan_at_data.wifiBand = AT_WIBAND_40M;                    
                }
                else
                {   
                    PLAT_WLAN_ERR("Error wifi mode,must in n mode");
                    ret = AT_RETURN_FAILURE;
                }
                break;
            }             
        default:
            ret = AT_RETURN_FAILURE;
            break;
    }
     
    return ret;
}

/*****************************************************************************
 ��������  : int32 WlanATGetWifiBand(WLAN_AT_BUFFER_STRU *strBuf)
 ��������  :��ȡ��ǰ�������� 
            ��ǰ�������ַ�����ʽ����eg: 0
 �������  : NA
 �������  : NA
 �� �� ֵ  : 0 �ɹ�
             1 ʧ��
 ����˵��  : 
*****************************************************************************/
STATIC int32 ATGetWifiBand(WLAN_AT_BUFFER_STRU *strBuf)
{
    if (NULL == strBuf)
    {
        return (AT_RETURN_FAILURE);
    }

    OSA_SNPRINTF(strBuf->buf, WLAN_AT_BUFFER_SIZE, "%d", g_wlan_at_data.wifiBand);
    return (AT_RETURN_SUCCESS);
}

/*****************************************************************************
 ��������  : int32 WlanATGetWifiBandSupport(WLAN_AT_BUFFER_STRU *strBuf)
 ��������  :��ȡWiFi֧�ֵĴ������� 
            ֧�ִ������ַ�����ʽ����eg: 0,1
 �������  : NA
 �������  : NA
 �� �� ֵ  : 0 �ɹ�
             1 ʧ��
 ����˵��  : 
*****************************************************************************/
STATIC int32 ATGetWifiBandSupport(WLAN_AT_BUFFER_STRU *strBuf)
{
    if (NULL == strBuf)
    {
        return (AT_RETURN_FAILURE);
    }

    OSA_SNPRINTF(strBuf->buf, WLAN_AT_BUFFER_SIZE, "%s", WLAN_AT_BAND_SUPPORT);
    return (AT_RETURN_SUCCESS);
}

//////////////////////////////////////////////////////////////////////////
/*(4)^WIFREQ ����WiFiƵ�� */
//////////////////////////////////////////////////////////////////////////
/*****************************************************************************
 ��������  : int32 WlanATSetWifiFreq(WLAN_AT_WIFREQ_STRU *pFreq)
 ��������  : ����WiFiƵ��
 �������  : NA
 �������  : NA
 �� �� ֵ  : 0 �ɹ�
             1 ʧ��
 ����˵��  : 
*****************************************************************************/
STATIC int32 ATSetWifiFreq(WLAN_AT_WIFREQ_STRU *pFreq)
{
    /*2.4GƵ�㼯��*/
    const uint16   ausChannels[] = {2412,2417,2422,2427,2432,2437,2442,2447,2452,2457,2462,2467,2472,2484};/*2.4G*/
    /*5GƵ�㼯��*/
    const uint16 aulChannel036[] = {5180, 5200, 5220, 5240, 5260, 5280, 5300, 5320};/*w52��w53*/  
    const uint16 aulChannel100[] = {5500,5520,5540,5560,5580,5600,5620,5640,5660,5680,5700};/*w56*/
    const uint16 aulChannel149[] = {5745,5765,5785,5805,5825};/*w57*/
  
    /*2.4G 40MƵ�㼯��*/
    const uint16   ausChannels_40M[] = {2422, 2427, 2432, 2437, 2442, 2447, 2452, 2457, 2462};/*2.4G 40M*/
    /*5G 40MƵ�㼯��*/
    const uint16 aulChannel036_40M[] = {5190, 5230, 5270, 5310};/*5G 40M*/
    const uint16 aulChannel100_40M[] = {5510, 5550, 5590, 5630, 5670};/*5G 40M*/
    const uint16 aulChannel149_40M[] = {5755, 5795};/*5G 40M*/
   
    /*5G 40M������Ҫ��u���ŵ�*/
    const uint16 channel_5g_40M_u[] = {40,48,56,64,104,112,120,128,136,153,161};    
    /*5G 40M������Ҫ��l���ŵ�*/
    const uint16 channel_5g_40M_l[] = {36,44,52,60,100,108,116,124,132,149,157};
    
    int8 acCmd[WIFI_CMD_MAX_SIZE] = {0};
    int8 channeStr[WIFI_CMD_8_SIZE] = {0}; 
    uint16 ulWifiFreq = 0;
    uint16 i = 0;    
    int32 ret = AT_RETURN_SUCCESS;
    
    if (NULL == pFreq)
    {
        return (AT_RETURN_FAILURE);
    }

    /*20M����*/
    if (AT_WIBAND_20M == g_wlan_at_data.wifiBand)
    {  
        if (pFreq->value <= WLAN_FREQ_2G_MAX)
        {
            for (i = 0; i < (sizeof(ausChannels) / sizeof(uint16)); i++)
            {
                if (pFreq->value == ausChannels[i])
                {
                    ulWifiFreq = (i + 1);
                    break;
                }
            }
        }
        /*WIFI 5G �ŵ��㷨���£�Ƶ�����������ŵ��ţ������Ӧ��Ӧ��Ϊ

        case 36:
            iWIFIchannel = 5180;
            break;
        case 40: 
            iWIFIchannel = 5200;
            break;
        case 44:    
            iWIFIchannel = 5220;
            break;
        case 48: 
            iWIFIchannel = 5240;
            break;
        case 52:    
            iWIFIchannel = 5260;
            break;
        case 56: 
            iWIFIchannel = 5280;
            break;
        case 60:    
            iWIFIchannel = 5300;
            break;
        case 64: 
            iWIFIchannel = 5320;
            break;
        */
        else if ((pFreq->value >= WLAN_FREQ_5G_W52_MIN) && (pFreq->value <= WLAN_FREQ_5G_W53_MAX))
        {
            for (i = 0; i < (sizeof(aulChannel036) / sizeof(uint16)); i++)
            {
                if (pFreq->value == aulChannel036[i])
                {
                    ulWifiFreq = (i * WLAN_CHANNEL_5G_INTERVAL + WLAN_CHANNEL_5G_MIN);
                    break;
                }
            }
        }
        /*WIFI 5G �ŵ��㷨���£�Ƶ�����������ŵ��ţ������Ӧ��Ӧ��Ϊ

        case 100:
            iWIFIchannel = 5500;
            break;
        case 104: 
            iWIFIchannel = 5520;
            break;
        case 108:
            iWIFIchannel = 5540;
            break;
        case 112: 
            iWIFIchannel = 5560;
            break;
        case 116:
            iWIFIchannel = 5580;
            break;
        case 120: 
            iWIFIchannel = 5600;
            break;
        case 124:
            iWIFIchannel = 5620;
            break;
        case 128: 
            iWIFIchannel = 5640;
            break;
        case 132: 
            iWIFIchannel = 5660;
            break;
        case 136:
            iWIFIchannel = 5680;
            break;
        case 140: 
            iWIFIchannel = 5700;
            break;
        */
        else if ((pFreq->value >= WLAN_FREQ_5G_W56_MIN) && (pFreq->value <= WLAN_FREQ_5G_W56_MAX))
        {
            for (i = 0; i < (sizeof(aulChannel100) / sizeof(uint16)); i++)
            {
                if (pFreq->value == aulChannel100[i])
                {
                    ulWifiFreq = (i * WLAN_CHANNEL_5G_INTERVAL + HUNDRED);
                    break;
                }
            }

        }
        /*WIFI 5G �ŵ��㷨���£�Ƶ�����������ŵ��ţ������Ӧ��Ӧ��Ϊ
        case 149: 
            iWIFIchannel = 5745;
            break;
        case 153:
            iWIFIchannel = 5765;
            break;
        case 157: 
            iWIFIchannel = 5785;
            break;
        case 161: 
            iWIFIchannel = 5805;
            break;
        case 165:
            iWIFIchannel = 5825;
            break;
        */
        else if ((pFreq->value >= WLAN_FREQ_5G_W57_MIN) && (pFreq->value <= WLAN_FREQ_5G_W57_MAX))
        {
            for (i = 0; i < (sizeof(aulChannel149) / sizeof(uint16)); i++)
            {
                if (pFreq->value == aulChannel149[i])
                {
                    ulWifiFreq = (i * WLAN_CHANNEL_5G_INTERVAL + WLAN_CHANNEL_5G_W57_START);
                    break;
                }
            }
        }
        else
        {        
            PLAT_WLAN_INFO("Error 20M wifiFreq parameters\n");      
            return AT_RETURN_FAILURE;
        }

        PLAT_WLAN_INFO("Target Channel = %d\n", ulWifiFreq);
    
        if (!(((WLAN_CHANNEL_2G_MIN <= ulWifiFreq) && (WLAN_CHANNEL_2G_MAX >= ulWifiFreq))
            || ((WLAN_CHANNEL_5G_MIN <= ulWifiFreq) && (WLAN_CHANNEL_5G_MAX >= ulWifiFreq))))
        {
            PLAT_WLAN_INFO("Target Channel ERROR,ulWifiFreq = %u!\n", ulWifiFreq);
            return AT_RETURN_FAILURE;
        }

        if(1 == g_rx)
        {
            WIFI_TEST_CMD("wl down");
        }

        WIFI_TEST_CMD("wl phy_watchdog 0");
        WIFI_TEST_CMD("wl mpc 0");
        WIFI_TEST_CMD("wl country ALL");
        WIFI_TEST_CMD("wl scansuppress 1");
        WIFI_TEST_CMD("wl mimo_bw_cap 0");
        if(ulWifiFreq <= WLAN_CHANNEL_2G_MAX)
        {
            WIFI_TEST_CMD("wl band b");    
        }
        else
        {
            WIFI_TEST_CMD("wl band a");
        }
        OSA_SNPRINTF(acCmd, sizeof(acCmd), "wl channel %d", ulWifiFreq);       
        WIFI_TEST_CMD(acCmd);  
        msleep(DALEY_100_TIME*3); /* ��ʱ300ms */
    }
    else if(AT_WIBAND_40M == g_wlan_at_data.wifiBand)
    {
        if (pFreq->value <= WLAN_FREQ_2G_MAX)
        {
            for (i = 0; i < (sizeof(ausChannels_40M) / sizeof(uint16)); i++)
            {
                if (pFreq->value == ausChannels_40M[i])
                {
                    ulWifiFreq = (i + 1);
                    break;
                }
            }
        }       
        else if ((pFreq->value >= WLAN_FREQ_5G_W52_40M_MIN) && (pFreq->value <= WLAN_FREQ_5G_W53_40M_MAX))
        {
            for (i = 0; i < (sizeof(aulChannel036_40M) / sizeof(uint16)); i++)
            {
                if (pFreq->value == aulChannel036_40M[i])
                {
                    ulWifiFreq = (i * WLAN_CHANNEL_5G_40M_INTERVAL + WLAN_CHANNEL_5G_MIN);
                    break;
                }
            }
        }        
        else if ((pFreq->value >= WLAN_FREQ_5G_W56_40M_MIN) && (pFreq->value <= WLAN_FREQ_5G_W56_40M_MAX))
        {
            for (i = 0; i < (sizeof(aulChannel100_40M) / sizeof(uint16)); i++)
            {
                if (pFreq->value == aulChannel100_40M[i])
                {
                    ulWifiFreq = (i * WLAN_CHANNEL_5G_40M_INTERVAL + HUNDRED);
                    break;
                }
            }

        }        
        else if ((pFreq->value >= WLAN_FREQ_5G_W57_40M_MIN) && (pFreq->value <= WLAN_FREQ_5G_W57_40M_MAX))
        {
            for (i = 0; i < (sizeof(aulChannel149_40M) / sizeof(uint16)); i++)
            {
                if (pFreq->value == aulChannel149_40M[i])
                {
                    ulWifiFreq = (i * WLAN_CHANNEL_5G_40M_INTERVAL + WLAN_CHANNEL_5G_W57_START);
                    break;
                }
            }
        }
        else
        {        
            PLAT_WLAN_INFO("Error 40M wifiFreq parameters\n");      
            return AT_RETURN_FAILURE;
        }

        PLAT_WLAN_INFO("Target Channel = %d\n", ulWifiFreq);
    
        if (!(((WLAN_CHANNEL_2G_MIN <= ulWifiFreq) && (WLAN_CHANNEL_2G_MAX >= ulWifiFreq))
            || ((WLAN_CHANNEL_5G_MIN <= ulWifiFreq) && (WLAN_CHANNEL_5G_MAX >= ulWifiFreq))))
        {
            PLAT_WLAN_INFO("Target Channel ERROR!\n");
            return AT_RETURN_FAILURE;
        }

        /*40M�ŵ���U/L����*/
        if ((ulWifiFreq > 0) 
                && (ulWifiFreq <= WLAN_CHANNEL_2G_MIDDLE))
        {
            OSA_SNPRINTF(channeStr, sizeof(channeStr), "%dl", ulWifiFreq);             
        }
        else if ((ulWifiFreq > WLAN_CHANNEL_2G_MIDDLE)
                && (ulWifiFreq <= WLAN_CHANNEL_2G_MAX))
        {
            OSA_SNPRINTF(channeStr, sizeof(channeStr), "%dl", ulWifiFreq);
        }
        else if ((ulWifiFreq >= WLAN_CHANNEL_5G_MIN)
                && (ulWifiFreq <= WLAN_CHANNEL_5G_MAX))
        {            
            for (i = 0;i < (sizeof(channel_5g_40M_l) / sizeof(uint16));i++)
            {
                if(ulWifiFreq == channel_5g_40M_l[i])
                {
                    OSA_SNPRINTF(channeStr, sizeof(channeStr), "%dl", ulWifiFreq);                     
                    break;
                }                
            }

            if(i == (sizeof(channel_5g_40M_l) / sizeof(uint16)))
            {
                for (i = 0;i < (sizeof(channel_5g_40M_u) / sizeof(uint16));i++)
                {
                    if(ulWifiFreq == channel_5g_40M_u[i])
                    {
                        OSA_SNPRINTF(channeStr, sizeof(channeStr), "%du", ulWifiFreq);
                        break;
                    }                 
                } 
            }
            
            if(i == (sizeof(channel_5g_40M_u) / sizeof(uint16)))
            {
                OSA_SNPRINTF(channeStr, sizeof(channeStr), "%d", ulWifiFreq); 
            }
        }
        else
        {
            PLAT_WLAN_INFO("40M Channel Process ERROR!\n");
            return AT_RETURN_FAILURE;
        }       

        if ( 1 == g_rx)
        {
            WIFI_TEST_CMD("wl down");
        }
        WIFI_TEST_CMD("wl mpc 0");
        WIFI_TEST_CMD("wl country ALL");
        WIFI_TEST_CMD("wl scansuppress 1");
        WIFI_TEST_CMD("wl mimo_bw_cap 1");
        WIFI_TEST_CMD("wl mimo_txbw 4");

        if(ulWifiFreq <= WLAN_CHANNEL_2G_MAX)
        {
            WIFI_TEST_CMD("wl band b");    
        }
        else
        {
            WIFI_TEST_CMD("wl band a");
        }    
        OSA_SNPRINTF(acCmd, sizeof(acCmd), "wl chanspec %s", channeStr);        
        PLAT_WLAN_INFO("Ready to execute command string:%s\n", acCmd);
        WIFI_TEST_CMD(acCmd);
        WIFI_TEST_CMD("wl status");
    }

    /* ����ȫ�ֱ�����Ա���ѯ */
    g_wlan_at_data.wifiFreq.value = pFreq->value;
    g_wlan_at_data.wifiFreq.offset = pFreq->offset;
    g_wlan_at_data.wifiChannel = ulWifiFreq;
    return ret;
}

/*****************************************************************************
 ��������  : int32 WlanATGetWifiFreq(WLAN_AT_WIFREQ_STRU *pFreq)
 ��������  : ��ȡWiFiƵ��
 �������  : NA
 �������  : NA
 �� �� ֵ  : 0 �ɹ�
             1 ʧ��
 ����˵��  : 
*****************************************************************************/
STATIC int32 ATGetWifiFreq(WLAN_AT_WIFREQ_STRU *pFreq)
{   
    if (NULL == pFreq)
    {
        return (AT_RETURN_FAILURE);
    }    
    
    memcpy(pFreq, &(g_wlan_at_data.wifiFreq), sizeof(WLAN_AT_WIFREQ_STRU));
      
    return (AT_RETURN_SUCCESS);
}

//////////////////////////////////////////////////////////////////////////
/*(5)^WIDATARATE ���úͲ�ѯ��ǰWiFi���ʼ�����
  WiFi���ʣ���λΪ0.01Mb/s��ȡֵ��ΧΪ0��65535 */
//////////////////////////////////////////////////////////////////////////
/*****************************************************************************
 ��������  : int32 WlanATSetWifiDataRate(uint32 rate)
 ��������  : ����WiFi��������
 �������  : NA
 �������  : NA
 �� �� ֵ  : 0 �ɹ�
             1 ʧ��
 ����˵��  : 
*****************************************************************************/
STATIC int32 ATSetWifiDataRate(uint32 rate)
{    
    int8    acCmd[WIFI_CMD_MAX_SIZE] = {0};
    uint32  ulWifiRate = rate / HUNDRED;
    uint32  ulNRate = 0;  
    
    const BRATE_ST wifi_brates_table[] = {{100, "1"}, {200, "2"}, {550, "5.5"}, {1100, "11"}};//b 
    const uint32 wifi_20m_nrates_table[] = {650, 1300, 1950, 2600, 3900, 5200, 5850, 6500};//n ht20
    const uint32 wifi_40m_nrates_table[] = {1350, 2700, 4050, 5400, 8100, 10800, 12150, 13500};//n ht40 
    const uint32 wifi_20m_nrates_table_mimo[] = {1300, 2600, 3900, 5200, 7800, 10400, 11700, 13000};//n mimo ht20
    const uint32 wifi_40m_nrates_table_mimo[] = {2700, 5400, 8100, 10800, 16200, 21600, 24300, 27000};//n mimo ht40}

    #define WIFI_BRATES_TABLE_SIZE (sizeof(wifi_brates_table) / sizeof(BRATE_ST))
    #define WIFI_20M_NRATES_TABLE_SIZE (sizeof(wifi_20m_nrates_table) / sizeof(uint32))
    #define WIFI_40M_NRATES_TABLE_SIZE (sizeof(wifi_40m_nrates_table) / sizeof(uint32))
    #define WIFI_20M_NRATES_TABLE_MIMO_SIZE (sizeof(wifi_20m_nrates_table_mimo) / sizeof(uint32))
    #define WIFI_40M_NRATES_TABLE_MIMO_SIZE (sizeof(wifi_40m_nrates_table_mimo) / sizeof(uint32))

    
    PLAT_WLAN_INFO("WifiRate = %u\n", ulWifiRate);
    
    switch (g_wlan_at_data.wifiMode)
    {
        case AT_WIMODE_CW:
            PLAT_WLAN_INFO("AT_WIMODE_CW\n");
            return (AT_RETURN_FAILURE);            
        case AT_WIMODE_80211a:
            OSA_SNPRINTF(acCmd, sizeof(acCmd), "wl nrate -r %u", ulWifiRate);
            break;           
        case AT_WIMODE_80211b:
            for (ulNRate = 0; ulNRate < WIFI_BRATES_TABLE_SIZE; ulNRate++)
            {
                if (wifi_brates_table[ulNRate].brate == rate)
                {
                    PLAT_WLAN_INFO("bRate Index = %u\n", ulNRate);                    
                    PLAT_WLAN_INFO("bRate Str = %s\n", wifi_brates_table[ulNRate].rate_str);
                    break;
                }                
            }

            if (WIFI_BRATES_TABLE_SIZE == ulNRate)
            {
                WLAN_TRACE_ERROR("bRate Error!\n");
                return (AT_RETURN_FAILURE);
            }
            OSA_SNPRINTF(acCmd, sizeof(acCmd), "wl bg_rate %s", wifi_brates_table[ulNRate].rate_str);
            break;
        case AT_WIMODE_80211g:  
            OSA_SNPRINTF(acCmd, sizeof(acCmd), "wl bg_rate %u", ulWifiRate);
            break;
        case AT_WIMODE_80211n:
            if(g_wlan_at_data.wifiGroup <= WIFI_SISO_MODE)
            {
                if (AT_WIBAND_20M == g_wlan_at_data.wifiBand)
                {
                    /* WIFI 20M nģʽWL���������ֵΪ0~7����8�� */
                    for (ulNRate = 0; ulNRate < WIFI_20M_NRATES_TABLE_SIZE; ulNRate++)
                    {
                        if (wifi_20m_nrates_table[ulNRate] == rate)
                        {
                            PLAT_WLAN_INFO("20M NRate Index = %u\n", ulNRate);                        
                            break;
                        }
                    }

                    if (WIFI_20M_NRATES_TABLE_SIZE == ulNRate)
                    {
                        WLAN_TRACE_ERROR("20M NRate Error!\n");
                        return (AT_RETURN_FAILURE);
                    }
                }
                else if (AT_WIBAND_40M == g_wlan_at_data.wifiBand)
                {
                    for (ulNRate = 0; ulNRate < WIFI_40M_NRATES_TABLE_SIZE; ulNRate++)
                    {
                        if (wifi_40m_nrates_table[ulNRate] == rate)
                        {
                            PLAT_WLAN_INFO("40M NRate Index = %u\n", ulNRate);
                            break;
                        }
                    }

                    if (WIFI_40M_NRATES_TABLE_SIZE == ulNRate)
                    {
                        WLAN_TRACE_ERROR("40M NRate Error!\n");
                        return (AT_RETURN_FAILURE);
                    }          
                }            
                OSA_SNPRINTF(acCmd, sizeof(acCmd), "wl nrate -m %u -s 0", ulNRate);           
            }
            else if(g_wlan_at_data.wifiGroup == WIFI_MIMO_MODE)
            {
                if (AT_WIBAND_20M == g_wlan_at_data.wifiBand)
                {
                    /* WIFI 20M mimo nģʽWL���������ֵΪ8~15����8�� */
                    for (ulNRate = 0; ulNRate < WIFI_20M_NRATES_TABLE_MIMO_SIZE; ulNRate++)
                    {
                        if (wifi_20m_nrates_table_mimo[ulNRate] == rate)
                        {
                            PLAT_WLAN_INFO("20M Mimo NRate Index = %d\n", ulNRate + WIFI_20M_NRATES_TABLE_MIMO_SIZE);
                            break;
                        }
                    }

                    if (WIFI_20M_NRATES_TABLE_MIMO_SIZE == ulNRate)
                    {
                        WLAN_TRACE_ERROR("20M Mimo NRate Error!\n");
                        return (AT_RETURN_FAILURE);
                    }
                    ulNRate += WIFI_20M_NRATES_TABLE_MIMO_SIZE;
                }
                else if (AT_WIBAND_40M == g_wlan_at_data.wifiBand)
                {
                    for (ulNRate = 0; ulNRate < WIFI_40M_NRATES_TABLE_MIMO_SIZE; ulNRate++)
                    {
                        if (wifi_40m_nrates_table_mimo[ulNRate] == rate)
                        {
                            PLAT_WLAN_INFO("40M Mimo NRate Index = %d\n", ulNRate + WIFI_40M_NRATES_TABLE_MIMO_SIZE);
                            break;
                        }
                    }

                    if (WIFI_40M_NRATES_TABLE_MIMO_SIZE == ulNRate)
                    {
                        WLAN_TRACE_ERROR("40M Mimo NRate Error!\n");
                        return (AT_RETURN_FAILURE);
                    }          
                    ulNRate += WIFI_40M_NRATES_TABLE_MIMO_SIZE;
                }            
                OSA_SNPRINTF(acCmd, sizeof(acCmd), "wl nrate -m %d -s 3", ulNRate);
            }
            else
            {
                WLAN_TRACE_ERROR("Group Error!\n");
            }
            break;
        default:
            return (AT_RETURN_FAILURE);            
    }    
    WIFI_TEST_CMD(acCmd);    
    /*����ȫ�ֱ�����Ա���ѯ*/
    g_wlan_at_data.wifiRate = rate;
       
    return (AT_RETURN_SUCCESS);    
}
/*****************************************************************************
 ��������  : uint32 WlanATGetWifiDataRate()
 ��������  : ��ѯ��ǰWiFi��������
 �������  : NA
 �������  : NA
 �� �� ֵ  : wifi����
 ����˵��  : 
*****************************************************************************/
STATIC uint32 ATGetWifiDataRate(void)
{
    return g_wlan_at_data.wifiRate;
}

//////////////////////////////////////////////////////////////////////////
/*(6)^WIPOW ������WiFi���书�� 
   WiFi���书�ʣ���λΪ0.01dBm��ȡֵ��ΧΪ -32768��32767 */
//////////////////////////////////////////////////////////////////////////
/*****************************************************************************
 ��������  : int32 WlanATSetWifiPOW(int32 power_dBm_percent)
 ��������  : ����WiFi���书��
 �������  : NA
 �������  : NA
 �� �� ֵ  : 0 �ɹ�
             1 ʧ��
 ����˵��  : 
*****************************************************************************/
STATIC int32 ATSetWifiPOW(int32 power_dBm_percent)
{
    int8    acCmd[WIFI_CMD_MAX_SIZE] = {0};
    int32 lWifiPower = power_dBm_percent / HUNDRED;

    if ((lWifiPower >= WLAN_AT_POWER_MIN) && (lWifiPower <= WLAN_AT_POWER_MAX))
    {
        OSA_SNPRINTF(acCmd, sizeof(acCmd), "wl txpwr1 -d -o %u", lWifiPower);
        WIFI_TEST_CMD(acCmd);

        /*����ȫ�ֱ�����Ա���ѯ*/
        g_wlan_at_data.wifiPower = power_dBm_percent;
        return (AT_RETURN_SUCCESS);
    }
    else
    {
        PLAT_WLAN_INFO("Invalid argument\n");
        return (AT_RETURN_FAILURE);
    }    
}

/*****************************************************************************
 ��������  : int32 WlanATGetWifiPOW()
 ��������  : ��ȡWiFi��ǰ���书��
 �������  : NA
 �������  : NA
 �� �� ֵ  : NA
 ����˵��  : 
*****************************************************************************/
STATIC int32 ATGetWifiPOW(void)
{
    return g_wlan_at_data.wifiPower;
}

//////////////////////////////////////////////////////////////////////////
/*(7)^WITX ������WiFi��������� */
//////////////////////////////////////////////////////////////////////////
/*****************************************************************************
 ��������  : int32 WlanATSetWifiTX(WLAN_AT_FEATURE_TYPE onoff)
 ��������  : �򿪻�ر�wifi�����
 �������  : 0 �ر�
             1 ��
 �������  : NA
 �� �� ֵ  : 0 �ɹ�
             1 ʧ��
 ����˵��  : 
*****************************************************************************/
STATIC int32 ATSetWifiTX(WLAN_AT_FEATURE_TYPE onoff)
{
    int8    tcmd[WIFI_CMD_MAX_SIZE] = {0};
    
    if (AT_WIMODE_CW == g_wlan_at_data.wifiMode)
    {
        if (AT_FEATURE_DISABLE == onoff)
        {
            WIFI_TEST_CMD("wl fqacurcy 0");
        }
        else
        {
            WIFI_TEST_CMD("wl down");
            WIFI_TEST_CMD("wl mpc 0");
            WIFI_TEST_CMD("wl mimo_bw_cap 0");
            WIFI_TEST_CMD("wl mimo_txbw -1");
            WIFI_TEST_CMD("wl txant 1");
            WIFI_TEST_CMD("wl antdiv 1");
            if(WLAN_CHANNEL_2G_MAX < g_wlan_at_data.wifiChannel)
            {
                WIFI_TEST_CMD("wl band a");
            }
            else
            {
                WIFI_TEST_CMD("wl band b");
            }
            WIFI_TEST_CMD("wl up");
            WIFI_TEST_CMD("wl phy_txpwrctrl 0");
            WIFI_TEST_CMD("wl phy_txpwrindex 0 127");
            WIFI_TEST_CMD("wl phy_txpwrctrl 1");
            WIFI_TEST_CMD("wl out");
            OSA_SNPRINTF(tcmd, sizeof(tcmd), "wl fqacurcy %d", g_wlan_at_data.wifiChannel);
            WIFI_TEST_CMD(tcmd);
        }
    }
    else 
    {
        if(AT_FEATURE_DISABLE == onoff)
        {
            WIFI_TEST_CMD("wl pkteng_stop tx");
            WIFI_TEST_CMD("wl down");
        }
        else
        {
            /* ���õ���ӿ� */
            WIFI_TEST_CMD("wl mpc 0");
            WIFI_TEST_CMD("wl country ALL");
            WIFI_TEST_CMD("wl frameburst 1");
            WIFI_TEST_CMD("wl scansuppress 1");
            WIFI_TEST_CMD("wl up");
            msleep(DALEY_100_TIME*4); /* ��ʱ400ms */

            WIFI_TEST_CMD("wl pkteng_start 00:11:22:33:44:55 tx 100 1500 0"); 
            msleep(DALEY_100_TIME*4); /* ��ʱ400ms */
        } 
    }
    
    /*����ȫ�ֱ�����ѱ���ѯ*/
    g_wlan_at_data.wifiTX = onoff;
     
    return (AT_RETURN_SUCCESS);
}
/*****************************************************************************
 ��������  : WLAN_AT_FEATURE_TYPE WlanATGetWifiTX()
 ��������  : ��ѯ��ǰWiFi�����״̬��Ϣ
 �������  : NA
 �������  : NA
 �� �� ֵ  : 0 �رշ����
             1 �򿪷����
 ����˵��  : 
*****************************************************************************/
STATIC WLAN_AT_FEATURE_TYPE ATGetWifiTX(void)
{
    return g_wlan_at_data.wifiTX;
}

//////////////////////////////////////////////////////////////////////////
/*(8)^WIRX ����WiFi���ջ����� */
//////////////////////////////////////////////////////////////////////////
STATIC int32 ATSetWifiRX(WLAN_AT_WIRX_STRU *params)
{
    int8    acCmd[WIFI_CMD_MAX_SIZE] = {0};
    if (NULL == params)
    {
        return (AT_RETURN_FAILURE);
    }
    

    PLAT_WLAN_INFO("WlanATSetWifiRX in, g_rx = %d\n", g_rx);
    if (WLAN_AT_GROUP_MAX == g_wlan_at_data.wifiGroup)
    {  
        g_rx = 0; // 0 ��ʾ��ǰTxģʽ����
    }
    else
    {
        g_rx = 1; // 1 ��ʾ��ǰrxģʽ����
    }    
    PLAT_WLAN_INFO("WlanATSetWifiRX, g_rx = %d\n", g_rx);
     
    switch (params->onoff)
    {
        case AT_FEATURE_DISABLE:
            WIFI_TEST_CMD("wl pkteng_stop rx");
            break;
            
        case AT_FEATURE_ENABLE:
            PLAT_WLAN_INFO("src mac is %s\n",params->src_mac);
            WIFI_TEST_CMD("wl up");
            OSA_SNPRINTF(acCmd, sizeof(acCmd), "wl pkteng_start %s rx", params->src_mac);
            WIFI_TEST_CMD(acCmd);
            WIFI_TEST_CMD("wl counters"); 
            memcpy(&g_wifi_packet_report, &g_wifi_packet_new_rep, sizeof(g_wifi_packet_report));
            break;
            
        default:
            return (AT_RETURN_FAILURE);
    }
    
    memcpy(&g_wlan_at_data.wifiRX, params, sizeof(WLAN_AT_WIRX_STRU));
    
    return (AT_RETURN_SUCCESS);
}

/*****************************************************************************
 ��������  : int32 WlanATGetWifiRX(WLAN_AT_WIRX_STRU *params)
 ��������  : ��ȡwifi���ջ���״̬
 �������  : NA
 �������  : NA
 �� �� ֵ  : NA
 ����˵��  : 
*****************************************************************************/
STATIC int32 ATGetWifiRX(WLAN_AT_WIRX_STRU *params)
{
    if (NULL == params)
    {
        return (AT_RETURN_FAILURE);
    }    
    
    memcpy(params, &g_wlan_at_data.wifiRX, sizeof(WLAN_AT_WIRX_STRU));
    
    return (AT_RETURN_SUCCESS);
}

//////////////////////////////////////////////////////////////////////////
/*(9)^WIRPCKG ��ѯWiFi���ջ�����룬�ϱ����յ��İ�������*/
//////////////////////////////////////////////////////////////////////////
/*****************************************************************************
 ��������  : int32 WlanATSetWifiRPCKG(int32 flag)
 ��������  : ���Wifi����ͳ�ư�Ϊ��
 �������  : 0 ���wifiͳ�ư�
             ��0 ��Ч����
 �������  : NA
 �� �� ֵ  : 0 �ɹ�
             1 ʧ��
 ����˵��  : 
*****************************************************************************/
STATIC int32 ATSetWifiRPCKG(int32 flag)
{
    ASSERT_WiFi_OFF(AT_RETURN_FAILURE);

    if (0 != flag)
    {
        PLAT_WLAN_INFO("Exit on flag = %d\n", flag);
        return (AT_RETURN_FAILURE);
    }

    WIFI_TEST_CMD("wl counters");
    memcpy(&g_wifi_packet_report, &g_wifi_packet_new_rep, sizeof(g_wifi_packet_report));
    
    return (AT_RETURN_SUCCESS);
}

/*****************************************************************************
 ��������  : wlan_at_get_packet_report
 ��������  : ��ȡwifi���ջ����հ�����
 �������  : char * pValueStr
 �������  : NA             
 �� �� ֵ  : uiRetPcktsNumBuf
 ����˵��  : 
*****************************************************************************/
void wlan_at_get_packet_report(unsigned int total, unsigned int bad, unsigned int ucast, unsigned int mcast)
{
    PLAT_WLAN_INFO("Enter [Total=%d,Bad=%d,Ucast=%d,Mcast=%d]\n", total, bad, ucast, mcast);
    g_wifi_packet_new_rep.TotalRxPkts = total;
    g_wifi_packet_new_rep.BadRxPkts = bad;
    g_wifi_packet_new_rep.UcastRxPkts = ucast;
    g_wifi_packet_new_rep.McastRxPkts = mcast;
}

/*****************************************************************************
 ��������  : int32 WlanATSetWifiRPCKG(int32 flag)
 ��������  : ��ѯWiFi���ջ�����룬�ϱ����յ��İ�������
 �������  : WLAN_AT_WIRPCKG_STRU *params
 �������  : uint16 good_result; //������յ��ĺð�����ȡֵ��ΧΪ0~65535
             uint16 bad_result;  //������յ��Ļ�������ȡֵ��ΧΪ0~65535
 �� �� ֵ  : NA
 ����˵��  : 
*****************************************************************************/
STATIC int32 ATGetWifiRPCKG(WLAN_AT_WIRPCKG_STRU *params)
{
    int32 ret = AT_RETURN_SUCCESS; 
    if (NULL == params)
    {        
        WLAN_TRACE_ERROR("%s:POINTER_NULL!\n", __FUNCTION__);
        ret = AT_RETURN_FAILURE;
        return ret;
    }
    /* �жϽ��ջ��Ƿ�� */
    if(AT_FEATURE_DISABLE == g_wlan_at_data.wifiRX.onoff)
    {
        WLAN_TRACE_ERROR("%s:Not Rx Mode.\n", __FUNCTION__);
        ret = AT_RETURN_FAILURE;
        return ret;
    }   

    WIFI_TEST_CMD("wl counters");
    PLAT_WLAN_INFO("Enter [old = %d, new = %d]\n", g_wifi_packet_report.UcastRxPkts, g_wifi_packet_new_rep.UcastRxPkts);      
    
    params->good_result = (uint16)(g_wifi_packet_new_rep.UcastRxPkts - g_wifi_packet_report.UcastRxPkts);
    params->bad_result = 0;   
    
    PLAT_WLAN_INFO("Exit [good = %d, bad = %d]\n", params->good_result, params->bad_result);   
    
    return (AT_RETURN_SUCCESS);
}

//////////////////////////////////////////////////////////////////////////
/*(10)^WIINFO ��ѯWiFi�������Ϣ*/
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
/*(11)^WIPLATFORM ��ѯWiFi����ƽ̨��Ӧ����Ϣ */
//////////////////////////////////////////////////////////////////////////
/*****************************************************************************
 ��������  : WLAN_AT_WIPLATFORM_TYPE WlanATGetWifiPlatform()
 ��������  : ��ѯWiFi����ƽ̨��Ӧ����Ϣ
 �������  : NA
 �������  : NA
 �� �� ֵ  : NA
 ����˵��  : 
*****************************************************************************/
STATIC WLAN_AT_WIPLATFORM_TYPE ATGetWifiPlatform(void)
{
    return (AT_WIPLATFORM_BROADCOM);
}

//////////////////////////////////////////////////////////////////////////
/*(12)^TSELRF ��ѯ���õ����WiFi��Ƶͨ·*/
//////////////////////////////////////////////////////////////////////////
/*****************************************************************************
 ��������  : int32 WlanATSetTSELRF(uint32 group)
 ��������  : �������ߣ��Ƕ�ͨ·��0
 �������  : NA
 �������  : NA
 �� �� ֵ  : NA
 ����˵��  : 
*****************************************************************************/
STATIC int32 ATSetTSELRF(uint32 group)
{
    int8 acCmd[WIFI_CMD_MAX_SIZE] = {0};
    
    if(WLAN_AT_GROUP_MAX < group)
    {
        return AT_RETURN_FAILURE;
    }
    
    g_wlan_at_data.wifiGroup = group;

    PLAT_WLAN_INFO("Enter,group = %u\n", group); 
    
    if(group <= WIFI_SISO_MODE)
    {
        WIFI_TEST_CMD("wl sgi_tx 0");

        OSA_SNPRINTF(acCmd, sizeof(acCmd), "wl txchain %u", group % 2 + 1);   /*����ģʽ*/
        WIFI_TEST_CMD(acCmd);
    
        OSA_SNPRINTF(acCmd, sizeof(acCmd), "wl rxchain %u", group % 2 + 1);   /*����ģʽ*/
        WIFI_TEST_CMD(acCmd);

        OSA_SNPRINTF(acCmd, sizeof(acCmd), "wl txant %u", group % 2);   /*����ģʽ*/
        WIFI_TEST_CMD(acCmd);
    
        OSA_SNPRINTF(acCmd, sizeof(acCmd), "wl antdiv %u", group % 2);  /*����ģʽ*/
        WIFI_TEST_CMD(acCmd);
    }
    else if(group == WIFI_MIMO_MODE)
    {
        OSA_SNPRINTF(acCmd, sizeof(acCmd), "wl txchain 3");  /*����ģʽ*/
        WIFI_TEST_CMD(acCmd);
        
        OSA_SNPRINTF(acCmd, sizeof(acCmd), "wl rxchain 3");  /*����ģʽ*/
        WIFI_TEST_CMD(acCmd);
    }   
    return (AT_RETURN_SUCCESS);
}

/*****************************************************************************
 ��������  : int32 WlanATGetTSELRFSupport(WLAN_AT_BUFFER_STRU *strBuf)
 ��������  : ֧�ֵ������������У����ַ�����ʽ����eg: 0,1,2,3
 �������  : NA
 �������  : NA
 �� �� ֵ  : NA
 ����˵��  : 
*****************************************************************************/
STATIC int32 ATGetTSELRFSupport(WLAN_AT_BUFFER_STRU *strBuf)
{
    if (NULL == strBuf)
    {
        return (AT_RETURN_FAILURE);
    }

    /*begin add by yangzhiyong for E5573S-607 antenna*/
    uint32  *pu32HwId = (u32 *)SHM_MEM_HW_VER_ADDR;
    if(HW_VER_PRODUCT_E5573S_607  ==  *pu32HwId)
    {
        OSA_SNPRINTF(strBuf->buf, WLAN_AT_BUFFER_SIZE, "%s", "0,1");
        return (AT_RETURN_SUCCESS);
    }
    /*end add by yangzhiyong for E5573S-607 antenna*/

    OSA_SNPRINTF(strBuf->buf, sizeof(strBuf->buf), "%s", WLAN_AT_TSELRF_SUPPORT);
    return (AT_RETURN_SUCCESS);
}

//////////////////////////////////////////////////////////////////////////
/*(13)^WiPARANGE���á���ȡWiFi PA���������*/
//////////////////////////////////////////////////////////////////////////
/*****************************************************************************
 ��������  : int wifi_set_pa_mode(int wifiPaMode)
 ��������  : ����WiFi PA���������
 �������  : ����ģʽ
 �������  : NA
 �� �� ֵ  : NA
 ����˵��  : Ŀǰ��ֻ֧��NO PAģʽ
*****************************************************************************/
STATIC int wifi_set_pa_mode(int wifiPaMode)
{
    if (AT_WIFI_MODE_ONLY_PA == wifiPaMode)
    {
        return AT_RETURN_SUCCESS;
    }
    else
    {
        return AT_RETURN_FAILURE;
    }
}

/*****************************************************************************
 ��������  : int wifi_set_pa_mode(int wifiPaMode)
 ��������  : ��ȡ֧�ֵ�WiFi PA���������
 �������  : NA
 �������  : NA
 �� �� ֵ  : NA
 ����˵��  : Ŀǰ��ֻ֧��NO PAģʽ
*****************************************************************************/
STATIC int wifi_get_pa_mode(void)
{
    return AT_WIFI_MODE_ONLY_PA;
}
/*****************************************************************************
 ��������  : int32 WlanATSetWifiParange(WLAN_AT_WiPARANGE_TYPE pa_type)
 ��������  : ����WiFi PA���������
 �������  : NA
 �������  : NA
 �� �� ֵ  : NA
 ����˵��  : 
*****************************************************************************/
STATIC int32 ATSetWifiParange(WLAN_AT_WiPARANGE_TYPE pa_type)
{
    int32 ret = AT_RETURN_FAILURE;
    switch (pa_type)
    {
        case AT_WiPARANGE_LOW:
            ret = wifi_set_pa_mode(AT_WIFI_MODE_ONLY_NOPA);
            break;
        case AT_WiPARANGE_HIGH:
            ret = wifi_set_pa_mode(AT_WIFI_MODE_ONLY_PA);
            break;
        default:
            break;
    }
    return ret;
}

/*****************************************************************************
 ��������  : WLAN_AT_WiPARANGE_TYPE WlanATGetWifiParange()
 ��������  : ��ȡWiFi PA���������
 �������  : NA
 �������  : NA
 �� �� ֵ  : NA
 ����˵��  : 
*****************************************************************************/
STATIC WLAN_AT_WiPARANGE_TYPE ATGetWifiParange(void)
{
    WLAN_AT_WiPARANGE_TYPE lWifiPAMode = AT_WiPARANGE_BUTT;
    switch (wifi_get_pa_mode())
    {
        case AT_WIFI_MODE_ONLY_NOPA:
            lWifiPAMode = AT_WiPARANGE_LOW;
            break;
        case AT_WIFI_MODE_ONLY_PA:
            lWifiPAMode = AT_WiPARANGE_HIGH;
            break;
        default:
            break;
    }
    return lWifiPAMode;
}

/*****************************************************************************
 ��������  : int32 WlanATGetWifiParangeSupport(WLAN_AT_BUFFER_STRU *strBuf)
 ��������  : ֧�ֵ�paģʽ���У����ַ�����ʽ����eg: l,h
 �������  : NA
 �������  : NA
 �� �� ֵ  : NA
 ����˵��  : 
*****************************************************************************/
STATIC int32 ATGetWifiParangeSupport(WLAN_AT_BUFFER_STRU *strBuf)
{
    if (NULL == strBuf)
    {
        return (AT_RETURN_FAILURE);
    }
    switch (wifi_get_pa_mode())
    {
        case AT_WIFI_MODE_ONLY_NOPA:
            OSA_SNPRINTF(strBuf->buf, sizeof(strBuf->buf), "%c", AT_WiPARANGE_LOW);
            break;
        case AT_WIFI_MODE_ONLY_PA:
            OSA_SNPRINTF(strBuf->buf, sizeof(strBuf->buf), "%c", AT_WiPARANGE_HIGH);
            break;
        case AT_WIFI_MODE_PA_NOPA:
            OSA_SNPRINTF(strBuf->buf, sizeof(strBuf->buf), "%c, %c"
                                                 , AT_WiPARANGE_LOW ,AT_WiPARANGE_HIGH);
            break;
        default:
            return (AT_RETURN_FAILURE);
    }
    
    return (AT_RETURN_SUCCESS);
}


/* ����241оƬ��֧�� */
/*===========================================================================
 (14)^WICALTEMP���á���ȡWiFi���¶Ȳ���ֵ
===========================================================================*/
/*****************************************************************************
 ��������  : int32 WlanATGetWifiCalTemp(WLAN_AT_WICALTEMP_STRU *params)
 ��������  : ����WiFi���¶Ȳ���ֵ
 �������  : NA
 �������  : params:�¶Ȳ�������
 �� �� ֵ  : WLAN_AT_RETURN_TYPE
*****************************************************************************/
STATIC int32 ATGetWifiCalTemp(WLAN_AT_WICALTEMP_STRU *params)
{
    return  AT_RETURN_FAILURE;
}

/*****************************************************************************
 ��������  : int32 WlanATGetWifiCalTemp(WLAN_AT_WICALTEMP_STRU *params)
 ��������  : ����WiFi���¶Ȳ���ֵ
 �������  : params:�¶Ȳ�������
 �������  : NA
 �� �� ֵ  : WLAN_AT_RETURN_TYPE
*****************************************************************************/
STATIC int32 ATSetWifiCalTemp(WLAN_AT_WICALTEMP_STRU *params)
{
    return  AT_RETURN_FAILURE;
}

/*===========================================================================
 (15)^WICALDATA���á���ȡָ�����͵�WiFi��������
===========================================================================*/
/*****************************************************************************
 ��������  : int32 WlanATGetWifiCalData(WLAN_AT_WICALDATA_STRU * params)
 ��������  : ָ�����͵�WiFi��������
 �������  : NA
 �������  : params:��������
 �� �� ֵ  : WLAN_AT_RETURN_TYPE
*****************************************************************************/
STATIC int32 ATGetWifiCalData(WLAN_AT_WICALDATA_STRU * params)
{   
    return  AT_RETURN_FAILURE;
}

/*****************************************************************************
 ��������  : int32 WlanATSetWifiCalData(WLAN_AT_WICALDATA_STRU *params)
 ��������  : ָ�����͵�WiFi��������
 �������  : params:��������
 �������  : NA
 �� �� ֵ  : WLAN_AT_RETURN_TYPE
*****************************************************************************/
STATIC int32 ATSetWifiCalData(WLAN_AT_WICALDATA_STRU *params)
{
    return  AT_RETURN_FAILURE;
}

/*===========================================================================
 (16)^WICAL���á���ȡУ׼������״̬���Ƿ�֧�ֲ���
===========================================================================*/
/*****************************************************************************
 ��������  : int32 WlanATSetWifiCal(WLAN_AT_FEATURE_TYPE onoff)
 ��������  : ����У׼������״̬
 �������  : onoff:0,����У׼��1,����У׼
 �������  : NA
 �� �� ֵ  : WLAN_AT_FEATURE_TYPE
*****************************************************************************/
STATIC int32 ATSetWifiCal(WLAN_AT_FEATURE_TYPE onoff)
{
    return  AT_RETURN_FAILURE;
}

/*****************************************************************************
 ��������  : WLAN_AT_FEATURE_TYPE WlanATGetWifiCal(void)
 ��������  : ��ȡУ׼������״̬
 �������  : NA
 �������  : NA
 �� �� ֵ  : WLAN_AT_FEATURE_TYPE
*****************************************************************************/
STATIC WLAN_AT_FEATURE_TYPE ATGetWifiCal(void)
{
    return  AT_RETURN_FAILURE;
}

/*****************************************************************************
 ��������  : WLAN_AT_FEATURE_TYPE WlanATGetWifiCalSupport(void)
 ��������  : �Ƿ�֧��У׼
 �������  : NA
 �������  : NA
 �� �� ֵ  : WLAN_AT_FEATURE_TYPE
*****************************************************************************/
STATIC WLAN_AT_FEATURE_TYPE ATGetWifiCalSupport(void)
{
    return  AT_FEATURE_DISABLE;
}

/*===========================================================================
 (17)^WICALFREQ ���á���ѯƵ�ʲ���ֵ
===========================================================================*/
/*****************************************************************************
 ��������  : int32 WlanATSetWifiCalFreq(WLAN_AT_WICALFREQ_STRU *params)
 ��������  : ����Ƶ�ʲ���
 �������  : params:��������
 �������  : NA
 �� �� ֵ  : WLAN_AT_FEATURE_TYPE
*****************************************************************************/
STATIC int32 ATSetWifiCalFreq(WLAN_AT_WICALFREQ_STRU *params)
{
    return  AT_RETURN_FAILURE;
}

/*****************************************************************************
 ��������  : int32 WlanATGetWifiCalFreq(WLAN_AT_WICALFREQ_STRU *params)
 ��������  : ����Ƶ�ʲ���
 �������  : NA
 �������  : params:��������
 �� �� ֵ  : WLAN_AT_FEATURE_TYPE
*****************************************************************************/
STATIC int32 ATGetWifiCalFreq(WLAN_AT_WICALFREQ_STRU *params)
{
    return  AT_RETURN_FAILURE;
}

/*===========================================================================
 (18)^WICALPOW ���á���ѯ���ʲ���ֵ
===========================================================================*/
/*****************************************************************************
 ��������  : int32 WlanATGetWifiCalFreq(WLAN_AT_WICALFREQ_STRU *params)
 ��������  : ���ù��ʲ���
 �������  : NA
 �������  : params:��������
 �� �� ֵ  : WLAN_AT_FEATURE_TYPE
*****************************************************************************/
STATIC int32 ATSetWifiCalPOW(WLAN_AT_WICALPOW_STRU *params)
{
    return  AT_RETURN_FAILURE;
}

/*****************************************************************************
 ��������  : int32 WlanATGetWifiCalPOW(WLAN_AT_WICALPOW_STRU *params)
 ��������  : У׼���书��ʱ����ѯ��Ӧֵ
 �������  : NA
 �������  : NA
 �� �� ֵ  : int32
*****************************************************************************/
STATIC int32 ATGetWifiCalPOW(WLAN_AT_WICALPOW_STRU *params)
{
    return  AT_RETURN_FAILURE;
}



STATIC WLAN_CHIP_OPS bcm43241_ops = 
{
    .WlanATSetWifiEnable = ATSetWifiEnable,
    .WlanATGetWifiEnable = ATGetWifiEnable,
    .WlanATSetWifiMode   = ATSetWifiMode,
    .WlanATGetWifiMode   = ATGetWifiMode,
    .WlanATGetWifiModeSupport   = ATGetWifiModeSupport,

    .WlanATSetWifiBand = ATSetWifiBand,
    .WlanATGetWifiBand = ATGetWifiBand,
    .WlanATGetWifiBandSupport = ATGetWifiBandSupport,

    .WlanATSetWifiFreq = ATSetWifiFreq,
    .WlanATGetWifiFreq = ATGetWifiFreq,

    .WlanATSetWifiDataRate = ATSetWifiDataRate,
    .WlanATGetWifiDataRate = ATGetWifiDataRate,

    .WlanATSetWifiPOW = ATSetWifiPOW,
    .WlanATGetWifiPOW = ATGetWifiPOW,

    .WlanATSetWifiTX = ATSetWifiTX,
    .WlanATGetWifiTX = ATGetWifiTX,

    .WlanATSetWifiRX = ATSetWifiRX,
    .WlanATGetWifiRX = ATGetWifiRX,

    .WlanATSetWifiRPCKG = ATSetWifiRPCKG,
    .WlanATGetWifiRPCKG = ATGetWifiRPCKG,
    .WlanATGetWifiPlatform = ATGetWifiPlatform,
    
    .WlanATGetTSELRF = NULL,
    .WlanATSetTSELRF = ATSetTSELRF,
    .WlanATGetTSELRFSupport =  ATGetTSELRFSupport,

    .WlanATSetWifiParange = ATSetWifiParange,
    .WlanATGetWifiParange = ATGetWifiParange,

    .WlanATGetWifiParangeSupport = ATGetWifiParangeSupport,
    
    .WlanATGetWifiCalTemp = NULL,
    .WlanATSetWifiCalTemp = NULL,
    .WlanATSetWifiCalData = NULL,
    .WlanATGetWifiCalData = NULL,
    .WlanATSetWifiCal = NULL,
    .WlanATGetWifiCal = NULL,
    .WlanATGetWifiCalSupport = NULL,
    .WlanATSetWifiCalFreq = NULL,
    .WlanATGetWifiCalFreq = NULL,
    .WlanATSetWifiCalPOW = NULL,
    .WlanATGetWifiCalPOW = NULL
};

/* ע��bcm43241��wlan atģ�� */
void wlan_at_reg_bcm43241(WLAN_CHIP_OPS **wlan_ops)
{
    PLAT_WLAN_INFO("enter");
    wlan_ops[bcm43241] = &bcm43241_ops;
    PLAT_WLAN_INFO("exit");
}


//////////////////////////////////////////////////////////////////////////
EXPORT_SYMBOL(wlan_at_get_packet_report);
