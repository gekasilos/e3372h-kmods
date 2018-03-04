/******************************************************************************

                  ��Ȩ���� (C), 2001-2011,��Ϊ�ն����޹�˾

 ******************************************************************************
  �� �� ��   : chg_charge_adc.h
  �� �� ��   : ����
  ��������   : 2012��10��31��
  ����޸�   :
  ��������   : ͷ�ļ�
  �����б�   :
******************************************************************************/
#ifndef CHG_BOOT_CHIP_PLATFORM_H
#define CHG_BOOT_CHIP_PLATFORM_H

/******************************���ⵥ�޸ļ�¼**********************************
    ����       �޸���       ���ⵥ��                 �޸�����

******************************************************************************/

/*----------------------------------------------*
* ����ͷ�ļ�                                   *
*----------------------------------------------*/
#include "types.h"
#include <product_config.h>
/*************************Note:��Ʒ���Ժ궨��***********************/

 /*************************Note:��Ʒ���Ժ궨��***********************/



/********Note:����ͷ�ļ���ƽ̨ǿ��أ���ֲ��Ա����ƽ̨��Ҫ���*******/
#if (MBB_CHG_PLATFORM_V7R2 == FEATURE_ON)
#include <balongv7r2/gpio.h>
#include "drv_onoff.h"
#endif

/********Note:����ͷ�ļ���ƽ̨ǿ��أ���ֲ��Ա����ƽ̨��Ҫ���*******/

/*----------------------------------------------*
 * �ⲿ����˵��                                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * �ⲿ����ԭ��˵��                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * �ڲ�����ԭ��˵��                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * ȫ�ֱ���                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * ģ�鼶����                                   *
 *----------------------------------------------*/

/*----------------------------------------------*
 * ��������                                     *
 *----------------------------------------------*/
/*----------------------------------------------*
 * ö�ٶ���                                       *
 *----------------------------------------------*/
/* Begin daizhicheng 2013-07-01 added */
/* ����BOOT�׶�LOG�ȼ����� */
typedef enum
{
    CHG_MSG_ERR = 0,
    CHG_MSG_INFO,
    CHG_MSG_DEBUG,
    CHG_MSG_MAX
}CHG_MSG_TYPE;
/* End daizhicheng 2013-07-01 added */

/*----------------------------------------------*
 * �궨��                                       *
 *----------------------------------------------*/

//#define chg_print_level_message( print_level, tring, arg1, arg2, arg3)

/* BEGIN: PN:dongle121,chendongguo, 2013/6/28 added/deleted/modified*/
typedef unsigned long           ulong64_t;
//typedef unsigned int            uint32_t;
typedef signed int              int32_t;
typedef unsigned short          uint16_t;
typedef signed short            int16_t;
typedef unsigned char           uint8_t;
typedef signed char             int8_t;
typedef uint8_t                 boolean;
/* END: PN:dongle121,chendongguo, 2013/6/28 added/deleted/modified*/


#define TRUE 1
#define FALSE 0

/* Begin daizhicheng 2013-07-01 added */
/* BOOT�׶�LOGĬ�ϵȼ�----CHG_MSG_INFO */
#define CHG_BOOT_LOG_LEVEL CHG_MSG_INFO
/* End daizhicheng 2013-07-01 added */
#if (MBB_CHG_PLATFORM_V7R2 == FEATURE_ON)
#define CHG_BQ24192_CD_GPIO    CHG_ENABLE_GPIO
#else
#define CHG_BQ24192_CD_GPIO 31
#endif
/*----------------------------------------------*
 * ö�ٶ���                                       *
 *----------------------------------------------*/
