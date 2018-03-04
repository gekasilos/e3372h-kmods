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

/******************************************************************************************************

*****************************************************************************************************/
#include <linux/timer.h>
#include <linux/notifier.h>
#include <linux/irqreturn.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/delay.h>
#include <linux/usb/usb_interface_external.h>
#include "usb_vendor.h"
#include "dwc_otg_extern_charge.h"
#include "usb_charger_manager.h"
#include "mbb_usb_adp.h"
#include "usb_platform_comm.h"
#include "usb_debug.h"
#include "usb_notify.h"
#include <product_config.h>
#include <mbb_config.h>
#include "mbb_fast_on_off.h"


#define DWC_OTG_AB      0xAB
#define OTG_DETECT_OVERTIME 2000
#define  INVALID_CMD  (-1)

static USB_INT  otg_old_cmd = INVALID_CMD;
static USB_INT  otg_new_cmd = INVALID_CMD;

otg_charger_st g_otg_extern_charge;
USB_INT extern_otg_dev = DWC_OTG_AB;
static USB_INT g_otg_usb2lan = MBB_USB_FALSE;


static  USB_INT otg_is_host_mode = MBB_USB_FALSE;


static USB_INT usb_host_charger_current_notify(struct notifier_block* self,
        USB_ULONG action, USB_PVOID dev);


static struct notifier_block usb_host_charger_current_nb =
{
    .notifier_call = usb_host_charger_current_notify
};

USB_INT old_otg_inserted_status = GPIO_HIGH;
USB_INT curr_otg_inserted_status = GPIO_HIGH;
USB_INT otg_det_gpio_value = 1;
USB_INT otg_is_inserted = MBB_USB_FALSE;
static USB_INT otg_first_report_chg_type = 1;


/*********************************************************************
����  : otg_detect_id_thread
����  : ֪ͨ���ģ�飬�������߲���¼�
����  : w
����ֵ: void
*********************************************************************/
#define vbus_irq_delay 300
USB_VOID otg_detect_id_thread( USB_VOID )
{
    usb_adp_ctx_t* ctx = NULL;
    USB_INT fast_on_off = 0;
    fast_on_off = usb_fast_on_off_stat();
    ctx = usb_get_adp_ctx();
    if (NULL == ctx)
    {
        DBG_E(MBB_OTG_CHARGER, "--otg_detect_id_thread:  ctx == NULL -- \n");
        return;
    }
    /*�жϳ��ģʽ���Ƿ��ִ�ж�����*/
    if (!usb_chg_check_current_mode(USB_CHG_EXTERN))
    {
        DBG_E(MBB_OTG_CHARGER, "charge mode is mutually exclusive \n");
        return;
    }
    /*�ж��Ƿ�Ϊ�ٹػ�������ǣ����������*/
    if (fast_on_off)
    {
        wake_unlock(&g_otg_extern_charge.otg_chg_wakelock);
        return;
    }

    DBG_I(MBB_OTG_CHARGER, "old_otg_inserted_status = %d,curr_otg_inserted_status=%d", \
                            old_otg_inserted_status, curr_otg_inserted_status);

    /*�жϵ�ǰ�ϱ�״̬��֮ǰ��ͬ�����ϱ���Ϣ������*/
    if (old_otg_inserted_status == curr_otg_inserted_status )
    {
        DBG_E(MBB_OTG_CHARGER,"old_otg_inserted_status == curr_otg_inserted_status\n");
        return;
    }

    old_otg_inserted_status = curr_otg_inserted_status;
    DBG_I(MBB_OTG_CHARGER, "notify usb id is %8s.\n", 
        otg_is_inserted ? "inserted" : "removed");

    /*AF18Ϊusbת�������Ʒ���˴�Ӧ������usbת������Ʒ���Ժ�*/
    /*��֧��usbת��������,�ж�Ϊusbת����ʱ�β� ��������###*/
    msleep(vbus_irq_delay);
    if ((MBB_USB_TRUE == otg_is_inserted) && (MBB_USB_TRUE == usb_get_vbus_status()))
    {
        g_otg_usb2lan = MBB_USB_TRUE;
        DBG_I(MBB_OTG_CHARGER, "OTG device AF18 is inserted.\r\n");
        return;
    }
    else if (MBB_USB_FALSE == otg_is_inserted)
    {
        /*�ж��Ƿ�usbת��������*/
        if (g_otg_usb2lan == MBB_USB_TRUE)
        {
            /*���usbת����״̬*/
            g_otg_usb2lan = MBB_USB_FALSE;
            DBG_I(MBB_OTG_CHARGER, "OTG device AF18 is removed.\r\n");
            return;
        }
    }

    /*��һ���ϱ���ֱ���ó�����ͣ���ֹ���û��*/
    if (ctx->stm_set_chg_type_cb && otg_first_report_chg_type && otg_is_inserted)
    {
        DBG_I(MBB_OTG_CHARGER, "-----first notify exchg----\n");
        ctx->stm_set_chg_type_cb(CHG_EXGCHG_CHGR);
        otg_first_report_chg_type = 0;
    }
    /*�������߲���*/
    if (otg_is_inserted)
    {   
        wake_lock(&g_otg_extern_charge.otg_chg_wakelock);
        /*���õ�ǰ��繤��ģʽΪ������*/
        usb_chg_set_work_mode(USB_CHG_EXTERN);
        otg_old_cmd = INVALID_CMD;
        otg_new_cmd = INVALID_CMD;
        ctx->battery_notifier_call_chain_cb(otg_is_inserted, CHG_EXGCHG_CHGR);
        msleep(OTG_DETECT_OVERTIME);
    }
    /*�������߰γ�*/
    else
    {
        ctx->battery_notifier_call_chain_cb(otg_is_inserted, CHG_EXGCHG_CHGR);
        usb_notify_event(USB_OTG_ID_PULL_OUT, NULL);
        msleep(OTG_DETECT_OVERTIME);
        /*�˳������磬��ճ�繤��ģʽ*/
        usb_chg_set_work_mode(USB_CHG_NON);
        otg_old_cmd = INVALID_CMD;
        otg_new_cmd = INVALID_CMD;
        wake_unlock(&g_otg_extern_charge.otg_chg_wakelock);
    }
}

