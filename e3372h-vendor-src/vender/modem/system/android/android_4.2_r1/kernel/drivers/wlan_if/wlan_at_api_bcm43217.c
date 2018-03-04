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
/* linux ϵͳͷ�ļ� */
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/string.h>
#include <asm/uaccess.h>
#include <linux/ctype.h>
#include <linux/kernel.h>
#include <linux/fs.h>

/* ƽ̨ͷ�ļ� */
#include "wlan_at.h"
#include "wlan_utils.h"


/*2.4g adapter interface*/
#define IF_NAME   "-i wl0"

#define WL_CONFIG_EXE_PATH   "/system/bin/bcm43217/exe/wl " 
#define WL_CMD_BUF_LEN          (128)
#define WL_COUNTER_BUF_LEN      (2048)

#define WL_DEFAULT_POWER    (600) /* Ĭ�Ϲ��� */

/*��WiFiоƬ�·���������*/
#define WIFI_TEST_CMD(cmd) do{ \
    if (WLAN_SUCCESS != wlan_run_cmd(cmd)) \
    { \
        PLAT_WLAN_INFO("Run CMD Error!!\n"); \
        return AT_RETURN_FAILURE; \
    } \
    msleep(DALEY_50_TIME);\
}while(0)

#define WIFI_SHELL_CMD(cmd) do { \
    if (WLAN_SUCCESS != wlan_run_shell(cmd)) \
    { \
        PLAT_WLAN_INFO("Run shellcmd Error!!!"); \
        return AT_RETURN_FAILURE; \
    } \
}while(0)

typedef enum
{    
    AT_TSELRF_A   = 0,  /*��ƵA ·*/
    AT_TSELRF_B   = 1,  /*��ƵB ·*/
    AT_TSELRF_MAX      /*���ֵ*/
}WLAN_AT_TSELRF_TYPE;

#define WLAN_AT_MODE_SUPPORT            "2,3,4"          /*֧�ֵ�ģʽ(a/b/g/n/ac)*/

#define WLAN_AT_BAND_SUPPORT            "0,1"              /*֧�ֵĴ���(20M/40M/80M/160M)*/
#define WLAN_AT_TSELRF_SUPPORT          "0,1"          /*֧�ֵ�������������*/

#define WLAN_AT_GROUP_MAX               1                  /*֧�ֵ������������*/

#define IS_TSELRF_VAILD(tselrf) ((AT_TSELRF_A == tselrf) || (AT_TSELRF_B == tselrf))

#define ACCMD_LEN               (200)

/*WiFi����ģʽ*/
#define AT_WIFI_MODE_ONLY_PA            0x00                /*WIFIֻ֧��PAģʽ*/
#define AT_WIFI_MODE_ONLY_NOPA          0x01                /*WIFIֻ֧��NO PAģʽ*/
#define AT_WIFI_MODE_PA_NOPA            0x02                /*WIFIͬʱ֧��PAģʽ��NO PAģʽ*/

#define WLAN_CHANNEL_2G_MIN             1                   /*2.4G�ŵ���Сֵ*/
#define WLAN_CHANNEL_2G_MAX             14                  /*2.4G�ŵ����*/
#define WLAN_CHANNEL_2G_MIDDLE          6

#define WLAN_FREQ_2G_MAX                2484                  /*2.4G���Ƶ��*/

#define WIFI_CMD_MAX_SIZE               256                   /*cmd�ַ���256����*/
#define WIFI_CMD_8_SIZE                 8                     /*cmd�ַ���8����*/
#define HUNDRED                         100

#define DALEY_50_TIME  50
#define DALEY_100_TIME  100
#define DALEY_500_TIME  500
#define DALEY_1000_TIME 1000
#define DALEY_5000_TIME 5000
#define DALEY_2000_TIME 2000


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
    uint32                  wifiRate;      /*wifi��������*/
    int32                   wifiPower;     /*wifi���书��*/
    WLAN_AT_FEATURE_TYPE    wifiTX;        /*wifi�����״̬*/
    WLAN_AT_WIRX_STRU       wifiRX;        /*wifi���ջ�״̬*/
    WLAN_AT_WIRPCKG_STRU    wifiPckg;      /*wifi�����*/  
    uint32                  wifiGroup;     /*wifi����ģʽ*/

    WLAN_AT_FEATURE_TYPE    calonoff;      /* У׼�л����� */
    WLAN_AT_WICALDATA_STRU  WiCalData;     /* У׼���� */ 
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
         , {2412, 0}, 6500, 3175, AT_FEATURE_DISABLE, {AT_FEATURE_DISABLE, {0}, {0}}, AT_WiPARANGE_HIGH};/*����WiFi��Ĭ�ϲ���*/


STATIC unsigned int g_ulUcastWifiRxPkts;
STATIC unsigned int g_ulMcastWifiRxPkts;
STATIC unsigned int g_ulRxState = 0;

/*�ŵ��б�*/
const uint16 g_ausChannels[] = {2412,2417,2422,2427,2432,2437,2442,2447,2452,2457,2462,2467,2472,2484};
/* ��Ƶ�ṩ��У׼�� */

#define STEP (10)
#define HALF_STEP (STEP / 2)
#define MAX_POW_DATA (300)
#define MIN_POW_DATA (-300)

#define TBL_PA_INDEX_MAX    ((MAX_POW_DATA - MIN_POW_DATA) / STEP)
/*WIFIУ׼������*/

