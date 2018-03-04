 /*
 * Copyright (C) Huawei Technologies Co., Ltd. 2012-2015. All rights reserved.
 * foss@huawei.com
 *
 * If distributed as part of the Linux kernel, the following license terms
 * apply:
 *
 * * This program is free software; you can redistribute it and/or modify
 * * it under the terms of the GNU General Public License version 2 and 
 * * only version 2 as published by the Free Software Foundation.
 * *
 * * This program is distributed in the hope that it will be useful,
 * * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * * GNU General Public License for more details.
 * *
 * * You should have received a copy of the GNU General Public License
 * * along with this program; if not, write to the Free Software
 * * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA
 *
 * Otherwise, the following license terms apply:
 *
 * * Redistribution and use in source and binary forms, with or without
 * * modification, are permitted provided that the following conditions
 * * are met:
 * * 1) Redistributions of source code must retain the above copyright
 * *    notice, this list of conditions and the following disclaimer.
 * * 2) Redistributions in binary form must reproduce the above copyright
 * *    notice, this list of conditions and the following disclaimer in the
 * *    documentation and/or other materials provided with the distribution.
 * * 3) Neither the name of Huawei nor the names of its contributors may 
 * *    be used to endorse or promote products derived from this software 
 * *    without specific prior written permission.
 * 
 * * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */
/*lint -save -e19 -e123 -e537 -e713*/
/******************************************************************************************************
    ���ⵥ��                       ʱ��                   �޸���                        ˵��
DTS2013111808849               2013-11-18          ankang 00252937                hilink PNP
DTS2013111907319               2013-11-19          zhangkuo 00248699   coverity,fortify�澯����  
DTS2013111808826               2013-11-22          liulimin 00193392              ���߳��USB�����Ͽ�
DTS2013111202833               2013-11-22          ligang 00212897              PMU���жϴ���
DTS2013111809086               2013-11-25          liulimin 00193392     ���USB����������ԭ��Ϊ�л��˿ڵ�workû��ִ���꣬
                                                                         �����յ��γ��жϣ�����gadget�������ָ��
DTS2013111809248               2013-12-04           huangfuli 00122846     ƽ̨coverity,fortify �������㡣																		 
DTS2013122404067               2013-12-26           ankang 00252937      Setport ȥ��RNDIS��
DTS2014012201641               2014-01-21           ankang 00252937      ʵ��at^getportmode��at^dialmode����
DTS2014021906342               2014-04-03           hepanzhou00230714    �����Ż�
DTS2014081408221               2014-08-14           hepanzhou00230714     fortify��������
DTS2014102905845               2014-10-30           h00122846             USB��һ����������
******************************************************************************************************/
#include "drv_usb.h"
#include "bsp_usb.h"
#include "usb_vendor.h"
#if (FEATURE_ON == MBB_USB)
#include "mbb_usb_adp.h"
#if USB_IS_SUPPORT_NV
#include "drv_nvim.h"
#include "bsp_nvim.h"
#include "drv_nv_id.h"
#endif
#endif
#include <linux/kernel.h>
#include <linux/printk.h>
#include <linux/module.h>

static unsigned g_usb_enum_done_cur = 0;
static unsigned g_usb_enum_dis_cur = 0;
#if (FEATURE_ON == MBB_USB)
#define MAX_EPIN_NUM            15
#define MAX_EPOUT_NUM           15
#endif

#if (FEATURE_ON == MBB_COMMON)
#define ERROR (-1)
#endif

static struct notifier_block gs_adp_usb_nb;
static struct notifier_block *gs_adp_usb_nb_ptr = NULL;
static int g_usb_enum_done_notify_complete = 0;
static int g_usb_disable_notify_complete = 0;
static USB_CTX_S g_usb_ctx = {{0},{0},{0}};

#if (FEATURE_ON == MBB_USB)

const unsigned char gDevProfileSupported[] = 
{
    USB_IF_PROTOCOL_3G_DIAG, 
#if (FEATURE_OFF == MBB_USB_E5)
    USB_IF_PROTOCOL_MODEM,
    USB_IF_PROTOCOL_3G_MODEM,
#endif
    USB_IF_PROTOCOL_CTRL, 
    USB_IF_PROTOCOL_PCUI,
    USB_IF_PROTOCOL_DIAG,
    USB_IF_PROTOCOL_3G_GPS,
    USB_IF_PROTOCOL_GPS,
    USB_IF_PROTOCOL_BLUETOOTH,
    USB_IF_PROTOCOL_NCM,
    USB_IF_PROTOCOL_CDROM,
#if (FEATURE_ON == MBB_USB_SD)    
    USB_IF_PROTOCOL_SDRAM,
#endif
};
#endif

