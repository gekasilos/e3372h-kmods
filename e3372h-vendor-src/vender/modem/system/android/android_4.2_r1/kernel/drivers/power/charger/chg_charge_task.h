/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2013-2015. All rights reserved.
 *
 * mobile@huawei.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#ifndef CHG_CHARGE_TASK_H
#define CHG_CHARGE_TASK_H
/*----------------------------------------------*
 * ����ͷ�ļ�                                   *
 *----------------------------------------------*/

/*----------------------------------------------*
 * �ڲ�����ԭ��˵��                             *
 *----------------------------------------------*/
/**********************************************************************
�� �� ��: chg_main_task
��������: ��һ���������
�������: None
�������: None
�� �� ֵ: None
ע������: ������Linux��Vxworks��ʵ��
          ��Ϊ��ϵͳ����,���ע�������Ӧ�Ŀ��ļ�
***********************************************************************/
int32_t  chg_main_task( void  *task_para );

/**********************************************************************
�� �� ��      : chg_task_init
��������  :  ������ʼ��,��������е�charger�Ļ�������
                            ����������񼰶�ʱ��
�������  : �ޡ�
�������  : �ޡ�
�� �� ֵ      : �ޡ�
ע������  : �ޡ�
***********************************************************************/
extern void chg_task_init(void);
#endif/*CHG_CHARGE_TASK_H*/