static irqreturn_t dwc_otg_det_irq(USB_INT irq, USB_PVOID dev_id)
{
    USB_INT is_fast_off = 0;
    DBG_I(MBB_OTG_CHARGER, "dwc_otg_det_irq:enter\n");


    otg_det_gpio_value = gpio_get_value(GPIO_OTG_ID_DET);
    /*
    ���gpio�ϱ��˶���жϣ�2�ζ���һ����Ϣ����ô��Ϊ��Ϣ
    ���󣬲�������
    */
    curr_otg_inserted_status = otg_det_gpio_value;

    /*
    ����ǹػ�ģʽ����������ʱ���ϱ���Ϣ�����Ǽ�¼otg״̬
    */
    is_fast_off = usb_fast_on_off_stat();
    if (otg_det_gpio_value)
    {
        gpio_int_trigger_set(GPIO_OTG_ID_DET, IRQ_TYPE_LEVEL_LOW);
        /* ID �߰γ� */
        DBG_I(MBB_OTG_CHARGER, "OTG_ID pull out!\n");
        otg_is_inserted = MBB_USB_FALSE;
    }
    else
    {
        gpio_int_trigger_set(GPIO_OTG_ID_DET, IRQ_TYPE_LEVEL_HIGH);
        /* ID �߲���*/
        DBG_I(MBB_OTG_CHARGER, "OTG_ID pull in!\n");
        otg_is_inserted = MBB_USB_TRUE;
    }
    if (!is_fast_off)
    {
        schedule_work(&g_otg_extern_charge.otg_chg_notify_work);
    }
    else
    {
        DBG_E(MBB_OTG_CHARGER, "--it is in fast_off mode--\n");

    }
    return IRQ_HANDLED;
}

