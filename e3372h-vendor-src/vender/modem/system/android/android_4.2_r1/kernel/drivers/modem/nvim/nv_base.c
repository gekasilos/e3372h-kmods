 /*
 * Copyright (C) Huawei Technologies Co., Ltd. 2012-2015. All rights reserved.
 * foss@huawei.com
 *
 * If distributed as part of the Linux kernel, the following license terms
 * apply:
 *
 * * This program is free software; you can redistribute it and/or modify
 * * it under the terms of the GNU General Public License version 2 and 
 * * only version 2 as published by the Free Software Foundation.
 * *
 * * This program is distributed in the hope that it will be useful,
 * * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * * GNU General Public License for more details.
 * *
 * * You should have received a copy of the GNU General Public License
 * * along with this program; if not, write to the Free Software
 * * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA
 *
 * Otherwise, the following license terms apply:
 *
 * * Redistribution and use in source and binary forms, with or without
 * * modification, are permitted provided that the following conditions
 * * are met:
 * * 1) Redistributions of source code must retain the above copyright
 * *    notice, this list of conditions and the following disclaimer.
 * * 2) Redistributions in binary form must reproduce the above copyright
 * *    notice, this list of conditions and the following disclaimer in the
 * *    documentation and/or other materials provided with the distribution.
 * * 3) Neither the name of Huawei nor the names of its contributors may 
 * *    be used to endorse or promote products derived from this software 
 * *    without specific prior written permission.
 * 
 * * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*lint -save -e537*/
#include <linux/kthread.h>
#include <osl_thread.h>
#include <bsp_nvim.h>
#include "nv_comm.h"
#include "nv_file.h"
#include "nv_ctrl.h"
#include "nv_xml_dec.h"
#include "NVIM_ResumeId.h"
#include "mbb_leds.h"

#if(FEATURE_ON == MBB_CHG_BQ27510)
bool  nv_revert_flag = false;
extern ssize_t bq27510_coul_firmware_update_init(void);
#endif

/*lint -restore +e537*/


#if (FEATURE_ON == MBB_DLOAD)
#include <power_com.h>  /*fastboot���õ��쳣��Ϣ�ṹ��ر�ͷ�ļ�*/
#include "bsp_sram.h"
#if (FEATURE_ON == MBB_DLOAD_BBOU || FEATURE_ON == MBB_DLOAD_SDUP)

typedef enum
{
    NV_RESTRORE_SUCCESS,
    NV_RESTRORE_FAIL,
}NV_RESTORE_STATUS;
#endif /*MBB_DLOAD_BBOU || MBB_DLOAD_SDUP*/
bool nv_isSecListNv(u16 itemid)
{
    /*lint -save -e958*/
    u16 i = 0;

    /*lint -restore*/
    for(i = 0;i < bsp_nvm_getRevertNum(NV_SECURE_ITEM);i++)
    {
        if(itemid == g_ausNvResumeSecureIdList[i])
        {
            return true;
        }
    }
    return false;
}

#endif

/*lint -save -e713 -e830*/
u32 nv_readEx(u32 modem_id,u32 itemid,u32 offset,u8* pdata,u32 datalen)
{
    u32 ret = NV_ERROR;
    struct nv_file_list_info_stru file_info;
    struct nv_ref_data_info_stru  ref_info;

    nv_debug(NV_FUN_READ_EX,0,itemid,modem_id,datalen);

    if((NULL == pdata)||(0 == datalen))
    {
        nv_debug(NV_FUN_READ_EX,1,itemid,0,0);
        return BSP_ERR_NV_INVALID_PARAM;
    }

    ret = nv_search_byid(itemid,((u8*)NV_GLOBAL_CTRL_INFO_ADDR),&ref_info,&file_info);
    if(ret)
    {
        printf("\n[%s]:can not find 0x%x !\n",__FUNCTION__,itemid);
        return BSP_ERR_NV_NO_THIS_ID;
    }

    if((offset + datalen) > ref_info.nv_len)
    {
        ret = BSP_ERR_NV_ITEM_LEN_ERR;
        nv_debug(NV_FUN_READ_EX,3,offset,datalen,ref_info.nv_len);
        goto nv_readEx_err;
    }
    if(modem_id <= ref_info.modem_num)
    {
        ref_info.nv_off += (modem_id - NV_USIMM_CARD_1)*ref_info.nv_len;
    }
    else
    {
#if (FEATURE_OFF == FEATURE_MULTI_MODEM)
        ret = BSP_ERR_NV_INVALID_PARAM;
        nv_debug(NV_FUN_READ_EX,4,ret,itemid,modem_id);
        goto nv_readEx_err;
#endif
    }

    ret = nv_read_from_mem(pdata, datalen,file_info.file_id,(ref_info.nv_off+offset));
    if(ret)
    {
        nv_debug(NV_FUN_READ_EX,5,offset,datalen,ref_info.nv_len);
        goto nv_readEx_err;
    }
    /*lint -save -e578 -e530*/
    nv_debug_trace(pdata, datalen);
    /*lint -restore +e578 +e530*/

    return NV_OK;
nv_readEx_err:
    nv_mntn_record("\n[%s]:[0x%x]:[%d]\n",__FUNCTION__,itemid,modem_id);
    nv_help(NV_FUN_READ_EX);
    return ret;
}

u32 nv_writeEx(u32 modem_id,u32 itemid,u32 offset,u8* pdata,u32 datalen)
{
    u32 ret = NV_ERROR;
    struct nv_file_list_info_stru file_info;
    struct nv_ref_data_info_stru  ref_info;

    nv_debug(NV_FUN_WRITE_EX,0,itemid,modem_id,datalen);

    if((NULL == pdata)||(0 == datalen))
    {
        nv_debug(NV_FUN_WRITE_EX,1,itemid,datalen,0);
        return BSP_ERR_NV_INVALID_PARAM;
    }

    ret = nv_search_byid(itemid,((u8*)NV_GLOBAL_CTRL_INFO_ADDR),&ref_info,&file_info);
    if(ret)
    {

        printf("\n[%s]:can not find 0x%x !\n",__FUNCTION__,itemid);
        return BSP_ERR_NV_NO_THIS_ID;
    }

    /*lint -save -e578 -e530*/
    nv_debug_trace(pdata, datalen);
    /*lint -restore +e578 +e530*/

    if((datalen + offset) >ref_info.nv_len)
    {
        ret = BSP_ERR_NV_ITEM_LEN_ERR;
        nv_debug(NV_FUN_WRITE_EX,3,itemid,datalen,ref_info.nv_len);
        goto nv_writeEx_err;
    }
    if(modem_id <= ref_info.modem_num)
    {
        ref_info.nv_off += (modem_id - NV_USIMM_CARD_1)*ref_info.nv_len;
    }
    else
    {

#if (FEATURE_OFF == FEATURE_MULTI_MODEM)
        ret = BSP_ERR_NV_INVALID_PARAM;
        nv_debug(NV_FUN_WRITE_EX,4,itemid,ret,modem_id);
        goto nv_writeEx_err;
#endif
    }

    ret = nv_write_to_mem(pdata,datalen,file_info.file_id,ref_info.nv_off+offset);
    if(ret)
    {
        nv_debug(NV_FUN_WRITE_EX,5,itemid,datalen,0);
        goto nv_writeEx_err;
    }

    ret = nv_write_to_file(&ref_info);
    if(ret)
    {
        nv_debug(NV_FUN_WRITE_EX,6,itemid,datalen,ret);
        goto nv_writeEx_err;
    }
    nv_AddListNode(itemid);
    return NV_OK;
nv_writeEx_err:
    nv_mntn_record("\n[%s]:[0x%x]:[%d]\n",__FUNCTION__,itemid,modem_id);
    nv_help(NV_FUN_WRITE_EX);
    return ret;
}
/*
* Function   : bsp_nvm_get_nv_num
* Discription:
* Parameter  :
*
* Output     : return nv num
* History    : yuyangyang  00228784  create
*/
u32 bsp_nvm_get_nv_num(void)
{
    struct nv_ctrl_file_info_stru* ctrl_info = (struct nv_ctrl_file_info_stru*)NV_GLOBAL_CTRL_INFO_ADDR;
    return ctrl_info->ref_count;
}
/*
* Function   : bsp_nvm_get_nvidlist
* Discription:
* Parameter  : nvlist : hims parmeter
*
* Output     : return result
* History    : yuyangyang  00228784  create
*/
u32 bsp_nvm_get_nvidlist(NV_LIST_INFO_STRU*  nvlist)
{
    u32 i;
    struct nv_ctrl_file_info_stru* ctrl_info = (struct nv_ctrl_file_info_stru*)NV_GLOBAL_CTRL_INFO_ADDR;
    struct nv_ref_data_info_stru* ref_info   = (struct nv_ref_data_info_stru*)(NV_GLOBAL_CTRL_INFO_ADDR+NV_GLOBAL_CTRL_INFO_SIZE\
        +NV_GLOBAL_FILE_ELEMENT_SIZE*ctrl_info->file_num);

    if(NULL == nvlist)
    {
        return NV_ERROR;
    }

    for(i = 0;i<ctrl_info->ref_count;i++)
    {
        nvlist[i].usNvId       = ref_info[i].itemid;
        nvlist[i].ucNvModemNum = ref_info[i].modem_num;
    }
    return NV_OK;
}
/*
* Function   : bsp_nvm_get_len
* Discription: get nv len api
* Parameter  : itemid: nv id
*              [out]len : nv len
* Output     : read result
* History    : yuyangyang  00228784  create
*/
u32 bsp_nvm_get_len(u32 itemid,u32* len)
{
    u32 ret  = NV_ERROR;
    struct nv_ref_data_info_stru ref_info = {0};
    struct nv_file_list_info_stru file_info = {0};

    nv_debug(NV_API_GETLEN,0,itemid,0,0);
    if(NULL == len)
    {
        nv_debug(NV_API_GETLEN,1,itemid,0,0);
        return BSP_ERR_NV_INVALID_PARAM;
    }

    /*check init state*/
    if(false == nv_read_right(itemid))
    {
        nv_debug(NV_API_GETLEN,3,itemid,0,0);
        return BSP_ERR_NV_MEM_INIT_FAIL;
    }
    ret = nv_search_byid(itemid,(u8*)NV_GLOBAL_CTRL_INFO_ADDR,&ref_info, &file_info);
    if(NV_OK == ret)
    {
        *len = ref_info.nv_len;
        return NV_OK;
    }
    return ret;
}
/*
* Function   : bsp_nvm_authgetlen
* Discription: get nv len auth api
* Parameter  : itemid: nv id
*              [out]len : nv len
* Output     : read result
* History    : yuyangyang  00228784  create
*/
u32 bsp_nvm_authgetlen(u32 itemid,u32* len)
{
    return bsp_nvm_get_len(itemid,len);
}


