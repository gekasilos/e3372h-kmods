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
#include <osl_types.h>
#include "drv_chg.h"
/*****************************************************************************
 �� �� ��  : BSP_CHG_ChargingStatus
 ��������  :��ѯĿǰ�Ƿ����ڳ����
 �������  :��
 �������  :��
 ����ֵ��   0 δ���
                          1 �����
*****************************************************************************/
int DRV_CHG_GET_CHARGING_STATUS(void)
{
    return -1;
}

/*****************************************************************************
 �� �� ��  : BSP_CHG_StateGet
 ��������  :chg_state_set(UINT32 ulstate)��A�˵�ӳ�亯��,��ѯ�Ƿ���Ҫ����
 �������  :
 �������  :
 ����ֵ��
*****************************************************************************/
int BSP_CHG_StateGet(void)
{
    return -1;
}
/*****************************************************************************
 �� �� ��  : BSP_CHG_GetCbcState
 ��������  : ���ص��״̬�͵���
 �������  : pusBcs 0:����ڹ��� 1:�������ӵ��δ���� 2:û����������
             pucBcl  0:���������û����������
 �������  : pusBcs 0:����ڹ��� 1:�������ӵ��δ���� 2:û����������
             pucBcl  0:���������û����������
 ����ֵ��    0 �����ɹ�
             -1����ʧ��
*****************************************************************************/
int DRV_CHG_GET_CBC_STATE(unsigned char *pusBcs,unsigned char *pucBcl)
{
    return -1;
}


/*****************************************************************************
 �� �� ��  : DRV_CHG_GET_BATTERY_STATE
 ��������  : ��ȡ�ײ���״̬��Ϣ
 �������  : battery_state ������Ϣ
 �������  : battery_state ������Ϣ
 ����ֵ��    0 �����ɹ�
             -1����ʧ��
*****************************************************************************/
int DRV_CHG_GET_BATTERY_STATE(BATT_STATE_T *bttery_state)
{
    return -1;
}

/*****************************************************************************
 �� �� ��  : BSP_CHG_Sply
 ��������  :chg_state_set(UINT32 ulstate)��A�˵�ӳ�亯��,��ѯ�Ƿ���Ҫ����
 �������  :
 �������  :
 ����ֵ��
*****************************************************************************/
int BSP_CHG_Sply( void)
{
    return BSP_ERROR;
}

/*****************************************************************************
 �� �� ��  : DRV_CHG_STATE_SET
 ��������  : ʹ�ܻ��߽�ֹ���
 �������  : ulState    0:��ֹ���
 						1:ʹ�ܳ��
 �������  : ��
 ����ֵ��    ��
*****************************************************************************/
void  DRV_CHG_STATE_SET(unsigned long ulState)
{

}
