/***********************************************************************************************

                  ��Ȩ���� (C), 2013-2016, ��Ϊ�ն����޹�˾

 ***********************************************************************************************
  �� �� ��   : led_boot.c
  �� �� ��   : ����
  ��������   : 2014/7/23
  ����޸�   :
  ��������   : ��ģ���boot �׶��ṩled�ƵĿ�������ӿ�: 
               
***********************************************************************************************/
 /*******************************�޸ļ�¼��ʼ**********************************
����           �޸���          ���ⵥ��                 �޸�����
********************************�޸ļ�¼����**********************************/
#include "bsp_pmu.h"
#include <balongv7r2/gpio.h>
#include <mbb_leds.h>

#define long_time    5000000    /*dr�����Ĵ���ֵ*/

#ifdef BSP_CONFIG_BOARD_E5
#define current    4500    /*��λuA��E5�ĵ�����Ϊ4.5mA*/
#else
#define current    3000    /*��λuA��������Ϊ3mA*/
#endif

int g_dr_id = 0;    /*ȫ�ֱ���dr����id*/

static int strncmp(const char *s1, const char *s2, int n)
{
    if (n == 0)
    {
        return (0);
    }
   
    do {
            if (*s1 != *s2++)
            {
                return (*(unsigned char *)s1 - *(unsigned char *)--s2);
            }

            if (*s1++ == 0)
            {
                break;
            }

        } while (--n != 0);
    return (0);
}

#if(FEATURE_ON == MBB_LED_DR)
/************************************************************************
 *����ԭ�� �� static int change_name(const char *name)
 *����     �� ���ݲ�����led����ѡ��dr����id
 *����     ��������led����
 *���     �� ��
 *����ֵ   �� �ɹ�0��ʧ��-1
 *�޸���ʷ     :
   ��    ��   : 20140723
   �޸�����   : �����ɺ���
*************************************************************************/
static int change_name(const char *name)
{
    /* reject null */
    if(NULL == name)
    {
        cprintf("[%s] name is null. \n", __func__);
        return  - 1;
    }


    if(!strncmp(name, LED_NODE_1, strlen(LED_NODE_1)))
    {
        g_dr_id = PMU_DR01;
        return 0;
    }
    else if(!strncmp(name, LED_NODE_2, strlen(LED_NODE_2)))
    {
        g_dr_id = PMU_DR02;
        return 0;
    }
    else if(!strncmp(name, LED_NODE_3, strlen(LED_NODE_3)))
    {
        g_dr_id = PMU_DR03;
        return 0;
    }
    else if(!strncmp(name, LED_NODE_4, strlen(LED_NODE_4)))
    {
        g_dr_id = PMU_DR04;
        return 0;
    }
    else if(!strncmp(name, LED_NODE_5, strlen(LED_NODE_5)))
    {
        g_dr_id = PMU_DR05;
        return 0;
    }
    else
    {
        cprintf("[%s] match name is fail. \n", __func__);
        return  -1;
    }
    
}

/************************************************************************
 *����ԭ�� �� static void dr_led_switch(int numb, int onoff)
 *����     �� dr����led��״̬�л�����
 *����     ����Ӧ�����±�int numb, ״̬ int onoff
 *���     �� ��
 *����ֵ   �� 
 *�޸���ʷ     :
   ��    ��   : 20140723
   �޸�����   : �����ɺ���
*************************************************************************/
static void dr_led_switch(int numb, int onoff)
{
        int ret = 0;
        DR_BRE_TIME dr_bre_time_st;
 
        ret = change_name(led_dr[numb].name);
        if(0 != ret)
        {
            return;
        }

        else
        {
            /* ���ú���������ֵ */
            (void)bsp_dr_current_set(g_dr_id, current, current);

            /* ���ú�����ʱ�� �˴�ֻ���ó���ʱ�䣬�����ܿ��ؿ���*/
            dr_bre_time_st.bre_fall_ms = 0;
            dr_bre_time_st.bre_off_ms = 0;
            dr_bre_time_st.bre_on_ms = (unsigned int)(long_time);
            dr_bre_time_st.bre_rise_ms = 0;
            (void)bsp_dr_bre_time_set(g_dr_id, &dr_bre_time_st);

            if(PMU_DR02 < g_dr_id)
            {
                bsp_dr_start_delay_set(g_dr_id, 0);    /*dr345��Ҫ��ʱ��������*/
            }

            /* �����ƿ��� */
            (void)bsp_dr_switch(g_dr_id, onoff);
        
        }

    return;
}
#endif

#if(FEATURE_ON == MBB_LED_GPIO)
/************************************************************************
 *����ԭ�� �� static void gpio_led_switch(int numb, int onoff)
 *����     �� gpio����led��״̬�л�����
 *����     ����Ӧ�����±�int numb, ״̬ int onoff
 *���     �� ��
 *����ֵ   �� 
 *�޸���ʷ     :
   ��    ��   : 20140723
   �޸�����   : �����ɺ���
*************************************************************************/
static void gpio_led_switch(int numb, int onoff)
{    
    (void)gpio_direction_output(gpio_exp_leds_config[numb].gpio, onoff);   
 
    return;
}
#endif

/************************************************************************
 *����ԭ�� �� int led_boot_status_set(char* led_name, int onoff)
 *����     �� �ṩboot�׶� ��led�ƿ��ƽӿ�
 *����     ��Ҫ���Ƶ�led������, ״̬ int onoff
 *���     �� ��
 *����ֵ   �� �ɹ�0��ʧ��-1
 *�޸���ʷ     :
   ��    ��   : 20140723
   �޸�����   : �����ɺ���
*************************************************************************/
int led_boot_status_set(char* led_name, int onoff)
{
    int i = 0;
    int num = 0;
    
  
    if(NULL == led_name)
    {
        cprintf("[%s] input led_name is NULL. \n",  __func__);
        return - 1;
    }
    if((LEDS_GPIO_DEFSTATE_OFF != onoff) && (LEDS_GPIO_DEFSTATE_ON != onoff))
    {
        cprintf("[%s] input onoff = [%d] wrong. \n", __func__, onoff);
        return -1;
    }
    
#if(FEATURE_ON == MBB_LED_GPIO)
    num = (int)(ARRAY_SIZE(gpio_exp_leds_config));

    for(i = 0; i < num; i++)     /*����name  ���Ҷ�Ӧ���Ƶ�led*/
    {
        if(!strncmp(gpio_exp_leds_config[i].name , led_name , strlen(led_name) ))
        {
            gpio_led_switch( i , onoff );      /*���ҳɹ�gpio���Ƶ���*/
            return 0;    
        }       
    }
#endif

#if(FEATURE_ON == MBB_LED_DR)
    num = (int)(ARRAY_SIZE(led_dr));

    for(i = 0; i < num; i++)     /*����name  ���Ҷ�Ӧ���Ƶ�led*/
    {
        if(!strncmp(led_dr[i].name , led_name , strlen(led_name) ))
        {
            dr_led_switch( i , onoff);      /*���ҳɹ�dr���Ƶ���*/
            return 0;    
        }       
    }
#endif

    cprintf("[%s] input led_name no found. \n", __func__);  /*����ʧ��*/

return  -1;

}