/*
* Function   : bsp_nv_dcread_direct
* Discription: double card nv read api
* Parameter  : itemid: nv id
*              pdata:  user buff
*              datalen: nv length
* Output     : read result
* History    : yuyangyang  00228784  create
*/
u32 bsp_nvm_dcread_direct(u32 modem_id,u32 itemid, u8* pdata,u32 datalen)
{
    return nv_readEx(modem_id,itemid,0,pdata,datalen);
}
/*
* Function   : bsp_nvm_dcread
* Discription: double card nv read api
* Parameter  : itemid: nv id
*              pdata:  user buff
*              datalen: nv length
* Output     : read result
* History    : yuyangyang  00228784  create
*/
u32 bsp_nvm_dcread(u32 modem_id,u32 itemid, u8* pdata,u32 datalen)
{
    /*check init state*/
    if(false == nv_read_right(itemid))
    {
        return BSP_ERR_NV_MEM_INIT_FAIL;
    }

    return nv_readEx(modem_id,itemid,0,pdata,datalen);
}
/*
* Function   : bsp_nvm_auth_dcread
* Discription: nv authication read api
* Parameter  : itemid: nv id
*              pdata:  user buff
*              datalen: nv length
* Output     : read result
* History    : yuyangyang  00228784  create
*/
u32 bsp_nvm_auth_dcread(u32 modem_id,u32 itemid, u8* pdata,u32 datalen)
{
    return bsp_nvm_dcread(modem_id,itemid,pdata,datalen);
}
/*
* Function   : bsp_nvm_dcreadpart
* Discription: double card nv read part api
* Parameter  : itemid: nv id
*              offset: offset to read
*              pdata:  user buff
*              datalen: nv length
* Output     : read result
* History    : yuyangyang  00228784  create
*/
u32 bsp_nvm_dcreadpart(u32 modem_id,u32 itemid,u32 offset,u8* pdata,u32 datalen)
{
    /*check init state*/
    if(false == nv_read_right(itemid))
    {
        return BSP_ERR_NV_MEM_INIT_FAIL;
    }

    return nv_readEx(modem_id,itemid,offset,pdata,datalen);
}
/*
* Function   : bsp_nvm_dcwrite
* Discription: double card nv write api
* Parameter  : itemid: nv id
*              pdata:  user buff
*              datalen: nv length
* Output     : write result
* History    : yuyangyang  00228784  create
*/
u32 bsp_nvm_dcwrite(u32 modem_id,u32 itemid, u8* pdata,u32 datalen)
{
    if(false == nv_write_right(itemid))
    {
        return BSP_ERR_NV_MEM_INIT_FAIL;
    }

    return nv_writeEx(modem_id,itemid,0,pdata,datalen);
}
/*
* Function   : bsp_nvm_auth_dcwrite
* Discription: double card nv authication write api
* Parameter  : itemid: nv id
*              pdata:  user buff
*              datalen: nv length
* Output     : read result
* History    : yuyangyang  00228784  create
*/
u32 bsp_nvm_auth_dcwrite(u32 modem_id,u32 itemid, u8* pdata,u32 datalen)
{
    return bsp_nvm_dcwrite(modem_id,itemid,pdata,datalen);
}
/*
* Function   : bsp_nvm_dcwritepart
* Discription: nv write part api
* Parameter  : itemid: nv id
*              offset: offset to write
*              pdata:  user buff
*              datalen: nv length
* Output     : write result
* History    : yuyangyang  00228784  create
*/
u32 bsp_nvm_dcwritepart(u32 modem_id,u32 itemid, u32 offset,u8* pdata,u32 datalen)
{
    if(false == nv_write_right(itemid))
    {
        return BSP_ERR_NV_MEM_INIT_FAIL;
    }

    return nv_writeEx(modem_id,itemid,offset,pdata,datalen);
}
/*
* Function   : bsp_nvm_dcwrite_direct
* Discription: nv write api
* Parameter  : itemid: nv id
*              pdata:  user buff
*              datalen: nv length
* Output     : write result
* History    : yuyangyang  00228784  create
*/
u32 bsp_nvm_dcwrite_direct(u32 modem_id,u32 itemid, u8* pdata,u32 datalen)
{
    return nv_writeEx(modem_id,itemid,0,pdata,datalen);
}


/*
* Function   : bsp_nvm_flushEx
* Discription: flush the memory to file/flash/rfile
* Parameter  : off :write data offset len:write len
*              itemid: item id,invalid :0xffffffff
* Output     : result
* History    : yuyangyang  00228784  create
*/
/*lint -save -e529*/
u32 bsp_nvm_flushEx(u32 off,u32 len,u32 itemid)
{
    u32 ret = NV_ERROR;
    FILE* fp = NULL;
    struct nv_global_ddr_info_stru* ddr_info = (struct nv_global_ddr_info_stru*)NV_GLOBAL_INFO_ADDR;

    nv_debug(NV_API_FLUSH,0,0,0,0);
    if(nv_file_access((s8*)NV_IMG_PATH,0))
    {
        fp = nv_file_open((s8*)NV_IMG_PATH,(s8*)NV_FILE_WRITE);
        off = 0;
        len = ddr_info->file_len;
    }
    else
    {
        fp = nv_file_open((s8*)NV_IMG_PATH,(s8*)NV_FILE_RW);
    }
    if(NULL == fp)
    {
        ret = BSP_ERR_NV_NO_FILE;
        nv_debug(NV_API_FLUSH,1,ret,0,0);
        goto nv_flush_err;
    }
    if((off+len) > (ddr_info->file_len))
    {
        off = 0;
        len = ddr_info->file_len;
    }

    (void)nv_file_seek(fp,(s32)off,SEEK_SET);/*jump to write*/

    ret = (u32)nv_file_write((u8*)NV_GLOBAL_CTRL_INFO_ADDR+off,1,len,fp);

    nv_file_close(fp);
    if(ret != len)
    {
        nv_debug(NV_API_FLUSH,2,0,ret,len);
        ret = BSP_ERR_NV_WRITE_FILE_FAIL;
        goto nv_flush_err;
    }

    return NV_OK;

nv_flush_err:
    nv_mntn_record("\n[%s] len :0x%x, off :0x%x\n",__FUNCTION__,len,off);
    nv_help(NV_API_FLUSH);
    return ret;
}
/*lint -restore +e529*/
/*
* Function   : bsp_nvm_flush
* Discription: flush the memory to file/flash/rfile
* Parameter  : none
* Output     : result
* History    : yuyangyang  00228784  create
*/
u32 bsp_nvm_flush(void)
{
    u32 ret;
    struct nv_global_ddr_info_stru* ddr_info = (struct nv_global_ddr_info_stru*)NV_GLOBAL_INFO_ADDR;
    struct nv_ctrl_file_info_stru* ctrl_info = (struct nv_ctrl_file_info_stru*)NV_GLOBAL_CTRL_INFO_ADDR;

    ret = bsp_nvm_flushEx(ctrl_info->ctrl_size,(ddr_info->file_len-ctrl_info->ctrl_size),NV_ERROR);
    if(ret)
    {
        return ret;
    }
    memset(ddr_info->priority,0,sizeof(ddr_info->priority));/*clear*/
    nv_flush_cache((void*)NV_GLOBAL_INFO_ADDR, (u32)NV_GLOBAL_INFO_SIZE);
    return NV_OK;
}

u32 bsp_nvm_flushEn(void)
{
    u32 ret = NV_ERROR;
    FILE* fp = NULL;
    struct nv_global_ddr_info_stru* ddr_info = (struct nv_global_ddr_info_stru*)NV_GLOBAL_INFO_ADDR;


    nv_create_flag_file((s8*)NV_IMG_FLAG_PATH);
    nv_debug(NV_API_FLUSH,0,0,0,0);
    fp = nv_file_open((s8*)NV_IMG_PATH,(s8*)NV_FILE_WRITE);
    if(NULL == fp)
    {
        ret = BSP_ERR_NV_NO_FILE;
        nv_debug(NV_API_FLUSH,1,ret,0,0);
        goto nv_flush_err;
    }

    ret = (u32)nv_file_write((u8*)NV_GLOBAL_CTRL_INFO_ADDR,1,ddr_info->file_len,fp);
    nv_file_close(fp);
    fp = NULL;
    if(ret != ddr_info->file_len)
    {
        nv_debug(NV_API_FLUSH,2,(u32)fp,ret,ddr_info->file_len);
        ret = BSP_ERR_NV_WRITE_FILE_FAIL;
        goto nv_flush_err;
    }
    nv_delete_flag_file((s8*)NV_IMG_FLAG_PATH);

    return NV_OK;

nv_flush_err:
    nv_mntn_record("\n[%s]\n",__FUNCTION__);
    nv_help(NV_API_FLUSH);
    return ret;
}

/*
* Function   : bsp_nvm_flushSys
* Discription: flush the memory to sys sec for system use
* Parameter  : none
* Output     : result
* History    : yuyangyang  00228784  create
*/
u32 bsp_nvm_flushSys(u32 itemid)
{
    u32 ret = NV_ERROR;
    FILE* fp = NULL;
    u32 ulTotalLen = 0;
    struct nv_global_ddr_info_stru* ddr_info = (struct nv_global_ddr_info_stru*)NV_GLOBAL_INFO_ADDR;

    nv_create_flag_file((s8*)NV_SYS_FLAG_PATH);

    nv_debug(NV_FUN_FLUSH_SYS,0,0,0,0);
    if(nv_file_access((s8*)NV_FILE_SYS_NV_PATH,0))
    {
        fp = nv_file_open((s8*)NV_FILE_SYS_NV_PATH,(s8*)NV_FILE_WRITE);
    }
    else
    {
        fp = nv_file_open((s8*)NV_FILE_SYS_NV_PATH,(s8*)NV_FILE_RW);
    }
    if(NULL == fp)
    {
        nv_debug(NV_FUN_FLUSH_SYS,1,ret,0,0);
        ret = BSP_ERR_NV_NO_FILE;
        goto nv_flush_err;
    }
    ulTotalLen = ddr_info->file_len;
#if defined(FEATURE_NV_FLASH_ON)
/*BEGIN DTS2014040404831 g00175336 2014-04-03 added*/
    /*��nvdload�����ļ�ĩβ�ñ�־0xabcd8765*/
    *( unsigned int* )( (u8*)NV_GLOBAL_CTRL_INFO_ADDR + ddr_info->file_len )
        = ( unsigned int )NV_FILE_TAIL_MAGIC_NUM;
    ulTotalLen += sizeof(unsigned int);
/*END DTS2014040404831 g00175336 2014-04-03 added*/
#endif
    ret = (u32)nv_file_write((u8*)NV_GLOBAL_CTRL_INFO_ADDR,1,ulTotalLen,fp);
    nv_file_close(fp);
    if(ret != ulTotalLen)
    {
        nv_debug(NV_FUN_FLUSH_SYS,3,ret,ulTotalLen,0);
        ret = BSP_ERR_NV_WRITE_FILE_FAIL;
        goto nv_flush_err;
    }

    nv_delete_flag_file((s8*)NV_SYS_FLAG_PATH);
    return NV_OK;

nv_flush_err:
    nv_mntn_record("\n[%s]\n",__func__);
    nv_help(NV_FUN_FLUSH_SYS);
    return ret;
}


/*
* Function   : bsp_nvm_backup
* Discription: backup the memory to file/flash/rfile
* Parameter  : none
* Output     : result
* History    : yuyangyang  00228784  create
*/
u32 bsp_nvm_backup(void)
{
    u32 ret = NV_ERROR;
    struct nv_global_ddr_info_stru* ddr_info = (struct nv_global_ddr_info_stru*)NV_GLOBAL_INFO_ADDR;
    FILE* fp = NULL;

    nv_debug(NV_API_BACKUP,0,0,0,0);

    if( (ddr_info->acore_init_state != NV_INIT_OK)&&
        (ddr_info->acore_init_state != NV_KERNEL_INIT_DOING))
    {
        return NV_ERROR;
    }

    nv_create_flag_file((s8*)NV_BACK_FLAG_PATH);

    if(nv_file_access((s8*)NV_BACK_PATH,0))
    {
        fp = nv_file_open((s8*)NV_BACK_PATH,(s8*)NV_FILE_WRITE);
    }
    else
    {
        fp = nv_file_open((s8*)NV_BACK_PATH,(s8*)NV_FILE_RW);
    }
    if(NULL == fp)
    {
        ret = BSP_ERR_NV_NO_FILE;
        nv_debug(NV_API_BACKUP,1,ret,0,0);
        goto nv_backup_fail;
    }


    ret = (u32)nv_file_write((u8*)NV_GLOBAL_CTRL_INFO_ADDR,1,ddr_info->file_len,fp);
    nv_file_close(fp);
    fp = NULL;
    if(ret != ddr_info->file_len)
    {
        nv_debug(NV_API_BACKUP,3,ret,ddr_info->file_len,0);
        ret = BSP_ERR_NV_WRITE_FILE_FAIL;
        goto nv_backup_fail;
    }

    nv_delete_flag_file((s8*)NV_BACK_FLAG_PATH);

    return NV_OK;
nv_backup_fail:
    nv_mntn_record("\n[%s]\n",__FUNCTION__);
    nv_help(NV_API_BACKUP);
    return ret;

}
/*
* Function   : bsp_nvm_revert_user
* Discription: revert all nv belong to user need to revert(revert one by one)
* Parameter  : none
* Output     : result
* History    : yuyangyang  00228784  create
*/
u32 bsp_nvm_revert_user(void)
{
    return nv_revert_data(NV_BACK_PATH,g_ausNvResumeUserIdList,\
        bsp_nvm_getRevertNum(NV_USER_ITEM));
}

