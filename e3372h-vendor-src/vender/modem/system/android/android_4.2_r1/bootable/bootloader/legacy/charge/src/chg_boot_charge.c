/******************************�޸ļ�¼***************************************************************************************
  ����         �޸���          ���ⵥ��            �޸�����
******************************************************************************************************************************/
/******************************************************************
* Copyright (C),  2012-2018, HUAWEI Tech. Co., Ltd.
*
* File name:      chg_boot.c
*
* Description:    E5����һ��BOOT�׶����������
*
*

* Editer:         2012.10.29
* Version:        v1.0
*
* Function List:
*
����               ����                    �޸���            �޸�ԭ��

*******************************************************************/
/******************************���ⵥ�޸ļ�¼**********************************
    ����       �޸���       ���ⵥ��                 �޸�����
******************************************************************************/
#include "chg_boot_charge.h"
#include "chg_boot_charge_drv.h"
#include "chg_boot_chip_platform.h"
#include "balongv7r2/preboot.h"
#include "bsp_coul.h"

/*�����boot�׶β���ʹ��I2C��������Ҫ��GPIOģ������Ҫ����chg_boot_2c_virtual.hͷ�ļ�*/
//#include "chg_boot_2c_virtual.h"

unsigned char g_poweroff_charger_flag = FALSE;

#define SBL1_DELAY_500MS 500
#define SBL1_DELAY_1S 1000
#define SBL1_DELAY_2S 2000
#define SBL1_DELAY_3S 3000

boolean g_is_batt_over_discharge = FALSE;
/***********************************************************************************
�� �� ��  :chg_boot_set_batt_over_discharge_flag
��������  :���õ���Ƿ���ű�־
�������  :value
�������  :��
�� �� ֵ  : none
ע������  : 
***********************************************************************************/
void chg_boot_set_batt_over_discharge_flag(boolean value)
{
    g_is_batt_over_discharge = value;
}
/***********************************************************************************
�� �� ��  :chg_boot_is_batt_over_discharge
��������  :��ѯ����Ƿ����
�������  :none
�������  :��
�� �� ֵ  : TRUE:���ţ�FALSE:�ǹ���
ע������  : 
***********************************************************************************/
boolean chg_boot_is_batt_over_discharge(void)
{
    return g_is_batt_over_discharge;
}


