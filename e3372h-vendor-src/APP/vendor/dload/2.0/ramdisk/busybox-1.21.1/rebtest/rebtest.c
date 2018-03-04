/******************************************************************************

                  ��Ȩ���� (C), 2013-2023, ��Ϊ�������޹�˾

 ******************************************************************************
  �� �� ��   : reb_test.c
  �� �� ��   : ����
  ��    ��   :  qiaoyichuan   00206465
  ��������   : 2013��11��15 ��
  ����޸�   :
  ��������   : �ɿ��Բ��Խӿ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2013��11��15 ��
    ��    ��   : qiaoyichuan   00206465
    �޸�����   : �����ļ�
    2013.11.28     q00206465       DTS2013112803803       ����MTDдFLASH�ӿ�
    2013.11.30     q00206465       DTS2013120200913       ����MTD��FLASH�ӿ�
    2013.12.2      q00206465       DTS2013120201310       ����MTD��ȡ������Ϣ�ӿ�   
    2013.12.2      q00206465       DTS2013120200993       ���������ͷ��ڴ�Ĳ��Խӿ�   
******************************************************************************/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <mtd/mtd-user.h>
#include <sys/param.h>

#include "libbb.h"

/******************************�ڲ��궨��***********************************/
/*�ɿ��Բ����豸·��*/
#define     REB_TEST_FILE   "/dev/reb_test"
#define     LOG_BUF_LEN   (100)

#define     FILE_NAME_LENGTH        (128)

#define     REB_TEST_ERROR               (-1)
#define     REB_TEST_OK                       (0)

#define     FLASH_TEST_NUMBER     (0X55)
#define     STRTOL_BASE_10             (10)

/*��������*/
#define     REB_TEST_CMD_PARAM_MIN    (2)
#define     REB_TEST_CMD_PARAM_3         (3)
#define     REB_TEST_CMD_PARAM_4         (4)
#define     REB_TEST_CMD_PARAM_5         (5)
/*BEGIN DTS2013120200913 00206465 qiaoyichuan 2013-12-2 added*/
#define     REB_TEST_CMD_PARAM_6         (6)
/*END DTS2013120200913 00206465 qiaoyichuan 2013-12-2 added*/

/*BEGIN DTS2013120200993  00206465 qiaoyichuan 2013-11-30 modified*/
/*IOCTL ��Ϣ����*/
#define     REB_MEM_DOUBLE_FREE          (0x0A)
#define     REB_MEM_ABNORMAL_FREE     (0x0B)
#define     REB_ABNORMAL_RESTART       (0x0C)
#define     REB_MEM_NORMAL_FREE         (0x0D)

#define     REB_MEM_DOUBLE_FREE_SET         (0)
#define     REB_MEM_ABNORMAL_FREE_SET   (1)
#define     REB_MEM_NORMAL_FREE_SET        (2)
/* END  DTS2013120200993  00206465 qiaoyichuan 2013-11-30 modified*/

/*�ɿ��Բ�������*/
#define     COMMAND_TYPE_FILE_SYSTEM_PROTECT  (0)
#define     COMMAND_TYPE_FAST_POWEROFF    (1)
#define     COMMAND_TYPE_MEM_FREE      (2)
#define     COMMAND_TYPE_POWEROFF       (3)
#define     COMMAND_TYPE_NV_TEST     (4)
#define     COMMAND_TYPE_FLASH_WRITE_BEYOND   (5)
#define     COMMAND_TYPE_FLASH_ERASE          (6)
#define     COMMAND_TYPE_ABNORMAL_RESTART       (7)
/*BEGIN DTS2013112803803 00206465 qiaoyichuan 2013-11-28 added*/
#define     COMMAND_TYPE_FLASH_WRITE   (8)
/*BEGIN DTS2013120200913 00206465 qiaoyichuan 2013-11-30 added*/
#define     COMMAND_TYPE_FLASH_READ     (9)
/*BEGIN DTS2013120201310 00206465 qiaoyichuan 2013-12-2 added*/
#define     COMMAND_TYPE_GET_MTDINFO    (10)
#define     COMMAND_TYPE_INVIABLE           (11)
/* END  DTS2013120201310 00206465 qiaoyichuan 2013-12-2 added*/
/* END  DTS2013120200913 00206465 qiaoyichuan 2013-11-30 added*/
/* END  DTS2013112803803 00206465 qiaoyichuan 2013-11-28 added*/
#define     REB_TWO     (2)

/****************************����ԭ������************************************/
void reb_printf(char *func_name);
int reb_test_file_system_protect(void);
int reb_fast_poweroff_protect(bool lock);
int reb_mem_free_test(int func);
void reb_poweroff_test(void);
int reb_nv_test( unsigned int start_block, unsigned int end_block, char *mtd_name);
int reb_erase_block(unsigned int start_block, unsigned int end_block, char *mtd_name );
int reb_flash_write_beyond_test(char *mtd_name);
int reb_abnormal_restart(void);
/*BEGIN DTS2013112803803 00206465 qiaoyichuan 2013-11-28 added*/
int reb_flash_write_test( unsigned int start_block,
                    unsigned int end_block, char *mtd_name);
