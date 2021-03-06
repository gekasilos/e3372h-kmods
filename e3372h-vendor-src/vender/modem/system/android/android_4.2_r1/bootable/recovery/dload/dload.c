/*************************************************************************
*   版权所有(C) 1987-2020, 深圳华为技术有限公司.
*
*   文 件 名 :  dload.c
*
*
*   描    述 :  下载数据包处理
*
*
*************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/reboot.h>
#include <pthread.h>
#include "../oeminfo.h"
#include "dload_comm.h"
#include "usb_acm.h"
#include "dload_lib.h"
#include "dload.h"
#include "hdlc.h"
#include "at.h"
#include "cutils/android_reboot.h"

#define IMAGE_VERSION_SIZE (128)
#define MAX_CDROMISO_SIZE (0x2800000)

pthread_mutex_t Dbug_lock = PTHREAD_MUTEX_INITIALIZER;

struct dload_ctrl_block *dcb;

/*下面的变量地址要保证4字节对齐*/
char hdlc_cmd_val[][8] = {
       /*ACK*/                     {0x7E, 0x02, 0x6A, 0xD3, 0x7E},                    /*0x3A*/                                  
      /*CMD_NAK_INVALID_FCS*/      {0x7E, 0x03, 0x00, 0x01, 0x21, 0x38, 0x7E},        /*CRC校验错误*/ 
      /*CMD_NAK_INVALID_DEST*/     {0x7E, 0x03, 0x00, 0x02, 0xBA, 0x0A, 0x7E},        /*目标内存空间超出范围*/ 
      /*CMD_NAK_INVALID_LEN*/      {0x7E, 0x03, 0x00, 0x03, 0x33, 0x1B, 0x7E},        /*接收到的与预置的长度不一致*/ 
      /*CMD_NAK_EARLY_END*/        {0x7E, 0x03, 0x00, 0x04, 0x8C, 0x6F, 0x7E},        /*数据长度过短*/ 
      /*CMD_NAK_TOO_LARGE*/        {0x7E, 0x03, 0x00, 0x05, 0x05, 0x7D, 0x5E, 0x7E},  /*数据长度过长*/
      /*CMD_NAK_INVALID_CMD*/      {0x7E, 0x03, 0x00, 0x06, 0x9E, 0x4C, 0x7E},        /*无效的命令*/ 
      /*CMD_NAK_FAILED*/           {0x7E, 0x03, 0x00, 0x07, 0x17, 0x5D, 0x7E},        /*操作无法完成*/
      /*CMD_NAK_WRONG_IID*/        {0x7E, 0x03, 0x00, 0x08, 0xE0, 0xA5, 0x7E},        /*未使用*/
      /*CMD_NAK_BAD_VPP*/          {0x7E, 0x03, 0x00, 0x09, 0x69, 0xB4, 0x7E},        /*未使用*/
      /*CMD_NAK_VERIFY_FAILED*/    {0x7E, 0x03, 0x00, 0x0A, 0xF2, 0x86, 0x7E},        /*鉴权失败,没有权限操作*/ 
      /*CMD_NAK_NO_SEC_CODE*/      {0x7E, 0x03, 0x00, 0x0B, 0x7B, 0x97, 0x7E},        /*没有安全码*/
      /*CMD_NAK_BAD_SEC_CODE*/     {0x7E, 0x03, 0x00, 0x0C, 0xC4, 0xE3, 0x7E},        /*安全码错误/无效*/
      /*CMD_Reserved*/             {0x7E, 0x03, 0x00, 0x0D, 0x4D, 0xF2, 0x7E},        /*保留*/
      /*CMD_NAK_OP_NOT_PERMITTED*/ {0x7E, 0x03, 0x00, 0x0E, 0xD6, 0xC0, 0x7E},        /*现特性不允许该操作*/
      /*CMD_NAK_INVALID_ADDR*/     {0x7E, 0x03, 0x00, 0x0F, 0x5F, 0xD1, 0x7E},        /*此地址的内存无法访问*/                                     
      /*CMD_DLOAD_SWITCH*/         {0x7E, 0x3A, 0xA1, 0x6E, 0x7E},                    /*0x3A*/                
      /*CMD_CDROM_STATUS*/         {0x7E, 0xFB, 0x00, 0xE7, 0x97, 0x7E},              /*0xFB+0x00(无需删除CDROM)*/
                                   {0},
                                   {0}                                                                                                                                  
    };