/*****************************************************************************
 �� �� ��  : BSP_USB_SetPid
 ��������  :
 �������  : ��
 �������  : ��
 �� �� ֵ  : ��
*****************************************************************************/
int BSP_USB_SetPid(unsigned char u2diagValue)
{
    return OK;
}

/*****************************************************************************
��������   BSP_USB_PortTypeQuery
��������:  ��ѯ��ǰ���豸ö�ٵĶ˿���ֵ̬
��������� stDynamicPidType  �˿���̬
��������� stDynamicPidType  �˿���̬
����ֵ��   0:    ��ѯ�ɹ�
           ��������ѯʧ��
*****************************************************************************/
unsigned int BSP_USB_PortTypeQuery(DRV_DYNAMIC_PID_TYPE_STRU *pstDynamicPidType)
{
#if (FEATURE_ON == MBB_USB)
/* BEGIN PN: DTS2013091401665, add by h00122846, 2013/09/14 */
    unsigned  int ret = 0;
    if (NULL != pstDynamicPidType)
    {
        memset(pstDynamicPidType, 0, sizeof(DRV_DYNAMIC_PID_TYPE_STRU));
        ret = NVM_Read(NV_ID_DRV_USB_DYNAMIC_PID_TYPE_PARAM, (void*)pstDynamicPidType,sizeof(DRV_DYNAMIC_PID_TYPE_STRU));
    }
   
    return ret;
/* END PN: DTS2013091401665, add by h00122846, 2013/09/14 */
#else
    return OK;
#endif	
}