/* END  DTS2013112803803 00206465 qiaoyichuan 2013-11-28 added*/
/*BEGIN DTS2013120200913 00206465 qiaoyichuan 2013-11-30 added*/
int reb_read_block(unsigned int start_block, 
    unsigned int end_block, char *mtd_name, char *filename);
/* END  DTS2013120200913 00206465 qiaoyichuan 2013-11-30 added*/
/*BEGIN DTS2013120201310 00206465 qiaoyichuan 2013-12-2 added*/
int reb_mtd_get_info(char *mtd_name);
/* END  DTS2013120201310 00206465 qiaoyichuan 2013-12-2 added*/
int rebtest_main(int argc, char *argv[]);

/*******************************����ʵ��**************************************/

/*****************************************************************************
 ��������  : int printf_usage(char *func_name)
 ��������  : ��ʾ�÷�
 �������  : func_name
 �������  : None
 �� �� ֵ  : None
 �޸���ʷ  :
             1. 2013-11-16 :  00206465 qiaoyichuan created
*****************************************************************************/
void reb_printf(char *func_name)
{
    printf("Usage:\n");
    printf("file system protect test:       --> ");
    printf("%s %d\n", func_name, COMMAND_TYPE_FILE_SYSTEM_PROTECT);
    printf("fast poweroff test: --> ");
    printf("%s %d vote\n",
           func_name, COMMAND_TYPE_FAST_POWEROFF);
    printf("mem free test :      --> ");
    printf("%s %d function\n", func_name, COMMAND_TYPE_MEM_FREE);
    printf("poweroff test:       --> ");
    printf("%s %d \n", func_name, COMMAND_TYPE_POWEROFF);
    printf("nv recovery test:          --> ");
    printf("%s %d start_block end_block mtd_name\n",
           func_name, COMMAND_TYPE_NV_TEST);
    printf("flash write beyond test:    --> ");
    printf("%s %d   mtd_name\n",
           func_name, COMMAND_TYPE_FLASH_WRITE_BEYOND);
    printf("erase some area test: --> ");
    printf("%s %d start_block end_block mtd_name \n",
           func_name, COMMAND_TYPE_FLASH_ERASE);
    printf("abnormal restart test: --> ");
    printf("%s %d\n", func_name, COMMAND_TYPE_ABNORMAL_RESTART);
    /*BEGIN DTS2013112803803 00206465 qiaoyichuan 2013-11-28 added*/
    printf("write mtd  test: --> ");
    printf("%s %d start_block end_block mtd_name \n",
           func_name, COMMAND_TYPE_FLASH_WRITE);
    /* END  DTS2013112803803 00206465 qiaoyichuan 2013-11-28 added*/
    /*BEGIN DTS2013120200913 00206465 qiaoyichuan 2013-12-2 added*/
    printf("read mtd test: --> ");
    printf("%s %d start_block end_block mtd_name filename \n",
           func_name, COMMAND_TYPE_FLASH_READ);
    /* END  DTS2013120200913 00206465 qiaoyichuan 2013-12-2 added*/
    /*BEGIN DTS2013120201310 00206465 qiaoyichuan 2013-12-2 added*/
    printf("Get a partition info: --> ");
    printf("%s %d mtd_name;\n", 
        func_name, COMMAND_TYPE_GET_MTDINFO);
    /* END  DTS2013120201310 00206465 qiaoyichuan 2013-12-2 added*/
    return ;
}