/*****************************************************************************
* 函 数 名  : 
*
* 功能描述  : 获取hdlc 命令
*
* 输入参数  : 
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
char * dload_get_hdlc_cmd(unsigned id)
{
	if(id>CMD_CDROM_STATUS_ID||id<CMD_ACK_ID){
		Dbug("Error:COM ID is invalid\n");
		return NULL;
	}
	/*下面的变量地址要保证4字节对齐*/
	#if 0
	char hdlc_cmd_val[][8] = {
	       /*ACK*/                     {0x7E, 0x02, 0x6A, 0xD3, 0x7E},                    /*0x3A*/                                  
	      /*CMD_NAK_INVALID_FCS*/      {0x7E, 0x03, 0x00, 0x01, 0x21, 0x38, 0x7E},        /*CRC校验错误*/ 
	      /*CMD_NAK_INVALID_DEST*/     {0x7E, 0x03, 0x00, 0x02, 0xBA, 0x0A, 0x7E},        /*目标内存空间超出范围*/ 
	      /*CMD_NAK_INVALID_LEN*/      {0x7E, 0x03, 0x00, 0x03, 0x33, 0x1B, 0x7E},        /*接收到的与预置的长度不一致*/ 
	      /*CMD_NAK_EARLY_END*/        {0x7E, 0x03, 0x00, 0x04, 0x8C, 0x6F, 0x7E},        /*数据长度过短*/ 
	      /*CMD_NAK_TOO_LARGE*/        {0x7E, 0x03, 0x00, 0x05, 0x05, 0x7D, 0x5E, 0x7E},  /*数据长度过长*/
	      /*CMD_NAK_INVALID_CMD*/      {0x7E, 0x03, 0x00, 0x06, 0x9E, 0x4C, 0x7E},        /*无效的命令*/ 
	      /*CMD_NAK_FAILED*/           {0x7E, 0x03, 0x00, 0x07, 0x17, 0x5D, 0x7E},        /*操作无法完成*/
	      /*CMD_NAK_WRONG_IID*/        {0x7E, 0x03, 0x00, 0x08, 0xE0, 0xA5, 0x7E},        /*未使用*/
	      /*CMD_NAK_BAD_VPP*/          {0x7E, 0x03, 0x00, 0x09, 0x69, 0xB4, 0x7E},        /*未使用*/
	      /*CMD_NAK_VERIFY_FAILED*/    {0x7E, 0x03, 0x00, 0x0A, 0xF2, 0x86, 0x7E},        /*鉴权失败,没有权限操作*/ 
	      /*CMD_NAK_NO_SEC_CODE*/      {0x7E, 0x03, 0x00, 0x0B, 0x7B, 0x97, 0x7E},        /*没有安全码*/
	      /*CMD_NAK_BAD_SEC_CODE*/     {0x7E, 0x03, 0x00, 0x0C, 0xC4, 0xE3, 0x7E},        /*安全码错误/无效*/
	      /*CMD_Reserved*/             {0x7E, 0x03, 0x00, 0x0D, 0x4D, 0xF2, 0x7E},        /*保留*/
	      /*CMD_NAK_OP_NOT_PERMITTED*/ {0x7E, 0x03, 0x00, 0x0E, 0xD6, 0xC0, 0x7E},        /*现特性不允许该操作*/
	      /*CMD_NAK_INVALID_ADDR*/     {0x7E, 0x03, 0x00, 0x0F, 0x5F, 0xD1, 0x7E},        /*此地址的内存无法访问*/                                     
	      /*CMD_DLOAD_SWITCH*/         {0x7E, 0x3A, 0xA1, 0x6E, 0x7E},                    /*0x3A*/                
	      /*CMD_CDROM_STATUS*/         {0x7E, 0xFB, 0x00, 0xE7, 0x97, 0x7E},              /*0xFB+0x00(无需删除CDROM)*/
	                                   {0},
	                                   {0}                                                                                                                                  
	    };
	#endif
	//printf("hdlc_cmd_id=%d\n",id);
	return hdlc_cmd_val[id];
}

/*****************************************************************************
* 函 数 名  : dload_cleanup
*
* 功能描述  : 全局变量清理
*
* 输入参数  : 
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/

void dload_cleanup(void)
{
    acm_usb_close();

    free(dcb);
}

/*****************************************************************************
* 函 数 名  : dcb_init
*
* 功能描述  : 下载控制块初始化
*
* 输入参数  : 
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
void dcb_init( void)
{
    if(NULL == dcb){
        Dbug("dcb == NULL\n");
        return;
    }
    
    dcb->image_type                = IMAGE_PART_TOP;/*当前升级部件的映像类型*/
    dcb->image_size                = 0;             /*当前升级部件的大小*/
    dcb->flash_current_offset      = 0xFFFFFFFF;    /*当前包在FLASH中的烧写地址,仅FLASH映像相关*/
    dcb->ram_addr                  = 0xFFFFFFFF;    /*当前接收到的包在RMA中的源地址*/     
    dcb->current_loaded_length     = 0;             /*当前接收到的包长度*/
    dcb->last_loaded_total_length  = 0;             /*上次为止已收到的包总长度*/
    dcb->last_loaded_packet        = 0;             /*上次已收到的包序号*/ 
    dcb->finish_image_type	=IMAGE_PART_TOP;
}

