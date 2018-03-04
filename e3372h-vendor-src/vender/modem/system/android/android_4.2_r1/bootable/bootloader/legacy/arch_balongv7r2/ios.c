/*********************************���ⵥ�޸ļ�¼*******************************
  ���ⵥ��               �޸���         �޸�ʱ��           �޸�˵��                             
******************************************************************************/
#include <boot/boot.h>
#include "product_config.h"
#include "bsp_reg_def.h"
#include "version.h"
#include "gpio.h"

#include "ios_list.h"
#include "ios.h"

#if defined(BSP_CONFIG_P531_ASIC)
#include "ios_drv_macro.h"
#include "ios_save.h"
#include "p531_va_ios_config.h"
#include "p531_vc_ios_config.h"

#elif (defined(BSP_CONFIG_V7R2_SFT))
#include "ios_ao_drv_macro.h"
#include "ios_pd_drv_macro.h"
#include "ios_ao_save.h"
#include "ios_pd_save.h"
#include "v7r2_porting_ios_config.h"

#elif (defined(BSP_CONFIG_V7R2_ASIC))
#include "ios_ao_drv_macro.h"
#include "ios_pd_drv_macro.h"
#include "ios_ao_save.h"
#include "ios_pd_save.h"
#include "udp_ios_pd_config.h"
#include "udp_ios_ao_config.h"
#include "udp_ios_pd_config_save.h"
#include "udp_ios_ao_config_save.h"
#include "e5379_ios_pd_config.h"
#include "e5379_ios_ao_config.h"
#include "e5379_ios_pd_config_save.h"
#include "e5379_ios_ao_config_save.h"

#elif (defined(BSP_CONFIG_V711_PORTING))
#include "ios_ao_drv_macro.h"
#include "ios_pd_drv_macro.h"
#include "ios_ao_save.h"
#include "ios_pd_save.h"
#include "v711_porting_ios_config.h"

#elif (defined(BSP_CONFIG_V711_ASIC))
#include "ios_ao_drv_macro.h"
#include "ios_pd_drv_macro.h"
#include "ios_ao_save.h"
#include "ios_pd_save.h"
#include "v711_udp_ios_ao_config.h"
#include "v711_udp_ios_ao_config_save.h"
#include "v711_udp_ios_pd_config.h"
#include "v711_udp_ios_pd_config_save.h"
#if (FEATURE_ON == MBB_COMMON)
/* ͨ��E5 GPIO����ͷ�ļ�֧�� */
/* e5 GPIO����ͷ�ļ�֧�� */
/* ����ͷ�ļ�ʱ�������õ�V7R11_E5 ,��V7R2���֣�ʹ�õ����׸�E5 E5573S-606������ */
#include "v711_e5_ios_pd_config.h"
#include "v711_e5_ios_ao_config.h"
#include "v711_e5_ios_pd_config_save.h"
#include "v711_e5_ios_ao_config_save.h"

#include "v711_e5578_ios_pd_config.h"
#include "v711_e5578_ios_ao_config.h"
#include "v711_e5578_ios_pd_config_save.h"
#include "v711_e5578_ios_ao_config_save.h"

/* CPE GPIO����ͷ�ļ�֧�� */
#include "cpe_ios_pd_config.h"
#include "cpe_ios_ao_config.h"
#include "cpe_ios_pd_config_save.h"
#include "cpe_ios_ao_config_save.h"
/* STICK GPIO����ͷ�ļ�֧�� */
#include "v711_stick_ios_pd_config.h"
#include "v711_stick_ios_ao_config.h"
#include "v711_stick_ios_pd_config_save.h"
#include "v711_stick_ios_ao_config_save.h"
/* WINGLE GPIO����ͷ�ļ�֧�� */
#include "v711_wingle_ios_pd_config.h"
#include "v711_wingle_ios_ao_config.h"
#include "v711_wingle_ios_pd_config_save.h"
#include "v711_wingle_ios_ao_config_save.h"