/*
* Function   : bsp_nvm_revert_manufacture
* Discription: revert factory default edition,revert one by one
* Parameter  : none
* Output     : result
* History    : yuyangyang  00228784  create
*/
u32 bsp_nvm_revert_manufacture(void)
{
    return nv_revert_data(NV_BACK_PATH,g_ausNvResumeManufactureIdList,\
        bsp_nvm_getRevertNum(NV_MANUFACTURE_ITEM));
}
/*
* Function   : bsp_nvm_revert_secure
* Discription: revert factory default edition,revert one by one
* Parameter  : none
* Output     : result
* History    : yuyangyang  00228784  create
*/
u32 bsp_nvm_revert_secure(void)
{
    return nv_revert_data(NV_BACK_PATH,g_ausNvResumeSecureIdList,\
        bsp_nvm_getRevertNum(NV_SECURE_ITEM));
}
/*
* Function   : bsp_nvm_revert
* Discription: revert all nv need to revert(revert one by one)
* Parameter  : none
* Output     : result
* History    : yuyangyang  00228784  create
*/
u32 bsp_nvm_revert(void)
{
    u32 ret  = NV_ERROR;

    nv_debug(NV_API_REVERT,0,0,0,0);

    nv_printf("enter to revert nv !\n");
    ret = bsp_nvm_revert_user();
    if(ret)
    {
        nv_debug(NV_API_REVERT,1,ret,0,0);
        goto nv_revert_fail;
    }

    ret = bsp_nvm_revert_manufacture();
    if(ret)
    {
        nv_debug(NV_API_REVERT,2,ret,0,0);
        goto nv_revert_fail;
    }
    ret = bsp_nvm_revert_secure();
    if(ret)
    {
        nv_debug(NV_API_REVERT,3,ret,0,0);
        goto nv_revert_fail;
    }
    nv_printf("revert nv end !\n");

    return NV_OK;
nv_revert_fail:
    nv_mntn_record("\n[%s]\n",__FUNCTION__);
    nv_help(NV_API_REVERT);
    return ret;
}



/*
* Function   : bsp_nvm_update_default
* Discription: update mem data to default section
* Parameter  : none
* Output     : result
* History    : yuyangyang  00228784  create
*/
u32 bsp_nvm_update_default(void)
{
    u32 ret = NV_ERROR;
    FILE* fp = NULL;
    u32 datalen = 0;
    struct nv_global_ddr_info_stru* ddr_info = (struct nv_global_ddr_info_stru*)NV_GLOBAL_INFO_ADDR;

    nv_debug(NV_FUN_UPDATE_DEFAULT,0,0,0,0);

    if(ddr_info->acore_init_state != NV_INIT_OK)
    {
        return NV_ERROR;
    }

#if defined(BSP_CONFIG_HI3630)
    if(false == nv_check_update_default_right())
    {
        nv_printf("no right to write factory data!\n");
    }
#endif

    if(nv_file_access((s8*)NV_DEFAULT_PATH,0))
    {
        fp = nv_file_open((s8*)NV_DEFAULT_PATH,(s8*)NV_FILE_WRITE);
    }
    else
    {
        fp = nv_file_open((s8*)NV_DEFAULT_PATH,(s8*)NV_FILE_RW);
    }
    if(NULL == fp)
    {
        ret = BSP_ERR_NV_NO_FILE;
        nv_debug(NV_FUN_UPDATE_DEFAULT,2,ret,0,0);
        goto nv_update_default_err;
    }

    datalen = (u32)nv_file_write((u8*)NV_GLOBAL_CTRL_INFO_ADDR,1,ddr_info->file_len,fp);

    nv_file_close(fp);
    if(datalen != ddr_info->file_len)
    {
        nv_debug(NV_FUN_UPDATE_DEFAULT,3,ret,ddr_info->file_len,0);
        ret = BSP_ERR_NV_WRITE_FILE_FAIL;
        goto nv_update_default_err;
    }

    ret = bsp_nvm_backup();
    if(ret)
    {
        nv_debug(NV_FUN_UPDATE_DEFAULT,4,ret,0,0);
        goto nv_update_default_err;
    }

#if defined(BSP_CONFIG_HI3630)
    ret = (u32)sc_file_backup();
    if(ret)
    {
        nv_mntn_record("sec file backup failed !\n");
    }
    nv_delete_update_default_right();
#endif

    return NV_OK;
nv_update_default_err:
    nv_mntn_record("\n[%s]\n",__FUNCTION__);
    nv_help(NV_FUN_UPDATE_DEFAULT);
    return ret;
}

/*
* Function   : bsp_nvm_remove_dload_packet
* Discription:
* Parameter  : none
* Output     : result
* History    : yuyangyang  00228784  create
*/
void bsp_nvm_remove_dload_packet(void)
{
    if(!nv_file_access(NV_DLOAD_PATH,0))
    {
        nv_file_remove(NV_DLOAD_PATH);
    }

    if(!nv_file_access(NV_XNV_CARD1_PATH,0))
    {
        nv_file_remove(NV_XNV_CARD1_PATH);
    }

    if(!nv_file_access(NV_XNV_CARD2_PATH,0))
    {
        nv_file_remove(NV_XNV_CARD2_PATH);
    }

    if(!nv_file_access(NV_CUST_CARD1_PATH,0))
    {
        nv_file_remove(NV_CUST_CARD1_PATH);
    }

    if(!nv_file_access(NV_CUST_CARD2_PATH,0))
    {
        nv_file_remove(NV_CUST_CARD2_PATH);
    }

    if(!nv_file_access(NV_XNV_CARD1_MAP_PATH,0))
    {
        nv_file_remove(NV_XNV_CARD1_MAP_PATH);
    }

    if(!nv_file_access(NV_XNV_CARD2_MAP_PATH,0))
    {
        nv_file_remove(NV_XNV_CARD2_MAP_PATH);
    }
}



u32 bsp_nvm_revert_defaultEx(const s8* path)
{
    u32 ret = NV_ERROR;
    u32 i = 0;
    FILE* fp = NULL;
    struct nv_ctrl_file_info_stru  manu_ctrl_file = {0};
    u8* ctrl_file_data = NULL;
    struct nv_global_ddr_info_stru manu_ddr_info = {0};



    nv_debug(NV_FUN_REVERT_DEFAULT,0,0,0,0);
    fp = nv_file_open((s8*)path,(s8*)NV_FILE_READ);
    if(NULL == fp)
    {
        nv_debug(NV_FUN_REVERT_DEFAULT,1,0,0,0);
        return BSP_ERR_NV_NO_FILE;
    }

    ret = (u32)nv_file_read((u8*)&manu_ctrl_file,1,sizeof(manu_ctrl_file),fp);
    if(ret != sizeof(manu_ctrl_file))
    {
        nv_debug(NV_FUN_REVERT_DEFAULT,2,ret,0,0);
        ret = BSP_ERR_NV_READ_FILE_FAIL;
        goto file_close_err;
    }
    nv_file_seek(fp,0,SEEK_SET);
    ctrl_file_data = (u8*)nv_malloc(manu_ctrl_file.ctrl_size+1);
    if(NULL == ctrl_file_data)
    {
        nv_debug(NV_FUN_REVERT_DEFAULT,3,BSP_ERR_NV_MALLOC_FAIL,0,0);
        ret = BSP_ERR_NV_MALLOC_FAIL;
        goto file_close_err;
    }

    ret = (u32)nv_file_read(ctrl_file_data,1,manu_ctrl_file.ctrl_size,fp);
    if(ret != manu_ctrl_file.ctrl_size)
    {
        nv_error_printf("ret 0x%x,ctrl size 0x%x\n",ret,manu_ctrl_file.ctrl_size);
        nv_debug(NV_FUN_REVERT_DEFAULT,4,ret,manu_ctrl_file.ctrl_size,0);
        ret = BSP_ERR_NV_READ_FILE_FAIL;
        goto free_ctrl_file;
    }

    ret = nv_init_file_info((u8*)ctrl_file_data,(u8*)&manu_ddr_info);
    if(ret)
    {
        nv_debug(NV_FUN_REVERT_DEFAULT,5,ret,0,0);
        ret = BSP_ERR_NV_MEM_INIT_FAIL;
        goto free_ctrl_file;
    }
    for(i = 0;i<manu_ctrl_file.file_num;i++)
    {
        ret = nv_revert_default(fp,manu_ddr_info.file_info[i].size);
        if(ret)
        {
            nv_debug(NV_FUN_REVERT_DEFAULT,6,ret,manu_ddr_info.file_info[i].size,0);
            goto free_ctrl_file;
        }
    }
    nv_file_close(fp);
    nv_free(ctrl_file_data);


    return NV_OK;
free_ctrl_file:
    nv_free(ctrl_file_data);
file_close_err:
    nv_file_close(fp);
    nv_mntn_record("\n[%s]\n",__FUNCTION__);
    nv_help(NV_FUN_REVERT_DEFAULT);
    return ret;

}

/*
* Function   : bsp_nvm_revert_default
* Discription: revert factory default edition,revert one by one
* Parameter  : none
* Output     : result
* History    : yuyangyang  00228784  create
*/
#if (FEATURE_ON == MBB_DLOAD)
u32 bsp_nvm_revert_default(void)
{
    u32 ret = NV_ERROR;

    printf("enter to set default nv !\r\n");
    
    ret = nv_revert_data(NV_DEFAULT_PATH,g_ausNvResumeDefualtIdList,\
             bsp_nvm_getRevertNum(NV_MBB_DEFUALT_ITEM));
    if(ret)
    {
        printf("Set default nv nv_revert_data error!\r\n");
        goto err_out;
    }
    
    ret = bsp_nvm_flush();
    if(ret)
    {
        printf("Set default nv bsp_nvm_flush error!\r\n");
        goto err_out;
    }

err_out:
    return ret;
}
#else
u32 bsp_nvm_revert_default(void)
{
    u32 ret;

    ret = bsp_nvm_revert_defaultEx((s8*)NV_DEFAULT_PATH);
    if(ret)
    {
        return ret;
    }

    /*��Ҫnv��ָ�*/
    ret = nv_revert_data(NV_IMG_PATH,g_ausNvResumeSecureIdList,\
        bsp_nvm_getRevertNum(NV_SECURE_ITEM));
    if(ret)
    {
        return ret;
    }

    ret = bsp_nvm_flush();
    ret |= bsp_nvm_flushSys(NV_ERROR);
    return ret;
}
#endif /*MBB_DLOAD*/


