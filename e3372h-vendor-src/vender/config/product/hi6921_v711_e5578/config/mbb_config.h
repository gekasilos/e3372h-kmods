/***************************************************************************************************
  Copyright, 1988-2013, Huawei Tech. Co., Ltd.
  File name:      mbb_config.h
  Author:         daizhicheng 00183683
  Version:        V1.0 
  Date:           2013-05-24
  Description:    Ϊ��ͨ����������Hisi��MBB�Ĵ���
                  �궨��ԭ�����£�
                  1.�꿪���ڴ���رյ�������ܱ�֤�������ͨ���������ܱ�֤��������������ر�
                  2.����������ڸ�������ͨ��������ͳһ����ʹ��ǰͳһ����
  Others: 
                  1��MBB����һ�����꣺MBBͨ�ú꣨MBB_COMMON��
                     ����������ĺ꣬�ر��������꣬��������ľ��Ǵ����ߴ���
                  2��һ���꣺�������Ժ꣨USB���Ժ�--MBB_USB����ͬ�������в�ͬ�����Ժ꣩
                  3�������꣺�൱��һ�����Ժ�������Ժ꣨�����������»������߳���������������ԣ�
                  4��������ϸ˵������ϸ�Ķ���MBB��Ʒ�����淶�������в���֮��������ϵAuthor
  Warning:        
                  1: ����ʹ�ó�#if��#endif��#ifdef��#ifndef��#define֮��Ĺؼ��ֶ�����жϺ�
                  2������ʹ��ʹ�á��롱�������Լ�#else����ʹ��#ifʵ�ֶ��Ƕ��
                  3������define�ֳɶ��У�����д��һ����
******************************************************************************************************/
/******************************************************************************************************
    ���ⵥ��                       ʱ��                   �޸���                        ˵��
DTS2013060607539               2013-06-06          daizhicheng 00183683        ����mbb_config.hͷ�ļ�
DTS2013070203191               2013-07-02          xuchao 00202188             ����ͷ�ļ�����������˵��
*****************************************************************************************************/
#ifndef __MBB_CONFIG_H__
#define __MBB_CONFIG_H__

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef FEATURE_ON
#define FEATURE_ON 1
#endif
#ifndef FEATURE_OFF
#define FEATURE_OFF 0
#endif

#define MBB_COMMON FEATURE_ON    /* MBBͨ�ú� */

/***********************�궨�忪ʼ******************************/
/*******************************************************************
                        һ���궨��
********************************************************************/
#if (FEATURE_ON == MBB_COMMON)
#define    MBB_KEY                      FEATURE_ON      /* һ���꣺���� */
#define    MBB_MLOG                     FEATURE_ON      /* һ���꣺MobileLog(��C��ʹ��) */
#define    MBB_CHARGE                   FEATURE_ON      /* һ���꣺��� */
#define    MBB_THERMAL_PROTECT          FEATURE_ON      /* һ���꣺�¶ȱ��� */
#define    MBB_WIFI                     FEATURE_ON      /* һ���꣺WiFi */
#define    MBB_FAST_ON_OFF              FEATURE_OFF     /* һ���꣺���ٿ��ػ� */
#define    MBB_WPG_COMMON               FEATURE_ON      /* һ���꣺����Э�������ƺ� */
#define    MBB_TOUCHSCREEN_MELFAS       FEATURE_OFF  	/* һ���꣺touch���� */
#define    MBB_FEATURE_SD_POLLING       FEATURE_OFF     /* һ���꣺SD����ѯ���� */
#define    MBB_FEATURE_PHY_NUM          FEATURE_ON      /* һ���꣺Ӳ���汾������ */
#define    MBB_REB                      FEATURE_ON      /* һ���꣺����ɿ��� */
#define    MBB_CTF_COMMON               FEATURE_ON     /* һ���꣺CTF ��ʱ�رգ�����ͨ����� */
#define    MBB_FEATURE_CAT_MODULE_SET   FEATURE_OFF     /* һ���꣺����CAT�ȼ� */
#define    MBB_SIMLOCK_THREE            FEATURE_ON      /* һ���꣺simlock 3.0 �� */
#define    MBB_DLOAD                    FEATURE_ON      /*һ���꣺���� */
#define    MBB_LED                      FEATURE_OFF      /* һ���� :LED*/
#define    MBB_LCD                      FEATURE_OFF      /* һ���� :LCD*/
#define    MBB_OLED                     FEATURE_ON      /* һ���� :0.96 OLED*/
#define    MBB_FEATURE_32K_CLK_SRC      FEATURE_OFF     /* һ���꣺32Kʱ��Դ��Ƶ */
#define    MBB_FEATURE_FASTIP           FEATURE_OFF     /* һ���꣺FastIP */
#define    MBB_ANTENNA                  FEATURE_OFF     /* һ���꣺�����л� */
#define    MBB_SLIC	                    FEATURE_OFF     /* һ���꣺����slic����һ���� */
#define    MBB_FEATURE_ETH		      	FEATURE_OFF     /* һ���꣺ethernet */
#endif/* MBB_COMMON == FEATURE_ON */