int dload_init(void)
{
	int fd;

    /*打开usb设备*/ 
    if (access(DEV_ACM_AT_NAME, OK) != 0) {
        Dbug(" [ can't access file %s ]\n", DEV_ACM_AT_NAME);
        return -1;
    }

    fd = acm_usb_open(DEV_ACM_AT_NAME);
    if(fd<0){
        return -1;
    }
/* 下载控制块 */
    dcb = calloc(1, sizeof(struct dload_ctrl_block));
   Dbug("dcb info :\ndload state:%d;\nimage type:%x\nimage size:%x\n",dcb->dload_state,dcb->image_type,dcb->image_size);
   Dbug("current_loaded_length:%d;\nram_addr:%x\nlast_loaded_total_length:%x\n",dcb->current_loaded_length,dcb->ram_addr,dcb->last_loaded_total_length);

    if(NULL == dcb){
        Dbug("calloc error\n");
        return -1;
    }
	
        
    return 0;
}

/*****************************************************************************
* 函 数 名  : get_dload_id
*
* 功能描述  : 获取DLOADID
*
* 输入参数  : 
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
int get_dload_id( char *pbuf, int len  )
{
//TODO:硬件版本号需要根据sysfs实现
    return ERROR;
}

/*****************************************************************************
* 函 数 名  : get_ver
*
* 功能描述  : 获取软件版本号
*
* 输入参数  : 
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
void get_software_version( char *pbuf, int len  )
{
    
}

/*****************************************************************************
* 函 数 名  : cmd_verify_rsp
*
* 功能描述  : 响应PC工具的DloadID匹配命令
*
* 输入参数  : 
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
int cmd_verify_rsp( void )
{
    int str_len=0;
    int send_len;
    char dload_id[64];
    char *org_msg_buf=NULL;
    char *hdlc_send_buf=NULL;
	
    org_msg_buf=(char *)malloc(HDLC_SEND_BUF_MAX_LEN);
    if(NULL==org_msg_buf){
	Dbug("Error:malloc fail!\n");
	return ERROR;
    }

    hdlc_send_buf=(char *)malloc(HDLC_SEND_BUF_MAX_LEN);
    if(NULL==hdlc_send_buf){
	Dbug("Error:malloc fail!\n");
	free(org_msg_buf);
	return ERROR;

    }
    memset(org_msg_buf, 0, HDLC_SEND_BUF_MAX_LEN);

    org_msg_buf[0] = CMD_VERIFY;
    str_len++;

    strcat(org_msg_buf, CMD_VERIFY_FRONT_VAL);
    str_len += strlen(CMD_VERIFY_FRONT_VAL);

    memset(dload_id, 0, 64);    
    if(get_dload_id(dload_id, 64))
    {
        strcat(dload_id, STUB_STR_HW_VER);
    }
    
    strcat(org_msg_buf, dload_id);
    str_len += strlen(dload_id);

    str_len = (CMD_VERIFY_LEN+1) > str_len ? (CMD_VERIFY_LEN+1) : str_len;

    /*HDLC封装*/
    if(HDLC_Encap(org_msg_buf, str_len, hdlc_send_buf, HDLC_SEND_BUF_MAX_LEN, &send_len)){
       // acm_write(hdlc_cmd_val[7], 7);
       //printf("cmd_id=7\n");
       acm_write(dload_get_hdlc_cmd(CMD_NAK_FAILED_ID),CMD_NAK_FAILED_LEN);
	free(org_msg_buf);
	free(hdlc_send_buf);
        return ERROR;
    }

    acm_write(hdlc_send_buf, send_len);
    free(org_msg_buf);
    free(hdlc_send_buf);
    return OK;
    
}

/*****************************************************************************
* 函 数 名  : cmd_get_version_rsp
*
* 功能描述  :  获取软件版本号
*
* 输入参数  : 
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
int cmd_get_version_rsp( void )
{
    char ver[VER_LEN_MAX];
    int str_len, send_len;
    char *org_msg_buf=NULL;
    char *hdlc_send_buf=NULL;
    org_msg_buf=(char *)malloc(HDLC_SEND_BUF_MAX_LEN);
    if(NULL==org_msg_buf){
	Dbug("Error:malloc fail!\n");
	return ERROR;
    }

    hdlc_send_buf=(char *)malloc(HDLC_SEND_BUF_MAX_LEN);
    if(NULL==hdlc_send_buf){
	Dbug("Error:malloc fail!\n");
	free(org_msg_buf);
	return ERROR;

    }

    memset(ver, 0, VER_LEN_MAX);
    memset(org_msg_buf, 0, HDLC_SEND_BUF_MAX_LEN);

    get_software_version(ver, VER_LEN_MAX);

    org_msg_buf[0] = (char)CMD_TCPU_VER;
    org_msg_buf[1] = (char)CMD_TCPU_VER_LEN;
    strcat(org_msg_buf, ver);
    str_len = CMD_TCPU_VER_LEN+2;
    
    /*HDLC封装*/
    if(HDLC_Encap(org_msg_buf, str_len, hdlc_send_buf, HDLC_SEND_BUF_MAX_LEN, &send_len)){
        //acm_write(hdlc_cmd_val[7], 7);
        acm_write(dload_get_hdlc_cmd(CMD_NAK_FAILED_ID),CMD_NAK_FAILED_LEN);
	free(org_msg_buf);
	free(hdlc_send_buf);
        return ERROR;
    }

    acm_write(hdlc_send_buf, send_len);
    free(org_msg_buf);
    free(hdlc_send_buf);
    return 0;
}

