/******************************************************************************
  Copyright, 1988-2013, Huawei Tech. Co., Ltd.
  File name:      mbb_common.c
  Author:         �쳬 x00202188
  Version:        V1.0 
  Date:           �쳬     ��������      2013-11-25
  Description:    MBBͨ������ʵ���ļ���
                  ��������Ϊ�˽�MBB��߱Ƚ���ɢ�����㵥��д��������Ķ���ͳһ��һ��������������
  Others:         ���ļ�����д�����������MBBͨ��������ع���ʹ�ã����������޹ش���
                  д�뱾�ļ���
******************************************************************************/
/**********************���ⵥ�޸ļ�¼******************************************
 ��    ��          �޸���         ���ⵥ��           �޸�����
******************************************************************************/


#include <linux/errno.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include "bsp_ipc.h"
#include "bsp_icc.h"
#include "bsp_sram.h"
#include <mbb_common.h>
#include <linux/platform_device.h> 
#if ( FEATURE_ON == MBB_MLOG )
#include <linux/mlog_lib.h>
#include <linux/string.h>
#endif
#include "drv_ipc_enum.h"
struct mbb_common_struct mbb_common_config;

#ifdef CONFIG_USB_GADGET_SNPS_DWC_OTG
extern void dwc_otg_enter_suspend(void);
extern void dwc_otg_exit_suspend(void);
#endif
#if ( FEATURE_ON == MBB_MLOG )
typedef struct
{
    uint32 one_class_temp_start_time;
    uint32 one_class_temp_last_time;
    uint32 two_class_temp_start_time;
    uint32 two_class_temp_last_time;
    uint32 dropline_class_temp_start_time;
    uint32 dropline_class_temp_last_time;
} ThermStatTime_t;

ThermStatTime_t  g_ThermStatTime = {0, 0, 0, 0, 0, 0};

typedef enum _THERM_STATE_TYPE
{
    THERM_STATE_NORMAL = 0,
    THERM_STATE_LIMIT_1 = 1 ,  /*20M*/
    THERM_STATE_LIMIT_2 = 2 ,  /*2M*/
    THERM_STATE_EMERG_1 = 3 ,  /*fly mode*/
    THERM_STATE_EMERG_2 = 4 ,  /*shutdown*/
    THERM_STATE_MAX = 0xFFFF,  /*stop*/
} THERM_STATE_TYPE;

THERM_STATE_TYPE current_therm_state = THERM_STATE_MAX;
THERM_STATE_TYPE pre_therm_state = THERM_STATE_MAX;

#define SIX_MINUTE_TICKS   (HZ * 60 * 6)

void  mstat_thermal_protection(char* p);

#endif
#if ( FEATURE_ON == MBB_MLOG )
/********************************************************
*������   : mbb_common_mlog_recv
*�������� : ����C����Ҫ����mobile logģ�����־
*������� : u32 channel_id , u32 len, void* context
*������� : ��
*����ֵ   : ִ�гɹ�����0��ʧ�ܷ��ط�0ֵ
*�޸���ʷ :
*           2014-2-18 �쳬 ��������
********************************************************/
static int mbb_common_mlog_recv( u32 channel_id , u32 len, void* context )
{
    int rt = 0;
    int read_len = 0;
    stMbbCommonMlog mlogbuf;
    mlogbuf.len = 0;
    mlogbuf.loglv = 0;

    read_len = bsp_icc_read(channel_id, (u8*)&mlogbuf, len);
    if((read_len > len) || (read_len < 0))
    {
        printk(KERN_ERR "[Mbb Common Drv] mlogbuf.len =(%d),read len(%d),len(%d), msg: %s.\n", mlogbuf.len, read_len,len, mlogbuf.msg);
        return -1;
    }

    printk(KERN_INFO "[Mbb Common Drv] module=(%s),mlogbuf.len =(%d),read len(%d),len(%d),lv=(%d),msg: %s.\n",
        mlogbuf.module, mlogbuf.len, read_len, len, mlogbuf.loglv, mlogbuf.msg);
#if (FEATURE_ON == MBB_MLOG)
    mlog_print(mlogbuf.module, mlogbuf.loglv, "%s", mlogbuf.msg);
#endif
#if ( FEATURE_ON == MBB_MLOG)
    mstat_thermal_protection(mlogbuf.module);
#endif
    return 0;
}
#endif

#if ( FEATURE_ON == MBB_MLOG)
void  mstat_thermal_protection(char* p)
{
    char* tp_sign = "TP_CLASS_" ;
    char* tp_class = NULL;
    char* tp_string = p ;
    unsigned int last_times = 0;
    unsigned int tp_class_max_size = 0 ;

    if ( NULL == p)
    {
        return;
    }

    if ( 0 != strncmp(tp_string, tp_sign, strlen(tp_sign)))
    {
        return;
    }

    tp_class = p + strlen(tp_sign) ;
    tp_class_max_size = MLOG_MAX_MD_LEN - strlen(tp_sign) - 1;

    printk("%s tp_class_max_size= %d  %s %d \n" , tp_class, tp_class_max_size, __FUNCTION__, __LINE__);
    if (0 == strncmp(tp_class, "ONE", min(strlen(tp_sign), tp_class_max_size )))
    {
        current_therm_state = THERM_STATE_LIMIT_1;
        if (pre_therm_state != current_therm_state)
        {
            mlog_set_statis_info("one_class_temp_prot_count", 1);
            g_ThermStatTime.one_class_temp_start_time = jiffies;
        }
    }

    if (0 == strncmp(tp_class, "TWO", min(strlen(tp_sign), tp_class_max_size )))
    {
        current_therm_state = THERM_STATE_LIMIT_2;
        if (pre_therm_state != current_therm_state)
        {
            mlog_set_statis_info("two_class_temp_prot_count", 1);
            g_ThermStatTime.two_class_temp_start_time = jiffies;
        }
    }

    if (0 == strncmp(tp_class, "DROPLINE", min(strlen(tp_sign), tp_class_max_size)))
    {
        current_therm_state = THERM_STATE_EMERG_1;
        if (pre_therm_state != current_therm_state)
        {
            mlog_set_statis_info("dropline_class_temp_prot_count", 1);
            g_ThermStatTime.dropline_class_temp_start_time = jiffies;
        }
    }

    if (0 == strncmp(tp_class, "NORMAL", min(strlen(tp_sign), tp_class_max_size)))
    {
        printk("f00174127 debug %s  %s %d \n" , tp_class, __FUNCTION__, __LINE__);
        current_therm_state = THERM_STATE_NORMAL;
    }

    if (0 == strncmp(tp_class, "POWEROFF", min(strlen(tp_sign), tp_class_max_size)))
    {
        printk("f00174127 debug %s  %s %d \n" , tp_class, __FUNCTION__, __LINE__);
        current_therm_state = THERM_STATE_EMERG_2;
    }

    if (pre_therm_state != current_therm_state)
    {
        if (THERM_STATE_LIMIT_1 == pre_therm_state)
        {
            /*��ȡ����ʱ��*/
            g_ThermStatTime.one_class_temp_last_time = \
                    g_ThermStatTime.one_class_temp_last_time \
                    + jiffies - g_ThermStatTime.one_class_temp_start_time;

            /*����ʱ��ת����6����*/
            last_times =  g_ThermStatTime.one_class_temp_last_time / SIX_MINUTE_TICKS;

            /*����ʣ���ticks*/
            g_ThermStatTime.one_class_temp_last_time = \
                    g_ThermStatTime.one_class_temp_last_time % SIX_MINUTE_TICKS;

            /*д��ͳ����Ϣ*/
            mlog_set_statis_info("one_class_temp_prot_time", last_times);
        }
        else if (THERM_STATE_LIMIT_2 == pre_therm_state)
        {
            /*��ȡ����ʱ��*/
            g_ThermStatTime.two_class_temp_last_time = \
                    g_ThermStatTime.two_class_temp_last_time \
                    + jiffies - g_ThermStatTime.two_class_temp_start_time;

            /*����ʱ��ת����6����*/
            last_times = g_ThermStatTime.two_class_temp_last_time / SIX_MINUTE_TICKS;
            mlog_set_statis_info("two_class_temp_prot_time", last_times);

            /*����ʣ���ticks*/
            g_ThermStatTime.two_class_temp_last_time = \
                    g_ThermStatTime.two_class_temp_last_time % SIX_MINUTE_TICKS;

            /*д��ͳ����Ϣ*/
            mlog_set_statis_info("two_class_temp_prot_time", last_times);

        }
        else if (THERM_STATE_EMERG_1 == pre_therm_state)
        {
            /*��ȡ����ʱ��*/
            g_ThermStatTime.dropline_class_temp_last_time = \
                    g_ThermStatTime.dropline_class_temp_last_time \
                    + jiffies - g_ThermStatTime.dropline_class_temp_start_time;

            /*����ʱ��ת����6����*/
            last_times = g_ThermStatTime.dropline_class_temp_last_time / SIX_MINUTE_TICKS;

            /*����ʣ���ticks*/
            g_ThermStatTime.dropline_class_temp_last_time = \
                    g_ThermStatTime.dropline_class_temp_last_time % SIX_MINUTE_TICKS;
            mlog_set_statis_info("dropline_class_temp_prot_time", last_times);
        }
    }
    pre_therm_state = current_therm_state;
}
#endif


