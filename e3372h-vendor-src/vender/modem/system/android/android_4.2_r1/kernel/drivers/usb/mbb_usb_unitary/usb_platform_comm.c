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
#include "usb_platform_comm.h"
#ifdef MBB_USB_UNITARY_B  
#include "bsp_nvim.h"
#include "drv_nvim.h"
/* ����ƽ̨�ӿ�*/
//extern USB_ULONG NVM_Read(USB_ULONG  ulid, USB_PVOID pdata, USB_UINT16 usdatalen);

#endif


/*****************************************************************************
* �� �� ��  : usb_read_nv
*
* ��������  : A�˶�NV�ӿں���
*
* �������  : usID -> NV ID
              *pItem -> Save NV data buffer
              ulLength -> buffer length
* �������  : ��
*
* �� �� ֵ  : 0:    �����ɹ�
*                       -1: ����ʧ��
*
* ����˵��  : ��
*
*****************************************************************************/
USB_INT usb_read_nv(USB_UINT16 usID, USB_PVOID pItem, USB_UINT ulLength)
{
    /*����ƽ̨ */
#ifdef MBB_USB_UNITARY_B    
    return NVM_Read(usID, pItem, ulLength);
#else
    /*��ͨƽ̨��֧���ں˶�ȡNV*/
    return -1;
#endif
}