/*****************************************************************************
* 函 数 名  : cmd_get_bcpu_ver_rsp
*
* 功能描述  : 获取B版本号
*
* 输入参数  : 
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
int cmd_get_bcpu_ver_rsp( void )
{
    char ver[VER_LEN_MAX];
    int str_len, send_len;
    char *org_msg_buf=NULL;
    char *hdlc_send_buf=NULL;
		
    org_msg_buf=(char *)malloc(HDLC_SEND_BUF_MAX_LEN);
    if(NULL==org_msg_buf){
	Dbug("Error:malloc fail!\n");
	return ERROR;
    }

    hdlc_send_buf=(char *)malloc(HDLC_SEND_BUF_MAX_LEN);
    if(NULL==hdlc_send_buf){
	Dbug("Error:malloc fail!\n");
	free(org_msg_buf);
	return ERROR;
    }

    memset(ver, 0, VER_LEN_MAX);
    memset(org_msg_buf, 0, HDLC_SEND_BUF_MAX_LEN);

    get_software_version(ver, VER_LEN_MAX);

    org_msg_buf[0] = (char)CMD_BCPU_VER;
    org_msg_buf[1] = (char)0xC9;
    org_msg_buf[2] = (char)0x03;
    org_msg_buf[3] = (char)0xFF;
    strcat(org_msg_buf, ver);
    str_len = CMD_BCPU_VER_LEN+2;
    
    /*HDLC封装*/
    if(HDLC_Encap(org_msg_buf, str_len, hdlc_send_buf, HDLC_SEND_BUF_MAX_LEN, &send_len)){
        //acm_write(hdlc_cmd_val[7], 7);
        acm_write(dload_get_hdlc_cmd(CMD_NAK_FAILED_ID),CMD_NAK_FAILED_LEN);
	free(org_msg_buf);
	free(hdlc_send_buf);
        return ERROR;
    }

    acm_write(hdlc_send_buf, send_len);
    free(org_msg_buf);
    free(hdlc_send_buf);
    return OK;
}

/*****************************************************************************
* 函 数 名  : hdlc_send_cmd
*
* 功能描述  : 向PC发送HDLC回复命令
*
* 输入参数  : enum dload_cmd  
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
int hdlc_send_cmd(enum dload_cmd cmd)
{
    char hdlc_cmd_val_rsp[24]   = {0x0D, 0x0F, 0x37, 0x32, 0x30, 0x30, 0x42, 0x2D, 0x2D, 0x53, 0x4B, 0x43, 0x42, 0x41, 0x44, 0x5A, 0x4D, 0xC3, 0x7B, 0x7E};     /*0x0D+len+7200B--SKCBADZM*/

    switch (cmd){
        case CMD_ACK:
        	acm_write(dload_get_hdlc_cmd(CMD_ACK_ID),CMD_ACK_LEN);
        break;
        
        case CMD_NAK_FAILED:
        {
           	acm_write(dload_get_hdlc_cmd(CMD_NAK_FAILED_ID),CMD_NAK_FAILED_LEN);
        }
        break;
        
        case CMD_NAK_EARLY_END:
        	acm_write(dload_get_hdlc_cmd(CMD_NAK_EARLY_END_ID), CMD_NAK_EARLY_END_LEN);
        break;
        
        case CMD_NAK_INVALID_LEN:        	
        	acm_write(dload_get_hdlc_cmd(CMD_NAK_INVALID_LEN_ID),CMD_NAK_INVALID_LEN_LEN);
        break;
        
        case CMD_DLOAD_SWITCH:
        	acm_write(dload_get_hdlc_cmd(CMD_DLOAD_SWITCH_ID),CMD_DLOAD_SWITCH_LEN);
        break;
        
        case CMD_CDROM_STATUS:
        	acm_write(dload_get_hdlc_cmd(CMD_CDROM_STATUS_ID),CMD_CDROM_STATUS_LEN);
        break;
        
        case CMD_VERRSP:
        {
            	acm_write(hdlc_cmd_val_rsp, 20);
        }
        break;

        /*单板版本ID信息回复,需与将下载的映像文件的硬件版本完全一致,否则不能下载*/
        case CMD_VERIFY:
        {
            return cmd_verify_rsp();
        }
        break;
        
        case CMD_TCPU_VER:
        {
           /*获取单板软件版本信息字符串信息*/            
           return cmd_get_version_rsp();
        }
        break;
        case CMD_BCPU_VER:
        {
           /*获取单板软件版本信息字符串信息*/            
           return cmd_get_bcpu_ver_rsp();
        }
        break;
        
        default:
        return ERROR;
    }
	return OK;
}