unsigned char Tbl_PA_2G_ANT0[TBL_PA_INDEX_MAX][3][7] = {
    "0xfe29", "0x191c", "0xf98f",    /*0.1dB*/
    "0xfe29", "0x194f", "0xf984",    /*-0.1dB*/
    "0xfe29", "0x1902", "0xf995",    /*0.2dB*/
    "0xfe29", "0x1969", "0xf97e",    /*-0.2dB*/
    "0xfe29", "0x18e9", "0xf99b",    /*0.3dB*/
    "0xfe29", "0x1982", "0xf978",    /*-0.3dB*/
    "0xfe29", "0x18cf", "0xf9a1",    /*0.4dB*/
    "0xfe29", "0x199c", "0xf972",    /*-0.4dB*/
    "0xfe29", "0x18b6", "0xf9a7",    /*0.5dB*/
    "0xfe29", "0x19b6", "0xf96c",    /*-0.5dB*/
    "0xfe29", "0x189c", "0xf9ad",    /*0.6dB*/
    "0xfe29", "0x19cf", "0xf966",    /*-0.6dB*/
    "0xfe29", "0x1882", "0xf9b3",    /*0.7dB*/
    "0xfe29", "0x19e9", "0xf960",    /*-0.7dB*/
    "0xfe29", "0x1869", "0xf9b9",    /*0.8dB*/
    "0xfe29", "0x1a02", "0xf95a",    /*-0.8dB*/
    "0xfe29", "0x184f", "0xf9be",    /*0.9dB*/
    "0xfe29", "0x1a1c", "0xf955",    /*-0.9dB*/
    "0xfe29", "0x1836", "0xf9c4",    /*1dB*/
    "0xfe29", "0x1a36", "0xf94f",    /*-1dB*/
    "0xfe29", "0x181c", "0xf9ca",    /*1.1dB*/
    "0xfe29", "0x1a4f", "0xf949",    /*-1.1dB*/
    "0xfe29", "0x1802", "0xf9d0",    /*1.2dB*/
    "0xfe29", "0x1a69", "0xf943",    /*-1.2dB*/
    "0xfe29", "0x17e9", "0xf9d6",    /*1.3dB*/
    "0xfe29", "0x1a82", "0xf93d",    /*-1.3dB*/ 
    "0xfe29", "0x17cf", "0xf9dc",    /*1.4dB*/
    "0xfe29", "0x1a9c", "0xf937",    /*-1.4dB*/   
    "0xfe29", "0x17b6", "0xf9e2",    /*1.5dB*/     
    "0xfe29", "0x1ab6", "0xf931",    /*-1.5dB*/    
    "0xfe29", "0x179c", "0xf9e8",    /*1.6dB*/   
    "0xfe29", "0x1acf", "0xf92b",    /*-1.6dB*/    
    "0xfe29", "0x1782", "0xf9ee",    /*1.7dB*/    
    "0xfe29", "0x1ae9", "0xf925",    /*-1.7dB*/    
    "0xfe29", "0x1769", "0xf9f3",    /*1.8dB*/    
    "0xfe29", "0x1b02", "0xf920",    /*-1.8dB*/    
    "0xfe29", "0x174f", "0xf9f9",    /*1.9dB*/    
    "0xfe29", "0x1b1c", "0xf91a",    /*-1.9dB*/    
    "0xfe29", "0x1736", "0xf9ff",    /*2dB*/
    "0xfe29", "0x1b36", "0xf914",    /*-2dB*/
    "0xfe29", "0x171c", "0xfa05",    /*2.1dB*/
    "0xfe29", "0x1b4f", "0xf90e",    /*-2.1dB*/
    "0xfe29", "0x1702", "0xfa0b",    /*2.2dB*/
    "0xfe29", "0x1b69", "0xf908",    /*-2.2dB*/
    "0xfe29", "0x16e9", "0xfa11",    /*2.3dB*/    
    "0xfe29", "0x1b82", "0xf902",    /*-2.3dB*/    
    "0xfe29", "0x16cf", "0xfa17",    /*2.4dB*/    
    "0xfe29", "0x1b9c", "0xf8fc",    /*-2.4dB*/    
    "0xfe29", "0x16b6", "0xfa1d",    /*2.5dB*/    
    "0xfe29", "0x1bb6", "0xf8f6",    /*-2.5dB*/    
    "0xfe29", "0x169c", "0xfa23",    /*2.6dB*/
    "0xfe29", "0x1bcf", "0xf8f0",    /*-2.6dB*/    
    "0xfe29", "0x1682", "0xfa28",    /*2.7dB*/    
    "0xfe29", "0x1be9", "0xf8eb",    /*-2.7dB*/    
    "0xfe29", "0x1669", "0xfa2e",    /*2.8dB*/    
    "0xfe29", "0x1c02", "0xf8e5",    /*-2.8dB*/   
    "0xfe29", "0x164f", "0xfa34",    /*2.9dB*/  
    "0xfe29", "0x1c1c", "0xf8df",    /*-2.9dB*/
};

unsigned char Tbl_PA_2G_ANT1[TBL_PA_INDEX_MAX][3][7] = {
    "0xfe59", "0x1b03", "0xf962",    /*0.1dB*/
    "0xfe59", "0x1b36", "0xf958",    /*-0.1dB*/
    "0xfe59", "0x1ae9", "0xf968",    /*0.2dB*/
    "0xfe59", "0x1b50", "0xf952",    /*-0.2dB*/
    "0xfe59", "0x1ad0", "0xf96d",    /*0.3dB*/
    "0xfe59", "0x1b69", "0xf94d",    /*-0.3dB*/
    "0xfe59", "0x1ab6", "0xf972",    /*0.4dB*/
    "0xfe59", "0x1b83", "0xf948",    /*-0.4dB*/
    "0xfe59", "0x1a9d", "0xf977",    /*0.5dB*/
    "0xfe59", "0x1b9d", "0xf942",    /*-0.5dB*/
    "0xfe59", "0x1a83", "0xf97d",    /*0.6dB*/
    "0xfe59", "0x1bb6", "0xf93d",    /*-0.6dB*/
    "0xfe59", "0x1a69", "0xf982",    /*0.7dB*/
    "0xfe59", "0x1bd0", "0xf938",    /*-0.7dB*/
    "0xfe59", "0x1a50", "0xf987",    /*0.8dB*/
    "0xfe59", "0x1be9", "0xf933",    /*-0.8dB*/
    "0xfe59", "0x1a36", "0xf98d",    /*0.9dB*/
    "0xfe59", "0x1c03", "0xf92d",    /*-0.9dB*/
    "0xfe59", "0x1a1d", "0xf992",    /*1dB*/
    "0xfe59", "0x1c1d", "0xf928",    /*-1dB*/
    "0xfe59", "0x1a03", "0xf997",    /*1.1dB*/
    "0xfe59", "0x1c36", "0xf923",    /*-1.1dB*/
    "0xfe59", "0x19e9", "0xf99c",    /*1.2dB*/
    "0xfe59", "0x1c50", "0xf91d",    /*-1.2dB*/
    "0xfe59", "0x19d0", "0xf9a2",    /*1.3dB*/
    "0xfe59", "0x1c69", "0xf918",    /*-1.3dB*/
    "0xfe59", "0x19b6", "0xf9a7",    /*1.4dB*/
    "0xfe59", "0x1c83", "0xf913",    /*-1.4dB*/   
    "0xfe59", "0x199d", "0xf9ac",    /*1.5dB*/     
    "0xfe59", "0x1c9d", "0xf90e",    /*-1.5dB*/    
    "0xfe59", "0x1983", "0xf9b2",    /*1.6dB*/   
    "0xfe59", "0x1cb6", "0xf908",    /*-1.6dB*/    
    "0xfe59", "0x1969", "0xf9b7",    /*1.7dB*/    
    "0xfe59", "0x1cd0", "0xf903",    /*-1.7dB*/    
    "0xfe59", "0x1950", "0xf9bc",    /*1.8dB*/    
    "0xfe59", "0x1ce9", "0xf8fe",    /*-1.8dB*/    
    "0xfe59", "0x1936", "0xf9c1",    /*1.9dB*/    
    "0xfe59", "0x1d03", "0xf8f8",    /*-1.9dB*/    
    "0xfe59", "0x191d", "0xf9c7",    /*2dB*/
    "0xfe59", "0x1d1d", "0xf8f3",    /*-2dB*/  
    "0xfe59", "0x1903", "0xf9cc",    /*2.1dB*/    
    "0xfe59", "0x1d36", "0xf8ee",    /*-2.1dB*/    
    "0xfe59", "0x18e9", "0xf9d1",    /*2.2dB*/    
    "0xfe59", "0x1d50", "0xf8e9",    /*-2.2dB*/    
    "0xfe59", "0x18d0", "0xf9d7",    /*2.3dB*/    
    "0xfe59", "0x1d69", "0xf8e3",    /*-2.3dB*/    
    "0xfe59", "0x18b6", "0xf9dc",    /*2.4dB*/    
    "0xfe59", "0x1d83", "0xf8de",    /*-2.4dB*/    
    "0xfe59", "0x189d", "0xf9e1",    /*2.5dB*/    
    "0xfe59", "0x1d9d", "0xf8d9",    /*-2.5dB*/    
    "0xfe59", "0x1883", "0xf9e6",    /*2.6dB*/
    "0xfe59", "0x1db6", "0xf8d3",    /*-2.6dB*/    
    "0xfe59", "0x1869", "0xf9ec",    /*2.7dB*/    
    "0xfe59", "0x1dd0", "0xf8ce",    /*-2.7dB*/    
    "0xfe59", "0x1850", "0xf9f1",    /*2.8dB*/    
    "0xfe59", "0x1de9", "0xf8c9",    /*-2.8dB*/   
    "0xfe59", "0x1836", "0xf9f6",    /*2.9dB*/  
    "0xfe59", "0x1e03", "0xf8c4",    /*-2.9dB*/
};


