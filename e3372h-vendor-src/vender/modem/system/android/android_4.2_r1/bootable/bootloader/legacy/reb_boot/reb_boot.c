/******************************************************************************

                   ��Ȩ���� (C), 2013-2023,��Ϊ�ն����޹�˾

 ******************************************************************************
  �� �� ��   : reb_func.c
  �� �� ��   : ����
  ��    ��   : gongashi 00175336
  ��������   : 2013��11��08��
  ����޸�   :
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2013��11��08��
    ��    ��   : gongashi 00175336
    �޸�����   : �����ļ�

******************************************************************************/
/******************************���ⵥ�޸ļ�¼**********************************
    ����         �޸���         ���ⵥ��                 �޸�����
******************************************************************************/

/*----------------------------------------------*
 * ����ͷ�ļ�                                   *
 *----------------------------------------------*/
#include "types.h"
#include "reb_boot.h"
#include <bsp_nandc.h>
#include "ptable_com.h"
#include "time.h"
#include <bsp_nvim.h>

/*----------------------------------------------*
 * �궨��                                       *
 *----------------------------------------------*/
/*��Σ������\��Ϣ������*/
#define PART_WB_DATA "wbdata"

/*������I(���ݱ���)blk����*/
#define REB_DATA_BAK_BLK_SUM   (5)

/*������II(��д��Ϣ������blk����)*/
#define REB_INFO_BAK_BLK_SUM   (3)

/*ħ��,�����жϻ�д��Ϣ�Ƿ���Ч*/
#define OUR_MAGIC_NUM (0x19830609)

/*NV 50442 ID*/
#define   NV_ID_SOFT_RELIABLE_CFG  (50442)

/*----------------------------------------------*
 * ȫ�ֱ���˵��                                 *
 *----------------------------------------------*/
/*boot �׶ζ�ȡ��NV 50442��ֵ*/
SOFT_RELIABLE_CFG_STRU g_boot_soft_reliable_info = {0};
extern struct ST_PART_TBL ptable_product[];

/*----------------------------------------------*
 * ģ�鼶����                                   *
 *----------------------------------------------*/
typedef struct __nand_wb_info
{
    unsigned int magic;                 /*ħ����*/
    unsigned int sick_mtd_id;          /*��ǰ���д�Ĵ������id*/

    /*��ǰ���д�Ĵ������block id, ע����sick_mtd_id������blkƫ��*/
    unsigned int sick_blk_id;           
    
    /*���ݴ����block id-��wbdata������ʼ��ַ��ƫ��*/
    unsigned int backup_blk_id;         

    /*���ݻ�д��Ϣ��block id-��wbdata������ʼ��ַ��ƫ��*/
    unsigned int wb_info_blk_id;    
}nand_wb_info;


/*----------------------------------------------*
 * �ⲿ����ԭ������                             *
 *----------------------------------------------*/
extern int bsp_nand_isbad(const char *partition_name, u32 partition_offset);
extern int bsp_nand_write(const char *partition_name, u32 partition_offset, void* ptr_ram_addr, u32 length);

extern int bsp_get_nand_info(struct nand_spec *spec);
extern int bsp_nand_read(const char *partition_name, u32 partition_offset, void* ptr_ram_addr, u32 length, u32 *skip_len);

//extern unsigned int bsp_nvm_read(unsigned int itemid, unsigned char * pdata,
//                                        unsigned int datalen);
extern int bsp_nand_erase(const char *partition_name, u32 partition_offset);

/*----------------------------------------------*
 * �ڲ�����ԭ��˵��                             *
 *----------------------------------------------*/