/*****************************************************************************
* 函 数 名  : pkt_process
*
* 功能描述  : 
*
* 输入参数  : void  
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
int pkt_process(DLOAD_HDLC_S *pkt)
{
    enum dload_cmd cmd_recv, cmd_send;
    char *pbuf = pkt->au8DecapBuf;

    cmd_recv = (enum dload_cmd)*pbuf;
    cmd_send = CMD_NAK_INVALID_CMD;
    unsigned int ulPacketNum = 0;    
    static struct dload_buf_ctrl write_buf={0};
    static unsigned int align_size;
    static struct version_image_type iso_ver;
    static struct version_image_type webui_ver;
    enum IMAGE_TYPE finish_image_type;

    switch(cmd_recv){
        /*升级结束,工具将发命令重启单板*/
        case CMD_RESET:
        {
            if( 1 != pkt->u32InfoLen )
            {
                cmd_send = CMD_NAK_INVALID_LEN;
            }
            else
            {
                hdlc_send_cmd(CMD_ACK); 

                /*升级成功*/
		state_show(DLOAD_STATE_SUCCESS);
		usleep(1000*1000);    /*1s*/
		
                //TODO:保存分区表，重启;                
                reboot(RB_AUTOBOOT);  // reboot immediately, with dirty filesystems
                Dbug("Reboot failed?!\n");
		exit(0);
            }
        }
        break;

        /*下载模式切换虚拟命令,UE侧无需任何动作*/
        case CMD_DLOAD_SWITCH:
        {
            if( 1 != pkt->u32InfoLen )
            {
                cmd_send = CMD_NAK_INVALID_LEN;
            }
            else
            {
                cmd_send = CMD_DLOAD_SWITCH;
            }        
        }
        break;        
        
        /*用于工具判断是否已正确进入DLOAD模式*/
        case CMD_VERREQ:
        {
            if( 1 != pkt->u32InfoLen )
            {
                cmd_send = CMD_NAK_INVALID_LEN;
                break;
            }
            
            if(DLOAD_STATE_TF_RUNNING == dcb->dload_state)    
            {
                /*已进入TF卡升级模式,返回NAK并点灯报警*/
                /*dloadLEDInd(DLOAD_FAILED);*/
                cmd_send = CMD_NAK_FAILED;
                Dbug("error:[CMD_VERREQ] tf update is running.\n");
            }
            else
            {                
                /*设置当前加载状态为USB加载中,并点灯指示*/
                dcb->dload_state = DLOAD_STATE_USB_RUNNING;
                state_show(DLOAD_STATE_USB_RUNNING);
                cmd_send = CMD_VERRSP;
            }       
        }
        break;
        
        /*单板硬件版本信息查询*/
        case CMD_VERIFY:
        {
            if( 1 != pkt->u32InfoLen )
            {
                cmd_send = CMD_NAK_INVALID_LEN;
            }
            else
            {
                cmd_send = CMD_VERIFY;
            }
            
            if(DLOAD_STATE_TF_RUNNING == dcb->dload_state)    
            {
                /*已进入TF卡升级模式,返回NAK并点灯报警*/
                cmd_send = CMD_NAK_FAILED;
		printf("error:[CMD_VERREQ] tf update is running.\n");
                Dbug("error:[CMD_VERREQ] tf update is running.\n");
            }
            else
            {                
                /*设置当前加载状态为USB加载中,并点灯指示*/
                dcb->dload_state = DLOAD_STATE_USB_RUNNING;
                state_show(DLOAD_STATE_USB_RUNNING);
                cmd_send = CMD_VERIFY;
            } 
            
        }
        break;

        /*单板软件版本信息查询*//*仅存在于强制加载模式*/
        case CMD_TCPU_VER: 
        {   
            if( 1 != pkt->u32InfoLen )
            {
                cmd_send = CMD_NAK_INVALID_LEN;
            }
            else
            {
                cmd_send = CMD_TCPU_VER;
            }            
        }
        break;
        /*单板软件版本信息查询*//*仅存在于bootrom二次重启优化方案 by 
m00176101 */
        case CMD_BCPU_VER: 
        {   
            if( 1 != pkt->u32InfoLen )/*遗留问题*/
            {
                cmd_send = CMD_BCPU_VER;

            }
            else
            {
                cmd_send = CMD_BCPU_VER;
            }            
        }
        break;
		
        /*CDROM状态查询*/
        case CMD_CDROM_STATUS: 
        {   
            if( 1 != pkt->u32InfoLen )
            {
                cmd_send = CMD_NAK_INVALID_LEN;
            }
            else
            {
                cmd_send = CMD_CDROM_STATUS;
            }
        }
        break;

        /*一个部件升级初始化*/
        case CMD_DLOAD_INIT:
        {
            Dbug("\r\nCMD_DLOAD_INIT!!\n");
            if( 12 != pkt->u32InfoLen ) 
            {
                cmd_send = CMD_NAK_INVALID_LEN;
                break;
            }            
            write_buf.used = 0;
            dcb_init();
            
	    state_show(DLOAD_STATE_USB_RUNNING);

            HDLC_Init(&pkt);
            
            pkt->u32DecapBufSize = HDLC_PACKET_MAXLEN;
            
            /*获取当前升级部件的映像类型*/
            dcb->image_type = (enum IMAGE_TYPE)HighToLow(pkt->au8DecapBuf + 1, 4);

	    /*设置升级buf的大小*/
	    if(IMAGE_SYSTEM==dcb->image_type)
	    {
		align_size= DLOAD_NAND_SPARE_ALIGN_ADD;
	    }
	    else
	    {
		align_size = DLOAD_NAND_SPARE_ALIGN;
	    }
		
            /*获取当前升级部件的大小*/
            dcb->image_size = HighToLow(pkt->au8DecapBuf + 5, 4);
            if( 0 == dcb->image_size )
            {
                cmd_send = CMD_NAK_FAILED;
                Dbug("dcb->image_size=0x%x error.\n", dcb->image_size);
            }       
            
            if(dload_buf_init(dcb->image_type, dcb->image_size,&write_buf))
            {
                cmd_send = CMD_NAK_FAILED;
                Dbug("dloadUSBRecvPro: dloadInitRamBuffer error.\n");
                break;                
            }

            Dbug("image_size=%d\n",dcb->image_size);

            /*在dload iso verison 文件初始化命令时，先擦除isoversion分区*/ 
            if((IMAGE_CDROMISOVER == dcb->image_type)||( IMAGE_WEBUIVER == dcb->image_type))
            {
                if(IMAGE_CDROMISOVER == dcb->image_type)
                {
		        oeminfo_erase(OEMINFO_ISO_VER_TYPE);
                }
                else
                {
			oeminfo_erase(OEMINFO_WEBUI_TYPE);
                }
				
                Dbug("pkt_process: erase ISO or webui version block !\n");
            }

            /*如果升级的是后台，判断后台大小是否合适*/
            if(IMAGE_CDROMISO == dcb->image_type)
            {
                if(dcb->image_size > MAX_CDROMISO_SIZE)/*大于88MB ，不升级*/
                {
                    cmd_send = CMD_NAK_FAILED;
                    Dbug("pkt_process: image_size=0x%x is more than 88MB!\n",(int)dcb->image_size);
                    break;
                }
            }

            cmd_send = CMD_ACK;                        
#if 0
            if(dloadNeedMidwayRestart())
            {
               /*迫使工具发送单板复位命令*/
                taskDelay(5);  /*加入延迟防PC卡死:遗留问题 m00176101*/  
                Dbug("dloadNeedMidwayRestart!\n");
                cmd_send = CMD_NAK_FAILED;
                taskDelay(5);/*加入延迟防PC卡死:遗留问题*/
            }
            
            #if defined (DLOAD_DEBUG_USB)
            Dbug("pkt_process: Type=0x%x,Size=0x%x.\n", dcb->image_type, dcb->image_size);
            #endif
#endif
        }


        break;

        /*升级包传输*/        
        case CMD_DATA_PACKET:
        { 
			/*Refresh LCD to show the updating image.*/
			state_show(DLOAD_STATE_USB_RUNNING);
	            
	            /*获取当前接收到的包长度*/       
	            dcb->current_loaded_length = HighToLow(pkt->au8DecapBuf + 5, 2);    //zfh
	            if( pkt->u32InfoLen!= dcb->current_loaded_length + 7 )
	            {
	                cmd_send = CMD_NAK_EARLY_END;
	                Dbug("pkt_process: pkt->u32InfoLen=0x%x,dcb->current_loaded_length=0x%x error.\n",pkt->u32InfoLen, (int)dcb->current_loaded_length);
	                break;
	            }              

	            /*获取当前接收包的序号*/
	            ulPacketNum = HighToLow(pkt->au8DecapBuf+ 1, 4);
				

	             /*有丢包或者包乱序,直接丢弃*/
	            if( (ulPacketNum > (dcb->last_loaded_packet + 1)) || (ulPacketNum < dcb->last_loaded_packet) )   
	            {
	                cmd_send = CMD_NAK_FAILED;
	                Dbug("pkt_process: packet discard, ulPacketNum=%d,dcb->last_loaded_packet=%d.\n", (int)ulPacketNum, (int)dcb->last_loaded_packet);
	                break;
	            }
	            else if( ulPacketNum == (dcb->last_loaded_packet) )   /*重复包*/
	            {          
	                Dbug("pkt_process: packet repeat, ulPacketNum=%d,dcb->last_loaded_packet=%d..\n", (int)ulPacketNum, (int)dcb->last_loaded_packet);
	                cmd_send = CMD_ACK;  /*重复包,直接丢弃,回复ACK*/
	                break;
	            }
	            else    /*(ulPacketNum == dcb->last_loaded_packet+1)顺序包正常烧写*/
	            {
	                ;
	            }
              
		 /*获取当前接收到的包在RMA中的源地址*/
		dcb->ram_addr = (unsigned int)(pkt->au8DecapBuf+ 7); 
		if( IMAGE_CDROMISOVER == dcb->image_type )	
                {
			 /*将下发的ISO version保存，等ISO文件升级完成后再写入文件系统*/ 
			 iso_ver.len= dcb->current_loaded_length;

			Dbug("pkt_process: g_ulIsoverLen=%d.\n", iso_ver.len);	
			if(IMAGE_NAME_SIZE>= iso_ver.len)
			{
	                        memcpy(iso_ver.buf, (void *)dcb->ram_addr, dcb->current_loaded_length);						
	                        cmd_send = CMD_ACK;			  
			}
			else
			{
	                        Dbug("pkt_process: g_ulIsoverLen too long fail.\n",1,2,3,4,5,6);
	                        cmd_send= CMD_NAK_FAILED;                    
			}
                }
                else if( IMAGE_WEBUIVER == dcb->image_type )
                {
			 /*将下发的ISO version保存，等ISO文件升级完成后再写入文件系统*/ 
			webui_ver.len= dcb->current_loaded_length;

			Dbug("pkt_process: g_ulWebUiverLen=%d.\n", (int)webui_ver.len);	
			if(IMAGE_VERSION_SIZE>= webui_ver.len)
			{
				memcpy(webui_ver.buf, (void *)dcb->ram_addr, dcb->current_loaded_length);						
				cmd_send = CMD_ACK;			  
			}
			else
			{
				Dbug("pkt_process: memcpy fail.\n");
				cmd_send = CMD_NAK_FAILED;                    
			}
                }
                /*将本数据包烧写至FLASH或文件系统*/
                else if(image_dload_process(&write_buf,align_size))
                {
			write_buf.used = 0; 
			dcb_init();
			Dbug("pkt_process: imageDloadProcess error.\n");
	                cmd_send= CMD_NAK_FAILED;
                }
                else
                { 
	                dcb->last_loaded_packet = ulPacketNum;                 
			cmd_send = CMD_ACK;
                }
	}
	break;

        /*一个部件升级结束*/
        case CMD_DLOAD_END:
        {
		finish_image_type=dcb->finish_image_type;
		if( 23 !=pkt->u32InfoLen)
		{
	                cmd_send = CMD_NAK_INVALID_LEN;
	                Dbug("pkt_process: usDestLen=0x%x error.\n", (int)pkt->u32InfoLen);
	                break;
            	} 

		/*判断如果为recovery镜像，需要修改分区表中的count值，以便支持双备份*/
		if(update_image_count(finish_image_type)){
			Dbug("Error: modify recovery count in ptalbe fail!\n");
			cmd_send = CMD_NAK_FAILED;
		}

            	/*将设置NV恢复标记位的操作移到一个部件升级完成后*/
            	/*当升级的组件有一个不为后台文件的时候，启动后cdrom不加载，进入nv恢复流程*/
		if((IMAGE_CDROMISOVER != dcb->image_type)\
		&&(IMAGE_CDROMISO != dcb->image_type)\
                &&(IMAGE_WEBUI != dcb->image_type)\
                &&(IMAGE_WEBUIVER != dcb->image_type))
            	{
                	dload_SetCdromMarker(FALSE);
		}

		cmd_send = CMD_ACK; 
		if(IMAGE_CDROMISO == finish_image_type)
		{
			/* initialize flash device */			
			if (!oeminfo_init(OEMINFO_ISO_VER_TYPE,iso_ver.len))
			{
				Dbug("oeminfo partition init failed!\n");
				cmd_send = CMD_NAK_FAILED;
			}
			if(!oeminfo_program(OEMINFO_ISO_VER_TYPE,iso_ver.buf,iso_ver.buf))
			{
				Dbug("oeminfo partition init failed!\n");
				cmd_send = CMD_NAK_FAILED;
			}

			if(!oeminfo_finalize(OEMINFO_ISO_VER_TYPE))
			{
				Dbug("oeminfo partition finalize faild!\n");
				cmd_send = CMD_NAK_FAILED;
			}
		}
		else if( IMAGE_WEBUI == finish_image_type )
		{
			/* initialize flash device */			
			if (!oeminfo_init(OEMINFO_WEBUI_TYPE,webui_ver.len))
			{
				Dbug("oeminfo partition init failed!\n");
				cmd_send = CMD_NAK_FAILED;
			}
			if(!oeminfo_program(OEMINFO_WEBUI_TYPE,webui_ver.buf,webui_ver.buf))
			{
				Dbug("oeminfo partition write failed!\n");
				cmd_send = CMD_NAK_FAILED;
			}

			if(!oeminfo_finalize(OEMINFO_WEBUI_TYPE))
			{
				Dbug("oeminfo partition finalize faild!\n");
				cmd_send = CMD_NAK_FAILED;
			}

                }
		else if(IMAGE_PTABLE==finish_image_type)
		{
			/*判断如果为ptable镜像，需要修改分区表中的count值，以便支持双备份*/
			if(update_ptable_count()){
				Dbug("Error: modify recovery count in ptalbe fail!\n");
				cmd_send = CMD_NAK_FAILED;
			}
			int ret=get_ptable_changed();
			if(ret>0){
				Dbug("ptable changed ,system will reboot\n");
				dload_reboot();
			}
			else if(ret<0)
			{
				Dbug("Error:can't get ptable is changed\n");
				cmd_send=CMD_NAK_FAILED;
			}
			else{
				;
			}
		}
		else{
			;
		}			
            
	   printf("\nimagetype:%xdload_seccuss!\n",dcb->image_type);
           dcb->finish_image_type = IMAGE_PART_TOP;
            /*本映像正常烧写完毕，将全局变量复位*/
            write_buf.used = 0;    /*清空g_stDloadWriteBuffer ,不需要释放么�*/ 
            dcb_init();    /*清空DCB表*/
        
            Dbug("CMD_DLOAD_END!!\n\n");
        }
        break;
        
        default:    /*无效的命令*/ 
	cmd_send = CMD_NAK_INVALID_CMD;
	}
	
	hdlc_send_cmd(cmd_send);
	return 0;
}