/****Note:����ö�ٱ����ڲ�ͬ��ƽ̨���ڲ�ͬ���ļ����ж��壬���ƽ̨�����ļ����ж��屾�ļ��е����¶������ע��******/
#if (MBB_CHG_PLATFORM_V7R2 == FEATURE_ON)
#define  NV_BOOT_POWERUP_MODE                  (50364)
#define  NV_BOOT_FACTORY_MODE                   (36)
/* NV50364��ز������ݽṹ**/
typedef struct
{
    /*�ǹ���ģʽ�µ�ز���λ����ʹ�ܱ�־*/
    uint8_t no_battery_powerup_enable;
    /*�쳣�ػ����´β����Դ���뿪��ģʽʹ�ܱ�־*/
    uint8_t exception_poweroff_poweron_enable;
    /*�͵�ػ���ֹ��־*/
    uint8_t low_battery_poweroff_disable;
    /*����*/
    uint8_t reserved;
}BOOT_POWERUP_MODE_TYPE;
#endif
typedef enum POWER_ON_STATUS_tag
{
    POWER_ON_STATUS_INVALID         = 0x00000000, /* */
    POWER_ON_STATUS_BOOTING         = 0x00000000, /* */
    POWER_ON_STATUS_CHARGING        = 0x504F5343, /* */
    POWER_ON_STATUS_CAPACITY_LOW    = 0x504F434C, /* */
    POWER_ON_STATUS_BATTERY_ERROR   = 0x504F4245, /* */
    POWER_ON_STATUS_FINISH_NORMAL   = 0x6f656d00, /* */
    POWER_ON_STATUS_FINISH_CHARGE   = 0x59455353, /* */
    POWER_ON_STATUS_BUTT            = 0xFFFFFFFF  /* */
}POWER_ON_STATUS_ENUM;
#if (MBB_CHG_PLATFORM_V7R2 == FEATURE_ON)
typedef DRV_START_MODE DRV_START_MODE_ENUM;
#else
typedef enum DRV_START_MODE_tag
{
     DRV_START_MODE_EXCEPTION,  /* �쳣����ģʽ����ʱ�󲿷�ҵ�񶼲�����, ����ʾ��Ϣ */
     DRV_START_MODE_NORMAL,   /*��������*/
     DRV_START_MODE_CHARGING,   /* ��翪��ģʽ, ps ��ҵ������  */
     DRV_START_MODE_NOAPP,      /*SD�������ڶ�������������APP*/
     DRV_START_MODE_BUTT
}DRV_START_MODE_ENUM;
#endif

 /*chgģ���õ���ADC�ɼ�ͨ������ö��*/
typedef enum CHG_PARAMETER_ENUM_tag
{
    CHG_PARAMETER__BATTERY_VOLT,        /* ��ص�ѹ */
    CHG_PARAMETER__BATT_THERM_DEGC,     /* ����¶� */
    CHG_PARAMETER__VBUS_VOLT,           /*vbus��ѹ*/
    CHG_PARAMETER__BATT_BOT_THERM_DEGC,  /*�弶����¶�*/
    CHG_PARAMETER__BATTERY_ID_VOLT,  /*���ID��ѹ*/ 
    CHG_PARAMETER__INVALID
} CHG_PARAMETER_ENUM;

/****Note:����ö�ٱ����ڲ�ͬ��ƽ̨���ڲ�ͬ���ļ����ж��壬���ƽ̨�����ļ����ж��屾�ļ��е����¶������ע��******/
/* BEGIN: PN:dongle121,chendongguo, 2013/6/28 added/deleted/modified*/
typedef enum
{
    CHG_DISP_OK,
    CHG_DISP_FAIL,
    CHG_DISP_BATTERY_LOWER,  /* LOW BATTERY   */
    CHG_DISP_BATTERY_BAD,  /*BATTERY ERROR*/
    CHG_DISP_OVER_HEATED,/* Over-heated */
    CHG_DISP_TEMP_LOW, /* Temp. Too Low */
    CHG_DISP_CHARGING, /*charging*/
    CHG_DISP_MAX
}CHG_BATT_DISPLAY_TYPE;

/* END: PN:dongle121,chendongguo, 2013/6/28 added/deleted/modified*/

/*----------------------------------------------*
 * �ṹ����                                       *
 *----------------------------------------------*/
 /*ͨ�����Ͷ�Ӧͨ���Žṹ*/