/*****************************************************************************
 ��������  :  void reb_nv_boot_read()
 ��������  : ���ݲ�ͬƽ̨��ȡNV
 �������  : No
 �������  : No
 �� �� ֵ  : No
 �޸���ʷ  : 
             1. 2013-11-28 :  00206465 qiaoyichuan created
*****************************************************************************/
 void reb_nv_boot_read()
{
    int ret = 0;
#if ( YES == Reb_Platform_V7R2 )
    ret = bsp_nvm_read(NV_ID_SOFT_RELIABLE_CFG, (unsigned char *)&g_boot_soft_reliable_info,
                       sizeof(SOFT_RELIABLE_CFG_STRU));
#else
    /*����ƽ̨��չ*/
#endif

    if (0 != ret)
    {
        cprintf( "REB_DFT:boot get g_boot_soft_reliable_info fail\r\n" );
    }
}
/*************************************************************
 ��������  : reb_reliability_nv_cfg_boot_init
 ��������  : ��ȡnv 50442 ��ֵ����ȡ�ɿ���������Ϣ
 �������  : ��
 �������  : ��
 �� �� ֵ  :��
*************************************************************/
void  reb_reliability_nv_cfg_boot_init(void)
{
    reb_nv_boot_read();
}
/*****************************************************************************
 �� �� ��  : wb_info_valid_check
 ��������  : ����д��Ϣ����Ч��
 �������  : pt_wb_info-��д��Ϣ\offset-��д��Ϣ�ڷ����е�ƫ��λ��
             ������С\blk��С
 �������  : none
 �� �� ֵ  : 1-��Ч 0-��Ч
*****************************************************************************/
unsigned int wb_info_valid_check( nand_wb_info* pt_wb_info, unsigned int offset,
    unsigned int chip_size, unsigned int blk_size )
{
    unsigned int blk_sum = chip_size / blk_size;

    /*�鿴ħ�����Ƿ����*/
    if ( OUR_MAGIC_NUM != pt_wb_info->magic )
    {
        cprintf( "REB_DFT:Invalid write back info\r\n" );
        return 0;
    }

    if ( offset != pt_wb_info->wb_info_blk_id )
    {
        cprintf( "REB_DFT:Invalid write back info\r\n" );
        return 0;
    }

    /*�鿴����Ϣ�Ƿ���Ч*/
    if ( pt_wb_info->sick_blk_id >= blk_sum )
    {
        cprintf( "REB_DFT:Invalid sick blk info\r\n" );
        return 0;
    }

    /*�鿴����Ϣ�Ƿ���Ч*/
    if ( pt_wb_info->backup_blk_id >= blk_sum )
    {
        cprintf( "REB_DFT:Invalid back blk info\r\n" );
        return 0;
    }
#ifdef REB_BOOT_DEBUG
    /*lint -e515*/
    cprintf( "\r\nREB_DFT:correct write back info:\r\n" );
    cprintf( "REB_DFT:sick mtd is: %d\r\n", (int)pt_wb_info->sick_mtd_id );
    cprintf( "REB_DFT:sick blk is: %d\r\n", (int)pt_wb_info->sick_blk_id );
    cprintf( "REB_DFT:back blk is: %d\r\n", (int)pt_wb_info->backup_blk_id );
    cprintf( "REB_DFT:wb info blk is: %d\r\n", (int)pt_wb_info->wb_info_blk_id );
    /*lint +e515*/
#endif
    return 1;
}

