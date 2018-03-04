#include <osl_bio.h>
#include <product_config.h>
#include <drv_version.h>
#include <hi_syssc_interface.h>
#include <balongv7r2/types.h>
#include <bsp_shared_ddr.h>
#include <bsp_hkadc.h>
#include <boot/boot.h>
#include <balongv7r2/version.h>

#ifndef VER_ERROR
#define VER_ERROR (-1)
#endif

#ifndef VER_OK
#define VER_OK 0
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a) (sizeof(a)/(sizeof((a)[0])))
#endif

#define UDP_OFFSET 24
#define PRODUCT_OFFSET 16
#define BIG_VER_OFFSET 8


/* votage range */
typedef struct _tvol_range
{
    u16 vol_low;
    u16 vol_high;
}vol_range;


/* local variables */
static vol_range version_vol_tab[] =
{
    {0,147},
    {148,423},
    {424,699},
    {700,975},
    {976,1250},
    {1251,1526},
    {1527,1802},
    {1803,2078},
    {2079,2354},
    {2355,2500}
};


int balong_version_get_hkadc_value(hard_ware_version hw_ver,u8 *hkadc_ver)
{

/*TODO:waiting HKADC OK*/
	u16 adc_result = 0x00;/*�洢hkadc�����ĵ�ѹֵ*/
	u8 ret;
	u32 idx;

	/*get hkadc value*/
	ret=bsp_hkadc_convert((enum HKADC_CHANNEL_ID)hw_ver,&adc_result);
	if(VER_OK!=ret){
		cprintf("get hkadc value fail!\n");
		return VER_ERROR;
	}

	/*get version table value*/
	for(idx=0;idx<ARRAY_SIZE(version_vol_tab);idx++)
	{
		if(version_vol_tab[idx].vol_low<= adc_result
            && adc_result <= version_vol_tab[idx].vol_high)
		{
			/*cprintf("ver%d is:%d\n", hw_ver, idx);*/
			*hkadc_ver=idx;
			break;
		}
	}

	/* not found */
	if(idx == ARRAY_SIZE(version_vol_tab))
	{
		cprintf("get hkadc value fail, idx:%d, adc_result:%d\n", idx, adc_result);
		return VER_ERROR;
	}

	return VER_OK;

}

/*****************************************************************************
* �� �� ��  	: balong_version_get_hw_version
*
* ��������  : ���Ӳ���汾������
*
* �������  : ��
* �������  :
*
* �� �� ֵ  	: Ӳ���汾������
* �޸ļ�¼  :
*
*****************************************************************************/

u32 balong_version_get_hw_version(void)
{
	u8 udp          = 0;
	u8 product      = 0;
	u8 big_ver_high = 0;
	u8 big_ver_low  = 0;

	int ret         = VER_OK;
	u32 hardId      = 0xFFFFFFFF;
	u32 chip_type   = 0;
	u32 mask        = 0;

/*p531fpga*/
#if (defined(BSP_CONFIG_P531_ASIC)&&defined (BSP_CONFIG_BOARD_FPGA))
	return HW_VER_PRODUCT_P531_FPGA;
#endif

/*p531asic va*/
#if (defined (BSP_CONFIG_P531_ASIC)&&defined (BSP_CONFIG_BOARD_VA))
     return HW_VER_PRODUCT_P531_ASIC_VA;
#endif
/*p531asic vc*/
#if (defined (BSP_CONFIG_P531_ASIC)&&defined(BSP_CONFIG_BOARD_VC))
	return HW_VER_PRODUCT_P531_ASIC_VC;
#endif

/*v7r2 sft*/
#ifdef BSP_CONFIG_V7R2_SFT
	return HW_VER_PRODUCT_SFT;
#endif

/*v711 porting*/
#ifdef BSP_CONFIG_V711_PORTING
	return HW_VER_PRODUCT_PORTING;
#endif

	/*others by HKADC read*/
#if (FEATURE_ON == MBB_FEATURE_PHY_NUM)
	/*�ж��Ƿ�ΪUDP����*/
	/* coverity[un_reachable] */
	ret= balong_version_get_hkadc_value(HW_VERSION_UDP,&udp);
	if(VER_OK!=ret){
		cprintf("balong_version_get_hkadc_value HW_VERSION_UDP error!%d\n",ret);
		return HW_VER_INVALID;
	}
#endif
	/*��ѯ��汾�Ÿ�8bit*/
	ret=balong_version_get_hkadc_value(HW_VERSION_BIG_HIGH,&big_ver_high);

	if(VER_OK!=ret){
		cprintf("balong_version_get_hkadc_value HW_VERSION_BIG_HIGH error!%d\n",ret);
		return HW_VER_INVALID;
	}

	/*��ѯ��汾�ŵ�8bit*/
	ret=balong_version_get_hkadc_value(HW_VERSION_BIG_LOW,&big_ver_low);
	if(VER_OK!=ret){
		cprintf("balong_version_get_hkadc_value HW_VERSION_BIG_LOW error!%d\n",ret);
		return HW_VER_INVALID;
	}

	/*������UDP���壬��ѯ�����Ʒ��̬*/
#if (FEATURE_ON == MBB_FEATURE_PHY_NUM)
    cprintf("balong_version_get_hw_version doesn't judge udp!\n");
	udp = 1; /* ��Ʒ�߰汾Ĭ�ϲ��ж��Ƿ�ΪUDP */
#endif
	if(udp){
		ret=balong_version_get_hkadc_value(HW_VERSION_PRODUCT,&product);
		if(VER_OK!=ret){
			cprintf("balong_version_get_hkadc_value HW_VERSION_PRODUCT error!%d\n",ret);
			return HW_VER_INVALID;
		}

		/*�汾�ű�������н���udp�汾�Ÿ�8λ��0X00*/
		hardId=product<<PRODUCT_OFFSET|big_ver_high<<BIG_VER_OFFSET|big_ver_low;
		return hardId;
	}

	chip_type = bsp_version_get_chip_type();

	switch(chip_type){
		case CHIP_VER_HI6921_ASIC:
			mask = HW_VER_V711_UDP_MASK;
			break;
		case CHIP_VER_HI6930_ASIC:
			mask = HW_VER_UDP_MASK;
			break;
		case CHIP_VER_HI3630_ASIC:
			mask = HW_VER_K3V3_UDP_MASK;
			break;
		default:
			;
	}
	/*�汾�ű�������н�udp�汾�Ÿ�8λ��0X7F*/
	return (mask)|(big_ver_high<<BIG_VER_OFFSET)|big_ver_low;

}

