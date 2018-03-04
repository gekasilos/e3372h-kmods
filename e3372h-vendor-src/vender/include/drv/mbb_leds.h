/******************************************************************************
  Copyright, 1988-2014, Huawei Tech. Co., Ltd.
  File name:      mbb_leds.h
  Author:         �쳬 x00202188
  Version:        V1.0 
  Date:           �쳬     ��������      2014-7-20
  Description:    MBB LED����ͷ�ļ�
                  ���ļ��ж�����MBBͨ�������õ��ĺ����ͱ������塣
  Others:         ���ļ�����д�����������MBBͨ��������ع���ʹ�ã����������޹ش���
                  д�뱾�ļ���
******************************************************************************/
#ifndef __MBB_LEDS_H__
#define __MBB_LEDS_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <product_config.h>
#include <hi_gpio.h>

#if (FEATURE_ON == MBB_LED)
/**************************************************************************
  ͷ�ļ�����                            
**************************************************************************/

/**************************************************************************
  �궨�� 
**************************************************************************/
#define LEDS_GPIO_DEFSTATE_OFF        0
#define LEDS_GPIO_DEFSTATE_ON        1
#define LEDS_GPIO_DEFSTATE_KEEP        2
#ifndef UNUSED_PARAMETER
/* Suppress unused parameter warning */
#define UNUSED_PARAMETER(x) (void)(x)
#endif
/**************************************************************************
  �������ṹ���� 
**************************************************************************/
/*
 * Generic LED platform data for describing LED names and default triggers.
 */
struct led_info {
    const char    *name;
    const char    *default_trigger;
    int        flags;
};

/* For the leds-gpio driver */
struct gpio_led {
    const char *name;
    const char *default_trigger;
    unsigned     gpio;
    unsigned    active_low : 1;
    unsigned    retain_state_suspended : 1;
    unsigned    default_state : 2;
    /* default_state should be one of LEDS_GPIO_DEFSTATE_(ON|OFF|KEEP) */
};
#define NAME_MAX_SIZE 50    /*������������󳤶�*/

struct led_kernelset_date    /*led-kernel�ӿڣ�����led������Ϣ*/
{
    char name[NAME_MAX_SIZE];
    int statu;
    int flag;   /*�Ʊ�������־*/
};

#if(FEATURE_ON == MBB_LED_GPIO)
/*GPIO ��Ϣ�Լ��ڵ�������ݲ��õ���̬��Ҫ������*/
#ifdef BSP_CONFIG_BOARD_CPE
#define MODE_RED_EN            GPIO_1_23
#define MODE_BLUE_EN           GPIO_2_10
#define MODE_GREEN_EN          GPIO_2_11
#define WIFI_WHITE_EN          GPIO_NULL
#endif
#ifdef BSP_CONFIG_BOARD_E5
#define LTE_RED_EN             GPIO_2_7
#define LTE_GREEN_EN           GPIO_2_6
#endif
#ifdef BSP_CONFIG_BOARD_WINGLE
#define WIFI_GREEN_EN          GPIO_0_2
#define MSG_RED_EN             GPIO_0_5
#endif

static struct gpio_led gpio_exp_leds_config[] = {
#ifdef BSP_CONFIG_BOARD_CPE
    {
        .name = "mode_led:red",
        .default_trigger = "timer",
        .gpio = MODE_RED_EN,
        .active_low = 0,
        .retain_state_suspended = 0,
        .default_state = LEDS_GPIO_DEFSTATE_OFF,
    },
    {
        .name = "mode_led:green",
        .default_trigger = "timer",
        .gpio = MODE_GREEN_EN,
        .active_low = 0,
        .retain_state_suspended = 0,
        .default_state = LEDS_GPIO_DEFSTATE_OFF,
    },
    {
        .name = "mode_led:blue",
        .default_trigger = "timer",
        .gpio = MODE_BLUE_EN,
        .active_low = 0,
        .retain_state_suspended = 0,
        .default_state = LEDS_GPIO_DEFSTATE_OFF,
    },
    {
        .name = "wifi_led:white",
        .default_trigger = "timer",
        .gpio = WIFI_WHITE_EN,
        .active_low = 0,
        .retain_state_suspended = 0,
        .default_state = LEDS_GPIO_DEFSTATE_OFF,
    },
#endif
#ifdef BSP_CONFIG_BOARD_E5
    {
        .name = "lte_led:red",
        .default_trigger = "timer",
        .gpio = LTE_RED_EN,
        .active_low = 0,
        .retain_state_suspended = 0,
        .default_state = LEDS_GPIO_DEFSTATE_OFF,
    },
    {
        .name = "lte_led:green",
        .default_trigger = "timer",
        .gpio = LTE_GREEN_EN,
        .active_low = 0,
        .retain_state_suspended = 0,
        .default_state = LEDS_GPIO_DEFSTATE_ON,
    },
#endif
#ifdef BSP_CONFIG_BOARD_WINGLE
    {
        .name = "wifi_led:green",
        .default_trigger = "timer",
        .gpio = WIFI_GREEN_EN,
        .active_low = 0,
        .retain_state_suspended = 0,
        .default_state = LEDS_GPIO_DEFSTATE_OFF,
    },
    {
#if (FEATURE_ON ==MBB_TELSTRA_CUSTOMSIZE)
        .name = "flow_led:red",
#else
        .name = "msg_led:red",
#endif       
        .default_trigger = "timer",
        .gpio = MSG_RED_EN,
        .active_low = 0,
        .retain_state_suspended = 0,
        .default_state = LEDS_GPIO_DEFSTATE_OFF,
    },
#endif
};
#endif

