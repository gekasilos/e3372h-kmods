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
#ifndef __F_MASS_STORAGE_API_H__
#define __F_MASS_STORAGE_API_H__
#include "usb_debug.h"
#include "android.h"

/*****************************************************************************
 �� �� ��  :usb_mass_storage_init
 ��������  :mass storage FD ��ʼ����������PNPģ�����
 �������  :NA
 �������  :
 �� �� ֵ  :USB_INT:
 ���ú���  :
 ��������  :init()
 �޸���ʷ      :
  1.��    ��   : 2014��6��10��
    �޸�����   : ���ע��
*****************************************************************************/
USB_VOID usb_mass_storage_init(USB_VOID);
/*****************************************************************************
 �� �� ��  :usb_mass_storage_init
 ��������  :mass storage �˿�����lun
 �������  :NA
 �������  :
 �� �� ֵ  :USB_INT:
 ���ú���  :
 ��������  :
 �޸���ʷ      :
  1.��    ��   : 2014��6��10��
    �޸�����   : ���ע��
*****************************************************************************/
USB_VOID mass_function_add_lun(USB_CHAR* function_name, USB_CHAR* addname);
/*****************************************************************************
 �� �� ��  :usb_mass_storage_init
 ��������  :����SD������ģʽ
 �������  :NA
 �������  :
 �� �� ֵ  :USB_INT:
 ���ú���  :
 ��������  :
 �޸���ʷ      :
  1.��    ��   : 2014��6��10��
    �޸�����   : ���ע��
*****************************************************************************/
USB_VOID mass_storage_set_sd_card_workmode(USB_INT mode);
/*****************************************************************************
 �� �� ��  :usb_mass_storage_init
 ��������  :USB��SD��
 �������  :NA
 �������  :
 �� �� ֵ  :USB_INT:
 ���ú���  :
 ��������  :
 �޸���ʷ      :
  1.��    ��   : 2014��6��10��
    �޸�����   : ���ע��
*****************************************************************************/
USB_INT  mass_storage_open_usb_sd(USB_VOID);

/*****************************************************************************
 �� �� ��  :mass_storage_close_usb_sd
 ��������  :USB�ر�SD��
 �������  :NA
 �������  :
 �� �� ֵ  :USB_INT:
 ���ú���  :
 ��������  :
 �޸���ʷ      :
  1.��    ��   : 2014��6��10��
    �޸�����   : ���ע��
*****************************************************************************/
USB_INT mass_storage_close_usb_sd(USB_VOID);

/*****************************************************************************
 �� �� ��  :usb_mass_storage_init
 ��������  :��ѯ�Ƿ�USB��SD��
 �������  :NA
 �������  :
 �� �� ֵ  :USB_INT:
 ���ú���  :
 ��������  :
 �޸���ʷ      :
  1.��    ��   : 2014��6��10��
    �޸�����   : ���ע��
*****************************************************************************/
USB_INT mass_storage_usb_sd_is_open(USB_VOID);

/*****************************************************************************
 �� �� ��  :mass_storage_set_sd_card_status
 ��������  :����SD��������߰γ���״̬
 �������  :NA
 �������  :
 �� �� ֵ  :USB_VOID:
 ���ú���  :
 ��������  :
 �޸���ʷ      :
  1.��    ��   : 2014��6��10��
    �޸�����   : ���ע��
*****************************************************************************/
USB_VOID mass_storage_set_sd_card_status(USB_INT sd_removed);

/*****************************************************************************
 �� �� ��  :mass_function_cleanup_lun_info
 ��������  :���lun��Ϣ
 �������  :NA
 �������  :
 �� �� ֵ  :USB_INT:
 ���ú���  :
 ��������  :
 �޸���ʷ      :
  1.��    ��   : 2014��6��10��
    �޸�����   : ���ע��
*****************************************************************************/
USB_VOID mass_function_cleanup_lun_info(USB_CHAR* function_name);

/*****************************************************************************
 �� �� ��  :mass_function_cleanup_alllun_info
 ��������  :���lun info
 �� �� ֵ  :USB_INT:
 �޸���ʷ      :
  1.��    ��   : 2014��6��10��
    �޸�����   : ���ע��
*****************************************************************************/
USB_VOID mass_function_cleanup_alllun_info(USB_VOID);

#endif
