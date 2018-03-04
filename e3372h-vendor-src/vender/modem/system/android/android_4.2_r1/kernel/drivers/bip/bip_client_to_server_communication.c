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
#include "product_config.h"

#if (FEATURE_ON == MBB_FEATURE_BIP)

#include "bip_client_to_server_communication.h"

#include "drv_bip.h"

#define BIP_DEV_PRINTF(fmt...)
#ifndef BIP_DEV_PRINTF
#define BIP_DEV_PRINTF(fmt...)      printk(fmt)
#endif
/****************************************************************************
 �ⲿ����ԭ��˵��     
 
*****************************************************************************/
extern int device_event_report(void *data, int len);

/****************************************************************************
 �ڲ�����ԭ��˵��
 
*****************************************************************************/

/****************************************************************************
 ȫ�ֱ�������                         
 
*****************************************************************************/
BIP_Command_Event_STRU  g_BipDeviceStru;
BIP_Command_Event_STRU  g_BipClientEvent;

/*lint -e19 -e24 -e43 -e63 -e64 -e65 -e133  */
/*����BIP�ַ��豸*/
static const struct file_operations bip_device_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = BipDeviceIoctl,
    .open   = BipDeviceOpen,
    .release = BipDeviceRelease,
};

/*BIP�ַ��豸��*/
static struct miscdevice bip_miscdev = {
    .minor  = MISC_DYNAMIC_MINOR,
    .name   = "bip_dev",
    .fops = &bip_device_fops
};

/*lint +e19 +e24 +e43 +e63 +e64 +e65 +e133  */

/*****************************************************************************
�� �� ��  : BipDeviceOpen
��������  : ��BIP�ַ��豸
�������  : struct inode *inode, struct file *file
�������  : ��
���ú���  : ��
��������  : �ⲿ�ӿ�
History     :
1.��    ��  : 
  �޸�����  : Create
*****************************************************************************/
static int BipDeviceOpen(struct inode *inode, struct file *file)
{
    if (NULL != inode && NULL != file)
    {
       ;
    }
    return 0;
}

/*****************************************************************************
�� �� ��  : BipDeviceRelease
��������  : �ر�BIP�ַ��豸
�������  : struct inode *inode, struct file *file
�������  : ��
���ú���  : ��
��������  : �ⲿ�ӿ�
History     :
1.��    ��  : 
  �޸�����  : Create
*****************************************************************************/
static int BipDeviceRelease(struct inode *inode, struct file *file)
{
    if (NULL != inode && NULL != file)
    {
        ;
    }

    return 0;
}

SI_UINT16 SI_STK_DataSendBipEvent(SI_UINT16 DataLen, SI_UINT8 *pData)
{
    SI_UINT32 ulMsgId = 0;

    return 0;
}

/*****************************************************************************
�� �� ��  :BipDeviceShowInfo
��������  : ��ӡ M��BIPģ�飬���͹���������
�������  : BIP_Command_Event_STRU *stru
�������  : ��
�� �� ֵ  :��
���ú���  : ��
��������  : �ⲿ�ӿ�
History     :
1.��    ��  : 
  �޸�����  : Create
*****************************************************************************/
void BipDeviceShowInfo (BIP_Command_Event_STRU *Data)
{
    BSP_U32 message_tpye = 0;

    message_tpye = Data->CmdType;
    BIP_DEV_PRINTF("BIP TEST: ChannelNum:%d. message_tpye=%d\n", Data->ChannelNum, message_tpye);

    switch(message_tpye)
    {
        /*lint -e30 -e515*/
        case BIP_OPEN_CHANNEL_REQ:
        { 
            BIP_DEV_PRINTF("BIP TEST:BIP_OPEN_CHANNEL_REQ,BearerDesp=%d\n", Data->CmdEventStru.open_channel_req.BearerDesp);
            BIP_DEV_PRINTF("BIP TEST:BIP_OPEN_CHANNEL_REQ,IPAddrType=%d,ApnLen=%d,BufferSize=%d,PortNum=%d\n",
                Data->CmdEventStru.open_channel_req.IPAddrType, 
                Data->CmdEventStru.open_channel_req.ApnLen,
                Data->CmdEventStru.open_channel_req.BufferSize,
                Data->CmdEventStru.open_channel_req.PortNum);

            break;
        }

        case BIP_CLOSE_CHANNEL_REQ:
        {
            BIP_DEV_PRINTF("BIP TEST:BIP_CLOSE_CHANNEL_REQ\n");
            break;
        }

        case BIP_RECEIVE_DATA_REQ:
        {
            BIP_DEV_PRINTF("BIP TEST:BIP_RECEIVE_DATA_REQ ReqDataLen:%d\n",
                            Data->CmdEventStru.receive_data_req.ReqDataLen);
            break;
        }

        case BIP_SEND_DATA_REQ:
        {   
            BIP_DEV_PRINTF("BIP TEST:BIP_SEND_DATA_REQ,SendDataMode=%d,ChannelDataLen=%d\n",
                            Data->CmdEventStru.send_data_req.SendDataMode,
                            Data->CmdEventStru.send_data_req.ChannleDataLen);
            break;
        }

        case BIP_GET_CHANNEL_STATUS_REQ:
        {        
            BIP_DEV_PRINTF("BIP TEST:BIP_GET_CHANNEL_STATUS_REQ\n");
            break;
        }

        /*lint +e30 +e515*/

        default:
            break;
    }
}