/*****************************************************************************
��������   BSP_USB_PortTypeValidCheck
��������:  �ṩ���ϲ��ѯ�豸�˿���̬���úϷ��Խӿ�
           1���˿�Ϊ��֧�����ͣ�2������PCUI�ڣ�3�����ظ��˿ڣ�4���˵���������16��
           5����һ���豸��ΪMASS��
��������� pucPortType  �˿���̬����
           ulPortNum    �˿���̬����
����ֵ��   0:    �˿���̬�Ϸ�
           �������˿���̬�Ƿ�
*****************************************************************************/
unsigned int BSP_USB_PortTypeValidCheck(unsigned char *pucPortType, unsigned long ulPortNum)
{
#if (FEATURE_ON == MBB_USB)
/* BEGIN PN: DTS2013101203547,Added by h00122846, 2013/10/14*/
    uint32_t cnt = 0, type = 0, dev_loc = 0;
    uint32_t ep_in_cnt = 0, ep_out_cnt = 0;
    bool pcui_present =  false;
    uint8_t port_exist[256] = {0};

    printk(KERN_ERR "BSP_USB_PortTypeValidCheck");

    if ((NULL == pucPortType) || (0 == ulPortNum) || (DYNAMIC_PID_MAX_PORT_NUM < ulPortNum))
    {
        printk(KERN_ERR "%s: invalid param buf[%p] num[%ld]\n", __FUNCTION__, pucPortType, ulPortNum);
        return 1;
    }

    type = pucPortType[0];

    /* ����һ���豸����ΪMASS�豸����豸 */
    if ((USB_IF_PROTOCOL_CDROM == type) || (USB_IF_PROTOCOL_SDRAM == type) ||
        (USB_IF_PROTOCOL_VOID == type))
    {
        printk(KERN_ERR "%s: First device is Mass Storage device!\r\n", __FUNCTION__);
        return 1;
    }

#ifdef CONFIG_BALONG_RNDIS        // Hilink��̬��֧��Setport
    return 1;
#else
    /* ����л�����豸��̬ */
    for (cnt = 0; cnt < ulPortNum; cnt++)
    {
        type = pucPortType[cnt];

        if (0 < port_exist[type])
        {
            printk(KERN_ERR "%s: Port type repeat\n", __FUNCTION__);
            return 1;
        }

#if (FEATURE_ON == MBB_USB_E5)
        /* E5��Ʒ���ܴ�Modem�豸 */
        if ((USB_IF_PROTOCOL_3G_MODEM == type) || (USB_IF_PROTOCOL_MODEM == type))
        {
            printk(KERN_ERR "%s: There is MODEM with E5!\r\n", __FUNCTION__);
            return 1;
        }
#endif
        /* ���PCUI�˿��Ƿ���� */
        if ((USB_IF_PROTOCOL_3G_PCUI == type) || (USB_IF_PROTOCOL_PCUI == type))
        {
            pcui_present = true;
        }

        /* ����豸��̬�Ƿ���Ч֧�� */
        for (dev_loc = 0; dev_loc < sizeof(gDevProfileSupported); dev_loc++)
        {
            if (type == gDevProfileSupported[dev_loc])
            {
                break;
            }
        }

        if (dev_loc == sizeof(gDevProfileSupported))
        {
            printk(KERN_ERR "%s: Port type isn't supported by current device!\r\n", __FUNCTION__);
            printk(KERN_ERR "%s: dev_loc = [%d], type = [%d]\r\n", __FUNCTION__, dev_loc, type);
            return 1;
        }

        /* ͳ���ض��豸��̬��Ҫ��USB�˵���Ŀ */
        if ((USB_IF_PROTOCOL_NCM  == type) || (USB_IF_PROTOCOL_MODEM   == type) ||
            (USB_IF_PROTOCOL_NDIS == type) || (USB_IF_PROTOCOL_RNDIS   == type) ||
            (USB_IF_PROTOCOL_PCSC == type) || (USB_IF_PROTOCOL_3G_NDIS == type) ||
            (USB_IF_PROTOCOL_3G_MODEM == type))
        {
            ep_in_cnt  += 2;    /* Bulk IN + Interrupt IN */
            ep_out_cnt += 1;    /* Bulk OUT */
        }
        else
        {
            ep_in_cnt  += 1;    /* Bulk IN */
            ep_out_cnt += 1;    /* Bulk OUT */
        }

        port_exist[type]++;
    }

    /* ��PCUI�˿ڣ�����Ϊ�Ƿ� */
    if (false == pcui_present)
    {
        printk(KERN_ERR "%s: No PCUI!\r\n", __FUNCTION__);
        return 1;
    }

    /* �ж���Ҫ��USB�˵����Ƿ񳬹�USB IP��֧�� */
    if ((ep_in_cnt > MAX_EPIN_NUM) || (ep_out_cnt > MAX_EPOUT_NUM))
    {
        printk(KERN_ERR "%s: Too many ports-ep_in_cnt[%d], ep_out_cnt[%d]\n", __FUNCTION__,
                ep_in_cnt, ep_out_cnt);
        return 1;
    }
    return 0;
#endif /*#ifdef MBB_USB_RNDIS*/
    /* �豸��̬�������� */
/* END PN: DTS2013101203547,Added by h00122846, 2013/10/14*/
#else	
    return OK;
#endif
}

/*****************************************************************************
��������   BSP_USB_GetAvailabePortType
��������:  �ṩ���ϲ��ѯ��ǰ�豸֧�ֶ˿���̬�б�ӿ�
��������� ulPortMax    Э��ջ֧�����˿���̬����
�������:  pucPortType  ֧�ֵĶ˿���̬�б�
           pulPortNum   ֧�ֵĶ˿���̬����
����ֵ��   0:    ��ȡ�˿���̬�б�ɹ�
           ��������ȡ�˿���̬�б�ʧ��
*****************************************************************************/
unsigned int BSP_USB_GetAvailabePortType(unsigned char *pucPortType,
                            unsigned long *pulPortNum, unsigned long ulPortMax)
{
#if (FEATURE_ON == MBB_USB)
/* BEGIN PN: DTS2013101203547,Added by h00122846, 2013/10/14*/
    unsigned int i = 0;
    unsigned int cnt = 0;
    unsigned int j = 0;

    if ((NULL == pucPortType) || (NULL == pulPortNum) || (0 == ulPortMax))
    {
        return 1;
    }  

    cnt = sizeof(gDevProfileSupported) / sizeof(char);
    
    printk(KERN_ERR "aTm support  fds\n");
    if (cnt > ulPortMax)
    {
        return 1;
    }

    for (i = 0; i < cnt; i++)
    {
        pucPortType[j++] = gDevProfileSupported[i];
    }

    *pulPortNum = j;
 /* END PN: DTS2013101203547,Added by h00122846, 2013/10/14*/
#endif  
    return OK;
}

