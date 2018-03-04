#ifndef __CHG_CHARGE_STM_H__
#define __CHG_CHARGE_STM_H__
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
/******************************�޸ļ�¼*************************************************************************************
  ����         �޸���          ���ⵥ��            �޸�����
2012.11.09   Z00195562     DTS201211091020      2. ��Ӹߵ��¡���Ƿѹ�������¶ȡ����ƽ���㷨ƽ�������ֱ���
******************************************************************************************************************************/


/*----------------------------------------------*
 * �궨��                                       *
 *----------------------------------------------*/
#define CHG_CHGR_TYPE_CHECK_MAX_RETRY_TIMES       (4)

#define CHG_CHGR_TYPE_CHECK_INTERVAL_IN_MS        (3500)

/*Fast charge protection timer, in second. */
#define MS_IN_SECOND                            (1000)
#define SECOND_IN_HOUR                          (60 * 60)
#ifdef BSP_CONFIG_BOARD_E5_E5578
#define CHG_FAST_CHG_TIMER_VALUE                (6 * SECOND_IN_HOUR)
#define CHG_POWEROFF_FAST_CHG_TIMER_VALUE       (6  * SECOND_IN_HOUR)

#define CHG_FAST_USB_TIMER_VALUE                (12 * SECOND_IN_HOUR)
#define CHG_POWEROFF_FAST_USB_TIMER_VALUE       (12  * SECOND_IN_HOUR)
#else
#define CHG_FAST_CHG_TIMER_VALUE                (5 * SECOND_IN_HOUR)
#define CHG_POWEROFF_FAST_CHG_TIMER_VALUE       (5  * SECOND_IN_HOUR)

#define CHG_FAST_USB_TIMER_VALUE                (10 * SECOND_IN_HOUR)
#define CHG_POWEROFF_FAST_USB_TIMER_VALUE       (10  * SECOND_IN_HOUR)
#endif

/*Time Interval for toggle CEN.*/
#define CHG_TOGGLE_CEN_INTVAL_IN_MS             (100)

/*Time Interval for switch to SLOW POLLING while battery only.*/
#define CHG_SWITCH_TO_SLOW_POLL_INTERVAL_IN_SEC (60)

/*Indicate battery charging start/stop flag.*/
#define CHG_UI_START_CHARGING                   (1)
#define CHG_UI_STOP_CHARGING                    (0)

#define EXTCHG_DELAY_COUNTER_SIZE               (500)        /* ��ʱ500ms */

/* �����岹��� */
#if (MBB_CHG_COMPENSATE == FEATURE_ON)
#define CHG_DELAY_COUNTER_SIZE                  (10)        /* ��ʱ10ms */
#ifdef BSP_CONFIG_BOARD_E5_E5578  
#define TBAT_SUPPLY_VOLT                        (3780)      /*��ز�����ֵ40%����*/
#define TBAT_DISCHG_VOLT                        (4085)      /*��طŵ���ֵ80%����*/

#define TBAT_SUPPLY_STOP_VOLT                   (3825)      /*��س���ֹ��ѹ */
#define TBAT_DISCHG_STOP_VOLT                   (4050)      /*��طŵ��ֹ��ѹ */
#else
#define TBAT_SUPPLY_VOLT                        (3696)      /*��ز�����ֵ40%����*/
#define TBAT_DISCHG_VOLT                        (3970)      /*��طŵ���ֵ80%����*/

#define TBAT_SUPPLY_STOP_VOLT                   (3750)      /*��س���ֹ��ѹ */
#define TBAT_DISCHG_STOP_VOLT                   (3940)      /*��طŵ��ֹ��ѹ */
#endif

#define TBAT_SUPPLY_CURR_SUCCESS                (0x0)       /*����ɹ�*/
#define TBAT_NO_NEED_SUPPLY_CURR                (0x1)       /*����Ҫ����*/

#define TBAT_STOP_DELAY_COUNTER                 (100)       /* ����ֹͣʱ�� */
#define TBAT_SUPLY_DELAY_COUNTER                (2300)      /* ��������ʱ�� */
#endif /*HUAWEI_CHG_COMPENSATE */

/*�ػ����ػ�������**/
#define CHARGE_REMOVE_CHECK_MAX                 (1)
/*���ID��Ϣ����*/
typedef enum
{
    CHG_BATT_ID_DEF,
    CHG_BATT_ID_XINGWANGDA,
    CHG_BATT_ID_FEIMAOTUI, 
    CHG_BATT_ID_LISHEN, 
    CHG_BATT_ID_MAX
}CHG_BATT_ID;

/*----------------------------------------------*
 * �ṹ����                                       *
 *----------------------------------------------*/
typedef struct
{
    uint32_t pwr_supply_current_limit_in_mA;
    uint32_t chg_current_limit_in_mA;
    uint32_t chg_CV_volt_setting_in_mV;
    uint32_t chg_taper_current_in_mA;
    boolean  chg_is_enabled; /*FALSE: Charge disable; TRUE: Charge enable.*/
}chg_hw_param_t;

