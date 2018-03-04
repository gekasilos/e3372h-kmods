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
#ifdef __KERNEL__
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/list.h>
#include <linux/errno.h>
#include <linux/mutex.h>
#include <linux/clk.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <drv_comm.h>
#include <asm/uaccess.h>
#include <linux/workqueue.h>
#include <pmu_balong.h>
#endif

#include <osl_bio.h>
#include <bsp_ipc.h>
#include <bsp_memmap.h>
#include <bsp_om.h>

#include <osl_thread.h>
#include <osl_sem.h>
#include <osl_spinlock.h>
#include "hi_pmu.h"
#include "irq_hi6451.h"
#include "common_hi6451.h"
#include "exc_hi6451.h"

/*----------------------------ȫ�ֱ���\������---------------------------------------------*/
typedef struct {
    int irq;
    char*   name;
    hi6451funcptr   handler;
    } hi6451_exc_irq_st;

/*exc om data:0\uvp,ovp,otp flag; 1 \usb,ocp flag; 2\rtc;3~5\ocp */
u8 pmu_hi6451_om_data[6];
u8 pmu_hi6451_om_boot[3];
struct hi6451_exc_data{
    spinlock_t      lock;
    struct workqueue_struct *hi6451_om_wq;
    struct delayed_work hi6451_om_wk;
};
struct hi6451_exc_data g_hi6451_exc_st;

void hi6451_om_log_save(pmu_om_log_e hi6451_om_log_flag)
{
    int i,j = 0;

    switch(hi6451_om_log_flag)
    {
        case PMU_OM_LOG_START:
            /*Ƿѹ����ѹ*/
            if(pmu_hi6451_om_boot[0] & (0x01 << HI6451_VINI_UNDER_2P7_OFFSET ))
                print2file(PMU_OM_LOG,"pmu_hi6451:vsys under 2.5v last time!\n");
            if(pmu_hi6451_om_boot[0] & (0x01 << HI6451_VINI_OVER_6P0_OFFSET ))
                print2file(PMU_OM_LOG,"pmu_hi6451:vsys over 6.0v last time!\n");
            /*����*/
            if(pmu_hi6451_om_boot[1] & (0x01 << HI6451_CORE_OVER_CUR_OFFSET ) )
                print2file(PMU_OM_LOG,"pmu_hi6451:ldo/buck overflow last time!\n");
            /*����*/
            if(pmu_hi6451_om_boot[2] & (0x01 << HI6451_OTMP_150_OFFSET ) )
                print2file(PMU_OM_LOG,"pmu_hi6451:temperature over 150�� last time!\n");
            break;
        case PMU_OM_LOG_EXC:
            break;
        default:
            break;
    }

}

/*****************************************************************************
 �� �� ��  : hi6451_get_boot_state
 ��������  :ϵͳ����ʱ���pmu�Ĵ���״̬��
                ȷ���Ƿ�����pmu���������
 �������  : void
 �������  : reset.log
 �� �� ֵ  : pmu�����ok
 ���ú���  :
 ��������  :ϵͳ��ά�ɲ�
*****************************************************************************/
int hi6451_get_boot_state(void)
{

    u8 a_ucRecordReg[3] = {0};
    u32 i = 0;

    /*��ȡ���µ�reg*/
    for(i = 0; i < 3; i++)
    {
        bsp_hi6451_reg_read((HI_PMU_NO_PWR_IRQ1_OFFSET+i), &a_ucRecordReg[i]);
    }

    /*Ƿѹ����ѹ*/
    if(a_ucRecordReg[0] & (0x01 << HI6451_VINI_UNDER_2P7_OFFSET ))
        return PMU_STATE_UNDER_VOL;
    if(a_ucRecordReg[0] & (0x01 << HI6451_VINI_OVER_6P0_OFFSET ))
        return PMU_STATE_OVER_VOL;
    /*����*/
    if(a_ucRecordReg[1] & (0x01 << HI6451_CORE_OVER_CUR_OFFSET ) )
        return PMU_STATE_OVER_CUR;
    /*����*/
    if(a_ucRecordReg[2] & (0x01 << HI6451_OTMP_150_OFFSET ) )
        return PMU_STATE_OVER_TEMP;

    return PMU_STATE_OK;
}

void hi6451_otp_threshold_set(int threshold)
{
    hi6451_irqflags_t flags;

    spin_lock_irqsave(&g_hi6451_exc_st.lock,flags);
    switch(threshold)
    {
        case 105:
            bsp_hi6451_reg_write_mask(HI_PMU_ONOFF7_OFFSET, 0x00,0x18);
            break;
        case 115:
            bsp_hi6451_reg_write_mask(HI_PMU_ONOFF7_OFFSET, 0x08,0x18);
                break;
        case 125:
            bsp_hi6451_reg_write_mask(HI_PMU_ONOFF7_OFFSET, 0x10,0x18);;
                break;
        case 135:
            bsp_hi6451_reg_write_mask(HI_PMU_ONOFF7_OFFSET, 0x18,0x18);;
                break;
        default:
                break;
            /*����������Ĭ��ֵ125�� */
    }
    spin_unlock_irqrestore(&g_hi6451_exc_st.lock,flags);
}

