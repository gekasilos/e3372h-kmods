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
/*lint -save -e537*/
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/gpio.h>
#include <linux/module.h>
#include <linux/clk.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/debugfs.h>
#include <linux/seq_file.h>
#include <linux/regulator/consumer.h>

#include "product_config.h"
#include "osl_bio.h"
#include "osl_sem.h"
#include "osl_thread.h"
#include "osl_wait.h"
#include "osl_spinlock.h"
#include "soc_clk.h"
#include "bsp_memmap.h"
#include "hi_base.h"
#include "hi_efuse.h"

#include "bsp_clk.h"
#include "bsp_icc.h"
#include "bsp_hardtimer.h"
#include "bsp_om.h"
#include "bsp_efuse.h"


/*lint -restore*/

#define TIMEMAXDELAY   (0x1000000)       /*����ӳ�ʱ��*/

static struct spinlock efuse_lock;

typedef unsigned int (*punptr)(void);
/*********************************************************************************************  
Function:           test_bit_value
Description:        ���ԼĴ����е�ĳλ���Ƿ����ָ����ֵ
Calls:              ��
Data Accessed:      ��
Data Updated:       ��
Input:              addr��  �Ĵ�����ַ
                    bitMask����Ҫ����λ��
                    bitValue��ָ����ֵ
Output:             ��
Return:             �ɹ��򷵻�TRUE�� ʧ���򷵻�FALSE
Others:             ��
*********************************************************************************************/
int test_bit_value(punptr pfun,  u32 bitValue)
{
    u32 time;                       /*��ʱʱ��*/
    
    for(time = 0; time < TIMEMAXDELAY; time++)
    {
        if(bitValue == (*pfun)())
        {
            return EFUSE_OK;
        }
    }
    
    return EFUSE_ERROR;
}

/*lint -save -e718 -e746*/
/*****************************************************************************
* �� �� ��  : bsp_efuse_read
*
* ��������  : �����ȡEFUSE�е�����
*
* �������  : group  ��ʼgroup
*                   num  ���鳤��(word��,������16)
* �������  : pBuf ��EFUSE�е�����
*
* �� �� ֵ  : 
*
* ����˵��  : 
*
*****************************************************************************/
int bsp_efuse_read(u32* pBuf, const u32 group, const u32 num)
{
    u32* pSh = 0;
    u32 cnt = 0;
    
    unsigned long flags = 0;
    struct clk *efuse_clk = NULL;
       
    if((0 == num) || (group + num > EFUSE_MAX_SIZE) || (NULL == pBuf))
    {
        efuse_print_error(" parameter error, group=%d, num=%d, pBuf=0x%x.\n", group, num, pBuf);        
        return EFUSE_ERROR;
    }
 
    efuse_clk = (struct clk *)clk_get(NULL,"efuse_clk");
	if(IS_ERR(efuse_clk))
    {
		efuse_print_error("efuse clk cannot get, 0x%x.\n", efuse_clk);
		return EFUSE_ERROR;
	}

    spin_lock_irqsave(&efuse_lock, flags);
    
    if(0 != clk_enable(efuse_clk))
    {
		efuse_print_error("efuse clk enable is fail, 0x%x.\n", efuse_clk);
		goto clk_enable_error;
    }
    
    /*�ж��Ƿ���power_down״̬*/
    if(get_hi_efusec_status_pd_status())
    {
       /*�˳�powerd_down״̬*/
        set_hi_efusec_cfg_pd_en(0);
       
        /* �ȴ�powerd_down�˳���־���óɹ�������ʱ���ش��� */
        if(EFUSE_OK != test_bit_value(get_hi_efusec_status_pd_status, 0))
        {
            efuse_print_error(" powerd_down disable is fail .\n");
            goto efuse_rd_error;
        }
    }

#ifdef HI_RF_STATUS_OFFSET
    /*����efuse�ӿ�ʱ��Ϊ�ڲ�����*/
    set_hi_efusec_cfg_signal_sel(1);
#else
    /*ѡ���ź�Ϊapb����efuse*/
    set_hi_efusec_cfg_aib_sel(1);
#endif

    pSh = pBuf;
    
    /* ѭ����ȡefuseֵ */
    for(cnt = 0; cnt < num; cnt++)
    {
        /* ���ö�ȡ��ַ */
        set_hi_efuse_group_efuse_group( group+cnt);
        
        /* ʹ�ܶ� */
        set_hi_efusec_cfg_rden(1);
        
        /* �ȴ������ */
        if(EFUSE_OK != test_bit_value(get_hi_efusec_status_rd_status, 1))
        {
            efuse_print_error(" read finish is fail .\n");
            goto efuse_rd_error;
        }
        /* ��ȡ���� */
        *pSh = get_hi_efusec_data_efusec_data();
        pSh++;
        
    }
    
    clk_disable(efuse_clk);
    
    spin_unlock_irqrestore(&efuse_lock, flags);
    
    return EFUSE_OK;

efuse_rd_error:
    clk_disable(efuse_clk);
    
clk_enable_error:
    spin_unlock_irqrestore(&efuse_lock, flags);
    clk_put(efuse_clk);

    return EFUSE_ERROR;
}    
/*lint -restore*/