/*****************************************************************************
 ��������  : check_wifi_valid
 ��������  : �鿴wifiͨ·�Ƿ�ok
 �������  : NA
 �������  : NA
 �� �� ֵ  : 0 �ɹ�
             1 ʧ��
 ����˵��  : 
*****************************************************************************/
STATIC int32 check_wifi_valid()
{        
    WIFI_TEST_CMD(WL_CONFIG_EXE_PATH" ver");
    return AT_RETURN_SUCCESS;
}

/*****************************************************************************
 �� �� ��  : GetCmdRetValue
 ��������  : ���ļ��н�����ȡ��Ӧ�ֶε�ֵ
 �������  : ��Ҫ��ѯ���ֶ�
 �������  : ��
 �� �� ֵ  : ��Ӧ�ֶε�ֵ
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��05��31��
    �޸�����   : �����ɺ���
*****************************************************************************/
STATIC unsigned int GetCmdRetValue(char * pValueStr)
{
    char        *pcRecPcktsStart        = NULL;
    char         acMonitorStrValue[WIFI_CMD_MAX_SIZE] = {0};
    unsigned int uiRetPcktsNumBuf       = 0;
    unsigned int uiIdPcktsCntValue      = 0;
    char filebuff[WL_COUNTER_BUF_LEN + 1] = {0};

    /* ��μ�� */
    ASSERT_NULL_POINTER(pValueStr, AT_RETURN_FAILURE);

    if(0 == wlan_read_file("/var/wifirxpkt", filebuff, sizeof(filebuff)))
    {
        PLAT_WLAN_ERR("read_from_file fail");
        return AT_RETURN_FAILURE;
    }

    pcRecPcktsStart = strstr(filebuff, pValueStr);

    if(NULL == pcRecPcktsStart)
    {
        return;
    }    

    pcRecPcktsStart = pcRecPcktsStart + strlen(pValueStr)+ 1; /* 1:�ո�ռλ */
    
    while(' ' != *pcRecPcktsStart)
    {
        acMonitorStrValue[uiIdPcktsCntValue++] = *pcRecPcktsStart;
        pcRecPcktsStart++;
    }
    uiRetPcktsNumBuf = wlan_strtoi(acMonitorStrValue, NULL, WLAN_BASE10);
    return (uiRetPcktsNumBuf);
}

/*****************************************************************************
 �� �� ��  : wifi_get_rx_packet_report
 ��������  : ��ȡwifi���հ���
 �������  : ucastPkts��mcastPkts
 �������  : ucast mcast ����
 �� �� ֵ  : ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��05��31��
    �޸�����   : �����ɺ���
*****************************************************************************/
STATIC void wifi_get_rx_packet_report(unsigned int *ucastPkts, unsigned int *mcastPkts)
{

    char acCmdValueStr[WIFI_CMD_MAX_SIZE] = {0};
    
    if(NULL == ucastPkts || NULL == mcastPkts)
    {
        return;
    }

    OSA_SNPRINTF(acCmdValueStr,sizeof(acCmdValueStr), WL_CONFIG_EXE_PATH" %s counters > /var/wifirxpkt", IF_NAME );
 
    WIFI_SHELL_CMD(acCmdValueStr);

    *mcastPkts = GetCmdRetValue("pktengrxdmcast");
    *ucastPkts = GetCmdRetValue("pktengrxducast");
    PLAT_WLAN_INFO("*mcastPkts=%d, *ucastPkts=%d \n", *mcastPkts, *ucastPkts);
}


/*****************************************************************************
 �� �� ��  : cal_data_to_index
 ��������  : ���� ^WICALDATA��data���ֻ�ȡwifi������index 
 �������  : cal_data:^WICALDATA��data
 �������  : NA
 �� �� ֵ  : int cal_index
 ���ú���  :
 ��������  :
*****************************************************************************/
STATIC unsigned int cal_data_to_index(long int cal_data)
{
    int          residual = 0;
    int          quotient = 0;
    unsigned int cal_index = 0;
    int          temp_index = 0;
    residual = cal_data % STEP;    
    if (0 != residual)
    {
        /*�������ʲ�ֵ*/
        quotient = cal_data / STEP;
        if (((0 < residual) && (HALF_STEP >= residual)) ||
           ((0 > residual) && (((-1) * HALF_STEP) < residual)))
        {
            cal_data = quotient * STEP;
        }
        else if ((HALF_STEP < residual) && (STEP > residual))
        {
            cal_data = (quotient + 1 ) * STEP;
        }
        else if ((((-1) * HALF_STEP) >= residual) && (((-1) * HALF_STEP) < residual))
        {
            cal_data = (quotient - 1 ) * HALF_STEP;
        }
    }    

    temp_index = cal_data / STEP;
    if (0 > temp_index)
    {
        cal_index = (((-1) * temp_index) * 2) - 1; /*���㸺ֵ����*/
    }
    else
    {
        cal_index = ((temp_index) * 2) - 2;  /*������ֵ����*/
    }    

    return cal_index;
}