#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* V711��̬wm8990����i2c1, pcm��ԭzsi�����ó�����CONFIG_WM8990����Ĭ��Ϊ�� */
void wm8990_io_mux(void)
{
#if (defined(CONFIG_WM8990) && defined(BSP_CONFIG_V711_ASIC))

    /*i2c1_sda�ܽŸ�������*/
    CLR_IOS_I2C1_CTRL2_1;
    SET_IOS_I2C1_CTRL2_2;
    CLR_IOS_GPIO2_4_CTRL1_1;
    CLR_IOS_RGMII_CLK_CTRL1_1;
    CLR_IOS_UART1_CTRL3_1;
    CLR_IOS_LCD_CTRL3_1;
    /*i2c1_sda�ܽ�����������*/
    NASET_IOS_PD_IOM_CTRL32;

    /*i2c1_scl�ܽŸ�������*/
    CLR_IOS_I2C1_CTRL2_1;
    SET_IOS_I2C1_CTRL2_2;
    CLR_IOS_GPIO2_5_CTRL1_1;
    CLR_IOS_RGMII_CTRL1_1;
    CLR_IOS_UART1_CTRL3_1;
    CLR_IOS_SPI1_CS1_CTRL4_3;
    /*i2c1_scl�ܽ�����������*/
    NASET_IOS_PD_IOM_CTRL33;

    /*pcm_clk�ܽŸ�������*/
    CLR_IOS_PCM_CTRL2_2;
    SET_IOS_PCM_CTRL2_1;
    CLR_IOS_GPIO2_12_CTRL1_1;
    CLR_IOS_MMC0_CTRL1_1;
    /*pcm_clk�ܽ�����������*/
    NASET_IOS_PD_IOM_CTRL40;

    /*pcm_sync�ܽŸ�������*/
    CLR_IOS_PCM_CTRL2_2;
    SET_IOS_PCM_CTRL2_1;
    CLR_IOS_GPIO2_13_CTRL1_1;
    CLR_IOS_MMC0_CTRL1_1;
    /*pcm_sync�ܽ�����������*/
    NASET_IOS_PD_IOM_CTRL41;

    /*pcm_do�ܽŸ�������*/
    CLR_IOS_PCM_CTRL2_2;
    SET_IOS_PCM_CTRL2_1;
    OUTSET_IOS_PD_IOM_CTRL42;
    CLR_IOS_GPIO2_14_CTRL1_1;
    CLR_IOS_MMC0_CTRL1_1;
    /*pcm_do�ܽ�����������*/
    NASET_IOS_PD_IOM_CTRL42;

    /*pcm_di�ܽŸ�������*/
    CLR_IOS_PCM_CTRL2_2;
    SET_IOS_PCM_CTRL2_1;
    INSET_IOS_PD_IOM_CTRL43;
    CLR_IOS_GPIO2_15_CTRL1_1;
    CLR_IOS_MMC0_CTRL1_1;
    
    /*i2c1_sda�ܽŸ������ñ���*/
    add_ios_list(IOS_PD_MF_CTRL2);
    add_ios_list(IOS_PD_AF_CTRL5);
    /*i2c1_sda�ܽ����������ñ���*/
    add_ios_list(IOS_PD_IOM_CTRL32);

    /*i2c1_scl�ܽŸ������ñ���*/
    add_ios_list(IOS_PD_MF_CTRL2);
    add_ios_list(IOS_PD_AF_CTRL5);
    /*i2c1_scl�ܽ����������ñ���*/
    add_ios_list(IOS_PD_IOM_CTRL33);

   /*pcm_clk�ܽŸ������ñ���*/
    add_ios_list(IOS_PD_MF_CTRL2);
    add_ios_list(IOS_PD_AF_CTRL4);
    /*pcm_clk�ܽ����������ñ���*/
    add_ios_list(IOS_PD_IOM_CTRL40);

    /*pcm_sync�ܽŸ������ñ���*/
    add_ios_list(IOS_PD_MF_CTRL2);
    add_ios_list(IOS_PD_AF_CTRL4);
    /*pcm_sync�ܽ����������ñ���*/
    add_ios_list(IOS_PD_IOM_CTRL41);

    /*pcm_do�ܽŸ������ñ���*/
    add_ios_list(IOS_PD_MF_CTRL2);
    add_ios_list(IOS_PD_AF_CTRL4);
    add_ios_list(IOS_PD_IOM_CTRL42);
    /*pcm_do�ܽ����������ñ���*/
    add_ios_list(IOS_PD_IOM_CTRL42);

    /*pcm_di�ܽŸ������ñ���*/
    add_ios_list(IOS_PD_MF_CTRL2);
    add_ios_list(IOS_PD_AF_CTRL4);
    add_ios_list(IOS_PD_IOM_CTRL43);

#endif
}