typedef struct
{
    CHG_PARAMETER_ENUM  param_type;
    int32_t             adc_channel;
}CHG_ADC_PARAM_TO_CHANNEL_TYPE;

/*�¶ȶ�Ӧ��ѹ�ṹ*/
typedef struct
{
    int32_t   temperature;
    int32_t   voltage;
}CHG_TEMP2ADC_TYPE;
#if (MBB_CHG_PLATFORM_V7R2 == FEATURE_OFF)
typedef enum DRV_SHUTDOWN_REASON_tag_s
{
    DRV_SHUTDOWN_LOW_BATTERY,           /* ��ص�����                 */
    DRV_SHUTDOWN_BATTERY_ERROR,         /* ����쳣                   */
    DRV_SHUTDOWN_POWER_KEY,             /* ���� Power ���ػ�          */
    DRV_SHUTDOWN_TEMPERATURE_PROTECT,   /* ���±����ػ�               */
    DRV_SHUTDOWN_LOW_TEMP_PROTECT,
    DRV_SHUTDOWN_RESET,                 /* ϵͳ��λ                 */
    DRV_SHUTDOWN_CHARGE_REMOVE,         /* �ػ����ģʽ�£��γ������ */
    DRV_SHUTDOWN_UPDATE,                /* �ػ�����������ģʽ         */
    DRV_SHUTDOWN_BUTT
}DRV_SHUTDOWN_REASON_ENUM;
#endif
/**********************************************************************
FUNCTION:    chg_boot_set_power_off
DESCRIPTION: The poweroff func of CHG module, all the power-off operation
             except at boot phase MUST be performed via calling this.
INPUT:       The shutdown reason which triggered system poweroff.
             All VALID REASON:
             DRV_SHUTDOWN_BATTERY_ERROR --BATTERY ERROR;
             DRV_SHUTDOWN_TEMPERATURE_PROTECT --EXTREAM HIGH TEMPERATURE.
             DRV_SHUTDOWN_LOW_TEMP_PROTECT --EXTREAM LOW TEMPERATURE
             DRV_SHUTDOWN_CHARGE_REMOVE --CHGR REMOVAL WHILE POWEROFF CHG
             DRV_SHUTDOWN_LOW_BATTERY --LOW BATTERY
OUTPUT:      None.
RETURN:      None.
NOTE:        When this function get called to power system off, it record
             the shutdown reason, then simulate POWER_KEY event to APP to
             perform the real system shutdown process.
             THUS, THIS FUNCTION DOESN'T TAKE AFFECT IF APP DIDN'T STARTUP.
***********************************************************************/
void chg_boot_set_power_off(DRV_SHUTDOWN_REASON_ENUM real_reason);

/* Begin daizhicheng 2013-07-01 added */
/*****************************************************************************
 �� �� ��  : chg_print_level_message.fmt
 ��������  : LINUXƽ̨ͨ�õĴ�ӡ����
 �������  : int print_level
             const char *fmt...
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��5��8��
    ��    ��   : chendongguo
    �޸�����   : �����ɺ���

*****************************************************************************/
extern void chg_print_level_message(int print_level,const char *fmt,...);
/* End daizhicheng 2013-07-01 added */

/*****************************************************************************
 �� �� ��  : chg_boot_get_batt_volt_value
 ��������  : ��ȡ��ص�ѹ
 �������  : ��
 �������  : ��
 �� �� ֵ  : int32_t ��ѹֵ
 ���ú���  : ��
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��11��1��
    �޸�����   : �����ɺ���

*****************************************************************************/
extern int32_t chg_boot_get_batt_volt_value(void);

/*****************************************************************************
 �� �� ��  : chg_boot_get_temp_value
 ��������  : ��ȡ�¶Ȳ���
 �������  : param_type����������
 �������  : ��
 �� �� ֵ  : ��ȡ���¶�ֵ
 ���ú���  : ��
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��11��2��
    �޸�����   : �����ɺ���

*****************************************************************************/
extern int32_t chg_boot_get_temp_value(CHG_PARAMETER_ENUM param_type);