#if(FEATURE_ON == MBB_LED_DR)
/*NODE ���Ʋ��õĲ�Ʒ��Ҫ������*/
#ifdef BSP_CONFIG_BOARD_CPE
#define LED_NODE_1 "signal1_led:white"      
#define LED_NODE_2 "signal2_led:white" 
#define LED_NODE_3 "signal3_led:white" 
#define LED_NODE_4 "lan_led:white" 
#define LED_NODE_5 "5"
#endif 

#ifdef BSP_CONFIG_BOARD_HILINK
#define LED_NODE_1 "1"      
#define LED_NODE_2 "2" 
#define LED_NODE_3 "lte_led:red" 
#define LED_NODE_4 "lte_led:green" 
#define LED_NODE_5 "lte_led:blue"
#endif 

#ifdef BSP_CONFIG_BOARD_E5
#define LED_NODE_1 "1"
#define LED_NODE_2 "2" 
#define LED_NODE_3 "bat_led:red" 
#define LED_NODE_4 "bat_led:green" 
#define LED_NODE_5 "5"
#endif 

#ifdef BSP_CONFIG_BOARD_STICK
#define LED_NODE_1 "1"      
#define LED_NODE_2 "2" 
#define LED_NODE_3 "lte_led:red" 
#define LED_NODE_4 "lte_led:green" 
#define LED_NODE_5 "lte_led:blue"
#endif 

#ifdef BSP_CONFIG_BOARD_WINGLE
#if (FEATURE_ON ==MBB_TELSTRA_CUSTOMSIZE)
#define LED_NODE_1 "flow_led:green" 
#else
#define LED_NODE_1 "msg_led:green"   
#endif
#define LED_NODE_2 "2" 
#define LED_NODE_3 "lte_led:red" 
#define LED_NODE_4 "lte_led:green" 
#define LED_NODE_5 "lte_led:blue"
#endif

/*ע��Ľڵ��б�����Ҫ������*/
static struct led_info led_dr[] = {
#ifdef BSP_CONFIG_BOARD_CPE   
    [0] = {
        .name = LED_NODE_1,
    },
    [1] = {
        .name = LED_NODE_2,
    },
    [2] = {
        .name = LED_NODE_3,
    },
    [3] = {
        .name = LED_NODE_4,
    }
#endif
#ifdef BSP_CONFIG_BOARD_E5

    [0] = {
        .name = LED_NODE_3,
    },
    [1] = {
        .name = LED_NODE_4,
    }
#endif
#ifdef BSP_CONFIG_BOARD_WINGLE
    [0] = {
        .name = LED_NODE_1,
    },
    [1] = {
        .name = LED_NODE_3,
    },
    [2] = {
        .name = LED_NODE_4,
    },
    [3] = {
        .name = LED_NODE_5,
    }
#endif
#if  defined(BSP_CONFIG_BOARD_STICK) || defined (BSP_CONFIG_BOARD_HILINK)
    [0] = {
        .name = LED_NODE_3,
    },
    [1] = {
        .name = LED_NODE_4,
    },
    [2] = {
        .name = LED_NODE_5,
    }
#endif
};
#endif
/**************************************************************************
  �������� 
**************************************************************************/
/************************************************************************
 *����ԭ�� �� int led_kernel_status_set(char* led_name, int onoff)
 *����     �� �ṩkernel�׶� ��led�ƿ��ƽӿ�
 *����     ��Ҫ���Ƶ�led������, ״̬ int onoff
 *���     �� ��
 *����ֵ   �� �ɹ�0��ʧ��-1
 *�޸���ʷ     :
   ��    ��   : 20140729
   ��    ��   : z00278248
   �޸�����   : �����ɺ���
*************************************************************************/
int led_kernel_status_set(char* led_name, int onoff);

#endif
#ifdef __cplusplus
}
#endif

#endif