void sep_io_mux(void)
{
#if ((FEATURE_ON == FEATURE_MULTI_MODEM) && defined(BSP_CONFIG_V7R2_ASIC))
	/*USIM1(3PIN)*/
    /*usim1_clk*/
    SET_IOS_USIM1_CTRL1_1;
    CLR_IOS_GPIO0_10_CTRL1_1;
    /*usim1_clk*/
    NASET_IOS_AO_IOM_CTRL14;

    /*usim1_rst*/
    SET_IOS_USIM1_CTRL1_1;
    OUTSET_IOS_AO_IOM_CTRL15;
    CLR_IOS_GPIO0_11_CTRL1_1;
    /*usim1_rst*/
    NASET_IOS_AO_IOM_CTRL15;

    /*usim1_data*/
    SET_IOS_USIM1_CTRL1_1;
    CLR_IOS_GPIO0_12_CTRL1_1;
    /*usim1_data*/
    PUSET_IOS_AO_IOM_CTRL16;

    /*usim1_clk*/
    add_ios_list(IOS_AO_MF_CTRL2);
    add_ios_list(IOS_AO_AF_CTRL2);
    /*usim1_clk*/
    add_ios_list(IOS_AO_IOM_CTRL14);

    /*usim1_rst*/
    add_ios_list(IOS_AO_MF_CTRL2);
    add_ios_list(IOS_AO_AF_CTRL2);
    add_ios_list(IOS_AO_IOM_CTRL15);
    /*usim1_rst*/
    add_ios_list(IOS_AO_IOM_CTRL15);

    /*usim1_data*/
    add_ios_list(IOS_AO_MF_CTRL2);
    add_ios_list(IOS_AO_AF_CTRL2);
    /*usim1_data*/
    add_ios_list(IOS_AO_IOM_CTRL16);
#endif
}

void pcm_io_mux(void)
{
#if defined(BSP_CONFIG_V7R2_ASIC)
    I4MASET_IOS_PD_IOM_CTRL21;
    /*pcm_clk�ܽŸ�������*/
    CLR_IOS_PCM_CTRL2_1;
    SET_IOS_PCM_CTRL2_2;
    CLR_IOS_MMC0_CLK_CTRL1_1;
    CLR_IOS_JTAG1_CTRL1_1;
    /*pcm_clk�ܽ�����������*/
    NASET_IOS_PD_IOM_CTRL21;

    /*pcm_sync�ܽŸ�������*/
    CLR_IOS_PCM_CTRL2_1;
    SET_IOS_PCM_CTRL2_2;
    CLR_IOS_MMC0_CTRL1_1;
    CLR_IOS_JTAG1_CTRL1_1;
    /*pcm_sync�ܽ�����������*/
    NASET_IOS_PD_IOM_CTRL22;

    /*pcm_di�ܽŸ�������*/
    CLR_IOS_PCM_CTRL2_1;
    SET_IOS_PCM_CTRL2_2;
    INSET_IOS_PD_IOM_CTRL23;
    CLR_IOS_MMC0_CTRL1_1;
    CLR_IOS_JTAG1_CTRL1_1;
    /*pcm_di�ܽ�����������*/
    PDSET_IOS_PD_IOM_CTRL23;

    /*pcm_do�ܽŸ�������*/
    CLR_IOS_PCM_CTRL2_1;
    SET_IOS_PCM_CTRL2_2;
    OUTSET_IOS_PD_IOM_CTRL24;
    CLR_IOS_MMC0_CTRL1_1;
    CLR_IOS_JTAG1_CTRL1_1;
    /*pcm_do�ܽ�����������*/
    NASET_IOS_PD_IOM_CTRL24;

    /*pcm_clk�ܽŸ������ñ���*/
    add_ios_list(IOS_PD_MF_CTRL1);
    add_ios_list(IOS_PD_AF_CTRL3);
    /*pcm_clk�ܽ����������ñ���*/
    add_ios_list(IOS_PD_IOM_CTRL21);

    /*pcm_sync�ܽŸ������ñ���*/
    add_ios_list(IOS_PD_MF_CTRL1);
    add_ios_list(IOS_PD_AF_CTRL3);
    /*pcm_sync�ܽ����������ñ���*/
    add_ios_list(IOS_PD_IOM_CTRL22);

    /*pcm_di�ܽŸ������ñ���*/
    add_ios_list(IOS_PD_MF_CTRL1);
    add_ios_list(IOS_PD_AF_CTRL3);
    add_ios_list(IOS_PD_IOM_CTRL23);
    /*pcm_di�ܽ����������ñ���*/
    add_ios_list(IOS_PD_IOM_CTRL23);

    /*pcm_do�ܽŸ������ñ���*/
    add_ios_list(IOS_PD_MF_CTRL1);
    add_ios_list(IOS_PD_AF_CTRL3);
    add_ios_list(IOS_PD_IOM_CTRL24);
    /*pcm_do�ܽ����������ñ���*/
    add_ios_list(IOS_PD_IOM_CTRL24);

#endif

}