typedef  void (*chg_stm_func_type )(void);
typedef struct
{
  chg_stm_func_type        chg_stm_entry_func;
  chg_stm_func_type        chg_stm_period_func;
  chg_stm_func_type        chg_stm_exit_func;
}chg_stm_type;

/*���¹ػ��¶Ȳ���NV50016�ṹ����*/
typedef struct
{
    uint32_t      ulIsEnable;             //���¹ػ�ʹ�ܿ���
    int32_t       lCloseAdcThreshold;     //���¹ػ��¶�����
    uint32_t      ulTempOverCount;        //���¹ػ��¶ȼ�����
}CHG_BATTERY_OVER_TEMP_PROTECT_NV;

/*���¹ػ��¶Ȳ���NV52005�ṹ����*/
typedef struct
{
    uint32_t      ulIsEnable;
    int32_t       lCloseAdcThreshold;
    uint32_t      ulTempLowCount;
}CHG_BATTERY_LOW_TEMP_PROTECT_NV;


/*���������¶Ȳ���NV 50385�ṹ����*/
typedef struct
{
    uint32_t    ulChargeIsEnable;                   //����±���ʹ��
    int32_t     overTempchgStopThreshold;           //�����±�������
    int32_t     subTempChgLimitCurrentThreshold;    //���³���������
    int32_t     lowTempChgStopThreshold;            //�����±�������
    int32_t     overTempChgResumeThreshold;         //�����»ָ��¶�����
    int32_t     lowTempChgResumeThreshold;          //�����»ָ��¶�����
    uint32_t    chgTempProtectCheckTimes;           //���ͣ����ѯ����
    uint32_t    chgTempResumeCheckTimes;            //��縴����ѯ����
    int32_t     exitWarmChgToNormalChgThreshold;    //�ɸ��³��ָ������³���¶�����
    int32_t     reserved2;                           //Ԥ��
}CHG_SHUTOFF_TEMP_PROTECT_NV_TYPE;

/*���������¶Ȳ���NV50386�ṹ����*/
typedef struct
{
    int32_t         battVoltPowerOnThreshold;           //������ѹ����
    int32_t         battVoltPowerOffThreshold;          //�ػ���ѹ����
    int32_t         battOverVoltProtectThreshold;       //ƽ������ѹ��������(ƽ��ֵ)
    int32_t         battOverVoltProtectOneThreshold;    //���γ���ѹ��������(����ֵ)
    int32_t         battChgTempMaintThreshold;          //���ָ���ͣ�������ͣ����ж�����
    int32_t         battChgRechargeThreshold;           //����������θ�������
    int32_t         VbatLevelLow_MAX;                   //�͵���������
    int32_t         VbatLevel0_MAX;                     //0���ѹ��������
    int32_t         VbatLevel1_MAX;                     //1���ѹ��������
    int32_t         VbatLevel2_MAX;                     //2���ѹ��������
    int32_t         VbatLevel3_MAX;                     //3���ѹ��������
    int32_t         battChgFirstMaintThreshold;         //�״��ж��Ƿ�����
    int32_t         battNormalTempChgRechargeThreshold; //�������临������
}CHG_SHUTOFF_VOLT_PROTECT_NV_TYPE;

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
}POWERUP_MODE_TYPE;

struct chg_batt_data {
    unsigned int      id_voltage_min;
    unsigned int      id_voltage_max;
    CHG_SHUTOFF_VOLT_PROTECT_NV_TYPE    chg_batt_volt_paras;
    unsigned int      batt_id;
};

/*----------------------------------------------*
 * ȫ�ֱ���                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * �ڲ�����ԭ��˵��                             *
 *----------------------------------------------*/
 
/*set chg para*/ 
void chg_set_hardware_parameter(const chg_hw_param_t* ptr_hw_param);
/*******************************************************************
Function:      chg_poll_bat_level
Description:   ��ص�ѹֵ�ӹ���������̿��ƺ�����
Calls:         chg_get_parameter_level-------------�ɼ���ѹֵֵ�ӿں�����
               chg_calc_average_volt_value--------��ѹƽ���㷨��������
               huawei_set_temp------------------------��ѹ��������
Data Accessed: ��
Data Updated:  ȫ�ֱ������¶�ֵ
Input:         ��
Output:        ��
Return:        ��
*******************************************************************/
void chg_poll_bat_level(void);