/*BEGIN DTS2013120201310 00206465 qiaoyichuan 2013-12-2 added*/
/*****************************************************************************
 ��������  : int reb_mtd_get_info(char *mtd_name)
 ��������  : ��ȡ������Ϣ
 �������  : mtd_name
 �������  : None
 �� �� ֵ  :REB_TEST_ERROR
                         REB_TEST_OK
 �޸���ʷ  : 
             1. 2013-12-2 :  00206465 qiaoyichuan created
*****************************************************************************/
int reb_mtd_get_info(char *mtd_name)
{
    char    mtdfile[FILE_NAME_LENGTH] = {0};
    int     mtdfd = -1;
    mtd_info_t  info = {0};
    int     ret = 0;

    if (NULL == mtd_name)
    {
        printf("mtd_get_info param error\n");
        return REB_TEST_ERROR;
    }
    
    strcpy(mtdfile, "/dev/");
    strcat(mtdfile, mtd_name);

    mtdfd = open(mtdfile, O_RDONLY);
    if (mtdfd < 0)
    {
        printf("open %s error\n", mtdfile);
        return REB_TEST_ERROR;
    }
    /*��ȡ������Ϣ*/
    ret = ioctl(mtdfd, MEMGETINFO, &info);
    if (0 != ret)
    {
        printf("get meminfo faild\n"); 
        close(mtdfd);
        return REB_TEST_ERROR;
    }

    close(mtdfd);
    /*��ӡ������Ϣ*/
    printf("%s info: \n", mtd_name);
    printf("mtdsize: %d, erasesize: %d, writesize: %d, oobsize: %d\n", 
        info.size, info.erasesize, info.writesize, info.oobsize);
    printf("block number: [0 -- %d]\n", info.size / info.erasesize - 1);

    return REB_TEST_OK;
}
/* END  DTS2013120201310 00206465 qiaoyichuan 2013-12-2 added*/
/*****************************************************************************
 ��������  : int reb_test_file_system_protect(void)
 ��������  : �������ļ�д��ʱ�򣬹ػ���������ʱ
                               �ļ�������������
 �������  : None
 �������  : None
 �� �� ֵ  :   REB_TEST_ERROR
                            REB_TEST_OK
 �޸���ʷ  :
             1. 2013-11-13 :  00206465 qiaoyichuan created
*****************************************************************************/
int reb_test_file_system_protect(void)
{

    FILE* fd = NULL;
    char logTmpBuf[LOG_BUF_LEN] = {0};

    memset((void*)logTmpBuf, '*', LOG_BUF_LEN);

    /*���ļ������ڣ�����*/
    if ( NULL == (fd = fopen("/data/rebtest", "ab+")))
    {
        printf("Reb_DFT open file error\n");
        return REB_TEST_ERROR;
    }
    /*���ļ���д���Ҳ��ر�*/
    if (strlen(logTmpBuf) != fwrite((const void *)logTmpBuf, 1,
                                    (size_t)strlen(logTmpBuf), (FILE *)fd))
    {
        printf("Reb_DFT write file error\n" );
        return REB_TEST_ERROR;
    }

    return REB_TEST_OK;
}

/*****************************************************************************
 ��������  : int reb_fast_poweroff_protect(bool lock)
 ��������  : �ڼٹػ�ʱ��ͶƱģ��ٹػ�ʧ��
 �������  : lock
 �������  : None
 �� �� ֵ  :   REB_TEST_ERROR
                             REB_TEST_OK
 �޸���ʷ  :
             1. 2013-11-13 :  00206465 qiaoyichuan created
*****************************************************************************/
int reb_fast_poweroff_protect(bool lock)
{
    /*���Ϊ1��Ͷ����Ʊ��Ϊ0 Ͷ�޳�Ʊ*/
    if (TRUE == lock)
    {
        system("echo reb_test > /sys/power/wake_lock");
        printf("Reb_DFT fast poweroff fail \n");
    }
    else if (FALSE == lock)
    {
        system("echo reb_test > /sys/power/wake_unlock");
        printf("Reb_DFT fast poweroff will success\n");
    }
    else
    {
        printf("Reb_DFT the argv is error\n");
        return REB_TEST_ERROR;
    }
    return REB_TEST_OK;
}

/*****************************************************************************
 ��������  : int reb_mem_free_test(int func)
 ��������  : �ظ��ͷ��ڴ�����ͷ��쳣��ַ���ڴ�
 �������  : func
 �������  : None
 �� �� ֵ  : No
 �޸���ʷ  :
             1. 2013-11-13 :  00206465 qiaoyichuan created
*****************************************************************************/
int reb_mem_free_test(int func)
{
    int fd = REB_TEST_ERROR;
    int ret = REB_TEST_OK;

    /*���ַ��豸*/
    fd = open(REB_TEST_FILE, O_RDWR);
    if (fd < 0)
    {
        printf("Reb_DFT open reb test moudle fail \n");
        return REB_TEST_ERROR;
    }
    switch (func)
    {
            /*�ظ��ͷ��ڴ�*/
        case REB_MEM_DOUBLE_FREE_SET:
        {
            ret = ioctl(fd, REB_MEM_DOUBLE_FREE, 0);
            if (0 != ret)
            {
                printf("Reb_DFT mem double free ioctl fail \n");
                close(fd);
                return REB_TEST_ERROR;
            }
            break;
        }
        /*�ͷ��쳣�ڴ�*/
        case REB_MEM_ABNORMAL_FREE_SET:
        {
            ret = ioctl(fd, REB_MEM_ABNORMAL_FREE, 0);
            if (0 != ret)
            {
                printf("Reb_DFT mem abnormal ioctl fail \n");
                close(fd);
                return REB_TEST_ERROR;
            }
            break;
        }
        /*BEGIN DTS2013120200993  00206465 qiaoyichuan 2013-11-30 added*/
        /*�����ͷ��ڴ�*/
        case REB_MEM_NORMAL_FREE_SET:
        {
            ret = ioctl(fd, REB_MEM_NORMAL_FREE, 0);
            if (0 != ret)
            {
                printf("Reb_DFT mem normal ioctl fail \n");
                close(fd);
                return REB_TEST_ERROR;
            }
            break;
        }
        /* END  DTS2013120200993  00206465 qiaoyichuan 2013-11-30 added*/

        default:
            break;
    }
    close(fd);
    return REB_TEST_OK;
}