#if (FEATURE_OFF == MBB_COMMON)
#define    MBB_KEY                      FEATURE_OFF     /* һ���꣺���� */
#define    MBB_MLOG                     FEATURE_OFF     /* һ���꣺MobileLog(��C��ʹ��) */
#define    MBB_CHARGE                   FEATURE_OFF     /* һ���꣺��� */
#define    MBB_THERMAL_PROTECT          FEATURE_OFF     /* һ���꣺�¶ȱ��� */
#define    MBB_WIFI                     FEATURE_OFF     /* һ���꣺WiFi */
#define    MBB_FAST_ON_OFF              FEATURE_ON      /* һ���꣺���ٿ��ػ� */
#define    MBB_WPG_COMMON               FEATURE_OFF     /* һ���꣺����Э�������ƺ� */
#define    MBB_TOUCHSCREEN_MELFAS       FEATURE_OFF  	/* һ���꣺touch���� */
#define    MBB_FEATURE_SD_POLLING       FEATURE_OFF     /* һ���꣺SD����ѯ���� */
#define    MBB_FEATURE_PHY_NUM          FEATURE_OFF     /* һ���꣺Ӳ���汾������ */
#define    MBB_REB                      FEATURE_OFF     /* һ���꣺����ɿ��� */
#define    MBB_CTF_COMMON               FEATURE_OFF     /* һ���꣺CTF */
#define    MBB_FEATURE_CAT_MODULE_SET   FEATURE_OFF     /* һ���꣺����CAT�ȼ� */
#define    MBB_SIMLOCK_THREE            FEATURE_OFF     /* һ���꣺simlock 3.0 �� */
#define    MBB_DLOAD                    FEATURE_OFF     /*һ���꣺���� */
#define    MBB_LED                      FEATURE_OFF     /* һ���� :LED*/
#define    MBB_FEATURE_32K_CLK_SRC      FEATURE_OFF     /* һ���꣺32Kʱ��Դ��Ƶ */
#define    MBB_LCD                      FEATURE_OFF      /* һ���� :LCD*/
#define    MBB_OLED                     FEATURE_OFF      /* һ���� :0.96 OLED*/
#define    MBB_FEATURE_FASTIP           FEATURE_OFF     /* һ���꣺FastIP */
#define    MBB_ANTENNA                  FEATURE_OFF     /* һ���꣺�����л� */
#define    MBB_SLIC	                    FEATURE_OFF     /* һ���꣺����slic����һ���� */
#define    MBB_FEATURE_ETH		      	FEATURE_OFF     /* һ���꣺ethernet */
#endif/* MBB_COMMON == FEATURE_OFF */

/*******************************************************************
                        �����궨��
********************************************************************/
#if (FEATURE_ON == MBB_KEY)
#define    MBB_POWER_KEY                FEATURE_ON      /* �����꣺power���� */
#define    MBB_MENU_KEY                 FEATURE_OFF     /* �����꣺menu���� */
#define    MBB_RESET_KEY                FEATURE_ON      /* �����꣺reset���� */
#define    MBB_WLAN_KEY                 FEATURE_OFF     /* �����꣺wlan���� */
#endif

#if (FEATURE_OFF == MBB_KEY)
#define    MBB_POWER_KEY                FEATURE_OFF     /* �����꣺power���� */
#define    MBB_MENU_KEY                 FEATURE_OFF     /* �����꣺menu���� */
#define    MBB_RESET_KEY                FEATURE_OFF     /* �����꣺reset���� */
#define    MBB_WLAN_KEY                 FEATURE_OFF     /* �����꣺wlan���� */
#endif