#if (MBB_CHG_COULOMETER == FEATURE_ON || MBB_CHG_BQ27510 == FEATURE_ON)
/*******************************************************************
Function:      chg_poll_batt_soc
Description:   update the battery soc,if changed,report to app
Calls:         chg main task
Input:         none
Output:        none
Return:        none
*******************************************************************/
void chg_poll_batt_soc(void);
/*******************************************************************
Function:      chg_poll_batt_charging_state_for_coul
Description:   update the battery charging state,if changed,notify coul
Calls:         chg main task
Input:         none
Output:        none
Return:        none
*******************************************************************/
void chg_poll_batt_charging_state_for_coul(void);
/**********************************************************************
�� �� ��  :chg_is_batt_in_state_of_emergency
��������  : ��ѯ����Ƿ��ڽ�����Ҫ�ػ�״̬
�������  : �ޡ�
�������  : �ޡ�
�� �� ֵ  : TRUE:yes��FALSE:no
ע������  : �ޡ�
***********************************************************************/
boolean chg_is_batt_in_state_of_emergency();
/**********************************************************************
�� �� ��: chg_low_battery_event_handler
��������: �յ����ؼƵ͵��¼���Ĵ�����
�������: None
�������: None
�� �� ֵ: �ޡ�
ע������: Linuxϵͳ�У����øú���ʱ���ж��Ƿ����¼�û�д������
***********************************************************************/
void chg_low_battery_event_handler(void);
#endif
/*******************************************************************
Function:      chg_poll_batt_temp
Description:   ����¶�ֵ�ӹ���������̿��ƺ�����
Calls:          chg_get_parameter_level-----------------�ɼ��¶�ֵ�ӿں�����
                chg_calc_average_temp_value-------------ƽ���㷨��������
                chg_huawei_set_temp----------------------�¶Ȳ�������
                chg_temp_is_too_hot_or_too_cold_for_chg ----------����±������¶ȼ�⺯��
                chg_temp_is_too_hot_or_too_code_for_shutoff------�¶ȹػ�������⺯��
Data Accessed: ��
Data Updated:  ȫ�ֱ������¶�ֵ
Input:          ��
Output:         ��
Return:         ��
*******************************************************************/
void chg_poll_batt_temp(void);
/*******************************************************************
Function:      chg_batt_volt_init
Description:   ��ص�ѹ����ֵ��ʼ����������ȡNV�����ȡ��ֵ�浽ȫ�ֱ�����
Data Accessed: ��
Data Updated:  ��
Input:         ��
Return:        ��
*******************************************************************/
void chg_batt_volt_init(void);
/*******************************************************************
Function:      chg_batt_temp_init
Description:   ����¶�����ֵ��ʼ����������ȡNV�����ȡ��ֵ�浽ȫ�ֱ�����
Data Accessed: ��
Data Updated:  ��
Input:         ��
Return:        ��
*******************************************************************/
void chg_batt_temp_init(void);

/*****************************************************************************
 �� �� ��  : chg_stm_get_cur_state
 ��������  : Get current battery charge state: Fast charge, Transitition state, etc.
 �������  : None.
 �������  : None
 �� �� ֵ  : Current battery charge state.
 WHO-CALL  : DFT and other sub-modules.
 CALL-WHOM : None.
 NOTICE    : Need exported.
*****************************************************************************/
extern chg_stm_state_type chg_stm_get_cur_state(void);

/*****************************************************************************
 �� �� ��  : chg_set_cur_chg_mode
 ��������  : set the current  charge mode
 �������  : new_state  The new state we gonna update to.
 �������  : None
 �� �� ֵ  : VOID.
 WHO-CALL  :
 CALL-WHOM : None.
 NOTICE    : Helper function.
*****************************************************************************/
extern void chg_set_cur_chg_mode(CHG_MODE_ENUM chg_mode);

/*****************************************************************************
 �� �� ��  : chg_stm_get_chgr_type
 ��������  : Get current external charger type:
             CHG_CHGR_UNKNOWN: Chgr type has not been got from USB module.
             CHG_WALL_CHGR   : Wall standard charger, which D+/D- was short.
             CHG_USB_HOST_PC : USB HOST PC or laptop or pad, etc.
             CHG_NONSTD_CHGR : D+/D- wasn't short and USB enumeration failed.
             CHG_CHGR_INVALID: External Charger invalid or absent.
 �������  : None.
 �������  : None
 �� �� ֵ  : Current external charger type.
 WHO-CALL  : DFT and other sub-modules.
 CALL-WHOM : None.
 NOTICE    : Need exported.
             The real chgr type checking would be done by USB module.
*****************************************************************************/
extern chg_chgr_type_t chg_stm_get_chgr_type(void);

/*****************************************************************************
 �� �� ��  : chg_stm_get_cur_state
 ��������  : Get current battery charge state: Fast charge, Transitition state, etc.
 �������  : None.
 �������  : None
 �� �� ֵ  : Current battery charge state.
 WHO-CALL  : DFT and other sub-modules.
 CALL-WHOM : None.
 NOTICE    : Need exported.
*****************************************************************************/
extern CHG_MODE_ENUM chg_get_cur_chg_mode(void);


/*****************************************************************************
 �� �� ��  : chg_check_and_update_hw_param_per_chgr_type
 ��������  : When chgr_type_checking timer expired, this function would be called
             via charge main task.
             As its name suggests, it will check the chgr type from USB and if got,
             update the charge hardware parameter, or re-fire the timer utils max
             retry time reached.
 �������  : VOID.
 �������  : None
 �� �� ֵ  : VOID.
 WHO-CALL  : Charge Main Task.
 CALL-WHOM : None.
 NOTICE    : If max retry time reached, the chgr type would be set to CHG_NONSTD_CHGR.
             Need exported.
*****************************************************************************/
void chg_check_and_update_hw_param_per_chgr_type(void);

