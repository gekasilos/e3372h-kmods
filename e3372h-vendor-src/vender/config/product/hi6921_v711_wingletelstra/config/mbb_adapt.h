/***************************************************************************************************
  Copyright, 1988-2013, Huawei Tech. Co., Ltd.
  File name:      mbb_adapt.h
  Author:         xuchao x00202188
  Version:        V1.0 
  Date:           2014-07-20
  Description:    ��Ӳ�������ڴ��ļ���ͳһ����
  Others: 
                  1������GPIO����
                  2��LED��GPIO����
                  3��LED��DR����������
  Warning:        
                  1: ��Ӳ������ǰ�������Ӳ���ṩ�Ľӿ��ĵ������ͷ�ļ�
******************************************************************************************************/
#ifndef __MBB_ADAPT_H__
#define __MBB_ADAPT_H__

#include <product_config.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define GPIO_NULL               -1          /* �޴�GPIOʱȡ-1 */

/*  ����GPIO���ã�����޴˰�������Ϊ-1 */
#define MENU_KEY_GPIO          (GPIO_NULL)  /* MENU/WPS��*/
#define RESET_KEY_GPIO         (GPIO_2_16)   /* RESET��*/
#define WIFI_KEY_GPIO          (GPIO_NULL)  /* WIFI/WLAN��*/

/*  LED��GPIO�������ֶ��޸�led_dr.c �� leds-gpio.c */

#define WIFI_ACTIVE_GPIO     (GPIO_2_21)  /*wifi enable*/
#define WIFI_WAKEUP_BB_GPIO     (GPIO_NULL)  /*wifi wakeup bb*/

#define CHG_ENABLE_GPIO       (GPIO_NULL)  /*charge enable*/
#define CHG_BATT_LOW_INT      (GPIO_NULL)  /*batt low int*/
#define CHG_BATT_ID_CHAN      (0)  /*batt id hkadc channel*/
#define CHG_BATT_TEMP_CHAN    (2)  /*batt temp hkadc channel*/
#define CHG_BATT_VOLT_CHAN    (8)  /*batt volt hkadc channel*/
#define CHG_VBUS_VOLT_CHAN    (-1)  /*vbus volt hkadc channel*/

#define LCD_BACKLIGHT_GPIO   (GPIO_NULL)  /*lcd backlight*/
#define LCD_ID00_GPIO         (GPIO_NULL)  /*lcd id0*/
#define LCD_ID01_GPIO         (GPIO_NULL)  /*lcd id1*/

#define OLED_ID0_GPIO        (GPIO_NULL)  /*oled id0*/
#define OLED_ID1_GPIO        (GPIO_NULL)  /*oled id1*/
#define OLED_CD_GPIO         (GPIO_NULL)  /*oled cd*/

#define OTG_ID_DET_GPIO      (GPIO_NULL)  /*otg detect*/
#define OTG_ID_SET_GPIO      (GPIO_NULL)  /*otg set*/
#define DMDP_CONNECT_GPIO    (GPIO_NULL)  /*dmdp connect*/

#define SD_DETECT_GPIO      (GPIO_NULL)  /*sd detect*/

#define SIM0_DETECT_GPIO     (GPIO_0_5)  /*sim detect*/


#ifdef __cplusplus
}
#endif

#endif /*__MBB_ADAPT_H__ */