#if (MBB_CHARGE == FEATURE_ON)
#define    MBB_CHG_PLATFORM_V7R2        FEATURE_ON      /* �����꣺V7R2���Ժ� */
#define    MBB_CHG_PLATFORM_9X25        FEATURE_OFF     /* �����꣺9X25���Ժ� */
#define    MBB_CHG_BAT_KNOCK_DOWN       FEATURE_OFF     /* �����꣺�Ƿ�ɲ�ж���,FEATURE_ON:�ǣ�FEATURE_OFF:���ǣ������壺������� */
#define    MBB_CHG_LCD                  FEATURE_OFF     /* �����꣺LCD��Ʒ���Ժ� */
#define    MBB_CHG_LED                  FEATURE_OFF     /* �����꣺LED��Ʒ���Ժ� */
#define    MBB_CHG_OLED                 FEATURE_ON      /* �����꣺OLED��Ʒ���Ժ� */
#define    MBB_CHG_TOUCH                FEATURE_OFF      /* �����꣺TOHCH��Ʒ���Ժ� */
#define    MBB_CHG_LINUX                FEATURE_ON      /* �����꣺LINUXϵͳ���Ժ� */
#define    MBB_CHG_VXWORKS              FEATURE_OFF     /* �����꣺LINUXϵͳ���Ժ� */
#define    MBB_CHG_REX                  FEATURE_OFF     /* �����꣺REXϵͳ���Ժ� */
#define    MBB_CHG_EXTCHG               FEATURE_OFF      /* �����꣺���������Ժ� */
#define    MBB_CHG_WIRELESS             FEATURE_OFF      /* �����꣺���߳�����Ժ� */
#define    MBB_CHG_MC2                  FEATURE_ON      /* �����꣺MC2.0���Ժ� */
#define    MBB_CHG_COMPENSATE           FEATURE_ON      /* �����꣺�����������Ժ� */
#define    MBB_CHG_BQ24196              FEATURE_ON      /* �����꣺BQ24196���оƬ���Ժ� */
#define    MBB_CHG_BQ24161              FEATURE_OFF     /* �����꣺BQ24161���оƬ���Ժ� */
#define    MBB_CHG_MAX8903              FEATURE_OFF     /* �����꣺MAX8903���оƬ���Ժ� */
#define    MBB_CHG_SCHARGER             FEATURE_OFF     /* �����꣺scharger���оƬ���Ժ� */
#define    MBB_CHG_WARM_CHARGE          FEATURE_ON      /* �����꣺���³�����Ժ� */
#define    MBB_CHG_MC                   FEATURE_ON      /* �����꣺MC2.0���Ժ� */
#define    MBB_CHG_POWER_SUPPLY         FEATURE_ON      /* �����꣺power supply���Ժ� */
#define    MBB_CHG_COULOMETER           FEATURE_OFF     /* �����꣺���ؼ����Ժ� */
#define    MBB_CHG_BQ27510              FEATURE_OFF     /* �����꣺���ÿ��ؼ����Ժ� */
#define    MBB_CHG_HIGH_VOLT_BATT       FEATURE_OFF     /* �����꣺��ѹ������Ժ� */
#define    MBB_CHG_BAT_CAPABILITY       (3000)          /* �����꣺������� */
#endif