/**********************************************************************
�� �� ��  : chg_get_batt_id_valid
��������  : ��⵱ǰ�ĵ���Ƿ���λ���жϵ�������ntc�¶�
�������  : �ޡ�
�������  : �ޡ�
�� �� ֵ  : TRUE: �����λ
            FALSE: ��ز���λ
ע������  : �ޡ�
***********************************************************************/
extern boolean chg_get_batt_id_valid(void);

/**********************************************************************
�� �� ��  :chg_get_batt_level
��������  : ��ȡ��ǰ��ص��������Ľӿں���
�������  : �ޡ�
�������  : �ޡ�
�� �� ֵ  : ��ص�������
ע������  : �ޡ�
***********************************************************************/
extern BATT_LEVEL_ENUM chg_get_batt_level(void);


/**********************************************************************
�� �� ��  :chg_get_batt_level
��������  : ��ȡ��ǰ��ص����ٷֱȵĽӿں���
�������  : �ޡ�
�������  : �ޡ�
�� �� ֵ  : ��ص����ٷֱ�
ע������  : �ޡ�
***********************************************************************/
extern int32_t chg_get_sys_batt_capacity(void);

/**********************************************************************
�� �� ��  :chg_set_sys_batt_capacity
��������  : ���õ�ǰ��ص����ٷֱȵĽӿں���
�������  : capacity :��ص����ٷֱ�
�������  : �ޡ�
�� �� ֵ  : ��ص����ٷֱ�
ע������  : �ޡ�
***********************************************************************/
extern void chg_set_sys_batt_capacity(int32_t capacity);

/**********************************************************************
�� �� ��  :chg_set_batt_time_to_full
��������  : ����Ԥ���ʣ����ʱ��
�������  : �ޡ�
�������  : �ޡ�
�� �� ֵ  : ��ص�����������ж೤ʱ��
ע������  : �ޡ�
***********************************************************************/
extern void chg_set_batt_time_to_full(int32_t time_to_full);

/**********************************************************************
�� �� ��  :chg_get_batt_time_to_full
��������  : ��ȡ��ص�����������ж೤ʱ��
�������  : �ޡ�
�������  : �ޡ�
�� �� ֵ  : ��ص�����������ж೤ʱ��
ע������  : �ޡ�
***********************************************************************/
extern int32_t chg_get_batt_time_to_full(void);

/**********************************************************************
�� �� ��  : chg_is_batt_full
��������  : �жϵ���Ƿ�����
�������  : �ޡ�
�������  : �ޡ�
�� �� ֵ  : 1:�������

            0:��ط�����
ע������  : �ޡ�
***********************************************************************/
extern boolean chg_is_batt_full(void);

/**********************************************************************
�� �� ��  : chg_is_batt_full_for_start
��������  : �жϵ�ص�ѹ�Ƿ����㿪ʼ�������
�������  : �ޡ�
�������  : �ޡ�
�� �� ֵ  : 1:������磬����Ҫ�������
            0:��ط����磬��Ҫ�������
ע������  : �ޡ�
***********************************************************************/
extern boolean chg_is_batt_full_for_start(void);

/*****************************************************************************
 �� �� ��  : chg_get_bat_status
 ��������  : ��ȡ����Ƿ���λ��״̬����������power supply    USB online �ڵ�
             ��״̬
 �������  : void
 �������  : ��
 �� �� ֵ  :POWER_SUPPLY_STATUS_UNKNOWN = 0,   ���״̬δ֪
	       POWER_SUPPLY_STATUS_CHARGING,      �ػ�/�������ڳ��
	       POWER_SUPPLY_STATUS_DISCHARGING,   ��طŵ�(��δʹ��)
	       POWER_SUPPLY_STATUS_NOT_CHARGING,  �ػ�����ֹͣ���
	       POWER_SUPPLY_STATUS_FULL,          �������
	       POWER_SUPPLY_STATUS_NEED_SUPPLY,   �����Ҫ����
	       POWER_SUPPLY_STATUS_SUPPLY_SUCCESS,��ز���ɹ�
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��5��11��
    ��    ��   : chendongguo
    �޸�����   : �����ɺ���

*****************************************************************************/
extern int32_t chg_get_bat_status(void);

/*****************************************************************************
 �� �� ��  : chg_get_bat_health
 ��������  : ��ȡ����Ƿ���λ��״̬����������power supply ��� health �ڵ�
             ��״̬
 �������  : void
 �������  : ��
 �� �� ֵ  :ONLINE:���߳����λ
            OFFLINE:���߳�粻��λ
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��5��11��
    ��    ��   : chendongguo
    �޸�����   : �����ɺ���

*****************************************************************************/
extern int32_t chg_get_bat_health(void);

/*****************************************************************************
�� �� ��  : chg_get_extchg_status
��������  : ��ȡ����Ƿ���λ��״̬����������power supply ��� health �ڵ�
          ��״̬
�������  : void
�������  : ��
�� �� ֵ  :ONLINE:���߳����λ
         OFFLINE:���߳�粻��λ
���ú���  :
��������  :

�޸���ʷ      :
1.��    ��   : 2013��5��11��
 ��    ��   : chendongguo
 �޸�����   : �����ɺ���

*****************************************************************************/
extern int32_t chg_get_extchg_status(void);

