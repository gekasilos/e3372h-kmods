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


#include "bsp_om_api.h"
#include "bsp_om.h"
#include "drv_comm.h"
#include "osl_irq.h"


#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ENABLE_BUILD_OM
/*****************************************************************************
* �� �� ��  : DRV_OM_SET_HSO_CONN_FLAG
*
* ��������  : flag :���ӱ�־��1��ʾ���ӣ�0��ʾ�Ͽ�
*
* �������  :��
*
*
* �������  :��
*
* �� �� ֵ  :��
*****************************************************************************/

BSP_VOID DRV_OM_SET_HSO_CONN_FLAG(BSP_U32 flag)
{
    return;
}

/*****************************************************************************
* �� �� ��  : DRV_SYSVIEW_SWT_SET
*
* ��������  :sysview�ϱ��������ã���װ�������ṩ��MSPʹ��
*
* �������  :tarce_type :trace����
*                        set_swt  :����ֵ
*                       period  :�ϱ�����
*
* �������  : ��
*
* �� �� ֵ  : BSP_OK
*****************************************************************************/

BSP_U32 DRV_SYSVIEW_SWT_SET(BSP_U32 set_type,BSP_U32 set_swt,BSP_U32 period)
{
    return 0;
}

/*****************************************************************************
* �� �� ��  : DRV_GET_ALL_TASK_ID_NAME
*
* ��������  :��ȡ��ǰ���������ID������
*
* �������  :p_task_stru:�������ƺ�ID����Ľṹ��
*                       param_len:����1�ĳ��ȣ���λ�ֽ�
*
*
* �������  : ��
*
* �� �� ֵ  : BSP_OK
*****************************************************************************/
BSP_U32 DRV_GET_ALL_TASK_ID_NAME(BSP_VOID *p_task_stru,u32 param_len)
{
      return 0;
}


/*****************************************************************************
* �� �� ��  : DRV_INT_LOCK
*
* ��������  :���жϽӿڣ��������ж�ͳ��
*
* �������  :��
*
* �������  : ��
*
* �� �� ֵ  : ���жϺ��õ�key
*****************************************************************************/
BSP_S32 DRV_INT_LOCK(BSP_VOID)
{
    return 0;
}

/*****************************************************************************
* �� �� ��  : DRV_INT_UNLOCK
*
* ��������  : �����жϽӿڣ��������ж�ͳ��
*
* �������  : ���жϺ��õ�key
*
* �������  : ��
*
* �� �� ֵ  : ��
*****************************************************************************/
BSP_VOID DRV_INT_UNLOCK(BSP_S32 LockKey)
{
    return;
}


/*****************************************************************************
* �� �� ��  : DRV_SOCP_CHAN_ENABLE
*
* ��������  : ����SOCPͨ��ʹ�ܲ���
*
* �������  : ��
*
*
* �������  :��
*
* �� �� ֵ  :��
*****************************************************************************/

BSP_VOID DRV_SOCP_CHAN_ENABLE(BSP_VOID)
{
    return;
}

/*****************************************************************************
* �� �� ��  : DRV_LOG_BIN_IND
*
* ��������  : ���������ϱ��ӿ�
*
* �������  : 
*
*
* �������  :��
*
* �� �� ֵ  :��
*****************************************************************************/
BSP_VOID DRV_LOG_BIN_IND(BSP_S32 str_id, BSP_VOID* ind_data, BSP_S32 ind_data_size)
{
	return;
}

/*****************************************************************************
* �� �� ��  : DRV_LOG_BIN_IND_REGISTER
*
* ��������  : ���������ϱ�ע��ӿ�
*
* �������  : 
*
*
* �������  :��
*
* �� �� ֵ  :��
*****************************************************************************/
BSP_VOID DRV_LOG_BIN_IND_REGISTER(log_bin_ind_cb log_ind_cb)
{
	return;
}

#else
/*****************************************************************************
* �� �� ��  : DRV_OM_SET_HSO_CONN_FLAG
*
* ��������  : flag :���ӱ�־��1��ʾ���ӣ�0��ʾ�Ͽ�
*
* �������  :��
*
*
* �������  :��
*
* �� �� ֵ  :��
*****************************************************************************/

