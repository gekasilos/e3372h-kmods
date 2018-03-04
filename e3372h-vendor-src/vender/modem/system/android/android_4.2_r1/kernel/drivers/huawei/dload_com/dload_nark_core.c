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

#include <linux/ioctl.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/capability.h>
#include <linux/uaccess.h>
#include <linux/compat.h>
#include <asm/uaccess.h>
#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include "dload_nark_api.h"
#include "dload_nark_core.h"

#ifdef __cplusplus
extern "C" {
#endif
/******************************************************************************
  1 ģ��˽�� (�ꡢö�١��ṹ�塢�Զ�����������) ������:
******************************************************************************/
#define DLOAD_PRINT(level, fmt, ...) (printk(level"[*DLOAD_NARK_CORE*] %s:%d: "fmt"\n", __func__, __LINE__, ##__VA_ARGS__))
#define DLOAD_IOC_TYPE   'D'

typedef struct _IOCRL_MSG
{
    dload_module_t       module_index;  /* ������ģ��ID */
    uint32               msg_index;     /* ��ģ����ϢID */
    uint32               data_len;      /* ���ݻ��泤�� */
    void*                data_buffer;   /* ���ݻ���ָ�� */
} ioctl_msg_t;

enum
{
    DLOAD_SET_CMD  = _IOW(DLOAD_IOC_TYPE, 0, ioctl_msg_t),  /* �������� */
    DLOAD_GET_CMD  = _IOR(DLOAD_IOC_TYPE, 1, ioctl_msg_t),  /* ��ȡ���� */
};

/******************************************************************************
  2 ģ��˽�� (ȫ�ֱ���) ������:
******************************************************************************/
struct mutex ioctl_mutex;

/*******************************************************************
  ��������  : map_search
  ��������  : ����������Ϣ��Ϣ����ӳ�亯��
  �������  : ioctl_msg : ������Ϣ
  �������  : NV
  ��������ֵ: msg_func_t: ӳ�亯��
********************************************************************/
STATIC msg_func_t map_search(ioctl_msg_t*  ioctl_msg)
{
    uint32 msg_map_index = 0;
    msg_func_t  msg_func;

    /* ����ӳ������ */
    msg_map_index  = MAP_INDEX(ioctl_msg->module_index, ioctl_msg->msg_index);
    
    /* ������Ϣ���� */
    msg_func = search_map_callback(msg_map_index);
    if(NULL == msg_func)
    {
        DLOAD_PRINT(KERN_ERR, "msg_map_index = %lu : map callback is null.", msg_map_index);
        return NULL;
    }
    
    return msg_func;
}

/*******************************************************************
  ��������  : nark_get_data
  ��������  : ��ȡ������Ϣ�ɷ�
  �������  : ioctl_msg : ������Ϣ
  �������  : NA
  ��������ֵ: DLOAD_OK    : �ɹ�
              DLOAD_ERR : ʧ��
********************************************************************/
STATIC int32 nark_get_data(ioctl_msg_t*  ioctl_msg)
{
    int32           ret  = DLOAD_OK;
    int32           dret = -1;
    uint32          local_len = 0;
    int8*           local_buffer = NULL;
    msg_func_t      msg_func;

    msg_func = map_search(ioctl_msg);
    if(NULL == msg_func)
    {
        return DLOAD_ERR;
    }
    
    if(0 != ioctl_msg->data_len)
    {
        local_buffer = (char *)kzalloc(ioctl_msg->data_len, GFP_KERNEL);
        if(NULL == local_buffer)
        {
            DLOAD_PRINT(KERN_ERR, "kzalloc failed.");
            return DLOAD_ERR;
        }
        else
        {
            dret = copy_from_user(local_buffer, ioctl_msg->data_buffer, ioctl_msg->data_len);
            if(0 > dret)
            {
                if(NULL != local_buffer)
                {
                    kfree(local_buffer);
                }
                
                DLOAD_PRINT(KERN_ERR, "copy_from_user ERROR.");
                return DLOAD_ERR;
            }
        }
		
        local_len = ioctl_msg->data_len;
    }

    ret = msg_func((void *)local_buffer, local_len);
    
    if(NULL != local_buffer)
    {
        dret = copy_to_user(ioctl_msg->data_buffer, local_buffer, local_len);
        if(0 > dret)
        {
            DLOAD_PRINT(KERN_ERR, "copy_to_user ERROR");
            ret = DLOAD_ERR;
        }
    }

    if(NULL != local_buffer)
    {
        /* �ͷ��ڴ� */
        kfree(local_buffer);
    }
        
    return ret;
}

/*******************************************************************
  ��������  : nark_set_data
  ��������  : ����������Ϣ�ɷ�
  �������  : ioctl_msg : ������Ϣ
  �������  : NA
  ��������ֵ: DLOAD_OK    : �ɹ�
              DLOAD_ERR : ʧ��
********************************************************************/
STATIC int32 nark_set_data(ioctl_msg_t*  ioctl_msg)
{
    int32           ret  = DLOAD_OK;
    int32           dret = -1;
    uint32          local_len = 0;
    int8*           local_buffer = NULL;
    msg_func_t      msg_func;

    msg_func = map_search(ioctl_msg);
    if(NULL == msg_func)
    {
        return DLOAD_ERR;
    }
    
    if(0 != ioctl_msg->data_len)
    {
        local_buffer = (char *)kzalloc(ioctl_msg->data_len, GFP_KERNEL);
        if(NULL == local_buffer)
        {
            DLOAD_PRINT(KERN_ERR, "kzalloc failed.");
            return DLOAD_ERR;
        }
        else
        {
            dret = copy_from_user(local_buffer, ioctl_msg->data_buffer, ioctl_msg->data_len);
            if(0 > dret)
            {
                if(NULL != local_buffer)
                {
                    kfree(local_buffer);
                }
                
                DLOAD_PRINT(KERN_ERR, "copy_from_user ERROR.");
                return DLOAD_ERR;
            }
        }

        local_len = ioctl_msg->data_len;
    }
    
    ret = msg_func((void *)local_buffer, local_len);
    
    if(NULL != local_buffer)
    {
        kfree(local_buffer);
    }
    
    return ret;
}

/*******************************************************************
  ��������  : dload_Ioctl
  ��������  : �����������ioctl����
  �������  : 
  �������  : NA
  ��������ֵ: DLOAD_OK    : �ɹ�
              DLOAD_ERR : ʧ��
********************************************************************/
STATIC long dload_Ioctl(struct file *file, unsigned int cmd, unsigned long data)
{
    int32 ret  = DLOAD_OK;

    if((unsigned long)NULL == data)
    {
        DLOAD_PRINT(KERN_ERR, "data is NULL.");
        return DLOAD_ERR;
    }
    
    /* ����������Ч�� */
    if (DLOAD_IOC_TYPE != _IOC_TYPE(cmd)) 
    {
        DLOAD_PRINT(KERN_ERR, "ioctl_type = %c  Ioctl command TYPE ERROR.", _IOC_TYPE(cmd));
        return DLOAD_ERR;
    }

    if (sizeof(ioctl_msg_t) != _IOC_SIZE(cmd)) 
    {
        DLOAD_PRINT(KERN_ERR, "ioctl_size = %d  Ioctl command SIZE ERROR.", _IOC_SIZE(cmd));
        return DLOAD_ERR;
    }

    mutex_lock(&ioctl_mutex);
    switch(cmd)
    {
        case DLOAD_GET_CMD:
            if(_IOC_READ == _IOC_DIR(cmd))
            {
                ret = nark_get_data((ioctl_msg_t *)data);
            }
            else
            {
                DLOAD_PRINT(KERN_ERR, "ioctl_type = %c  Ioctl command DIR ERROR.", _IOC_DIR(cmd));
                ret = DLOAD_ERR;
            }
            break;
        case DLOAD_SET_CMD:
            if(_IOC_WRITE == _IOC_DIR(cmd))
            {
                ret = nark_set_data((ioctl_msg_t *)data);
            }
            else
            {
                DLOAD_PRINT(KERN_ERR, "ioctl_type = %c  Ioctl command DIR ERROR.", _IOC_DIR(cmd));
                ret = DLOAD_ERR;
            }
            break;
        default:
            ret = DLOAD_ERR;
            DLOAD_PRINT(KERN_ERR, "Ioctl command ERROR.");
            break;
    }
    mutex_unlock(&ioctl_mutex);
    
    return ret;
}

STATIC int dload_proc_show(struct seq_file *pSeq, void *pVal)
{
    int32  ret                 = -1;
    uint32 visit_value        = GET_SMEM_IS_SWITCH_PORT;

    mutex_lock(&ioctl_mutex);
    ret = visit_dload_smem((uint8 *)&visit_value, sizeof(visit_value));
    if(DLOAD_OK == ret)
    {
        seq_puts(pSeq, "nv_restore_start");
    }
    else
    {
        seq_puts(pSeq, "nv_restore_end");
	}
    mutex_unlock(&ioctl_mutex);
	
    return 0;
}

STATIC int dload_proc_open(struct inode *inode, struct file *file)
{
    return single_open(file, dload_proc_show, NULL);
}

STATIC struct file_operations dload_proc_fops = {
    .owner          = THIS_MODULE,
    .open           = dload_proc_open,
    .read           = seq_read,
    .llseek         = seq_lseek,
    .release        = single_release,
};

STATIC struct file_operations dload_dev_fops = {
    .owner          = THIS_MODULE,
    .unlocked_ioctl = dload_Ioctl,
};

STATIC struct miscdevice dload_miscdev = {
    .minor    = MISC_DYNAMIC_MINOR,
    .name     = "dload_nark",
    .fops     = &dload_dev_fops
};

STATIC __init int dload_init(void)
{
    int32 ret = DLOAD_OK;
    
    /*nark apiģ���ʼ��*/
    if (DLOAD_ERR == nark_api_init())
    {
        DLOAD_PRINT(KERN_ERR, "nark_api_init ERROR.");
        return DLOAD_ERR;
    }
    
    ret = misc_register(&dload_miscdev);
    if (0 > ret)
    {
        DLOAD_PRINT(KERN_ERR, "misc_register FAILED.");
    }

    /* ��ʼ�������� */
    mutex_init(&ioctl_mutex);

    /* ��������proc�ڵ� */
    proc_create("dload_nark", S_IRUGO, NULL, &dload_proc_fops);
    
    return ret;
}

STATIC void __exit dload_exit(void)
{
    int32 ret = 0;

    /* �Ƴ�proc�ڵ� */
    remove_proc_entry("dload_nark", NULL); 
    
    ret = misc_deregister(&dload_miscdev);
    if (0 > ret)
    {
        DLOAD_PRINT(KERN_ERR, "misc_deregister FAILED.");
    }
}

module_init(dload_init);
module_exit(dload_exit);

MODULE_AUTHOR("MBB.Huawei Device");
MODULE_DESCRIPTION("Dload Driver");
MODULE_LICENSE("GPL");
#ifdef __cplusplus
}
#endif