/*
* Function   : bsp_nvm_key_check
* Discription: check the key data
* Parameter  : none
* Output     : result
* History    : yuyangyang  00228784  create
*/
/*lint -save -e438*/
u32 bsp_nvm_key_check(void)
{
    FILE* fp = NULL;
    u32 ret = NV_ERROR;
    u32 datalen = 0;        /*read file len*/
    u32 file_offset = 0;
    u8* bak_ctrl_file = NULL;
    u8* bak_data = NULL;           /*single nv data ,max len 2048byte*/
    u8* mem_data = NULL;
    struct nv_ctrl_file_info_stru    bak_ctrl_info = {0};   /*bak file ctrl file head*/
    struct nv_file_list_info_stru    bak_file_info  = {0};
    struct nv_global_ddr_info_stru   bak_ddr_info   = {0};
    struct nv_ref_data_info_stru     bak_ref_info   = {0};

    struct nv_ref_data_info_stru    mem_ref_info  = {0};
    struct nv_file_list_info_stru   mem_file_info = {0};

    if(nv_file_access((s8*)NV_DEFAULT_PATH,0))  /*û���ļ���ֱ�ӷ���ok*/
    {
        return NV_OK;
    }

    nv_debug(NV_FUN_KEY_CHECK,0,0,0,0);
    fp = nv_file_open((s8*)NV_DEFAULT_PATH,(s8*)NV_FILE_READ);
    if(NULL == fp)
    {
        nv_debug(NV_FUN_KEY_CHECK,1,0,0,0);
        return BSP_ERR_NV_NO_FILE;
    }

    /*first read ctrl file head*/
    datalen = (u32)nv_file_read((u8*)(&bak_ctrl_info),1,sizeof(bak_ctrl_info),fp);
    if(datalen != sizeof(bak_ctrl_info))
    {
        nv_debug(NV_FUN_KEY_CHECK,2,datalen,0,0);
        ret = BSP_ERR_NV_READ_FILE_FAIL;
        goto close_file;
    }

    if(bak_ctrl_info.magicnum != NV_CTRL_FILE_MAGIC_NUM)
    {
        nv_debug(NV_FUN_KEY_CHECK,3,bak_ctrl_info.magicnum,0,0);
        ret = BSP_ERR_NV_FILE_ERROR;
        goto close_file;
    }

    bak_ctrl_file = (u8*)nv_malloc(bak_ctrl_info.ctrl_size);
    if(NULL == bak_ctrl_file)
    {
        nv_debug(NV_FUN_KEY_CHECK,4,bak_ctrl_info.ctrl_size,0,0);
        ret = BSP_ERR_NV_MALLOC_FAIL;
        goto close_file;
    }
    /*second :read all ctrl file*/
    nv_file_seek(fp,0,SEEK_SET); /*jump to file head*/
    ret = (u32)nv_file_read(bak_ctrl_file,1,bak_ctrl_info.ctrl_size,fp);
    if(ret != bak_ctrl_info.ctrl_size)
    {
        nv_debug(NV_FUN_KEY_CHECK,5,ret,bak_ctrl_info.ctrl_size,0);
        ret = BSP_ERR_NV_READ_FILE_FAIL;
        goto free_ctrl_data;

    }
    /*third :init ctrl file info to bak_ddr_info*/
    ret = nv_init_file_info((u8*)bak_ctrl_file,(u8*)&bak_ddr_info);
    if(ret)
    {
        nv_debug(NV_FUN_KEY_CHECK,6,ret,0,0);
        goto free_ctrl_data;
    }

    /*forth :look for imei id in bak & cur mem*/
    ret = nv_search_byid(NV_ID_DRV_IMEI,bak_ctrl_file,&bak_ref_info,&bak_file_info);
    if(ret)
    {
        nv_debug(NV_FUN_KEY_CHECK,7,ret,0,0);
        goto free_ctrl_data;
    }
    ret = nv_search_byid(NV_ID_DRV_IMEI,(u8*)NV_GLOBAL_CTRL_INFO_ADDR,&mem_ref_info,&mem_file_info);
    if(ret)
    {
        nv_debug(NV_FUN_KEY_CHECK,8,ret,0,0);
        goto free_ctrl_data;
    }

    /*compare info*/
    if( (mem_ref_info.nv_len    != bak_ref_info.nv_len) ||
        (mem_ref_info.modem_num != bak_ref_info.modem_num)
        )
    {
        nv_debug(NV_FUN_KEY_CHECK,9,bak_ref_info.nv_len,bak_ref_info.modem_num,mem_ref_info.nv_len);
        ret = BSP_ERR_NV_FILE_ERROR;
        goto free_ctrl_data;
    }

    if(   (mem_ref_info.modem_num == NV_USIMM_CARD_2)
        &&(bak_ref_info.modem_num == NV_USIMM_CARD_2))
    {
        datalen = mem_ref_info.nv_len * 2;
    }
    else
    {
        datalen = mem_ref_info.nv_len;
    }


    bak_data = (u8*)nv_malloc(datalen);
    mem_data = (u8*)nv_malloc(datalen);
    if((NULL == bak_data)||(mem_data == NULL))
    {
        nv_debug(NV_FUN_KEY_CHECK,10,0,0,0);
        goto free_ctrl_data;
    }

    /*count data offset in bak file*/
    file_offset = bak_ddr_info.file_info[bak_file_info.file_id-1].offset +bak_ref_info.nv_off;

    nv_file_seek(fp,(s32)file_offset,SEEK_SET);
    ret = (u32)nv_file_read(bak_data,1,datalen,fp);/*�����ݴ��ļ���ָ��ƫ�ƴ�����*/
    if(ret != datalen)
    {
        nv_debug(NV_FUN_KEY_CHECK,11,ret,datalen,0);
        goto free_data;
    }

    ret = nv_read_from_mem(mem_data,datalen,mem_file_info.file_id,mem_ref_info.nv_off);
    if(ret)
    {
        nv_debug(NV_FUN_KEY_CHECK,12,0,0,0);
        goto free_data;
    }

    nv_file_close(fp);
    ret = (u32)memcmp(mem_data,bak_data,datalen);  /*�Ƚ����ݲ���*/
    if(ret)
    {
        ret = bsp_nvm_revert_defaultEx((s8*)NV_DEFAULT_PATH);/* [false alarm]:ret is in using */

        ret |= bsp_nvm_flush();/* [false alarm]:ret is in using */
        ret |= bsp_nvm_flushSys(NV_ERROR);/* [false alarm]:ret is in using */
    }
    nv_free(mem_data);
    nv_free(bak_data);
    nv_free(bak_ctrl_file);


    return NV_OK;
free_data:
    nv_free(mem_data);
    nv_free(bak_data);
free_ctrl_data:
    nv_free(bak_ctrl_file);
close_file:
    nv_file_close(fp);
    nv_mntn_record("\n%s\n",__func__);
    nv_help(NV_FUN_KEY_CHECK);
    return ret;
}
/*lint -restore -e747*/


s32 bsp_nvm_icc_task(void* parm)
{
    s32 ret = -1;
    struct nv_icc_stru icc_req;
    /* coverity[var_decl] */
    struct nv_icc_stru icc_cnf;
    u32 chanid;


    /* coverity[no_escape] */
    for(;;)
    {
        osl_sem_down(&g_nv_ctrl.task_sem);
        memset(g_nv_ctrl.nv_icc_buf,0,NV_ICC_BUF_LEN);
        memset(&icc_req,0,sizeof(icc_req));

        g_nv_ctrl.opState = NV_OPS_STATE;
        wake_lock(&g_nv_ctrl.wake_lock);

        /*�����ǰ����˯��״̬����ȴ����Ѵ���*/
        if(g_nv_ctrl.pmState == NV_SLEEP_STATE)
        {
            printk("%s cur state in sleeping,wait for resume end!\n",__func__);
            continue;
        }

        chanid = ICC_CHN_NV << 16 | NV_RECV_FUNC_AC;
        ret = bsp_icc_read(chanid,g_nv_ctrl.nv_icc_buf,NV_ICC_BUF_LEN);
        if(((u32)ret > NV_ICC_BUF_LEN)||(ret <= 0))
        {
            nv_debug_printf("bsp icc read error, chanid :0x%x ret :0x%x\n",chanid,ret);

            chanid = ICC_CHN_MCORE_ACORE << 16 | NV_RECV_FUNC_AM;
            ret = bsp_icc_read(chanid,g_nv_ctrl.nv_icc_buf,NV_ICC_BUF_LEN);
            if(((u32)ret > NV_ICC_BUF_LEN)||(ret <= 0))
            {
                g_nv_ctrl.opState = NV_IDLE_STATE;
                wake_unlock(&g_nv_ctrl.wake_lock);
                nv_debug_printf("bsp icc read error, chanid :0x%x ret :0x%x\n",chanid,ret);
                continue;
            }
        }

        g_nv_ctrl.task_proc_count ++;

        memcpy(&icc_req,g_nv_ctrl.nv_icc_buf,sizeof(icc_req));
        /*lint -save -e578 -e530*/
        nv_debug_trace(&icc_req, sizeof(icc_req));
        /*lint -restore +e578 +e530*/
        if(icc_req.msg_type == NV_ICC_REQ)
        {
            icc_cnf.ret = bsp_nvm_flushEx(icc_req.data_off,icc_req.data_len,icc_req.itemid);
#if (FEATURE_ON == MBB_DLOAD)
            /*������߲�ͨ���˼�ͨ��Ҫ��д���ǻ�Ҫnv������������*/
            if(true == nv_isSecListNv(icc_req.itemid))
            {
                ret = bsp_nvm_backup();
            }
            if(NV_ERROR == ret )
            {
                printf("nvm_backup icc Err!\n");
            }
#endif /*FEATURE_ON == MBB_DLOAD*/
        }
        else if(icc_req.msg_type == NV_ICC_REQ_SYS)
        {
            icc_cnf.ret = bsp_nvm_flushSys(icc_req.itemid);
        }
        else
        {
            printf("[%s] invalid parameter :0x%x\n",__func__,icc_req.msg_type);
            wake_unlock(&g_nv_ctrl.wake_lock);
            osl_sem_up(&g_nv_ctrl.task_sem);
            continue;
        }

        nv_pm_trace(icc_req.itemid,icc_req.slice);

        icc_cnf.msg_type = NV_ICC_CNF;
        icc_cnf.data_off = icc_req.data_off;
        icc_cnf.data_len = icc_req.data_len;
        icc_cnf.itemid   = icc_req.itemid;
        icc_cnf.slice    = icc_req.slice;

        /* coverity[uninit_use_in_call] */
        ret = (s32)nv_icc_send(chanid,(u8*)&icc_cnf,sizeof(icc_cnf));
        if(ret)
        {
            printf("[%s] icc send error !\n",__func__);
        }
        wake_unlock(&g_nv_ctrl.wake_lock);
        osl_sem_up(&g_nv_ctrl.task_sem);
    }
}

/*
* Function   : bsp_nvm_xml_decode
* Discription: xml decode inter,first decode xnv.xml,second decode cust.xml
* Parameter  : none
* Output     : result
* History    : yuyangyang  00228784  create
*/
u32 bsp_nvm_xml_decode(void)
{
    u32 ret = NV_ERROR;

    if(!nv_file_access(NV_XNV_CARD1_PATH,0))
    {
        ret = nv_xml_decode(NV_XNV_CARD1_PATH,NV_XNV_CARD1_MAP_PATH,NV_USIMM_CARD_1);
        if(ret)
        {
            return ret;
        }
    }

    if(!nv_file_access(NV_XNV_CARD2_PATH,0))
    {
        ret = nv_xml_decode(NV_XNV_CARD2_PATH,NV_XNV_CARD2_MAP_PATH,NV_USIMM_CARD_2);
        if(ret)
        {
            return ret;
        }
    }


    /*CUST XML �޶�ӦMAP�ļ��������ֵ����*/
    if(!nv_file_access(NV_CUST_CARD1_PATH,0))
    {
        ret = nv_xml_decode(NV_CUST_CARD1_PATH,NULL,NV_USIMM_CARD_1);
        if(ret)
        {
            return ret;
        }
    }

    if(!nv_file_access(NV_CUST_CARD2_PATH,0))
    {
        ret = nv_xml_decode(NV_CUST_CARD2_PATH,NULL,NV_USIMM_CARD_2);
        if(ret)
        {
            return ret;
        }
    }

    return NV_OK;
}