BSP_VOID DRV_OM_SET_HSO_CONN_FLAG(BSP_U32 flag)
{
    bsp_om_set_hso_conn_flag(flag);
}

#ifndef ENABLE_BUILD_SYSVIEW
/*****************************************************************************
* �� �� ��  : DRV_SYSVIEW_SWT_SET
*
* ��������  :sysview�ϱ��������ã���װ�������ṩ��MSPʹ��
*
* �������  :tarce_type :trace����
*                        set_swt  :����ֵ
*                       period  :�ϱ�����
*
* �������  : ��
*
* �� �� ֵ  : BSP_OK
*****************************************************************************/

BSP_U32 DRV_SYSVIEW_SWT_SET(BSP_U32 set_type,BSP_U32 set_swt,BSP_U32 period)
{
    return 0;
}

/*****************************************************************************
* �� �� ��  : DRV_GET_ALL_TASK_ID_NAME
*
* ��������  :��ȡ��ǰ���������ID������
*
* �������  :p_task_stru:�������ƺ�ID����Ľṹ��
*                       param_len:����1�ĳ��ȣ���λ�ֽ�
*
*
* �������  : ��
*
* �� �� ֵ  : BSP_OK
*****************************************************************************/
BSP_U32 DRV_GET_ALL_TASK_ID_NAME(BSP_VOID *p_task_stru,u32 param_len)
{
      return 0;
}

#else
/*****************************************************************************
* �� �� ��  : DRV_SYSVIEW_SWT_SET
*
* ��������  :sysview�ϱ��������ã���װ�������ṩ��MSPʹ��
*
* �������  :tarce_type :trace����
*                        set_swt  :����ֵ
*                       period  :�ϱ�����
*
* �������  : ��
*
* �� �� ֵ  : BSP_OK
*****************************************************************************/

BSP_U32 DRV_SYSVIEW_SWT_SET(BSP_U32 set_type,BSP_U32 set_swt,BSP_U32 period)
{
    return bsp_sysview_swt_set(set_type,set_swt,period);
}

/*****************************************************************************
* �� �� ��  : DRV_GET_ALL_TASK_ID_NAME
*
* ��������  :��ȡ��ǰ���������ID������
*
* �������  :p_task_stru:�������ƺ�ID����Ľṹ��
*                       param_len:����1�ĳ��ȣ���λ�ֽ�
*
*
* �������  : ��
*
* �� �� ֵ  : BSP_OK
*****************************************************************************/
BSP_U32 DRV_GET_ALL_TASK_ID_NAME(BSP_VOID *p_task_stru,u32 param_len)
{
      return sysview_get_all_task_name((void *)p_task_stru,param_len );
}
#endif

/*****************************************************************************
* �� �� ��  : DRV_INT_LOCK
*
* ��������  :���жϽӿڣ��������ж�ͳ��
*
* �������  :��
*
* �������  : ��
*
* �� �� ֵ  : ���жϺ��õ�key
*****************************************************************************/
BSP_S32 DRV_INT_LOCK(BSP_VOID)
{
    unsigned long flags;

    local_irq_save(flags);

    bsp_int_lock_enter();
    return (BSP_S32)flags;
}

/*****************************************************************************
* �� �� ��  : DRV_INT_UNLOCK
*
* ��������  : �����жϽӿڣ��������ж�ͳ��
*
* �������  : ���жϺ��õ�key
*
* �������  : ��
*
* �� �� ֵ  : ��
*****************************************************************************/
BSP_VOID DRV_INT_UNLOCK(BSP_S32 LockKey)
{
    unsigned long lock_key;

    lock_key = (unsigned long)LockKey;
    
    bsp_int_lock_out();

    local_irq_restore(lock_key);
}


/*****************************************************************************
* �� �� ��  : DRV_SOCP_CHAN_ENABLE
*
* ��������  : ����SOCPͨ��ʹ�ܲ���
*
* �������  : ��
*
*
* �������  :��
*
* �� �� ֵ  :��
*****************************************************************************/

BSP_VOID DRV_SOCP_CHAN_ENABLE(BSP_VOID)
{
    bsp_socp_chan_enable();
}


#endif

#ifdef __cplusplus
}
#endif


