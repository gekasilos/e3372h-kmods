/******************************************************************************

                  ��Ȩ���� (C), 2001-2014, ��Ϊ�������޹�˾

*******************************************************************************
  �� �� ��   : dload_nark_core.h
  �� �� ��   : ����
  ��    ��   : y00216577
  ��������   : 2014��03��01��
  ����޸�   :
  ��������   : �û�̬�������ں�ͨ��IOCTL�ӿ�ʵ���ļ����������޸ġ�
  �����б�   :
  �޸���ʷ   :
*********************************���ⵥ�޸ļ�¼*******************************
  ���ⵥ��               �޸���         �޸�ʱ��           �޸�˵��                             
  DTS2014090106291      c00227249      2014-09-02         ͬ��������һ���޸�
  
                         
******************************************************************************/
#include <sys/types.h>
#include <sys/stat.h>
#include <asm/ioctl.h>
#include <linux/ioctl.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include "dload_debug.h"
#include "dload_ioctl.h"

#ifdef __cplusplus
extern "C" {
#endif
/******************************************************************************
  1 ģ��˽�� (�ꡢö�١��ṹ�塢�Զ�����������) ������:
******************************************************************************/
#define DLOAD_PRINT(level, fmt, ...) (dload_printf(level, "[*DLOAD_IOCTL*] %s:%d: "fmt"\n", __func__, __LINE__, ##__VA_ARGS__))
#define DLOAD_IOC_TYPE   'D'
enum
{
    DLOAD_SET_CMD  = _IOW(DLOAD_IOC_TYPE, 0, ioctl_msg_t),  /* �������� */
    DLOAD_GET_CMD  = _IOR(DLOAD_IOC_TYPE, 1, ioctl_msg_t),  /* ��ȡ���� */
};

/******************************************************************************
  2 ģ��˽�� (ȫ�ֱ���) ������:
******************************************************************************/
STATIC const uint8* dload_dev_name = "/dev/dload_nark";
STATIC int32        dload_nark_file = -1;

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
int32 dload_set_data(dload_module_t module_index, int32 msg_index, void* in_buffer, uint32 in_len)
{
    int32        ret = DLOAD_OK;
    ioctl_msg_t  msg_entry;

    msg_entry.module_index  = module_index;
    msg_entry.msg_index     = msg_index;
    if(NULL == in_buffer)
    {
        msg_entry.data_buffer  = NULL;
        msg_entry.data_len     = 0;
    }
    else
    {
        msg_entry.data_buffer  = in_buffer;
        msg_entry.data_len     = in_len;
    }
    
    ret = ioctl(dload_nark_file, DLOAD_SET_CMD, &msg_entry);

    return ret;
}

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
int32 dload_get_data(dload_module_t module_index, int32 msg_index, void* out_buffer, uint32 out_len)
{
    int32  ret = DLOAD_OK;
    ioctl_msg_t msg_entry;

    msg_entry.module_index   = module_index;
    msg_entry.msg_index      = msg_index;
    if(NULL == out_buffer)
    {
        msg_entry.data_buffer = NULL;
        msg_entry.data_len    = 0;
    }
    else
    {
        msg_entry.data_buffer  = out_buffer;
        msg_entry.data_len     = out_len;
    }

    if(0 > dload_nark_file)
    {
        DLOAD_PRINT(DEBUG_LEV_ERR, "file hander = 0x%x error", dload_nark_file);
        return DLOAD_ERR;
    }
    
    ret = ioctl(dload_nark_file, DLOAD_GET_CMD, &msg_entry);
    
    return ret;
}

/*******************************************************************
  ��������  : dload_ioctl_init
  ��������  : DLOAD IOCTLģ��init����
  �������  : 
  �������  : NA
  ��������ֵ: DLOAD_OK    : �ɹ�
              DLOAD_ERR : ʧ��
********************************************************************/
int32 dload_ioctl_init(void)
{
    dload_nark_file = open(dload_dev_name, O_RDWR);
    if(0 > dload_nark_file)
    {
    	DLOAD_PRINT(DEBUG_LEV_ERR, "file hander = 0x%x open failed", dload_nark_file); 
        return DLOAD_ERR;
    }
    
    return DLOAD_OK;
}

/*******************************************************************
  ��������  : dload_ioctl_exit
  ��������  : DLOAD IOCTLģ��destory����
  �������  : 
  �������  : NA
  ��������ֵ: DLOAD_OK    : �ɹ�
              DLOAD_ERR : ʧ��
********************************************************************/
int32 dload_ioctl_exit(void)
{
    int32 ret = DLOAD_OK;
    ret = close(dload_nark_file);
    if(0 > ret)
    {
        DLOAD_PRINT(DEBUG_LEV_ERR, "close return = %d failed", ret); 
        ret = DLOAD_ERR;
    }
    
    return ret;
}

#ifdef __cplusplus
}
#endif