/*****************************************************************************
 �� �� ��  : BSP_USB_ATProcessRewind2Cmd
 ��������  : rewind2 CMD ����
 �������  : pData������
 �������  : ��
 �� �� ֵ  : false(0):����ʧ��
             tool(1):����ɹ�
*****************************************************************************/
int BSP_USB_ATProcessRewind2Cmd(unsigned char *pData)
{
    return OK;
}

/*****************************************************************************
 �� �� ��  : BSP_USB_GetDiagModeValue
 ��������  : ����豸����
 �������  : ��
 �������  : ucDialmode:  0 - ʹ��Modem����; 1 - ʹ��NDIS����; 2 - Modem��NDIS����
              ucCdcSpec:   0 - Modem/NDIS������CDC�淶; 1 - Modem����CDC�淶;
                           2 - NDIS����CDC�淶;         3 - Modem/NDIS������CDC�淶
 �� �� ֵ  : VOS_OK/VOS_ERR
*****************************************************************************/
BSP_S32 BSP_USB_GetDiagModeValue(unsigned char *pucDialmode,
                             unsigned char *pucCdcSpec)
{
#if (FEATURE_ON == MBB_USB)
    unsigned int ret = 1;

    printk(KERN_ERR" BSP_USB_GetDiagModeValue \n");

    ret = Query_USB_PortType(pucDialmode,pucCdcSpec);
    if ( 0 != ret )
    {
        return 1; 
    }
#endif    
    return 0;
}

/*****************************************************************************
 �� �� ��  :     BSP_USB_GetPortMode
 ��������  :  ��ȡ�˿���̬ģʽ�����ضԽ����󣬴�׮��
 �������  :  PsBuffer   �����ѯ���Ķ˿����Ƽ��˿��ϱ���˳��
                            Length     ��¼*PsBuffer���ַ����ĳ���
 �������  : 
 �� �� ֵ  :      �ɹ�����0��ʧ�ܷ���1
*****************************************************************************/
unsigned char BSP_USB_GetPortMode(char*PsBuffer, unsigned long*Length )
{
#if (FEATURE_ON == MBB_USB)
    printk(KERN_ERR"BSP_USB_GetPortMode Enter!\r\n");
    unsigned char ret = 1;

    ret = Check_EnablePortName(PsBuffer,Length);
    if( 0 != ret )
    {
        return 1;
    }
#endif
    return 0;
}

/*****************************************************************************
 �� �� ��  : BSP_USB_GetU2diagDefaultValue
 ��������  : ��ö˿�Ĭ��u2diagֵ
 �������  : ��
 �������  : ��
 ����ֵ��   u2diagֵ

*****************************************************************************/
BSP_U32 BSP_USB_GetU2diagDefaultValue(void)
{
    return 0;
}

/*****************************************************************************
 �� �� ��  : ErrlogRegFunc
 ��������  : USB MNTNע���쳣��־�ӿڣ�Porting��Ŀ�д�׮
             Added by c00204787 for errorlog,20120211
 �������  : ��
 �������  : ��
 ����ֵ    ����

*****************************************************************************/
void MNTN_ERRLOG_REG_FUNC(MNTN_ERRLOGREGFUN pRegFunc)
{
    return ;
}

/*****************************************************************************
 �� �� ��  : BSP_USB_UdiagValueCheck
 ��������  : ���ӿ����ڼ��NV����USB��ֵ̬�ĺϷ���
 �������  : ��
 �������  : ��
 �� �� ֵ  : 0��OK
             -1��ERROR
*****************************************************************************/
int BSP_USB_UdiagValueCheck(unsigned long DiagValue)
{
    return OK;
}

/*****************************************************************************
 �� �� ��  : BSP_USB_GetLinuxSysType
 ��������  : ���ӿ����ڼ��PC���Ƿ�ΪLinux���Թ��Linux��̨���β���ʧ�ܵ�����.
 �������  : �ޡ�
 �������  : �ޡ�
 �� �� ֵ  : 0��Linux��
            -1����Linux��
*****************************************************************************/
int BSP_USB_GetLinuxSysType(void)
{
    return -1;
}