#ifdef BSP_CONFIG_HI3630
/*
* Function   : bsp_nvm_reload
* Discription: init mem,include first load the file to dload section,
*              this step ensure the nv data copy to ddr,enhance the dload file check
* Parameter  : none
* Output     : result
* History    : yuyangyang  00228784  create
*/
u32 bsp_nvm_reload(void)
{
    u32 ret = NV_ERROR;
    FILE* fp = NULL;
    u32 datalen = 0;

    nv_debug(NV_FUN_MEM_INIT,0,0,0,0);
    /*�����������ݴ��ڣ�����δд����ɵı�־�ļ�*/
    if( true == nv_check_img_validity())
    {
        nv_mntn_record("load from %s ...%s %s \n",NV_IMG_PATH,__DATE__,__TIME__);
        fp = nv_file_open((s8*)NV_IMG_PATH,(s8*)NV_FILE_READ);
        if(!fp)
        {
            nv_debug(NV_FUN_MEM_INIT,2,0,0,0);
            goto load_bak;
        }

        ret = nv_read_from_file(fp,(u8*)NV_GLOBAL_CTRL_INFO_ADDR,&datalen);
        nv_file_close(fp);
        if(ret)
        {
            nv_debug(NV_FUN_MEM_INIT,3,ret,0,0);
            goto load_bak;
        }
        return NV_OK;
    }

load_bak:
    if(true == nv_check_backup_validity())
    {
        nv_mntn_record("load from %s ...%s %s \n",NV_BACK_PATH,__DATE__,__TIME__);
        fp = nv_file_open((s8*)NV_BACK_PATH,(s8*)NV_FILE_READ);
        if(!fp)
        {
            nv_debug(NV_FUN_MEM_INIT,4,0,0,0);
            goto load_err_proc;
        }

        ret = nv_read_from_file(fp,(u8*)NV_GLOBAL_CTRL_INFO_ADDR,&datalen);
        nv_file_close(fp);
        if(ret)
        {
            nv_debug(NV_FUN_MEM_INIT,5,0,0,0);
            goto load_err_proc;
        }

        /*�ӱ�����������Ҫ����д�빤����*/
        ret = bsp_nvm_flushEn();
        if(ret)
        {
            nv_debug(NV_FUN_MEM_INIT,6,0,0,0);
            goto load_err_proc;
        }
        return NV_OK;
    }

load_err_proc:
    ret = nv_load_err_proc();
    if(ret)
    {
        nv_mntn_record("%s %d ,err revert proc ,ret :0x%x\n",__func__,__LINE__,ret);
        return ret;
    }
    return NV_OK;
}


/*
* Function   : bsp_nvm_upgrade
* Discription: upgrade proc
* Parameter  : none
* Output     : result
* History    : yuyangyang  00228784  create
*/
u32 bsp_nvm_upgrade(void)
{
    u32 ret;
    struct nv_global_ddr_info_stru* ddr_info = (struct nv_global_ddr_info_stru*)NV_GLOBAL_INFO_ADDR;

    nv_debug(NV_FUN_UPGRADE_PROC,0,0,0,0);

    /*�ж�fastboot�׶�xml �����Ƿ��쳣���������쳣������Ҫ���½���xml*/
    if(ddr_info->xml_dec_state != NV_XML_DEC_SUCC_STATE)
    {
        ret = bsp_nvm_xml_decode();
        if(ret)
        {
            nv_debug(NV_FUN_KERNEL_INIT,1,ret,0,0);
            goto out;
        }
    }

    /*�����ָ�������Ƭ�汾ֱ�ӷ���ok*/
    ret = nv_upgrade_revert_proc();
    if(ret)
    {
        nv_debug(NV_FUN_UPGRADE_PROC,2,ret,0,0);
        goto out;
    }

    /*����������д���������*/
    ret = nv_data_writeback();
    if(ret)
    {
        nv_debug(NV_FUN_UPGRADE_PROC,3,ret,0,0);
        goto out;
    }

    /*����������Ч*/
    ret = (u32)nv_modify_upgrade_flag(false);/*lint !e747 */
    if(ret)
    {
        nv_debug(NV_FUN_UPGRADE_PROC,4,ret,0,0);
        goto out;
    }

    return NV_OK;
out:
    nv_mntn_record("\n%s\n",__func__);
    nv_help(NV_FUN_UPGRADE_PROC);
    return ret;
}


/*
* Function   : bsp_nvm_kernel_init
* Discription: the second phase of nv init,this phase build upon the kernel init,
*              this phase need icc init,this phase ensure to use all nv api normal
*              start at this phase ,ops global ddr need spinlock
* Parameter  : none
* Output     : result
* History    : yuyangyang  00228784  create
*/
s32 bsp_nvm_kernel_init(void)
{
    u32 ret = NV_ERROR;
    struct nv_global_ddr_info_stru* ddr_info = (struct nv_global_ddr_info_stru*)NV_GLOBAL_INFO_ADDR;
    nv_debug(NV_FUN_KERNEL_INIT,0,0,0,0);

    /*sem & lock init*/
    spin_lock_init(&g_nv_ctrl.spinlock);
    osl_sem_init(0,&g_nv_ctrl.task_sem);
    osl_sem_init(1,&g_nv_ctrl.rw_sem);
    osl_sem_init(0,&g_nv_ctrl.cc_sem);
    wake_lock_init(&g_nv_ctrl.wake_lock,WAKE_LOCK_SUSPEND,"nv_wakelock");
    g_nv_ctrl.shared_addr = NV_GLOBAL_INFO_ADDR;

    nv_mntn_record("Balong nv init  start! %s %s\n",__DATE__,__TIME__);
    /*file info init*/
    ret = nv_file_init();
    if(ret)
    {
        nv_debug(NV_FUN_KERNEL_INIT,1,ret,0,0);
        goto out;
    }
    if(ddr_info->acore_init_state != NV_BOOT_INIT_OK)
    {
        nv_mntn_record("fast boot nv init fail !\n");
        nv_show_fastboot_err();
        memset(ddr_info,0,sizeof(struct nv_global_ddr_info_stru));
    }

    ddr_info->acore_init_state = NV_KERNEL_INIT_DOING;
    nv_flush_cache((void*)NV_GLOBAL_INFO_ADDR, (u32)NV_GLOBAL_INFO_SIZE);

    if((ddr_info->mem_file_type == NV_MEM_DLOAD) &&
       (!nv_file_access((s8*)NV_DLOAD_PATH,0)) &&/*����������������*/
       (true == nv_get_upgrade_flag())/*�����ļ���Ч*/
       )
    {
        ret = bsp_nvm_upgrade();
        if(ret)
        {
            nv_debug(NV_FUN_KERNEL_INIT,3,ret,0,0);
            goto out;
        }
    }
    else
    {
        /*���¼�����������*/
        ret = bsp_nvm_reload();
        if(ret)
        {
            nv_debug(NV_FUN_KERNEL_INIT,4,ret,0,0);
            goto out;
        }
    }

    /*�ó�ʼ��״̬ΪOK*/
    ddr_info->acore_init_state = NV_INIT_OK;
    nv_flush_cache((void*)NV_GLOBAL_INFO_ADDR, (u32)NV_GLOBAL_INFO_SIZE);

    /*��֤��������������д��*/
    nv_file_flag_check();

    INIT_LIST_HEAD(&g_nv_ctrl.stList);
/*lint -save -e740*/
    ret = (u32)osl_task_init("drv_nv",15,1024,bsp_nvm_icc_task,NULL,(u32*)&g_nv_ctrl.task_id);
    if(ret)
    {
        nv_mntn_record("[%s]:nv task init err! ret :0x%x\n",__func__,ret);
        goto out;
    }
/*lint -restore +e740*/

    if(   (ret = nv_icc_chan_init(NV_RECV_FUNC_AC))\
        ||(ret = nv_icc_chan_init(NV_RECV_FUNC_AM))\
        )
    {
        nv_debug(NV_FUN_KERNEL_INIT,5,ret,0,0);
        goto out;
    }

    /*to do:nv id use macro define*/
    ret = bsp_nvm_read(NV_ID_MSP_FLASH_LESS_MID_THRED,(u8*)(&(g_nv_ctrl.mid_prio)),sizeof(u32));
    if(ret)
    {
        g_nv_ctrl.mid_prio = 20;
        nv_printf("read 0x%x fail,use default value! ret :0x%x\n",NV_ID_MSP_FLASH_LESS_MID_THRED,ret);
    }
    nvchar_init();
    nv_mntn_record("Balong nv init ok!\n");
    return 0;

out:
    nv_mntn_record("\n[%s]\n",__FUNCTION__);
    nv_help(NV_FUN_KERNEL_INIT);
    show_ddr_info();
    return -1;
}

#else
/*
* Function   : bsp_nvm_reload
* Discription: init mem,include first load the file to dload section,
*              this step ensure the nv data copy to ddr,enhance the dload file check
* Parameter  : none
* Output     : result
* History    : yuyangyang  00228784  create
*/
u32 bsp_nvm_reload(void)
{
    u32 ret;
    FILE* fp;
    u32 datalen;
    struct nv_global_ddr_info_stru* ddr_info = (struct nv_global_ddr_info_stru*)NV_GLOBAL_INFO_ADDR;
#if (FEATURE_ON == MBB_DLOAD)
    u32 try_times = STARTUP_TRY_TIMES;
    /*��ȡfastboot���쳣��Ϣ�����ڴ��ַ*/
    power_info_s *power_info = (power_info_s *)SRAM_REBOOT_ADDR;
#endif
    nv_debug(NV_FUN_MEM_INIT,0,NV_GLOBAL_INFO_ADDR,NV_GLOBAL_CTRL_INFO_ADDR,0);

    /*���¼���֮ǰ���������״̬λ*/
    ddr_info->mem_file_type &= ~(0x1 << NV_MEM_DATA_NVSYS_IMG);
    ddr_info->mem_file_type &= ~(0x1 << NV_MEM_DATA_NVBACK);

    if(!nv_file_access(NV_IMG_PATH,0))
    {
#if (FEATURE_ON == MBB_DLOAD)
        /*������ַ��������Ĵ�������STARTUP_TRY_TIMES��*/
        if(try_times == power_info->wdg_rst_cnt)
        {
            /*���������û�����ݲ����κβ���*/
            if(!nv_file_access(NV_BACK_PATH,0))
            {
                /*��¼�����쳣��log��Ϣ��nvlog��*/
                nv_mntn_record("%s %s :The restart time has reached MAX:%d!\n",__DATE__,__TIME__,try_times);
                /*ɾ����������nv�ļ�*/
                nv_file_remove((s8*)NV_IMG_PATH);
                /*���ñ�����nv�ļ�*/
                goto load_back_file;
            }
        }
#endif
        nv_mntn_record("load from %s\n",NV_IMG_PATH);
        fp = nv_file_open(NV_IMG_PATH,NV_FILE_READ);
        if(NULL == fp)
        {
            ret = BSP_ERR_NV_NO_FILE;
            nv_debug(NV_FUN_MEM_INIT,1,ret,0,0);
            goto load_back_file;
        }

        ret = nv_read_from_file(fp,(u8*)NV_GLOBAL_CTRL_INFO_ADDR,&datalen);
        nv_file_close(fp);
        if(ret)
        {
            nv_debug(NV_FUN_MEM_INIT,2,ret,0,0);
            goto load_back_file;
        }

        ddr_info->mem_file_type |= 0x1 << NV_MEM_DATA_NVSYS_IMG;
        return NV_OK;

    }


/*hi3630�汾���mnvm2:0Ŀ¼��û�У����modem_logĿ¼�в���*/
#ifdef BSP_CONFIG_HI3630

    if(!nv_file_access(NV_IMG_BACK_PATH,0))
    {
        nv_mntn_record("load from %s\n",NV_IMG_BACK_PATH);
        fp = nv_file_open((s8*)NV_IMG_BACK_PATH,(s8*)NV_FILE_READ);
        if(NULL == fp)
        {
            ret = BSP_ERR_NV_NO_FILE;
            nv_debug(NV_FUN_MEM_INIT,1,ret,0,0);
            goto load_back_file;
        }

        ret = nv_read_from_file(fp,(u8*)NV_GLOBAL_CTRL_INFO_ADDR,&datalen);
        nv_file_close(fp);
        if(ret)
        {
            nv_debug(NV_FUN_MEM_INIT,2,ret,0,0);
            goto load_back_file;
        }

        ddr_info->mem_file_type |= 0x1 << NV_MEM_DATA_NVSYS_IMG;
        return NV_OK;

    }
#endif



load_back_file:
    if(!nv_file_access(NV_BACK_PATH,0))
    {
        nv_mntn_record("load from %s\n",NV_BACK_PATH);
        fp = nv_file_open(NV_BACK_PATH,NV_FILE_READ);
        if(NULL == fp)
        {
            ret = BSP_ERR_NV_NO_FILE;
            nv_debug(NV_FUN_MEM_INIT,3,ret,0,0);
            goto reload_err_out;
        }

        ret = nv_read_from_file(fp,(u8*)NV_GLOBAL_CTRL_INFO_ADDR,&datalen);
        nv_file_close(fp);
        if(ret)
        {
            nv_debug(NV_FUN_MEM_INIT,4,ret,0,0);
            goto reload_err_out;
        }
        ddr_info->mem_file_type |= 0x1 << NV_MEM_DATA_NVBACK;
    }
    else
    {
        ddr_info->mem_file_type = 0;
    }
    return NV_OK;
reload_err_out:
    nv_mntn_record("\n%s\n",__func__);
    nv_help(NV_FUN_MEM_INIT);
    return NV_ERROR;
}


