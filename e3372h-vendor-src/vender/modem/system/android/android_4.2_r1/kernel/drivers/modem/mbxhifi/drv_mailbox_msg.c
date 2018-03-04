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
#include "drv_mailbox_debug.h"
#include "drv_mailbox_gut.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef CONFIG_HIFI
/*lint --e{713, 732, 737, 569, 830}*/
/*****************************************************************************
    ��ά�ɲ���Ϣ�а�����C�ļ���ź궨��
*****************************************************************************/
#undef  _MAILBOX_FILE_
#define _MAILBOX_FILE_   "msg"
/*****************************************************************************
  2 ȫ�ֱ�������
*****************************************************************************/
/*****************************************************************************
 �� �� ��  : mailbox_msg_receiver
 ��������  : �˼��ʼ����ݽ��ջص�������ע��
 �������  : void *mb_buf     -- ��������ͨ�������
             void *handle     -- �û����.
             void *data       -- �û�����.
 �������  : ��
 �� �� ֵ  : MAILBOX_OK, �쳣����ֵ
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��9��29��
    ��    ��   : Ī�� 00176101
    �޸�����   : �����ɺ���

*****************************************************************************/

void mailbox_msg_receiver(void *mb_buf, void *handle, void *data)
{
    struct mb_queue * queue;  /*����buffer��ʱ��������ڴ����û��ص�*/
    struct mb_buff  * mbuf = ( struct mb_buff  *)mb_buf;
    mb_msg_cb  func = (mb_msg_cb)handle;

    queue = &mbuf->usr_queue;
    if (func) {
        func(data, (void *)queue, queue->size);
    } else {
        mailbox_logerro_p1(MAILBOX_ERR_GUT_READ_CALLBACK_NOT_FIND, mbuf->mailcode);
    }
}

MAILBOX_EXTERN unsigned long mailbox_reg_msg_cb(
                unsigned long             mailcode,
                mb_msg_cb                 func,
                void                     *data)
{
    /*[false alarm]:��*/
    return mailbox_register_cb(mailcode, mailbox_msg_receiver, func, data);
}

/*****************************************************************************
 �� �� ��  : mailbox_try_send_msg
 ��������  : ����һ���ʼ����Ͷ���
 �������  :
             unsigned long      mailcode        -- �����߼�ͨ����
             unsigned char      *pdata           -- ָ���ʼ����ݵ�ָ�롣
             unsigned long      length/byte     -- ���ݻ�����Ϣ�ܳ��� �ֽ�Ϊ��λ��
 �������  : ��
 �� �� ֵ  : unsigned long
             �ɹ�OK, �쳣����ֵ
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��9��28��
    ��    ��   : Ī�� 00176101
    �޸�����   : �����ɺ���

*****************************************************************************/
MAILBOX_EXTERN unsigned long mailbox_try_send_msg(
                unsigned long            mailcode,
                void                    *pdata,
                unsigned long            length)
{
    struct mb_buff      *mb_buf = MAILBOX_NULL;
    struct mb_queue     *queue  = MAILBOX_NULL;
    unsigned long        ret_val    = MAILBOX_OK;

   if ((0 == pdata) || (0 == length)) {
        /*[false alarm]:��*/
        return mailbox_logerro_p1(MAILBOX_ERRO, mailcode);
   }
    /*��ȡ����buffer*/
    ret_val = (unsigned long)mailbox_request_buff(mailcode, &mb_buf);
    if (MAILBOX_OK != ret_val) {
        goto exit_out;
    }

    /*����û�����*/
    queue = &mb_buf->usr_queue;
    if ( length != (unsigned long)mailbox_write_buff( queue, pdata, length)) {
         ret_val = mailbox_logerro_p1(MAILBOX_FULL, mailcode);
         goto exit_out;
    }

    /*����*/
    ret_val = mailbox_sealup_buff( mb_buf,  length);
    if (MAILBOX_OK == ret_val) {
         /*�����ʼ�*/
        ret_val = mailbox_send_buff(mb_buf);
    }

exit_out:
    /*�ͷ�����buffer*/
    if (MAILBOX_NULL != mb_buf) {
        mailbox_release_buff(mb_buf);
    }

    return ret_val;
}