/***********************************************************************************
�� �� ��  :chg_boot_batt_shorted_check
��������  :������Ƿ��·
�������  :��
�������  :��
�� �� ֵ  : TRUE����ض�·
            FALSE�����δ��·
ע������  : ������������ADC��ص�ѹ���ģ���ṩ�ĵ�ѹ��ȡ�ӿں�����������ṩ�ĳ��оƬʹ�ܽӿ�
***********************************************************************************/
boolean chg_boot_batt_shorted_check( void )
{
    int32_t batt_voltage = 0;
    int32_t index = 0;
	int32_t chagre_index = 0;
    int32_t tc_on = BOOT_PLUS_CHARGING_ON_TIME;    // ���ʱ��10s
    int32_t max_tc_on = BOOT_PLUS_CHARGING_ON_MAX_TIME;  //���5min
    
    chg_print_level_message( CHG_MSG_INFO,"PRE-CHG: batt_shorted_check BEGIN!!!\r\n ",0,0,0 );
        /*��ع���*/
        if(TRUE == chg_boot_batt_over_temp_check())
        {
            (void)chg_boot_set_charge_enable(FALSE);
            /*ֹͣ�����ʾ*/
            //chg_boot_display_interface( CHG_DISP_OVER_HEATED );
            return  FALSE;
        }

		for (chagre_index = 0; chagre_index < max_tc_on; chagre_index++)
		{
            /*��ع���*/
            if(TRUE == chg_boot_batt_over_temp_check())
            {
                (void)chg_boot_set_charge_enable(FALSE);
                /*ֹͣ�����ʾ*/
                //chg_boot_display_interface( CHG_DISP_OVER_HEATED );
                return  FALSE;
            }
            else
            {
                (void)chg_boot_set_charge_enable(TRUE);
            }
            /*���10s*/
            for (index = 0; index < tc_on; index++)
            {
                chg_boot_display_interface( CHG_DISP_CHARGING );
                chg_boot_delay_ms(SBL1_DELAY_500MS);
                chg_boot_display_interface( CHG_DISP_CHARGING );
                chg_boot_delay_ms(SBL1_DELAY_500MS);
                /*�߹�*/
                (void)chg_boot_set_tmr_rst();
                /*ϵͳ�߹�*/
                //BSP_WDT_Feed_Force();
                /*��ز���λ*/
                if(FALSE == chg_boot_batt_presence_check())
                {
                #if (MBB_CHG_PLATFORM_V7R2 == FEATURE_OFF)
                    chg_boot_batt_unpresence_handle();
                    #else
                    //(void)chg_boot_batt_unpresence_handle();
                    return  FALSE;
                    #endif
                }
                //chg_boot_delay_ms(SBL1_DELAY_1S);  /* */
                if(FALSE == chg_boot_is_charger_present())
                {
                    /*�����������Դ�γ�ֱ�ӷ���δ��·�����±ߵ����̾������Դ����λ������*/
                    chg_print_level_message( CHG_MSG_ERR,"PRE-CHG: charger output during batt_shorted_check!!!\r\n ",0,0,0 );
                    return  FALSE;
                }
                else
                {
                    //for lint
                }
            }

            (void)chg_boot_set_charge_enable(FALSE);
            chg_boot_delay_ms(SBL1_DELAY_2S);
                
            if((FALSE == chg_boot_batt_presence_check())
            || (FALSE == chg_boot_is_charger_present()))
            {
                return  FALSE;
            }
        
            batt_voltage = chg_boot_get_batt_volt_value();
            chg_print_level_message( CHG_MSG_INFO, "PRE-CHG: IN batt_shorted_check vbat=%d\r\n ", batt_voltage, 0, 0 );

    		if(batt_voltage >= BAT_CHECK_SHORTED_VOLT)
            {
                batt_voltage = chg_boot_get_batt_volt_value();
                if(batt_voltage >= BAT_CHECK_SHORTED_VOLT)
                {
                    batt_voltage = chg_boot_get_batt_volt_value();
                    if(batt_voltage >= BAT_CHECK_SHORTED_VOLT)
                    {
                        chg_print_level_message( CHG_MSG_ERR,"PRE-CHG:batt_shorted_check: vbat=%d\n",batt_voltage,0,0);
                        return  FALSE;
                    }
                }
            }

		}

    chg_print_level_message( CHG_MSG_INFO,"PRE-CHG: batt_shorted_check END!!!\n ",0,0,0 );
    return  TRUE;
}

/***********************************************************************************
�� �� ��  :chg_boot_batt_presence_check
��������  :������Ƿ���λ
�������  :��
�������  :��
�� �� ֵ  : TRUE��    �����λ
                  FALSE����ز���λ
ע������  : ������������ADC��ص�ѹ���ģ���ṩ�ĵ�ѹ��ȡ�ӿں�����������ṩ�ĳ��оƬʹ�ܽӿ�
***********************************************************************************/
boolean chg_boot_batt_presence_check( void )
{
    int32_t batt_temp = 0;

    batt_temp = chg_boot_get_temp_value(CHG_PARAMETER__BATT_THERM_DEGC);
    if(batt_temp <= BATT_NOT_PRESENCE_TEMP)
    {
        batt_temp = chg_boot_get_temp_value(CHG_PARAMETER__BATT_THERM_DEGC);
        if(batt_temp <= BATT_NOT_PRESENCE_TEMP)
        {
            batt_temp = chg_boot_get_temp_value(CHG_PARAMETER__BATT_THERM_DEGC);
            if(batt_temp <= BATT_NOT_PRESENCE_TEMP)
            {
                chg_print_level_message( CHG_MSG_ERR,"PRE-CHG: batt_presence_check: battary unpresence!!!\r\n ",0,0,0 );
                return FALSE;
            }
        }
    }
    return TRUE;
}