/*****************************************************************************
�� �� ��  : BipDeviceIoctl
��������  : BIP�ַ��豸��д����
�������  : struct inode *inode, struct file *file
�������  : ��
���ú���  : ��
��������  : �ⲿ�ӿ�
History     :
1.��    ��  : 
  �޸�����  : Create
*****************************************************************************/
static long BipDeviceIoctl(struct file *file, unsigned int  cmd, unsigned long arg)
{        
    long ret = BSP_OK;
    
    if (NULL == file)
    {  
        BIP_DEV_PRINTF(" balong_bip_ioctl receive fail,file is null \n");
        return BSP_ERROR;
    }

    switch (cmd)
    {
        /*��BIP�ַ��豸��д������*/
        case OTA_TO_UICC:
            ret = (long)copy_from_user((void*)&g_BipClientEvent, (void*)arg, sizeof(g_BipClientEvent));   /*lint !e420 */
            if ( 0 == ret )
            {
                ret = (long)BipDeviceReceiveDataFromBipClient((BIP_Command_Event_STRU *)&g_BipClientEvent);
            }
            break;
        
        /*��ȡBIP�ַ��豸������*/
        case UICC_TO_OTA:
            ret = (long)copy_to_user((void*)arg, (void*)&g_BipDeviceStru, sizeof(g_BipDeviceStru)); /*lint !e420 */
            break;   
        
        default:
            break;
    }

    if (BSP_OK != ret)
    {
        BIP_DEV_PRINTF("balong_bip_ioctl event=%d error:%ld\n", cmd, ret);
    }

    return ret;
}

/*****************************************************************************
�� �� ��  :BipDeviceInit
��������  : ע��BIP�ַ��豸
�������  :��
�������  : ��
���ú���  : ��
��������  : �ⲿ�ӿ�
History     :
1.��    ��  : 
  �޸�����  : Create
*****************************************************************************/
int BipDeviceInit(BSP_VOID)
{
    int ret  = 0;

    ret = misc_register(&bip_miscdev);

    if (BSP_OK != ret)
    {
        BIP_DEV_PRINTF("BIP init, misc_register failed.s32Ret=%d \n",ret);  
    }

    memset(&g_BipDeviceStru, 0x00, sizeof(g_BipDeviceStru));
    memset(&g_BipClientEvent, 0x00, sizeof(g_BipClientEvent));
    
    return ret;
}

/*****************************************************************************
�� �� ��  :BipDeviceReceiveDataFromBipClient
��������  : BIP�ַ��豸 ��BIP Client���յ�BIP��Ϣ�����д���
�������  : BIP_Command_Event_STRU *stru
�������  : ��
�� �� ֵ  : ��
��������  : �ⲿ�ӿ�
History     :
1.��    ��  : 
  �޸�����  : Create
 *****************************************************************************/
BSP_S32 BipDeviceReceiveDataFromBipClient( BIP_Command_Event_STRU *pDst)
{
    BSP_S32 ret = BSP_OK;
    BSP_U32 message_tpye = 0;

    if (NULL == pDst)
    {
        return BSP_ERROR;
    }
    BIP_DEV_PRINTF("BipDeviceReceiveDataFromBipClient()\n MessageTpye=%d", pDst->CmdType);

    message_tpye = pDst->CmdType;

    switch(message_tpye)
    {  
        case BIP_OPEN_CHANNEL_RSP:
        { 
            if (0 != pDst->ResultValue)
            {
                (void)BipDeviceNotifyDataToBipClient(pDst, UICC_COMPLETE);
            }
        }
        break;
        case BIP_CLOSE_CHANNEL_RSP:
        {
            if (0 == pDst->ResultValue)
            {
                (void)BipDeviceNotifyDataToBipClient(pDst, UICC_COMPLETE);
            }
        }
        break;
        default:
            break;
    }
    
    ret = BipDeviceSendDataToModem(pDst);
    
    return ret; 
}