static USB_INT otg_register_otg_det_irq(USB_VOID)
{
    const char* desc = "OTG_DET_GPIO";
    irq_handler_t isr = NULL;
    unsigned long irqflags = 0;
    USB_INT irq = -1;
    USB_INT error = -1;
    if (gpio_is_valid(GPIO_OTG_ID_DET))
    {
        DBG_I(MBB_OTG_CHARGER, "gipo is valid\n");
        error = gpio_request(GPIO_OTG_ID_DET, "otg_int");
        if (error < 0)
        {
            DBG_E(MBB_OTG_CHARGER, "Failed to request GPIO %d, error %d\n", GPIO_OTG_ID_DET, error);
            return error;
        }
        gpio_int_mask_set(GPIO_OTG_ID_DET);
        gpio_int_state_clear(GPIO_OTG_ID_DET);
        error = gpio_direction_input(GPIO_OTG_ID_DET);
        if (error < 0)
        {
            DBG_E(MBB_OTG_CHARGER, "Failed to configure direction for GPIO %d, error s%d\n", GPIO_OTG_ID_DET, error);
            goto fail;
        }

        gpio_set_function(GPIO_OTG_ID_DET, GPIO_INTERRUPT);

        /*�����Ǵ�������usb id���ϵ�ʱ����Ҫ�ж�һ�µ�ǰ��id״̬��֪ͨ���ģ���Ƿ���*/
        dwc_otg_det_irq(NULL, NULL);

        irq = gpio_to_irq(GPIO_OTG_ID_DET);
        if (irq < 0)
        {
            error = irq;
            DBG_E(MBB_OTG_CHARGER, "Unable to get irq number for GPIO %d, error %d\n", GPIO_OTG_ID_DET, error);
            goto fail;
        }

        isr = dwc_otg_det_irq;
        irqflags = IRQF_NO_SUSPEND | IRQF_SHARED;

    }


    error = request_irq(irq, isr, irqflags, desc, &extern_otg_dev);
    if (error < 0)
    {
        DBG_E(MBB_OTG_CHARGER, "Unable to claim irq %d; error %d\n", irq, error);

        goto fail;
    }
    gpio_int_state_clear(GPIO_OTG_ID_DET);
    gpio_int_unmask_set(GPIO_OTG_ID_DET);

    return MBB_USB_OK;

fail:
    if (gpio_is_valid(GPIO_OTG_ID_DET))
    {
        gpio_free(GPIO_OTG_ID_DET);
    }

    return error;
}

static USB_INT otg_unregister_otg_det_irq(USB_VOID)
{
    if (gpio_is_valid(GPIO_OTG_ID_DET))
    {
        gpio_free(GPIO_OTG_ID_DET);
    }
    if (gpio_is_valid(GPIO_DMDP_CONNECT))
    {
        gpio_free(GPIO_DMDP_CONNECT);
    }
    if (gpio_is_valid(GPIO_OTG_ID_SET))
    {
        gpio_free(GPIO_OTG_ID_SET);
    }

    old_otg_inserted_status = GPIO_HIGH;
    curr_otg_inserted_status = GPIO_HIGH;
    return MBB_USB_OK;
}

/*********************************************************************
����  :otg_exchg_status_clean
����  :����ٹػ������������״̬
����  :
����ֵ:
*********************************************************************/
static USB_VOID otg_exchg_status_clean(USB_VOID)
{
    USB_INT charger_type = USB_CHARGER_TYPE_INVALID;
    struct blocking_notifier_head *usb_notifier_list = NULL;
    gpio_int_mask_set(GPIO_OTG_ID_DET);
    gpio_int_state_clear(GPIO_OTG_ID_DET);

    /*�Ͽ�D+,D-,���� phy ID */
    gpio_set_value(GPIO_DMDP_CONNECT, GPIO_LOW);
    gpio_set_value(GPIO_OTG_ID_SET, GPIO_HIGH);

    if (MBB_USB_TRUE == otg_is_host_mode)
    {
        /*֪ͨ�ر�OTG�߳�*/
	usb_notifier_list = usb_get_notifier_handle();
        blocking_notifier_call_chain(usb_notifier_list,
                             USB_BALONG_PERIP_REMOVE, (void*)&charger_type);
        usb_balong_exit();
        if (power_off_dwc3_usb())
        {
            DBG_E(MBB_OTG_CHARGER , "fail to disable the dwc3 usb regulator!\n");
        }
    }
    otg_old_cmd = INVALID_CMD;
    otg_new_cmd = INVALID_CMD;
    otg_is_host_mode = MBB_USB_FALSE;
    old_otg_inserted_status = GPIO_HIGH;
    usb_chg_set_work_mode(USB_CHG_NON);
    wake_unlock(&g_otg_extern_charge.otg_chg_wakelock);
}

/*********************************************************************
����  :otg_switch_to_host_mode
����  :�������л��� OTG ģʽ��磬500mA
����  :
����ֵ:
*********************************************************************/
static USB_VOID otg_switch_to_host_mode(USB_VOID)
{
    /*��ʼ�� USB ����������  OTG  host mode*/
    if (power_on_dwc3_usb())
    {
        DBG_E(MBB_OTG_CHARGER, "---power on dwc3 usb failed!---\n");
        return;
    }
    usb_balong_init();
    otg_is_host_mode = MBB_USB_TRUE;
    /*�Ͽ� D+, D- , ���� ID */
    gpio_set_value(GPIO_DMDP_CONNECT, GPIO_LOW);
    gpio_set_value(GPIO_OTG_ID_SET, GPIO_LOW);
}