/********************************************************
����˵���� ���ص�ǰ�豸�б���֧��(sel=1)���߲�֧��(sel=0)PCSC���豸��ֵ̬
��������:
���������sel
          0: ͨ������dev_type���ص�ǰ����PCSC���豸��ֵ̬
          1��ͨ������dev_type���ص�ǰ��PCSC���豸��ֵ̬
���������dev_type д����Ҫ���豸��ֵ̬�����û����д��ֵ��
          NV�д洢���豸��ֵ̬
���������pulDevType ��ulCurDevType��Ӧ���豸��ֵ̬�����û�з���ֵ1��
����ֵ��
          0����ѯ����Ӧ���豸��ֵ̬��
          1��û�в�ѯ����Ӧ���豸��ֵ̬��
********************************************************/
int BSP_USB_PcscInfoSet(unsigned int  ulSel, unsigned int  ulCurDevType, unsigned int *pulDevType)
{
    return 0;
}

/********************************************************
����˵����Э��ջע��USBʹ��֪ͨ�ص�����
��������:
���������pFunc: USBʹ�ܻص�����ָ��
�����������
�����������
����ֵ  ��0���ɹ�
          1��ʧ��
********************************************************/
unsigned int BSP_USB_RegUdiEnableCB(USB_UDI_ENABLE_CB_T pFunc)
{
    if (g_usb_enum_done_cur >= USB_ENABLE_CB_MAX)
    {
        printk("BSP_USB_RegUdiEnableCB error:0x%x", (unsigned)pFunc);

        return 1;

    }

    g_usb_ctx.udi_enable_cb[g_usb_enum_done_cur] = pFunc;
    g_usb_enum_done_cur++;

	if (g_usb_enum_done_notify_complete)
    {
    	if (pFunc)
        	pFunc();
    }

    return 0;
}

/********************************************************
����˵����Э��ջע��USBȥʹ��֪ͨ�ص�����
��������:
���������pFunc: USBȥʹ�ܻص�����ָ��
�����������
�����������
����ֵ  ��0���ɹ�
          1��ʧ��
********************************************************/
unsigned int BSP_USB_RegUdiDisableCB(USB_UDI_DISABLE_CB_T pFunc)
{
    if (g_usb_enum_dis_cur >= USB_ENABLE_CB_MAX)
    {
        printk("BSP_USB_RegUdiDisableCB error:0x%x", (unsigned)pFunc);
        return (unsigned int)ERROR;
    }

    g_usb_ctx.udi_disable_cb[g_usb_enum_dis_cur] = pFunc;
    g_usb_enum_dis_cur++;

    return OK;
}

unsigned int BSP_USB_RegIpsTraceCB(USB_IPS_MNTN_TRACE_CB_T pFunc)
{
    if (!pFunc)
    {
        return 1;
    }

    return 0;
}

/********************************************************
����˵����Э��ջע��HSICʹ��֪ͨ�ص�����
��������:
���������pFunc: HSICʹ�ܻص�����ָ��
�����������
�����������
����ֵ  ��0���ɹ�
          1��ʧ��
********************************************************/
unsigned int BSP_HSIC_RegUdiEnableCB(HSIC_UDI_ENABLE_CB_T pFunc)
{
    return OK;
}

/********************************************************
����˵����Э��ջע��HSICȥʹ��֪ͨ�ص�����
��������:
���������pFunc: HSICȥʹ�ܻص�����ָ��
�����������
�����������
����ֵ��  0���ɹ�
          1��ʧ��
********************************************************/
unsigned int BSP_HSIC_RegUdiDisableCB(HSIC_UDI_DISABLE_CB_T pFunc)
{
    return OK;
}

unsigned long USB_ETH_DrvSetDeviceAssembleParam(
    unsigned long ulEthTxMinNum,
    unsigned long ulEthTxTimeout,
    unsigned long ulEthRxMinNum,
    unsigned long ulEthRxTimeout)
{
    return 0;
}

unsigned long USB_ETH_DrvSetHostAssembleParam(unsigned long ulHostOutTimeout)
{
#ifdef CONFIG_BALONG_NCM
    /* the interface don't include net_id, so we just set net_id:0 */
    (void)ncm_set_host_assemble_param(0, ulHostOutTimeout);
#endif
    return 0;
}

int USB_otg_switch_get(UINT8 *pvalue)
{
    return 0;
}

int USB_otg_switch_set(UINT8 value)
{
    return 0;
}

int USB_otg_switch_signal_set(UINT8 group,UINT8 pin, UINT8 value )
{
    return 0;
}

/********************************************************
����˵����Э��ջ��ѯHSICö��״̬
��������:
�����������
�����������
�����������
����ֵ��  1��ö�����
          0��ö��δ���
********************************************************/
unsigned int BSP_GetHsicEnumStatus(void)
{
    return OK;
}