#if (MBB_CHARGE == FEATURE_OFF)
#define    MBB_CHG_PLATFORM_V7R2        FEATURE_OFF    /* �����꣺V7R2���Ժ� */
#define    MBB_CHG_PLATFORM_9X25        FEATURE_OFF    /* �����꣺9X25���Ժ� */
#define    MBB_CHG_BAT_KNOCK_DOWN       FEATURE_OFF    /* �����꣺�Ƿ�ɲ�ж���,FEATURE_ON:�ǣ�FEATURE_OFF:���ǣ������壺������� */
#define    MBB_CHG_LCD                  FEATURE_OFF    /* �����꣺LCD��Ʒ���Ժ� */
#define    MBB_CHG_LED                  FEATURE_OFF    /* �����꣺LED��Ʒ���Ժ� */
#define    MBB_CHG_OLED                 FEATURE_OFF      /* �����꣺OLED��Ʒ���Ժ� */
#define    MBB_CHG_TOUCH                FEATURE_OFF    /* �����꣺TOHCH��Ʒ���Ժ� */
#define    MBB_CHG_LINUX                FEATURE_OFF    /* �����꣺LINUXϵͳ���Ժ� */
#define    MBB_CHG_VXWORKS              FEATURE_OFF    /* �����꣺LINUXϵͳ���Ժ� */
#define    MBB_CHG_REX                  FEATURE_OFF    /* �����꣺REXϵͳ���Ժ� */
#define    MBB_CHG_EXTCHG               FEATURE_OFF    /* �����꣺���������Ժ� */
#define    MBB_CHG_WIRELESS             FEATURE_OFF    /* �����꣺���߳�����Ժ� */
#define    MBB_CHG_MC2                  FEATURE_OFF    /* �����꣺MC2.0���Ժ� */
#define    MBB_CHG_COMPENSATE           FEATURE_OFF    /* �����꣺�����������Ժ�*/
#define    MBB_CHG_BQ24196              FEATURE_OFF    /* �����꣺BQ24196���оƬ���Ժ� */
#define    MBB_CHG_BQ24161              FEATURE_OFF    /* �����꣺BQ24161���оƬ���Ժ� */
#define    MBB_CHG_MAX8903              FEATURE_OFF    /* �����꣺MAX8903���оƬ���Ժ� */
#define    MBB_CHG_SCHARGER             FEATURE_OFF    /* �����꣺scharger���оƬ���Ժ� */
#define    MBB_CHG_WARM_CHARGE          FEATURE_OFF    /* �����꣺���³�����Ժ� */
#define    MBB_CHG_MC                   FEATURE_OFF    /* �����꣺MC2.0���Ժ� */
#define    MBB_CHG_POWER_SUPPLY         FEATURE_OFF    /* �����꣺power supply���Ժ� */
#define    MBB_CHG_COULOMETER           FEATURE_OFF    /* �����꣺���ؼ����Ժ� */
#define    MBB_CHG_BQ27510              FEATURE_OFF    /* �����꣺���ÿ��ؼ����Ժ� */
#define    MBB_CHG_HIGH_VOLT_BATT       FEATURE_OFF    /* �����꣺��ѹ������Ժ� */
#define    MBB_CHG_BAT_CAPABILITY       (3000)         /* �����꣺������� */
#endif


/* WiFi������:�ú���Ʊ����Ŀ�оƬ
 * оƬĿ¼��������Ϊ����+����磺rtl8129, bcm43217,���ǺͶ�Ӧ������Ŀ¼����Ҫһ���ġ�
 */
#if (FEATURE_ON == MBB_WIFI)
#define    MBB_WIFI_CHIP1               bcm43241
#define    MBB_WIFI_CHIP2               FEATURE_OFF
#endif

#if (FEATURE_OFF == MBB_WIFI)
#define    MBB_WIFI_CHIP1               FEATURE_OFF
#define    MBB_WIFI_CHIP2               FEATURE_OFF
#endif

/*����Э��ģ��궨�忪ʼ*/
#if (FEATURE_ON == MBB_WPG_COMMON)
#define    MBB_FEATURE_UNI_PS_CALL      FEATURE_OFF    /* �����꣺���� */
#define    MBB_FEATURE_BIP              FEATURE_OFF    /* �����꣺BIP���� */
#define    MBB_FEATURE_BIP_TEST         FEATURE_OFF    /* �����꣺BIP���Բ��Ժ� */
#define    MBB_FEATURE_MPDP             FEATURE_OFF    /* �����꣺MPDP���Բ��Ժ� */
#define    MBB_FEATURE_GATEWAY          FEATURE_OFF    /* �����꣺�����������Բ��Ժ� */
#define    MBB_FEATURE_AT_CMD_FILTER    FEATURE_ON     /* �����꣺����AT����� */
#endif

#if (FEATURE_OFF == MBB_WPG_COMMON)
#define    MBB_FEATURE_UNI_PS_CALL      FEATURE_OFF    /* �����꣺���� */
#define    MBB_FEATURE_BIP              FEATURE_OFF    /* �����꣺BIP���� */
#define    MBB_FEATURE_BIP_TEST         FEATURE_OFF    /* �����꣺BIP���Բ��Ժ� */
#define    MBB_FEATURE_MPDP             FEATURE_OFF    /* �����꣺MPDP���Բ��Ժ� */
#define    MBB_FEATURE_GATEWAY          FEATURE_OFF    /* �����꣺�����������Բ��Ժ� */
#define    MBB_FEATURE_AT_CMD_FILTER    FEATURE_OFF    /* �����꣺����AT����� */
#endif
/*����Э��ģ��궨�����*/