/**********************************************************************
�� �� ��  : chg_get_batt_temp_state
��������  : ��ȡ��ǰ����¶�״̬
�������  : �ޡ�
�������  : �ޡ�
�� �� ֵ  : ����¶�״̬
ע������  : �ޡ�
***********************************************************************/
TEMP_EVENT chg_get_batt_temp_state(void);

/*****************************************************************************
 �� �� ��  : chg_stm_switch_state
 ��������  : Charge State Machine Switch State function. State Machine or other
             Sub-Module call this function change the battery charge state.
 �������  : new_state  The new state we gonna switch to.
 �������  : None
 �� �� ֵ  : VOID
 WHO-CALL  : All states' periodic function.
             Polling timer and charger insertion/removal event(BH)
             from charge task.
 CALL-WHOM : The exit function of current state.
             The entry function of new state.
 NOTICE    : If the new_state is Transition State, its periodic would be called.
*****************************************************************************/
extern void chg_stm_switch_state(chg_stm_state_type new_state);

/*****************************************************************************
 �� �� ��  : chg_stm_periodic_checking_func
 ��������  : Periodic checking function of charge state machine. Charge main
             task would call it periodically once polling timer triggered.
 �������  : None
 �� �� ֵ  : VOID
 WHO-CALL  : chg main task.
 CALL-WHOM : All states' period function.
 NOTICE    : The periodic function of transition state should NOT be called
             here.
             It's better NOT to call this function in timer/interrupt context.
             Need Exported.
*****************************************************************************/
void chg_stm_periodic_checking_func(void);

/*****************************************************************************
 �� �� ��  : chg_stm_init
 ��������  : Charge State Machine Sub-Moudle initialize procedure.
 �������  : VOID
 �������  : VOID
 �� �� ֵ  : CHG_STM_SUCCESS for initialize successful
             CHG_STM_FAILED  for initialize failed
 WHO-CALL  :
 CALL-WHOM : chg_stm_set_chgr_type
             chg_stm_set_cur_state
             chg_stm_switch_state
*****************************************************************************/
int32_t chg_stm_init(void);


/*****************************************************************************
 �� �� ��  : chg_get_charging_status
 ��������  :��ȡ��ǰ�Ƿ����ڳ��
 �������  : None.
 �������  : None
 �� �� ֵ  : Current battery charge state.
 WHO-CALL  : DFT and other sub-modules.
 CALL-WHOM : None.
 NOTICE    : Need exported.
*****************************************************************************/
boolean chg_get_charging_status(void);

/**********************************************************************
�� �� ��  : chg_is_ftm_mode
��������  : �ж�Ŀǰ�Ƿ�Ϊftmģʽ
�������  : �ޡ�
�������  : �ޡ�
�� �� ֵ  : Ŀǰ�Ƿ�Ϊ����ģʽ
           TRUE : ����ģʽ
           FALSE: �ǹ���ģʽ
ע������  : �ޡ�
***********************************************************************/
extern boolean chg_is_ftm_mode(void);

/*****************************************************************************
 �� �� ��  : chg_stm_set_chgr_type
 ��������  : Update the current type of external charger.
 �������  : chgr_type  The charger type we gonna update to.
 �������  : None
 �� �� ֵ  : VOID.
 WHO-CALL  : chg_check_and_update_hw_param_per_chgr_type.
 CALL-WHOM : None.
 NOTICE    : Helper function.
*****************************************************************************/
extern void chg_stm_set_chgr_type(chg_chgr_type_t chgr_type);
/*****************************************************************************
 �� �� ��  : chg_start_chgr_type_checking
 ��������  : Callback function of charger type checking timer.
             This timer is not autoload, we may re-start it in
             chg_check_and_update_hw_param_per_chgr_type.
 �������  : VOID.
 �������  : None
 �� �� ֵ  : VOID.
 WHO-CALL  : OS timer interrupt or task.
 CALL-WHOM : None.
 NOTICE    : This function run in timer context.
*****************************************************************************/
extern void chg_start_chgr_type_checking(void);


#if (MBB_CHG_WARM_CHARGE == FEATURE_ON)
/*****************************************************************************
 �� �� ��  : chg_stm_set_pre_state
 ��������  : ��״̬������״̬�л�ʱ������һ��״̬ͨ��pre_state���룬���µ�ȫ�����ݽṹ��
 �������  : chg_stm_state_type pre_state  ǰһ�����״ֵ̬
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  : chg_stm_switch_state

 �޸���ʷ      :
  1.��    ��   : 2013��2��18��
    ��    ��   : chendongguo
    �޸�����   : �����ɺ���

*****************************************************************************/
void chg_stm_set_pre_state(chg_stm_state_type pre_state);

/*****************************************************************************
 �� �� ��  : chg_stm_get_pre_state
 ��������  : ��ȡǰһ�����״̬��ֵ
 �������  : void
 �������  : void
 �� �� ֵ  : ǰһ�����״ֵ̬
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��2��21��
    ��    ��   : chendongguo
    �޸�����   : �����ɺ���

*****************************************************************************/
chg_stm_state_type chg_stm_get_pre_state(void);