/* Begin PN: DTS2013093001868 added by miaoshiyang 20130930 */
#if (FEATURE_ON == MBB_DLOAD_BBOU)
/*
* Function   : bsp_nvm_restore_online_handle
* Discription: after NV Automatic recovery, 
               Clear signs deal with online upgrade
* Parameter  : none
* Output     : result
* History    : miaoshiyang 00230250
*/
s32 bsp_nvm_restore_online_handle(NV_RESTORE_STATUS stuType)
{
    huawei_smem_info *smem_data = NULL;
    smem_data = (huawei_smem_info *)SRAM_DLOAD_ADDR;
    
    if (NULL == smem_data)
    {
        printf("Dload smem_data malloc fail!\n");
        return -1;
    }

    printf("smem_data->smem_online_upgrade_flag :0x%x\n", 
                smem_data->smem_online_upgrade_flag);
    printf("smem_data->smem_multiupg_flag :0x%x\n", 
                smem_data->smem_multiupg_flag);
    if(SMEM_ONNR_FLAG_NUM == smem_data->smem_online_upgrade_flag)
    {
        if(NV_RESTRORE_SUCCESS == stuType)
        {
            /*��������NV�Զ��ָ��׶�ħ��������*/
            smem_data->smem_online_upgrade_flag = 0;

#if  (FEATURE_ON == MBB_DLOAD_MCPEUP)
            led_kernel_status_set("mode_led:red", 0);
            led_kernel_status_set("mode_led:green", 1);
            led_kernel_status_set("mode_led:blue", 0);
            led_kernel_status_set("signal1_led:white", 1);
            led_kernel_status_set("signal2_led:white", 0);
            led_kernel_status_set("signal3_led:white", 1);
#endif
            /*�鲥����������*/
            if(SMEM_MULTIUPG_FLAG_NUM == smem_data->smem_multiupg_flag)
            {
                smem_data->smem_multiupg_flag = 0;

                printf("MULTI UPG success, do not reboot.\n");
            }
            else
            {
                smem_data->smem_switch_pcui_flag = 0;
                printf("MBB:Online Upgrade Sucessful,reboot.\n");
                /*����������������ģʽ*/
                BSP_OM_SoftReboot();
            }
        }
        else
        {
            if(SMEM_MULTIUPG_FLAG_NUM == smem_data->smem_multiupg_flag)
            {
                smem_data->smem_multiupg_flag = 0;
            }
#if  (FEATURE_ON == MBB_DLOAD_MCPEUP)
            led_kernel_status_set("mode_led:red", 1);
            led_kernel_status_set("mode_led:green", 0);
            led_kernel_status_set("mode_led:blue", 0);
            led_kernel_status_set("signal1_led:white", 1);
            led_kernel_status_set("signal2_led:white", 0);
            led_kernel_status_set("signal3_led:white", 1);
#endif

            printf("MBB:Online Upgrade failed !\n");
        }
    }
    return 0;
}
#endif /* MBB_DLOAD_BBOU */
/* End PN: DTS2013093001868 added by miaoshiyang 20130930 */

#if (FEATURE_ON == MBB_DLOAD_SDUP)
/*
* Function   : bsp_nvm_restore_sd_handle
* Discription: after NV Automatic recovery, 
               Clear signs deal with sd upgrade
* Parameter  : none
* Output     : result
* History    : miaoshiyang 00230250
*/
void bsp_nvm_restore_sd_handle(NV_RESTORE_STATUS stuType)
{
    huawei_smem_info *smem_data = NULL;
    smem_data = (huawei_smem_info *)SRAM_DLOAD_ADDR;

    if(SMEM_SDNR_FLAG_NUM == smem_data->smem_online_upgrade_flag)
    {
        if(NV_RESTRORE_SUCCESS == stuType)
        {
            smem_data->smem_sd_upgrade = SMEM_SUOK_FLAG_NUM;
            printf("MBB:Sd Upgrade Sucessful !\n");
        }
        else
        {
            smem_data->smem_sd_upgrade = SMEM_SUFL_FLAG_NUM;
            printf("MBB:Sd Upgrade failed !\n");
        }
        
    }
    return;
}
#endif /* MBB_DLOAD_SDUP */

#if (FEATURE_ON == MBB_FEATURE_CAT_MODULE_SET)
/*
* Function   : bsp_special_nvm_proc
* Discription: special nv proc
* Parameter  : none
* Output     : result
* History    : ankang  00252937  create
*/
s32 bsp_special_nvm_proc(void)
{
    struct nv_platform_catgory_set_info uePatformCat = {0};
    u32 ret = NV_ERROR;
    u32 nvlen = 0;
    u8  nv_lte_cat = 0;

    /*  �ȶ�ȡNV50458��Ȼ���ж���״̬  */
    ret = bsp_nvm_read(NV_ID_MSP_PLATFORM_CATEGORY_VAL, &uePatformCat,
                       sizeof(uePatformCat));
    if(ret)
    {
        printf("bsp_special_nvm_proc Read NV50458 fail ! \n");
        goto proc_end;
    }

    /*  �����NVû�м���, ��ֱ�ӷ���OK  */
    if (0 == uePatformCat.nv_status)
    {
        printf("bsp_special_nvm_proc NV50458 ST 0 ! \n");
        goto proc_end;
    }

    /*  ֻ��ȡD22C��CAT��ʶֵ��ƫ��7��ֻȡһλ����  */
    ret = bsp_nvm_readpart(NV_ID_MSP_CATEGORY_VAL, 7, &nv_lte_cat, 1);
    if(ret)
    {
        printf("bsp_special_nvm_proc Read NVD22C fail ! \n");
        return NV_ERROR;
    }
    printf("bsp_special_nvm_proc DRV: %d, LTE: %d ! \n", 
           uePatformCat.cat_value, nv_lte_cat);

    if (uePatformCat.cat_value == nv_lte_cat)
    {
        printf("bsp_special_nvm_proc DRV = LTE ! \n");
        goto proc_end;
    }
    
    /*  Ŀǰ��Ʒֻ��������2(CAT3), 3(CAT4),����������ӻ��߱�������޸Ĵ˴��ж�  */
    if ((2 != uePatformCat.cat_value) && (3 != uePatformCat.cat_value))
    {
        printf("bsp_special_nvm_proc NV50458 VAL = %d 0 ! \n", uePatformCat.cat_value);
        goto proc_end;
    }

    /*  ��������NV50458��ֵд��D22C��  */
    nv_lte_cat = uePatformCat.cat_value;
    ret = bsp_nvm_writepart(NV_ID_MSP_CATEGORY_VAL, 7, &nv_lte_cat, 1);
    if(ret)
    {
        printf("bsp_special_nvm_proc Write NVD22C fail ! \n");
        return NV_ERROR;
    }
    printf("bsp_special_nvm_proc Write D22C OK ! \n");

    return NV_OK;
proc_end:

    return NV_ERROR;
}
#endif
/*
* Function   : bsp_nvm_upgrade
* Discription: upgrade proc
* Parameter  : none
* Output     : result
* History    : yuyangyang  00228784  create
*/
u32 bsp_nvm_upgrade(void)
{
    u32 ret;
    FILE* fp;
    u32 datalen = 0;
    struct nv_global_ddr_info_stru* ddr_info = (struct nv_global_ddr_info_stru*)NV_GLOBAL_INFO_ADDR;
    u32 mem_type = 0;
    bool RevertFlag = false;

    ddr_info->mem_file_type &= ~(0x1 << NV_MEM_DATA_NVDLOAD); /*�������ط����ڴ��־λ*/
    mem_type                 = ddr_info->mem_file_type;       /*��¼��ǰ�ڴ����ݱ�־λ*/

    nv_debug(NV_FUN_UPGRADE_PROC,0,0,0,0);

    /*����������д������¼����ļ��������Ϊ��������������Ҫ����У׼nv���ݻָ�*/
    if(((!nv_file_access(NV_DLOAD_PATH,0)) ||
        (!nv_file_access(NV_XNV_CARD1_PATH,0)) ||
        (!nv_file_access(NV_XNV_CARD2_PATH,0)) ||
        (!nv_file_access(NV_CUST_CARD1_PATH,0)) ||
        (!nv_file_access(NV_CUST_CARD2_PATH,0))
        )&&(mem_type)
       )
    {
        ret = bsp_nvm_backup();
        if(ret)
        {
            nv_debug(NV_FUN_UPGRADE_PROC,1,ret,0,0);
            goto upgrade_fail_out;
        }
        RevertFlag = true;    /*��¼���������еı��ݶ���*/
    }

    if(!nv_file_access(NV_DLOAD_PATH,0))
    {
        fp = nv_file_open(NV_DLOAD_PATH,NV_FILE_READ);
        if(NULL == fp)
        {
            ret = BSP_ERR_NV_NO_FILE;
            nv_debug(NV_FUN_UPGRADE_PROC,2,ret,0,0);
            goto upgrade_fail_out;
        }

        ret = nv_read_from_file(fp,(u8*)NV_GLOBAL_CTRL_INFO_ADDR,&datalen);
        nv_file_close(fp);
        if(ret)
        {
            nv_debug(NV_FUN_UPGRADE_PROC,3,ret,datalen,0);
            goto upgrade_fail_out;
        }
        if(false == nv_dload_file_check())
        {
            nv_debug(NV_FUN_UPGRADE_PROC,4,ret,0,0);
            goto upgrade_fail_out;
        }

        ddr_info->mem_file_type |= (0x1 << NV_MEM_DATA_NVDLOAD);
    }
    else if(!mem_type)  /*�ڴ����������о�û�����ݣ�����Ҫ���ش���*/
    {
        ret = BSP_ERR_NV_NO_FILE;
        nv_debug(NV_FUN_UPGRADE_PROC,5,ret,0,0);
        goto upgrade_fail_out;
    }

    ret = bsp_nvm_xml_decode();
    if(ret)
    {
        nv_debug(NV_FUN_UPGRADE_PROC,6,ret,0,0);
        goto upgrade_fail_out;
    }

/*lint -save -e731*/
    if(RevertFlag == true)/*�뱸�ݶ���ͬ����������ֱ��ݲ�������ͬ����Ҫ���лָ�����*/
    {
        ret = bsp_nvm_revert();

#if(FEATURE_ON == MBB_CHG_BQ27510)
        nv_revert_flag = true;
#endif

        if(ret)
        {
            nv_debug(NV_FUN_UPGRADE_PROC,4,ret,0,0);
            goto upgrade_fail_out;
        }
    }

    mem_type = ddr_info->mem_file_type &(~(0x1 << NV_MEM_DATA_NVSYS_IMG));/*�ڴ����Ƿ���ڷ�sys������������������*/
    if(mem_type)
    {
        ret = bsp_nvm_flushEn();
        if(ret)
        {
            nv_debug(NV_FUN_UPGRADE_PROC,7,ret,0,0);
            goto upgrade_fail_out;
        }
    }

    if(RevertFlag == true)/*ֻ������������¸��±�����*/
    {
        ret = bsp_nvm_backup();
        if(ret)
        {
            nv_debug(NV_FUN_UPGRADE_PROC,9,ret,0,0);
            goto upgrade_fail_out;
        }
    }
/*lint -restore +e731*/

    /*����Ƿ������������ݣ�����ɾ��*/
    bsp_nvm_remove_dload_packet();

    if(mem_type)/*����ÿ��������Ҫˢ��Ҫ����flushsys ����*/
    {
        ret = bsp_nvm_flushSys(NV_ERROR);
        if(ret)
        {
            nv_debug(NV_FUN_UPGRADE_PROC,8,ret,0,0);
            goto upgrade_fail_out;
        }
    }

    return NV_OK;
upgrade_fail_out:
    nv_mntn_record("\n%s\n",__func__);
    nv_help(NV_FUN_UPGRADE_PROC);
    return NV_ERROR;
}