/*****************************************************************************
* �� �� ��  : bsp_efuse_write
*
* ��������  : ��дEfsue
*
* �������  : pBuf:����д��EFUSEֵ
*                 group,Efuse��ַƫ��
*                 len,��д����
* �������  :
*
* �� �� ֵ  :
*
* ����˵��  :
*
*****************************************************************************/
int bsp_efuse_write( u32 *pBuf, const u32 group, const u32 len )
{
    u32 i = 0;
    u32 u32Grp = 0;
    u32 *pu32Value = 0;

    unsigned long flags = 0;
    struct clk *efuse_clk = NULL;
    struct regulator *efuse_regulator = NULL;
    
    if((0 == len) || (group + len > EFUSE_MAX_SIZE) || (NULL == pBuf))
    {
        efuse_print_error(" parameter error, group=%d, len=%d, pBuf=0x%x.\n", group, len, pBuf);
        return EFUSE_ERROR;
    }

    spin_lock_irqsave(&efuse_lock, flags);

#ifndef HI_RF_STATUS_OFFSET
    efuse_regulator = regulator_get(NULL, "EFUSE-vcc");
    if(NULL == efuse_regulator)
    {
        efuse_print_error("efuse regulator cannot get. \n");        
        goto regu_get_error;
    }

    if(0 != regulator_enable(efuse_regulator))
    {
        efuse_print_error("efuse regulator enable is fail. \n");
        goto regulator_error;
    }
    
#endif

    efuse_clk = (struct clk *)clk_get(NULL,"efuse_clk");
	if(IS_ERR(efuse_clk))
    {
		efuse_print_error("efuse clk cannot get, 0x%x.\n", efuse_clk);
		goto clk_get_error;
	}
    
    if(0 != clk_enable(efuse_clk))
    {
		efuse_print_error("efuse clk enable is fail, 0x%x.\n", efuse_clk);
		goto clk_enable_error;
    }
    
    udelay(1000);/*lint !e737*/
    
    /*�ж��Ƿ���power_down״̬*/
    if(get_hi_efusec_status_pd_status())
    {
       /*�˳�powerd_down״̬*/
        set_hi_efusec_cfg_pd_en(0);
        /* �ȴ�powerd_down�˳���־���óɹ�������ʱ���ش��� */
        if(EFUSE_OK != test_bit_value(get_hi_efusec_status_pd_status, 0))
        {
            efuse_print_error(" power_down disable is fail .\n");
            goto efuse_wt_error;
        }
    }
    
    /* �ж��Ƿ�������д,Ϊ1��ֹ��д,ֱ���˳� */
    if(hi_efuse_get_pgm_disable_flag())
    {
        efuse_print_error(" burn write is disable .\n");
        goto efuse_wt_error;
    }

#ifdef HI_RF_STATUS_OFFSET
    /*����efuse�ӿ�ʱ��Ϊ�ڲ�����*/
    set_hi_efusec_cfg_signal_sel(1);
#else
    /*ѡ���ź�Ϊapb����efuse*/
    set_hi_efusec_cfg_aib_sel(1);
#endif

    /* ����ʱ��Ҫ�� */
    set_hi_efusec_count_efusec_count(EFUSE_COUNT_CFG);
    set_hi_pgm_count_pgm_count(PGM_COUNT_CFG);
    
    /* ʹ��Ԥ��д */
    set_hi_efusec_cfg_pre_pg(1);
    
    /*��ѯ�Ƿ�ʹ����λ */
    if(EFUSE_OK != test_bit_value(get_hi_efusec_status_pgenb_status, 1))
    {
        efuse_print_error(" pre write enable is fail .\n");
        goto efuse_wt_error;
    }
    
    /* ѭ����д */
    u32Grp = group;
    pu32Value = pBuf;
    for (i = 0; i < len; i++)
    {
        /* ����group */
        set_hi_efuse_group_efuse_group(u32Grp);
        
        /* ����value */
        set_hi_pg_value_pg_value(*pu32Value);
        
        /* ʹ����д */
        set_hi_efusec_cfg_pgen(1);
        
        /* ��ѯ��д��� */
        if(EFUSE_OK != test_bit_value(get_hi_efusec_status_pg_status, 1))
        {
            efuse_print_error(" burn write finish is fail .\n");
            goto efuse_wt_error;
        }
        
        /* ��д��һ�� */
        u32Grp++;
        pu32Value++;
    }

    /*ȥʹ��Ԥ��д*/
    set_hi_efusec_cfg_pre_pg(0);
    
    clk_disable(efuse_clk);

#ifndef HI_RF_STATUS_OFFSET
    regulator_put(efuse_regulator);
#endif

    spin_unlock_irqrestore(&efuse_lock, flags);
     
    return EFUSE_OK;

efuse_wt_error:
    clk_disable(efuse_clk);
    
clk_enable_error:
    clk_put(efuse_clk);
    
clk_get_error:
    (void)regulator_disable(efuse_regulator);

regulator_error:
    regulator_put(efuse_regulator);
regu_get_error:
    spin_unlock_irqrestore(&efuse_lock, flags);
    
    return EFUSE_ERROR;
}


static int __init hi_efuse_init(void)
{    
    spin_lock_init(&efuse_lock);    
    
    efuse_print_info("efuse init ok.\n");
    
    return EFUSE_OK;
    
}

static void __exit hi_efuse_exit(void)
{
    /* do nothing */
}

module_init(hi_efuse_init);
module_exit(hi_efuse_exit);
MODULE_AUTHOR("Hisilicon Drive Group");
MODULE_DESCRIPTION("anten driver for the Hisilicon anten plug in/out");
MODULE_LICENSE("GPL");


#ifdef __cplusplus
}
#endif