/*****************************************************************************
 ��������  : int reb_poweroff_test(void)
 ��������  : �ػ��������쳣�������Ƿ��ܹػ�
 �������  : None
 �������  : None
 �� �� ֵ  : No
 �޸���ʷ  :
             1. 2013-11-13 :  00206465 qiaoyichuan created
*****************************************************************************/
void reb_poweroff_test(void)
{
    /*�ػ������У�ɾ��appvcom,ģ���޷��ػ�*/
    printf("Reb_DFT reb_poweroff_test\n");
    system("rm /dev/appvcom1");
    system("rm /dev/appvcom");
}

/*****************************************************************************
 ��������  : int reb_nv_test( unsigned int start_block,unsigned int end_block,char *mtd_name)
 ��������  : ���NV����ĳЩ��Ϊ���飬����NV�����ָ�����
 �������  : mtd_name  start_block end_block
 �������  : No
 �� �� ֵ  : REB_TEST_ERROR
                          REB_TEST_OK
 �޸���ʷ  :
             1. 2013-11-14 :  00206465 qiaoyichuan created
*****************************************************************************/
int reb_nv_test( unsigned int start_block, unsigned int end_block, char *mtd_name)
{
    char    mtdfile[FILE_NAME_LENGTH] = {0};
    int     mtdfd = REB_TEST_ERROR;
    int     ret = REB_TEST_OK;

    unsigned int    curblock = start_block;
    unsigned long long  curaddr = 0;
    mtd_info_t  info = {0};

    /*����ж�*/
    if (NULL == mtd_name || start_block > end_block)
    {
        printf("Reb_DFT mtd_mark_badblock param error\n");
        return REB_TEST_ERROR;
    }

    strcpy(mtdfile, "/dev/");
    strcat(mtdfile, mtd_name);

    /*�򿪶�Ӧ��MTD �豸*/
    mtdfd = open(mtdfile, O_RDONLY);
    if (mtdfd < 0)
    {
        printf("Reb_DFT open %s error\n", mtdfile);
        return REB_TEST_ERROR;
    }

    /*��ȡ��д������Ϣ*/
    ret = ioctl(mtdfd, MEMGETINFO, &info);
    if (0 != ret)
    {
        printf("Reb_DFT get meminfo faild\n");
        close(mtdfd);
        return REB_TEST_ERROR;
    }
    /* �ж��������block�Ƿ񳬳�������Χ */
    if (end_block * info.erasesize >= info.size)
    {
        printf("Reb_DFT block number %d over, max %d\n",
               end_block, info.size / info.erasesize - 1);
        return REB_TEST_ERROR;
    }

    /* ������Ŀ���Ϊ����*/
    while (curblock <= end_block)
    {
        curaddr = (unsigned long long)curblock * (unsigned long long)info.erasesize;
        ret = ioctl(mtdfd, MEMSETBADBLOCK, &curaddr);
        if (ret < 0)
        {
            printf("Reb_DFT GET MEMGETBADBLOCK error, offset: %lld\n", curaddr);
            close(mtdfd);
            return REB_TEST_ERROR;
        }

        curblock++;
    }

    return REB_TEST_OK;
}


/*****************************************************************************
 ��������  : int reb_erase_block(char *mtd_name, unsigned int start_block,
 ��������  :
 �������  : No
 �������  : No
 �� �� ֵ  : No
 �޸���ʷ  :
             1. 2013-11-15 :  00206465 qiaoyichuan created
*****************************************************************************/
int reb_erase_block(unsigned int start_block, unsigned int end_block, char *mtd_name )
{
    char    mtdfile[FILE_NAME_LENGTH] = {0};
    int     mtdfd = REB_TEST_ERROR;
    int     ret = REB_TEST_OK;

    unsigned int    curblock = start_block;
    unsigned long long  curaddr = 0;
    struct erase_info_user erase_info;
    mtd_info_t  info = {0};

    /*����ж�*/
    if (NULL == mtd_name || start_block > end_block)
    {
        printf("Reb_DFT mtd_erase_block param error\n");
        return REB_TEST_ERROR;
    }

    strcpy(mtdfile, "/dev/");
    strcat(mtdfile, mtd_name);

    /*�򿪶�Ӧ��MTD �豸*/
    mtdfd = open(mtdfile, O_RDWR);
    if (mtdfd < 0)
    {
        printf("Reb_DFT open %s error\n", mtdfile);
        return REB_TEST_ERROR;
    }

    /*��ȡ��д������Ϣ*/
    ret = ioctl(mtdfd, MEMGETINFO, &info);
    if (0 != ret)
    {
        printf("Reb_DFT get meminfo faild\n");
        close(mtdfd);
        return REB_TEST_ERROR;
    }

    /* �ж��������block�Ƿ񳬳�������Χ */
    if (end_block * info.erasesize >= info.size)
    {
        printf("Reb_DFT block number %d over, max %d\n",
               end_block, info.size / info.erasesize - 1);
        return REB_TEST_ERROR;
    }

    /* ������Ŀ����*/
    while (curblock <= end_block)
    {
        printf("Reb_DFT erase %d block \n", curblock);
        curaddr = (unsigned long long)curblock * (unsigned long long)info.erasesize;
        erase_info.start = curaddr;
        erase_info.length = info.erasesize;
        ret = ioctl(mtdfd, MEMERASE, &erase_info);
        if (ret < 0)
        {
            printf("Reb_DFT erase %d block error\n", curblock);
            close(mtdfd);
            return REB_TEST_ERROR;
        }
        curblock++;
    }

    return REB_TEST_OK;
}