/*
* Function   : bsp_nvm_init
* Discription: the second phase of nv init,this phase build upon the kernel init,
*              this phase need icc init,this phase ensure to use all nv api normal
*              start at this phase ,ops global ddr need spinlock
* Parameter  : none
* Output     : result
* History    : yuyangyang  00228784  create
*/
s32 bsp_nvm_kernel_init(void)
{
    u32 ret = NV_ERROR;
    struct nv_global_ddr_info_stru* ddr_info = (struct nv_global_ddr_info_stru*)NV_GLOBAL_INFO_ADDR;

    nv_debug(NV_FUN_KERNEL_INIT,0,0,0,0);
    
#if (FEATURE_ON == MBB_DLOAD)
    
    huawei_smem_info *smem_data = NULL;
    smem_data = (huawei_smem_info *)SRAM_DLOAD_ADDR;
    if (NULL == smem_data)
    {
        printf("nv_file_init: smem_data is NULL \n");
        return -1;  

    }

    if(SMEM_DLOAD_FLAG_NUM == smem_data->smem_dload_flag)
    {
        /*����ģʽ������nvģ�������*/
        printf("entry update not init nvim !\n");
        return -1;  
    }
#endif /*MBB_DLOAD*/

    /*sem & lock init*/
    spin_lock_init(&g_nv_ctrl.spinlock);
    osl_sem_init(0,&g_nv_ctrl.task_sem);
    osl_sem_init(1,&g_nv_ctrl.rw_sem);
    osl_sem_init(0,&g_nv_ctrl.cc_sem);
    wake_lock_init(&g_nv_ctrl.wake_lock,WAKE_LOCK_SUSPEND,"nv_wakelock");
    g_nv_ctrl.shared_addr = NV_GLOBAL_INFO_ADDR;

    nv_mntn_record("Balong nv init  start!\n");

    /*file info init*/
    ret = nv_file_init();
    if(ret)
    {
        nv_debug(NV_FUN_KERNEL_INIT,1,ret,0,0);
        goto nv_init_fail;
    }
    /*check boot init state*/
    if((ddr_info->acore_init_state != NV_BOOT_INIT_OK))
    {
        nv_mntn_record("fast boot nv init fail !\n");
        nv_show_fastboot_err();
        memset(ddr_info,0,sizeof(struct nv_global_ddr_info_stru));
    }

    ddr_info->acore_init_state = NV_KERNEL_INIT_DOING;
    nv_flush_cache((void*)NV_GLOBAL_INFO_ADDR, (u32)NV_GLOBAL_INFO_SIZE);

    /*��ʼ����ʼ�����¼������ݵ��ڴ���*/
    ret = bsp_nvm_reload();
    if(ret)
    {
        nv_debug(NV_FUN_KERNEL_INIT,2,ret,0,0);
        goto nv_init_fail;
    }

    /*������������*/
    ret = bsp_nvm_upgrade();
    if(ret)
    {
        nv_debug(NV_FUN_KERNEL_INIT,3,ret,0,0);
        goto nv_init_fail;
    }
#if 0    /*������*/
    ret = bsp_nvm_key_check();
    if(ret)
    {
        nv_debug(NV_FUN_KERNEL_INIT,4,ret,0,0);
        goto nv_init_fail;
    }
#endif
    ddr_info->acore_init_state = NV_INIT_OK;
    nv_flush_cache((void*)NV_GLOBAL_INFO_ADDR, (u32)NV_GLOBAL_INFO_SIZE);
    INIT_LIST_HEAD(&g_nv_ctrl.stList);
/*lint -save -e740*/
/*  �������CAT�ȼ����������ʧ�ܣ�����������  */
#if (FEATURE_ON == MBB_FEATURE_CAT_MODULE_SET)
    ret = bsp_special_nvm_proc();
    if (ret)
    {
        printf("bsp_special_nvm_proc call fail ! \n");
    }
#endif

    ret = (u32)osl_task_init("drv_nv",15,1024,bsp_nvm_icc_task,NULL,(u32*)&g_nv_ctrl.task_id);
    if(ret)
    {
        nv_mntn_record("[%s]:nv task init err! ret :0x%x\n",__func__,ret);
        goto nv_init_fail;
    }
/*lint -restore +e740*/

    if(   (ret = nv_icc_chan_init(NV_RECV_FUNC_AC))\
        ||(ret = nv_icc_chan_init(NV_RECV_FUNC_AM))\
        )
    {
        goto nv_init_fail;
    }

    /*to do:nv id use macro define*/
    ret = bsp_nvm_read(NV_ID_MSP_FLASH_LESS_MID_THRED,(u8*)(&(g_nv_ctrl.mid_prio)),sizeof(u32));
    if(ret)
    {
        g_nv_ctrl.mid_prio = 20;
        printf("read 0x%x fail,use default value! ret :0x%x\n",NV_ID_MSP_FLASH_LESS_MID_THRED,ret);
    }
    nvchar_init();
    nv_mntn_record("Balong nv init ok!\n");
#if (FEATURE_ON == MBB_DLOAD_BBOU)
    ret = bsp_nvm_restore_online_handle(NV_RESTRORE_SUCCESS);
    if(ret)
    {
        return ret;
    }
#endif
#if (FEATURE_ON == MBB_DLOAD_SDUP)
    /* sd������NV�ָ��ɹ����� */
    bsp_nvm_restore_sd_handle(NV_RESTRORE_SUCCESS);
#endif
    return NV_OK;

nv_init_fail:
    nv_mntn_record("\n[%s]\n",__FUNCTION__);
    ddr_info->acore_init_state = NV_INIT_FAIL;
    nv_help(NV_FUN_KERNEL_INIT);
    show_ddr_info();
#if (FEATURE_ON == MBB_DLOAD_SDUP)
    /* sd������NV�ָ�ʧ�ܴ��� */
    bsp_nvm_restore_sd_handle(NV_RESTRORE_FAIL);
#endif
#if (FEATURE_ON == MBB_DLOAD_BBOU)
    bsp_nvm_restore_online_handle(NV_RESTRORE_FAIL);
#endif
    return -1;
}
#endif


s32 bsp_nvm_remain_init(void)
{
    return 0;
}

/*lint -save -e529*/
static void bsp_nvm_exit(void)
{
    struct nv_global_ddr_info_stru* ddr_info = (struct nv_global_ddr_info_stru*)NV_GLOBAL_INFO_ADDR;

    /*�ػ�д����*/
    (void)bsp_nvm_flush();
    (void)bsp_nvm_backup();
    /*�����־*/
    memset(ddr_info,0,sizeof(struct nv_global_ddr_info_stru));
}
/*lint -restore +e529*/


u32 nvm_read_rand(u32 nvid)
{
    u32 ret;
    u8* tempdata;
    u32 i= 0;
    struct nv_ref_data_info_stru ref_info = {0};
    struct nv_file_list_info_stru file_info = {0};

    ret = nv_search_byid(nvid, (u8*)NV_GLOBAL_CTRL_INFO_ADDR,&ref_info,&file_info);
    if(NV_OK != ret)
    {
        return ret;
    }
    printf("[0x%x]:len 0x%x,off 0x%x,file id %d\n",nvid,ref_info.nv_len,ref_info.nv_off,ref_info.file_id);

    tempdata = (u8*)nv_malloc((u32)(ref_info.nv_len) +1);
    if(NULL == tempdata)
    {
        return BSP_ERR_NV_MALLOC_FAIL;
    }

    ret = bsp_nvm_read(nvid,tempdata,ref_info.nv_len);
    if(NV_OK != ret)
    {
        nv_free(tempdata);
        return BSP_ERR_NV_READ_DATA_FAIL;
    }

    for(i=0;i<ref_info.nv_len;i++)
    {
        if((i%32) == 0)
        {
            printf("\n");
        }
        printf("%02x ",(u8)(*(tempdata+i)));
    }
    nv_free(tempdata);
    printf("\n\n");
    return 0;
}


u32 nvm_read_randex(u32 nvid,u32 modem_id)
{
	u32 ret;    u8* tempdata;    u32 i= 0;
	struct nv_ref_data_info_stru ref_info = {0};
	struct nv_file_list_info_stru file_info = {0};

	ret = nv_search_byid(nvid, (u8*)NV_GLOBAL_CTRL_INFO_ADDR,&ref_info,&file_info);
	if(NV_OK != ret)
	{
		return ret;
	}
	if(ref_info.nv_len == 0)
	{
		return NV_ERROR;
	}

	printf("[0x%x]:len 0x%x,off 0x%x,file id %d\n",nvid,ref_info.nv_len,ref_info.nv_off,ref_info.file_id);
	printf("[0x%x]:dsda 0x%x\n",nvid,ref_info.modem_num);

	tempdata = (u8*)nv_malloc((u32)(ref_info.nv_len) +1);
	if(NULL == tempdata)
	{
		return BSP_ERR_NV_MALLOC_FAIL;
	}
	ret = bsp_nvm_dcread(modem_id,nvid,tempdata,ref_info.nv_len);
	if(NV_OK != ret)
	{
		nv_free(tempdata);
		return BSP_ERR_NV_READ_DATA_FAIL;
	}

	for(i=0;i<ref_info.nv_len;i++)
	{
		if((i%32) == 0)
		{
			printf("\n");
		}
		printf("%02x ",(u8)(*(tempdata+i)));
	}

	printf("\n\n");
	nv_free(tempdata);

	return 0;

}

