/******************************************************************************

                  ��Ȩ���� (C), 2001-2014, ��Ϊ�������޹�˾

******************************************************************************
  �� �� ��   : dload_comm.h
  �� �� ��   : ����
  ��    ��   : w00183456
  ��������   : 2014��2��28��
  ����޸�   :
  ��������   : �������干�ñ���/��/�ṹ��
  �����б�   :
  �޸���ʷ   :
*********************************���ⵥ�޸ļ�¼******************************
  ���ⵥ��               �޸���         �޸�ʱ��           �޸�˵��                             
  DTS2014090106291      c00227249      2014-09-02         ͬ��������һ���޸�
                   
******************************************************************************/
#ifndef _DLOAD_COMM_H_
#define _DLOAD_COMM_H_
/******************************************************************************
  1 ����ͷ�ļ�����
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#include "dload_debug.h"

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
#define DLOAD_OK         0

#define DLOAD_NAME_MAX_LEN    (256)

/* ��ģ���ʼ������ָ������ */
typedef int32 (*module_interface)(void);

/****************************************************************************
 3 ö�����ͣ��ṹ�壬�궨��
 �ṹ�����ͼ� :_s
 ö�����ͼ�   :_e
 ���������ͼ� :_u
****************************************************************************/
typedef enum
{
    FILE_NONE           =        0x00,  /* No Programming file yet */
    FILE_PARTI          =        0x01,  /* Partition table */
    /* ��ͨƽ̨��������һ�����뿼���ú���� */
    FILE_SBL1          =        0x02,  /*SBL1*/
    FILE_WDT           =        0x03 , /*WDT*/
    FILE_TZ            =        0x04,  /*TZ*/
    FILE_RPM           =        0x05 , /*RPM*/
    FILE_EFS2          =        0x06,  /*EFS2 for arm*/
    FILE_EFS2_4K       =        0x1C,  /*EFS2 for arm,4K FLASH PAGE����*/
    FILE_EFS2_2K       =        0x1D,  /*EFS2 for arm,2K FLASH PAGE����*/
    FILE_MBA            =        0x07,  /*MBA*/
    FILE_QDSP           =        0x08,  /*QDSP*/
    FILE_BOOT           =        0x09,  /*APPBOOT include linux kernel*/
    FILE_ADSP           =        0X0F,  /* */
    FILE_MODEM_CPIO  =          0x1E,  /*modem.cpio*/
    /* Balong ƽ̨���� */
    FILE_M3BOOT        =        0x20,  /*m3boot*/
    
    FILE_VXWORK        =        0x22,  /*vxwork*/
    FILE_M3IMAGE       =        0x23,  /*m3_firmware*/
    FILE_DSP           =         0x24,  /*DSP*/
    FILE_NVDLOAD       =        0x25,  /*upgrade NV+XML*/
    FILE_NVIMG         =        0x26,  /*NV Work*/


    /* ISO �������ļ�����ֵ�����޸ģ��ǹ��ߴ�����׵� */
    FILE_ISO_HD        =        0x0A,  /*ISO header file type*/
    FILE_ISO           =        0x0B,  /*ISO file type :save to MMC Partiton*/

    /* ��0X10 ��ʼԤ��4��ID ��Ϊ��ͬƽ���ӷ����� */
    FILE_SYSTEM         =         0x10,  /* */
    FILE_FASTBOOT       =        0x11,  /*FASTBOOT: QC  APPSBL include fastboot*/
    FILE_RECOVERY       =        0x12,  /* */
    FILE_LOGO           =         0x13,  /* */
    FILE_RESERVE1       =        0x14,  /*Ԥ������ 1*/
    FILE_RESERVE2       =        0x15,  /*Ԥ������ 2*/
    FILE_RESERVE3       =        0x16,  /*Ԥ������ 3*/
    FILE_WIMAXCFG       =        0x17,  /* */
    FILE_WIMAXCRF       =        0x18,  /* */
    FILE_USERDATA       =        0x19,  /* */
    FILE_OEMINFO        =        0x1A,  /* */
    FILE_BBOU           =        0x1B,  /* */

    /* �����ļ������ǹ̶������޸ĵ� */
    FILE_NV_MBN           =      0x50,    /* */
    FILE_XML_MBN          =      0x51,    /* */
    FILE_CUST_MBN         =      0x52,    /* */
    FILE_PRL_NAM0         =      0x53,    /* */
    FILE_PRL_NAM1         =      0x54,    /* */
    FILE_WEBUI_HD         =      0X55,    /*WEBUI header file type*/
    FILE_WEBUI            =      0x56,    /* WEBUI configature file type*/
    FILE_WEBSDK           =      0x57,    /*WEBSDK code file type*/ 

    FILE_WIMAXCFG_CPIO   =      0x58,    /* WIMAXCFG CPIO, balong not used*/
    FILE_SYSTEM_CPIO     =      0x59,    /* SYSTEM CPIO */
    FILE_WEBSDK_CPIO     =      0x5A,    /* WEBSDK CPIO */
    FILE_WEBUI_CPIO      =      0x5B,    /* WEBUI CPIO */
    FILE_WIMAXCRF_CPIO   =      0x5C,    /* WIMAXCRF CPIO ,balong not used*/

    FILE_HOTA             =      0x5D,    /*HOTA update.zip packet file id*/  
    FILE_FIRMWARE1       =      0x5E,    /*ONLINE partition file id*/
    FILE_FOTA_FS         =      0x5F,    /*file system delta file id*/
    FILE_FOTA_FW         =      0x60,    /*firmware delta file id*/
    FILE_DISCARD         =      0xc0,    /*discard all files which file type >= FILE_DISCARD*/
    FILE_MAX
} file_type_e;