/*****************************************************************************
 �� �� ��  : BSP_USB_HSIC_SupportNcm
 ��������  : ���ӿ����ڲ�ѯHSIC�Ƿ�֧��NCM
 �������  : NA
 �������  : NA
 �� �� ֵ  : 1-- ֧��NCM
             0--��֧��NCM
*****************************************************************************/
int BSP_USB_HSIC_SupportNcm(void)
{
    return 0;
}

/********************************************************
����˵����TTF��ѯԤ����SKB Num
��������:
�����������
�����������
�����������
����ֵ��  SKB Num
********************************************************/
BSP_U32 BSP_AcmPremallocSkbNum(void)
{
    return 0;
}

/********************************************************
��������   BSP_UDI_FindVcom
���������� ��ѯ��ǰ�豸��̬��ָ����UDI���⴮�ڶ˿��Ƿ����
��������� UDI_DEVICE_IDö��ֵ��������ѯ�����⴮�ڶ˿�ID
��������� ��
����ֵ��
           0����ǰ�豸��̬��֧�ֲ�ѯ�����⴮�ڶ˿ڣ�
           1����ǰ�豸��̬֧�ֲ�ѯ�����⴮�ڶ˿ڡ�
ע����� ��
********************************************************/
int BSP_UDI_FindVcom(UDI_DEVICE_ID enVCOM)
{
    return 0;
}

/*****************************************************************************
* �� �� ��  : DRV_USB_RegEnumDoneForMsp
* ��������  : �ṩ��MSPע��USBö����ɺ�֪ͨ����
* �������  : pFunc:ö����ɻص�����ָ��
* �������  : ��
* �� �� ֵ  : 0: �ɹ�ע��,�ȴ�ö�����֪ͨ;
*             1: USB�Ѿ�ö�����,����ֱ�ӳ�ʼ��USB����;
*             -1: ʧ��, ��USB��̬,û��USB����
*****************************************************************************/
signed int BSP_USB_RegEnumDoneForMsp(void *pFunc)
{
    /* we suggest to use the new interface for usb insert/remove */
    return BSP_USB_RegUdiEnableCB(pFunc);
}

/*****************************************************************************
* �� �� ��  : BSP_USB_RegEnumDoneForPs
* ��������  : �ṩ��PSע��USBö����ɺ�֪ͨ����
* �������  : pFunc:ö����ɻص�����ָ��
* �������  : ��
* �� �� ֵ  : 0: �ɹ�ע��,�ȴ�ö�����֪ͨ;
*             1: USB�Ѿ�ö�����,����ֱ�ӳ�ʼ��USB����;
*             -1: ʧ��,��USB��̬,û��USB����
*****************************************************************************/
signed int BSP_USB_RegEnumDoneForPs(void *pFunc)
{
    /* we suggest to use the new interface for usb insert/remove */
    return BSP_USB_RegUdiEnableCB(pFunc);
}

/*****************************************************************************
 �� �� ��  : BSP_USB_RndisAppEventDispatch
 ��������  : ���ӿ�����֪ͨAPP ��Ӧ��USB����¼�
 �������  : usb�¼�
 �� �� ֵ  : ��
*****************************************************************************/
void BSP_USB_RndisAppEventDispatch(unsigned ulStatus)
{
    return ;
}

/************************************************************************
 * FUNCTION
 *       rndis_app_event_dispatch
 * DESCRIPTION
 *       ���翨�汾����Ӧ�ý��в��Ż�Ͽ���������
 * INPUTS
 *       ���в��Ż�Ͽ�����ָʾ
 * OUTPUTS
 *       NONE
 *************************************************************************/
VOID rndis_app_event_dispatch(unsigned int ulStatus)
{
    return ;
}

/*****************************************************************************
 �� �� ��  : BSP_USB_NASSwitchGatewayRegExtFunc
 ��������  : ���ӿ�����NASע���л�����֪ͨ�ص�����
 �������  : �ص��ӿڡ�
 �������  : �ޡ�
 �� �� ֵ  : 0���ɹ�
             ��:ʧ��
*****************************************************************************/
int BSP_USB_NASSwitchGatewayRegFunc(USB_NET_DEV_SWITCH_GATEWAY switchGwMode)
{
#if (FEATURE_ON == MBB_USB)
    usb_adp_ctx_t* ctx = NULL;

    ctx = usb_get_adp_ctx();
    if (NULL == ctx)
    {
        printk(KERN_ERR"NASS set gateway func fail \n");
        return ERROR;
    }
    ctx->usb_switch_gatway_mode_cb = switchGwMode;
#endif
    return OK;
}