void ios_init(void)
{

	u32 product_type = 0xff;
	product_type = bsp_version_get_board_type();

    switch(product_type)
    {
#if defined(BSP_CONFIG_P531_ASIC)
        case HW_VER_PRODUCT_P531_ASIC_VA:
            P531_VA_IOS_CONFIG;
            break;

        case HW_VER_PRODUCT_P531_ASIC_VC:
            P531_VC_IOS_CONFIG;
            IOS_SAVE;
            break;

        case HW_VER_PRODUCT_P531_FPGA:
            P531_VA_IOS_CONFIG;
            break;

#elif (defined(BSP_CONFIG_V7R2_SFT))
        case HW_VER_PRODUCT_SFT:
            V7R2_PORTING_IOS_CONFIG;
            break;

#elif (defined(BSP_CONFIG_V7R2_ASIC))
        case HW_VER_PRODUCT_UDP:
            UDP_IOS_CONFIG;
            UDP_IOS_CONFIG_SAVE;
			sep_io_mux();
            break;

        case HW_VER_PRODUCT_E5379:
            E5379_IOS_CONFIG;
            E5379_IOS_CONFIG_SAVE;
            break;
#elif (defined(BSP_CONFIG_V711_PORTING))
        case HW_VER_PRODUCT_PORTING:
            V711_PORTING_IOS_CONFIG;
            break;

#elif (defined(BSP_CONFIG_V711_ASIC))
        case HW_VER_V711_UDP:
            V711_UDP_IOS_CONFIG;
			V711_UDP_IOS_CONFIG_SAVE;
            wm8990_io_mux();
            break;
#if (FEATURE_ON == MBB_COMMON)
        case HW_VER_PRODUCT_E5:
        case HW_VER_PRODUCT_E5573S_320:
        case HW_VER_PRODUCT_E5573S_607:
            V711_E5_IOS_CONFIG;  /* �������ɵ����� */
            V711_E5_IOS_CONFIG_SAVE;
            break;
        /* case HW_VER_PRODUCT_CPE: */
        /*lint -e30*/
        case HW_VER_PRODUCT_B310s_927:
        case HW_VER_PRODUCT_B310s_518:
        case HW_VER_PRODUCT_B310As_852:
        case HW_VER_PRODUCT_B315s_22:
        case HW_VER_PRODUCT_B310s_927_HISI:
        case HW_VER_PRODUCT_B310s_518_HISI:
        case HW_VER_PRODUCT_B310As_852_HISI:
        case HW_VER_PRODUCT_B315s_22_HISI:
        /*lint +e30*/
            CPE_IOS_CONFIG;
            CPE_IOS_CONFIG_SAVE;
            break;
        case HW_VER_PRODUCT_STICK:
        case HW_VER_PRODUCT_E3372H_510:
        case HW_VER_PRODUCT_E3372H_210:
        case HW_VER_PRODUCT_E3372H_607:
            V711_stick_IOS_CONFIG;
            V711_stick_IOS_CONFIG_SAVE;
            break;            
        case HW_VER_PRODUCT_WINGLE:
        case HW_VER_PRODUCT_E8372H_510:
        case HW_VER_PRODUCT_E8372H_927:
        case HW_VER_PRODUCT_E8372H_511:
            V711_wingle_IOS_CONFIG;
            V711_wingle_IOS_CONFIG_SAVE;
            break;            
        case HW_VER_PRODUCT_E5578S_932:
            V711_E5578_IOS_CONFIG;  /* �������ɵ����� */
            V711_E5578_IOS_CONFIG_SAVE;
            break;
#endif
#endif
        case HW_VER_INVALID:
            cprintf("hardware version is invalid.\n");
            break;

        default:
            cprintf("hardware version cannot be identified. id:0x%x\n", product_type);

    }


}