int dload_main(void)
{
	int fd;
	int i;
	int ret;
	int read_len;
	char *recv_buf, *send_buf, *pbuf;
	static DLOAD_HDLC_S *hdlcpkt;

	/*全局变量的清理*/
	atexit(dload_cleanup);	
	#if defined(HAVE_FORKEXEC)
    	/*No SIGCHLD. Let the service subproc handle its children.*/ 
    		signal(SIGPIPE, SIG_IGN);
	#endif

	/*一键升级的准备工作*/
	/* hdlc初始化 */
    	hdlcpkt = calloc(1, sizeof(DLOAD_HDLC_S));
	if(NULL == hdlcpkt){
	    Dbug("Error: hdlcpkt calloc error\n");
	    return ERROR;
	    }

    	HDLC_Init(&hdlcpkt);

	if(dload_init())
	{
		Dbug("Error:dload init fail!\n");
		free(hdlcpkt);
	}
	
    	recv_buf = malloc(RECV_BUF_SIZE);
	if(NULL == recv_buf)
	{
		Dbug("malloc error");
		free(hdlcpkt);

    	}
		
	send_buf = malloc(SEND_BUF_SIZE);
	if(NULL == send_buf)
	{
        	acm_usb_close();
        	free(recv_buf);
		free(hdlcpkt);
        	Dbug("malloc error");
	}
    
    	while(1)
	{
        	if(0 == (read_len = acm_read(recv_buf, RECV_BUF_SIZE)))
		{
            		continue;
        	}

        	/* AT命令处理 */
        	if(ATCOMMAND_PROCESS_SUCEED == at_cmd_proc(recv_buf,read_len))
		{
            		continue;
        	}

        	pbuf = recv_buf;
        
        	/* 数据包处理 */        
        	/*调用HDLC接口，将pBuf中的数据解析到pkt->au8DecapBuf*/
        	for ( i = 0; i < read_len; i++ )
        	{
            		ret = HDLC_Decap(hdlcpkt, *(pbuf + i) );

            		if ( HDLC_OK == ret )
            		{ 
                		/*一帧数据包接收完成*//*HDLC解封装函数通过0x7E标志符分帧*/     
                		pkt_process(hdlcpkt);
            		}
            		else if ( HDLC_ERR_INVALAID_FRAME == ret )
            		{
                		/*一帧数据包未接收完成*/
                		continue;
            		}
            		else
            		{
                 		/*HDLC解封装返回错误*/
                 		Dbug("Error: HdlcDecap return = %d error.\n", ret);
				free(hdlcpkt);
            		}
        	}
    	}
	return 0;
}