/*****************************************************************************
 �� �� ��  : bsp_usb_register_enablecb
 ��������  : ���ӿ�����Cshell֪ͨUSB����ص�����
 �������  : �ص��ӿڡ�
 �������  : �ޡ�
 �� �� ֵ  : 0���ɹ�
             ��:ʧ��
*****************************************************************************/
int bsp_usb_register_enablecb(USB_UDI_ENABLE_CB_T pFunc)
{
    /* we suggest to use the new interface for usb insert/remove */
    return BSP_USB_RegUdiEnableCB(pFunc);
}

/*****************************************************************************
 �� �� ��  : bsp_usb_register_disablecb
 ��������  : ���ӿ�����Cshell֪ͨUSB�γ��ص�����
 �������  : �ص��ӿڡ�
 �������  : �ޡ�
 �� �� ֵ  : 0���ɹ�
             ��:ʧ��
*****************************************************************************/
int bsp_usb_register_disablecb(USB_UDI_DISABLE_CB_T pFunc)
{
    /* we suggest to use the new interface for usb insert/remove */
    return BSP_USB_RegUdiDisableCB(pFunc);
}

unsigned long USB_ETH_DrvSetRxFlowCtrl(unsigned long ulParam1, unsigned long ulParam2)
{
    return 0;
}

unsigned long USB_ETH_DrvClearRxFlowCtrl(unsigned long ulParam1, unsigned long ulParam2)
{
    return 0;
}

/************************************************************************
 * FUNCTION
 *       rndis_app_event_dispatch
 * DESCRIPTION
 *       ���翨�汾����Ӧ�ý��в��Ż�Ͽ���������
 * INPUTS
 *       ���в��Ż�Ͽ�����ָʾ
 * OUTPUTS
 *       NONE
 *************************************************************************/
VOID DRV_AT_SETAPPDAILMODE(unsigned int ulStatus)
{

}

int l2_notify_register(FUNC_USB_LP_NOTIFY pUSBLPFunc)
{
    return 0;
}

static int gs_usb_adp_notifier_cb(struct notifier_block *nb,
            unsigned long event, void *priv)
{
    int loop;

    switch (event) {

    case USB_BALONG_DEVICE_INSERT:
        g_usb_disable_notify_complete = 0;
        break;
    case USB_BALONG_ENUM_DONE:
        /* enum done */
        g_usb_disable_notify_complete = 0;
        if (!g_usb_enum_done_notify_complete) {
            for(loop = 0; loop < USB_ENUM_DONE_CB_BOTTEM; loop++)
            {
                if(g_usb_ctx.enum_done_cbs[loop])
                    g_usb_ctx.enum_done_cbs[loop]();
            }

            for(loop = 0; loop < USB_ENABLE_CB_MAX; loop++)
            {
                if(g_usb_ctx.udi_enable_cb[loop])
                    g_usb_ctx.udi_enable_cb[loop]();
            }
        }
        g_usb_enum_done_notify_complete = 1;
        break;
    case USB_BALONG_DEVICE_DISABLE:
    case USB_BALONG_DEVICE_REMOVE:
        /* notify other cb */
        g_usb_enum_done_notify_complete = 0;
        if (!g_usb_disable_notify_complete) {
            for(loop = 0; loop < USB_ENABLE_CB_MAX; loop++)
            {
                if(g_usb_ctx.udi_disable_cb[loop])
                    g_usb_ctx.udi_disable_cb[loop]();
            }
            g_usb_disable_notify_complete = 1;
        }
        break;
    default:
        break;
    }
    return 0;
}


int __init adp_usb_init(void)
{
    /* we just regist once, and don't unregist any more */
    if (!gs_adp_usb_nb_ptr) {
        gs_adp_usb_nb_ptr = &gs_adp_usb_nb;
        gs_adp_usb_nb.priority = USB_NOTIF_PRIO_ADP;
        gs_adp_usb_nb.notifier_call = gs_usb_adp_notifier_cb;
        bsp_usb_register_notify(gs_adp_usb_nb_ptr);
    }
    return 0;
}
module_init(adp_usb_init);
/*lint -restore*/