/*------------------------------����������------------------------------------------*/
void hi6451_ocp_handle(void* data)
{
    u8 a_ucRecordReg[8] = {0};
    u32 i = 0;
    u32 j = 0;

    pmu_hi6451_om_data[1] |= (0x01 << HI6451_OCP_OFFSET);

    /*read and clear ocp int*/
    for(i = 0; i < 3; i++)
    {
        bsp_hi6451_reg_read((HI_PMU_STATUS1_OFFSET+i), &a_ucRecordReg[i]);
        bsp_hi6451_reg_write((HI_PMU_STATUS1_OFFSET+i), a_ucRecordReg[i]);
        /*����*/
        pmu_hi6451_om_data[i + 3] |= a_ucRecordReg[i];
    }

    /* �������OCP_RECORD�е�״̬,��ȫ���� */
    for (i = 0; i < 3; i++)
    {
        for (j = 0; j < 8; j++)
        {
            if (a_ucRecordReg[i] & (0x1 << (7-j)))
            {
                if(i < 2)
                {
                    bsp_hi6451_volt_disable(i*8+j+1);/*buck1~ldo10*/
                    print2file(PMU_OM_LOG,"pmu_hi6451:Volt %d overflow !\n",(i*8+j+1));
                }
                else
                {
                    bsp_hi6451_volt_disable(i*8+j-2);/*ldo11~ldo17*/
                    print2file(PMU_OM_LOG,"pmu_hi6451:Volt %d overflow !\n",(i*8+j-2));
                }
            }
        }
    }

    pmic_print_error("\n**********hi6451_ocp_isr**********\n");
}
/*------------------------------���´�����------------------------------------------*/
void hi6451_otp_handle(void)
{
    int i = 0;
    /*not core,need off:BUCK4\BUCK5\LDO3\LDO5\LDO7\LDO10\LDO11\LDO13\LDO14\LDO15*/
    int otp_need_off = 0x1d9518;
    pmu_hi6451_om_data[0] |= (0x01 << HI6451_OTP_OFFSET);
    /*�رշǺ��ĵ�Դ,�������Ӳ�Ʒ��̬������⴦��*/
    for(i = PMIC_HI6451_VOLT_MIN;i < PMIC_HI6451_VOLT_MAX;i++)
    {
        if(otp_need_off & (0x01 << i))
        {
            bsp_hi6451_volt_disable(i+1);
        }
    }

    /*��¼om log*/
    print2file(PMU_OM_LOG,"pmu_hi6451:temperature overflow (>135/125/115/105��)!\n");

    pmic_print_error("\n**********hi6451_otp_isr**********\n");
}
/*------------------------------��ѹ������------------------------------------------*/
void hi6451_ovp_handle(void)
{
    u8 reg_temp = 0;
    hi6451_irqflags_t flags;

    pmu_hi6451_om_data[0] |= (0x01 << HI6451_OVP_OFFSET);

    /*��¼om log*/
    print2file(PMU_OM_LOG,"pmu_hi6451:vsys over 6.0v !\n");

    pmic_print_error("\n**********hi6451_ovp_isr**********\n");
}

/*------------------------------Ƿѹ������2.5------------------------------------------*/
void hi6451_uvp_handle(void)
{
    u8 reg_temp = 0;
    hi6451_irqflags_t flags;

    pmu_hi6451_om_data[0] |= (0x01 << HI6451_UVP_OFFSET);

    /*��¼om log*/
    print2file(PMU_OM_LOG,"pmu_hi6451:vsys under 2.5v !\n");

    pmic_print_error("\n**********hi6451_uvp_2p5_isr**********\n");
}
/*test,usb in and out*/
void hi6451_usb_out_handle(void)
{
    pmic_print_error("\n**********hi6451_usb_out_handle**********\n");
}
void hi6451_usb_in_handle(void)
{
    pmic_print_error("\n**********hi6451_usb_in_handle**********\n");
}

/*-------------------------------------------------------------------------*/
void bsp_hi6451_exc_init(void)
{
    int ret;
    int i;
    hi6451_excflags_t flags;
    hi6451_exc_irq_st HI6451_EXC_IRQ[4] =
    {
        {IRQ_HI6451_OCP,"HI6451_OCP",hi6451_ocp_handle},
        {IRQ_HI6451_OTMP,"HI6451_OTMP",hi6451_otp_handle},
        {IRQ_HI6451_USB_UNDER_4P0,"HI6451_USB_OUT",hi6451_usb_out_handle},
        {IRQ_HI6451_USB_OVER_4P0,"HI6451_USB_IN",hi6451_usb_in_handle},
    };

    spin_lock_init(&g_hi6451_exc_st.lock);
    /*������µ�reg*/
    for(i = 0; i < 3; i++)
    {
        spin_lock_irqsave(&g_hi6451_exc_st.lock,flags);
        bsp_hi6451_reg_read((HI_PMU_NO_PWR_IRQ1_OFFSET+i), &pmu_hi6451_om_boot[i]);
        bsp_hi6451_reg_write((HI_PMU_NO_PWR_IRQ1_OFFSET+i), &pmu_hi6451_om_boot[i]);
        spin_unlock_irqrestore(&g_hi6451_exc_st.lock,flags);
    }
    /*write start flag to pmu_om_log */
    hi6451_om_log_save(PMU_OM_LOG_START);

    for(i = 0;i < ARRAY_SIZE(HI6451_EXC_IRQ);i++)
    {
        ret = hi6451_irq_callback_register(HI6451_EXC_IRQ[i].irq, HI6451_EXC_IRQ[i].handler, NULL);
        if (ret)
        {
            bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_PMU,"register exc %d interrupt failed!\n",HI6451_EXC_IRQ[i]);
        }
    }

    bsp_trace(BSP_LOG_LEVEL_INFO, BSP_MODU_PMU,"bsp_hi6451_exc_init,register exc interrupt ok!\n");
}

#ifdef __KERNEL__
late_initcall(bsp_hi6451_exc_init);
static void __exit bsp_hi6451_exc_exit(void)
{
}
module_exit(bsp_hi6451_exc_exit);
#endif