/****************************************************************************
 �� �� ��  : mailbox_read_msg_data
 ��������  : �����û������ݽ��ջص������е���, �������ж�ȡһ�����ȵ�����ʼ�
 �������  : mail_handle -- ������, ���ݽ��ջص��������
             buff      -- ������������ݵĻ����ַ
             *size     -- ���泤��, ��λbyte, ��Ҫ���ڴ���ȡ�ʼ�����
 �������  : *size     -- ʵ�ʶ�ȡ����, ��λbyte
 �� �� ֵ  : MAILBOX_OK / MAILBOX_ERRO
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��10��26��
    ��    ��   : Ī�� 00176101
    �޸�����   : �����ɺ���

*****************************************************************************/
MAILBOX_GLOBAL unsigned long mailbox_read_msg_data(
                void                   *mail_handle,
                char                   *buff,
                unsigned long          *size)
{
    struct mb_queue *pMailQueue = (struct mb_queue *)mail_handle;

    if ((MAILBOX_NULL == pMailQueue) || (MAILBOX_NULL == buff) || (MAILBOX_NULL == size)) {
        /*[false alarm]:��*/
        return mailbox_logerro_p1(MAILBOX_ERR_GUT_INPUT_PARAMETER, 0);
    }

    if (pMailQueue->size  >  *size) {
        /*[false alarm]:��*/
        return mailbox_logerro_p1(MAILBOX_ERR_GUT_USER_BUFFER_SIZE_TOO_SMALL, *size);
    }

    /*����û����ص��������ݶ��о������Ч��*/
    if ((0 == pMailQueue->length) ||
        ((pMailQueue->front - pMailQueue->base) >  pMailQueue->length ) ||
        ((pMailQueue->rear - pMailQueue->base) >  pMailQueue->length )) {
        /*[false alarm]:��*/
        return mailbox_logerro_p1(MAILBOX_CRIT_GUT_INVALID_USER_MAIL_HANDLE, pMailQueue);
    }

    *size =  mailbox_read_buff(pMailQueue, buff, pMailQueue->size);

    return MAILBOX_OK;
}

/*****************************************************************************
 �� �� ��  : mailbox_send_msg
 ��������  : �ṩ����ģ����к˼����ݷ��͵Ľӿڡ�
 �������  :
             unsigned long       mailcode      -- �ʼ���id��, �߼�ͨ���š�
             unsigned char      *data           -- ָ���ʼ����ݵ�ָ�롣
             unsigned long       length/byte     -- ���ݻ�����Ϣ�ܳ��� �ֽ�Ϊ��λ��
 �������  : ��
 �� �� ֵ  : unsigned long
             �ɹ�OK, �쳣����ֵ
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��9��28��
    ��    ��   : Ī�� 00176101
    �޸�����   : �����ɺ���

*****************************************************************************/
MAILBOX_EXTERN unsigned long mailbox_send_msg(
                unsigned long            mailcode,
                void                    *data,
                unsigned long            length)
{
    unsigned long  ret_val;
    unsigned long  try_go_on = MAILBOX_TRUE;
    unsigned long  try_times = 0;

/* Modified by c64416 for hifi mailbox, 2013/09/24, begin */
#if 0
    ret_val= BSP_CPU_StateGet(mailbox_get_dst_id(mailcode));
    if(!ret_val)
    {
        return MAILBOX_TARGET_NOT_READY;
    }
#else
    if(mailbox_get_dst_id(mailcode) != MAILBOX_CPUID(HIFI))
    {
        return MAILBOX_TARGET_NOT_READY;
    }
#endif
/* Modified by c64416 for hifi mailbox, 2013/09/24, end */

    ret_val = mailbox_try_send_msg(mailcode, data, length);

    if (MAILBOX_FALSE == mailbox_int_context()) {
        /*�������ȴ���ѯ����*/
        while (MAILBOX_FULL == ret_val) {
            mailbox_delivery(mailbox_get_channel_id(mailcode));
            try_go_on = mailbox_scene_delay(MAILBOX_DELAY_SCENE_MSG_FULL, &try_times);

            if (MAILBOX_TRUE == try_go_on) {
                ret_val = mailbox_try_send_msg(mailcode, data, length);
            } else {
                break;
            }
        }
    }

    if (MAILBOX_OK != ret_val) {
        /*mailbox_show(mailcode,0);*/
        /*mailbox_assert(ret_val);*/
        if (MAILBOX_FULL != ret_val) {
            ret_val = MAILBOX_ERRO;
        }
        return ret_val;
    }

    return ret_val;
}

#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