/*****************************************************************************
* �� �� ��  	: bsp_save_hw_version
*
* ��������  : ��AXI MEMORY�д洢hw_id
*
* �������  : ��
* �������  :
*
* �� �� ֵ  	:
				0:�ɹ�
				-1:ʧ��
* �޸ļ�¼  :
*
*****************************************************************************/
int bsp_save_hw_version(void)
{
	u32 hw_main = HW_VER_INVALID;

    u32 *pu32HwId = (u32 *)SHM_MEM_HW_VER_ADDR;

	/*sub_Ver Ҳ��ͨ��HAKDCʶ��ģ������ע*/
    hw_main = balong_version_get_hw_version();
	if(HW_VER_INVALID==hw_main){
		cprintf("[bsp_save_hw_version]:get hw version failed!\n");
	}

    *pu32HwId = hw_main;
    cprintf("[bsp_save_hw_version]:get hw version ok, hw_main=0x%x!\n", hw_main);
	return VER_OK;

}

/*****************************************************************************
* ����	: bsp_get_board_chip_type
* ����	: get board type and chip type
* ����	: void
* ���	: void
* ����	: BOARD_TYPE_E
*****************************************************************************/
u32 bsp_version_get_board_type(void)
{
	u32 type = 0xffffffff;
	type = *(u32 *)SHM_MEM_HW_VER_ADDR;

	if(HW_VER_INVALID==type){
		cprintf("[bsp_get_board_chip_type]:get hw version failed!hw_ver:0x%x\n",type);
		return type;
	}

	if(HW_VER_UDP_MASK==(type & HW_VER_UDP_MASK)){
		return HW_VER_PRODUCT_UDP;
	}

    if(HW_VER_V711_UDP_MASK == (type & HW_VER_V711_UDP_MASK)){
        return HW_VER_V711_UDP;
    }

	return type;
}

/*****************************************************************************
* ����	: bsp_get_board_mode_type
* ����	: get board type(GUL or LTE)
* ����	: void
* ���	: void
* ����	: BOARD_TYPE_E
*****************************************************************************/
BOARD_TYPE_E bsp_get_board_mode_type(void)
{
#if(defined(FEATURE_MULTIMODE_GUL))
	return BOARD_TYPE_GUL;
#else
	return BOARD_TYPE_LTE_ONLY;
#endif
}

/*****************************************************************************
* ����	: bsp_board_type_is_gutl
* ����	: get board type is GUTL
* ����	: void
* ���	: void
* ����	: BOARD_TYPE_E
*****************************************************************************/
bool bsp_board_type_is_gutl(void)
{
	return (BOARD_TYPE_GUL == bsp_get_board_mode_type() ? 1 : 0);
}

/*****************************************************************************
* ����	: bsp_version_get_chip_type
* ����	: get chip type
* ����	: void
* ���	: void
* ����	: chip id 
*
* ����˵��  : ��
*
*****************************************************************************/
u32 bsp_version_get_chip_type(void)
{
	return get_hi_version_id_version_id();
}

/*****************************************************************************
* ����	: bsp_get_board_actual_type
* ����	: get board actual type 
* ����	: void
* ���	: void
* ����	: BOARD_ACTUAL_TYPE_E       BBIT/SFT/ASIC
*
* ����˵��  : ��
*
*****************************************************************************/
BOARD_ACTUAL_TYPE_E bsp_get_board_actual_type(void)
{
	u32 chip_ver = 0;

	chip_ver = bsp_version_get_chip_type();

	switch(chip_ver){
		case CHIP_VER_P531_ASIC:
		case CHIP_VER_P532_ASIC:
			return BOARD_TYPE_BBIT;

		case CHIP_VER_HI6950_ASIC:			
		case CHIP_VER_HI3630_ASIC:
		case CHIP_VER_HI6921_ASIC:
		case CHIP_VER_HI6930_ASIC:
			return BOARD_TYPE_ASIC;

		case CHIP_VER_HI6950_SFT:
			return BOARD_TYPE_SFT;

		default:
			return BOARD_TYPE_MAX;		
	}
}