/*****************************************************************************
 �� �� ��  : is_batttemp_in_warm_chg_area
 ��������  : battery temprature of warm charge check function.
             when the battery temprature rise to the area of [45��,55��) change the
             charge state to warm charge.else if the battery temprature resume
             below 42�㣬change the charge state to fast charge state.
 �������  : void
 �������  : void
 �� �� ֵ  : TRUE: battery temperature is in warm charge area
             FALSE:battery temperature is in normal charge area
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��2��17��
    ��    ��   : chendongguo
    �޸�����   : �����ɺ���

*****************************************************************************/
boolean is_batttemp_in_warm_chg_area( void );
#endif /* MBB_CHG_WARM_CHARGE */

/* �����岹��� */
#if (MBB_CHG_COMPENSATE == FEATURE_ON)
/**********************************************************************
�� �� ��  :  chg_tbat_status_get
��������  :  TBAT AT^TCHRENABLE?�Ƿ���Ҫ����

�������  : ��
�������  : ��
�� �� ֵ      : 1:��Ҫ����
			    0:����Ҫ����
ע������  : ��
***********************************************************************/
int32_t chg_tbat_status_get(void);

/**********************************************************************
�� �� ��  :  chg_is_sply_finish
��������  :  �жϲ����Ƿ����

�������  : ��
�������  : ��
�� �� ֵ      : 1:��ɲ���
                0:����δ���
ע������  : ��
***********************************************************************/
boolean chg_is_sply_finish(void);

/**********************************************************************
�� �� ��  : chg_batt_suply_proc
��������  :  ��ز�������

�������  : ��
�������  : ��
�� �� ֵ      : 1:����Ҫ����
                  0:����ɹ�
ע������  : ��
***********************************************************************/
int32_t chg_batt_supply_proc(void *task_data);

/**********************************************************************
�� �� ��  :  chg_tbat_chg_sply
��������  :  TBAT AT^TCHRENABLE=4����ʵ��
                        ����ɹ�LCD��ʾ�ɹ�ͼ��
�������  : ��
�������  : ��
�� �� ֵ      :
ע������  : ��
***********************************************************************/
int32_t chg_tbat_chg_sply(void);
#endif /* MBB_CHG_COMPENSATE */

/*****************************************************************************
 �� �� ��  : chg_get_charging_status
 ��������  :��ȡ��ǰ�Ƿ����ڳ��
 �������  : None.
 �������  : None
 �� �� ֵ  : Current battery charge state.
 WHO-CALL  : DFT and other sub-modules.
 CALL-WHOM : None.
 NOTICE    : Need exported.
*****************************************************************************/
extern boolean chg_get_charging_status(void);
/**********************************************************************
�� �� ��  : boolean chg_is_exception_poweroff_poweron_mode(void)
��������  :  �ж�Ŀǰ�Ƿ�Ϊ�쳣�ػ����´β����Դ����뿪��ģʽʹ�ܿ���ʹ��ģʽ
�������  : �ޡ�
�������  : �ޡ�
�� �� ֵ  : �Ƿ�Ϊ�쳣�ػ����´β����Դ����뿪��ģʽʹ��
           TRUE : ��
           FALSE: ��
ע������  : �ޡ�
***********************************************************************/
extern boolean chg_is_exception_poweroff_poweron_mode(void);

/**********************************************************************
�� �� ��  : chg_get_cur_batt_temp(void)
��������  :  ��ȡ��ǰ��ص�ѹ
�������  : �ޡ�
�������  : �ޡ�
�� �� ֵ  : �Ƿ�Ϊ�쳣�ػ����´β����Դ����뿪��ģʽʹ��
           TRUE : ��
           FALSE: ��
ע������  : �ޡ�
***********************************************************************/
extern int32_t chg_get_cur_batt_temp(void);

/**********************************************************************
�� �� ��  : chg_get_sys_batt_temp(void)
��������  :  ��ȡ��ǰ����¶�ƽ��ֵ
�������  : �ޡ�
�������  : �ޡ�
�� �� ֵ  : �Ƿ�Ϊ�쳣�ػ����´β����Դ����뿪��ģʽʹ��
           TRUE : ��
           FALSE: ��
ע������  : �ޡ�
***********************************************************************/
extern int32_t chg_get_sys_batt_temp(void);

/**********************************************************************
�� �� ��  : chg_get_sys_batt_volt(void)
��������  :  ��ȡ��ǰ��ص�ѹƽ��ֵ
�������  : �ޡ�
�������  : �ޡ�
�� �� ֵ  : �Ƿ�Ϊ�쳣�ػ����´β����Դ����뿪��ģʽʹ��
           TRUE : ��
           FALSE: ��
ע������  : �ޡ�
***********************************************************************/
extern int32_t chg_get_sys_batt_volt(void);

/**********************************************************************
�� �� ��      : void load_on_off_mode_parameter(void)
��������  :  ��ȡӲ�����Կ���ģʽNV
�������  :none
�������  : �ޡ�
�� �� ֵ      : �ޡ�
ע������  : ��Ԥ�������ǰ��Ҫ����
***********************************************************************/
void load_on_off_mode_parameter(void);
/**********************************************************************
�� �� ��  : void load_factory_mode_flag_init(void)
��������  : ��ȡ����ģʽ��־NV����
�������  : none
�������  : �ޡ�
�� �� ֵ  : �ޡ�
ע������  : �ޡ�
***********************************************************************/
void load_ftm_mode_init(void);