/*********************************************************************
����  :otg_exchg_remove
����  :�������߰γ�����
����  :
����ֵ:
*********************************************************************/
static USB_VOID otg_exchg_remove(USB_VOID)
{
    USB_INT charger_type = USB_CHARGER_TYPE_INVALID;
   struct blocking_notifier_head *usb_notifier_list = NULL;
    DBG_E(MBB_OTG_CHARGER , "%s: USB_OTG_ID_PULL_OUT \n",
                             __func__  );

    /*�Ͽ�D+,D-,���� phy ID */
    gpio_set_value(GPIO_DMDP_CONNECT, GPIO_LOW);
    gpio_set_value(GPIO_OTG_ID_SET, GPIO_HIGH);

    if (MBB_USB_TRUE == otg_is_host_mode)
    {
        /*֪ͨ�ر�OTG �߳�*/
        usb_notifier_list = usb_get_notifier_handle();
        blocking_notifier_call_chain(&usb_notifier_list,
                                 USB_BALONG_PERIP_REMOVE, (void*)&charger_type);
        usb_balong_exit();
        if (power_off_dwc3_usb())
        {
            DBG_E(MBB_OTG_CHARGER , "fail to disable the dwc3 usb regulator!\n");
        }
    }
    otg_is_host_mode = MBB_USB_FALSE;
    /*�˴������usbת������λ###*/
}

/*********************************************************************
����  :usb_check_repeat_cmd
����  :����Ƿ��ظ����յ�ͬ�����������ǣ�
               ������ᣬע��( cmd >= 0)
����  :
����ֵ:
*********************************************************************/
USB_BOOL  usb_check_repeat_cmd(USB_INT cmd)
{
    USB_BOOL  ret = MBB_USB_FALSE;

    if (cmd < 0)
    {
        DBG_E(MBB_OTG_CHARGER ," illegal cmd=%d < 0 \n", cmd);
    }
    otg_new_cmd = cmd;
    DBG_E(MBB_OTG_CHARGER , "recieve cmd = %d \n", cmd);
    if ( otg_old_cmd ==  otg_new_cmd)
    {
        ret = MBB_USB_FALSE;
        DBG_E(MBB_OTG_CHARGER , "recieve repeated cmd = %d , ignore it\n", otg_new_cmd);
    }
    else
    {
        otg_old_cmd =  otg_new_cmd;
        ret = MBB_USB_TRUE;
    }
    return ret;
}

/*********************************************************************
����  :usb_otg_current_notify
����  :�����������������USB״̬
����  :action:
����ֵ:
*********************************************************************/
static USB_INT usb_host_charger_current_notify(struct notifier_block* self,
        USB_ULONG action, USB_PVOID dev)
{

    /*�жϳ��ģʽ���Ƿ��ִ�ж�����*/
    if (!usb_chg_check_current_mode(USB_CHG_EXTERN))
    {
        DBG_E(MBB_OTG_CHARGER, "charge mode is mutually exclusive \n");
        return;
    }

    if (!usb_check_repeat_cmd(action))
    {
        DBG_E(MBB_OTG_CHARGER, "repeated action = %ld ,do nothing.\n", action);
        return MBB_USB_ERROR;
    }

    gpio_int_mask_set(GPIO_DMDP_CONNECT);
    gpio_int_state_clear(GPIO_DMDP_CONNECT);
    gpio_direction_output(GPIO_DMDP_CONNECT, 0);


    gpio_int_mask_set(GPIO_OTG_ID_SET);
    gpio_int_state_clear(GPIO_OTG_ID_SET);
    gpio_direction_output(GPIO_OTG_ID_SET, 1);

    DBG_I(MBB_OTG_CHARGER, "usb_otg_current_notify action is %ld.\n", action);

    switch (action)
    {
        case USB_OTG_CONNECT_DP_DM:
            /*�̽� D+ D- , ���� 1 A ���*/
            gpio_set_value(GPIO_DMDP_CONNECT, GPIO_HIGH);
            break;
        /* �Ͽ�D+ D-�����Ժ����� ID �� ����Host ģʽ������otg��� */
        case USB_OTG_DISCONNECT_DP_DM:
            otg_switch_to_host_mode();
            break;
            /*�������ߣ��γ�*/
        case USB_OTG_ID_PULL_OUT:
            otg_exchg_remove();
            break;
            /*����ٹػ���������*/
        case USB_OTG_DISABLE_ID_IRQ:
            /*ģ����ػ����̣�ʹ�ٹػ��ָ�����ʼ״̬ */
            otg_exchg_status_clean();
            break;
            /*�˳��ٹػ���������*/
        case USB_OTG_ENABLE_ID_IRQ:
            /* ��ȡ��ǰID GPIO ״̬������ǵ�ID ��λ�ϱ�ID�����¼�,
            ���½������������ */
            dwc_otg_det_irq(NULL, NULL);
            gpio_int_state_clear(GPIO_OTG_ID_DET);
            gpio_int_unmask_set(GPIO_OTG_ID_DET);
            break;
        default:
            break;
    }

    return MBB_USB_OK;
}