/*****************************************************************************
�� �� ��  :BipDeviceSendDataToModem
��������  : BIP�ַ��豸�յ�Bip Cliennt����Ϣ��ͨ���˼�ͨ�ţ���Modem BIPģ�鷢������
�������  : BIP_Command_Event_STRU *stru
�������  : ��
�� �� ֵ  : BSP_OK:    �����ɹ�
             BSP_ERROR: ����ʧ��
���ú���  : ��
��������  : �ⲿ�ӿ�
History     :
1.��    ��  : 
  �޸�����  : Create
 *****************************************************************************/
BSP_S32 BipDeviceSendDataToModem( BIP_Command_Event_STRU *stru)
{
    BSP_S32 ret = BSP_OK;
    BSP_U16 len = (BSP_U16)sizeof(BIP_Command_Event_STRU);
    
    if (NULL == stru)
    {
        return BSP_ERROR;
    }

    ret = SI_STK_DataSendBipEvent( len, (SI_UINT8*)stru);
    
    if (BSP_OK != ret)
    {
        BIP_DEV_PRINTF("------BipDeviceSendDataToModem fail! ret=%d\n", ret);
        return BSP_ERROR;
    }
    
    return ret;
}

/*****************************************************************************
�� �� ��  : BipDeviceReceiveDataFromModem
��������  : BIP�ַ��豸��������M�˵� BIP��Ϣ,��֪ͨBip Client��ȡ����
��������  : ��ӡM��BIPģ�飬�������������
�������  : BSP_VOID *pMsgBody�����յ���Ϣʵ��
             BSP_U32 u32Len,��Ϣ����
�������  : ��
�� �� ֵ  : BSP_OK:    �����ɹ�
             BSP_ERROR: ����ʧ��
���ú���  : ��
��������  : �ⲿ�ӿ�
History     :
1.��    ��  : 
  �޸�����  : Create
*****************************************************************************/
BSP_S32 BipDeviceReceiveDataFromModem(BSP_VOID *pMsgBody, BSP_U32 u32Len)
{
    BIP_Command_Event_STRU *bip_event_data = &g_BipDeviceStru;
    BSP_S32  ret = 0;

    if ((NULL == pMsgBody) || (u32Len > sizeof(BIP_Command_Event_STRU)))
    {
        BIP_DEV_PRINTF("------BSP_BIP_CLIENT_ReceiveArg is null \n-----");
        return BSP_ERROR;
    }

    /*�������ݵ�g_BipDeviceStru���ȴ�BIP Client Fetch����*/
    if ( bip_event_data != (BIP_Command_Event_STRU *)pMsgBody )
    {
        memcpy((BSP_VOID*)bip_event_data, pMsgBody, u32Len);
    }
    
    /*��ӡM��BIP��Ϣ����*/
    BipDeviceShowInfo(bip_event_data);

    /*֪ͨBIP Client������*/
    ret = BipDeviceNotifyDataToBipClient(bip_event_data, UICC_INFORM);

    return ret;
}

void bip_event_receiver(BSP_VOID *pMsgBody, BSP_S32 u32Len)
{
    BipDeviceReceiveDataFromModem(pMsgBody, u32Len);
}

EXPORT_SYMBOL(bip_event_receiver); /*lint !e19 !e323 */

/*****************************************************************************
�� �� ��  :BipDeviceNotifyDataToBipClient
��������  : BIP�ַ��豸 ��Ӧ��Bip Client������Ϣ��֪ͨ�������ݵ�������ȡ����
�������  : BIP_Command_Event_STRU *stru
�������  : ��
�� �� ֵ  : ��
���ú���  : ��
��������  : �ⲿ�ӿ�
History     :
1.��    ��  : 
  �޸�����  : Create
 *****************************************************************************/
