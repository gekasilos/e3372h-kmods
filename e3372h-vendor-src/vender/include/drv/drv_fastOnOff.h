/******************************************************************************
  Copyright, 1988-2013, Huawei Tech. Co., Ltd.
  File name:      drv_fastOnOff.h
  Author:         �쳬 x00202188
  Version:        V1.0 
  Date:           �쳬     ��������      2013-05-15
  Description:    ���ٿ��ػ������ṩ���ⲿʹ�õĽӿںͱ�����
  Others:         
******************************************************************************/
#ifndef __FASTONOFF_H__
#define __FASTONOFF_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <product_config.h>

#if (FEATURE_ON == MBB_FAST_ON_OFF)
#include <linux/notifier.h>

/**************************************************************************
  �������ṹ���� 
**************************************************************************/
typedef enum
{

    FASTONOFF_MODE_CLR = 0,       /*��������״̬*/
    FASTONOFF_MODE_SET = 1,       /*�ٹػ�״̬  */
    FASTONOFF_MODE_MAX = 2,       /*�Ƿ�ֵ*/
}FASTONOFF_MODE;

typedef enum
{
    FASTONOFF_FAST_OFF_MODE = 5, /*����ٹػ�ģʽ*/
    FASTONOFF_FAST_ON_MODE  = 6, /*�˳��ٹػ�ģʽ*/
    FASTONOFF_IOCTL_CMD_MAX,      /*�Ƿ�ֵ*/
}FASTONOFF_IOCTL_CMD;

/* �ٹػ�֪ͨ����ע�ᵽ�����ĺ��������ڽ��롢�˳��ٹػ�ʱ������ */
extern struct blocking_notifier_head g_fast_on_off_notifier_list;

/**************************************************************************
  �������� 
**************************************************************************/

/********************************************************
*������	  : fastOnOffGetFastOnOffMode
*�������� : ��ȡ���ٿ��ػ���־λ�������ж��Ƿ����ٹػ�״̬
*������� : ��
*������� : ��
*����ֵ	  : ִ�гɹ����ؼٹػ�״̬��־��ʧ�ܷ���-1
*�޸���ʷ :
*	       2013-5-15 �쳬 ��������
********************************************************/
extern FASTONOFF_MODE fastOnOffGetFastOnOffMode(void);

#endif /* #if (FEATURE_ON == MBB_FAST_ON_OFF) */

#ifdef __cplusplus
}
#endif

#endif