#if (MBB_CTF_COMMON == FEATURE_ON)
/*����BCM�ṩ��ctf.ko��MBB_CTF_COMMON��MBB_CTF_WIFI_IPV6���Ǵ򿪵ģ�������Ǵ�������������ҲӦ��ͬʱ�򿪻�ر�*/
#define    MBB_CTF_WIFI_IPV6            FEATURE_ON     /* �����꣺CTF�Ƿ�֧��IPv6 */
#define    MBB_CTF_WAN_QUEUE            FEATURE_ON
#endif /* MBB_CTF_COMMON == FEATURE_ON */

#if (MBB_CTF_COMMON == FEATURE_OFF)
/*����BCM�ṩ��ctf.ko��MBB_CTF_COMMON��MBB_CTF_WIFI_IPV6���Ǵ򿪵ģ�������Ǵ�������������ҲӦ��ͬʱ�򿪻�ر�*/
#define    MBB_CTF_WIFI_IPV6            FEATURE_OFF    /* �����꣺CTF�Ƿ�֧��IPv6 */
#define    MBB_CTF_WAN_QUEUE            FEATURE_OFF
#endif /* MBB_CTF_COMMON == FEATURE_OFF */

#if (FEATURE_ON == MBB_DLOAD)
#define MBB_DLOAD_BALONG     FEATURE_ON/*�����꣺����balong�͸�ͨ�Ĳ���*/
#define MBB_DLOAD_LED       FEATURE_OFF      /*�����꣬��������led����*/
#define MBB_DLOAD_LCD       FEATURE_OFF     /*�����꣬��������lcd��ʾ���� */
#define MBB_DLOAD_BBOU       FEATURE_ON/*�����꣺��������*/
#define MBB_DLOAD_SDUP      FEATURE_OFF    /*�����꣺SD������*/
#define MBB_DLOAD_MCPEUP    FEATURE_OFF      /*������: AP+Modem��̬CPE����*/
#define MBB_DLOAD_HILINK    FEATURE_OFF     /*������: hilink����*/
#define MBB_DLOAD_SOLUTION  FEATURE_OFF     /*������: solution����*/
#define MBB_DLOAD_STICK     FEATURE_OFF     /*������: STICK����*/
#define MBB_DLOAD_WINGLE    FEATURE_OFF     /*������: WINGLE����*/
#define MBB_DLOAD_E5        FEATURE_OFF     /*������:  E5����*/
#define MBB_DLOAD_JFFS2     FEATURE_ON     /*������: ����Ϊjffs2�ļ�ϵͳ(��webui������*/
#endif

#if (FEATURE_OFF == MBB_DLOAD)
#define MBB_DLOAD_BALONG    FEATURE_OFF     /*�����꣺balong�͸�ͨ����*/
#define MBB_DLOAD_LED       FEATURE_OFF     /*�����꣬��������led����*/
#define MBB_DLOAD_LCD       FEATURE_OFF     /*�����꣬��������lcd��ʾ���� */
#define MBB_DLOAD_BBOU      FEATURE_OFF     /*������: ��������*/
#define MBB_DLOAD_SDUP      FEATURE_OFF     /*�����꣺SD������*/
#define MBB_DLOAD_MCPEUP    FEATURE_OFF     /*������: AP+Modem��̬CPE����*/
#define MBB_DLOAD_HILINK    FEATURE_OFF     /*������: hilink����*/
#define MBB_DLOAD_SOLUTION  FEATURE_OFF     /*������: solution����*/
#define MBB_DLOAD_STICK     FEATURE_OFF     /*������: STICK����*/
#define MBB_DLOAD_WINGLE    FEATURE_OFF     /*������: WINGLE����*/
#define MBB_DLOAD_E5        FEATURE_OFF     /*������: E5����*/
#define MBB_DLOAD_JFFS2     FEATURE_OFF     /*������: ����Ϊjffs2�ļ�ϵͳ(��webui������*/
#endif