void jtag1_config(void)
{
#if (defined(BSP_CONFIG_V7R2_SFT) || defined(BSP_CONFIG_V7R2_ASIC))
/*����MMC0��6��PIN����SD MASTER/SDIO SLAVE��*/
    /*jtag1_rtck�ܽŸ�������*/
    SET_IOS_JTAG1_CTRL1_1;
    OUTSET_IOS_PD_IOM_CTRL21;
    CLR_IOS_MMC0_CLK_CTRL1_1;
    /*jtag1_rtck�ܽ�����������*/
    NASET_IOS_PD_IOM_CTRL21;

    /*jtag1_tck�ܽŸ�������*/
    SET_IOS_JTAG1_CTRL1_1;
    INSET_IOS_PD_IOM_CTRL22;
    CLR_IOS_MMC0_CTRL1_1;
    /*jtag1_tck�ܽ�����������*/
    PDSET_IOS_PD_IOM_CTRL22;

    /*jtag1_trst_n�ܽŸ�������*/
    SET_IOS_JTAG1_CTRL1_1;
    CLR_IOS_MMC0_CTRL1_1;

    /*jtag1_tms�ܽŸ�������*/
    SET_IOS_JTAG1_CTRL1_1;
    INSET_IOS_PD_IOM_CTRL24;
    CLR_IOS_MMC0_CTRL1_1;

    /*jtag1_tdi�ܽŸ�������*/
    SET_IOS_JTAG1_CTRL1_1;
    INSET_IOS_PD_IOM_CTRL25;
    CLR_IOS_MMC0_CTRL1_1;

    /*jtag1_tdo�ܽŸ�������*/
    SET_IOS_JTAG1_CTRL1_1;
    CLR_IOS_MMC0_CTRL1_1;
	
/*����MMC0��6��PIN����SD MASTER/SDIO SLAVE��*/
    /*jtag1_rtck�ܽŸ������ñ���*/
    add_ios_list(IOS_PD_MF_CTRL1);
    add_ios_list(IOS_PD_AF_CTRL2);
    add_ios_list(IOS_PD_IOM_CTRL21);
    /*jtag1_rtck�ܽ����������ñ���*/
    add_ios_list(IOS_PD_IOM_CTRL21);

    /*jtag1_tck�ܽŸ������ñ���*/
    add_ios_list(IOS_PD_MF_CTRL1);
    add_ios_list(IOS_PD_AF_CTRL2);
    add_ios_list(IOS_PD_IOM_CTRL22);
    /*jtag1_tck�ܽ����������ñ���*/
    add_ios_list(IOS_PD_IOM_CTRL22);

    /*jtag1_trst_n�ܽŸ������ñ���*/
    add_ios_list(IOS_PD_MF_CTRL1);
    add_ios_list(IOS_PD_AF_CTRL2);

    /*jtag1_tms�ܽŸ������ñ���*/
    add_ios_list(IOS_PD_MF_CTRL1);
    add_ios_list(IOS_PD_AF_CTRL2);
    add_ios_list(IOS_PD_IOM_CTRL24);

    /*jtag1_tdi�ܽŸ������ñ���*/
    add_ios_list(IOS_PD_MF_CTRL1);
    add_ios_list(IOS_PD_AF_CTRL2);
    add_ios_list(IOS_PD_IOM_CTRL25);

    /*jtag1_tdo�ܽŸ������ñ���*/
    add_ios_list(IOS_PD_MF_CTRL1);
    add_ios_list(IOS_PD_AF_CTRL2);
#endif

}

#ifdef __cplusplus
}
#endif