/*****************************************************************************
 �� �� ��  : set_2g_rf0padata
 ��������  : ����wifi 2g_rf0 pa ��nvֵ
 �������  : ����ֵcal_index
 �������  : NA
 �� �� ֵ  : NA
 ���ú���  :
 ��������  :
*****************************************************************************/
STATIC void set_2g_rf0padata(long int  cal_index)
{
    char         acCmd[ACCMD_LEN] = {0};

    memset(acCmd, 0, ACCMD_LEN);
    OSA_SNPRINTF(acCmd, ACCMD_LEN, "nvram set 0:pa2gw0a0=%s", Tbl_PA_2G_ANT0[cal_index][0]);
    WIFI_TEST_CMD(acCmd);

    memset(acCmd, 0, ACCMD_LEN);
    OSA_SNPRINTF(acCmd, ACCMD_LEN, "nvram set 0:pa2gw1a0=%s", Tbl_PA_2G_ANT0[cal_index][1]);
    WIFI_TEST_CMD(acCmd);

    memset(acCmd, 0, ACCMD_LEN);
    OSA_SNPRINTF(acCmd, ACCMD_LEN, "nvram set 0:pa2gw2a0=%s", Tbl_PA_2G_ANT0[cal_index][2]);
    WIFI_TEST_CMD(acCmd);
    return;
}

/*****************************************************************************
 �� �� ��  : set_2g_rf1padata
 ��������  : ����wifi 2g_rf1 pa ��nvֵ
 �������  : ����ֵcal_index
 �������  : NA
 �� �� ֵ  : NA
 ���ú���  :
 ��������  :
*****************************************************************************/
STATIC void set_2g_rf1padata(long int  cal_index)
{
    char   acCmd[ACCMD_LEN] = {0};
    
    memset(acCmd, 0, ACCMD_LEN);
    OSA_SNPRINTF(acCmd, ACCMD_LEN, "nvram set 0:pa2gw0a1=%s", Tbl_PA_2G_ANT1[cal_index][0]);
    WIFI_TEST_CMD(acCmd);

    memset(acCmd, 0, ACCMD_LEN);
    OSA_SNPRINTF(acCmd, ACCMD_LEN, "nvram set 0:pa2gw1a1=%s", Tbl_PA_2G_ANT1[cal_index][1]);
    WIFI_TEST_CMD(acCmd);

    memset(acCmd, 0, ACCMD_LEN);
    OSA_SNPRINTF(acCmd, ACCMD_LEN, "nvram set 0:pa2gw2a1=%s", Tbl_PA_2G_ANT1[cal_index][2]);
    WIFI_TEST_CMD(acCmd);
    return;
}