/*****************************************************************************
 �� �� ��  : get_good_blk_offset
 ��������  : boot�׶β��Һÿ�ƫ����
 �������  : part_name-�������� blk_size-�����ߴ� 
 �������  : pt_blk_offset-�÷����е�һ���ÿ��ַ(�ӷ�����ʼ��ַ��ʼ)
 �� �� ֵ  : -1:û���ҵ� 0-�ҵ�
*****************************************************************************/
int get_good_blk_offset( char* part_name, unsigned int blk_size, unsigned int* pt_blk_offset )
{
    int ret_val = 0;
    unsigned int idx = 0;
    unsigned int offset_byte = 0; /*������ƫ�Ƶ�ַ*/

    if ( ( NULL == part_name ) || ( NULL == pt_blk_offset ) )
    {
        cprintf( "REB_DFT:No invalid blk fail\r\n" );
        return -1;
    }

    for ( idx = 0; idx < REB_INFO_BAK_BLK_SUM; idx++ )
    {
        offset_byte = ( idx + REB_DATA_BAK_BLK_SUM ) * blk_size;

        /*�鿴�Ƿ񻵿�,ע��ƫ����Ҫblk����*/
        ret_val = bsp_nand_isbad( PART_WB_DATA, offset_byte );
        if ( 0 == ret_val )
        {
            *pt_blk_offset = REB_DATA_BAK_BLK_SUM + idx;
            /*lint -e515*/
            cprintf( "REB_DFT:Find good blk:%d\r\n", (int)(* pt_blk_offset ) );
            /*lint +e515*/
            return 0; /*�ҵ��ÿ�,����*/
        }
    }

    /*�Ѿ�û�кÿ�,�˳�*/
    cprintf( "REB_DFT:No invalid blk fail\r\n" );
    return -1; /*���ش���*/

}

