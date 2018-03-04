/******************************************************************************

                  ��Ȩ���� (C), 2001-2014, ��Ϊ�������޹�˾

*******************************************************************************
  �� �� ��   : dload_nark_core.h
  �� �� ��   : ����
  ��    ��   : y00216577
  ��������   : 2014��03��01��
  ����޸�   :
  ��������   : ������Ϣ�����ļ�
  �����б�   :
  �޸���ʷ   :
*********************************���ⵥ�޸ļ�¼******************************
  ���ⵥ��               �޸���         �޸�ʱ��           �޸�˵��
  DTS2014090106291      c00227249      2014-09-02         ͬ��������һ���޸�


******************************************************************************/
#ifndef _DLOAD_IOCTRL_H
#define _DLOAD_IOCTRL_H
/******************************************************************************
  1 ����ͷ�ļ�����
******************************************************************************/
#include "dload_comm.h"

#ifdef  __cplusplus
extern "C" {
#endif

/******************************************************************************
  2 ģ����� (�ꡢö�١��ṹ�塢�Զ�����������) ������:
******************************************************************************/
/* ����ͨ�Ÿ���ģ��ID���� */
typedef enum _DLOAD_MODULE
{
    DLOAD_MAIN_MODULE,    /* ������ģ�� */
    DLOAD_SHOW_MODULE,    /* ������ʾģ�� */
    DLOAD_USB_MODULE,     /* һ������ģ�� */
    DLOAD_ONLINE_MODULE,  /* ��������ģ�� */
    DLOAD_SCARD_MODULE,   /* SCARD����ģ�� */
    DLOAD_HOTA_MODULE,    /* HOTA����ģ�� */
    DLOAD_FOTA_MODULE,    /* FOTA����ģ�� */
    DLOAD_FLASH_MODULE,   /* FLASH����ģ�� */
    DLOAD_AT_MODULE,      /* ����ATģ�� */
}dload_module_t;

/* ������ϢID�����б� */
enum _MAIN_MODULE_MSG_ID
{
    MTD_FLUSH_MSG_ID ,           /* ˢ��MTD�豸 */
    GET_BAT_LEVEL_MSG_ID,           /* ��ȡ��ص������� */
    GET_PLATFORMINFO_MSG_ID,    /* ��ȡƽ̨��Ϣ */
    VISIT_DLOAD_SMEM_MSG_ID,    /* �����ڴ������ϢID*/
    GET_MTD_FS_TYPE_MSG_ID,     /* ��ȡMTD�ļ�ϵͳ���� */
    GET_DLOAD_FEATURE_MSG_ID,   /* ��ȡ����������Ϣ */
    GET_PATH_INFO_SIZE_MSG_ID,  /* ��ȡ��ʾģ��·��ӳ����С */
    GET_PATH_INFO_MSG_ID,        /* ��ȡ��ʾģ��·��ӳ��� */
    GET_SHOW_INFO_SIZE_MSG_ID,  /* ��ȡ��ʾģ�������б��С */
    GET_SHOW_INFO_MSG_ID,        /* ��ȡ��ʾģ�������б� */
};


typedef struct _IOCRL_MSG
{
    dload_module_t       module_index;  /* ������ģ��ID */
    uint32               msg_index;     /* ��ģ����ϢID */
    uint32               data_len;      /* ���ݻ��泤�� */
    void*                data_buffer;   /* ���ݻ���ָ�� */
} ioctl_msg_t;

/*******************************************************************
  ��������  : dload_set_data
  ��������  : �û������������ں˲��������ݽӿ�
  �������  : module_index: ģ������
              msg_index   : ��Ϣ����
              in_buffer   : ���ݻ����ַ
              in_len      : ���ݻ��泤��
  �������  : NA
  ��������ֵ: TRUE  : �ɹ�
              FALSE : ʧ��
********************************************************************/
int32 dload_set_data(dload_module_t module_index, int32 msg_index, void* in_buffer, uint32 in_len);

/*******************************************************************
  ��������  : dload_get_data
  ��������  : �û�����������ں˲��ȡ���ݽӿ�
  �������  : module_index: ģ��ID
              msg_index   : ��ϢID
              out_buffer  : ���ݻ����ַ
              out_len     : ���ݻ��泤��
  �������  : out_buffer  : ���ݻ����ַ
  ��������ֵ: TRUE  : �ɹ�
              FALSE : ʧ��
********************************************************************/
int32 dload_get_data(dload_module_t module_index, int32 msg_index, void* out_buffer, uint32 out_len);

/*******************************************************************
  ��������  : dload_ioctl_init
  ��������  : DLOAD IOCTLģ��init����
  �������  :
  �������  : NA
  ��������ֵ: DLOAD_OK    : �ɹ�
              DLOAD_ERR : ʧ��
********************************************************************/
int32 dload_ioctl_init(void);

/*******************************************************************
  ��������  : dload_ioctl_exit
  ��������  : DLOAD IOCTLģ��destory����
  �������  :
  �������  : NA
  ��������ֵ: DLOAD_OK    : �ɹ�
              DLOAD_ERR : ʧ��
********************************************************************/
int32 dload_ioctl_exit(void);

#ifdef __cplusplus
}
#endif
#endif /* _DLOAD_IOCTRL_H */
