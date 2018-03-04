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


/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#include "drv_mailbox.h"
#include "drv_mailbox_cfg.h"
#include "bsp_nvim.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifndef BSP_CONFIG_HI3630
#ifdef CONFIG_HIFI


/*****************************************************************************
  2 ��������
*****************************************************************************/
unsigned long mailbox_send_msg(
                unsigned long            mailcode,
                void                    *data,
                unsigned long            length);

unsigned long mailbox_reg_msg_cb(
                unsigned long             mailcode,
                mb_msg_cb                 func,
                void                     *data);

unsigned long mailbox_read_msg_data(
                void                   *mail_handle,
                 char                  *buff,
                unsigned long          *size);
/*****************************************************************************
  3 ����ʵ��
*****************************************************************************/

/*****************************************************************************
 �� �� ��  : DRV_MAILBOX_SENDMAIL
 ��������  : �����û������ʼ�����
 �������  : MailCode   -- �ʱ�, ָ���ʼ�����Ŀ��
             pData      -- ָ���ʼ����ݵ�ָ��
             Length     -- ���������ݳ���, ��λbyte
 �������  : ��
 �� �� ֵ  : ��ȷ:MAILBOX_OK / ����:MAILBOX_ERRO��������������
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��9��28��
    ��    ��   : Ī�� 00176101
    �޸�����   : �����ɺ���

*****************************************************************************/
unsigned long DRV_MAILBOX_SENDMAIL(
                unsigned long           MailCode,
                void                   *pData,
                unsigned long           Length)
{
    return mailbox_send_msg(MailCode, pData, Length);
}

/*****************************************************************************
 �� �� ��  : DRV_MAILBOX_REGISTERRECVFUNC
 ��������  : ע���ʼ����ݽ��պ���
 �������  : MailCode   -- ���ݽ��պ�������Ķ����ʱ�
             pFun       -- �ʼ����ݽ��մ�����ָ��
             UserHandle -- �û�������, ���ݽ��պ���������ʱ����Ϊ���, �������
 �������  : ��
 �� �� ֵ  : ��ȷ:MAILBOX_OK / ����:MAILBOX_ERRO��������������
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��9��29��
    ��    ��   : Ī�� 00176101
    �޸�����   : �����ɺ���

*****************************************************************************/
unsigned long DRV_MAILBOX_REGISTERRECVFUNC(
                unsigned long           MailCode,
                mb_msg_cb               pFun,
                void                   *UserHandle)
{
    return mailbox_reg_msg_cb(MailCode, pFun, UserHandle);
}

/*****************************************************************************
 �� �� ��  : DRV_MAILBOX_READMAILDATA
 ��������  : �����û������ݽ��ջص������е���, �������ж�ȡһ�����ȵ�����ʼ�
 �������  : MailHandle -- ������, ���ݽ��ջص��������
             pData      -- ������������ݵĻ����ַ
             *pSize     -- ���泤��, ��λbyte, ��Ҫ���ڴ���ȡ�ʼ�����
 �������  : *pSize     -- ʵ�ʶ�ȡ����, ��λbyte
 �� �� ֵ  : ��ȷ:MAILBOX_OK / ����:MAILBOX_ERRO��������������
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��10��26��
    ��    ��   : Ī�� 00176101
    �޸�����   : �����ɺ���

*****************************************************************************/
unsigned long DRV_MAILBOX_READMAILDATA(
                void                   *MailHandle,
                unsigned char          *pData,
                unsigned long          *pSize)
{
    return mailbox_read_msg_data(MailHandle, (char *)pData, pSize);
}

