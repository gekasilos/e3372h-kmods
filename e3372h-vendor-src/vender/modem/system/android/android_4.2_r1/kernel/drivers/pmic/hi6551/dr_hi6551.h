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
#include <pmu_balong.h>
#include <hi_smartstar.h>

#define DR12_CUR_BASE   3000 /*3mA==3000uA*/
#define DR12_CUR_STEP   1000 /*3mA==3000uA*/
#define DR345_CUR_BASE   1000 /*1mA==1000uA*/
#define DR345_CUR_STEP   500 /*0.5mA==500uA*/
#define NUM_OF_DR   5
#define NUM_OF_DR_CURRENT   8
typedef struct
{
    u16 bre_onoff_reg_addr;/*����ģʽ�¿��ؼĴ�����ַ*/
    u16 fla_onoff_reg_addr;/*��˸ģʽ�¿��ؼĴ�����ַ*/

    u16 mode_sel_reg_addr;/*ģʽѡ��Ĵ�����ַ���ж�����˸ģʽ���Ǻ���ģʽ*/
    u16 addr_reserved;

    u16 fla_mode_reg_addr;/*��˸ģʽѡ��Ĵ�����ַ���ж�����˸����������˸����*/
    u16 bre_mode_reg_addr;/*����ģʽѡ��Ĵ�����ַ���ж��Ǻ����������Ǻ�����˸*/

    u16 bre_onoff_time_reg_adrr;/*������������ʱ�����üĴ�����ַ*/
    u16 bre_risefall_time_reg_adrr;/*������������ʱ�����üĴ�����ַ*/

    u16 current_reg_addr;/*�������üĴ�����ַ*/
    u16 start_del_reg_addr;/*������ʱ���üĴ�����ַ*/

    u8 bre_onoff_bit_offset;/*����ģʽ�¿���ƫ��*/
    u8 fla_onoff_bit_offset;/*��˸ģʽ�¿���ƫ��*/
    u8 current_bit_offset;/*��������bitƫ��*/
    u8 current_bit_mask;/*��������bit mask*/

    u8 mode_sel_bit_offset;/*ģʽѡ��Ĵ���ƫ��*/
    u8 fla_mode_bit_offset;/*��˸ģʽѡ��Ĵ���ƫ��*/
    u8 bre_mode_bit_offset;/*����ģʽѡ��Ĵ���ƫ��*/
    u8 offset_reserved;

    u8 bre_on_time_offset;/*����ʱ������ƫ��*/
    u8 bre_rise_time_offset;/*����ʱ������ƫ��*/
    u8 size_of_onoff_time_list;/**/
    u8 size_of_risefall_time_list;/**/

    int* current_list;
    unsigned int* bre_onoff_time_list;
    unsigned int* bre_risefall_time_list;
}PMIC_HI6551_DRS_ATTR;

#define ALWAYS_ON_OFF    0xf/*�������߳����Ĵ�������ֵ*/
#define BRE_TIME_NOT_SURE    0xfffffe/*�������߳����Ĵ�������ֵ*/

#define DR_INVAILD_ADDR 0x200/*��ַ����Чֵ����Ч��ַ���Ϊ0x1AE*/
#define DR_INVAILD_DATA 0/*���ݵ���Чֵ*/
#define DR_INVAILD_OFFSET 0x9/*ƫ�Ƶ���Чֵ�����ƫ��Ϊ8*/

#define PMIC_HI6551_DR_MIN PMIC_HI6551_DR01
#define PMIC_HI6551_DR_MAX PMIC_HI6551_DR05

#define bre_on_offset  4
#define bre_fall_offset  4

#define DR345_START_DELAY_MAX   32768
#define DR345_START_DELAY_MIN   0
#define FLA_PERIOD_STEP     31250
#define FLA_ON_STEP     7813
/*��������*/
void  bsp_dr_reg_read( u16 addr, u8 *pValue);
int hi6551_dr_fla_time_set(DR_FLA_TIME* dr_fla_time_st);
int hi6551_dr_fla_time_get(DR_FLA_TIME* dr_fla_time_st);
int hi6551_dr_bre_time_set(dr_id_e dr_id, DR_BRE_TIME* dr_bre_time_st);
int hi6551_dr_bre_time_get(dr_id_e dr_id, DR_BRE_TIME* dr_bre_time_st);
int hi6551_dr_start_delay_set(dr_id_e dr_id, unsigned delay_ms);
unsigned hi6551_dr_bre_time_list(dr_id_e dr_id, dr_bre_time_e bre_time_enum ,unsigned selector);
unsigned hi6551_dr_bre_time_selectors_get(dr_id_e dr_id, dr_bre_time_e bre_time_enum);