#if(FEATURE_ON == MBB_CHG_BQ27510)
module_init(bq27510_coul_firmware_update_init);
#endif

#if defined(FEATURE_NV_FLASH_ON)

/*lint -save -e19*/
module_init(bsp_nvm_kernel_init);
module_exit(bsp_nvm_exit);
/*lint -restore +e19*/
#elif defined(FEATURE_NV_EMMC_ON)

/*lint -save -e537*/
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/err.h>
#include <linux/printk.h>
#include <drv_rfile.h>
#include <drv_sync.h>
/*lint -restore +e537*/
struct sc_path_stru
{
    char new_path[64];
    char old_path[64];
};

static struct sc_path_stru g_sc_path[12] =
{
    {"/mnvm2:0/SC/Pers/CKFile.bin",      "/modem_log/yaffs0/SC/Pers/CKFile.bin"},
    {"/mnvm2:0/SC/Pers/DKFile.bin",      "/modem_log/yaffs0/SC/Pers/DKFile.bin"},
    {"/mnvm2:0/SC/Pers/AKFile.bin",      "/modem_log/yaffs0/SC/Pers/AKFile.bin"},
    {"/mnvm2:0/SC/Pers/PIFile.bin",      "/modem_log/yaffs0/SC/Pers/PIFile.bin"},
    {"/mnvm2:0/SC/Pers/ImeiFile_I0.bin", "/modem_log/yaffs0/SC/Pers/ImeiFile_I0.bin"},
    {"/mnvm2:0/SC/Pers/ImeiFile_I1.bin", "/modem_log/yaffs0/SC/Pers/ImeiFile_I1.bin"},
    {"/mnvm2:0/SC/Pers/CKSign.hash",     "/modem_log/yaffs0/SC/Pers/CKSign.hash"},
    {"/mnvm2:0/SC/Pers/DKSign.hash",     "/modem_log/yaffs0/SC/Pers/DKSign.hash"},
    {"/mnvm2:0/SC/Pers/AKSign.hash",     "/modem_log/yaffs0/SC/Pers/AKSign.hash"},
    {"/mnvm2:0/SC/Pers/PISign.hash",     "/modem_log/yaffs0/SC/Pers/PISign.hash"},
    {"/mnvm2:0/SC/Pers/ImeiFile_I0.hash","/modem_log/yaffs0/SC/Pers/ImeiFile_I0.hash"},
    {"/mnvm2:0/SC/Pers/ImeiFile_I1.hash","/modem_log/yaffs0/SC/Pers/ImeiFile_I1.hash"}
};

void sc_sigle_file_copy(const char* new_path,const char* old_path)
{
    FILE* new_fp = NULL;
    FILE* old_fp = NULL;
    void* file_data = NULL;
    int len = 0;
    int ret = 0;

    if((!BSP_access(new_path,0))||(BSP_access(old_path,0)))  /*���ļ����ڻ��߾��ļ������������追��*/
    {
        return;
    }

    old_fp = BSP_fopen(old_path,"rb");/*old file only to read*/
    new_fp = BSP_fopen(new_path,"wb+");/*new file use "wb+" to create*/
    if((NULL == new_fp)||(NULL == old_fp))
    {
        nv_mntn_record("open file err ,can not to translation %s to %s\n",old_path,new_path);
        return;
    }
    /*get old file length*/
    BSP_fseek(old_fp,0,SEEK_END);
    len = BSP_ftell(old_fp);
    BSP_fseek(old_fp,0,SEEK_SET);

    file_data = (void*)nv_malloc((unsigned int)len+1);
    if(!file_data)
    {
        nv_mntn_record("malloc fail %d ,can not to translation %s to %s\n",len,old_path,new_path);
        goto out1;
    }

    ret = BSP_fread(file_data,1, (unsigned int)len,old_fp);
    if(ret != len)
    {
        nv_mntn_record("read fail ret %d,len %d \n",ret,len);
        goto out;
    }

    ret = BSP_fwrite(file_data,1, (unsigned int)len,new_fp);
    if(ret != len)
    {
        nv_mntn_record("write fail ret %d,len %d \n",ret,len);
        goto out;
    }
    nv_free(file_data);
    BSP_fclose(new_fp);
    BSP_fclose(old_fp);
    return;

out:
    nv_free(file_data);
out1:
    BSP_fclose(new_fp);
    BSP_fclose(old_fp);
    BSP_remove(new_path);
    return;
}

void sc_file_copy(void)
{
    int i = 0;
    for(i =0;i<12;i++)
    {
        sc_sigle_file_copy(g_sc_path[i].new_path,g_sc_path[i].old_path);
    }
}
/*lint -save -e537*/
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/cdev.h>
#include <linux/proc_fs.h>
/*lint -restore +e537*/
/*lint -save -e26*/
static ssize_t Modem_NvRead(struct file *file,char __user *buf, size_t len, loff_t *ppos)
{
    return len;
}
/*lint -restore +e26*/
static ssize_t Modem_NvWrite(struct file *file,const char __user *buf, size_t len, loff_t *ppos)
{
    signed int ret;
    ret = nv_modify_upgrade_flag(true);
    if(ret)
        return ret;
    nv_mntn_record("%s %s modify upgrade flag success !\n",__DATE__,__TIME__);
    return len;
}
static const struct file_operations g_ModemNv ={
    .owner    = THIS_MODULE,
    .read     = Modem_NvRead,
    .write    = Modem_NvWrite,
};
/*lint -save -e745*/
int modemNv_ProcInit(void)
{
    if(NULL == proc_create("ModemNv", 0660, NULL, &g_ModemNv))
    {
        return -1;
    }
    return 0;
}

/* DTS2014053100296, y00171698 2014-05-31 Begin: */
static ssize_t Modem_ScRead(struct file *file,char __user *buf, size_t len, loff_t *ppos)
{
    return len;
}
/*lint -restore +e26*/
static ssize_t Modem_ScWrite(struct file *file,const char __user *buf, size_t len, loff_t *ppos)
{
    signed int ret;
    ret = sc_file_restore();
    if(ret)
        return ret;
    nv_mntn_record("%s %s sc file restore success !\n",__DATE__,__TIME__);
    return len;
}
static const struct file_operations g_ModemSc ={
    .owner    = THIS_MODULE,
    .read     = Modem_ScRead,
    .write    = Modem_ScWrite,
};
/*lint -save -e745*/

int modemSc_ProcInit(void)
{
    if(NULL == proc_create("ModemSc", 0660, NULL, &g_ModemSc))
    {
        return -1;
    }
    return 0;
}
/* DTS2014053100296, y00171698 2014-05-31 End! */

/*lint -restore +e745*/
#include <linux/mtd/mtd.h>
void modem_nv_delay(void)
{
    struct mtd_info* mtd;
    int i;

    /*��ȴ�ʱ��5s*/
    for(i=0;i<5000;i++)
    {
        if( 0 == (i+1)%1000 )/*ÿ����1s����ӡ��¼һ��*/
        {
            nv_printf("modem nv wait for mtd device %d ms\n",i);
        }
        mtd = get_mtd_device_nm(NV_BACK_SEC_NAME);
        if(IS_ERR(mtd)){
            nv_taskdelay(1);
            continue;
        }
        put_mtd_device(mtd);

        mtd = get_mtd_device_nm(NV_DLOAD_SEC_NAME);
        if(IS_ERR(mtd)){
            nv_taskdelay(1);
            continue;
        }
        put_mtd_device(mtd);

        mtd = get_mtd_device_nm(NV_SYS_SEC_NAME);
        if(IS_ERR(mtd)){
            nv_taskdelay(1);
            continue;
        }
        put_mtd_device(mtd);

        mtd = get_mtd_device_nm(NV_DEF_SEC_NAME);
        if(IS_ERR(mtd)){
            nv_taskdelay(1);
            continue;
        }
        put_mtd_device(mtd);

        return;
    }
}


static int __init modem_nv_probe(struct platform_device *dev)
{
    int ret = -1;

    g_nv_ctrl.pmState = NV_WAKEUP_STATE;
    g_nv_ctrl.opState = NV_IDLE_STATE;

    modem_nv_delay();

    if(BSP_access("/modem_log/modem_nv/",0))
        BSP_mkdir("/modem_log/modem_nv/");

    sc_file_copy();

    ret = bsp_nvm_kernel_init();

    ret |= modemNv_ProcInit();
    
    /* DTS2014053100296, y00171698 2014-05-31 Begin: */
    ret |= modemSc_ProcInit();
    /* DTS2014053100296, y00171698 2014-05-31 End! */

    BSP_SYNC_Give(SYNC_MODULE_NV);

    return ret;
}

#define NV_SHUTDOWN_STATE   NV_BOOT_INIT_OK
static void modem_nv_shutdown(struct platform_device *dev)
{
    struct nv_global_ddr_info_stru* ddr_info = (struct nv_global_ddr_info_stru*)NV_GLOBAL_INFO_ADDR;

    printk("%s shutdown start %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n",__func__);

    /*read only*/
    ddr_info->acore_init_state = NV_SHUTDOWN_STATE;
    ddr_info->ccore_init_state = NV_SHUTDOWN_STATE;
    ddr_info->mcore_init_state = NV_SHUTDOWN_STATE;

}
#ifdef CONFIG_PM
static s32 modem_nv_suspend(struct device *dev)
{
    static int count = 0;
    if(g_nv_ctrl.opState == NV_OPS_STATE)
    {
        printk(KERN_ERR"%s Modem nv in doing !\n",__func__);
        return -1;
    }
    g_nv_ctrl.pmState = NV_SLEEP_STATE;
    printk(KERN_ERR"Modem nv enter suspend! %d times\n",++count);
    return 0;
}
static s32 modem_nv_resume(struct device *dev)
{
    static int count;
    g_nv_ctrl.pmState = NV_WAKEUP_STATE;
    if(NV_OPS_STATE== g_nv_ctrl.opState)
    {
        printk(KERN_ERR"%s need to enter task proc!\n",__func__);
        osl_sem_up(&g_nv_ctrl.task_sem);
    }
    printk(KERN_ERR"Modem nv enter resume! %d times\n",++count);
    return 0;
}
static const struct dev_pm_ops modem_nv_pm_ops ={
	.suspend = modem_nv_suspend,
    .resume  = modem_nv_resume,
};

#define MODEM_NV_PM_OPS (&modem_nv_pm_ops)
#else
#define MODEM_NV_PM_OPS  NULL
#endif

static struct platform_driver modem_nv_drv = {
    .probe      = modem_nv_probe,
    .shutdown   = modem_nv_shutdown,
    .driver     = {
        .name     = "modem_nv",
        .owner    = THIS_MODULE,
        .pm       = MODEM_NV_PM_OPS,
    },
};


static struct platform_device modem_nv_device = {
    .name = "modem_nv",
    .id = 0,
    .dev = {
    .init_name = "modem_nv",
    },
};


static int __init modem_nv_init(void)
{
    int ret;

    ret = platform_device_register(&modem_nv_device);
    if(ret)
    {
        printk(KERN_ERR"platform_device_register modem_nv_device fail !\n");
        return -1;
    }

    ret = platform_driver_register(&modem_nv_drv);
    if(ret)
    {
        printk(KERN_ERR"platform_device_register modem_nv_drv fail !\n");
        platform_device_unregister(&modem_nv_device);
        return -1;
    }

    return 0;
}

static void __init modem_nv_exit(void)
{
    bsp_nvm_exit();
    platform_device_unregister(&modem_nv_device);
    platform_driver_unregister(&modem_nv_drv);
}

device_initcall(modem_nv_init);
module_exit(modem_nv_exit);

#endif

/*lint -restore*/

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif



