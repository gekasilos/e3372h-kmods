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
#ifndef _DLOAD_COMM_H_
#define _DLOAD_COMM_H_
#ifdef  __cplusplus
    extern "C"{
#endif


/****************************************************************************
  2 ���������ض���
*****************************************************************************/

typedef int                int32;
typedef short              int16;
typedef char               int8;
typedef unsigned long long uint64;
typedef long long          int64;
typedef unsigned long      uint32;
typedef unsigned short     uint16;
typedef unsigned char      uint8;
typedef unsigned char      boolean;

/****************************************************************************
 3 ö�����ͣ��ṹ�壬�궨��
 �ṹ�����ͼ� :_s
 ö�����ͼ�   :_e
 ���������ͼ� :_u
 
****************************************************************************/
#define EXTERN   extern
#define STATIC   static
#define CONST    const

#ifndef NULL
#define NULL 0
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#define DLOAD_ERR        -1
#define DLOAD_OK          0

#define DLOAD_NAME_MAX_LEN 256

/****************************************************************************
 3 ö�����ͣ��ṹ�壬�궨��
 �ṹ�����ͼ� :_s
 ö�����ͼ�   :_e
 ���������ͼ� :_u
****************************************************************************/
typedef enum
{
    DLOAD_BALONG,  /* ����ƽ̨ */
    DLOAD_QCOMM,   /* ��ͨƽ̨ */
}platform_type_t;

typedef enum
{
    DLOAD_STICK,    /* ���ݿ���̬ */
    DLOAD_HILINK,   /* ���翨��̬ */
    DLOAD_WINGLE,   /* wingle��̬ */
    DLOAD_E5,       /* ͨ��E5��̬ */
    DLOAD_CPE,      /* CPE��̬ */
    DLOAD_MOD,      /* ģ����̬ */
    DLOAD_UDP,      /* UDP��̬ */
}product_type_t;

/* ����״̬���� */
typedef enum
{
    DLOAD_STATE_NORMAL_IDLE,       /*����״̬*/
    DLOAD_STATE_NORMAL_RUNNING,    /*����������*/
    DLOAD_STATE_AUTOUG_RUNNING,    /*����������*/
    DLOAD_STATE_NORMAL_LOW_POWER,  /*SD��������ص͵�*/
    DLOAD_STATE_NORMAL_SUCCESS,    /*SD�������ɹ�*/
    DLOAD_STATE_NORMAL_FAIL,       /*SD����������������ʧ��ʱ��LCD��ʾ*/
    DLOAD_STATE_FAIL,              /*������������������ʾʧ�� �� usb�����������������ʧ�ܣ�E5 �ڴ�����״̬��lcd ��ʾidle*/
    DLOAD_STATE_INVALID,
    /* ���֧��10��״̬ */
}dload_state_t;

typedef struct _PRODUCT_INFO
{
    platform_type_t  platfm_id_e; /* ƽ̨��ʶ */
    product_type_t   prodct_id_e; /* ��Ʒ��̬ */
    uint32           hw_major_id; /* ��ƷӲ����ID */
    uint32           hw_minor_id; /* ��ƷӲ����ID */
    uint8            platfm_name[DLOAD_NAME_MAX_LEN]; /* ƽ̨���� */
    uint8            prodct_name[DLOAD_NAME_MAX_LEN]; /* �����Ʒ���� */
    uint8            upgrade_id[DLOAD_NAME_MAX_LEN];  /* ��Ʒ����ID */
}product_info_t;

/* ƽ̨��Ϣ�ṹ�� */
typedef struct _PLATFORM_INFO
{
    product_info_t  product_info_st; /* ƽ̨��Ʒ��Ϣ */
    struct _PLATFORM_FEATURE
    {
        boolean  led_feature;           /* LED���� TRUE:֧�֣�FALSE: ��֧�� */
        boolean  lcd_feature;           /* LCD���� TRUE:֧�֣�FALSE: ��֧�� */
        boolean  reserved4;             /* Ԥ�� */
        boolean  reserved3;             /* Ԥ�� */
        boolean  reserved2;             /* Ԥ�� */
        boolean  reserved1;             /* Ԥ�� */
    }platform_feature;
}platform_info_t;

/* �����������ýṹ�� */
#define  FEATURE_RESERV_NUM   50        /* ����Ԥ�����Ը��� */
typedef struct _DLOAD_FEATURE
{
    uint32           hw_major_id;        /* ��ƷӲ����ID */
    uint32           hw_minor_id;        /* ��ƷӲ����ID */
    boolean          usb_dload;          /* ����: USB���� */
    boolean          scard_dload;        /* ����: SD���� */
    boolean          online_dload;       /* ����: �������� */
    boolean          safety_dload;       /* ����: ��ȫ���� */
    boolean          hota_dload;         /* ����: HOTA���� */
    boolean          fota_dload;         /* ����: FOTA���� */
    boolean          reserved[FEATURE_RESERV_NUM]; /* Ԥ�� */
} dload_feature_t;


/* �����ڴ����ö�ٶ��� */
typedef enum
{
    SET_SMEM_DLOAD_MODE_MAGIC,             /* ���ý�������ģʽ��־ */
    SET_SMEM_DLOAD_MODE_ZERO,              /* ���ý�������ģʽ��־���� */
    GET_SMEM_IS_DLOAD_MODE,                /* ��ȡ�Ƿ��������ģʽ */
    SET_SMEM_SCARD_DLOAD_MAGIC,            /* ���ð�������SD������־ */
    SET_SMEM_SCARD_DLOAD_ZERO,             /* ���ð�������SD������־���� */  
    GET_SMEM_IS_SCARD_DLOAD,               /* ��ȡ�Ƿ���ҪSD���� */  
    SET_SMEM_SWITCH_PORT_MAGIC,            /* ����NV�Զ��ָ��׶�USB�˿���̬��־ */
    SET_SMEM_SWITCH_PORT_ZERO,             /* ����NV�Զ��ָ��׶�USB�˿���̬��־���� */ 
    GET_SMEM_IS_SWITCH_PORT,               /* ��ȡ�Ƿ�NV�Զ��ָ��׶�USB�˿���̬���л� */  
    SET_SMEM_ONL_RESTORE_REBOOT_MAGIC,    /* ������������NV�Զ��ָ���ɺ󵥰���Ҫ���� */
    SET_SMEM_ONL_RESTORE_REBOOT_ZERO,     /* ������������NV�Զ��ָ���ɺ󵥰���Ҫ�������� */
    GET_SMEM_ONL_IS_RESTORE_REBOOT,       /* ��ȡ��������NV�Զ��ָ���ɺ󵥰��Ƿ���Ҫ���� */
    SET_SMEM_SD_RESTORE_REBOOT_MAGIC,     /* ����SD����NV�Զ��ָ���ɺ󵥰�������־ */  
    SET_SMEM_SD_RESTORE_REBOOT_ZERO,      /* ����SD����NV�Զ��ָ���ɺ󵥰�������־���� */  
    GET_SMEM_SD_IS_RESTORE_REBOOT,        /* ��ȡSD����NV�Զ��ָ���ɺ󵥰��Ƿ���Ҫ���� */  
    SET_SMEM_FORCE_MODE_MAGIC,             /* ���ý�������ģʽ��־ */
    SET_SMEM_FORCE_MODE_ZERO,              /* ���ý�������ģʽ��־���� */  
    SET_SMEM_FORCE_SUCCESS_MAGIC,          /* ��������ģʽ�����ɹ���־ */  
    GET_SMEM_IS_FORCE_MODE,                 /* ��ȡ�Ƿ��������ģʽ */  
    SET_SMEM_EXCEP_REBOOT_INTO_ONL_MAGIC, /* �����쳣������ν�������������־ */
    SET_SMEM_EXCEP_REBOOT_INTO_ONL_ZERO,  /* �����쳣������ν�������������־���� */  
    GET_SMEM_IS_EXCEP_REBOOT_INTO_ONL,    /* ��ȡ�쳣��������Ƿ������������ */  
    SET_SMEM_UT_DLOAD_MAGIC,                /* ���ö�̨����������־ */  
    SET_SMEM_UT_DLOAD_ZERO,                 /* ���ö�̨����������־���� */  
    GET_SMEM_IS_UT_DLOAD,                   /* ��ȡ�Ƿ��Ƕ�̨�������� */ 
    SET_SMEM_MULTIUPG_DLOAD_MAGIC,         /* �����鲥������־ */
}smem_visit_t;

/*************************************************��ʾģ�鹫����Ϣ************************************************/
#define STR_MAX_LEN            (64)
#define PATH_MAX_LEN           (256)

/* ״̬����ָʾ */
typedef enum
{
    LED_READ,
    LED_WRITE,
    LCD_READ,
    LCD_WRITE,
}oper_mode_t;

/* ·����ʶ��ӳ��ṹ */
typedef struct
{
    uint32  path_index;
    char    path_content[PATH_MAX_LEN];
}path_info_t;

/* ���� lcd/led ����״̬���ݽṹ */
typedef struct
{
    uint32        up_state;
    oper_mode_t   oper_mode;
    union
    {
        uint32    path_index;
        int32     num_value;
        char      str_value[STR_MAX_LEN];
    };
    uint32        sleep_len;
}upstate_info_t;

#ifdef  __cplusplus
    }
#endif

#endif /* _DLOAD_COMM_H_ */