/*****************************************************************************
 ��������  : int reb_flash_write_beyond_test( unsigned int start_block, unsigned int end_block,
                                 char *mtd_name, )
 ��������  : ָ������ָ��BLOCKָ��д
 �������  : start_block  end_block mtd_name
 �������  : No
 �� �� ֵ  : No
 �޸���ʷ  :
             1. 2013-11-15 :  00206465 qiaoyichuan created
*****************************************************************************/
int reb_flash_write_beyond_test(char *mtd_name)
{
    char    mtdfile[FILE_NAME_LENGTH] = {0};
    int     mtdfd = REB_TEST_ERROR;
    mtd_info_t  info = {0};
    int     ret = REB_TEST_OK;
    char    *databuf = NULL;
    int     blocksize = 0;
    int     end = 0;
    unsigned long long  curaddr = 0;
    unsigned int    curblock = 0;
    unsigned int    startblock = 0;
    struct erase_info_user erase_info;

    /*����ж�*/
    if (NULL == mtd_name)
    {
        printf("Reb_DFT mtd_write_block param error\n");
        return REB_TEST_ERROR;
    }

    strcpy(mtdfile, "/dev/");
    strcat(mtdfile, mtd_name);

    /*�򿪶�Ӧ��MTD �豸*/
    mtdfd = open(mtdfile, O_RDWR);
    if (mtdfd < 0)
    {
        printf("Reb_DFT open %s error\n", mtdfile);
        return REB_TEST_ERROR;
    }

    /*��ȡ��д������Ϣ*/
    ret = ioctl(mtdfd, MEMGETINFO, &info);
    if (0 != ret)
    {
        printf("Reb_DFT get meminfo faild\n");
        close(mtdfd);
        return REB_TEST_ERROR;
    }

    blocksize = info.erasesize;

    /* ����2��blocksize��С���ڴ� */
    databuf = malloc(REB_TWO * blocksize);
    if (NULL == databuf)
    {
        printf("Reb_DFT databuf malloc error\n");
        close(mtdfd);
        return REB_TEST_ERROR;
    }
    memset(databuf, FLASH_TEST_NUMBER, REB_TWO * blocksize);

    /* ������ʼ�ͽ�����block*/
    curblock = info.size / info.erasesize - 1;
    end = curblock + 1;

    startblock = curblock;
    /* д���ݵ�ָ����block��*/
    while (curblock != end)
    {
        /*�Ȳ���д*/
        curaddr = (unsigned long long)curblock * (unsigned long long)info.erasesize;
        erase_info.start = curaddr;
        erase_info.length = info.erasesize;
        ret = ioctl(mtdfd, MEMERASE, &erase_info);
        if (ret < 0)
        {
            /*����ʧ�ܣ����Ϊ����*/
            ret = ioctl(mtdfd, MEMSETBADBLOCK, &curaddr);
            if (ret < 0)
            {
                printf("GET MEMSETBADBLOCK error, offset: %lld\n", curaddr);
                free(databuf);
                close(mtdfd);
                return REB_TEST_ERROR;
            }
        }
        printf("Block %d Erasing...\n", curblock);
        curblock++;
    }

    /*�������д�������С������*/
    ret = pwrite(mtdfd, databuf, REB_TWO * blocksize, startblock * info.erasesize);
    if (ret != blocksize)
    {
        printf("Reb_DFT pwrite error\n");
        free(databuf);
        close(mtdfd);
        return REB_TEST_ERROR;
    }
    printf("Reb_DFT Block %d Write 0x%x\n", startblock, databuf[0]);

    free(databuf);
    close(mtdfd);

    return REB_TEST_OK;

}
/*BEGIN DTS2013112803803 00206465 qiaoyichuan 2013-11-28 added*/
/*****************************************************************************
 ��������  : int reb_flash_write_test( unsigned int start_block, unsigned int end_block,
                                 char *mtd_name, )
 ��������  : ָ������ָ��BLOCKָ��д
 �������  : start_block  end_block mtd_name
 �������  : No
 �� �� ֵ  : No
 �޸���ʷ  :
             1. 2013-11-15 :  00206465 qiaoyichuan created
*****************************************************************************/
int reb_flash_write_test( unsigned int start_block,
                    unsigned int end_block, char *mtd_name)
{
    char    mtdfile[FILE_NAME_LENGTH] = {0};
    int     mtdfd = REB_TEST_ERROR;
    mtd_info_t  info = {0};
    int     ret = REB_TEST_OK;
    char    *databuf = NULL;
    int     blocksize = 0;
    int     end = 0;
    unsigned long long  curaddr = 0;
    unsigned int    curblock = 0;
    struct erase_info_user erase_info;

    /*����ж�*/
    if (NULL == mtd_name)
    {
        printf("Reb_DFT mtd_write_block param error\n");
        return REB_TEST_ERROR;
    }

    strcpy(mtdfile, "/dev/");
    strcat(mtdfile, mtd_name);

    /*�򿪶�Ӧ��MTD �豸*/
    mtdfd = open(mtdfile, O_RDWR);
    if (mtdfd < 0)
    {
        printf("Reb_DFT open %s error\n", mtdfile);
        return REB_TEST_ERROR;
    }

    /*��ȡ��д������Ϣ*/
    ret = ioctl(mtdfd, MEMGETINFO, &info);
    if (0 != ret)
    {
        printf("Reb_DFT get meminfo faild\n");
        close(mtdfd);
        return REB_TEST_ERROR;
    }

    /* �ж��������block�Ƿ񳬳�������Χ */
    if (end_block * info.erasesize >= info.size)
    {
        printf("Reb_DFT block number %d over, max %d\n",
               end_block, info.size / info.erasesize - 1);
        return REB_TEST_ERROR;
    }

    blocksize = info.erasesize;

    /* ����1 ��blocksize��С���ڴ� */
    databuf = malloc( blocksize);
    if (NULL == databuf)
    {
        printf("Reb_DFT databuf malloc error\n");
        close(mtdfd);
        return REB_TEST_ERROR;
    }
    memset(databuf, FLASH_TEST_NUMBER,  blocksize);

    /* ������ʼ�ͽ�����block*/
    curblock = start_block;
    end = end_block + 1;

    /* д���ݵ�ָ����block��*/
    while (curblock != end)
    {
        /*�Ȳ���д*/
        curaddr = (unsigned long long)curblock * (unsigned long long)info.erasesize;
        erase_info.start = curaddr;
        erase_info.length = info.erasesize;
        ret = ioctl(mtdfd, MEMERASE, &erase_info);
        if (ret < 0)
        {
            /*����ʧ�ܣ����Ϊ����*/
            ret = ioctl(mtdfd, MEMSETBADBLOCK, &curaddr);
            if (ret < 0)
            {
                printf("GET MEMSETBADBLOCK error, offset: %lld\n", curaddr);
                free(databuf);
                close(mtdfd);
                return REB_TEST_ERROR;
            }
        }
        printf("Block %d Erasing...\n", curblock);

        /*�������дһ�����С������*/
        ret = pwrite(mtdfd, databuf, blocksize, curblock * info.erasesize);
        if (ret != blocksize)
        {
            printf("Reb_DFT pwrite error\n");
            free(databuf);
            close(mtdfd);
            return REB_TEST_ERROR;
        }
        printf("Reb_DFT Block %d Write 0x%x\n", curblock, databuf[0]);
        curblock++;
    }
    free(databuf);
    close(mtdfd);

    return REB_TEST_OK;
}
/* END  DTS2013112803803 00206465 qiaoyichuan 2013-11-28 added*/

