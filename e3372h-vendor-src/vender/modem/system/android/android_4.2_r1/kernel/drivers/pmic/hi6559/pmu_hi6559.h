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
#ifndef __PMU_HI6559_H
#define __PMU_HI6559_H
#ifdef __cplusplus
extern "C"
{
#endif

#if defined(__VXWORKS__)

#include <drv_dpm.h>

/*****************************************************************************
 �� �� ��  : bsp_pmu_hi6559_pa_rf_init
 ��������  : pa/rf���粿�ֳ�ʼ��
 �������  : ��
 �������  : ��
 �� �� ֵ  : 0:�ɹ���else - ʧ�ܡ�ʧ��ʱ����ֵ��bit0~bit4��Ӧ��ȡʧ�ܵ�regulator_pmu id
*****************************************************************************/
s32 bsp_pmu_hi6559_pa_rf_init(void);

/*****************************************************************************
 �� �� ��  : bsp_pmu_hi6559_rf_poweroff
 ��������  : ��rf�µ�
 �������  : @modem_id - ����or����(6559ֻ֧�ֵ������˲�������)
 �������  : ��
 �� �� ֵ  : 0 - �ɹ�����0 - ʧ��
*****************************************************************************/
s32 bsp_pmu_hi6559_rf_poweroff(PWC_COMM_MODEM_E modem_id);

/*****************************************************************************
 �� �� ��  : bsp_pmu_hi6559_rf_poweron
 ��������  : ��rf�ϵ�
 �������  : @modem_id - ����or����(6559ֻ֧�ֵ������˲�������)
 �������  : ��
 �� �� ֵ  : 0 - �ɹ�����0 - ʧ��
*****************************************************************************/
s32 bsp_pmu_hi6559_rf_poweron(PWC_COMM_MODEM_E modem_id);

/*****************************************************************************
 �� �� ��  : bsp_pmu_hi6559_get_rf_powerstatus
 ��������  : ��ȡrf���µ�״̬
 �������  : @modem_id - ����or����(6559ֻ֧�ֵ������˲�������)
 �������  : ��
 �� �� ֵ  : 0 - �ɹ�����0 - ʧ��
*****************************************************************************/
PWC_COMM_STATUS_E bsp_pmu_hi6559_get_rf_powerstatus(PWC_COMM_MODEM_E modem_id);

/*****************************************************************************
 �� �� ��  : bsp_pmu_hi6559_pa_poweroff
 ��������  : ��pa�µ�
 �������  : @modem_id - ����or����(6559ֻ֧�ֵ������˲�������)
 �������  : ��
 �� �� ֵ  : 0 - �ɹ�����0 - ʧ��
*****************************************************************************/
s32 bsp_pmu_hi6559_pa_poweroff(PWC_COMM_MODEM_E modem_id);

/*****************************************************************************
 �� �� ��  : bsp_pmu_hi6559_pa_poweron
 ��������  : ��pa�ϵ�
 �������  : @modem_id - ����or����(6559ֻ֧�ֵ������˲�������)
 �������  : ��
 �� �� ֵ  : 0 - �ɹ�����0 - ʧ��
*****************************************************************************/
s32 bsp_pmu_hi6559_pa_poweron(PWC_COMM_MODEM_E modem_id);

/*****************************************************************************
 �� �� ��  : bsp_pmu_hi6559_get_pa_powerstatus
 ��������  : ��ȡpa���µ�״̬
 �������  : @modem_id - ����or����(6559ֻ֧�ֵ������˲�������)
 �������  : ��
 �� �� ֵ  : 0 - �ɹ�����0 - ʧ��
*****************************************************************************/
PWC_COMM_STATUS_E bsp_pmu_hi6559_get_pa_powerstatus(PWC_COMM_MODEM_E modem_id);

/*****************************************************************************
 �� �� ��  : bsp_hi6559_parf_reg_save
 ��������  : ������˯ʱ����Ĵ���
 �������  : ��
 �������  : ��
 �� �� ֵ  : 0 - �ɹ�����0 - ʧ��
*****************************************************************************/
u32 bsp_hi6559_parf_reg_save(void);

/*****************************************************************************
 �� �� ��  : bsp_hi6559_parf_reg_resume
 ��������  : �˳���˯ʱ�ָ��Ĵ���
 �������  : ��
 �������  : ��
 �� �� ֵ  : 0 - �ɹ�����0 - ʧ��
*****************************************************************************/
u32 bsp_hi6559_parf_reg_resume(void);

#endif /* #if defined(__VXWORKS__) */
#ifdef __cplusplus
}
#endif

#endif /* #ifndef __PMU_HI6559_H */

