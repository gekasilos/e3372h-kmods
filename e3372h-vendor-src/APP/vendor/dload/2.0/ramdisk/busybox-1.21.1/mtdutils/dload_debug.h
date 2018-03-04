/******************************************************************************

                  ��Ȩ���� (C), 2001-2014, ��Ϊ�������޹�˾

 ******************************************************************************
  �� �� ��   : dload_debug.h
  �� �� ��   : ����
  ��    ��   : q00208481
  ��������   : 2013��4��12��
  ����޸�   :
  ��������   :
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2013��4��12��
    ��    ��   : q00208481
    �޸�����   : �����ļ�
    
                                   
******************************************************************************/
#ifndef _HUAWEI_DLOAD_DEBUG_H_
#define _HUAWEI_DLOAD_DEBUG_H_


#ifdef  __cplusplus
#if  __cplusplus
    extern "C"{
#endif
#endif

#define     MBB_DLOAD_LOGREC_TOCONSOLE  (1)
#define     MBB_DLOAD_LOGREC_TOFLASH    (1)
//#define     MBB_DLOAD_LOGREC_TOMMC      (1)
#define     MBB_DLOAD_FLASHLOG_PATH     "/data/huawei_dload/"
#define     MBB_DLOAD_MMCLOG_PATH       "/mnt/sdcard/dload_log/"

#define     DLOAD_LOGREC_LINE_LENGTH    (2048)
#define     DLOAD_LOGREC_FILENAME_A     "dload_logA.txt"
#define     DLOAD_LOGREC_FILENAME_B     "dload_logB.txt"
#define     DLOAD_LOGREC_PATH_LENGTH    (128)
#define     DLOAD_LOGREC_MAX_FILESIZE   (128 * 1024)

enum dload_debug_lev
{
    DEBUG_LEV_CRIL,
    DEBUG_LEV_ERR,
    DEBUG_LEV_INFO
}debug_lev_e;

#define  DLOAD_DEBUG_LEVEL  (DEBUG_LEV_INFO)

/***********************************************************************
  ��������  :    dload_printf
  ��������  :    ���log�ӿ� 
  �������  :    printf ��ʽ
  �������  :    NA
  ��������ֵ:    DLOAD_OK: �����ɹ�
                 DLOAD_ERR: ����ʧ��
************************************************************************/
int dload_printf(int debuglev, const char *format, ...);

#ifdef  __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif /*_HUAWEI_DLOAD_FLASH_H_*/