/*****************************************************************************
 ��������  : void reb_abnormal_restart(void)
 ��������  :
 �������  : No
 �������  : No
 �� �� ֵ  : No
 �޸���ʷ  :
             1. 2013-11-19 :  00206465 qiaoyichuan created
*****************************************************************************/
int reb_abnormal_restart(void)
{
    int fd = REB_TEST_ERROR;
    int ret = REB_TEST_OK;

    /*���ַ��豸*/
    fd = open(REB_TEST_FILE, O_RDWR);
    if (fd < 0)
    {
        printf("Reb_DFT open reb test moudle fail \n");
        return REB_TEST_ERROR;
    }

    /*�·��쳣��������*/
    ret = ioctl(fd, REB_ABNORMAL_RESTART, 0);
    if (0 != ret)
    {
        printf("Reb_DFT abnormal restart ioctl fail \n");
        close(fd);
        return REB_TEST_ERROR;
    }
    close(fd);
    return REB_TEST_OK;
}

/*BEGIN DTS2013120200913 00206465 qiaoyichuan 2013-12-2 added*/
/*****************************************************************************
 ��������  : int reb_read_block(unsigned int start_block, unsigned int end_block,
                                        char *mtd_name, char *filename)
 ��������  : ��ȡFLASH���ݵ��ļ���
 �������  : start_block end_block  mtd_name  filename
 �������  : No
 �� �� ֵ  : REB_TEST_OK  REB_TEST_ERROR
 �޸���ʷ  : 
             1. 2013-11-29 :  00206465 qiaoyichuan created
*****************************************************************************/
int reb_read_block(unsigned int start_block, 
    unsigned int end_block, char *mtd_name, char *filename)
{
    char    mtdfile[FILE_NAME_LENGTH] = {0};
    int     mtdfd = -1;
    int     ret = 0;
    char    *databuf = NULL;
    int     blocksize = 0;
    FILE    *destfp = NULL;
    
    unsigned int    curblock = start_block;
    mtd_info_t  info = {0};

    /*����ж�*/
    if (NULL == mtd_name || NULL == filename
        || start_block > end_block)
    {
        printf("Reb_DFT mtd_read_block param error\n");
        return REB_TEST_ERROR;
    }

    strcpy(mtdfile, "/dev/");
    strcat(mtdfile, mtd_name);
    /*��MTD�豸*/
    mtdfd = open(mtdfile, O_RDONLY);
    if (mtdfd < 0)
    {
        printf("Reb_DFT open %s error\n", mtdfile);
        return REB_TEST_ERROR;
    }
    /*��ȡ������Ϣ*/
    ret = ioctl(mtdfd, MEMGETINFO, &info);
    if (0 != ret)
    {
        printf("Reb_DFT get meminfo faild\n"); 
        close(mtdfd);
        return REB_TEST_ERROR;
    }
    /*�ж��Ƿ񳬳�������Χ*/
    if (end_block * info.erasesize >= info.size)
    {
        printf("Reb_DFT block number %d over, max %d\n", 
            end_block, info.size / info.erasesize - 1);
        return REB_TEST_ERROR;
    }


    destfp = fopen(filename, "w");
    if (NULL == destfp)
    {
        printf("Reb_DFT fopen error\n");
        close(mtdfd);
        return REB_TEST_ERROR;
    }

    blocksize = info.erasesize;
    databuf = malloc(blocksize);
    if (NULL == databuf)
    {
        printf("Reb_DFT read block malloc error\n");
        close(mtdfd);
        fclose(destfp);
        return REB_TEST_ERROR;
    }
    /*��ȡFLASHĳЩblock�����ݵ�ָ���ļ���*/
    while(curblock <= end_block)
    {      
        /*lint -e119*/
        ret = pread(mtdfd, databuf, blocksize, curblock * info.erasesize);
        if (ret != blocksize)
        {
            printf("Reb_DFT read error\n");
            goto out;
        }
        /*lint +e119*/
        ret = fwrite(databuf, 1, blocksize, destfp);
        if (ret != blocksize)
        {
            printf("Reb_DFT fwrite error\n");
            goto out;
        }
        curblock++;
    }

out:
    free(databuf);
    close(mtdfd);
    fclose(destfp);

    return REB_TEST_OK;
}
/* END  DTS2013120200913 00206465 qiaoyichuan 2013-12-2 added*/
/*****************************************************************************
*Function:         rebtest_main
*
*Description:      flash test main function
*Input:            argc:
*                  argv:
*
*Output:           NA
*Return:           REB_TEST_ERROR:  failure
*                  REB_TEST_OK:     success
*
*Others:           REB_TEST_OK
*******************************************************************************/
int rebtest_main(int argc, char *argv[])
{
    int             ret = REB_TEST_OK;
    int             comtype = COMMAND_TYPE_INVIABLE;
    int             cmd_para = 0;
    int             cmd_para1 = 0;
    int             cmd_para2 = 0;

    /*��θ����ж�*/
    if (REB_TEST_CMD_PARAM_MIN > argc)
    {
        reb_printf(argv[0]);
        return REB_TEST_ERROR;
    }

    /*��δ���*/
    cmd_para = strtol(argv[1], NULL, STRTOL_BASE_10);
    if (LONG_MIN == cmd_para
            || LONG_MAX == cmd_para)
    {
        printf("Reb_DFT param %s error\n", argv[1]);
        return REB_TEST_ERROR;
    }
    comtype = cmd_para;

    if (REB_TEST_CMD_PARAM_3 == argc)
    {
        cmd_para1 = strtol(argv[REB_TEST_CMD_PARAM_MIN], NULL, STRTOL_BASE_10);
        if (LONG_MIN == cmd_para1 || LONG_MAX == cmd_para1)
        {
            printf("Reb_DFT param1 %s error\n", argv[REB_TEST_CMD_PARAM_MIN]);
            return REB_TEST_ERROR;
        }
    }
    else if (REB_TEST_CMD_PARAM_4 <= argc)
    {
        cmd_para1 = strtol(argv[REB_TEST_CMD_PARAM_MIN], NULL, STRTOL_BASE_10);
        if (LONG_MIN == cmd_para1 || LONG_MAX == cmd_para1)
        {
            printf("Reb_DFT param1 %s error\n", argv[REB_TEST_CMD_PARAM_MIN]);
            return REB_TEST_ERROR;
        }
        cmd_para2 = strtol(argv[REB_TEST_CMD_PARAM_3], NULL, STRTOL_BASE_10);
        if (LONG_MIN == cmd_para2 || LONG_MAX == cmd_para2)
        {
            printf("Reb_DFT param2 %s error\n", argv[REB_TEST_CMD_PARAM_3]);
            return REB_TEST_ERROR;
        }
    }
    else
    {
        //donothing
    }

    switch (comtype)
    {
            /*�ػ��ļ�ϵͳ����*/
        case (COMMAND_TYPE_FILE_SYSTEM_PROTECT):
        {
            if (argc != REB_TEST_CMD_PARAM_MIN)
            {
                reb_printf(argv[0]);
                return REB_TEST_ERROR;
            }
            ret = reb_test_file_system_protect();
        }
        break;

        /*�ٹػ�ʧ�ܽ�����ػ�*/
        case (COMMAND_TYPE_FAST_POWEROFF):
        {
            if (argc != REB_TEST_CMD_PARAM_3)
            {
                reb_printf(argv[0]);
                return REB_TEST_ERROR;
            }
            ret = reb_fast_poweroff_protect(cmd_para1);
        }
        break;

        /*�ڴ��ظ��ͷŻ��쳣�ͷ�*/
        case (COMMAND_TYPE_MEM_FREE):
        {
            if (argc != REB_TEST_CMD_PARAM_3)
            {
                reb_printf(argv[0]);
                return REB_TEST_ERROR;
            }
            ret = reb_mem_free_test(cmd_para1);
        }
        break;

        /* �ػ��ɿ��Բ���*/
        case (COMMAND_TYPE_POWEROFF):
        {
            if (argc != REB_TEST_CMD_PARAM_MIN)
            {
                reb_printf(argv[0]);
                return REB_TEST_ERROR;
            }
            reb_poweroff_test();
        }
        break;

        /* NV �ָ�����*/
        case (COMMAND_TYPE_NV_TEST):
        {
            if (argc != REB_TEST_CMD_PARAM_5)
            {
                reb_printf(argv[0]);
                return REB_TEST_ERROR;
            }
            ret = reb_nv_test(cmd_para1, cmd_para2, argv[REB_TEST_CMD_PARAM_4]);
        }
        break;

        /* ����дԽ�����*/
        case (COMMAND_TYPE_FLASH_WRITE_BEYOND):
        {
            if (argc != REB_TEST_CMD_PARAM_3)
            {
                reb_printf(argv[0]);
                return REB_TEST_ERROR;
            }
            ret = reb_flash_write_beyond_test(argv[REB_TEST_CMD_PARAM_MIN]);
        }
        break;

        /* ���������ӿ�*/
        case (COMMAND_TYPE_FLASH_ERASE):
        {
            if (argc != REB_TEST_CMD_PARAM_5)
            {
                reb_printf(argv[0]);
                return REB_TEST_ERROR;
            }
            ret = reb_erase_block( cmd_para1, cmd_para2, argv[REB_TEST_CMD_PARAM_4]);
        }
        break;

        /* �쳣��������*/
        case (COMMAND_TYPE_ABNORMAL_RESTART):
        {
            if (argc != REB_TEST_CMD_PARAM_MIN)
            {
                reb_printf(argv[0]);
                return REB_TEST_ERROR;
            }
            ret = reb_abnormal_restart();
        }
        break;

        /*BEGIN DTS2013112803803 00206465 qiaoyichuan 2013-11-28 added*/
        /* ����д�ӿ�*/
        case (COMMAND_TYPE_FLASH_WRITE):
        {
            if (argc != REB_TEST_CMD_PARAM_5)
            {
                reb_printf(argv[0]);
                return REB_TEST_ERROR;
            }
            ret = reb_flash_write_test( cmd_para1, cmd_para2, argv[REB_TEST_CMD_PARAM_4]);
        }
        break;
        /* END  DTS2013112803803 00206465 qiaoyichuan 2013-11-28 added*/
        /*BEGIN DTS2013120200913 00206465 qiaoyichuan 2013-12-2 added*/
        /* �������ӿ�*/
        case (COMMAND_TYPE_FLASH_READ):
        {
            if (argc != REB_TEST_CMD_PARAM_6)
            {
                reb_printf(argv[0]);
                return REB_TEST_ERROR;
            }
            ret = reb_read_block( cmd_para1, cmd_para2, 
                    argv[REB_TEST_CMD_PARAM_4],argv[REB_TEST_CMD_PARAM_5]);
        }
        break;
        /* END  DTS2013120200913 00206465 qiaoyichuan 2013-12-2 added*/

        /*BEGIN DTS2013120201310 00206465 qiaoyichuan 2013-12-2 added*/
        case (COMMAND_TYPE_GET_MTDINFO):
        {
            if (argc != REB_TEST_CMD_PARAM_3)
            {
                reb_printf(argv[0]);
                return REB_TEST_ERROR;
            }
            ret = reb_mtd_get_info(argv[REB_TEST_CMD_PARAM_MIN]);
        }
        break;
        /* END  DTS2013120201310 00206465 qiaoyichuan 2013-12-2 added*/

        default:
        {
            printf("Reb_DFT command type error\n");
            return REB_TEST_ERROR;
        }
    }

    if (0 != ret)
    {
        printf("Reb_DFT COMMAND EXECUTE ERROR\n");
    }

    return ret;

}

