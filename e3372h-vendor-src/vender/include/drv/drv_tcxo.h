/*************************************************************************
*   ��Ȩ����(C) 1987-2011, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  drv_clk.h
*
*   ��    �� :  xujingcui
*
*   ��    �� :  ���ļ�����Ϊ"drv_clk", ����V7R2�����Э��ջ֮���CLK_API�ӿ�
*
*   �޸ļ�¼ :  2013��1��18��  v1.00 xujingcui����
*************************************************************************/
#ifndef __DRV_TCXO_H__
#define __DRV_TCXO_H__

#include <drv_dpm.h>
typedef BSP_S32 (*PWC_TCXO_FUNCPTR)(int);

/***************************************************************************
 �� �� ��  : BSP_TCXO_RfclkStatus
 ��������  : ��ȡRF CLK״̬
 �������  :
		enModemId  Modem ID
 �������  : ��
 �� �� ֵ  : PWRCTRL_COMM_ON         ʹ��;
             PWRCTRL_COMM_OFF        ��ֹ;
             RET_ERR                 �����쳣
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��6��24��
    ��    ��   : ��ɺ 212992
    �޸�����   : �����ɺ���
**************************************************************************/
 int BSP_TCXO_RfclkStatus(PWC_COMM_MODEM_E enModemId);
#define DRV_TCXO_RFCLKSTATUS(enModemId) BSP_TCXO_RfclkStatus(enModemId)
/*****************************************************************************
 �� �� ��  : DRV_TCXO_RfclkEnable
 ��������  : ͨ��ģ�����RF 19.2Mʱ��ʹ��
 �������  : ��
 �������  : ��
 �� �� ֵ  : RET_OK         �ɹ�
             RET_ERR        ʧ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��6��24��
    ��    ��   : ��ɺ 212992
    �޸�����   : �����ɺ���

*****************************************************************************/
 int BSP_TCXO_RfclkEnable(PWC_COMM_MODEM_E  enModemId, PWC_COMM_MODE_E enModeType);
#define DRV_TCXO_RFCLKENABLE(enModemId, enModeType) BSP_TCXO_RfclkEnable(enModemId, enModeType)
/*****************************************************************************
 �� �� ��  : BSP_TCXO_RfclkDisable
 ��������  : ͨ��ģ�����RF 19.2Mʱ��ȥʹ��
 �������  : ��
 �������  : ��
 �� �� ֵ  : RET_OK         �ɹ�
             RET_ERR        ʧ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��6��24��
    ��    ��   : ��ɺ 212992
    �޸�����   : �����ɺ���

*****************************************************************************/
 int BSP_TCXO_RfclkDisable(PWC_COMM_MODEM_E  enModemId, PWC_COMM_MODE_E enModeType);
#define DRV_TCXO_RFCLKDISABLE(enModemId, enModeType) BSP_TCXO_RfclkDisable(enModemId, enModeType)
/***************************************************************************
 �� �� ��  : DRV_TCXO_GETSTATUS
 ��������  : ��ȡTCXO״̬
 �������  :
		enModemId  Modem ID
 �������  : ��
 �� �� ֵ  : PWRCTRL_COMM_ON         ʹ��;
             PWRCTRL_COMM_OFF        ��ֹ;
             RET_ERR                 �����쳣
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��4��19��
    ��    ��   : ����ΰ 00176398
    �޸�����   : �����ɺ���
**************************************************************************/
 int bsp_tcxo_getstatus(PWC_COMM_MODEM_E enModemId);
#define DRV_TCXO_GETSTATUS(enModemId)   bsp_tcxo_getstatus(enModemId)
/***************************************************************************
 �� �� ��  : DRV_TCXO_ENABLE
 ��������  : ʹ��TCXO
 �������  : enModemId  Modem ID
	         enModeType    Mode type
 �������  : ��
 �� �� ֵ  : RET_OK         �ɹ�
             RET_ERR        ʧ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��4��19��
    ��    ��   : ����ΰ 00176398
    �޸�����   : �����ɺ���
**************************************************************************/
 int bsp_tcxo_enable(PWC_COMM_MODEM_E  enModemId, PWC_COMM_MODE_E enModeType);
#define DRV_TCXO_ENABLE(enModemId, enModeType)  bsp_tcxo_enable(enModemId, enModeType)
/***************************************************************************
 �� �� ��  : DRV_TCXO_DISABLE
 ��������  : �ر�TCXO
 �������  : enModemId  Modem ID
	         enModeType    Mode type
 �������  : ��
 �� �� ֵ  : RET_OK         �ɹ�
             RET_ERR        ʧ��
	         RET_ERR_NOT_FOUND û��ͶƱʹ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��4��19��
    ��    ��   : ����ΰ 00176398
    �޸�����   : �����ɺ���
**************************************************************************/
 int bsp_tcxo_disable(PWC_COMM_MODEM_E  enModemId, PWC_COMM_MODE_E enModeType);
#define DRV_TCXO_DISABLE(enModemId, enModeType) bsp_tcxo_disable(enModemId, enModeType)
/***************************************************************************
 �� �� ��  : DRV_TCXO_TIMER_START
 ��������  : TCXO ��ʱ������
 �������  : enModemId  Modem ID
    	     enModeType    Mode type
    	     routine:�ص�����  ע:�ص��������ж���������ִ��
             arg:�ص�����
             timerValue:����ֵ ��λms
 �������  : ��
 �� �� ֵ  : RET_OK         �ɹ�
             RET_ERR        ʧ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��4��19��
    ��    ��   : ����ΰ 00176398
    �޸�����   : �����ɺ���
**************************************************************************/
 int bsp_tcxo_timer_start(PWC_COMM_MODEM_E  enModemId, PWC_COMM_MODE_E enModeType, PWC_TCXO_FUNCPTR routine, int arg, unsigned int timerValue);
#define DRV_TCXO_TIMER_START(enModemId, enModeType, routine, arg, timerValue)   bsp_tcxo_timer_start(enModemId, enModeType, routine, arg, timerValue)





#endif