/*USB��������,PC->UE���ܵ�����*/
typedef enum
{
    DLOAD_CMD_ACK           = 0x02,    /*������Ӧ, UE->PC*/
    DLOAD_CMD_RESET         = 0x0A,    /*��λ����, PC->UE*/
    DLOAD_CMD_VERREQ        = 0x0C,    /*�����Ϣ��������, PC->UE*/
    DLOAD_CMD_VERRSP        = 0x0D,    /*�����Ϣ������Ӧ, UE->PC*/
    DLOAD_CMD_SWITCH        = 0x3A,    /*����ģʽ�л���������, UE<->PC*/
    DLOAD_CMD_INIT          = 0x41,    /*һ�����س�ʼ������, PC->UE*/
    DLOAD_CMD_DATA          = 0x42,    /*�������ݰ���������, PC->UE*/
    DLOAD_CMD_END           = 0x43,    /*����һ����������, PC->UE*/
    DLOAD_CMD_VERIFY        = 0x45,    /*��ƷID��ѯ�뷵������, UE<->PC*/
    DLOAD_CMD_TCPU_VER      = 0x46,    /*��������汾��ѯ�뷵������,��������ǿ�Ƽ���ģʽ,UE<->PC*/  
    DLOAD_CMD_BCPU_VER      = 0x4B,    /*��������汾��ѯ�뷵������,��������bootrom����ģʽ,UE<->PC*/  
    DLOAD_CMD_CDROM_STATUS  = 0xFB,    /*CDROM״̬��ѯ, ��������ǿ�Ƽ���ģʽ,UE<->PC*/
    DLOAD_CMD_BUF
}dload_cmd_e;

/*USB��������,UE->PC���͵�����*/
typedef enum
{
    DLOAD_RSP_ACK,                   /*������Ӧ*/
    /*�Ƿ���Ӧ, UE->PC*/
    DLOAD_RSP_NAK_INVALID_FCS,       /*CRCУ�����*/  
    DLOAD_RSP_NAK_INVALID_DEST,      /*Ŀ���ڴ�ռ䳬����Χ*/ 
    DLOAD_RSP_NAK_INVALID_LEN,       /*���յ�����Ԥ�õĳ��Ȳ�һ��*/ 
    DLOAD_RSP_NAK_EARLY_END,         /*���ݳ��ȹ���*/ 
    DLOAD_RSP_NAK_TOO_LARGE,         /*���ݳ��ȹ���*/ 
    DLOAD_RSP_NAK_INVALID_CMD,       /*��Ч������*/ 
    DLOAD_RSP_NAK_FAILED,            /*�����޷����*/ 
    DLOAD_RSP_NAK_WRONG_IID,         /*δʹ��*/ 
    DLOAD_RSP_NAK_BAD_VPP,           /*δʹ��*/ 
    DLOAD_RSP_NAK_OP_NOT_PERMITTED,  /*�����Բ�����ò���*/ 
    DLOAD_RSP_NAK_INVALID_ADDR,      /*�˵�ַ���ڴ��޷�����*/ 
    DLOAD_RSP_NAK_VERIFY_FAILED,     /*��Ȩʧ��,û��Ȩ�޲���*/ 
    DLOAD_RSP_NAK_NO_SEC_CODE,       /*û�а�ȫ��*/ 
    DLOAD_RSP_NAK_BAD_SEC_CODE,      /*��ȫ�������Ч */
    DLOAD_RSP_NAK_FILE_TOO_BIG,      /* Failure: the file to be down is larger than the section in flash */
    DLOAD_RSP_NAK_FLASH_OPER_FAIL    /* Failure: the flash operation required failed*/
}dload_rsp_type_e;

/*��������ö��ֵ*/
typedef  enum
{
    DLOAD_MMC_UPDATE = 0,           /*  SD������ */
    DLOAD_ONLINE_UPDATE,           /*  �������� */
    DLOAD_USB_UPDATE,               /*  һ������/��̨���� */
    DLOAD_UPDATE_MAX 
} dload_type_e;

/*״̬ö��ֵ*/
typedef enum
{
    IDLEUP                       = 10,
    QUERYING                     = 11,
    NEWVERSIONFOUND              = 12,
    DOWNLOAD_FAILED              = 20,
    DOWNLOAD_PROGRESSING         = 30,
    DOWNLOADPENDING              = 31,
    DOWNLOAD_COMPLETE            = 40,
    READY_TO_UPDATE              = 50,
    UPDATE_PROGRESSING           = 60,
    UPDATE_FAILED_HAVE_DATA      = 70,
    UPDATE_FAILED_NO_DATA        = 80,
    UPDATE_SUCCESSFUL_HAVE_DATA  = 90,
    UPDATE_SUCCESSFUL_NO_DATA    = 100,
    UPDATE_SIGN_VERIFY_FAIL      = 110,      /*��������ǩ����֤ʧ�� */
    UPDATE_MESSAGE_TYPE_COUNT
} update_mesg_type_e, update_status_e;

typedef enum
{
    NVNOTRESTORE = 0,            /*�����˿���̬*/
    NVRESTORE    = 0X444E5350,   /*NV�ָ��˿���̬ħ����*/
} dload_port_state_e;

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
    DLOAD_STATE_NORMAL_RUNNING,   /*����������*/
    DLOAD_STATE_AUTOUG_RUNNING,   /*����������*/
    DLOAD_STATE_NORMAL_LOW_POWER, /*SD��������ص͵�*/
    DLOAD_STATE_NORMAL_SUCCESS,   /*SD�������ɹ�*/
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
    SET_SMEM_UT_DLOAD_MAGIC,               /* ���ö�̨����������־ */  
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