/***********************************************************************************
�� �� ��  :chg_boot_batt_unpresence_handle
��������  :��⵽��ز���λʱ����
�������  :��
�������  :��
�� �� ֵ  : TRUE��    �����λ
                  FALSE����ز���λ
ע������  : ������������ADC��ص�ѹ���ģ���ṩ�ĵ�ѹ��ȡ�ӿں�����������ṩ�ĳ��оƬʹ�ܽӿ�
***********************************************************************************/

void chg_boot_batt_unpresence_handle( void )
{
    /* 9x25ƽ̨����Ҫ����ֵ */

    for ( ; ; )
    {
        /*����OLED/LEDģ���ṩ�Ľӿ���ʾ���в���λ��ʾ*/
        chg_boot_display_interface( CHG_DISP_BATTERY_BAD );
        chg_boot_delay_ms(SBL1_DELAY_500MS);
        chg_boot_display_interface( CHG_DISP_BATTERY_BAD );
        chg_boot_delay_ms(SBL1_DELAY_500MS);
        if ( TRUE == chg_boot_batt_presence_check() )
        {
            break;
        }

        //��������������ṩ�����Դ��λ���ӿڣ�
        if ( FALSE == chg_boot_is_charger_present() )
        {
            chg_boot_delay_ms(SBL1_DELAY_1S);
            /*2��    ���ùػ��ӿڹػ�*/
            chg_boot_set_power_off(DRV_SHUTDOWN_LOW_BATTERY);
        }
    }
    /*2�����ùػ��ӿڹػ��������Դ��λ����������*/
    last_shutdown_reason_set( POWER_OFF_REASON_LOW_BATTERY );
    chg_boot_set_power_off(DRV_SHUTDOWN_LOW_BATTERY);

}


/***********************************************************************************
�� �� ��  :chg_boot_batt_over_temp_check
��������  :������Ƿ����
�������  :��
�������  :��
�� �� ֵ  : FALSE�� ����¶�����
            TRUE��  ��ع���
ע������  : ������������ADC��ص�ѹ���ģ���ṩ�ĵ�ѹ��ȡ�ӿں�����������ṩ�ĳ��оƬʹ�ܽӿ�
***********************************************************************************/
boolean chg_boot_batt_over_temp_check( void )
{
    int32_t batt_temp = 0;
    boolean rtnValue = FALSE;
    static boolean last_rtnValue = FALSE;

    batt_temp = chg_boot_get_temp_value(CHG_PARAMETER__BATT_THERM_DEGC);
    chg_print_level_message( CHG_MSG_ERR, "PRE-CHG: batt_temp=%d!\r\n ",batt_temp,0,0 );
    /*����45��С��0��ͣ��*/
    if(FALSE == last_rtnValue)
    {
        if(((batt_temp > BATT_STOP_CHARGINE_LOW_TEMP) && (batt_temp < BATT_STOP_CHARGINE_HIGH_TEMP))
            || (batt_temp <= BATT_NOT_PRESENCE_TEMP))
        {
            rtnValue = FALSE;
        }
        else
        {
            chg_print_level_message( CHG_MSG_ERR,"PRE-CHG: batt_over_temp_check:batt over temp!\r\n ",0,0,0 );
            rtnValue = TRUE;
        }

    }
    /*����3��С��42�ȸ���*/
    else
    {
        if(((batt_temp > BATT_RECHARGINE_LOW_TEMP) && (batt_temp < BATT_RECHARGINE_HIGH_TEMP))
            || (batt_temp <= BATT_NOT_PRESENCE_TEMP))
        {
            chg_print_level_message( CHG_MSG_ERR,"PRE-CHG: batt_over_temp_check:batt temp resum!\r\n ",0,0,0 );
            rtnValue = FALSE;
        }
        else
        {
            rtnValue = TRUE;
        }
    }

    last_rtnValue = rtnValue;
    return rtnValue;
}
/**********************************************************************
�� �� ��      : chg_basic_init
��������  :  ������ʼ��:
                          ���оƬ+˫��ͨ��
�������  : ��
�������  : ��
�� �� ֵ      : ��
ע������  : ��Ԥ��粿�ֵ���
                         �ɸ��ݾ���ƽ̨���ɾ��
***********************************************************************/
void chg_basic_init( void )
{
    static int32_t s_chg_basic_init_flag = FALSE;
    chg_print_level_message(CHG_MSG_DEBUG,"CHG_TASK:chg_basic_init \n ",0,0,0 );

    if( FALSE == s_chg_basic_init_flag )
    {
        s_chg_basic_init_flag = TRUE;

/***************Note:ƽ̨��ش��룬����ƽ̨����Ҫ��ӣ��е�ƽ̨��V7R1��Ҫ
              ��ֲ��Ա������Ҫ����ӻ����Ƴ��±ߺ�������***************************/
        /* �˼�ͨ�ų�ʼ��*/
        //BSP_CHGC_Init();

        /* ���оƬ��ʼ��*/
        (void)chg_boot_chip_init();
    }
}