/*****************************************************************************
 �� �� ��  : Restore_CalPAToDefault
 ��������  : �ָ�WIFIУ׼Ĭ�ϲ���
 �������  : ��
 �������  : ִ�н��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��07��20��
    �޸�����   : �����ɺ���

*****************************************************************************/
STATIC WLAN_AT_RETURN_TYPE WLANRestoreCalPAToDefault(void)
{
    char         acCmd[ACCMD_LEN] = {0};
    memset(acCmd, 0, ACCMD_LEN);
    OSA_SNPRINTF(acCmd, ACCMD_LEN, "nvram set 0:pa2gw0a0=0xfe29");
    WIFI_TEST_CMD(acCmd);

    memset(acCmd, 0, ACCMD_LEN);
    OSA_SNPRINTF(acCmd, ACCMD_LEN, "nvram set 0:pa2gw1a0=0x191c");
    WIFI_TEST_CMD(acCmd);

    memset(acCmd, 0, ACCMD_LEN);
    OSA_SNPRINTF(acCmd, ACCMD_LEN, "nvram set 0:pa2gw2a0=0xf98f");
    WIFI_TEST_CMD(acCmd);

    memset(acCmd, 0, ACCMD_LEN);
    OSA_SNPRINTF(acCmd, ACCMD_LEN, "nvram set 0:pa2gw0a1=0xfe59");
    WIFI_TEST_CMD(acCmd);

    memset(acCmd, 0, ACCMD_LEN);
    OSA_SNPRINTF(acCmd, ACCMD_LEN, "nvram set 0:pa2gw1a1=0x1b1d");
    WIFI_TEST_CMD(acCmd);

    memset(acCmd, 0, ACCMD_LEN);
    OSA_SNPRINTF(acCmd, ACCMD_LEN, "nvram set 0:pa2gw2a1=0xf95d");
    WIFI_TEST_CMD(acCmd);

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
    WLAN_AT_RETURN_TYPE ret = AT_RETURN_SUCCESS;
    char wl_cmd[WL_CMD_BUF_LEN] = {0};

    PLAT_WLAN_INFO("%s in, onoff = %d\n",__FUNCTION__,onoff);
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
                OSA_SNPRINTF(wl_cmd, WL_CMD_BUF_LEN, WL_CONFIG_EXE_PATH" %s down", IF_NAME);
                WIFI_TEST_CMD(wl_cmd);
                g_wlan_at_data.wifiStatus = AT_WIENABLE_OFF;
            }            
            break;
        case AT_WIENABLE_ON:
            {
                memset(wl_cmd, 0, WL_CMD_BUF_LEN);
                OSA_SNPRINTF(wl_cmd, WL_CMD_BUF_LEN, WL_CONFIG_EXE_PATH" %s down", IF_NAME);
                WIFI_TEST_CMD(wl_cmd);
#if 0
                memset(wl_cmd, 0, WL_CMD_BUF_LEN);
                OSA_SNPRINTF(wl_cmd, WL_CMD_BUF_LEN, "ifconfig %s up", IF_NAME);
                WIFI_TEST_CMD(wl_cmd);

                memset(wl_cmd, 0, WL_CMD_BUF_LEN);
                OSA_SNPRINTF(wl_cmd, WL_CMD_BUF_LEN, WL_CONFIG_EXE_PATH" %s mpc 0", IF_NAME);
                WIFI_TEST_CMD(wl_cmd);

                memset(wl_cmd, 0, WL_CMD_BUF_LEN);
                OSA_SNPRINTF(wl_cmd, WL_CMD_BUF_LEN, WL_CONFIG_EXE_PATH" %s interference 0", IF_NAME);
                WIFI_TEST_CMD(wl_cmd);

                memset(wl_cmd, 0, WL_CMD_BUF_LEN);
                OSA_SNPRINTF(wl_cmd, WL_CMD_BUF_LEN, WL_CONFIG_EXE_PATH" %s phy_watchdog 0", IF_NAME);
                WIFI_TEST_CMD(wl_cmd);

                memset(wl_cmd, 0, WL_CMD_BUF_LEN);
                OSA_SNPRINTF(wl_cmd, WL_CMD_BUF_LEN, WL_CONFIG_EXE_PATH" %s scansuppress 1", IF_NAME);
                WIFI_TEST_CMD(wl_cmd);

                memset(wl_cmd, 0, WL_CMD_BUF_LEN);
                OSA_SNPRINTF(wl_cmd, WL_CMD_BUF_LEN, WL_CONFIG_EXE_PATH" %s down", IF_NAME);
                WIFI_TEST_CMD(wl_cmd);

                memset(wl_cmd, 0, WL_CMD_BUF_LEN);
                OSA_SNPRINTF(wl_cmd, WL_CMD_BUF_LEN, WL_CONFIG_EXE_PATH" %s country ALL", IF_NAME);
                WIFI_TEST_CMD(wl_cmd);

                memset(wl_cmd, 0, WL_CMD_BUF_LEN);
                OSA_SNPRINTF(wl_cmd, WL_CMD_BUF_LEN, WL_CONFIG_EXE_PATH" %s frameburst 1", IF_NAME);
                WIFI_TEST_CMD(wl_cmd);

                memset(wl_cmd, 0, WL_CMD_BUF_LEN);
                OSA_SNPRINTF(wl_cmd, WL_CMD_BUF_LEN, WL_CONFIG_EXE_PATH" %s ampdu 1", IF_NAME);
                WIFI_TEST_CMD(wl_cmd);

                memset(wl_cmd, 0, WL_CMD_BUF_LEN);
                OSA_SNPRINTF(wl_cmd, WL_CMD_BUF_LEN, WL_CONFIG_EXE_PATH" %s mimo_bw_cap 1", IF_NAME);
                WIFI_TEST_CMD(wl_cmd);

                memset(wl_cmd, 0, WL_CMD_BUF_LEN);
                OSA_SNPRINTF(wl_cmd, WL_CMD_BUF_LEN, WL_CONFIG_EXE_PATH" %s bi 65535", IF_NAME);
                WIFI_TEST_CMD(wl_cmd);
#endif
                memset(wl_cmd, 0, WL_CMD_BUF_LEN);
                OSA_SNPRINTF(wl_cmd, WL_CMD_BUF_LEN, WL_CONFIG_EXE_PATH" %s up", IF_NAME);
                WIFI_TEST_CMD(wl_cmd);
                
                g_wlan_at_data.wifiStatus = AT_WIENABLE_ON;
            }
            break;
        case AT_WIENABLE_TEST:
            {                
                OSA_SNPRINTF(wl_cmd, WL_CMD_BUF_LEN, WL_CONFIG_EXE_PATH" %s down", IF_NAME);
                WIFI_TEST_CMD(wl_cmd);
                msleep(DALEY_100_TIME*10);
                
                memset(wl_cmd,0x0,sizeof(wl_cmd));
                OSA_SNPRINTF(wl_cmd, WL_CMD_BUF_LEN, WL_CONFIG_EXE_PATH" %s up", IF_NAME);  
                WIFI_TEST_CMD(wl_cmd);

                g_wlan_at_data.wifiStatus = AT_WIENABLE_TEST;
            }         
            break;
        default: 
            ret = AT_RETURN_FAILURE;
            break;
    }   
    PLAT_WLAN_INFO("%s out, onoff = %d\n",__FUNCTION__,onoff);
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
    PLAT_WLAN_INFO("%s in\n",__FUNCTION__);
    ret = check_wifi_valid();
    if (AT_RETURN_SUCCESS != ret)
    {       
        PLAT_WLAN_INFO("check_wifi_valid failed!\n");        
        return AT_WIENABLE_OFF;
    }
    PLAT_WLAN_INFO("%s in, wifiStatus = %d\n",__FUNCTION__,g_wlan_at_data.wifiStatus);
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
    PLAT_WLAN_INFO("WLAN_AT_WIMODE_TYPE:%d\n", mode);  

    if (mode > AT_WIMODE_80211n)
    {
        return (AT_RETURN_FAILURE);
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
    ASSERT_NULL_POINTER(strBuf, AT_RETURN_FAILURE);

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
    char wl_cmd[WL_CMD_BUF_LEN] = {0};

    PLAT_WLAN_INFO("%s:in,band = %d\n", __FUNCTION__,band);
    
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
                    PLAT_WLAN_INFO("Error wifi mode,must in n mode\n");
                    g_wlan_at_data.wifiBand = AT_WIBAND_40M; /* rx���Բ�����ģʽ */
                }
                break;
            }             
        default:
            ret = AT_RETURN_FAILURE;
            break;
    }

    memset(wl_cmd, 0, WL_CMD_BUF_LEN);
    OSA_SNPRINTF(wl_cmd, WL_CMD_BUF_LEN, WL_CONFIG_EXE_PATH" %s down", IF_NAME);
    WIFI_TEST_CMD(wl_cmd);

    memset(wl_cmd, 0, WL_CMD_BUF_LEN);
    OSA_SNPRINTF(wl_cmd, WL_CMD_BUF_LEN, WL_CONFIG_EXE_PATH" %s mimo_txbw %d", 
        IF_NAME, 2 * (band + 1));
    WIFI_TEST_CMD(wl_cmd);

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
    ASSERT_NULL_POINTER(strBuf, AT_RETURN_FAILURE);

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
  
    /*2.4G 40MƵ�㼯��*/
    const uint16   ausChannels_40M[] = {2422, 2427, 2432, 2437, 2442, 2447, 2452, 2457, 2462};/*2.4G 40M*/
   
    int8 wl_cmd[WIFI_CMD_MAX_SIZE] = {0};
    int8 channeStr[WIFI_CMD_8_SIZE] = {0}; 
    uint16 ulWifiFreq = 0;
    uint16 i = 0;    
    int32 ret = AT_RETURN_SUCCESS;
    
    if (NULL == pFreq)
    {
        return (AT_RETURN_FAILURE);
    }

    if (g_ulRxState)
    {      
        memset(wl_cmd, 0, WL_CMD_BUF_LEN);
        OSA_SNPRINTF(wl_cmd, WL_CMD_BUF_LEN, WL_CONFIG_EXE_PATH" %s down", IF_NAME);
        WIFI_TEST_CMD(wl_cmd);   
        g_ulRxState = 0;
    }
    
    memset(wl_cmd, 0, WL_CMD_BUF_LEN);
    OSA_SNPRINTF(wl_cmd, WL_CMD_BUF_LEN, WL_CONFIG_EXE_PATH" %s band b", IF_NAME);
    WIFI_TEST_CMD(wl_cmd);
    
    memset(wl_cmd, 0, WL_CMD_BUF_LEN);
    OSA_SNPRINTF(wl_cmd, WL_CMD_BUF_LEN, WL_CONFIG_EXE_PATH" %s country ALL", IF_NAME);
    WIFI_TEST_CMD(wl_cmd);
    
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
        else
        {        
            PLAT_WLAN_INFO("Error 20M wifiFreq parameters\n");      
            return AT_RETURN_FAILURE;
        }

        PLAT_WLAN_INFO("Target Channel = %d\n", ulWifiFreq);

        if (!(((WLAN_CHANNEL_2G_MIN <= ulWifiFreq) && (WLAN_CHANNEL_2G_MAX >= ulWifiFreq))))
        {
            PLAT_WLAN_INFO("Target Channel ERROR,ulWifiFreq = %u!\n", ulWifiFreq);
            return AT_RETURN_FAILURE;
        }
        
        memset(wl_cmd, 0, WL_CMD_BUF_LEN);
        OSA_SNPRINTF(wl_cmd, WL_CMD_BUF_LEN, WL_CONFIG_EXE_PATH" bw_cap 2g 1");
        WIFI_TEST_CMD(wl_cmd);
        
        memset(wl_cmd, 0, WL_CMD_BUF_LEN);
        OSA_SNPRINTF(wl_cmd, WL_CMD_BUF_LEN,WL_CONFIG_EXE_PATH" %s chanspec %d", IF_NAME, ulWifiFreq);
        WIFI_TEST_CMD(wl_cmd);

                
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
        else
        {        
            PLAT_WLAN_INFO("Error 40M wifiFreq parameters\n");      
            return AT_RETURN_FAILURE;
        }
         /* �ӵ�3���ŵ���ʼ����Ҫ��2 */
        ulWifiFreq = ulWifiFreq + 2;
        PLAT_WLAN_INFO("Target Channel = %d\n", ulWifiFreq);
    
        if (!(((WLAN_CHANNEL_2G_MIN <= ulWifiFreq) && (WLAN_CHANNEL_2G_MAX >= ulWifiFreq))))
        {
            PLAT_WLAN_INFO("Target Channel ERROR!\n");
            return AT_RETURN_FAILURE;
        }
        /*40M�ŵ���U/L����*/
        memset(wl_cmd, 0, WL_CMD_BUF_LEN);
        OSA_SNPRINTF(wl_cmd, WL_CMD_BUF_LEN, WL_CONFIG_EXE_PATH" bw_cap 2g 3");
        WIFI_TEST_CMD(wl_cmd);
        
        memset(wl_cmd, 0, WL_CMD_BUF_LEN);
        OSA_SNPRINTF(wl_cmd, WL_CMD_BUF_LEN, 
            WL_CONFIG_EXE_PATH" %s chanspec -c %d -b 2 -w 40 -s 1",  
            IF_NAME, ulWifiFreq);
        WIFI_TEST_CMD(wl_cmd);


    }

    /* ����ȫ�ֱ�����Ա���ѯ */
    g_wlan_at_data.wifiFreq.value = pFreq->value;
    g_wlan_at_data.wifiFreq.offset = pFreq->offset;
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
    ASSERT_NULL_POINTER(pFreq, AT_RETURN_FAILURE);
    
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
    int8    wl_cmd[WIFI_CMD_MAX_SIZE] = {0};
    uint32  ulWifiRate = rate / HUNDRED;
    uint32  ulNRate = 0;  
    
    const BRATE_ST wifi_brates_table[] = {{100, "1"}, {200, "2"}, {550, "5.5"}, {1100, "11"}};//b 
    const uint32 wifi_20m_nrates_table[] = {650, 1300, 1950, 2600, 3900, 5200, 5850, 6500};//n ht20
    const uint32 wifi_40m_nrates_table[] = {1350, 2700, 4050, 5400, 8100, 10800, 12150, 13500};//n ht40 

    #define WIFI_BRATES_TABLE_SIZE (sizeof(wifi_brates_table) / sizeof(BRATE_ST))
    #define WIFI_20M_NRATES_TABLE_SIZE (sizeof(wifi_20m_nrates_table) / sizeof(uint32))
    #define WIFI_40M_NRATES_TABLE_SIZE (sizeof(wifi_40m_nrates_table) / sizeof(uint32))
    
    PLAT_WLAN_INFO("WifiRate = %u\n", ulWifiRate);
    
    switch (g_wlan_at_data.wifiMode)
    {
        case AT_WIMODE_CW:
            PLAT_WLAN_INFO("AT_WIMODE_CW\n");
            return (AT_RETURN_FAILURE);   
            
        case AT_WIMODE_80211b:
        case AT_WIMODE_80211g:
            OSA_SNPRINTF(wl_cmd,sizeof(wl_cmd), WL_CONFIG_EXE_PATH" %s nrate -r %d", IF_NAME, ulWifiRate);
            WIFI_TEST_CMD(wl_cmd);
            break;

        case AT_WIMODE_80211n:
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
                    PLAT_WLAN_INFO("20M NRate Error!\n");
                    return (AT_RETURN_FAILURE);
                }

                sprintf(wl_cmd, WL_CONFIG_EXE_PATH" %s nrate -m %d", IF_NAME, ulNRate);
                WIFI_TEST_CMD(wl_cmd);
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
                    PLAT_WLAN_INFO("40M NRate Error!\n");
                    return (AT_RETURN_FAILURE);
                }    
                
                sprintf(wl_cmd, WL_CONFIG_EXE_PATH" %s nrate -m %d -s 3", IF_NAME, ulNRate);
                WIFI_TEST_CMD(wl_cmd);              
            }    
            break;
        default:
            return (AT_RETURN_FAILURE);            
    }    
    WIFI_TEST_CMD(wl_cmd);
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
    /*����ȫ�ֱ�����Ա���ѯ*/
    g_wlan_at_data.wifiPower = power_dBm_percent;
    return (AT_RETURN_SUCCESS);
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
    int8    wl_cmd[WIFI_CMD_MAX_SIZE] = {0};
    int32 WifiPower = 0;
    
    if (AT_WIMODE_CW == g_wlan_at_data.wifiMode)
    {
        /* cw ���� */
    }
    else 
    {
        if(AT_FEATURE_DISABLE == onoff)
        {
            memset(wl_cmd, 0, WL_CMD_BUF_LEN);
            OSA_SNPRINTF(wl_cmd, WL_CMD_BUF_LEN, WL_CONFIG_EXE_PATH" %s pkteng_stop tx", IF_NAME);
            WIFI_TEST_CMD(wl_cmd);

            memset(wl_cmd, 0, WL_CMD_BUF_LEN);
            OSA_SNPRINTF(wl_cmd, WL_CMD_BUF_LEN, WL_CONFIG_EXE_PATH " %s down", IF_NAME);
            WIFI_TEST_CMD(wl_cmd);
        }
        else
        {
            memset(wl_cmd, 0, WL_CMD_BUF_LEN);
            OSA_SNPRINTF(wl_cmd, WL_CMD_BUF_LEN, WL_CONFIG_EXE_PATH" %s down", IF_NAME);
            WIFI_TEST_CMD(wl_cmd);

            memset(wl_cmd, 0, WL_CMD_BUF_LEN);
            OSA_SNPRINTF(wl_cmd, sizeof(wl_cmd), WL_CONFIG_EXE_PATH" txchain %d", 
                (g_wlan_at_data.wifiGroup % 2 + 1));   /*����ģʽ*/
            WIFI_TEST_CMD(wl_cmd);

            memset(wl_cmd, 0, WL_CMD_BUF_LEN);
            OSA_SNPRINTF(wl_cmd, WL_CMD_BUF_LEN, WL_CONFIG_EXE_PATH" %s up", IF_NAME);
            WIFI_TEST_CMD(wl_cmd);
            msleep(DALEY_1000_TIME);
            
            memset(wl_cmd, 0, WL_CMD_BUF_LEN);
            OSA_SNPRINTF(wl_cmd, WL_CMD_BUF_LEN, WL_CONFIG_EXE_PATH" %s ssid \"\"", IF_NAME);
            WIFI_SHELL_CMD(wl_cmd);

            WifiPower = g_wlan_at_data.wifiPower / 100;
            
            memset(wl_cmd, 0, WL_CMD_BUF_LEN);
            if (WL_DEFAULT_POWER == WifiPower )
            {
                OSA_SNPRINTF(wl_cmd, WL_CMD_BUF_LEN, WL_CONFIG_EXE_PATH" txpwr1 -1");
            }
            else
            {
                OSA_SNPRINTF(wl_cmd, WL_CMD_BUF_LEN, WL_CONFIG_EXE_PATH" %s txpwr1 -o -q %d", IF_NAME, WifiPower * 4);
            }
            WIFI_TEST_CMD(wl_cmd);

            memset(wl_cmd, 0, WL_CMD_BUF_LEN);
            OSA_SNPRINTF(wl_cmd, WL_CMD_BUF_LEN, WL_CONFIG_EXE_PATH" %s phy_forcecal 1", IF_NAME);
            WIFI_TEST_CMD(wl_cmd);

            memset(wl_cmd, 0, WL_CMD_BUF_LEN);
            OSA_SNPRINTF(wl_cmd, WL_CMD_BUF_LEN, WL_CONFIG_EXE_PATH" %s pkteng_start 00:90:4c:21:00:8e tx 100 1500 0", IF_NAME);
            WIFI_TEST_CMD(wl_cmd);
            msleep(DALEY_500_TIME);
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
    int8    wl_cmd[WIFI_CMD_MAX_SIZE] = {0};
    ASSERT_NULL_POINTER(params, AT_RETURN_FAILURE);
        
    switch (params->onoff)
    {
        case AT_FEATURE_DISABLE:
            memset(wl_cmd, 0, WL_CMD_BUF_LEN);
            OSA_SNPRINTF(wl_cmd, WL_CMD_BUF_LEN, WL_CONFIG_EXE_PATH" %s pkteng_stop rx", IF_NAME);
            WIFI_TEST_CMD(wl_cmd);      
            
            memset(wl_cmd, 0, WL_CMD_BUF_LEN);
            OSA_SNPRINTF(wl_cmd, WL_CMD_BUF_LEN, WL_CONFIG_EXE_PATH" %s down", IF_NAME);
            WIFI_TEST_CMD(wl_cmd);
            g_ulRxState = 0;
            break;
            
        case AT_FEATURE_ENABLE:
            if (0 == strncmp(params->src_mac,"",MAC_ADDRESS_LEN))
            {
                PLAT_WLAN_INFO("src mac is NULL\n");
                return (AT_RETURN_FAILURE);
            }
            PLAT_WLAN_INFO("src mac is %s\n",params->src_mac);
            
            memset(wl_cmd, 0, WL_CMD_BUF_LEN);
            OSA_SNPRINTF(wl_cmd, sizeof(wl_cmd), WL_CONFIG_EXE_PATH" rxchain %u", 
                (g_wlan_at_data.wifiGroup % 2 + 1));   /*����ģʽ*/
            WIFI_TEST_CMD(wl_cmd);
            
            memset(wl_cmd, 0, WL_CMD_BUF_LEN);
            OSA_SNPRINTF(wl_cmd, WL_CMD_BUF_LEN, WL_CONFIG_EXE_PATH" %s ssid \"\"", IF_NAME);
            WIFI_SHELL_CMD(wl_cmd);
            
            memset(wl_cmd, 0, WL_CMD_BUF_LEN);
            OSA_SNPRINTF(wl_cmd, sizeof(wl_cmd), WL_CONFIG_EXE_PATH"%s up", IF_NAME);
            WIFI_TEST_CMD(wl_cmd);
            
            memset(wl_cmd, 0, WL_CMD_BUF_LEN);
            OSA_SNPRINTF(wl_cmd, sizeof(wl_cmd), WL_CONFIG_EXE_PATH" phy_forcecal 1");
            WIFI_TEST_CMD(wl_cmd);
            
            OSA_SNPRINTF(wl_cmd, sizeof(wl_cmd), WL_CONFIG_EXE_PATH" %s pkteng_start %s rx", IF_NAME, params->src_mac);
            WIFI_TEST_CMD(wl_cmd);
            g_ulRxState = 1;           
            msleep(DALEY_100_TIME);
            wifi_get_rx_packet_report(&g_ulUcastWifiRxPkts,  &g_ulMcastWifiRxPkts);
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
    ASSERT_NULL_POINTER(params, AT_RETURN_FAILURE);

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

    wifi_get_rx_packet_report(&g_ulUcastWifiRxPkts,  &g_ulMcastWifiRxPkts);
    
    return (AT_RETURN_SUCCESS);
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
    unsigned int ulUcastWifiRxPkts = 0;
    unsigned int ulMcastWifiRxPkts = 0;
    unsigned int ulWifiRxPkts = 0;
    unsigned int ulWifiMcastRxPkts = 0;

    int32 ret = AT_RETURN_SUCCESS; 
    ASSERT_NULL_POINTER(params, AT_RETURN_FAILURE);

    /* �жϽ��ջ��Ƿ�� */
    if(AT_FEATURE_DISABLE == g_wlan_at_data.wifiRX.onoff)
    {
        PLAT_WLAN_INFO("Not Rx Mode.\n");
        ret = AT_RETURN_FAILURE;
        return ret;
    }   
    
    wifi_get_rx_packet_report(&ulUcastWifiRxPkts,  &ulMcastWifiRxPkts);
    ulWifiRxPkts = (ulUcastWifiRxPkts - g_ulUcastWifiRxPkts);
    ulWifiMcastRxPkts = (ulMcastWifiRxPkts - g_ulMcastWifiRxPkts);
    if(ulWifiMcastRxPkts > ulWifiRxPkts)
    {
        ulWifiRxPkts = ulWifiMcastRxPkts;
    }
   
    params->good_result = ulWifiRxPkts;
    params->bad_result = 0;   
    
    PLAT_WLAN_INFO("Exit [good = %d, bad = %d]\n", params->good_result, params->bad_result);   
    
    return (AT_RETURN_SUCCESS);
}

//////////////////////////////////////////////////////////////////////////
/*(10)^WIINFO ��ѯWiFi�������Ϣ(���ڵ���ǰ����д��nv��)  */ 
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
STATIC int32  ATSetTSELRF(uint32 group)
{
    int8 acCmd[WIFI_CMD_MAX_SIZE] = {0};
    
    if(WLAN_AT_GROUP_MAX < group)
    {
        return AT_RETURN_FAILURE;
    }
    
    g_wlan_at_data.wifiGroup = group;

    PLAT_WLAN_INFO("[%s]:Enter,group = %u\n", __FUNCTION__, group); 

    OSA_SNPRINTF(acCmd, sizeof(acCmd), WL_CONFIG_EXE_PATH" txchain %u", group % 2 + 1);   /*����ģʽ*/
    WIFI_TEST_CMD(acCmd);
    
    OSA_SNPRINTF(acCmd, sizeof(acCmd), WL_CONFIG_EXE_PATH" rxchain %u", group % 2 + 1);   /*����ģʽ*/
    WIFI_TEST_CMD(acCmd);

    OSA_SNPRINTF(acCmd, sizeof(acCmd), WL_CONFIG_EXE_PATH" txant %u", group % 2);   /*����ģʽ*/
    WIFI_TEST_CMD(acCmd);
    
    OSA_SNPRINTF(acCmd, sizeof(acCmd), WL_CONFIG_EXE_PATH" antdiv %u", group % 2);  /*����ģʽ*/
    WIFI_TEST_CMD(acCmd);
    
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
STATIC int32 ATGetTSELRF(WLAN_AT_BUFFER_STRU *strBuf)
{
    ASSERT_NULL_POINTER(strBuf, AT_RETURN_FAILURE);

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
    ASSERT_NULL_POINTER(strBuf, AT_RETURN_FAILURE);

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
    PLAT_WLAN_INFO("[%s in ]not support \n", __FUNCTION__);
    return AT_RETURN_SUCCESS;
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
    PLAT_WLAN_INFO("[%s in ]not support \n", __FUNCTION__);
    return AT_RETURN_SUCCESS;
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
    ASSERT_NULL_POINTER(params, AT_RETURN_FAILURE);
    memcpy(params, &g_wlan_at_data.WiCalData, sizeof(g_wlan_at_data.WiCalData));
    
    return  AT_RETURN_SUCCESS;
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
    uint32  channel = 0;
    int32   data = 0;
    uint32  cal_index = 0;
    char acCmd[ACCMD_LEN] = {0};

    ASSERT_NULL_POINTER(params, AT_RETURN_FAILURE);
    
    PLAT_WLAN_INFO("Entry type = %d, group = %d, mode = %d, band = %d, bandwidth = %d, freq = %d, data = %s"
        , params->type, (int)params->group, params->mode, params->band, params->bandwidth, (int)params->freq, params->data);
    
    /* Ƶ������ */
    if (AT_WIFREQ_24G != params->band)
    {
        PLAT_WLAN_ERR("Error, band = %d", params->band);
        return AT_RETURN_FAILURE;
    }
    
    /* ���߼�� */
    if (!IS_TSELRF_VAILD(params->group))
    {
        PLAT_WLAN_INFO("Error  wtselrf =%d",  params->group);
        return AT_RETURN_FAILURE;
    }

    /* �ŵ���� */
    for (channel = 0; channel < ARRAY_SIZE(g_ausChannels); channel++)
    {
        if (params->freq == g_ausChannels[channel])
        {
            break;
        }
    }
    if (channel == ARRAY_SIZE(g_ausChannels))
    {
        PLAT_WLAN_ERR("Error, freq = %d", (int)params->freq);
        return AT_RETURN_FAILURE;
    }
    channel++;
    data = wlan_strtoi(params->data, NULL, WLAN_BASE10);
    PLAT_WLAN_INFO("channel = %d, data = %d", (int)channel, (int)data);

    cal_index = cal_data_to_index(data);
    PLAT_WLAN_INFO("cal_index = %d \n", (int)cal_index);


    if (AT_TSELRF_A == params->group)      /*ANT 0*/
    {
        set_2g_rf0padata(cal_index);
    }
    else /*ANT 1*/
    {
        set_2g_rf1padata(cal_index);
    }

    /* commit */
    OSA_SNPRINTF(acCmd, ACCMD_LEN, "nvram commit");
    WIFI_TEST_CMD(acCmd);

    memcpy(&g_wlan_at_data.WiCalData, params, sizeof(g_wlan_at_data.WiCalData));
    return AT_RETURN_SUCCESS;
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
    int     idx = 0;
    int8    acCmd[WIFI_CMD_MAX_SIZE] = {0};
    
    /* �ǽ���У׼״̬��ֱ�ӷ��� */
    PLAT_WLAN_INFO("onoff = %d\n");

    g_wlan_at_data.calonoff = onoff;

    if (AT_FEATURE_DISABLE == onoff)
    {
        return AT_RETURN_SUCCESS;
    }
    
    /* �ָ�����ʼ״̬ */
    (void)WLANRestoreCalPAToDefault();
    
    memset(acCmd, 0, ACCMD_LEN);
    OSA_SNPRINTF(acCmd, ACCMD_LEN, "nvram commit");
    WIFI_TEST_CMD(acCmd);

    return AT_RETURN_SUCCESS;
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
    PLAT_WLAN_INFO("[%s in ]cal_switch = %d", __FUNCTION__, g_wlan_at_data.calonoff);
    return  g_wlan_at_data.calonoff;
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
    return AT_FEATURE_ENABLE;
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
    PLAT_WLAN_INFO("not support");

    return AT_RETURN_SUCCESS;
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
    PLAT_WLAN_INFO("not support");
    return AT_RETURN_SUCCESS;
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
    PLAT_WLAN_INFO("not support");
    return AT_RETURN_SUCCESS;
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
    PLAT_WLAN_INFO("not support \n");
    return AT_RETURN_SUCCESS;
}


STATIC WLAN_CHIP_OPS bcm43217_ops = 
{
    .WlanATSetWifiEnable = ATSetWifiEnable,
    .WlanATGetWifiEnable = ATGetWifiEnable,
    .WlanATSetWifiMode   = ATSetWifiMode,
    .WlanATGetWifiMode   = ATGetWifiMode,

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
    .WlanATGetTSELRF = ATGetTSELRF,
    .WlanATSetTSELRF = ATSetTSELRF,
    .WlanATGetTSELRFSupport = ATGetTSELRF,

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
void wlan_at_reg_bcm43217(WLAN_CHIP_OPS **wlan_ops)
{
    PLAT_WLAN_INFO("enter");
    wlan_ops[bcm43217] = &bcm43217_ops;
    PLAT_WLAN_INFO("exit");
}