/*******************************************************************
Function:      chg_detect_batt_chg_for_shutoff
Description:   �ػ������������Դ�Ƴ������йػ�����
Data Accessed: ��
Data Updated:  ��
Input:         ��
Return:        ��
*******************************************************************/
void chg_detect_batt_chg_for_shutoff(void);

/*****************************************************************************
 �� �� ��  : chg_update_power_suply_info
 ��������  : ����power supply���ڵ��״̬��Ϣ
 �������  : void
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��5��10��
    ��    ��   : chendongguo
    �޸�����   : �����ɺ���

*****************************************************************************/
extern void chg_update_power_suply_info(void);

/*****************************************************************************
 �� �� ��  : chg_print_test_view_info
 ��������  : ��ά�ɲ��ӡ����
 �������  : void
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��6��17��
    ��    ��   : chendongguo
    �޸�����   : �����ɺ���

*****************************************************************************/
void chg_print_test_view_info(void);

/**********************************************************************
FUNCTION:    chg_charger_insert_proc
DESCRIPTION: usb remove process
INPUT:       chg_chgr_type_t chg_type
OUTPUT:      None.
RETURN:      None.
NOTE:        None
***********************************************************************/
void chg_charger_insert_proc(chg_chgr_type_t chg_type);
/**********************************************************************
FUNCTION:    chg_charger_remove_proc
DESCRIPTION: usb remove process
INPUT:       chg_chgr_type_t chg_type
OUTPUT:      None.
RETURN:      None.
NOTE:        None
***********************************************************************/
void chg_charger_remove_proc(chg_chgr_type_t chg_type);

/**********************************************************************
FUNCTION:    chg_get_batt_id_volt
DESCRIPTION: get batt id volt
INPUT:       None
OUTPUT:      None.
RETURN:      batt id volt
NOTE:        None
***********************************************************************/
int32_t chg_get_batt_id_volt(void);

#ifdef CONFIG_MBB_FAST_ON_OFF
/*****************************************************************************
 �� �� ��  : chg_get_system_suspend_status
 ��������  : ���ģ���ȡ�ٹػ�״̬�Ĵ�����������ٹػ��رն�����ͬʱ��
             ��USB����notify chains�����ر�USB ID�жϣ��˳��ٹػ�(����)�򿪶�����ͬʱ��
             ��USB����notify chains����ʹ��USB ID�жϣ�
 �������  : boolean suspend_status TRUE:����ٹػ�
                                    FALSE:�˳��ٹػ�
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��5��29��
    ��    ��   : chendongguo
    �޸�����   : �����ɺ���

*****************************************************************************/
extern void chg_get_system_suspend_status(ulong64_t suspend_status);
#endif/*CONFIG_MBB_FAST_ON_OFF*/

#if (MBB_CHG_WIRELESS == FEATURE_ON)
/*****************************************************************************
 �� �� ��  : chg_stm_set_wireless_online_st
 ��������  : get the wireless online status.
 �������  : ONLINE: ���߳����λ
             OFFLINE:���߳�粻��λ
 �������  : None
 �� �� ֵ  : VOID.
*****************************************************************************/
extern void chg_stm_set_wireless_online_st(boolean online);

/*****************************************************************************
 �� �� ��  : chg_stm_get_wireless_online_st
 ��������  : get the wireless online status.
 �������  :
 �������  : ONLINE: ���߳����λ
             OFFLINE:���߳�粻��λ
 �� �� ֵ  : VOID.
*****************************************************************************/
extern boolean chg_stm_get_wireless_online_st(void);

#endif/*MBB_CHG_WIRELESS*/

#if (MBB_CHG_EXTCHG == FEATURE_ON)
/*****************************************************************************
�� �� ��  : chg_stm_set_extchg_online_st
��������  : get the extchg online status.
�������  : ONLINE: ���߳����λ
          OFFLINE:���߳�粻��λ
�������  : None
�� �� ֵ  : VOID.
*****************************************************************************/
extern void chg_stm_set_extchg_online_st(boolean online);

/*****************************************************************************
�� �� ��  : chg_stm_get_extchg_online_st
��������  : get the extchg online status.
�������  : ONLINE: ���߳����λ
          OFFLINE:���߳�粻��λ
�������  : None
�� �� ֵ  : VOID.
*****************************************************************************/
boolean chg_stm_get_extchg_online_st(void);

/*****************************************************************************
 �� �� ��  : extchg_config_data_init
 ��������  : ������������ʼ���׶δ�userdata������ȡUI���õĶ������ͣ��
             ����ֵ���Ƿ�����ʹ�ܶ����������ֵ�����������ֵ������ΪĬ��-
             3.55Vֹͣ�����磬�����ܶ�����
 �������  : void
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��7��3��
    ��    ��   : chendongguo
    �޸�����   : �����ɺ���

*****************************************************************************/
void chg_extchg_config_data_init(void);

