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


#ifndef _DRV_MAILBOX_PORT_LINUX_H_
#define _DRV_MAILBOX_PORT_LINUX_H_


/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include <linux/kernel.h>

/*��ʵ��ַת���ӿڶ���*/
#ifndef _DRV_LLT_

/* Modified by c64416 for hifi mailbox, 2013/09/24, begin */
/*ACPUʵ��ַ�������ַӳ���ת���ӿ�*/
#define MEM_CORE_SHARE_PHY2VIRT(phy)         (((unsigned int)phy)  - DDR_SHARED_MEM_ADDR + DDR_SHARED_MEM_VIRT_ADDR)
#define MEM_CORE_SHARE_VIRT2PHY(virt)        (((unsigned int)virt) - DDR_SHARED_MEM_VIRT_ADDR + DDR_SHARED_MEM_ADDR)
/* Modified by c64416 for hifi mailbox, 2013/09/24, end */

#else

/*ACPUʵ��ַ�������ַӳ���ת���ӿ�*/
#define MEM_CORE_SHARE_PHY2VIRT(phy)         (phy)
#define MEM_CORE_SHARE_VIRT2PHY(virt)        (virt)
#endif

/*����ͨ�������ܣ��˵�������ýӿڶ���*/
/*������CPU��ID*/
#define MAILBOX_LOCAL_CPUID                         MAILBOX_CPUID_ACPU


/* Modified by c64416 for hifi mailbox, 2013/09/24, begin */
/*����C��ƽ̨��ص� ����ͨ�� �����������շ�ͨ�� */
#define MAILBOX_CHANNEL_NUM \
    ( MAILBOX_CHANNEL_BUTT(ACPU, HIFI)   \
    + MAILBOX_CHANNEL_BUTT(HIFI, ACPU)   \
    )

/*����C��ƽ̨��ص� �ʼ� ���� , ֻ���Ľ���ͨ��*/
#define MAILBOX_USER_NUM    \
    ( MAILBOX_USER_BUTT(HIFI, ACPU, MSG) \
    )
/* Modified by c64416 for hifi mailbox, 2013/09/24, end */

/*��ӡ���������ö���*/
/*����̨��ӡ����ӿ�*/
#ifdef _DRV_LLT_
#define mailbox_out(p)                              (printf p)
#else
#define mailbox_out(p)                              (printk p)
#endif

/*���������Ƿ��ӡ����*/
#define _MAILBOX_LINE_                              __LINE__

/*���������Ƿ��ӡ�ļ���,(����ӡ�ļ����ɽ�ʡ����ռ�)*/
#define _MAILBOX_FILE_                              (void*)(0) /*__FILE__*/

#define MAILBOX_LOG_LEVEL                           MAILBOX_LOG_INFO 


/*������Թ���������ö���*/
/*�����ڴ�CPU�ϴ򿪿�ά�ɲ⹦��*/
#ifndef MAILBOX_OPEN_MNTN
#define MAILBOX_OPEN_MNTN
#endif

/*��ά�ɲ��¼�������ID��*/
#define MAILBOX_RECORD_USEID_NUM                    (64)

#define RT  "\n"   /*��ӡ�س�����*/

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  /*_DRV_MAILBOX_LINUX_H_*/