/********************************************************
*������   : mbb_common_ipc_send
*�������� : ��C�˷�����Ϣ
*������� : u32 flag
*������� : ��
*����ֵ   : ִ�гɹ�����0��ʧ�ܷ��ط�0ֵ
*�޸���ʷ :
*           2013-11-25 �쳬 ��������
********************************************************/
void mbb_common_ipc_send(u32 flag)
{
    SRAM_SMALL_SECTIONS * sram_mem = ((SRAM_SMALL_SECTIONS * )SRAM_SMALL_SECTIONS_ADDR);
    sram_mem->SRAM_MBB_COMMON = flag;
    bsp_ipc_int_send(IPC_CORE_CCORE,IPC_CCPU_INT_SRC_ACPU_MBB_COMM);
}


/********************************************************
*������   : Mbb_Common_Probe
*�������� : 
*������� : struct platform_device *dev
*������� : ��
*����ֵ   : ִ�гɹ�����0��ʧ�ܷ��ط�0ֵ
*�޸���ʷ :
*           2013-11-25 �쳬 ��������
********************************************************/
static int Mbb_Common_Probe(struct platform_device *dev)
{

    int ret = 0;
#if ( FEATURE_ON == MBB_MLOG )
    u32 channel_id = ICC_CHN_IFC << 16 | IFC_RECV_FUNC_MLOG; /* ���ڽ���C�˵�mobile log��Ϣ */
#endif

    printk(KERN_ERR "\r\n [Mbb Common Drv] Mbb_Common_Probe!\n");

    if(NULL == dev )
    {
        printk(KERN_ERR "\r\n [Mbb Common Drv]Mbb_Common_Probe: dev is NULL!\r\n");
        return -1;
    }
    
#if ( FEATURE_ON == MBB_MLOG )
    /* ע�����ڽ���C�˵�mobile log��Ϣ�ĺ��� */
    ret = bsp_icc_event_register(channel_id, (read_cb_func)mbb_common_mlog_recv, NULL, NULL, NULL);
    if(ret != 0)
    {
        printk(KERN_ERR "\r\n [Mbb Common Drv] bsp_icc_event_register error��ret=%d.\n", ret);
    }
#endif

    return ret;
}
/********************************************************
*������   : Mbb_Common_Remove
*�������� : 
*������� : 
*������� : ��
*����ֵ   : ִ�гɹ�����0��ʧ�ܷ��ط�0ֵ
*�޸���ʷ :
*           2013-11-25 �쳬 ��������
********************************************************/
static int Mbb_Common_Remove(struct platform_device *dev)
{
    int ret = 0;

    printk(KERN_ERR "\r\n [Mbb Common Drv] Mbb_Common_Remove!\n");
    
    if(NULL == dev )
    {
        printk(KERN_ERR "\r\n [Mbb Common Drv]Mbb_Common_Remove: dev is NULL!\r\n");
        return -1;
    }

    return ret;
}

#ifdef CONFIG_PM
static int Mbb_Common_Prepare(struct device *pdev)
{
    return 0;
}