USB_INT chager_test(USB_ULONG action)
{
    gpio_int_mask_set(GPIO_DMDP_CONNECT);
    gpio_int_state_clear(GPIO_DMDP_CONNECT);
    gpio_direction_output(GPIO_DMDP_CONNECT, 0);

    gpio_int_mask_set(GPIO_OTG_ID_SET);
    gpio_int_state_clear(GPIO_OTG_ID_SET);
    gpio_direction_output(GPIO_OTG_ID_SET, 1);

    DBG_I(MBB_OTG_CHARGER, "usb_otg_current_notify action is %ld.\n", action);

    switch (action)
    {
        case USB_OTG_CONNECT_DP_DM:
            gpio_set_value(GPIO_DMDP_CONNECT, GPIO_HIGH);
            break;

        case USB_OTG_DISCONNECT_DP_DM:
            /* �Ͽ�D+ D-�����Ժ����� ID �� ����Host ģʽ������otg��� */
            gpio_set_value(GPIO_DMDP_CONNECT, GPIO_HIGH);
            gpio_set_value(GPIO_DMDP_CONNECT, GPIO_LOW);
            gpio_set_value(GPIO_OTG_ID_SET, GPIO_LOW);
            break;

        case USB_OTG_ID_PULL_OUT:
            /* ID�γ� �Ͽ�D+ D-���ӣ����� ID �� �ָ�USB ģʽ */
            gpio_set_value(GPIO_OTG_ID_SET, GPIO_HIGH);
            gpio_set_value(GPIO_DMDP_CONNECT, GPIO_LOW);
            break;

        default:
            break;
    }

    return MBB_USB_OK;
}

EXPORT_SYMBOL(chager_test);
/*********************************************************************
����  : usb_otg_extern_charge_init
����  : �������¼���ʼ��
����  : void
����ֵ: void
*********************************************************************/
USB_VOID usb_otg_extern_charge_init(USB_VOID)
{
    USB_INT ret = 0;

    ret = gpio_request(GPIO_DMDP_CONNECT, "OTG_DP_DM_CONNECT_GPIO");
    if (ret < 0)
    {
        DBG_E(MBB_OTG_CHARGER, "gpio request failed for OTG_DP_DM_CONNECT_GPIO\n");
        gpio_free(GPIO_DMDP_CONNECT);
    }
    ret = gpio_request(GPIO_OTG_ID_SET, "GPIO_OTG_ID_SET");
    if (ret < 0)
    {
        DBG_E(MBB_OTG_CHARGER, "gpio request failed for GPIO_OTG_ID_SET\n");
        gpio_free(GPIO_OTG_ID_SET);
    }
    wake_lock_init(&g_otg_extern_charge.otg_chg_wakelock, WAKE_LOCK_SUSPEND, "otg-wakelock");
    INIT_WORK(&g_otg_extern_charge.otg_chg_notify_work, otg_detect_id_thread);
    init_waitqueue_head(&g_otg_extern_charge.wait_wq);
    /*��ʼ��������¼���ӿڣ������ģ��ʹ��*/
    usb_register_otg_notify(&usb_host_charger_current_nb);
    otg_register_otg_det_irq();

}

/*********************************************************************
����  : wakeup_wait_wq
����  : ���ѵȴ��¼��Ľӿ�
����  : void
����ֵ: void
*********************************************************************/
USB_VOID wakeup_wait_wq(USB_VOID)
{
    wake_up_interruptible(&g_otg_extern_charge.wait_wq);
}
EXPORT_SYMBOL_GPL(wakeup_wait_wq);

/*********************************************************************
����  : usb_otg_extern_charge_remove
����  : �������¼�ж��
����  : viod
����ֵ: void
*********************************************************************/
USB_VOID usb_otg_extern_charge_remove(USB_VOID)
{
    /*�Ƴ�������¼���ӿ�*/
    usb_unregister_otg_notify(&usb_host_charger_current_nb);
    otg_unregister_otg_det_irq();
    cancel_work_sync(&g_otg_extern_charge.otg_chg_notify_work);
    wake_lock_destroy(&g_otg_extern_charge.otg_chg_wakelock);

}