/*****************************************************************************
�� �� ��  : chg_get_extchg_online_status
��������  : ��ȡ������USB ID���Ƿ���λ��״̬����������power supply extchg online �ڵ�
         ��״̬
�������  : void
�������  : ��
�� �� ֵ  :ONLINE:USB ID����λ
          OFFLINE:USB ID�߲���λ
���ú���  :
��������  :

�޸���ʷ      :
1.��    ��   : 2013��5��11��
��    ��   : chendongguo
�޸�����   : �����ɺ���

*****************************************************************************/
extern boolean chg_get_extchg_online_status(void);

/******************************************************************************
*  Function:  chg_extchg_insert_proc
*  Description: �������豸����
*  Called by  :
*  Input      : None
*  Output     : None
*  Return     : None
*  Note       : �������豸����
********************************************************************************/
extern void chg_extchg_insert_proc(void);
/******************************************************************************
*  Function:  chg_extchg_remove_proc
*  Description: �������豸�γ�
*  Called by  :
*  Input      : None
*  Output     : None
*  Return     : None
*  Note       : �������豸�γ�
********************************************************************************/
extern void chg_extchg_remove_proc(void);
/******************************************************************************
*  Function:  chg_extchg_monitor_func
*  Description: ������������
*  Called by  :
*  Input      : None
*  Output     : None
*  Return     : None
*  Note       : ������״̬���
********************************************************************************/
extern void chg_extchg_monitor_func(void);

#endif/*MBB_CHG_EXTCHG*/

/*****************************************************************************
 �� �� ��  : chg_get_usb_online_status
 ��������  : ��ȡUSB�Ƿ���λ��״̬����������power supply USB online �ڵ�
             ��״̬
 �������  : void
 �������  : ��
 �� �� ֵ  :ONLINE:USB��λ
            OFFLINE:USB����λ
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��5��11��
    ��    ��   : chendongguo
    �޸�����   : �����ɺ���

*****************************************************************************/
boolean chg_get_usb_online_status(void);

/*****************************************************************************
 �� �� ��  : chg_set_usb_online_status
 ��������  : ��ȡUSB�Ƿ���λ��״̬����������power supply USB online �ڵ�
             ��״̬
 �������  : void
 �������  : ��
 �� �� ֵ  :1:USB��λ
            0:USB����λ
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��5��11��
    ��    ��   : chendongguo
    �޸�����   : �����ɺ���

*****************************************************************************/
void chg_set_usb_online_status(boolean online);

/*****************************************************************************
 �� �� ��  : chg_get_ac_online_status
 ��������  : ��ȡAC�Ƿ���λ��״̬����������power supply ac online �ڵ�
             ��״̬
 �������  : void
 �������  : ��
 �� �� ֵ  :ONLINE:AC��λ
            OFFLINE:AC����λ
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��5��11��
    ��    ��   : chendongguo
    �޸�����   : �����ɺ���

*****************************************************************************/
boolean chg_get_ac_online_status(void);

/*****************************************************************************
 �� �� ��  : chg_set_ac_online_status
 ��������  : ��ȡAC�Ƿ���λ��״̬����������power supply ac online �ڵ�
             ��״̬
 �������  : void
 �������  : ��
 �� �� ֵ  :ONLINE:AC��λ
            OFFLINE:AC����λ
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��5��11��
    ��    ��   : chendongguo
    �޸�����   : �����ɺ���

*****************************************************************************/
void chg_set_ac_online_status(boolean online);

/*****************************************************************************
 �� �� ��  : is_chg_charger_removed
 ��������  : �����������Դ�γ��жϺ���
 �������  : void
 �������  : ��
 �� �� ֵ  : TRUE:���Դ�γ�
             FALSE:���Դ��λ
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��5��29��
    ��    ��   : chendongguo
    �޸�����   : �����ɺ���

*****************************************************************************/
extern boolean is_chg_charger_removed(void);

/*****************************************************************************
 �� �� ��  : chg_hardware_paras_init
 ��������  : Ӳ����ز�����ʼ��
 �������  : void
 �������  : ��
 �� �� ֵ  : extern
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��5��19��
    ��    ��   : chendongguo
    �޸�����   : �����ɺ���

*****************************************************************************/
extern int32_t chg_hardware_paras_init(void);

/*****************************************************************************
 �� �� ��  : chg_get_batt_data
 ��������  : �����ز�����ʼ��
 �������  : batt id
 �������  : 
 �� �� ֵ  : batt data 
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��8��5��
    �޸�����   : �����ɺ���

*****************************************************************************/
extern struct chg_batt_data *chg_get_batt_data(unsigned int id_voltage);

/*****************************************************************************
 �� �� ��  : chg_batt_volt_paras_init
 ��������  : �����ز�����ʼ��
 �������  : none
 �������  : none
 �� �� ֵ  : 0:success;-1:fail
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��8��5��
    �޸�����   : �����ɺ���

*****************************************************************************/
extern int32_t chg_batt_volt_paras_init(void);

/*get batt brand*/
extern uint32_t chg_get_batt_id(void);
#endif /*__CHG_CHARGE_STM_H__*/