static void Mbb_Common_Complete(struct device *pdev)
{
    return ;
}

/********************************************************
*������   : Mbb_Common_Suspend
*�������� : Mbb_Common���������ڴ����������ʱ��һЩ����
*������� : 
*������� : ��
*����ֵ   : ִ�гɹ�����0��ʧ�ܷ��ط�0ֵ
*�޸���ʷ :
*           2013-11-25 �쳬 ��������
********************************************************/
static int Mbb_Common_Suspend(struct device *dev)
{
    int ret = 0;

    printk(KERN_INFO "\r\n [Mbb Common Drv] Mbb_Common_Suspend!\n");
    
    if(NULL == dev )
    {
        printk(KERN_ERR "\r\n [Mbb Common Drv]Mbb_Common_Suspend: dev is NULL!\r\n");
        return -1;
    }
#ifdef CONFIG_USB_GADGET_SNPS_DWC_OTG
    dwc_otg_enter_suspend();
#endif 

    /* ��A��Ҫ�����������Ϣ֪ͨC��*/
    mbb_common_ipc_send(MBB_SUSPEND);
    return ret;
}

/********************************************************
*������   : Mbb_Common_Resume
*�������� : Mbb_Common���������ڴ����������ʱ��һЩ����
*������� : 
*������� : ��
*����ֵ   : ִ�гɹ�����0��ʧ�ܷ��ط�0ֵ
*�޸���ʷ :
*           2013-11-25 �쳬 ��������
********************************************************/
static int Mbb_Common_Resume(struct device *dev)
{
    int ret = 0;

    printk(KERN_INFO "\r\n [Mbb Common Drv] Mbb_Common_Resume!\n");
    
    if(NULL == dev )
    {
        printk(KERN_ERR "\r\n [Mbb Common Drv]Mbb_Common_Resume: dev is NULL!\r\n");
        return -1;
    }
    /* ��A�˱����ѵ���Ϣ֪ͨC��*/
#ifdef CONFIG_USB_GADGET_SNPS_DWC_OTG
    dwc_otg_exit_suspend();
#endif 

    mbb_common_ipc_send(MBB_RESUME);
    return ret;
}

const struct dev_pm_ops Mbb_Common_dev_Pm_Ops =
{
    .suspend    =   Mbb_Common_Suspend,
    .resume     =   Mbb_Common_Resume,
    .prepare    =   Mbb_Common_Prepare,
    .complete   =   Mbb_Common_Complete,
};

#define MBB_COMMON_DEV_PM_OPS (&Mbb_Common_dev_Pm_Ops)

#else

#define MBB_COMMON_DEV_PM_OPS NULL

#endif

static struct platform_driver Mbb_Common_drv = {
    .probe = Mbb_Common_Probe,
    .remove = Mbb_Common_Remove,
    .driver = {  
        .name  = "Mbb_Common",  
        .owner = THIS_MODULE, 
        .pm    = MBB_COMMON_DEV_PM_OPS,
    },  
};

static struct platform_device Mbb_Common_dev = {
    .name           = "Mbb_Common",
    .id             = 1,
    .dev ={
        .platform_data = &mbb_common_config,
    },
};

static int __init mbb_common_init(void)
{
    int ret = 0;
    
    ret = platform_device_register(&Mbb_Common_dev);
    if (ret < 0)
    {
        printk(KERN_ERR "\r\n [Mbb Common Drv] platform_device_register [Mbb Common Module] failed.\r\n");
        return ret;
    }
    
    ret = platform_driver_register(&Mbb_Common_drv);
    if (0 > ret)
    {
        platform_device_unregister(&Mbb_Common_dev);
        printk(KERN_ERR "\r\n [Mbb Common Drv] platform_driver_register [Mbb Common Module] failed.\r\n");
        return ret;
    }
    return ret;
}

static void __exit mbb_common_exit(void)
{
    platform_driver_unregister(&Mbb_Common_drv);

    platform_device_unregister(&Mbb_Common_dev);
}

module_init(mbb_common_init);
module_exit(mbb_common_exit);

MODULE_AUTHOR("MBB.Huawei Device");
MODULE_DESCRIPTION("Mbb Common Driver");
MODULE_LICENSE("GPL");