#if (FEATURE_ON == MBB_LED)
#define    MBB_LED_DR                   FEATURE_ON       /*�����꣺DR��*/
#define    MBB_LED_GPIO                 FEATURE_ON      /*�����꣺GPIO��*/
#endif
#if (FEATURE_OFF == MBB_LED)
#define    MBB_LED_DR                   FEATURE_OFF     /*�����꣺DR��*/
#define    MBB_LED_GPIO                 FEATURE_OFF     /*�����꣺GPIO��*/
#endif                      
#if (FEATURE_ON == MBB_FEATURE_ETH)
#define    MBB_FEATURE_ETH_PHY      	FEATURE_OFF     /* �����꣺RGMII�ڽ�PHYоƬ */
#define    MBB_FEATURE_ETH_SWITCH      	FEATURE_OFF     /* �����꣺RGMII�ڽ�SWITCHоƬ */
#endif
#if (FEATURE_OFF == MBB_FEATURE_ETH)
#define    MBB_FEATURE_ETH_PHY      	FEATURE_OFF     /* �����꣺RGMII�ڽ�PHYоƬ */
#define    MBB_FEATURE_ETH_SWITCH      	FEATURE_OFF     /* �����꣺RGMII�ڽ�SWITCHоƬ */
#endif
#if (FEATURE_ON == MBB_FEATURE_FASTIP)
#define    MBB_FEATURE_FASTIP_IPV6             FEATURE_ON  /* �����꣺FastIP ��IPv6��֧�� */
#define    MBB_FEATURE_FASTIP_WAN_QUEUE        FEATURE_ON  /* �����꣺FastIP ʹ��WAN����� */
#endif

#if (FEATURE_OFF == MBB_FEATURE_FASTIP)
#define    MBB_FEATURE_FASTIP_IPV6             FEATURE_OFF /* �����꣺FastIP ��IPv6��֧�� */
#define    MBB_FEATURE_FASTIP_WAN_QUEUE        FEATURE_OFF /* �����꣺FastIP ʹ��WAN����� */
#endif
/*******************************************************************
                        �����궨��
********************************************************************/

/***********************�궨�����******************************/

/*BEGIN DTS2013072504345  l00212897 2013/07/25 modified*/
#ifndef YES
#define YES  1
#endif
#ifndef NO
#define NO   0
#endif
/*END DTS2013072504345  l00212897 2013/07/25 modified*/

/**************************************MBB_COMMON == FEATURE_ON****************************************************/
#if (MBB_COMMON == FEATURE_ON)         /* ���꣺�����Ѿ����� */ 
/*******************************************************************/



/* BEGIN: Add for PN: DTS2013070304541  by h00122846,2013/7/4 */
/*******************************************************************
           USBģ�飬�¼Ӻ궨�����밴�ձ��ļ����ͳһ���
*******************************************************************/
/*******************************************************************
                        һ���궨��
********************************************************************/
#if (FEATURE_ON == MBB_COMMON)
#define MBB_USB            FEATURE_ON      
#endif

#if (FEATURE_OFF == MBB_COMMON)
#define MBB_USB            FEATURE_OFF     
#endif

/*******************************************************************
                        �����궨��
********************************************************************/
#if (FEATURE_ON == MBB_USB)
#define MBB_USB_MULTICONFIG         FEATURE_OFF
#define MBB_USB_SECURITY            FEATURE_ON
#define MBB_USB_SD                  FEATURE_ON

#define MBB_USB_CPE                 FEATURE_OFF
#define MBB_USB_E5                  FEATURE_ON
#define MBB_USB_HILINK              FEATURE_OFF
#define MBB_USB_STICK               FEATURE_OFF
#define MBB_USB_WINGLE              FEATURE_OFF

#define MBB_USB_CAPABILITY_THREE    FEATURE_OFF
#define MBB_USB_CHARGE              FEATURE_ON
#endif

#if (FEATURE_OFF == MBB_USB)
#define MBB_USB_MULTICONFIG         FEATURE_OFF
#define MBB_USB_SECURITY            FEATURE_OFF
#define MBB_USB_SD                  FEATURE_OFF

#define MBB_USB_CPE                 FEATURE_OFF
#define MBB_USB_E5                  FEATURE_OFF
#define MBB_USB_HILINK              FEATURE_OFF
#define MBB_USB_STICK               FEATURE_OFF
#define MBB_USB_WINGLE              FEATURE_OFF

#define MBB_USB_CAPABILITY_THREE    FEATURE_OFF
#define MBB_USB_CHARGE              FEATURE_OFF
#endif



#endif /* MBB_COMMON == FEATURE_ON */
#ifdef __cplusplus
}
#endif

#endif /*__MBB_CONFIG_H__ */