#define HIFI_MBX_OFFSET    0xa0000000
/*****************************************************************************
 �� �� ��  : drv_hifi_fill_mb_info
 ��������  : ������Ϣ���ýӿ�
 �������  : addr ����Ϣ��ַ(��HIFI����ģ�鴫��)
 �������  : 
 �� �� ֵ  : void
 ���ú���  : V9R1����fastboot�е��ã�V7R2����DSPһ����ص���
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��11��4��
    ��    ��   : �޾�ǿ 64416
    �޸�����   : �����ɺ���

*****************************************************************************/
void drv_hifi_fill_mb_info(unsigned int addr)
{
    CARM_HIFI_DYN_ADDR_SHARE_STRU *pdata;

    pdata = (CARM_HIFI_DYN_ADDR_SHARE_STRU*)addr;
    
    pdata->stCarmHifiMB.uwHifi2CarmMailBoxLen   = MAILBOX_QUEUE_SIZE(HIFI, CCPU, MSG);
    pdata->stCarmHifiMB.uwCarm2HifiMailBoxLen   = MAILBOX_QUEUE_SIZE(CCPU, HIFI, MSG);
    pdata->stCarmHifiMB.uwHifiCarmHeadAddr      = HIFI_MBX_OFFSET + MAILBOX_HEAD_ADDR(HIFI, CCPU, MSG);
    pdata->stCarmHifiMB.uwHifiCarmBodyAddr      = HIFI_MBX_OFFSET + MAILBOX_QUEUE_ADDR(HIFI, CCPU, MSG);
    pdata->stCarmHifiMB.uwCarmHifiHeadAddr      = HIFI_MBX_OFFSET + MAILBOX_HEAD_ADDR(CCPU, HIFI, MSG);
    pdata->stCarmHifiMB.uwCarmHifiBodyAddr      = HIFI_MBX_OFFSET + MAILBOX_QUEUE_ADDR(CCPU, HIFI, MSG);
    pdata->stCarmHifiMB.uwProtectWord           = HIFI_MB_ADDR_PROTECT;

    pdata->stAarmHifiMB.uwHifi2AarmMailBoxLen   = MAILBOX_QUEUE_SIZE(HIFI, ACPU, MSG);
    pdata->stAarmHifiMB.uwAarm2HifiMailBoxLen   = MAILBOX_QUEUE_SIZE(ACPU, HIFI, MSG);
    pdata->stAarmHifiMB.uwHifiAarmHeadAddr      = HIFI_MBX_OFFSET + MAILBOX_HEAD_ADDR(HIFI, ACPU, MSG);
    pdata->stAarmHifiMB.uwHifiAarmBodyAddr      = HIFI_MBX_OFFSET + MAILBOX_QUEUE_ADDR(HIFI, ACPU, MSG);
    pdata->stAarmHifiMB.uwAarmHifiHeadAddr      = HIFI_MBX_OFFSET + MAILBOX_HEAD_ADDR(ACPU, HIFI, MSG);
    pdata->stAarmHifiMB.uwAarmHifiBodyAddr      = HIFI_MBX_OFFSET + MAILBOX_QUEUE_ADDR(ACPU, HIFI, MSG);
    pdata->stAarmHifiMB.uwProtectWord           = HIFI_MB_ADDR_PROTECT;
    
    pdata->uwNvBaseAddrPhy = HIFI_MBX_OFFSET + SHD_DDR_V2P(NV_GLOBAL_CTRL_INFO_ADDR);
    pdata->uwNvBaseAddrVirt = HIFI_MBX_OFFSET + NV_GLOBAL_CTRL_INFO_ADDR;
    
    pdata->uwProtectWord = HIFI_MB_ADDR_PROTECT;
    
    return ;
}
#else
/*****************************************************************************
 �� �� ��  : DRV_MAILBOX_SENDMAIL
 ��������  : �����û������ʼ�����
 �������  : MailCode   -- �ʱ�, ָ���ʼ�����Ŀ��
             pData      -- ָ���ʼ����ݵ�ָ��
             Length     -- ���������ݳ���, ��λbyte
 �������  : ��
 �� �� ֵ  : ��ȷ:MAILBOX_OK / ����:MAILBOX_ERRO��������������
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��9��28��
    ��    ��   : Ī�� 00176101
    �޸�����   : �����ɺ���

*****************************************************************************/
unsigned long DRV_MAILBOX_SENDMAIL(
                unsigned long           MailCode,
                void                   *pData,
                unsigned long           Length)
{
    return MAILBOX_OK;
}

/*****************************************************************************
 �� �� ��  : DRV_MAILBOX_REGISTERRECVFUNC
 ��������  : ע���ʼ����ݽ��պ���
 �������  : MailCode   -- ���ݽ��պ�������Ķ����ʱ�
             pFun       -- �ʼ����ݽ��մ�����ָ��
             UserHandle -- �û�������, ���ݽ��պ���������ʱ����Ϊ���, �������
 �������  : ��
 �� �� ֵ  : ��ȷ:MAILBOX_OK / ����:MAILBOX_ERRO��������������
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��9��29��
    ��    ��   : Ī�� 00176101
    �޸�����   : �����ɺ���

*****************************************************************************/
unsigned long DRV_MAILBOX_REGISTERRECVFUNC(
                unsigned long           MailCode,
                mb_msg_cb               pFun,
                void                   *UserHandle)
{
    return MAILBOX_OK;
}

/*****************************************************************************
 �� �� ��  : DRV_MAILBOX_READMAILDATA
 ��������  : �����û������ݽ��ջص������е���, �������ж�ȡһ�����ȵ�����ʼ�
 �������  : MailHandle -- ������, ���ݽ��ջص��������
             pData      -- ������������ݵĻ����ַ
             *pSize     -- ���泤��, ��λbyte, ��Ҫ���ڴ���ȡ�ʼ�����
 �������  : *pSize     -- ʵ�ʶ�ȡ����, ��λbyte
 �� �� ֵ  : ��ȷ:MAILBOX_OK / ����:MAILBOX_ERRO��������������
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��10��26��
    ��    ��   : Ī�� 00176101
    �޸�����   : �����ɺ���

*****************************************************************************/
unsigned long DRV_MAILBOX_READMAILDATA(
                void                   *MailHandle,
                unsigned char          *pData,
                unsigned long          *pSize)
{
    *pSize = 0;
    return MAILBOX_OK;
}
/*****************************************************************************
 �� �� ��  : drv_hifi_fill_mb_info
 ��������  : ������Ϣ���ýӿ�
 �������  : addr ����Ϣ��ַ(��HIFI����ģ�鴫��)
 �������  : 
 �� �� ֵ  : void
 ���ú���  : V9R1����fastboot�е��ã�V7R2����DSPһ����ص���
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��11��4��
    ��    ��   : �޾�ǿ 64416
    �޸�����   : �����ɺ���

*****************************************************************************/
void drv_hifi_fill_mb_info(unsigned int addr)
{
    return ;
}
#endif
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