/*****************************************************************************
 �� �� ��  : reb_boot_wb_check
 ��������  : boot�׶μ���д��Ϣ
 �������  : ��������ַ
 �������  : none
 �� �� ֵ  : none
*****************************************************************************/
void reb_boot_wb_check(void)
{
    struct nand_spec nand_info = {0};
    unsigned int idx = 0;
    int ret_val = 0;
    unsigned int blk_offset = 0, byte_offset = 0;
    char* pt_page = NULL;
    nand_wb_info wb_info = {0};
    unsigned int read_and_write_sum = 0; 
    unsigned int sick_base_offset = 0;
    unsigned int bak_base_offset = 0;
    unsigned int alloc_mem_size = 0; /*pagesize or blocksize*/

    /*��ȡflash��Ϣ*/
    ret_val = bsp_get_nand_info( &nand_info );
    if ( 0 != ret_val )
    {
        cprintf( "REB_DFT:get nand info fail\r\n" );
        return;
    }

    /*��ȡ�����һ���ÿ�*/
    ret_val = get_good_blk_offset( PART_WB_DATA, nand_info.blocksize, &blk_offset );
    if ( ret_val < 0 )
    {
        cprintf( "REB_DFT:Maybe no good blk for wb info\r\n" );
        return;
    }

    alloc_mem_size = nand_info.blocksize;
    pt_page = (char *)alloc( alloc_mem_size );
    if ( NULL == pt_page )
    {
        /*û�����뵽�ڴ�128k*/
        /*lint -e515*/
        cprintf( "REB_DFT:No enough memory for %d\r\n", alloc_mem_size );
        /*lint +e515*/
        alloc_mem_size = nand_info.pagesize;
        pt_page = (char *)alloc( alloc_mem_size );
        if ( NULL == pt_page )
        {
            /*û�����뵽�ڴ�2k*/
            /*lint -e515*/
            cprintf( "REB_DFT:No enough memory for %d\r\n", alloc_mem_size );
            /*lint +e515*/
            return;
        }
    }


    /*��ȡ��д��Ϣ��pt_page*/
    byte_offset = blk_offset * nand_info.blocksize;
    ret_val = bsp_nand_read( PART_WB_DATA, byte_offset, pt_page, nand_info.pagesize, NULL );
    if ( 0 != ret_val )
    {
        /*��ȡ��д��Ϣʧ��*/
        cprintf( "REB_DFT:Read wb info fail\r\n" );
        goto free_and_ret;
    }
    /*lint -e18 -e119 -e419*/
    memcpy(&wb_info, pt_page, sizeof(nand_wb_info) );
    /*lint +e18 +e119 +e419*/
    /*����д��Ϣ����Ч��*/
    ret_val = wb_info_valid_check( &wb_info, blk_offset, nand_info.chipsize, nand_info.blocksize );
    if ( 0 == ret_val )
    {
        cprintf( "REB_DFT:No invalid wb info magic\r\n" );
        goto free_and_ret;
    }

    /*�Ȳ�����Ҫ��д�Ĳ�Σ��*/
    sick_base_offset = wb_info.sick_blk_id * nand_info.blocksize;
    ret_val = bsp_nand_erase( ptable_product[wb_info.sick_mtd_id].name, sick_base_offset );
    if ( 0 != ret_val )
    {
        cprintf( "REB_DFT:Oh God! erase sick blk err\r\n" );
        goto free_and_ret;
    }

    /*���ж�ȡ��������*/
    read_and_write_sum = ( unsigned int )nand_info.blocksize / alloc_mem_size;
    
    bak_base_offset  = wb_info.backup_blk_id * nand_info.blocksize;
    sick_base_offset = wb_info.sick_blk_id * nand_info.blocksize;
    for ( idx = 0; idx < read_and_write_sum; idx++ )
    {
        /*�����ݱ��ݿ��ȡ����*/
        /*lint -e119*/
        ret_val = bsp_nand_read( PART_WB_DATA, bak_base_offset,
                       pt_page, alloc_mem_size, NULL );
        /*lint +e119*/
        if ( 0 != ret_val )
        {
            /*lint -e515*/
            cprintf( "REB_DFT:Oh God! read bak blk err, idx=%d\r\n", (int)idx );
            /*lint +e515*/
            goto free_and_ret;
        }
        bak_base_offset += alloc_mem_size; /*pageдƫ���ۼ�*/

        /*����Σ���д����*/
        /*lint -e119*/
        ret_val = bsp_nand_write( ptable_product[wb_info.sick_mtd_id].name,
            sick_base_offset, pt_page, alloc_mem_size );
        /*lint +e119*/
        if ( 0 != ret_val )
        {
            /*lint -e515*/
            cprintf( "REB_DFT:Oh God! write sick blk err, idx=%d\r\n", (int)idx );
            /*lint +e515*/
            goto free_and_ret;
        }
        sick_base_offset += alloc_mem_size; /*pageдƫ���ۼ�*/
    }

    /*����wbdata������д��Ϣ���ڵĿ�*/
    bak_base_offset  = wb_info.wb_info_blk_id * nand_info.blocksize;
    ret_val = bsp_nand_erase( PART_WB_DATA, bak_base_offset );
    if ( 0 != ret_val )
    {
        /*todo:�껵��*/
        cprintf( "REB_DFT:Oh God! erase wb blk err\r\n" );
        goto free_and_ret;
    }

free_and_ret:
    /*boot�����ͷ��ڴ�*/
    return;
}
/*****************************************************************************
 �� �� ��  : reb_boot_init
 ��������  : boot�׶γ�ʼ��
 �������  : ��������ַ
 �������  : none
 �� �� ֵ  : none
*****************************************************************************/
void reb_boot_init(void)
{
    volatile unsigned int start = 0;
    volatile unsigned int end = 0;
    unsigned int delt = 0;

    reb_reliability_nv_cfg_boot_init();

    start = get_timer_value();
#ifdef REB_BOOT_DEBUG
    cprintf( "REB_DFT: check wb info begin......\r\n" );
#endif
    /*��boot�׶β�ʹ�ú����*/
    reb_boot_wb_check();

    end = get_timer_value();
    if ( start >= end )
    {
        delt = start - end; /*��ȡ����ʱ��tick*/
    }
    else
    {
        delt = end - start; /*��ȡ����ʱ��tick*/
    }
    delt = (delt * 1000) >> 15; /*timer clock is 32768 per second*/

#ifdef REB_BOOT_DEBUG
    /*lint -e515*/
    cprintf( "REB_DFT: it takes %dms, check wb info end...\r\n", (int)( delt ) );
    /*lint +e515*/
#endif
    return;
}