/**********************************************************************
�� �� ��      : chg_get_powdown_charging
��������  :  �жϵ�ǰ�Ƿ�Ϊ�ػ����ģʽ
�������  : �ޡ�
�������  : �ޡ�
�� �� ֵ      :     TRUE:  �ػ����ģʽ
                               FALSE:  �ǹػ����ģʽ
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��10��30��
    �޸�����   : �����ɺ���

***********************************************************************/
extern boolean chg_boot_is_powdown_charging (void);
/**********************************************************************
�� �� ��  : chg_boot_set_powdown_charging
��������  : ���õ�ǰΪ�ػ����ģʽ
�������  : �ޡ�
�������  : �ޡ�
�� �� ֵ  : ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��10��30��
    �޸�����   : �����ɺ���

***********************************************************************/
void chg_boot_set_powdown_charging (void);

/*****************************************************************************
 �� �� ��  : chg_boot_display_interface
 ��������  : ��װ����ͳһ��ʾ�ӿ�
 �������  : CHG_BATT_DISPLAY_TYPE disp_type��ʾ����
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��10��30��
    �޸�����   : �����ɺ���

*****************************************************************************/
void chg_boot_display_interface(CHG_BATT_DISPLAY_TYPE disp_type);

extern void kernel_power_off(void);

/**********************************************************************
�� �� ��  : chg_boot_is_ftm_mode
��������  : �ж�Ŀǰ�Ƿ�Ϊftmģʽ
�������  : �ޡ�
�������  : �ޡ�
�� �� ֵ  : Ŀǰ�Ƿ�Ϊ����ģʽ
           TRUE : ����ģʽ
           FALSE: �ǹ���ģʽ
ע������  : �ޡ�
***********************************************************************/
boolean chg_boot_is_ftm_mode(void);
/**********************************************************************
�� �� ��  : chg_boot_is_no_battery_powerup_enable
��������  : �ж�Ŀǰ�Ƿ�Ϊ�ǹ���ģʽ�µĵ�ز���λ����ʹ��ģʽ
�������  : �ޡ�
�������  : �ޡ�
�� �� ֵ  : Ŀǰ�Ƿ�Ϊ�ǹ���ģʽ��ز���λ����ʹ��
                           TRUE : ʹ��
                           FALSE: ��ʹ��
ע������  : �ޡ�
***********************************************************************/
extern boolean chg_boot_is_no_battery_powerup_enable(void);

/**********************************************************************
�� �� ��  : chg_boot_is_exception_poweroff_poweron_enable
��������  : �ж�Ŀǰ�Ƿ�Ϊ����USBǿ�ƿ���ʹ��ģʽ
�������  : �ޡ�
�������  : �ޡ�
�� �� ֵ  : Ŀǰ�Ƿ�Ϊ����USBǿ�ƿ���ʹ��ģʽ
                           TRUE : ʹ��
                           FALSE: ��ʹ��
ע������  : �ޡ�
***********************************************************************/
extern boolean chg_boot_is_exception_poweroff_poweron_enable(void);

/*****************************************************************************
 �� �� ��  : chg_boot_delay_ms
 ��������  : boot�׶ε���ʱ��������ͬƽ̨��Ҫ��������
 �������  : unsigned int microsecond
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��6��27��
    ��    ��   : chendongguo
    �޸�����   : �����ɺ���

*****************************************************************************/
/* modify by daizhicheng */
void chg_boot_delay_ms (uint32_t microsecond);

/*****************************************************************************
 �� �� ��  : chg_boot_get_volt_from_adc
 ��������  : ��ADCͨ����ȡ��ѹֵ
 �������  : param_typeͨ������
 �������  : ��
 �� �� ֵ  : ��ȡ�ĵ�ѹֵ
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��10��31��
    �޸�����   : �����ɺ���

*****************************************************************************/
int32_t chg_boot_get_volt_from_adc(CHG_PARAMETER_ENUM param_type);

#endif/*CHG_BOOT_CHIP_PLATFORM_H*/