BSP_S32 BipDeviceNotifyDataToBipClient( BIP_Command_Event_STRU *pDst, BIP_EVENT event_code)
{
    BSP_U32 size = 0;
    BSP_S32  ret = 0;
    DEVICE_EVENT event; /*lint !e10 !e522 */

    if (NULL == pDst)
    {
        BIP_DEV_PRINTF("------BipDeviceNotifyDataToBipClient pDst is NULL! \n");
        return BSP_ERROR;
    }
    
    /*��װNetLink�ṹ��ָ��Event��DeviceId,EventCode,Length*/
    /*lint -e10 -e63*/
    event.device_id  = DEVICE_ID_BIP; 
    event.event_code = (int)event_code;
    event.len = 0;
    size  = sizeof(DEVICE_EVENT);
    /*lint +e10 +e63*/

    /*֪ͨBIP Client�����ݣ��ȴ�BIP Client Fetch����*/
    ret = (BSP_S32)device_event_report((void*)&event, (int)size);

    if (BSP_OK != ret)
    {
       BIP_DEV_PRINTF("BipDeviceNotifyDataToBipClient  Fail-----\n");
    }

    return ret;
}

/*lint -e10 -e528 */
/*ģ���ʼ��*/
late_initcall(BipDeviceInit);
/*lint +e10 +e528 */

#if (FEATURE_ON == MBB_FEATURE_BIP_TEST)
/*****************************************************************************
�� �� ��  :BipClientTestOpenChannel
��������  : ģ��Bip Client �ظ�Open Channel��Ӧ
�������  :  ��
�������  : ��
�� �� ֵ  :��
���ú���  : ��
��������  : �ⲿ�ӿ�
History     :
1.��    ��  : 
  �޸�����  : Create
*****************************************************************************/
void BipClientTestOpenChannel(BSP_U8 ResultValue)
{
    BIP_Command_Event_STRU  bip_stru;
    memset((void*)&bip_stru, 0, sizeof(BIP_Command_Event_STRU));
    
    bip_stru.CmdType = BIP_OPEN_CHANNEL_RSP;
    bip_stru.ResultValue = ResultValue;
    bip_stru.ChannelNum = 0 ;

    bip_stru.CmdEventStru.open_channel_rsp.BufferSize = BIP_MAX_BUFFER_LENGTH;
    
    bip_stru.CmdEventStru.open_channel_rsp.IPAddrType = 0x21;  /* ipv4 */
    bip_stru.CmdEventStru.open_channel_rsp.IPAddrLen = 4;       /* ipv4 */
    bip_stru.CmdEventStru.open_channel_rsp.IPAddr[0] = 81;      /* ipv4 */
    bip_stru.CmdEventStru.open_channel_rsp.IPAddr[1] = 23;      /* ipv4 */
    bip_stru.CmdEventStru.open_channel_rsp.IPAddr[2] = 12;      /* ipv4 */
    bip_stru.CmdEventStru.open_channel_rsp.IPAddr[3] = 21;      /* ipv4 */
    
    bip_stru.CmdEventStru.open_channel_rsp.ChannelStatus = CHANNEL_OPEN;
    
    BipDeviceReceiveDataFromBipClient(&bip_stru);
}

/*****************************************************************************
�� �� ��  :BipClientTestCloseChannel
��������  : ģ��Bip Client �ظ�Close Channel��Ӧ
�������  :  ��
�������  : ��
�� �� ֵ  :��
���ú���  : ��
��������  : �ⲿ�ӿ�
History     :
1.��    ��  : 
  �޸�����  : Create
 *****************************************************************************/
void BipClientTestCloseChannel(void)
{
    BIP_Command_Event_STRU bip_stru;
    memset((void*)&bip_stru, 0, sizeof(BIP_Command_Event_STRU));
    
    BIP_DEV_PRINTF( " bip_client_test_close_channel \n");
    bip_stru.CmdType = BIP_CLOSE_CHANNEL_RSP;
    bip_stru.ResultValue = 0;
    
    BipDeviceReceiveDataFromBipClient(&bip_stru);
}

/*****************************************************************************
�� �� ��  :BipClientTestSendData
��������  : ģ��Bip Client �ظ�Send Datal��Ӧ
�������  :  ��
�������  : ��
�� �� ֵ  :��
���ú���  : ��
��������  : �ⲿ�ӿ�
History     :
1.��    ��  : 
  �޸�����  : Create
 *****************************************************************************/