/***********************************************************************************
�� �� ��  :chg_boot_low_power_trickle_charg_check
��������  :��ض�·�͹��ż��
�������  :��
�������  :��
�� �� ֵ  : ���״̬
ע������  : ������������ADC��ص�ѹ���ģ���ṩ�ĵ�ѹ��ȡ�ӿں�����������ṩ�ĳ��оƬʹ�ܽӿ�
***********************************************************************************/
#if (MBB_CHG_PLATFORM_V7R2 == FEATURE_OFF)
/*9X25ƽ̨ʹ�ô˺����ӿ�*/
void chg_boot_low_power_trickle_charg_check(bl_shared_data_type *bl_shared_data)
#else
void chg_boot_low_power_trickle_charg_check( void )
#endif/*MBB_CHG_PLATFORM_V7R2 == NO*/
{
    int32_t batt_voltage = 0;
    int32_t wait_idx  = 0;
    int32_t trickle_charg_time_count = 0;
    boolean is_over_discharge = FALSE;

/***************Note:ƽ̨��ش��룬����ƽ̨����Ҫ��ӣ��е�ƽ̨��9x25��Ҫ
              ��ֲ��Ա������Ҫ����ӻ����Ƴ��±ߺ�������***************************/
    //�ж��Ƿ��ǹػ����ģʽ������������͹���ģʽ
    #if (MBB_CHG_PLATFORM_V7R2 == FEATURE_OFF)
    if (TRUE == chg_boot_is_powdown_charging())
    {
        g_poweroff_charger_flag = TRUE;
        #if 0 /*PN:dongle121, chendongguo, 2013/6/27 deleted*/
        /*��������ģʽ������recovery ǰ����������������ص����*/
        /* Iterate over config table to find appsbl entry */
        while (sbl1_config_table[index].host_img_id != NULL)
        {
            if ((sbl1_config_table[index].target_img_sec_type != SECBOOT_APPSBL_SW_TYPE ) &&
                (sbl1_config_table[index].target_img_sec_type != SECBOOT_TZ_KERNEL_SW_TYPE) &&
                (sbl1_config_table[index].target_img_sec_type != SECBOOT_WDT_SW_TYPE) &&
                (sbl1_config_table[index].target_img_sec_type != SECBOOT_RPM_FW_SW_TYPE))
            {
                sbl1_config_table[index].load = FALSE;
                index++;
                continue;
            }
            index++;
        }
        #endif
    }
    #endif

    chg_basic_init();


    /*����ģʽ��ֱ������������*/
    /*��ά�ɲ�:�����ز���λ����*/
    if( ( TRUE == chg_boot_is_ftm_mode () ) \
        || ( TRUE == chg_boot_is_exception_poweroff_poweron_enable() ) \
        || ( TRUE == chg_boot_is_no_battery_powerup_enable() ) )
    {
        set_pw_charge_flag(FALSE);
        return ;

    }
    /*��Ҫ�ȴ����ؼ��ȶ������ʹ��*/
    #ifdef CONFIG_COUL
    bsp_coul_ready();
    #endif
    //�����������������Դ��λ���Ľӿ�
    if ( TRUE == chg_boot_is_charger_present() )
    {
        if ( FALSE == chg_boot_batt_presence_check() )
        {

            chg_boot_batt_unpresence_handle();

        }
        else
        {
            /*��ӵ�ص�ѹ��⣬�����ص�ѹ����3.0���˳����*/
#ifdef CONFIG_COUL
            int32_t ocv_voltage = 1;
            int32_t count = 3;
            ocv_voltage = bsp_coul_ocv_get();

            if(0 == ocv_voltage)
            {
                /*��ؿ��ܹ��ţ���ʱ���ؼƲɼ���ѹ��Ч����Ҫ�����*/
                is_over_discharge = TRUE;
                while(count--)
                {
                    chg_boot_set_charge_enable(TRUE);
                    chg_boot_delay_ms(SBL1_DELAY_3S);
                    chg_boot_set_charge_enable(FALSE);
                    chg_boot_delay_ms(SBL1_DELAY_1S);
                }
            }
            batt_voltage = chg_boot_get_batt_volt_value();
#else
            batt_voltage = chg_boot_get_batt_volt_value();
            if(0 == batt_voltage)
            {
                is_over_discharge = TRUE;
            }
#endif
            chg_print_level_message( CHG_MSG_ERR,"PRE-CHG: charge in and vbat=%d\r\n ",batt_voltage,0,0 );
            if ( batt_voltage > BAT_CHECK_JUDGMENT_VOLT )
            {
                /* 9x25ƽ̨����Ҫ����ֵ */
                if ((batt_voltage < BAT_CHECK_POWERON_VOLT)
                    && (FALSE == chg_boot_is_powdown_charging()))
                {
                    chg_boot_display_interface( CHG_DISP_BATTERY_LOWER );

                    chg_boot_delay_ms(SBL1_DELAY_3S);
                    chg_boot_display_interface( CHG_DISP_OK );

                    chg_boot_set_powdown_charging(); //lint -e628
                }
                else
                {
                    //do nothing
                }
                return;

            }

            /*�����ض�·*/
            if ( TRUE == chg_boot_batt_shorted_check() )
            {
                /* 9x25ƽ̨����Ҫ����ֵ */

                /*1������OLED/LEDģ���ṩ�Ľӿ���ʾ���л������ʾ */

                for(;;)
                {
                    /*����OLED/LEDģ���ṩ�Ľӿ���ʾ���л������ʾ*/
                    chg_boot_display_interface( CHG_DISP_BATTERY_BAD );
                    if(FALSE == chg_boot_is_charger_present())
                    {
                        chg_boot_delay_ms(SBL1_DELAY_1S);
                        /*���ùػ��ӿڹػ�*/
                        chg_boot_set_power_off(DRV_SHUTDOWN_LOW_BATTERY);
                    }
                    else
                    {
                        chg_boot_delay_ms(SBL1_DELAY_500MS);
                    }
                }

            }
            else
            {
                if(is_over_discharge)
                {
                    chg_boot_set_batt_over_discharge_flag(TRUE);
                    chg_print_level_message(CHG_MSG_ERR, "PRE-CHG:batt is over discharge!\n",0,0,0);
                }
                #if (MBB_CHG_PLATFORM_V7R2 == FEATURE_ON)
                if ( FALSE == chg_boot_batt_presence_check() )
                {
                    chg_boot_batt_unpresence_handle();
                }
                if (FALSE == chg_boot_is_charger_present())
                {
                    chg_boot_set_power_off(DRV_SHUTDOWN_LOW_BATTERY);
                }
                #endif
                #ifdef CONFIG_COUL
                /*�ս���丳�ǰ������һ�ο��ؼ�ǿ��У׼*/
                chg_print_level_message(CHG_MSG_ERR, "PRE-CHG:bsp_coul_cail_on COUL_CAIL_ON");
                bsp_coul_cail_on();
                bsp_coul_ready();
                #endif
                batt_voltage = chg_boot_get_batt_volt_value();
                chg_print_level_message( CHG_MSG_ERR, "PRE-CHG: Before while poll vbat=%d\r\n ", batt_voltage, 0, 0 );
                if(batt_voltage <= BAT_CHECK_JUDGMENT_VOLT)
                {
                    while(trickle_charg_time_count < BOOT_TRICKLE_CHARGE_TIME)
                    {
                        if (FALSE == chg_boot_is_charger_present())
                        {
                            /* 9x25ƽ̨����Ҫ����ֵ */
                            chg_boot_delay_ms(SBL1_DELAY_1S);
                            /* 1,���ùػ��ӿڹػ�*/
                            chg_boot_set_power_off(DRV_SHUTDOWN_LOW_BATTERY);
                        }

                        /*����¶�����*/
                        if (FALSE == chg_boot_batt_over_temp_check())
                        {
                            /*ʹ�ܳ��*/
                            (void)chg_boot_set_charge_enable( TRUE );


                            /*��ʾ��綯��*/
                            //chg_boot_display_interface( CHG_DISP_CHARGING );

                            /*��10S��*/
                            for ( wait_idx = 0; wait_idx < BOOT_TRICKLE_CHARGING_TIME;wait_idx++)
                            {
                                //�߹�
                                (void)chg_boot_set_tmr_rst();


                                chg_boot_display_interface( CHG_DISP_CHARGING );
                                chg_boot_delay_ms(SBL1_DELAY_500MS);
                                chg_boot_display_interface( CHG_DISP_CHARGING );

                                chg_boot_delay_ms( SBL1_DELAY_500MS );  /* */
                                /*��Դ����λ*/
                                if (FALSE == chg_boot_is_charger_present())
                                {
                                    /* 9x25ƽ̨����Ҫ����ֵ */
                                    chg_boot_delay_ms(SBL1_DELAY_1S);
                                    //���ùػ��ӿڹػ�
                                    chg_boot_set_power_off(DRV_SHUTDOWN_LOW_BATTERY);
                                    
                                }
                                /*��ز���λ*/
                                if(FALSE == chg_boot_batt_presence_check())
                                {
                                    chg_boot_batt_unpresence_handle();

                                }
                                /*����������е�ض�·���*/
                                batt_voltage = chg_boot_get_batt_volt_value();
                                if(batt_voltage < BAT_CHECK_SHORTED_VOLT)
                                {
                                    batt_voltage = chg_boot_get_batt_volt_value();
                                    if(batt_voltage < BAT_CHECK_SHORTED_VOLT)
                                    {
                                        batt_voltage = chg_boot_get_batt_volt_value();
                                        if(batt_voltage < BAT_CHECK_SHORTED_VOLT)
                                        {
                                            chg_print_level_message(CHG_MSG_ERR,
                                                "PRE-CHG:batt_shorted_check:battary shorted\n",0,0,0);
                                            /* 9x25ƽ̨����Ҫ����ֵ */

                                            for(;;)
                                            {
                                                /*����OLED/LEDģ���ṩ�Ľӿ���ʾ���л������ʾ*/
                                                chg_boot_display_interface( CHG_DISP_BATTERY_BAD );
                                                if(FALSE == chg_boot_is_charger_present())
                                                {
                                                    chg_boot_delay_ms(SBL1_DELAY_1S);
                                                    /*���ùػ��ӿڹػ�*/
                                                    chg_boot_set_power_off(DRV_SHUTDOWN_LOW_BATTERY);
                                                }
                                                else
                                                {
                                                    chg_boot_delay_ms(SBL1_DELAY_500MS);
                                                }
                                            }

                                        }
                                    }
                                }
                            }
                            trickle_charg_time_count++;
                        }
                        /*����¶��쳣*/
                        else
                        {

                            /*����¶��쳣�ڼ䲻����30����Ԥ����ʱ*/
                            /*��ֹ���*/
                            (void)chg_boot_set_charge_enable(FALSE);
#if (MBB_CHG_LED == FEATURE_ON)
                            chg_boot_display_interface( CHG_DISP_BATTERY_LOWER );
#else
                            /*ֹͣ�����ʾ*/
                            chg_boot_display_interface( CHG_DISP_OVER_HEATED );
#endif
                            chg_boot_delay_ms(SBL1_DELAY_3S);
                        }

                        batt_voltage = chg_boot_get_batt_volt_value();
                        chg_print_level_message( CHG_MSG_ERR, "PRE-CHG: vbat=%d\r\n ", batt_voltage, 0, 0 );
                        chg_print_level_message( CHG_MSG_ERR, "PRE-CHG: charging=%d\r\n ",
                        chg_boot_is_IC_charging(),0,0 );
                        /*����������ڼ��ص�ѹ����3.0V����������������п�������*/
                        if(batt_voltage > BAT_CHECK_JUDGMENT_VOLT)
                        {
                            chg_print_level_message( CHG_MSG_ERR, "PRE-CHG: vbat=%d,exit pre-charging!\r\n ", batt_voltage, 0, 0 );
                            break;
                        }
                    }
                    /*丳�ʱ�����30����*/
                    if( trickle_charg_time_count >= BOOT_TRICKLE_CHARGE_TIME)
                    {
                        /*��������������ṩ�Ľӿڽ�ֹ���*/
                        (void)chg_boot_set_charge_enable(FALSE);
                        /* 9x25ƽ̨����Ҫ����ֵ */

                        for(;;)
                        {
                            /*����OLED/LEDģ���ṩ�Ľӿ���ʾ���л������ʾ*/
                            chg_boot_display_interface( CHG_DISP_BATTERY_BAD );
                            if(FALSE == chg_boot_is_charger_present())
                            {
                                chg_boot_delay_ms(SBL1_DELAY_1S);
                                /*���ùػ��ӿڹػ�*/
                                chg_boot_set_power_off(DRV_SHUTDOWN_LOW_BATTERY);
                            }
                            else
                            {
                                chg_boot_delay_ms(SBL1_DELAY_500MS);
                            }
                        }
                    }
                }
            }
        }
    }
    else
    {
        //�жϵ���Ƿ���λ����ֹNTC��·�ĳ������忪����
        if ( FALSE == chg_boot_batt_presence_check() )
        {
            chg_boot_batt_unpresence_handle();
        }
        else
        {
            batt_voltage = chg_boot_get_batt_volt_value();
            chg_print_level_message( CHG_MSG_ERR,"PRE-CHG: trickle charg batt only batt_voltage=%d\r\n ",batt_voltage,0,0 );
            if(batt_voltage > BAT_CHECK_POWERON_VOLT)
            {

                return;

            }
            else
            {
                /* 9x25ƽ̨����Ҫ����ֵ */

                /*1�����õ͵����ѽӿڽ��е͵�����*/
                chg_boot_display_interface( CHG_DISP_BATTERY_LOWER );

                chg_boot_delay_ms(SBL1_DELAY_3S);

                /* 2�����ùػ��ӿڽ��йػ�*/
                chg_boot_set_power_off(DRV_SHUTDOWN_LOW_BATTERY);

            }

        }
    }
}



