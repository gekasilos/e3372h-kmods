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
#include <linux/kernel.h>
#include "bsp_sram.h"
#include "bsp_shared_ddr.h"
#include "dload_nark_api.h"
#include "dload_product.h"

#ifdef  __cplusplus
    extern "C"{
#endif

/******************************************************************************
  1 ģ��˽�� (�ꡢö�١��ṹ�塢�Զ�����������) ������:
******************************************************************************/
#define DLOAD_PRINT(level, fmt, ...) (printk(level"[*DLOAD_PRODUCT*] %s:%d: "fmt"\n", __func__, __LINE__, ##__VA_ARGS__))


#define PRODUCT_OFFSET              (16)
#define SHOW_NOT_DELAY             (0)       /*��ʱ0s*/

static uint8 g_product_id = 0;
static uint16 g_big_ver = 0;
static boolean dload_burn = FALSE;

/* ·����ʶ�붨�� */
typedef enum 
{
    LCD_DEV_NAME,       /* LCD�豸���� */
    LCD_PIC_SUCCESS,    /* �����ɹ�ͼƬ���� */
    LCD_PIC_FAILED,     /* ����ʧ��ͼƬ���� */
    LCD_PIC_AUTOUG,     /* ��������ͼƬ���� */
    LCD_PIC_LOWPOWER,   /* �͵�ͼƬ���� */
    LCD_PIC_INSTALL0,   /* ��װ����ͼƬ0 */
    LCD_PIC_INSTALL1,   /* ��װ����ͼƬ1 */
    LCD_PIC_INSTALL2,   /* ��װ����ͼƬ2 */
    LCD_PIC_INSTALL3,   /* ��װ����ͼƬ3 */
    LCD_PIC_INSTALL4,   /* ��װ����ͼƬ4 */
    LCD_PIC_INSTALL5,   /* ��װ����ͼƬ5 */
    LCD_PIC_INSTALL6,   /* ��װ����ͼƬ6 */
    LCD_PIC_INSTALL7,   /* ��װ����ͼƬ7 */
    LCD_PIC_INSTALL8,   /* ��װ����ͼƬ8 */
    LCD_PIC_INSTALL9,   /* ��װ����ͼƬ9 */
    LCD_PIC_INSTALL10,  /* ��װ����ͼƬ10 */   
    LCD_PIC_INSTALL11,  /* ��װ����ͼƬ11 */
    
    LED_ATTR_ONOFF,     /* LED��������·�� */
    LED_ATTR_BRIGHT,    /* LED��������·�� */
    LED_ATTR_DELAY_ON,  /* LED DELAY_ON����·�� */
    LED_ATTR_DELAY_OFF, /* LED DELAY_OFF����·�� */
    
    LED_SIGNAL1_ATTR_ONOFF,
    LED_SIGNAL2_ATTR_ONOFF,
    LED_SIGNAL3_ATTR_ONOFF,
    LED_MODE_RED_ATTR_ONOFF,
    LED_MODE_GREEN_ATTR_ONOFF,
    LED_MODE_BLUE_ATTR_ONOFF,

    LED_LTE_RED_ATTR_ONOFF,
    LED_LTE_RED_ATTR_DELAY_ON,
    LED_LTE_RED_ATTR_DELAY_OFF,
    LED_LTE_GREEN_ATTR_ONOFF,
    LED_LTE_GREEN_ATTR_DELAY_ON,
    LED_LTE_GREEN_ATTR_DELAY_OFF,
    LED_LTE_BLUE_ATTR_ONOFF,
    LED_LTE_BLUE_ATTR_DELAY_ON,
    LED_LTE_BLUE_ATTR_DELAY_OFF,

    LED_BAT_RED_ATTR_ONOFF,
    LED_BAT_GREEN_ATTR_ONOFF,
    LED_BAT_GREEN_ATTR_DELAY_ON,
    LED_BAT_GREEN_ATTR_DELAY_OFF, 
    
}show_path_t;



/******************************************************************************
  2 ģ��˽�� (ȫ�ֱ���) ������:
******************************************************************************/

/* ����̬��ƷID����
    E5�� touch E5��BV7R11E   
         ͨ��E5(LED)��BV7R11EU
         ͨ��E5(LCD)��BV7R11EC
    CPE�ࣺBV7R11C
    hilink�� BV7R11HS
    stick�� BV7R11HS
    wingle: BV7R11W
 */
 /*ƽ̨��ʶ����Ʒ��̬��Ӳ���汾��ID()��Ӳ���汾��ID,ƽ̨���ƣ����ڲ�Ʒ���ƣ�����ID*/
STATIC product_info_t product_info_st[] = 
{
    {DLOAD_BALONG, DLOAD_E5, 0X0, 0X3, "V7R11", "E5573s-606",   "BV7R11EU"},   
    {DLOAD_BALONG, DLOAD_E5, 0X0, 0X6, "V7R11", "E5573s-320",   "BV7R11EU"},
    {DLOAD_BALONG, DLOAD_E5, 0X0, 0X306, "V7R11", "E5573s-607",   "BV7R11EU"},
    {DLOAD_BALONG, DLOAD_STICK, 0X1, 0X100, "V7R11", "E3372h-153", "BV7R11HS"},
    {DLOAD_BALONG, DLOAD_STICK, 0X1, 0X101, "V7R11", "E3372h-510", "BV7R11HS"},
    {DLOAD_BALONG, DLOAD_STICK, 0X1, 0X102, "V7R11", "E3372h-210", "BV7R11HS"},
    {DLOAD_BALONG, DLOAD_STICK, 0X1, 0X103, "V7R11", "E3372h-607", "BV7R11HS"},
    {DLOAD_BALONG, DLOAD_WINGLE, 0X1, 0X0, "V7R11", "E8372h-608", "BV7R11W"},
    {DLOAD_BALONG, DLOAD_WINGLE, 0X1, 0X2, "V7R11", "E8372h-510", "BV7R11W"},
    {DLOAD_BALONG, DLOAD_WINGLE, 0X1, 0X3, "V7R11", "E8372h-927", "BV7R11W"},
    {DLOAD_BALONG, DLOAD_WINGLE, 0X1, 0X5, "V7R11", "E8372h-511", "BV7R11W"},
    {DLOAD_BALONG, DLOAD_CPE, 0X4, 0X0, "V7R11", "B310s-927", "BV7R11C"},/*B310s-927*/
    {DLOAD_BALONG, DLOAD_CPE, 0X4, 0X1, "V7R11", "B310s_512", "BV7R11C"},/*B310s_518*/
    {DLOAD_BALONG, DLOAD_CPE, 0X4, 0X2, "V7R11", "B310As_852", "BV7R11C"},/*B310As_852*/
    {DLOAD_BALONG, DLOAD_CPE, 0X4, 0X3, "V7R11", "B315s_22", "BV7R11C"},/*B315s_22*/
    {DLOAD_BALONG, DLOAD_CPE, 0X8, 0X0, "V7R11", "B310s-927", "BV7R11C"},/*B310s-927hisi*/
    {DLOAD_BALONG, DLOAD_CPE, 0X8, 0X1, "V7R11", "B310s_512", "BV7R11C"},/*B310s_518hisi*/
    {DLOAD_BALONG, DLOAD_CPE, 0X8, 0X2, "V7R11", "B310As_852", "BV7R11C"},/*B310As_852hisi*/
    {DLOAD_BALONG, DLOAD_CPE, 0X8, 0X3, "V7R11", "B315s_22", "BV7R11C"},/*B315s_22hisi*/
    {DLOAD_BALONG, DLOAD_E5, 0X0, 0X0, "V7R11", "E5578S-932", "BV7R11EC"},
};

/* �����������ñ�
   ��ƷӲ����ID, ��ƷӲ����ID, USB����, SD����
   ��������, ��ȫ����, HOTA����, FOTA����, Ԥ��*/
STATIC dload_feature_t dload_feature_st[] = 
{
    {0X0, 0X3, TRUE, TRUE, TRUE, TRUE, FALSE, FALSE, {FALSE}},        /* E5573s-606 */
    {0X0, 0X6, TRUE, TRUE, TRUE, TRUE, FALSE, FALSE, {FALSE}},        /* E5573s-320 */
    {0X0, 0X306, TRUE, TRUE, TRUE, TRUE, FALSE, FALSE, {FALSE}},      /* E5573s-607 */
    {0X1, 0X100, TRUE, FALSE, FALSE, TRUE, FALSE, FALSE, {FALSE}},    /* E3372h-153 */
    {0X1, 0X101, TRUE, FALSE, FALSE, TRUE, FALSE, FALSE, {FALSE}},    /* E3372h-510 */
    {0X1, 0X102, TRUE, FALSE, FALSE, TRUE, FALSE, FALSE, {FALSE}},    /* E3372h-210 */
    {0X1, 0X103, TRUE, FALSE, FALSE, TRUE, FALSE, FALSE, {FALSE}},    /* E3372h-607 */
    {0X1, 0X0, TRUE, TRUE, TRUE, TRUE, FALSE, FALSE, {FALSE}},        /* E8372h-608 */
    {0X1, 0X2, TRUE, TRUE, TRUE, TRUE, FALSE, FALSE, {FALSE}},        /* E8372h-510 */
    {0X1, 0X3, TRUE, TRUE, TRUE, TRUE, FALSE, FALSE, {FALSE}},        /* E8372h-927 */
    {0X1, 0X5, TRUE, TRUE, TRUE, TRUE, FALSE, FALSE, {FALSE}},        /* E8372h-511 */
    {0X4, 0X0, FALSE, FALSE, TRUE, TRUE, FALSE, FALSE, {FALSE}},      /* B310s-927 */
    {0X4, 0X1, FALSE, FALSE, TRUE, TRUE, FALSE, FALSE, {FALSE}},      /* B310s_518 */
    {0X4, 0X2, FALSE, FALSE, TRUE, TRUE, FALSE, FALSE, {FALSE}},      /* B310As_852 */
    {0X4, 0X3, FALSE, FALSE, TRUE, TRUE, FALSE, FALSE, {FALSE}},      /* B315s_22 */
    {0X8, 0X0, FALSE, FALSE, TRUE, TRUE, FALSE, FALSE, {FALSE}},      /* B310s-927hisi*/
    {0X8, 0X1, FALSE, FALSE, TRUE, TRUE, FALSE, FALSE, {FALSE}},      /* B310s_518hisi*/
    {0X8, 0X2, FALSE, FALSE, TRUE, TRUE, FALSE, FALSE, {FALSE}},      /* B310As_852hisi*/
    {0X8, 0X3, FALSE, FALSE, TRUE, TRUE, FALSE, FALSE, {FALSE}},      /* B315s_22hisi*/
    {0X0, 0X0, TRUE, TRUE, TRUE, TRUE, FALSE, FALSE, {FALSE}},        /* E5578S-932 */
};


/* ���ݲ�Ʒ����״̬���� �������*/
#if (FEATURE_ON == MBB_DLOAD_MCPEUP)
#define SHOW_YES_DELAY         (200000) /*��ʱ200ms*/
STATIC path_info_t pathinfo_st[] = 
{
    {LED_SIGNAL1_ATTR_ONOFF,        "/sys/class/leds/signal1_led:white/brightness"},
    {LED_SIGNAL2_ATTR_ONOFF,        "/sys/class/leds/signal2_led:white/brightness"},
    {LED_SIGNAL3_ATTR_ONOFF,        "/sys/class/leds/signal3_led:white/brightness"},
    {LED_MODE_RED_ATTR_ONOFF,       "/sys/class/leds/mode_led:red/brightness"},
    {LED_MODE_GREEN_ATTR_ONOFF,     "/sys/class/leds/mode_led:green/brightness"},
    {LED_MODE_BLUE_ATTR_ONOFF,      "/sys/class/leds/mode_led:blue/brightness"},
    
};
STATIC upstate_info_t upstateinfo_st[] = 
{  
    /* LED��ʾ���� */
    /* ͨ������״̬��ʾ���� */
    {DLOAD_STATE_NORMAL_RUNNING,   LED_READ,   {.str_value  = "0"},                  SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LED_WRITE,  {.path_index = LED_MODE_RED_ATTR_ONOFF},       SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LED_READ,   {.str_value  = "0"},                  SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LED_WRITE,  {.path_index = LED_MODE_GREEN_ATTR_ONOFF},       SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LED_READ,   {.str_value  = "255"},                 SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LED_WRITE,  {.path_index = LED_MODE_BLUE_ATTR_ONOFF},       SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LED_READ,   {.str_value  = "0"},                 SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LED_WRITE,  {.path_index = LED_SIGNAL1_ATTR_ONOFF},    SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LED_READ,   {.str_value  = "255"},               SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LED_WRITE,  {.path_index = LED_SIGNAL2_ATTR_ONOFF},      SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LED_READ,   {.str_value  = "255"},                 SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LED_WRITE,  {.path_index = LED_SIGNAL3_ATTR_ONOFF},    SHOW_YES_DELAY},

    /* �����ɹ�״̬��ʾ���� */
    {DLOAD_STATE_NORMAL_SUCCESS,   LED_READ,   {.str_value  = "255"},                  SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_SUCCESS,   LED_WRITE,  {.path_index = LED_MODE_RED_ATTR_ONOFF},       SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_SUCCESS,   LED_READ,   {.str_value  = "255"},                  SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_SUCCESS,   LED_WRITE,  {.path_index = LED_MODE_GREEN_ATTR_ONOFF},       SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_SUCCESS,   LED_READ,   {.str_value  = "255"},                 SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_SUCCESS,   LED_WRITE,  {.path_index = LED_MODE_BLUE_ATTR_ONOFF},       SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_SUCCESS,   LED_READ,   {.str_value  = "0"},                 SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_SUCCESS,   LED_WRITE,  {.path_index = LED_SIGNAL1_ATTR_ONOFF},    SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_SUCCESS,   LED_READ,   {.str_value  = "255"},               SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_SUCCESS,   LED_WRITE,  {.path_index = LED_SIGNAL2_ATTR_ONOFF},      SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_SUCCESS,   LED_READ,   {.str_value  = "255"},                 SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_SUCCESS,   LED_WRITE,  {.path_index = LED_SIGNAL3_ATTR_ONOFF},    SHOW_YES_DELAY},

    /* ����ʧ��״̬��ʾ���� */
    {DLOAD_STATE_NORMAL_FAIL,   LED_READ,   {.str_value  = "255"},                  SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_FAIL,   LED_WRITE,  {.path_index = LED_MODE_RED_ATTR_ONOFF},       SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_FAIL,   LED_READ,   {.str_value  = "0"},                  SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_FAIL,   LED_WRITE,  {.path_index = LED_MODE_GREEN_ATTR_ONOFF},       SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_FAIL,   LED_READ,   {.str_value  = "0"},                 SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_FAIL,   LED_WRITE,  {.path_index = LED_MODE_BLUE_ATTR_ONOFF},       SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_FAIL,   LED_READ,   {.str_value  = "0"},                 SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_FAIL,   LED_WRITE,  {.path_index = LED_SIGNAL1_ATTR_ONOFF},    SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_FAIL,   LED_READ,   {.str_value  = "255"},               SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_FAIL,   LED_WRITE,  {.path_index = LED_SIGNAL2_ATTR_ONOFF},      SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_FAIL,   LED_READ,   {.str_value  = "255"},                 SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_FAIL,   LED_WRITE,  {.path_index = LED_SIGNAL3_ATTR_ONOFF},    SHOW_YES_DELAY},

    {DLOAD_STATE_FAIL,   LED_READ,   {.str_value  = "255"},                  SHOW_NOT_DELAY},
    {DLOAD_STATE_FAIL,   LED_WRITE,  {.path_index = LED_MODE_RED_ATTR_ONOFF},       SHOW_NOT_DELAY},
    {DLOAD_STATE_FAIL,   LED_READ,   {.str_value  = "0"},                  SHOW_NOT_DELAY},
    {DLOAD_STATE_FAIL,   LED_WRITE,  {.path_index = LED_MODE_GREEN_ATTR_ONOFF},       SHOW_NOT_DELAY},
    {DLOAD_STATE_FAIL,   LED_READ,   {.str_value  = "0"},                 SHOW_NOT_DELAY},
    {DLOAD_STATE_FAIL,   LED_WRITE,  {.path_index = LED_MODE_BLUE_ATTR_ONOFF},       SHOW_NOT_DELAY},
    {DLOAD_STATE_FAIL,   LED_READ,   {.str_value  = "0"},                 SHOW_NOT_DELAY},
    {DLOAD_STATE_FAIL,   LED_WRITE,  {.path_index = LED_SIGNAL1_ATTR_ONOFF},    SHOW_NOT_DELAY},
    {DLOAD_STATE_FAIL,   LED_READ,   {.str_value  = "255"},               SHOW_NOT_DELAY},
    {DLOAD_STATE_FAIL,   LED_WRITE,  {.path_index = LED_SIGNAL2_ATTR_ONOFF},      SHOW_NOT_DELAY},
    {DLOAD_STATE_FAIL,   LED_READ,   {.str_value  = "255"},                 SHOW_NOT_DELAY},
    {DLOAD_STATE_FAIL,   LED_WRITE,  {.path_index = LED_SIGNAL3_ATTR_ONOFF},    SHOW_YES_DELAY},

};
#elif (FEATURE_ON == MBB_DLOAD_STICK || FEATURE_ON == MBB_DLOAD_HILINK || FEATURE_ON == MBB_DLOAD_WINGLE)
STATIC path_info_t pathinfo_st[] = 
{
    {LED_LTE_GREEN_ATTR_ONOFF,       "/sys/class/leds/lte_led:green/trigger"},
    {LED_LTE_GREEN_ATTR_DELAY_ON,    "/sys/class/leds/lte_led:green/delay_on"},
    {LED_LTE_GREEN_ATTR_DELAY_OFF,   "/sys/class/leds/lte_led:green/delay_off"},
    
};

STATIC upstate_info_t upstateinfo_st[] = 
{
    /* ����״̬��ʾ���� */
    {DLOAD_STATE_NORMAL_IDLE,   LED_READ,   {.str_value  = "timer"},                       SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_IDLE,   LED_WRITE,  {.path_index = LED_LTE_GREEN_ATTR_ONOFF},        SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_IDLE,   LED_READ,   {.str_value  = "100"},                           SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_IDLE,   LED_WRITE,  {.path_index = LED_LTE_GREEN_ATTR_DELAY_OFF},    SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_IDLE,   LED_READ,   {.str_value  = "100"},                           SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_IDLE,   LED_WRITE,  {.path_index = LED_LTE_GREEN_ATTR_DELAY_ON},     SHOW_NOT_DELAY},

    /* ͨ������״̬��ʾ���� */
    {DLOAD_STATE_NORMAL_RUNNING,   LED_READ,   {.str_value  = "timer"},                    SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LED_WRITE,  {.path_index = LED_LTE_GREEN_ATTR_ONOFF},     SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LED_READ,   {.str_value  = "100"},                        SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LED_WRITE,  {.path_index = LED_LTE_GREEN_ATTR_DELAY_OFF}, SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LED_READ,   {.str_value  = "100"},                        SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LED_WRITE,  {.path_index = LED_LTE_GREEN_ATTR_DELAY_ON},  SHOW_NOT_DELAY},

    /* �����ɹ�״̬��ʾ���� */
    {DLOAD_STATE_NORMAL_SUCCESS,   LED_READ,   {.str_value  = "timer"},                    SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_SUCCESS,   LED_WRITE,  {.path_index = LED_LTE_GREEN_ATTR_ONOFF},     SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_SUCCESS,   LED_READ,   {.str_value  = "100"},                        SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_SUCCESS,   LED_WRITE,  {.path_index = LED_LTE_GREEN_ATTR_DELAY_OFF}, SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_SUCCESS,   LED_READ,   {.str_value  = "100"},                        SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_SUCCESS,   LED_WRITE,  {.path_index = LED_LTE_GREEN_ATTR_DELAY_ON},  SHOW_NOT_DELAY},

    /* ����ʧ��״̬��ʾ���� */
    {DLOAD_STATE_NORMAL_FAIL,   LED_READ,   {.str_value  = "timer"},                       SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_FAIL,   LED_WRITE,  {.path_index = LED_LTE_GREEN_ATTR_ONOFF},        SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_FAIL,   LED_READ,   {.str_value  = "100"},                           SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_FAIL,   LED_WRITE,  {.path_index = LED_LTE_GREEN_ATTR_DELAY_OFF},    SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_FAIL,   LED_READ,   {.str_value  = "100"},                           SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_FAIL,   LED_WRITE,  {.path_index = LED_LTE_GREEN_ATTR_DELAY_ON},     SHOW_NOT_DELAY},

};

STATIC path_info_t pathinfo_burn_st[] = 
{
    {LED_LTE_GREEN_ATTR_ONOFF,       "/sys/class/leds/lte_led:green/trigger"},
    {LED_LTE_GREEN_ATTR_DELAY_ON,    "/sys/class/leds/lte_led:green/delay_on"},
    {LED_LTE_GREEN_ATTR_DELAY_OFF,   "/sys/class/leds/lte_led:green/delay_off"},
    {LED_LTE_BLUE_ATTR_ONOFF,        "/sys/class/leds/lte_led:blue/trigger"},
    {LED_LTE_BLUE_ATTR_DELAY_ON,     "/sys/class/leds/lte_led:blue/delay_on"},
    {LED_LTE_BLUE_ATTR_DELAY_OFF,    "/sys/class/leds/lte_led:blue/delay_off"},
    {LED_LTE_RED_ATTR_ONOFF,         "/sys/class/leds/lte_led:red/trigger"},
    {LED_LTE_RED_ATTR_DELAY_ON,      "/sys/class/leds/lte_led:red/delay_on"},
    {LED_LTE_RED_ATTR_DELAY_OFF,     "/sys/class/leds/lte_led:red/delay_off"},
    
};

STATIC upstate_info_t upstateinfo_burn_st[] = 
{
    /* ����״̬��ʾ���� */
    {DLOAD_STATE_NORMAL_IDLE,   LED_READ,   {.str_value  = "timer"},                       SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_IDLE,   LED_WRITE,  {.path_index = LED_LTE_GREEN_ATTR_ONOFF},        SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_IDLE,   LED_READ,   {.str_value  = "100"},                           SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_IDLE,   LED_WRITE,  {.path_index = LED_LTE_GREEN_ATTR_DELAY_OFF},    SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_IDLE,   LED_READ,   {.str_value  = "100"},                           SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_IDLE,   LED_WRITE,  {.path_index = LED_LTE_GREEN_ATTR_DELAY_ON},     SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_IDLE,   LED_READ,   {.str_value  = "timer"},                       SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_IDLE,   LED_WRITE,  {.path_index = LED_LTE_BLUE_ATTR_ONOFF},        SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_IDLE,   LED_READ,   {.str_value  = "100"},                           SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_IDLE,   LED_WRITE,  {.path_index = LED_LTE_BLUE_ATTR_DELAY_ON},    SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_IDLE,   LED_READ,   {.str_value  = "100"},                           SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_IDLE,   LED_WRITE,  {.path_index = LED_LTE_BLUE_ATTR_DELAY_OFF},     SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_IDLE,   LED_READ,   {.str_value  = "timer"},                       SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_IDLE,   LED_WRITE,  {.path_index = LED_LTE_RED_ATTR_ONOFF},        SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_IDLE,   LED_READ,   {.str_value  = "100"},                           SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_IDLE,   LED_WRITE,  {.path_index = LED_LTE_RED_ATTR_DELAY_ON},    SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_IDLE,   LED_READ,   {.str_value  = "100"},                           SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_IDLE,   LED_WRITE,  {.path_index = LED_LTE_RED_ATTR_DELAY_OFF},     SHOW_NOT_DELAY},    

    /* ͨ������״̬��ʾ���� */
    {DLOAD_STATE_NORMAL_RUNNING,   LED_READ,   {.str_value  = "timer"},                    SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LED_WRITE,  {.path_index = LED_LTE_GREEN_ATTR_ONOFF},     SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LED_READ,   {.str_value  = "100"},                        SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LED_WRITE,  {.path_index = LED_LTE_GREEN_ATTR_DELAY_OFF}, SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LED_READ,   {.str_value  = "100"},                        SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LED_WRITE,  {.path_index = LED_LTE_GREEN_ATTR_DELAY_ON},  SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LED_READ,   {.str_value  = "timer"},                       SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LED_WRITE,  {.path_index = LED_LTE_BLUE_ATTR_ONOFF},        SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LED_READ,   {.str_value  = "100"},                           SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LED_WRITE,  {.path_index = LED_LTE_BLUE_ATTR_DELAY_ON},    SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LED_READ,   {.str_value  = "100"},                           SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LED_WRITE,  {.path_index = LED_LTE_BLUE_ATTR_DELAY_OFF},     SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LED_READ,   {.str_value  = "timer"},                       SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LED_WRITE,  {.path_index = LED_LTE_RED_ATTR_ONOFF},        SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LED_READ,   {.str_value  = "100"},                           SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LED_WRITE,  {.path_index = LED_LTE_RED_ATTR_DELAY_ON},    SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LED_READ,   {.str_value  = "100"},                           SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LED_WRITE,  {.path_index = LED_LTE_RED_ATTR_DELAY_OFF},     SHOW_NOT_DELAY},    

    /* �����ɹ�״̬��ʾ���� */
    {DLOAD_STATE_NORMAL_SUCCESS,   LED_READ,   {.str_value  = "timer"},                    SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_SUCCESS,   LED_WRITE,  {.path_index = LED_LTE_GREEN_ATTR_ONOFF},     SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_SUCCESS,   LED_READ,   {.str_value  = "100"},                        SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_SUCCESS,   LED_WRITE,  {.path_index = LED_LTE_GREEN_ATTR_DELAY_OFF}, SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_SUCCESS,   LED_READ,   {.str_value  = "100"},                        SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_SUCCESS,   LED_WRITE,  {.path_index = LED_LTE_GREEN_ATTR_DELAY_ON},  SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_SUCCESS,   LED_READ,   {.str_value  = "timer"},                       SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_SUCCESS,   LED_WRITE,  {.path_index = LED_LTE_BLUE_ATTR_ONOFF},        SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_SUCCESS,   LED_READ,   {.str_value  = "100"},                           SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_SUCCESS,   LED_WRITE,  {.path_index = LED_LTE_BLUE_ATTR_DELAY_ON},    SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_SUCCESS,   LED_READ,   {.str_value  = "100"},                           SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_SUCCESS,   LED_WRITE,  {.path_index = LED_LTE_BLUE_ATTR_DELAY_OFF},     SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_SUCCESS,   LED_READ,   {.str_value  = "timer"},                       SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_SUCCESS,   LED_WRITE,  {.path_index = LED_LTE_RED_ATTR_ONOFF},        SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_SUCCESS,   LED_READ,   {.str_value  = "100"},                           SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_SUCCESS,   LED_WRITE,  {.path_index = LED_LTE_RED_ATTR_DELAY_ON},    SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_SUCCESS,   LED_READ,   {.str_value  = "100"},                           SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_SUCCESS,   LED_WRITE,  {.path_index = LED_LTE_RED_ATTR_DELAY_OFF},     SHOW_NOT_DELAY},    

    /* ����ʧ��״̬��ʾ���� */
    {DLOAD_STATE_NORMAL_FAIL,   LED_READ,   {.str_value  = "timer"},                       SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_FAIL,   LED_WRITE,  {.path_index = LED_LTE_GREEN_ATTR_ONOFF},        SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_FAIL,   LED_READ,   {.str_value  = "100"},                           SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_FAIL,   LED_WRITE,  {.path_index = LED_LTE_GREEN_ATTR_DELAY_OFF},    SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_FAIL,   LED_READ,   {.str_value  = "100"},                           SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_FAIL,   LED_WRITE,  {.path_index = LED_LTE_GREEN_ATTR_DELAY_ON},     SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_FAIL,   LED_READ,   {.str_value  = "timer"},                       SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_FAIL,   LED_WRITE,  {.path_index = LED_LTE_BLUE_ATTR_ONOFF},        SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_FAIL,   LED_READ,   {.str_value  = "100"},                           SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_FAIL,   LED_WRITE,  {.path_index = LED_LTE_BLUE_ATTR_DELAY_ON},    SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_FAIL,   LED_READ,   {.str_value  = "100"},                           SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_FAIL,   LED_WRITE,  {.path_index = LED_LTE_BLUE_ATTR_DELAY_OFF},     SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_FAIL,   LED_READ,   {.str_value  = "timer"},                       SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_FAIL,   LED_WRITE,  {.path_index = LED_LTE_RED_ATTR_ONOFF},        SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_FAIL,   LED_READ,   {.str_value  = "100"},                           SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_FAIL,   LED_WRITE,  {.path_index = LED_LTE_RED_ATTR_DELAY_ON},    SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_FAIL,   LED_READ,   {.str_value  = "100"},                           SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_FAIL,   LED_WRITE,  {.path_index = LED_LTE_RED_ATTR_DELAY_OFF},     SHOW_NOT_DELAY},    
};
#elif (FEATURE_ON == MBB_DLOAD_E5)
#ifdef BSP_CONFIG_BOARD_E5_E5578
#define SHOW_YES_DELAY         (1000000) /*��ʱ1s*/
STATIC path_info_t pathinfo_st[] = 
{
    {LCD_DEV_NAME,          "/dev/fb0"},
    {LCD_PIC_SUCCESS,       "/pic/%d_%d/upgrade_success"},
    {LCD_PIC_FAILED,        "/pic/%d_%d/upgrade_fail"},
    {LCD_PIC_AUTOUG,        "/pic/%d_%d/auto_update"},
    {LCD_PIC_LOWPOWER,      "/pic/%d_%d/lowpower"},
    {LCD_PIC_INSTALL0,      "/pic/%d_%d/installing_0"},
    {LCD_PIC_INSTALL1,      "/pic/%d_%d/installing_1"},
    {LCD_PIC_INSTALL2,      "/pic/%d_%d/installing_2"},
    {LCD_PIC_INSTALL3,      "/pic/%d_%d/installing_3"},
    {LCD_PIC_INSTALL4,      "/pic/%d_%d/installing_4"},
    {LCD_PIC_INSTALL5,      "/pic/%d_%d/installing_5"},
    {LCD_PIC_INSTALL6,      "/pic/%d_%d/installing_6"},
    {LCD_PIC_INSTALL7,      "/pic/%d_%d/installing_7"},
    {LCD_PIC_INSTALL8,      "/pic/%d_%d/installing_8"},
    {LCD_PIC_INSTALL9,      "/pic/%d_%d/installing_9"},
    {LCD_PIC_INSTALL10,     "/pic/%d_%d/installing_10"},
    {LCD_PIC_INSTALL11,     "/pic/%d_%d/installing_11"},
};

/* LCD ����״̬�б� */
STATIC upstate_info_t upstateinfo_st[] = 
{
    /* LCD��ʾ���� */
    /* ����״̬��ʾ���� */
    {DLOAD_STATE_NORMAL_IDLE,      LCD_READ,   {.path_index = LCD_PIC_INSTALL0},     SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_IDLE,      LCD_WRITE,  {.path_index = LCD_DEV_NAME},         SHOW_YES_DELAY},
    
    /* ͨ������״̬��ʾ���� */
    {DLOAD_STATE_NORMAL_RUNNING,   LCD_READ,   {.path_index = LCD_PIC_INSTALL0},     SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LCD_WRITE,  {.path_index = LCD_DEV_NAME},         SHOW_YES_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LCD_READ,   {.path_index = LCD_PIC_INSTALL1},     SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LCD_WRITE,  {.path_index = LCD_DEV_NAME},         SHOW_YES_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LCD_READ,   {.path_index = LCD_PIC_INSTALL2},     SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LCD_WRITE,  {.path_index = LCD_DEV_NAME},         SHOW_YES_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LCD_READ,   {.path_index = LCD_PIC_INSTALL3},     SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LCD_WRITE,  {.path_index = LCD_DEV_NAME},         SHOW_YES_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LCD_READ,   {.path_index = LCD_PIC_INSTALL4},     SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LCD_WRITE,  {.path_index = LCD_DEV_NAME},         SHOW_YES_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LCD_READ,   {.path_index = LCD_PIC_INSTALL5},     SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LCD_WRITE,  {.path_index = LCD_DEV_NAME},         SHOW_YES_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LCD_READ,   {.path_index = LCD_PIC_INSTALL6},     SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LCD_WRITE,  {.path_index = LCD_DEV_NAME},         SHOW_YES_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LCD_READ,   {.path_index = LCD_PIC_INSTALL7},     SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LCD_WRITE,  {.path_index = LCD_DEV_NAME},         SHOW_YES_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LCD_READ,   {.path_index = LCD_PIC_INSTALL8},     SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LCD_WRITE,  {.path_index = LCD_DEV_NAME},         SHOW_YES_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LCD_READ,   {.path_index = LCD_PIC_INSTALL9},     SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LCD_WRITE,  {.path_index = LCD_DEV_NAME},         SHOW_YES_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LCD_READ,   {.path_index = LCD_PIC_INSTALL10},    SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LCD_WRITE,  {.path_index = LCD_DEV_NAME},         SHOW_YES_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LCD_READ,   {.path_index = LCD_PIC_INSTALL11},    SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LCD_WRITE,  {.path_index = LCD_DEV_NAME},         SHOW_YES_DELAY},

    /* ������������״̬��ʾ���� */
    {DLOAD_STATE_AUTOUG_RUNNING,   LCD_READ,   {.path_index = LCD_PIC_AUTOUG},       SHOW_NOT_DELAY},
    {DLOAD_STATE_AUTOUG_RUNNING,   LCD_WRITE,  {.path_index = LCD_DEV_NAME},         SHOW_YES_DELAY},

    /* �͵�״̬��ʾ���� */
    {DLOAD_STATE_NORMAL_LOW_POWER, LCD_READ,   {.path_index = LCD_PIC_LOWPOWER},     SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_LOW_POWER, LCD_WRITE,  {.path_index = LCD_DEV_NAME},         SHOW_YES_DELAY},

    /* �����ɹ�״̬��ʾ���� */
    {DLOAD_STATE_NORMAL_SUCCESS,   LCD_READ,   {.path_index = LCD_PIC_SUCCESS},      SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_SUCCESS,   LCD_WRITE,  {.path_index = LCD_DEV_NAME},         SHOW_YES_DELAY},

    /* ����ʧ��״̬��ʾ���� */
    {DLOAD_STATE_NORMAL_FAIL,      LCD_READ,   {.path_index = LCD_PIC_FAILED},       SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_FAIL,      LCD_WRITE,  {.path_index = LCD_DEV_NAME},         SHOW_YES_DELAY},

};
#else
STATIC path_info_t pathinfo_st[] = 
{
    {LED_LTE_GREEN_ATTR_ONOFF,       "/sys/class/leds/lte_led:green/trigger"},
    {LED_LTE_GREEN_ATTR_DELAY_ON,    "/sys/class/leds/lte_led:green/delay_on"},
    {LED_LTE_GREEN_ATTR_DELAY_OFF,   "/sys/class/leds/lte_led:green/delay_off"},
    {LED_BAT_GREEN_ATTR_ONOFF,       "/sys/class/leds/bat_led:green/trigger"},
    {LED_BAT_GREEN_ATTR_DELAY_ON,    "/sys/class/leds/bat_led:green/delay_on"},
    {LED_BAT_GREEN_ATTR_DELAY_OFF,   "/sys/class/leds/bat_led:green/delay_off"},
    
};
STATIC upstate_info_t upstateinfo_st[] = 
{
    /* ����״̬��ʾ���� */
    {DLOAD_STATE_NORMAL_IDLE,   LED_READ,   {.str_value  = "timer"},                     SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_IDLE,   LED_WRITE,  {.path_index = LED_LTE_GREEN_ATTR_ONOFF},    SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_IDLE,   LED_READ,   {.str_value  = "500"},                       SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_IDLE,   LED_WRITE,  {.path_index = LED_LTE_GREEN_ATTR_DELAY_OFF},SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_IDLE,   LED_READ,   {.str_value  = "500"},                       SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_IDLE,   LED_WRITE,  {.path_index = LED_LTE_GREEN_ATTR_DELAY_ON}, SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_IDLE,   LED_READ,   {.str_value  = "timer"},                     SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_IDLE,   LED_WRITE,  {.path_index = LED_BAT_GREEN_ATTR_ONOFF},    SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_IDLE,   LED_READ,   {.str_value  = "500"},                       SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_IDLE,   LED_WRITE,  {.path_index = LED_BAT_GREEN_ATTR_DELAY_OFF},SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_IDLE,   LED_READ,   {.str_value  = "500"},                       SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_IDLE,   LED_WRITE,  {.path_index = LED_BAT_GREEN_ATTR_DELAY_ON}, SHOW_NOT_DELAY},

    /* ͨ������״̬��ʾ���� */
    {DLOAD_STATE_NORMAL_RUNNING,   LED_READ,   {.str_value  = "timer"},                     SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LED_WRITE,  {.path_index = LED_LTE_GREEN_ATTR_ONOFF},    SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LED_READ,   {.str_value  = "500"},                       SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LED_WRITE,  {.path_index = LED_LTE_GREEN_ATTR_DELAY_OFF},SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LED_READ,   {.str_value  = "500"},                       SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LED_WRITE,  {.path_index = LED_LTE_GREEN_ATTR_DELAY_ON}, SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LED_READ,   {.str_value  = "timer"},                     SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LED_WRITE,  {.path_index = LED_BAT_GREEN_ATTR_ONOFF},    SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LED_READ,   {.str_value  = "500"},                       SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LED_WRITE,  {.path_index = LED_BAT_GREEN_ATTR_DELAY_OFF},SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LED_READ,   {.str_value  = "500"},                       SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LED_WRITE,  {.path_index = LED_BAT_GREEN_ATTR_DELAY_ON}, SHOW_NOT_DELAY},

    /* �����ɹ�״̬��ʾ���� */
    {DLOAD_STATE_NORMAL_SUCCESS,   LED_READ,   {.str_value  = "timer"},                     SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_SUCCESS,   LED_WRITE,  {.path_index = LED_LTE_GREEN_ATTR_ONOFF},    SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_SUCCESS,   LED_READ,   {.str_value  = "500"},                       SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_SUCCESS,   LED_WRITE,  {.path_index = LED_LTE_GREEN_ATTR_DELAY_OFF},SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_SUCCESS,   LED_READ,   {.str_value  = "500"},                       SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_SUCCESS,   LED_WRITE,  {.path_index = LED_LTE_GREEN_ATTR_DELAY_ON}, SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_SUCCESS,   LED_READ,   {.str_value  = "timer"},                     SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_SUCCESS,   LED_WRITE,  {.path_index = LED_BAT_GREEN_ATTR_ONOFF},    SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_SUCCESS,   LED_READ,   {.str_value  = "500"},                       SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_SUCCESS,   LED_WRITE,  {.path_index = LED_BAT_GREEN_ATTR_DELAY_OFF},SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_SUCCESS,   LED_READ,   {.str_value  = "500"},                       SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_SUCCESS,   LED_WRITE,  {.path_index = LED_BAT_GREEN_ATTR_DELAY_ON}, SHOW_NOT_DELAY},

    /* ����ʧ��״̬��ʾ���� */
    {DLOAD_STATE_NORMAL_FAIL,   LED_READ,   {.str_value  = "timer"},                     SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_FAIL,   LED_WRITE,  {.path_index = LED_LTE_GREEN_ATTR_ONOFF},    SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_FAIL,   LED_READ,   {.str_value  = "500"},                       SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_FAIL,   LED_WRITE,  {.path_index = LED_LTE_GREEN_ATTR_DELAY_OFF},SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_FAIL,   LED_READ,   {.str_value  = "500"},                       SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_FAIL,   LED_WRITE,  {.path_index = LED_LTE_GREEN_ATTR_DELAY_ON}, SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_FAIL,   LED_READ,   {.str_value  = "timer"},                     SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_FAIL,   LED_WRITE,  {.path_index = LED_BAT_GREEN_ATTR_ONOFF},    SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_FAIL,   LED_READ,   {.str_value  = "500"},                       SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_FAIL,   LED_WRITE,  {.path_index = LED_BAT_GREEN_ATTR_DELAY_OFF},SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_FAIL,   LED_READ,   {.str_value  = "500"},                       SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_FAIL,   LED_WRITE,  {.path_index = LED_BAT_GREEN_ATTR_DELAY_ON}, SHOW_NOT_DELAY},


};
#endif
#else
#define SHOW_YES_DELAY         (1000000) /*��ʱ1s*/

STATIC path_info_t pathinfo_st[] = 
{
    {LCD_DEV_NAME,          "/dev/fb0"},
    {LCD_PIC_SUCCESS,       "/pic/%d_%d/upgrade_success"},
    {LCD_PIC_FAILED,        "/pic/%d_%d/upgrade_fail"},
    {LCD_PIC_AUTOUG,        "/pic/%d_%d/auto_update"},
    {LCD_PIC_LOWPOWER,      "/pic/%d_%d/lowpower"},
    {LCD_PIC_INSTALL0,      "/pic/%d_%d/installing_0"},
    {LCD_PIC_INSTALL1,      "/pic/%d_%d/installing_1"},
    {LCD_PIC_INSTALL2,      "/pic/%d_%d/installing_2"},
    {LCD_PIC_INSTALL3,      "/pic/%d_%d/installing_3"},
    {LCD_PIC_INSTALL4,      "/pic/%d_%d/installing_4"},
    {LCD_PIC_INSTALL5,      "/pic/%d_%d/installing_5"},
    {LCD_PIC_INSTALL6,      "/pic/%d_%d/installing_6"},
    {LCD_PIC_INSTALL7,      "/pic/%d_%d/installing_7"},
    {LCD_PIC_INSTALL8,      "/pic/%d_%d/installing_8"},
    {LCD_PIC_INSTALL9,      "/pic/%d_%d/installing_9"},
    {LCD_PIC_INSTALL10,     "/pic/%d_%d/installing_10"},
    {LCD_PIC_INSTALL11,     "/pic/%d_%d/installing_11"},
    {LED_ATTR_ONOFF,        "/sys/class/leds/test-backlight/trigger"},
    {LED_ATTR_BRIGHT,       "/sys/class/leds/test-backlight/brightness"},
    {LED_ATTR_DELAY_ON,     "/sys/class/leds/test-backlight/delay_on"},
    {LED_ATTR_DELAY_OFF,    "/sys/class/leds/test-backlight/delay_off"},
};

/* LCD ����״̬�б� */
STATIC upstate_info_t upstateinfo_st[] = 
{
    /* LCD��ʾ���� */
    /* ����״̬��ʾ���� */
    {DLOAD_STATE_NORMAL_IDLE,      LCD_READ,   {.path_index = LCD_PIC_INSTALL0},     SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_IDLE,      LCD_WRITE,  {.path_index = LCD_DEV_NAME},         SHOW_YES_DELAY},
    
    /* ͨ������״̬��ʾ���� */
    {DLOAD_STATE_NORMAL_RUNNING,   LCD_READ,   {.path_index = LCD_PIC_INSTALL0},     SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LCD_WRITE,  {.path_index = LCD_DEV_NAME},         SHOW_YES_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LCD_READ,   {.path_index = LCD_PIC_INSTALL1},     SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LCD_WRITE,  {.path_index = LCD_DEV_NAME},         SHOW_YES_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LCD_READ,   {.path_index = LCD_PIC_INSTALL2},     SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LCD_WRITE,  {.path_index = LCD_DEV_NAME},         SHOW_YES_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LCD_READ,   {.path_index = LCD_PIC_INSTALL3},     SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LCD_WRITE,  {.path_index = LCD_DEV_NAME},         SHOW_YES_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LCD_READ,   {.path_index = LCD_PIC_INSTALL4},     SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LCD_WRITE,  {.path_index = LCD_DEV_NAME},         SHOW_YES_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LCD_READ,   {.path_index = LCD_PIC_INSTALL5},     SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LCD_WRITE,  {.path_index = LCD_DEV_NAME},         SHOW_YES_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LCD_READ,   {.path_index = LCD_PIC_INSTALL6},     SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LCD_WRITE,  {.path_index = LCD_DEV_NAME},         SHOW_YES_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LCD_READ,   {.path_index = LCD_PIC_INSTALL7},     SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LCD_WRITE,  {.path_index = LCD_DEV_NAME},         SHOW_YES_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LCD_READ,   {.path_index = LCD_PIC_INSTALL8},     SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LCD_WRITE,  {.path_index = LCD_DEV_NAME},         SHOW_YES_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LCD_READ,   {.path_index = LCD_PIC_INSTALL9},     SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LCD_WRITE,  {.path_index = LCD_DEV_NAME},         SHOW_YES_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LCD_READ,   {.path_index = LCD_PIC_INSTALL10},    SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LCD_WRITE,  {.path_index = LCD_DEV_NAME},         SHOW_YES_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LCD_READ,   {.path_index = LCD_PIC_INSTALL11},    SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LCD_WRITE,  {.path_index = LCD_DEV_NAME},         SHOW_YES_DELAY},

    /* ������������״̬��ʾ���� */
    {DLOAD_STATE_AUTOUG_RUNNING,   LCD_READ,   {.path_index = LCD_PIC_AUTOUG},       SHOW_NOT_DELAY},
    {DLOAD_STATE_AUTOUG_RUNNING,   LCD_WRITE,  {.path_index = LCD_DEV_NAME},         SHOW_YES_DELAY},

    /* �͵�״̬��ʾ���� */
    {DLOAD_STATE_NORMAL_LOW_POWER, LCD_READ,   {.path_index = LCD_PIC_LOWPOWER},     SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_LOW_POWER, LCD_WRITE,  {.path_index = LCD_DEV_NAME},         SHOW_YES_DELAY},

    /* �����ɹ�״̬��ʾ���� */
    {DLOAD_STATE_NORMAL_SUCCESS,   LCD_READ,   {.path_index = LCD_PIC_SUCCESS},      SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_SUCCESS,   LCD_WRITE,  {.path_index = LCD_DEV_NAME},         SHOW_YES_DELAY},

    /* ����ʧ��״̬��ʾ���� */
    {DLOAD_STATE_NORMAL_FAIL,      LCD_READ,   {.path_index = LCD_PIC_FAILED},       SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_FAIL,      LCD_WRITE,  {.path_index = LCD_DEV_NAME},         SHOW_YES_DELAY},

    /* LED��ʾ���� */
    /* ����״̬��ʾ���� */
    {DLOAD_STATE_NORMAL_IDLE,      LED_READ,   {.str_value  = "1"},                  SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_IDLE,      LED_WRITE,  {.path_index = LED_ATTR_ONOFF},       SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_IDLE,      LED_READ,   {.str_value  = "3000"},               SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_IDLE,      LED_WRITE,  {.path_index = LED_ATTR_BRIGHT},      SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_IDLE,      LED_READ,   {.str_value  = "50"},                 SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_IDLE,      LED_WRITE,  {.path_index = LED_ATTR_DELAY_ON},    SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_IDLE,      LED_READ,   {.str_value  = "50"},                 SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_IDLE,      LED_WRITE,  {.path_index = LED_ATTR_DELAY_OFF},   SHOW_NOT_DELAY},

    {DLOAD_STATE_FAIL,      LED_READ,   {.str_value  = "1"},                  SHOW_NOT_DELAY},
    {DLOAD_STATE_FAIL,      LED_WRITE,  {.path_index = LED_ATTR_ONOFF},       SHOW_NOT_DELAY},
    {DLOAD_STATE_FAIL,      LED_READ,   {.str_value  = "3000"},               SHOW_NOT_DELAY},
    {DLOAD_STATE_FAIL,      LED_WRITE,  {.path_index = LED_ATTR_BRIGHT},      SHOW_NOT_DELAY},
    {DLOAD_STATE_FAIL,      LED_READ,   {.str_value  = "50"},                 SHOW_NOT_DELAY},
    {DLOAD_STATE_FAIL,      LED_WRITE,  {.path_index = LED_ATTR_DELAY_ON},    SHOW_NOT_DELAY},
    {DLOAD_STATE_FAIL,      LED_READ,   {.str_value  = "50"},                 SHOW_NOT_DELAY},
    {DLOAD_STATE_FAIL,      LED_WRITE,  {.path_index = LED_ATTR_DELAY_OFF},   SHOW_NOT_DELAY},

    /* ͨ������״̬��ʾ���� */
    {DLOAD_STATE_NORMAL_RUNNING,   LED_READ,   {.str_value  = "1"},                  SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LED_WRITE,  {.path_index = LED_ATTR_ONOFF},       SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LED_READ,   {.str_value  = "3000"},               SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LED_WRITE,  {.path_index = LED_ATTR_BRIGHT},      SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LED_READ,   {.str_value  = "50"},                 SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LED_WRITE,  {.path_index = LED_ATTR_DELAY_ON},    SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LED_READ,   {.str_value  = "50"},                 SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_RUNNING,   LED_WRITE,  {.path_index = LED_ATTR_DELAY_OFF},   SHOW_NOT_DELAY},

    /* ��������״̬��ʾ���� */
    {DLOAD_STATE_AUTOUG_RUNNING,   LED_READ,   {.str_value  = "1"},                  SHOW_NOT_DELAY},
    {DLOAD_STATE_AUTOUG_RUNNING,   LED_WRITE,  {.path_index = LED_ATTR_ONOFF},       SHOW_NOT_DELAY},
    {DLOAD_STATE_AUTOUG_RUNNING,   LED_READ,   {.str_value  = "3000"},               SHOW_NOT_DELAY},
    {DLOAD_STATE_AUTOUG_RUNNING,   LED_WRITE,  {.path_index = LED_ATTR_BRIGHT},      SHOW_NOT_DELAY},
    {DLOAD_STATE_AUTOUG_RUNNING,   LED_READ,   {.str_value  = "50"},                 SHOW_NOT_DELAY},
    {DLOAD_STATE_AUTOUG_RUNNING,   LED_WRITE,  {.path_index = LED_ATTR_DELAY_ON},    SHOW_NOT_DELAY},
    {DLOAD_STATE_AUTOUG_RUNNING,   LED_READ,   {.str_value  = "50"},                 SHOW_NOT_DELAY},
    {DLOAD_STATE_AUTOUG_RUNNING,   LED_WRITE,  {.path_index = LED_ATTR_DELAY_OFF},   SHOW_NOT_DELAY},

    /* �͵�״̬��ʾ���� */
    {DLOAD_STATE_NORMAL_LOW_POWER, LED_READ,   {.str_value  = "1"},                  SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_LOW_POWER, LED_WRITE,  {.path_index = LED_ATTR_ONOFF},       SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_LOW_POWER, LED_READ,   {.str_value  = "3000"},               SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_LOW_POWER, LED_WRITE,  {.path_index = LED_ATTR_BRIGHT},      SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_LOW_POWER, LED_READ,   {.str_value  = "50"},                 SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_LOW_POWER, LED_WRITE,  {.path_index = LED_ATTR_DELAY_ON},    SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_LOW_POWER, LED_READ,   {.str_value  = "50"},                 SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_LOW_POWER, LED_WRITE,  {.path_index = LED_ATTR_DELAY_OFF},   SHOW_NOT_DELAY},

    /* �����ɹ�״̬��ʾ���� */
    {DLOAD_STATE_NORMAL_SUCCESS,   LED_READ,   {.str_value  = "1"},                  SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_SUCCESS,   LED_WRITE,  {.path_index = LED_ATTR_ONOFF},       SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_SUCCESS,   LED_READ,   {.str_value  = "3000"},               SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_SUCCESS,   LED_WRITE,  {.path_index = LED_ATTR_BRIGHT},      SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_SUCCESS,   LED_READ,   {.str_value  = "50"},                 SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_SUCCESS,   LED_WRITE,  {.path_index = LED_ATTR_DELAY_ON},    SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_SUCCESS,   LED_READ,   {.str_value  = "50"},                 SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_SUCCESS,   LED_WRITE,  {.path_index = LED_ATTR_DELAY_OFF},   SHOW_NOT_DELAY},

    /* ����ʧ��״̬��ʾ���� */
    {DLOAD_STATE_NORMAL_FAIL,      LED_READ,   {.str_value  = "1"},                  SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_FAIL,      LED_WRITE,  {.path_index = LED_ATTR_ONOFF},       SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_FAIL,      LED_READ,   {.str_value  = "3000"},               SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_FAIL,      LED_WRITE,  {.path_index = LED_ATTR_BRIGHT},      SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_FAIL,      LED_READ,   {.str_value  = "50"},                 SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_FAIL,      LED_WRITE,  {.path_index = LED_ATTR_DELAY_ON},    SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_FAIL,      LED_READ,   {.str_value  = "50"},                 SHOW_NOT_DELAY},
    {DLOAD_STATE_NORMAL_FAIL,      LED_WRITE,  {.path_index = LED_ATTR_DELAY_OFF},   SHOW_NOT_DELAY},

};
#endif

/*******************************************************************
  ��������  : get_product_mark
  ��������  : ��ȡ��ƷӲ����ʶ��
  �������  : NA
  �������  : NA
  ��������ֵ: Ӳ����ʶ
********************************************************************/
static uint32  get_product_mark(void)
{
    uint32 hw_main = *(unsigned int *)SHM_MEM_HW_VER_ADDR;
    
    g_product_id = (hw_main >> PRODUCT_OFFSET) & 0xFF;
    g_big_ver = hw_main & 0xFFFF;    
    
    return 0;
    
}

/*******************************************************************
  ��������  : get_product_info
  ��������  : ��ȡ��Ʒ������Ϣ
  �������  : NA
  �������  : NA
  ��������ֵ: ��Ʒ��Ϣ�ṹ
********************************************************************/
product_info_t*  get_product_info(void)
{
    uint32  ret = 0;
    uint32  index = 0;
    uint32  count = 0;
    product_info_t*  local_product_info_st = NULL;

    /* ��Ʒ��ʶ��ȡ */
    ret = get_product_mark();
    if(0 != ret)
    {
        return NULL;
    }
    
    count = sizeof(product_info_st) / sizeof(product_info_t);

    for(index = 0; index < count; index ++)
    {
        local_product_info_st = &product_info_st[index];
        if((g_product_id == (uint8)local_product_info_st->hw_major_id) && 
            (g_big_ver == (uint16)local_product_info_st->hw_minor_id))
        {
            return local_product_info_st;
        }
    }

    DLOAD_PRINT(KERN_ERR, "g_product_id = %u  g_big_ver = %u  product_info is NULL.", g_product_id, g_big_ver);
    
    return NULL;
}

/*******************************************************************
  ��������  : get_dload_feature
  ��������  : ��ȡ��������������Ϣ
  �������  : NA
  �������  : NA
  ��������ֵ: ��Ʒ��Ϣ�ṹ
********************************************************************/
dload_feature_t*  get_dload_feature(void)
{
    uint32  index = 0;
    uint32  count = 0;
    dload_feature_t*  local_dload_feature_st = NULL;
    dload_smem_info_t* smem_info = NULL;
    count = sizeof(dload_feature_st) / sizeof(dload_feature_t);

    smem_info = get_smem_info();

    for(index = 0; index < count; index ++)
    {
        local_dload_feature_st = &dload_feature_st[index];
        if((g_product_id == (uint8)local_dload_feature_st->hw_major_id) && 
            (g_big_ver == (uint16)local_dload_feature_st->hw_minor_id))
        {
            if(SMEM_BURN_UPDATE_FLAG_NUM == *((uint32*)(smem_info->ut_dload_addr)))
            {
                /* Ϊ��Ƭ�汾������Ƭ�汾��ʶ����������ȫУ�� */
                /* ���ݿ���Ƭ�汾�������汾led��ʾ��һ�� */
#if (FEATURE_ON == MBB_DLOAD_STICK || FEATURE_ON == MBB_DLOAD_HILINK || FEATURE_ON == MBB_DLOAD_WINGLE)
                dload_burn = TRUE;
#endif
                local_dload_feature_st->safety_dload = FALSE;
            }
            return local_dload_feature_st;
        }
    }
    
    return NULL;
}

/*******************************************************************
  ��������  : get_show_info
  ��������  : ��ȡpath ��path_indexת��������
  �������  : NA
  �������  : pppath_info_st : pathinfo_st ����ĵ�ַ
                            info_size: pathinfo_st��size
  ��������ֵ: DLOAD_OK    : �ɹ�
              DLOAD_ERR   : ʧ��
********************************************************************/
uint32  get_show_path_info(path_info_t **pppath_info_st, uint32 *info_size)
{
    if(NULL != pppath_info_st)
    {
        if(FALSE == dload_burn)
        {
            *pppath_info_st = pathinfo_st;
        }
#if (FEATURE_ON == MBB_DLOAD_STICK || FEATURE_ON == MBB_DLOAD_HILINK || FEATURE_ON == MBB_DLOAD_WINGLE)
        else
        {
            *pppath_info_st = pathinfo_burn_st;
        }
#endif
    }
    if(NULL != info_size)
    {
        if(FALSE == dload_burn)
        {
            *info_size= sizeof(pathinfo_st) / sizeof(path_info_t);
        }
#if (FEATURE_ON == MBB_DLOAD_STICK || FEATURE_ON == MBB_DLOAD_HILINK || FEATURE_ON == MBB_DLOAD_WINGLE)
        else
        {
            *info_size= sizeof(pathinfo_burn_st) / sizeof(path_info_t);
        }
#endif
    }
    return DLOAD_OK;

}

/*******************************************************************
  ��������  : get_show_info
  ��������  : ��ȡlcd/led������Ϣ
  �������  : NA
  �������  : ppupstate_info : upstateinfo_st ����ĵ�ַ
                            data_len: upstate_info_st��size
  ��������ֵ: DLOAD_OK    : �ɹ�
              DLOAD_ERR   : ʧ��
********************************************************************/
uint32  get_show_info(upstate_info_t **ppupstate_info, uint32 *info_size)
{
    if(NULL != ppupstate_info)
    {
        if(FALSE == dload_burn)
        {
            *ppupstate_info = upstateinfo_st;
        }
#if (FEATURE_ON == MBB_DLOAD_STICK || FEATURE_ON == MBB_DLOAD_HILINK || FEATURE_ON == MBB_DLOAD_WINGLE)
        else
        {
            *ppupstate_info = upstateinfo_burn_st;
        }
#endif
    }
    if(NULL != info_size)
    {
        if(FALSE == dload_burn)
        {
            *info_size= sizeof(upstateinfo_st) / sizeof(upstate_info_t);
        }
#if (FEATURE_ON == MBB_DLOAD_STICK || FEATURE_ON == MBB_DLOAD_HILINK || FEATURE_ON == MBB_DLOAD_WINGLE)
        else
        {
            *info_size= sizeof(upstateinfo_burn_st) / sizeof(upstate_info_t);
        }
#endif
    }
    return DLOAD_OK;

}



#ifdef  __cplusplus
    }
#endif