void BipClientTestSendData(BSP_U8 dataLen)
{    
    BIP_Command_Event_STRU bip_stru;
    memset((void*)&bip_stru, 0, sizeof(BIP_Command_Event_STRU));
    
    bip_stru.CmdType = BIP_SEND_DATA_RSP;
    bip_stru.ResultValue = 0;
    
    bip_stru.CmdEventStru.send_data_rsp.IdleChannleDataLen = dataLen;
    
    BipDeviceReceiveDataFromBipClient(&bip_stru);
}

/*****************************************************************************
�� �� ��  :BipClientTestSendData
��������  : ģ��Bip Client �ظ�Receive Datal��Ӧ
�������  :  ��
�������  : ��
�� �� ֵ  :��
���ú���  : ��
��������  : �ⲿ�ӿ�
History     :
1.��    ��  : 
  �޸�����  : Create
 *****************************************************************************/
void BipClientTestReceiveData( BSP_U8 dataLen, BSP_U8 LeftDataLen  )
{
    BIP_Command_Event_STRU bip_stru;
    BSP_U8 i;

    memset((void*)&bip_stru, 0, sizeof(BIP_Command_Event_STRU));
        
    bip_stru.CmdType = BIP_RECEIVE_DATA_RSP;
    bip_stru.ResultValue = 0;
    
    bip_stru.CmdEventStru.receive_data_rsp.DataLen = dataLen;
    bip_stru.CmdEventStru.receive_data_rsp.LeftDataLen = LeftDataLen;

    for ( i = 0; i < dataLen; i++ )
    {
        bip_stru.CmdEventStru.receive_data_rsp.ChannelData[i] = i;
    }
    
    BipDeviceReceiveDataFromBipClient(&bip_stru);
}

/*****************************************************************************
�� �� ��  :BipClientTestGetChannelStatus
��������  : ģ��Bip Client �ظ�Get Channel Statusl��Ӧ
�������  :  ��
�������  : ��
�� �� ֵ  :��
���ú���  : ��
��������  : �ⲿ�ӿ�
History     :
1.��    ��  : 
  �޸�����  : Create
 *****************************************************************************/
void BipClientTestGetChannelStatus(BSP_U8 ChannelStatus)
{
    BIP_Command_Event_STRU bip_stru;
    memset((void*)&bip_stru,0,sizeof(BIP_Command_Event_STRU));
        
    bip_stru.CmdType = BIP_GET_CHANNEL_STATUS_RSP;
    bip_stru.ResultValue = 0;
    
    bip_stru.ChannelNum = 0 ;
    bip_stru.CmdEventStru.channel_status_rsp.ChannelStatus = ChannelStatus;
    
    BipDeviceReceiveDataFromBipClient(&bip_stru);
}

/*****************************************************************************
�� �� ��  :BipClientTestChannelStatusNotify
��������  : ģ��Bip Client �·�Channel Statusl Event
�������  :  ��
�������  : ��
�� �� ֵ  :��
���ú���  : ��
��������  : �ⲿ�ӿ�
History     :
1.��    ��  : 
  �޸�����  : Create
 *****************************************************************************/
void BipClientTestChannelStatusNotify(BSP_U8 ChannelStatus)
{    
    BIP_Command_Event_STRU bip_stru;
    memset((void*)&bip_stru, 0, sizeof(BIP_Command_Event_STRU));
        
    bip_stru.CmdType = BIP_CHANNEL_STATUS_NOTIFY;
    bip_stru.ResultValue = 0;
    bip_stru.ChannelNum = 0;
    
    bip_stru.CmdEventStru.channel_status_event.ChannelStatus = ChannelStatus;
    
    BipDeviceReceiveDataFromBipClient(&bip_stru);
}

/*****************************************************************************
�� �� ��  :BipClientTestFail
��������  : ģ��Bip Client �·������Ӧfail�¼�
�������  :  ��
�������  : ��
�� �� ֵ  :��
���ú���  : ��
��������  : �ⲿ�ӿ�
History     :
1.��    ��  : 
  �޸�����  : Create
 *****************************************************************************/
void BipClientTestFail(BSP_U8 CmdType, BSP_U8 ResultVal)
{    
    BIP_Command_Event_STRU bip_stru;
    memset((void*)&bip_stru, 0, sizeof(BIP_Command_Event_STRU));
        
    bip_stru.CmdType = CmdType;
    bip_stru.ResultValue = ResultVal;
        
    BipDeviceReceiveDataFromBipClient(&bip_stru);
}

#endif /*#if (FEATURE_ON == MBB_FEATURE_BIP_TEST_KERNEL) */

#endif/*(FEATURE_ON == MBB_FEATURE_BIP)*/

