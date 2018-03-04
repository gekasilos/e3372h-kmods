/*************************************************************************
*   ��Ȩ����(C) 1987-2011, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  DrvInterface.h
*
*   ��    �� :  yangzhi
*
*   ��    �� :  ���ļ�����Ϊ"DrvInterface.h", ����V7R1�����Э��ջ֮���API�ӿ�ͳ��
*
*   �޸ļ�¼ :  2011��1��18��  v1.00  yangzhi����
*************************************************************************/

#ifndef __DRV_EFUSE_H__
#define __DRV_EFUSE_H__
#ifdef __cplusplus
extern "C"
{
#endif

#include "drv_comm.h"


#define     BUF_ERROR          (-55)
#define     LEN_ERROR          (-56)
#define     READ_EFUSE_ERROR   (-57)



/*************************************************
 �� �� ��   : efuseWriteHUK
 ��������   : HUKдefuse�ӿ�
 �������   : pBuf:��д����;len:��д����(�ֽ�)
 �������   :
 �� �� ֵ      : OK/ERROR
 ���ú���   :
 ��������   :

 �޸���ʷ   :
    ��    ��       : 2012��3��27��
   �޸����� : �����ɺ���

*************************************************/
extern int efuseWriteHUK(char *pBuf,unsigned int len);

#define EFUSE_WRITE_HUK(pBuf,len) efuseWriteHUK(pBuf,len)

/*************************************************
 �� �� ��   : CheckHukIsValid
 ��������   : �ж�HUK�Ƿ���Ч����Ϊȫ0����Ч����ȫ0����Ч
 �������   : ��
 �������   :
 �� �� ֵ   : BSP_FALSE:HUK��Ч��BSP_TRUE:HUK��Ч
 ���ú���   :
 ��������   :

 �޸���ʷ   :
   ��    �� : 2012��3��27��
   �޸����� : �����ɺ���

*************************************************/

extern int CheckHukIsValid(void);
#define DRV_CHECK_HUK_IS_VALID() CheckHukIsValid()

/*************************************************
 �� �� ��   : DRV_GET_DIEID
 ��������   : DRV_GET_DIEID
 �������   : buf: data buffer
              len: length of the group
 �������   :
 �� �� ֵ   : OK                (0)
              BUF_ERROR         (-55)
              LEN_ERROR         (-56)
              READ_EFUSE_ERROR  (-57)
 ���ú���   :
 ��������   :

 �޸���ʷ   :
   ��    �� : 2013��8��12��
   �޸����� : �����ɺ���

*************************************************/
extern int DRV_GET_DIEID(unsigned char* buf,int length);

/*************************************************
 �� �� ��   : DRV_GET_CHIPID
 ��������   : DRV_GET_CHIPID
 �������   : buf: data buffer
              len: length of the group
 �������   :
 �� �� ֵ   : OK                (0)
              BUF_ERROR         (-55)
              LEN_ERROR         (-56)
              READ_EFUSE_ERROR  (-57)
 ���ú���   :
 ��������   :

 �޸���ʷ   :
   ��    �� : 2013��8��12��
   �޸����� : �����ɺ���

*************************************************/
extern int DRV_GET_CHIPID(unsigned char* buf,int length);

#ifdef __cplusplus
}
#endif

#endif

