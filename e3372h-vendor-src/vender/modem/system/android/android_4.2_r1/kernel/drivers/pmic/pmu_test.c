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
#include "pmu_balong.h"

struct pmu_test g_pmu_test;

/*****************************************************************************
 �� �� ��  : bsp_pmu_test_enable
 ��������  : ��pmuģ��test����
 �������  : void
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  : �쳣��λ
*****************************************************************************/
void bsp_pmu_test_enable(void)
{
    g_pmu_test.test_sw = 1;
}
/*****************************************************************************
 �� �� ��  : bsp_pmu_test_disable
 ��������  :�ر�pmuģ��test����
 �������  : void
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  :�쳣��λ
*****************************************************************************/
void bsp_pmu_test_disable(void)
{
    g_pmu_test.test_sw = 0;
}

/*****************************************************************************
 �� �� ��  : bsp_pmu_test_register
 ��������  : pmuģ��test��Ϣע��
 �������  : void
 �������  : ��
 �� �� ֵ  : ע��ɹ���ʧ��
 ���ú���  :
 ��������  : �쳣��λ
*****************************************************************************/
int bsp_pmu_test_register(pmic_id_e pmic_id,struct pmu_test_ops ops_func)
{
    if(PMIC_BUTTOM <= pmic_id)
    {
        pmic_print_error("platform doesn't support this PMIC!\n");
        return  BSP_PMU_PARA_ERROR;
    }

    g_pmu_test.test_ops[pmic_id]= ops_func;

    return  BSP_PMU_OK;

}
/*****************************************************************************
 �� �� ��  : bsp_pmu_volt_onoff_test
 ��������  : ����pmuģ���ѹԴ���ؽӿ�
 �������  : uctimes:���Դ���
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  : �쳣��λ
*****************************************************************************/
int bsp_pmu_volt_onoff_test(u32 uctimes)
{
    int i = 0;
    int test_result = BSP_PMU_OK;

    pmic_print_info("*****bsp_pmu_volt_onoff_test*****\n");
    for(i = 0;i < PMIC_BUTTOM;i++)
    {
        if(NULL != g_pmu_test.test_ops[i].pmu_volt_onoff_test)
            test_result |= g_pmu_test.test_ops[i].pmu_volt_onoff_test(uctimes);
    }
    return test_result;
}
/*****************************************************************************
 �� �� ��  : bsp_pmu_volt_setget_test
 ��������  : ����pmuģ���ѹԴ���õ�ѹ�ӿ�
 �������  : uctimes:���Դ���
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  : �쳣��λ
*****************************************************************************/
int bsp_pmu_volt_setget_test(u32 uctimes)
{
    int i = 0;
    int test_result = BSP_PMU_OK;

    pmic_print_info("*****bsp_pmu_volt_setget_test*****\n");
    for(i = 0;i < PMIC_BUTTOM;i++)
    {
        if(NULL != g_pmu_test.test_ops[i].pmu_volt_setget_test)
            test_result |= g_pmu_test.test_ops[i].pmu_volt_setget_test(uctimes);
    }
    return test_result;
}
/*****************************************************************************
 �� �� ��  : bsp_pmu_volt_mode_test
 ��������  : ����pmuģ���ѹԴ����ģʽ�ӿ�
 �������  : uctimes:���Դ���
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  : �쳣��λ
*****************************************************************************/
int bsp_pmu_volt_mode_test(u32 uctimes)
{
    int i = 0;
    int test_result = BSP_PMU_OK;

    pmic_print_info("*****bsp_pmu_mode_setget_test*****\n");
    for(i = 0;i < PMIC_BUTTOM;i++)
    {
        if(NULL != g_pmu_test.test_ops[i].pmu_volt_mode_test)
            test_result |= g_pmu_test.test_ops[i].pmu_volt_mode_test(uctimes);
    }
    return test_result;
}
/*****************************************************************************
 �� �� ��  : bsp_pmu_mode_setget_test
 ��������  : ����pmuģ���ѹԴ��ѹ����
 �������  : uctimes:���Դ���
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  : �쳣��λ
*****************************************************************************/
int bsp_pmu_volt_table_test(u32 uctimes)
{
    int i = 0;
    int test_result = BSP_PMU_OK;

    pmic_print_info("*****bsp_pmu_volt_table_test*****\n");
    for(i = 0;i < PMIC_BUTTOM;i++)
    {
        if(NULL != g_pmu_test.test_ops[i].pmu_volt_table_test)
            test_result |= g_pmu_test.test_ops[i].pmu_volt_table_test(uctimes);
    }
    return test_result;
}
/*****************************************************************************
 �� �� ��  : bsp_pmu_test_init
 ��������  : ��ʼ��pmu testģ�飬����pmic���Ժ���ע�ᵽ���Կ��
 �������  : ��
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  : �쳣��λ
*****************************************************************************/
int bsp_pmu_test_init(void)
{
    int iret = BSP_PMU_OK;

#ifdef CONFIG_PMIC_HI6451
    iret = bsp_hi6451_test_init();
#endif
#ifdef CONFIG_PMIC_HI6551
    iret |= bsp_hi6551_test_init();
#endif

#ifdef CONFIG_PMIC_HI6559
    iret |= bsp_hi6559_test_init();
#endif

    if(iret)
        pmic_print_error("init error!\n");
    else
        pmic_print_info("init ok!\n");

    return iret;
}
/*pmu adp�ӿڵĲ���*/
#ifdef __KERNEL__
/*����Դ���ֽӿ�*/
/*****************************************************************************
 �� �� ��  : bsp_pmu_dr_onoff_test
 ��������  : ����pmuģ�����Դ���ؽӿ�
 �������  : uctimes:���Դ���
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  : �Զ�������
*****************************************************************************/
int bsp_pmu_dr_onoff_test(u32 uctimes)
{
    int i = 0;
    int test_result = BSP_PMU_OK;

    pmic_print_info("*****bsp_pmu_dr_onoff_test*****\n");
    for(i = 0;i < PMIC_BUTTOM;i++)
    {
        if(NULL != g_pmu_test.test_ops[i].pmu_dr_onoff_test)
            test_result |= g_pmu_test.test_ops[i].pmu_dr_onoff_test(uctimes);
    }
    return test_result;
}
/*****************************************************************************
 �� �� ��  : bsp_pmu_dr_setget_test
 ��������  : ����pmuģ�����Դ���õ����ӿ�
 �������  : uctimes:���Դ���
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  : �쳣��λ
*****************************************************************************/
int bsp_pmu_dr_setget_test(u32 uctimes)
{
    int i = 0;
    int test_result = BSP_PMU_OK;

    pmic_print_info("*****bsp_pmu_volt_setget_test*****\n");
    for(i = 0;i < PMIC_BUTTOM;i++)
    {
        if(NULL != g_pmu_test.test_ops[i].pmu_dr_setget_test)
            test_result |= g_pmu_test.test_ops[i].pmu_dr_setget_test(uctimes);
    }
    return test_result;
}
/*****************************************************************************
 �� �� ��  : bsp_pmu_dr_mode_test
 ��������  : ����pmuģ�����Դ����ģʽ�ӿ�
 �������  : uctimes:���Դ���
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  : �쳣��λ
*****************************************************************************/
int bsp_pmu_dr_mode_test(u32 uctimes)
{
    int i = 0;
    int test_result = BSP_PMU_OK;

    pmic_print_info("*****bsp_pmu_mode_setget_test*****\n");
    for(i = 0;i < PMIC_BUTTOM;i++)
    {
        if(NULL != g_pmu_test.test_ops[i].pmu_dr_mode_test)
            test_result |= g_pmu_test.test_ops[i].pmu_dr_mode_test(uctimes);
    }
    return test_result;
}
/*����Ƽ���ɫ�ƹ��ܲ�������*/
/*****************************************************************************
 �� �� ��  : bsp_dr_fla_time_set_test
 ��������  : ����drģ����˸ʱ�����ýӿ�
 �������  : uctimes:���Դ���
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  : ϵͳ����
*****************************************************************************/
int bsp_dr_fla_time_set_test(unsigned on_us,unsigned off_us)
{
    DR_FLA_TIME fla_time;
    int iret;

    fla_time.fla_on_us = on_us;
    fla_time.fla_off_us = off_us;

    iret = bsp_dr_fla_time_set(&fla_time);
    return  iret;
}
/*****************************************************************************
 �� �� ��  : bsp_dr_fla_time_set_test
 ��������  : ����drģ�����ʱ�����ýӿ�
 �������  : uctimes:���Դ���
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  : ϵͳ����
*****************************************************************************/
int bsp_dr_bre_time_set_test(dr_id_e dr_id,unsigned on_ms,unsigned off_ms,unsigned rise_ms,unsigned fall_ms)
{
    DR_BRE_TIME bre_time;
    int iret;

    bre_time.bre_on_ms = on_ms;
    bre_time.bre_off_ms = off_ms;
    bre_time.bre_rise_ms = rise_ms;
    bre_time.bre_fall_ms = fall_ms;

    iret = bsp_dr_bre_time_set(dr_id,&bre_time);
    return  iret;
}

/*��˸ģʽ*/
int bsp_dr_fla_test_001()
{
    return bsp_dr_fla_time_set_test(DR_VALUE_INVALIED,DR_VALUE_INVALIED);
}
#endif
