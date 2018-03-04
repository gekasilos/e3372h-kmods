/****************************************************************************************************************
     <���ⵥ��>       <��  ��>    <�޸�ʱ��>   <�汾> <�޸�����>
    2032203980  m00182044   2012/6/10           DHCP6c����ͨ��RA������״̬IPv6��ַ���� 
    2060902101  m00182044   2012/6/24           IPV6 NS�ಥ�����쳣
    2101203859  l00170266   2012/11/2           ����·��Э������Ժ�ı������������FEATURE_HUAWEI_MBB_SNTP��
                                                      FEATURE_HUAWEI_MBB_DLNA��FEATURE_HUAWEI_MBB_SAMBA
    2110706738  y00188255   2012/11/7           prefix��dns��һ���仯�������ӽ����stacall
	N/A             y00186923   2012.11.16          9615ƽ̨����������
    3050907167  z00186446   2013/15/2           BCM43241��9625����Ľӿ�����
    3082010042  y00188255  2013/08/23           ����fortify����
    3112001144  s00216562   2013-11-21          pppoe֧��ipv6
    3112501445  y00188255   2013-11-25          �޸ļ���DNS�ļ�Ϊdhcpdns6
    4031801302  s00216562   2014-03-18          ��̬ip֧��ipv6
    4091702971  f00178256   2014/09/20          offload˫ջ�л�Ϊoffload ipv4��ջbr0���ܻ�ȡIPv6��ַ
****************************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <syslog.h>
#include <linux/if.h>
#include <unistd.h>

#include "addrstate.h"
#include "misc.h"
#include "ra.h"
#include "sockutils.h"

#include <linux/rtnetlink.h>

#include "wanapi.h"
#include "wat_comm_def.h"

#if 0
#if 1
#define     LOG_ERR(str, args ...)                                  \
{                                                                   \
    printf("\r\n[%-16s : %-5d] ", __FUNCTION__, __LINE__);          \
    printf(str, ## args);                                           \
    printf("\r\n\r\n");                                             \
}
#else
#define LOG_ERR(str, args...)
#endif
#endif

/*BEGIN 2032203980  m00182044 2012-06-10 modify*/
typedef enum
{
    /* �ȴ� */
    WAN_TRIGGER_IDLE = 0,
    /* ���� */
    WAN_TRIGGER_DISCONNECTING = 1,
    /* �ֶ� */
    WAN_TRIGGER_MANUAL = 2,
    /* �Զ� */
    WAN_TRIGGER_AUTOON = 3,
    /* ���貦�ţ��ݲ�֧�� */
    /* WAN_TRIGGER_ONDEMAND, */
}ATP_DHCPC_TRIGGER_STATUS_EN;
/*END 2032203980  m00182044 2012-06-10 modify*/

/* WAN����״̬ */
typedef enum
{
    /* δ��ʼ�� */
    ATP_WAN_STATUS_UNCONFIGURED,
    /* �������� */
    ATP_WAN_STATUS_CONNECTING,
    /* ������֤ */
    ATP_WAN_STATUS_AUTHENTICATING,
    /* ������ */
    ATP_WAN_STATUS_CONNECTED,
    /* δ���� */
    ATP_WAN_STATUS_PENDING,
    /* ���ڶϿ� */
    ATP_WAN_STATUS_DISCONNECTING,
    /* �ѶϿ� */
    ATP_WAN_STATUS_DISCONNECTED 
}ATP_DHCPC_WAN_STATUS_EN;
static pthread_mutex_t addrstate_mutex = PTHREAD_MUTEX_INITIALIZER;

ADDRSTATE_ST        *g_pstAddrState = NULL;


#define ADDRSTATE_LOCK()    pthread_mutex_lock(&addrstate_mutex);
#define ADDRSTATE_UNLOCK()  pthread_mutex_unlock(&addrstate_mutex);

extern g_lSignalPipev6[0];

static int AddrStateInit(ADDRSTATE_ST *pstAddrInfo, int ifindex, 
                    int iEnableRA, int iIANAFlag,
                    int iIAPDFlag, int iDsliteFlag, int iReadDialFile);

static int PrepareNetLinkSocket(NETLINK_SOCKET_ST *pstNetLink, int iProtocol, 
    uint32_t uiGrp, int ifindex);

static int GetNetlinkSocket(NETLINK_SOCKET_ST *pstNetLink);

static int PutNetlinkSocket(NETLINK_SOCKET_ST *pstNetLink);

int WriteAllWanGlobalIP(ADDRSTATE_ST *pstState);

/*BEGIN 2032203980  m00182044 2012-06-10 modify*/
int GetAllWanGlobalIP(char *devname, char *buf);


/******************************************************************************
  ��������  : BSP_SYS_WanGetSwitchName
  ��������  : ��ȡ��ǰ���ӵ�wan������
  �������  : 
              1.  *pWanName:
 
  ���ú���  : 
  ��������  : 
  �������  : ��
  �� �� ֵ  :  0: �ɹ�
              ����: ʧ��

  �޸���ʷ      :

     �޸�����   : ��ɳ���

******************************************************************************/
int32 BSP_SYS_WanGetSwitchName(int8 *pWanName)
{
    char acCmd[128] = {0};
    char acinterface[64] = {0};
    FILE* pstFile  = NULL;
    unsigned int lStatus = DISABLE_FEATURE;
    unsigned int i = 0;

    if (NULL == pWanName)
    {
        return -1;
    }

    *pWanName = '\0';
    for (i = 0; i < WMR_WANTYPE_ALL; i++)
    {
        switch (i)
        {
        case WMR_WANTYPE_UMTS:
            strcpy(acinterface, WAN_3G_NAME);
            break;
        case WMR_WANTYPE_WIFI:
            strcpy(acinterface, WAN_STA_NAME);
            break;
        case WMR_WANTYPE_CRADLE:
            strcpy(acinterface, WAN_CRADLE_NAME);
            break;
        default:
            strcpy(acinterface, WAN_3G_NAME);
            break;
        }

        sprintf(acCmd, "/var/wan/%s/switch", acinterface);
        pstFile = fopen(acCmd, "r");
        if (NULL != pstFile)
        {
            if (fgets(acCmd, 127, pstFile))
            {
                lStatus = atoi(acCmd);
                if ((ENABLE_FEATURE < lStatus) || (DISABLE_FEATURE >= lStatus))
                {
                    lStatus = DISABLE_FEATURE;
                }
                else
                {
                    fclose(pstFile);
                    strcpy(pWanName, acinterface);
                    return i;
                }
            }

            fclose(pstFile);
        }
    }

    return -1;
}

/* BEGIN 2110706738 y00188255 2012-11-7 modified */
/* ���ú����޸�Ϊ��ȡdns��ַ */
/*BEGIN 2060902101  m00182044 2012-06-24 modify*/
/******************************************************************************
  ��������  : BSP_SYS_GetDnsAddress
  ��������  :��ȡdns
  �������  : 
              1.  pWanName:
              
  ���ú���  : 
  ��������  : 
  �������  : pDnsAddress ��ȡ����dns��ַ
  �� �� ֵ  :  0: �ɹ�
              ����: ʧ��

  �޸���ʷ      :

     �޸�����   : ��ɳ���

******************************************************************************/
int BSP_SYS_GetDnsAddress(char *pWanName, char *pDnsAddress)
{
    char acFile[128] = {0};
    FILE *fp = NULL;
    
    if (NULL == pDnsAddress)
    {
        loginfo(FUNC, "args NULL");
        return (-1);
    }
        
    snprintf(acFile, sizeof(acFile), WAN_DHCPC6_DNS, pWanName);
    fp = fopen(acFile, "r");
    if( NULL == fp)
    {
        loginfo(FUNC, "fopen file %s fail\n", acFile);
        return (-1);   
    }
    
    fread(pDnsAddress, 512, 1, fp);

    loginfo(FUNC, "GetDnsAddress :%s", pDnsAddress);
    
    fclose(fp);
    
    return 0;
}
/*END 2060902101  m00182044 2012-06-24 modify*/
/* END 2110706738 y00188255 2012-11-7 modified */
/******************************************************************************
  ��������  : BSP_SYS_WakeupMonitorTask
  ��������  :�ϱ�dhcpv6�������
  �������  : 
              1.  iIfindex:
 
  ���ú���  : 
  ��������  : 
  �������  : ��
  �� �� ֵ  :  0: �ɹ�
              ����: ʧ��

  �޸���ʷ      :

     �޸�����   : ��ɳ���

******************************************************************************/
int BSP_SYS_WakeupMonitorTask(int iIfindex)
{
    char acIfName[IFNAMSIZ] = {0};
    char acBuf[512] = {0};
    static char acLastBuf[512] = {0};
    static int laststatus = ATP_WAN_STATUS_DISCONNECTED;

    /* BEGIN 2110706738 y00188255 2012-11-7 Added */
    char acDnsBuf[512] = {0};
    static char acLastDns[512] = {0};
    /* END 2110706738 y00188255 2012-11-7 Added */

    if (!if_indextoname(iIfindex, acIfName))
    {
        fprintf(stderr, "get wan name by index[%d] error", iIfindex);
        return (-1);
    }
    
    loginfo(FUNC, "wanup wan %s", acIfName);
    GetAllWanGlobalIP(acIfName, acBuf);
    if (0 >= strlen(acBuf))
    {
        memset(acBuf, 0, sizeof(acBuf));
        loginfo(FUNC, "wanup wan prefix null");
    }

    /* BEGIN 2110706738 y00188255 2012-11-7 modified */
    BSP_SYS_GetDnsAddress(acIfName, acDnsBuf);
    if (0 >= strlen(acDnsBuf))
    {
        memset(acDnsBuf, 0, sizeof(acDnsBuf));
        loginfo(FUNC, "wanup wan dns6 null");
    }

    if ((laststatus == GetWanStatus(iIfindex)) 
           && (!strcmp(acLastBuf, acBuf)) 
           && (!strcmp(acLastDns, acDnsBuf)))
    {
        loginfo(FUNC, "wanup wan %s status unchange no need report", acIfName);
        return -1;
    }
    /* END 2110706738 y00188255 2012-11-7 modified */
    
    if (!strcmp(acIfName ,WANNAME))
    {
        loginfo(FUNC, "wanup wan %s get wan switch ", WANNAME);
        if (0 >= BSP_SYS_WanGetSwitchName(acIfName))
        {
            loginfo(FUNC, "wanup wan %s get wan switch name failure", acIfName);
            return -1;
        }
    }

    /*BEGIN 2101203859  l00170266 2012-11-02 modify*/
    switch(GetWanStatus(iIfindex))
    {
        case ATP_WAN_STATUS_CONNECTED:
#if defined FEATURE_HUAWEI_MBB_WIFIOFFLOAD && defined FEATURE_HUAWEI_MBB_WIFIOFFLOAD_IPV6          
            if (!strcmp(acIfName ,WAN_STA_NAME))
            {
                ReportStaCallDhcpV6Result(WAN_SUCCESS);
            }
#endif            
#if defined FEATURE_HUAWEI_MBB_CRADLE && defined MBB_FEATURE_PPP6C       
            if (!strcmp(acIfName ,WAN_VPPP_NAME))
            {
                ReportCradlePpp6cResult(WAN_SUCCESS);
            }
#endif
#if defined FEATURE_HUAWEI_MBB_CRADLE && defined MBB_FEATURE_CRADLE_IPV6          
            if (!strcmp(acIfName ,WAN_CRADLE_NAME))
            {
                ReportCradleDhcpV6Result(WAN_SUCCESS);
            }
#endif            

            break;
        case ATP_WAN_STATUS_DISCONNECTED:
#if defined FEATURE_HUAWEI_MBB_WIFIOFFLOAD && defined FEATURE_HUAWEI_MBB_WIFIOFFLOAD_IPV6            
            if (!strcmp(acIfName ,WAN_STA_NAME))
            {
                ReportStaCallDhcpV6Result(WAN_FAILURE);
            }
#endif            
#if defined FEATURE_HUAWEI_MBB_CRADLE && defined MBB_FEATURE_PPP6C       
            if (!strcmp(acIfName ,WAN_VPPP_NAME))
            {
                ReportCradlePpp6cResult(WAN_FAILURE);
            }
#endif
#if defined FEATURE_HUAWEI_MBB_CRADLE && defined MBB_FEATURE_CRADLE_IPV6          
            if (!strcmp(acIfName ,WAN_CRADLE_NAME))
            {
                ReportCradleDhcpV6Result(WAN_FAILURE);
            }
#endif            

            break;
        default :
            break;
    }
    /*END 2101203859  l00170266 2012-11-02 modify*/
    laststatus = GetWanStatus(iIfindex);
    if (strcmp(acLastBuf, acBuf))
    {
        memcpy(acLastBuf, acBuf, sizeof(acBuf));
    }
    /* BEGIN 2110706738 y00188255 2012-11-7 Added */
    if (strcmp(acLastDns, acDnsBuf))
    {
        memcpy(acLastDns, acDnsBuf, sizeof(acDnsBuf));
    }
    /* END 2110706738 y00188255 2012-11-7 Added */
    return 0;
}
/*END 2032203980  m00182044 2012-06-10 modify*/

static int AddrStateInit(ADDRSTATE_ST *pstAddrInfo, int ifindex, 
                    int iEnableRA, int iIANAFlag,
                    int iIAPDFlag, int iDsliteFlag, int iReadDialFile)
{
    int i = 0;

    FUNCBEGIN();
    
    if (!pstAddrInfo || (0 >= ifindex))
    {
        loginfo(FUNC, "args NULL");
        return (-1);
    }

    memset(pstAddrInfo, 0, sizeof(ADDRSTATE_ST));

    pstAddrInfo->iEnableRA = iEnableRA;
    pstAddrInfo->iIANAFlag = iIANAFlag;
    pstAddrInfo->iIAPDFlag = iIAPDFlag;
    pstAddrInfo->iDsliteFlag = iDsliteFlag;
    pstAddrInfo->iReadDialFile = iReadDialFile;
    
    pstAddrInfo->ifindex = ifindex;
#if 0
    pstAddrInfo->iState = en_no_addr;
#endif
    pstAddrInfo->iWanState = en_wan_down;

    for (i = 0; i < ADDR_MAX_NUM; i++)
    {
        pstAddrInfo->astAddr[i].iFlg = en_item_free;
    }

    FUNCEND();
    
    return (0);
}


static int PrepareNetLinkSocket(NETLINK_SOCKET_ST *pstNetLink, int iProtocol, 
    uint32_t uiGrp, int ifindex)
{
    if (!pstNetLink || (0 >= ifindex))
    {
        loginfo(FUNC, "args NULL");
        return (-1);
    }

    memset(pstNetLink, 0, sizeof(NETLINK_SOCKET_ST));

    pstNetLink->iProtocol = iProtocol;
    pstNetLink->iIfIdx = ifindex;
    pstNetLink->stLocalAddr.nl_family = AF_NETLINK;
    pstNetLink->stLocalAddr.nl_groups = uiGrp;
    pstNetLink->stLocalAddr.nl_pid = (ifindex << 16) | getpid();
    pstNetLink->iSeq = time(NULL);

    return (0);
}

static int GetNetlinkSocket(NETLINK_SOCKET_ST *pstNetLink)
{
    if (!pstNetLink)
    {
        loginfo(FUNC, "malloc error");
        return (-1);
    }

    if (AF_NETLINK != pstNetLink->stLocalAddr.nl_family)
    {
        loginfo(FUNC, "protocol family error");
        return (-1);
    }

    pstNetLink->iSockfd = socket(AF_NETLINK, SOCK_RAW, pstNetLink->iProtocol);
    if (0 > pstNetLink->iSockfd)
    {
        loginfo(FUNC, "socket for netlink error");
        return (-1);
    }

    if (0 > bind(pstNetLink->iSockfd, (struct sockaddr *)(&pstNetLink->stLocalAddr), 
                    sizeof(pstNetLink->stLocalAddr)))
    {
        loginfo(FUNC, "bind err: %s", strerror(errno));
        close(pstNetLink->iSockfd);
        return (-1);
    }

    return 0;
}

static int PutNetlinkSocket(NETLINK_SOCKET_ST *pstNetLink)
{
    if (!pstNetLink)
    {
        loginfo(FUNC, "args NULL");
        return (-1);
    }

    if (0 <= pstNetLink->iSockfd)
    {
        close(pstNetLink->iSockfd);
    }

    memset(pstNetLink, 0, sizeof(NETLINK_SOCKET_ST));

    return (0);
}

int WriteAllWanGlobalIP(ADDRSTATE_ST *pstState)
{
    FILE *fp = NULL;
    char acFilePath[64] = {0};
    char acIfname[IFNAMSIZ] = {0};
    int i = 0;

    FUNCBEGIN();
    
    if (!pstState)
    {
        loginfo(FUNC, "args NULL");
        return (-1);
    }

    if (!if_indextoname(pstState->ifindex, acIfname))
    {
        loginfo(FUNC, "if_indextoname error");
        return (-1);
    }

    snprintf(acFilePath, 63, WAN_IPADDR6_FILE, acIfname);

    fp = fopen(acFilePath, "w");
    if (!fp)
    {
        fp = fopen(acFilePath, "w");
        if (!fp)
        {
            fprintf(stderr, "open file [%s] error", acFilePath);
            return (-1);
        }
    }

    for (i = 0; i < ADDR_MAX_NUM; i++)
    {
        if (pstState->astAddr[i].iFlg & en_item_used)
        {
            WriteIPIntoFile(fp, pstState->astAddr[i].stAddr, 
                    pstState->astAddr[i].iPrefixLen);
        }
    }

    fclose(fp);

    FUNCEND();

    return (0);
}

/*BEGIN 2032203980  m00182044 2012-06-10 modify*/
int GetAllWanGlobalIP(char *devname, char *buf)
{
    FILE *fp = NULL;
    char acFilePath[64] = {0};
    char acIfname[IFNAMSIZ] = {0};
    int i = 0;

    FUNCBEGIN();
    
    if (!buf)
    {
        loginfo(FUNC, "args NULL");
        return (-1);
    }

    snprintf(acFilePath, 63, WAN_PREFIX_FILE, devname);

    fp = fopen(acFilePath, "r");
    if (!fp)
    {
        fp = fopen(acFilePath, "r");
        if (!fp)
        {
            fprintf(stderr, "open file [%s] error", acFilePath);
            return (-1);
        }
    }

    fread(buf, 512, 1, fp);

    loginfo(FUNC, "GetAllWanGlobalIP prefix:%s", buf);
    
    fclose(fp);

    FUNCEND();

    return (0);
}
/*END 2032203980  m00182044 2012-06-10 modify*/


/* ��⵽WAN UP�� */
int WanMonitiorUpProcess(ADDRSTATE_ST *pstState)
{
    FUNCBEGIN();
    
    if (!pstState)
    {
        loginfo(FUNC, "Args NULL");
        return (-1);
    }

    /* ����״̬ */
    SetWanStatus(pstState->ifindex, ATP_WAN_STATUS_CONNECTING);


    /* �����еĵ�ַд���ļ��� */
    WriteAllWanGlobalIP(pstState);

    /* ���������RA����ô��RA����ϢҲ���и��� */

    /* BEGIN: Added by z67728, 2011/4/18 
       �޸��й���������:�����Ƿ�ΪSLAACģʽ,������RA����. */
#ifdef SUPPORT_ATP_IPV6_ALWAYS_SEND_RS
    UpdateRAInfoByWanMonitor(pstState->ifindex);
#else
    if (pstState->iEnableRA)
    {
        UpdateRAInfoByWanMonitor(pstState->ifindex);
        // TODO: append dhcp info
    }
    else
    {
        // TODO: write dhcp info
    }
#endif
    /* END:   Added by z67728, 2011/4/18 */

    loginfo(FUNC, "............set wan status6.................");

    /* ���ˣ�WAN ״̬ΪUP�� */
    SetWanStatus(pstState->ifindex, ATP_WAN_STATUS_CONNECTED);

    /* �ı�״̬ */
    NotifyWANStatus(pstState->ifindex);

    /* ֪ͨWAN Up�� */
    /*BEGIN 2032203980  m00182044 2012-06-10 modify*/
    BSP_SYS_WakeupMonitorTask(pstState->ifindex);
    /*END 2032203980  m00182044 2012-06-10 modify*/

    FUNCEND();

    return (0);
}


/* ��⵽WAN down��, down�Ŀ����б���״̬��ı����WAN�ĵ�ַ������� */
int WanMonitorDownProcess(ADDRSTATE_ST *pstState)
{
    FUNCBEGIN();
    
    if (!pstState)
    {
        loginfo(FUNC, "Args NULL");
        return (-1);
    }
    
    SetWanStatus(pstState->ifindex, ATP_WAN_STATUS_DISCONNECTING);

    /* �����⵽DOWN��������Ϊdial6�ļ����µ� */
    if(pstState->iReadDialFile)
    {
        if (pstState->iEnableRA)
        {
            //StopRunningRA(pstState->ifindex);
        }
        else
        {
            // TODO: clear DHCP status     
            //StopDhcp6c(pstState->ifindex, 1);
        }
        
        DelAllGlobalAddrs(pstState);
        DelAllGlobalRoute(pstState);
        ReInitSettings(pstState);
        
        pstState->iWanState = en_wan_down;
    }

    loginfo(FUNC, "detect wan down......");

    /* ���ipaddr6�ļ� */
    WriteAllWanGlobalIP(pstState);

    /* BEGIN: Added by z67728, 2011/4/18 
       �޸��й���������:�����Ƿ�ΪSLAACģʽ,������RA����. */
#ifdef SUPPORT_ATP_IPV6_ALWAYS_SEND_RS
    UpdateRAInfoByWanMonitor(pstState->ifindex);
#else
    /* ����û�е�ַ�˿��ܱ���ǰ׺����Ϣ��Ȼ������? */
    if (pstState->iEnableRA)
    {
        UpdateRAInfoByWanMonitor(pstState->ifindex);        
    }
    else
    {
        loginfo(FUNC, "need to clear dhcp info......");
        // TODO: write dhcp info
    }
#endif
    /* END:   Added by z67728, 2011/4/18 */

    loginfo(FUNC, "............set wan status6.................");
    
    SetWanStatus(pstState->ifindex, ATP_WAN_STATUS_DISCONNECTED);

    NotifyWANStatus(pstState->ifindex);
    
    /*BEGIN 2032203980  m00182044 2012-06-10 modify*/
    BSP_SYS_WakeupMonitorTask(pstState->ifindex);
    /*END 2032203980  m00182044 2012-06-10 modify*/


#if 0
    if ((!exit_addrMonitor) && (pstState->iReadDialFile))
    {
        //StartWanProcess(pstState);
    }
#endif

    FUNCEND();

    return (0);
}



int UpdateAddrState(ADDRSTATE_ST *pstState, int iCmd)
{
    int iAlreadyWakUp = 0;

    FUNCBEGIN();
    
    if (!pstState)
    {
        loginfo(FUNC, "args NULL");
        return (-1);
    }

    /*BEGIN 2032203980  m00182044 2012-06-10 modify*/
    loginfo(FUNC, "pstState->iInit:%d \n", pstState->iInit);
    loginfo(FUNC, "pstState->iGlobalAddrCnt:%d \n", pstState->iGlobalAddrCnt);
    /*END 2032203980  m00182044 2012-06-10 modify*/
    /* ����ǵ�һ�λ�ȡ״̬ ��ϵͳ��ʼ��ʱ���ȡһ�ε�ַ */
    if (UNINIT == pstState->iInit)
    {
        if (WAN_FIRST_GET_IP6_ADDR <= pstState->iGlobalAddrCnt)
        {
            loginfo(FUNC, "Wan up now.....\r\n");

            if (en_wan_up != pstState->iWanState)
            {
                pstState->iWanState = en_wan_up;
                WanMonitiorUpProcess(pstState);
                iAlreadyWakUp = 1;
            }
        }

        pstState->iInit = INIT;        
    }
    else
    {
        if (en_wan_up != pstState->iWanState)
        {
            if (WAN_FIRST_GET_IP6_ADDR <= pstState->iGlobalAddrCnt)
            {
                loginfo(FUNC, "Wan up now....\r\n");
                pstState->iWanState = en_wan_up;
                WanMonitiorUpProcess(pstState);
                iAlreadyWakUp = 1;
            }
        }
        else
        {
            if (en_wan_up == pstState->iWanState)
            {
                if (INTERFACE_NO_G_ADDR >= pstState->iGlobalAddrCnt)
                {
                    loginfo(FUNC, "Wan down now...\r\n");
                    pstState->iGlobalAddrCnt = INTERFACE_NO_G_ADDR;
                    WanMonitorDownProcess(pstState);
                    pstState->iWanState = en_wan_down;
                    //pstState->uiDialCmd = -1;
                    iAlreadyWakUp = 1;
                }
            }
        }
    }

    /* ��ַ�иı䣬��Ҫ֪ͨWAN���¶�ȡ */
    if ((!iAlreadyWakUp) && (iCmd))
    {
        NotifyWANStatus(pstState->ifindex);
        WriteAllWanGlobalIP(pstState);
        /*BEGIN 2032203980  m00182044 2012-06-10 modify*/
        BSP_SYS_WakeupMonitorTask(pstState->ifindex);
        /*END 2032203980  m00182044 2012-06-10 modify*/
    }

    loginfo(FUNC, "wan status: [%s]", 
        en_wan_up == pstState->iWanState? "UP" : "DOWN");

    FUNCEND();
    
    return (0);
}


ADDR_ITEM_ST *FindAddrItem(ADDRSTATE_ST *pstState, struct in6_addr stAddr)
{
    int i = 0;
    
    if (!pstState || (INTERFACE_NO_G_ADDR == pstState->iGlobalAddrCnt))
    {
        loginfo(FUNC, "args NULL");
        return (NULL);
    }

    for (; i < ADDR_MAX_NUM; i++)
    {
        if (en_item_free == pstState->astAddr[i].iFlg)
        {
            continue;
        }

        loginfo(FUNC, "compare : "NIP6_FMT " with " NIP6_FMT, 
            NIP6(stAddr), NIP6(pstState->astAddr[i].stAddr));
        
        if (!memcmp(&stAddr, &pstState->astAddr[i].stAddr, sizeof(stAddr)))
        {
            loginfo(FUNC, "Found address");
            return &pstState->astAddr[i];
        }
    }

    return (NULL);
}


ADDR_ITEM_ST *GetFreeAddrItem(ADDRSTATE_ST *pstState)
{
    int i = 0;
    
    if (!pstState || (ADDR_MAX_NUM <= pstState->iGlobalAddrCnt))
    {
        loginfo(FUNC, "args NULL");
        return (NULL);
    }

    for (; i < ADDR_MAX_NUM; i++)
    {
        if (en_item_free == pstState->astAddr[i].iFlg)
        {
            return &(pstState->astAddr[i]);
        }
    }

    return (NULL);
}


/* ����0��ʾ��ַ���ڣ�����1��ʾ��ַ�������� */
int AddAddrItem(ADDRSTATE_ST *pstState, struct in6_addr stAddr, int iPrefixLen)
{
    int i = 0;
    ADDR_ITEM_ST *p = NULL;
    
    if (ADDR_MAX_NUM <= pstState->iGlobalAddrCnt)
    {
        loginfo(FUNC, "address overflow");
        return (-1);
    }

    p = FindAddrItem(pstState, stAddr);
    if (p)
    {
        loginfo(FUNC, "already exist address");
        return (0);
    }

    p = GetFreeAddrItem(pstState);
    if (!p)
    {
        loginfo(FUNC, "Can not get free address");
        return (0);
    }

    pstState->iGlobalAddrCnt += 1;
    p->iFlg |= en_item_used;
    //p->iFlg |= en_item_new;
    p->iPrefixLen = iPrefixLen;
    memcpy(&p->stAddr, &stAddr, sizeof(stAddr));


    return (1);
}


/* ����1��ʾ�õ�ַ�����ұ�ɾ�� */
int DelAddrItem(ADDRSTATE_ST *pstState, struct in6_addr stAddr)
{
    int i = 0;
    ADDR_ITEM_ST *p = NULL;
    
    if (INTERFACE_NO_G_ADDR >= pstState->iGlobalAddrCnt)
    {
        loginfo(FUNC, "address not exist");
        return (-1);
    }

    p = FindAddrItem(pstState, stAddr);
    if (!p)
    {
        loginfo(FUNC, "no exist address");
        return (0);
    }

    pstState->iGlobalAddrCnt -= 1;
    if (INTERFACE_NO_G_ADDR > pstState->iGlobalAddrCnt)
    {
        pstState->iGlobalAddrCnt = INTERFACE_NO_G_ADDR;
    }
    
    p->iFlg = en_item_free;
    p->iPrefixLen = 0;
    memset(&p->stAddr, 0, sizeof(stAddr));

    return (1);
}


int GetGlobalAddress(ADDRSTATE_ST *pstState)
{
    FILE *fp = NULL;
    char  aucAddr[8][5] = {0};
    int ifindex = -1;
    int iPrefixLen = -1;
    int iScope;
    int iFlg;
    char acIfName[20] = {0};
    char acIpv6Addr[64] = {0};
    struct in6_addr stAddr;

    FUNCBEGIN();

    if (!pstState)
    {
        loginfo(FUNC, "nothing to do");
        return (0);
    }

    fp = fopen(INET6_ADDR_FILE, "r");
    if (!fp)
    {
        loginfo(FUNC, "open file %s error", INET6_ADDR_FILE);
        return (-1);
    }
    
    while (1)
    {
        if (EOF == fscanf(fp, "%4s%4s%4s%4s%4s%4s%4s%4s %7x %7x %7x %7x %20s\n",
            aucAddr[0], aucAddr[1], aucAddr[2], aucAddr[3], aucAddr[4],
            aucAddr[5], aucAddr[6], aucAddr[7], &ifindex, &iPrefixLen,
            &iScope, &iFlg, acIfName))
        {
            loginfo(FUNC, "read file [%s] over", INET6_ADDR_FILE);
            break;
        }

        if (ifindex != pstState->ifindex)
        {
            loginfo(FUNC, "skip %s", acIfName);
            continue;
        }

        snprintf(acIpv6Addr, 63, "%s:%s:%s:%s:%s:%s:%s:%s",
            aucAddr[0], aucAddr[1], aucAddr[2], aucAddr[3], aucAddr[4],
            aucAddr[5], aucAddr[6], aucAddr[7]);

        if (0 >= inet_pton(AF_INET6, acIpv6Addr, &stAddr))
        {
            loginfo(FUNC, "address error");
            continue;
        }
        
        if (IN6_IS_ADDR_LINKLOCAL(&stAddr) 
                || IN6_IS_ADDR_MULTICAST(&stAddr))
        {
            loginfo(FUNC, "skip link local address or multicast address [%s]", acIpv6Addr);
            continue;
        }

        loginfo(FUNC, "get global address : "NIP6_FMT, NIP6(stAddr));

        AddAddrItem(pstState, stAddr, iPrefixLen);
    }

    fclose(fp);

    FUNCEND();
    
    return (0);
}

static int PacketAttr(struct nlmsghdr *pstNlHdr, unsigned short uiType, 
    unsigned short uiLen, void *pvData)
{    
    int iLen = RTA_LENGTH(uiLen);
    struct rtattr *pstAttr = NULL;
    pstAttr = (struct rtattr *)(((char *)pstNlHdr) + NLMSG_ALIGN(pstNlHdr->nlmsg_len));
    pstAttr->rta_type = uiType;
    pstAttr->rta_len = iLen;
    memcpy(RTA_DATA(pstAttr), pvData, uiLen);
    pstNlHdr->nlmsg_len = 
        NLMSG_ALIGN(pstNlHdr->nlmsg_len) + iLen;
    
    return 0;
}

static int DelAddress(ADDRSTATE_ST *pstState, ADDR_ITEM_ST *pstAddrItem)
{
    struct msghdr stMsgHdr;
    struct iovec  stIOvec;
    ADDR_DEL_REQ_ST stReq;
    struct rtattr *pstAttr = NULL;
    
    if (!pstState || !pstAddrItem)
    {
        loginfo(FUNC, "No need to delete address");
        return (-1);
    }

    memset(&stReq, 0, sizeof(ADDR_DEL_REQ_ST));

    stReq.stNlMsgHdr.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifaddrmsg));
    stReq.stNlMsgHdr.nlmsg_flags = NLM_F_REQUEST;
    stReq.stNlMsgHdr.nlmsg_type = RTM_DELADDR;
    stReq.stNlMsgHdr.nlmsg_pid = pstState->stStateMonitor.stLocalAddr.nl_pid;
    stReq.stNlMsgHdr.nlmsg_seq = pstState->stStateMonitor.iSeq++;

    stReq.stAddr.ifa_family = AF_INET6;
    stReq.stAddr.ifa_prefixlen = pstAddrItem->iPrefixLen;
    stReq.stAddr.ifa_index = pstState->ifindex;

#if 1
    PacketAttr(&stReq.stNlMsgHdr, IFA_ADDRESS, 
        sizeof(struct in6_addr), &pstAddrItem->stAddr);
#else
    pstAttr = (struct rtattr *)stReq.acBuff;
    pstAttr->rta_type = IFA_ADDRESS;
    pstAttr->rta_len = RTA_LENGTH(sizeof(struct in6_addr));
    memcpy(RTA_DATA(pstAttr), &pstAddrItem->stAddr, sizeof(struct in6_addr));
    stReq.stNlMsgHdr.nlmsg_len = 
        NLMSG_ALIGN(stReq.stNlMsgHdr.nlmsg_len) + RTA_LENGTH(sizeof(struct in6_addr));
#endif

    memset(&stMsgHdr, 0, sizeof(struct msghdr));
    stIOvec.iov_base = (caddr_t)&stReq;
    stIOvec.iov_len = stReq.stNlMsgHdr.nlmsg_len;
    stMsgHdr.msg_iov = &stIOvec;
    stMsgHdr.msg_iovlen = 1;

    return sendmsg(pstState->stStateMonitor.iSockfd, &stMsgHdr, 0);
}

int DelAllGlobalAddrs(ADDRSTATE_ST *pstState)
{
    int i = 0;
    
    if (!pstState || (0 >= pstState->stStateMonitor.iSockfd))
    {
        loginfo(FUNC, "Error: state not right");
        return (-1);
    }

    for (i = 0; i < ADDR_MAX_NUM; i++)
    {
        if (en_item_free == pstState->astAddr[i].iFlg)
        {
            continue;
        }

        loginfo(FUNC, "del address.......");
        
        DelAddress(pstState, &pstState->astAddr[i]);

        memset(&pstState->astAddr[i].stAddr, 0, sizeof(struct in6_addr));
        pstState->astAddr[i].iFlg = en_item_free;

        pstState->iGlobalAddrCnt -= 1;
    }

    return 0;
}

static int DelRoute(ADDRSTATE_ST *pstState, struct in6_addr stDst, int iDstLen, 
    struct in6_addr stSrc, int iSrcLen, struct in6_addr stNextHop,
    unsigned int uiMetric)
{
    struct msghdr stMsgHdr;
    struct iovec  stIOvec;
    ROUTE_DEL_REQ_ST stReq;
    struct rtattr *pstAttr = NULL;
    int iLen = 0;
    
    if (!pstState)
    {
        loginfo(FUNC, "args NULL");
        return (-1);
    }
    
    memset(&stReq, 0, sizeof(ADDR_DEL_REQ_ST));
    stReq.stNlMsgHdr.nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg));
    stReq.stNlMsgHdr.nlmsg_flags = NLM_F_REQUEST;
    stReq.stNlMsgHdr.nlmsg_type = RTM_DELROUTE;
    stReq.stNlMsgHdr.nlmsg_pid = pstState->stStateMonitor.stLocalAddr.nl_pid;
    stReq.stNlMsgHdr.nlmsg_seq = pstState->stStateMonitor.iSeq++;

    stReq.stRt.rtm_family = AF_INET6;
    stReq.stRt.rtm_table = 0;
    stReq.stRt.rtm_dst_len = iDstLen;
    stReq.stRt.rtm_src_len = iSrcLen;

#if 1
    PacketAttr(&stReq.stNlMsgHdr, RTA_GATEWAY, 
        sizeof(struct in6_addr), (void *)&stNextHop);

    PacketAttr(&stReq.stNlMsgHdr, RTA_DST, 
        sizeof(struct in6_addr), (void *)&stDst);
    
    PacketAttr(&stReq.stNlMsgHdr, RTA_SRC, 
        sizeof(struct in6_addr), (void *)&stSrc);
    
    PacketAttr(&stReq.stNlMsgHdr, RTA_OIF, 
        4, (void *)&pstState->ifindex);
    
    PacketAttr(&stReq.stNlMsgHdr, RTA_METRICS, 
        4, (void *)&uiMetric);
#else
    /* gateway */
    pstAttr = (struct rtattr *)stReq.acBuff;
    pstAttr->rta_type = RTA_GATEWAY;
    pstAttr->rta_len = RTA_LENGTH(sizeof(struct in6_addr));
    memcpy(RTA_DATA(pstAttr), &stNextHop, sizeof(struct in6_addr));
    iLen += pstAttr->rta_len;

    /* dst */
    pstAttr = (struct rtattr *)(stReq.acBuff + iLen);
    pstAttr->rta_type = RTA_DST;
    pstAttr->rta_len = RTA_LENGTH(sizeof(struct in6_addr));
    memcpy(RTA_DATA(pstAttr), &stDst, sizeof(struct in6_addr));
    iLen += pstAttr->rta_len;

    /* src */
    pstAttr = (struct rtattr *)(stReq.acBuff + iLen);
    pstAttr->rta_type = RTA_SRC;
    pstAttr->rta_len = RTA_LENGTH(sizeof(struct in6_addr));
    memcpy(RTA_DATA(pstAttr), &stSrc, sizeof(struct in6_addr));
    iLen += pstAttr->rta_len;

    /* outbound index */
    pstAttr = (struct rtattr *)(stReq.acBuff + iLen);
    pstAttr->rta_type = RTA_OIF;
    pstAttr->rta_len = RTA_LENGTH(4);
    memcpy(RTA_DATA(pstAttr), &pstState->ifindex, 4);
    iLen += pstAttr->rta_len;
    
    pstAttr = (struct rtattr *)(stReq.acBuff + iLen);
    pstAttr->rta_type = RTA_METRICS;
    pstAttr->rta_len = RTA_LENGTH(4);
    memcpy(RTA_DATA(pstAttr), &uiMetric, 4);
    iLen += pstAttr->rta_len;
#endif

    memset(&stMsgHdr, 0, sizeof(struct msghdr));
    stIOvec.iov_base = (caddr_t)&stReq;
    stIOvec.iov_len = stReq.stNlMsgHdr.nlmsg_len;
    stMsgHdr.msg_iov = &stIOvec;
    stMsgHdr.msg_iovlen = 1;

    return sendmsg(pstState->stStateMonitor.iSockfd, &stMsgHdr, 0);
}

int DelAllGlobalRoute(ADDRSTATE_ST *pstState)
{
    FILE *fp = NULL;
    unsigned char acDst[8][5];
    unsigned char acSrc[8][5];
    unsigned char acNextHop[8][5];
    int iDstLen = -1;
    int iSrcLen = -1;
    unsigned int uiMetric = -1;
    int iRef = -1;
    int iUse = -1;
    unsigned int uiFlg = -1;
    char acDevName[IFNAMSIZ];
    char acIfName[IFNAMSIZ];   
    char acTmp[64];
    struct in6_addr stDst, stSrc, stNextHop;

    FUNCBEGIN();

    fp = fopen(INET6_ROUTE_FILE, "r");
    if (!fp)
    {
        loginfo(FUNC, "open file [%s] err", INET6_ROUTE_FILE);
        return (-1);
    }

    if (!if_indextoname(pstState->ifindex, acIfName))
    {
        loginfo(FUNC, "if_nametoindex error");
        fclose(fp);
        return (-1);
    }


    while (1)
    {
        if (EOF == fscanf(fp, "%4s%4s%4s%4s%4s%4s%4s%4s %2x %4s%4s%4s%4s%4s%4s%4s%4s"
            " %2x %4s%4s%4s%4s%4s%4s%4s%4s %8x %8x %8x %8x %15s\n",
            acDst[0], acDst[1], acDst[2], acDst[3], acDst[4], acDst[5], acDst[6], 
            acDst[7], &iDstLen, acSrc[0], acSrc[1], acSrc[2], acSrc[3], acSrc[4], 
            acSrc[5], acSrc[6], acSrc[7], &iSrcLen, acNextHop[0], acNextHop[1],
            acNextHop[2], acNextHop[3], acNextHop[4], acNextHop[5], acNextHop[6],
            acNextHop[7], &uiMetric, &iRef, &iUse, &uiFlg, acDevName))
        {
            loginfo(FUNC, "file read over......");
            break;
        }

        if (0 != strcmp(acIfName, acDevName))
        {
            loginfo(FUNC, "[%s] not dev [%s]", acDevName, acIfName);
            continue;
        }

        snprintf(acTmp, 63, "%s:%s:%s:%s:%s:%s:%s:%s",
            acDst[0], acDst[1], acDst[2], acDst[3],
            acDst[4], acDst[5], acDst[6], acDst[7]);
        if (0 >= inet_pton(AF_INET6, acTmp, (void *)&stDst))
        {
            loginfo(FUNC, "inet_pton error [%s]", acTmp);
            continue;
        }

        if (IN6_IS_ADDR_LINKLOCAL(&stDst)
            || IN6_IS_ADDR_MULTICAST(&stDst))
        {
            loginfo(FUNC, "link-local or multicast route skip");
            continue;
        }
        
        snprintf(acTmp, 63, "%s:%s:%s:%s:%s:%s:%s:%s",
            acSrc[0], acSrc[1], acSrc[2], acSrc[3],
            acSrc[4], acSrc[5], acSrc[6], acSrc[7]);
        if (0 >= inet_pton(AF_INET6, acTmp, (void *)&stSrc))
        {
            loginfo(FUNC, "inet_pton error");
            continue;
        }

        
        snprintf(acTmp, 63, "%s:%s:%s:%s:%s:%s:%s:%s",
            acNextHop[0], acNextHop[1], acNextHop[2], acNextHop[3],
            acNextHop[4], acNextHop[5], acNextHop[6], acNextHop[7]);
        if (0 >= inet_pton(AF_INET6, acTmp, (void *)&stNextHop))
        {
            loginfo(FUNC, "inet_pton error");
            continue;
        }

        loginfo(FUNC, "del route: dst:[%s/%d] src:[%s/%d] nexthop[%s] dev[%s]",
            acDst, iDstLen, acSrc, iSrcLen, acNextHop, acDevName);

        DelRoute(pstState, stDst, iDstLen, stSrc, iSrcLen, stNextHop,
            uiMetric);
    }

    fclose(fp);

    FUNCEND();

    return 0;
}



int ReInitSettings(ADDRSTATE_ST *pstState)
{
    FUNCBEGIN();
    
    if (!pstState)
    {
        loginfo(FUNC, "args NULL");
        return (-1);
    }

    pstState->iGlobalAddrCnt = INTERFACE_NO_G_ADDR;
    pstState->iInit = UNINIT;

    FUNCEND();

    return (0);
}



/* ����������1��ʾ��ַ�иı� */
int ProcessKrnlAddrStateMsg(ADDRSTATE_ST *pstState, int iAccept)
{
    int iRet = -1;
    int iLen = -1;
    int iLeft = -1;
    int iCnt = 0;
    struct iovec iov;
    struct sockaddr_nl stNlAddr;
    struct nlmsghdr *pstNhdr;
    struct msghdr stMsgHdr;
    struct ifaddrmsg *pstAddr = NULL;
	struct rtattr * pastrta_tb[IFA_MAX+1];    
	struct rtattr * pstAddrAttr = NULL;    
	struct rtattr * pstTmpAttr = NULL;
    struct in6_addr stAddr;
    unsigned char *p = NULL;
    char   acBuff[MAX_BUFFER];

    FUNCBEGIN();
    
    if (!pstState || (0 > pstState->stStateMonitor.iSockfd))
    {
        loginfo(FUNC, "args NULL");
        return (-1);
    }

    loginfo(FUNC, "Begin to process packet....");

    memset(&stNlAddr, 0, sizeof(struct sockaddr_nl));
    stNlAddr.nl_family = AF_NETLINK;
	stNlAddr.nl_pid = 0;
	stNlAddr.nl_groups = 0;
    
    memset(&stMsgHdr, 0, sizeof(struct msghdr));
    stMsgHdr.msg_name = (void *)&stNlAddr;
    stMsgHdr.msg_namelen = sizeof(stNlAddr);
    iov.iov_base = (void *)acBuff;
    iov.iov_len = 2048;
    stMsgHdr.msg_iovlen = 1;
    stMsgHdr.msg_iov = &iov;


    iRet = recvmsg(pstState->stStateMonitor.iSockfd, &stMsgHdr, 0);
    if (0 >= iRet)
    {
        loginfo(FUNC, "recvmsg error");
        return (-1);
    }

    if (!iAccept)
    {
        loginfo(FUNC, "drop packet.....");
        return (0);
    }
    
    if (sizeof(stNlAddr) != stMsgHdr.msg_namelen)
    {
        loginfo(FUNC, "Size error");
        return (-1);
    }

    for (pstNhdr = (struct nlmsghdr *)acBuff; iRet > sizeof(*pstNhdr); )
    {
        loginfo(FUNC, "loop begin......");
        
        iLen = pstNhdr->nlmsg_len;
        iLeft = iLen - NLMSG_LENGTH(sizeof(*pstNhdr));
        
        if ((0 >= iLeft) || (iLeft > iRet) 
                || (stMsgHdr.msg_flags & MSG_TRUNC))
        {
            loginfo(FUNC, "msg error");
            break;
        }

        loginfo(FUNC, "msg type: %d", pstNhdr->nlmsg_type);

        if ((RTM_NEWADDR == pstNhdr->nlmsg_type) 
            || (RTM_DELADDR == pstNhdr->nlmsg_type) )
        {
            pstAddr = NLMSG_DATA(pstNhdr);
            if ((AF_INET6 != pstAddr->ifa_family)
                || (pstState->ifindex != pstAddr->ifa_index))
            {
                loginfo(FUNC, "not ipv6 address or family");
                iRet -= NLMSG_ALIGN(iLen);
                pstNhdr = (struct nlmsghdr *)((char *)pstNhdr + NLMSG_ALIGN(iLen));
                continue;
            }
            if (0 >= (iRet - NLMSG_LENGTH(sizeof(*pstAddr))))
            {
                loginfo(FUNC, "not ipv6 address");
                iRet -= NLMSG_ALIGN(iLen);
                pstNhdr = (struct nlmsghdr *)((char *)pstNhdr + NLMSG_ALIGN(iLen));
                continue;
            }

            pstTmpAttr = IFA_RTA(pstAddr);
            while (RTA_OK(pstTmpAttr, iLeft)) 
            {
                if (IFA_ADDRESS == pstTmpAttr->rta_type)
                {
                    loginfo(FUNC, "found address attr");
                    pstAddrAttr = pstTmpAttr;
                    break;
                }
                pstTmpAttr = RTA_NEXT(pstTmpAttr,iLeft);
            }
            
            if (!pstAddrAttr)
            {
                loginfo(FUNC, "Error: no address give");
                iRet -= NLMSG_ALIGN(iLen);
                pstNhdr = (struct nlmsghdr *)((char *)pstNhdr + NLMSG_ALIGN(iLen));
                continue;
            }

            memcpy(&stAddr, RTA_DATA(pstAddrAttr), sizeof(stAddr));
            
            if ((IN6_IS_ADDR_LINKLOCAL(&stAddr) 
                || IN6_IS_ADDR_MULTICAST(&stAddr)))
            {
                loginfo(FUNC, "Error: no global address");
                iRet -= NLMSG_ALIGN(iLen);
                pstNhdr = (struct nlmsghdr *)((char *)pstNhdr + NLMSG_ALIGN(iLen));
                continue;
            }

            if (RTM_NEWADDR == pstNhdr->nlmsg_type)
            {
                loginfo(FUNC, "good, get new global address");
                iCnt = AddAddrItem(pstState, stAddr, pstAddr->ifa_prefixlen);
            }
            else
            {   
                loginfo(FUNC, "good, del new global address");
                iCnt = DelAddrItem(pstState, stAddr);
            }
        }

        iRet -= NLMSG_ALIGN(iLen);
        pstNhdr = (struct nlmsghdr *)((char *)pstNhdr + NLMSG_ALIGN(iLen));
    }

    FUNCEND();

    return iCnt;
}

ADDRSTATE_ST *FindAddrStateByInx(int iIfindex)
{
    ADDRSTATE_ST *p = NULL;
    if (NULL == g_pstAddrState)
    {
        loginfo(FUNC, "can not found one");
        return (NULL);
    }

    ADDRSTATE_LOCK();

    for (p = g_pstAddrState; p; p = p->pstNext)
    {
        if (iIfindex == p->ifindex)
        {            
            ADDRSTATE_UNLOCK();
            return (p);
        }
    }

    ADDRSTATE_UNLOCK();
    return (NULL);
}

ADDRSTATE_ST *GetAddrState(void)
{
    ADDRSTATE_ST *p = NULL;
    p = malloc(sizeof(ADDRSTATE_ST));
    if (!p)
    {
        loginfo(FUNC, "error: malloc error");
        return (NULL);
    }

    memset(p, 0, sizeof(*p));
    
    ADDRSTATE_LOCK();
    
    p->pstNext = g_pstAddrState;
    g_pstAddrState = p;
    
    ADDRSTATE_UNLOCK();

    return p;
}


int PutAddrState(ADDRSTATE_ST *pstNode, int ifindex)
{
    ADDRSTATE_ST *p = NULL;

    if (!g_pstAddrState)
    {
        loginfo(FUNC, "list NULL");
        return (-1);
    }

    if (!pstNode)
    {
        pstNode = FindAddrStateByInx(ifindex);
        if (!pstNode)
        {
            loginfo(FUNC, "can not found the node");
            return (-1);
        }
    }

    ADDRSTATE_LOCK();

    p = g_pstAddrState;
    if (pstNode == g_pstAddrState)
    {
        g_pstAddrState = pstNode->pstNext;
        free(pstNode);
    }
    else
    {
        for (; p; p = p->pstNext)
        {
            if (pstNode == p->pstNext)
            {
                break;
            }
        }
        if (p)
        {
            p->pstNext = pstNode->pstNext;
        }
        free(pstNode);
    }
    
    ADDRSTATE_UNLOCK();

    return (0);
}


/* ����⵽����״̬Ϊ������ʱ����ñ���������ɲ��Ŷ��� */
int StartWanProcess(ADDRSTATE_ST *p)
{
    FUNCBEGIN();
    
    p->iWanState = en_wan_down;

    SetWanStatus(p->ifindex, ATP_WAN_STATUS_CONNECTING);

    loginfo(FUNC, "now, begin to start process wan up.......");
    
    /* ����Ҳ���������³�ʼ��һ�Σ��Է�ֹ�ڴ�֮ǰ�Ѿ���ԭ���ı��ĵ��½ӿ�UP�� */
    DelAllGlobalAddrs(p);
    DelAllGlobalRoute(p);

    ReInitSettings(p);

    /* BEGIN: Added by z67728, 2011/4/18 
       �޸��й���������:�����Ƿ�ΪSLAACģʽ,������RA����. */
#ifdef SUPPORT_ATP_IPV6_ALWAYS_SEND_RS
    StartRunningRA(p->ifindex);
#else
    if (p->iEnableRA)
    {
        StartRunningRA(p->ifindex);
    }
#endif
    /* END:   Added by z67728, 2011/4/18 */

    GetGlobalAddress(p);
    UpdateAddrState(p, 0);

    if (!p->iEnableRA)
    {
        loginfo(FUNC, "now, begin to start dhcp6c.....");
        StartDhcp6c(p->ifindex, p->iIANAFlag, p->iIAPDFlag, p->iDsliteFlag);
    }

    FUNCEND();

    return (0);
}


void StopWanProcessByIndex(int iIndex)
{
    ADDRSTATE_ST *p = FindAddrStateByInx(iIndex);

    FUNCBEGIN();
    
    if (!p)
    {
        return ;
    }
    StopWanProcess(p);

    FUNCEND();
}

/* ����⵽����״̬Ϊ��������ʱ����ñ����������ȥ���Ŷ��� */
int StopWanProcess(ADDRSTATE_ST *p)
{
    FUNCBEGIN();

    /* ����״̬Ϊ�Ͽ��� */
    SetWanStatus(p->ifindex, ATP_WAN_STATUS_DISCONNECTING);

    loginfo(FUNC, "begin to stop dhcp client or ra....");

    /* BEGIN: Added by z67728, 2011/4/18 
       �޸��й���������:�����Ƿ�ΪSLAACģʽ,������RA����. */
#ifdef SUPPORT_ATP_IPV6_ALWAYS_SEND_RS
    /* ���������RA,��ôͣ��RA */
    StopRunningRA(p->ifindex);

    if (!p->iEnableRA)
    {
        loginfo(FUNC, "now, begin to stop dhcp6c....");
        StopDhcp6c(p->ifindex, 1);
    }
#else
    /* ���������RA,��ôͣ��RA */
    if (p->iEnableRA)
    {
        StopRunningRA(p->ifindex);
    }
    else        /* ���û�п���RA����ô�����Ǵ�DHCP״̬��ʱ��Ҫ���DHCP��Ϣ */
    {
        loginfo(FUNC, "now, begin to stop dhcp6c....");
        StopDhcp6c(p->ifindex, 1);
    }
#endif
    /* END:   Added by z67728, 2011/4/18 */

    loginfo(FUNC, "begin to del all info about wan");

    /* �����нӿ�ȫ�ֵ�ַ��·����Ϣɾ�� */
    DelAllGlobalAddrs(p);
    DelAllGlobalRoute(p);

    /* Ȼ�����״̬ */
    UpdateAddrState(p, 1);

    //p->iGlobalAddrCnt = INTERFACE_NO_G_ADDR;    
    ReInitSettings(p);

    p->iWanState = en_wan_down;

    //p->iInit = UNINIT;

    SetWanStatus(p->ifindex, ATP_WAN_STATUS_DISCONNECTED);


    FUNCEND();

    return (0);
}


static int GetWANDialStatusCode(ADDRSTATE_ST *p)
{
    FILE *fp = NULL;    
    unsigned int uiDialCmd = -1;
    char    acIfname[IFNAMSIZ] = {0};
    char    acFilePath[32] = {0};

    FUNCBEGIN();

    if (!p)
    {
        loginfo(FUNC, "args NULL");
        return (-1);
    }
    
    if (!if_indextoname(p->ifindex, acIfname))
    {
        loginfo(FUNC, "error: if_indextoname");
        exit(-1);
    }
    
    snprintf(acFilePath, 31, WAN_DIAL6_FILE, acIfname);

    /* ��ȡWAN�����ļ������Ƿ��𲦺� */
    fp = fopen(acFilePath, "r");
    if (NULL == fp)
    {
        loginfo(FUNC, "open file [%s] error", acFilePath);
        return (-1);
    }

    fscanf(fp, "%u", &uiDialCmd);
    fclose(fp);

    /* ���״̬û�иı� */
    if (uiDialCmd == p->uiDialCmd)
    {
        /* ���״̬���ǲ���״̬����ô����0���Ӷ������ȴ� */
        /*BEGIN 2032203980  m00182044 2012-06-10 modify*/
        if (WAN_TRIGGER_AUTOON != uiDialCmd)
        {
            loginfo(FUNC, "wan status not up , so still sleep......");
            return (0);
        }

        return (1);
    }
    
    loginfo(FUNC, "new status......: from [%u]-->[%u]", 
        p->uiDialCmd, uiDialCmd);

    /* ״̬�ı� */
    switch(uiDialCmd)
    {
        case WAN_TRIGGER_IDLE:
        case WAN_TRIGGER_MANUAL:
        case WAN_TRIGGER_DISCONNECTING:
        {
            loginfo(FUNC, "wan not up status......");
            
            /* ���֮ǰ�����ߵģ���ô����Ҫ���� */
            if (WAN_TRIGGER_AUTOON == p->uiDialCmd)
            {
                loginfo(FUNC, "wan have up before... so stop it......");
                StopWanProcess(p);
            }                        
            p->uiDialCmd = uiDialCmd;                
            return (0);
        }
        
        case WAN_TRIGGER_AUTOON:
        {
            loginfo(FUNC, "wan is set up now.....");
            /* ���֮ǰ�ǲ����ߵģ���ô����Ҫ���ߣ��������Ǳ��翪��RA���ص� */
            if (WAN_TRIGGER_AUTOON != p->uiDialCmd)
            {
                loginfo(FUNC, "begin to wan up....... .......");
                StartWanProcess(p);
            }
            p->uiDialCmd = uiDialCmd;                
            return (1);
        }
        
        default:
        {
            if (WAN_TRIGGER_AUTOON == p->uiDialCmd)
            {
                loginfo(FUNC, "wan have up before... so stop it......");
                StopWanProcess(p);
            }                        
            p->uiDialCmd = uiDialCmd;                
            return (0);
        }
    }                
}


void *MonitorLoop(void *arg)
{
    int iCnt = 0;
    fd_set  stFdSet;
    ADDRSTATE_ST *p = (ADDRSTATE_ST *)arg;
    int     err = 0;
    int     iRet = 0;
    struct timeval t;
    static unsigned long ulTimeLft = 0;
    unsigned long ulTime = 0;
    int lMaxSock = 0;

    if (!p)
    {
        loginfo(FUNC, "How can args NULL");
        exit(-1);
    }


    loginfo(FUNC, "MonitorLoop: %d\r\n\r\n", getpid());
    /*END 2032203980  m00182044 2012-06-10 modify*/

    SetWanStatus(p->ifindex, ATP_WAN_STATUS_DISCONNECTED);

    /* ����Ҫ��⣬ֱ�Ӿ���Ϊ��UP */
    if (p->iReadDialFile)
    {
        loginfo(FUNC, "no need to read dial6");
        StartWanProcess(p);
    }
    
    memset(&t, 0, sizeof(struct timeval));
    
    while (!exit_addrMonitor)
    {
        if (!(p->iReadDialFile))
        {
            if (0 < GetWANDialStatusCode(p))
            {
                iRet = 1;
            }
            else
            {
                loginfo(FUNC, "wan status no up, sleep..");
                iRet = 0;
            }
        }
        else
        {
            iRet = 1;
        }
        
        t.tv_sec = 1;
        t.tv_usec = 0;
        
        FD_ZERO(&stFdSet);
        FD_SET(p->stStateMonitor.iSockfd, &stFdSet);
        
        FD_SET(g_lSignalPipev6[0], &stFdSet);
        lMaxSock = g_lSignalPipev6[0] > p->stStateMonitor.iSockfd ? g_lSignalPipev6[0] : p->stStateMonitor.iSockfd;
        
        err = select(lMaxSock + 1,&stFdSet, NULL, NULL, &t);
        if (0 > err)
        {
            if (EINTR == errno)
            {
                continue;
            }
            loginfo(FUNC, "select error, please check it.....");
            //break;
        }

        if (FD_ISSET(p->stStateMonitor.iSockfd, &stFdSet))
        {
            loginfo(FUNC, "Get message form kernel....");
            iCnt = ProcessKrnlAddrStateMsg(p, iRet); 
            
            if (iRet)
            {
                UpdateAddrState(p, iCnt);
            }
        }
        else if (0 == err)
        {
            //loginfo(FUNC, "time out here...");
            continue;
        }
    }

    StopWanProcess(p);
    PutNetlinkSocket(&p->stStateMonitor);    

    /* ֪ͨDHCPC�˳��� */
    ExitProcessID(p->ifindex);

    PutAddrState(p, 0);    
    exit_ok = 1;

    loginfo(FUNC, "pthread %0x exit......\r\n\r\n\r\n\r\n", pthread_self());

    pthread_exit(0);

    return 0;
}

int StartMonitorWanPthread(char *pszIfName, int iEnableRA, 
    int iIANAFlag, int iIAPDFlag, int iDsliteFlag, int iReadDialFile)
{
    int     ifindex = -1;
    int     err = 0;
    ADDRSTATE_ST *p = NULL;
    
    if (!pszIfName)
    {
        loginfo(FUNC, "args NULL");
        return (-1);
    }
    
    ifindex = if_nametoindex(pszIfName);
    if (0 >= ifindex)
    {
        loginfo(FUNC, "dev [%s] error", pszIfName);
        return (-1);
    }

    p = FindAddrStateByInx(ifindex);
    if (p)
    {
        loginfo(FUNC, "Error: dev [%s] already exist", pszIfName);
        return (-1);
    }

    p = GetAddrState();
    if (!p)
    {
        loginfo(FUNC, "malloc error");
        return (-1);
    }
    
    if (0 != AddrStateInit(p, ifindex, iEnableRA, iIANAFlag, iIAPDFlag, iDsliteFlag, iReadDialFile))
    {
        PutAddrState(p, ifindex);
        loginfo(FUNC, "Interface state error");
        return (-1);
    }
    
    if (0 != PrepareNetLinkSocket(&p->stStateMonitor, NETLINK_ROUTE, 
                RTMGRP_IPV6_IFADDR, ifindex))
    {
        PutAddrState(p, ifindex);
        loginfo(FUNC, "prepare for socket error");
        return (-1);
    }
    if (0 != GetNetlinkSocket(&p->stStateMonitor))
    {
        loginfo(FUNC, "get netlink socket error");
        PutNetlinkSocket(&p->stStateMonitor);        
        PutAddrState(p, ifindex);
        return (-1);
    }

    loginfo(FUNC, "MonitorLoop proc start\n");
    if (0 != pthread_create(&p->tidp, NULL, MonitorLoop, (void *)p))
    {
        loginfo(FUNC, "pthread create error");
        PutNetlinkSocket(&p->stStateMonitor);        
        PutAddrState(p, ifindex);
        return (-1);
    }
    
    return (0);
}

int StopMonitorWanPthread(char *pszIfName)
{
    ADDRSTATE_ST *p = NULL;
    int     ifindex = -1;
    
    ifindex = if_nametoindex(pszIfName);
    if (0 >= ifindex)
    {
        loginfo(FUNC, "dev [%s] error", pszIfName);
        return (-1);
    }

    p = FindAddrStateByInx(ifindex);
    if (!p)
    {
        loginfo(FUNC, "can not get pthread for [%s]", pszIfName);
        return (-1);
    }

    pthread_exit(p->tidp);

    PutNetlinkSocket(&p->stStateMonitor);

    PutAddrState(p, 0);

    return (0);
}


#if 0

int main(int argc, char **argv)
{
    fd_set  stFdSet;

    if (2 != argc)
    {
        LOG_ERR("uage: %s dev", argv[0]);
        return (-1);
    }


    if (0 != AddrStateInit(&g_stAddrState, ifindex))
    {
        LOG_ERR("Interface state error");
        return (-1);
    }
    
    if (0 != PrepareNetLinkSocket(&g_stAddrState.stStateMonitor, NETLINK_ROUTE, 
                RTMGRP_IPV6_IFADDR, ifindex))
    {
        LOG_ERR("prepare for socket error");
        return (-1);
    }
    if (0 != GetNetlinkSocket(&g_stAddrState.stStateMonitor))
    {
        LOG_ERR("get netlink socket error");
        PutNetlinkSocket(&g_stAddrState.stStateMonitor);
        return (-1);
    }

    /* ѭ��֮ǰ�ȳ��Ի�ȡһ��IP��ַ */
    GetGlobalAddress(&g_stAddrState);
    UpdateAddrState(&g_stAddrState);

    while (1)
    {
        FD_ZERO(&stFdSet);
        FD_SET(g_stAddrState.stStateMonitor.iSockfd, &stFdSet);
        err = select((g_stAddrState.stStateMonitor.iSockfd + 1), 
                    &stFdSet, NULL, NULL, NULL);
        if (0 > err)
        {
            if (EINTR == errno)
            {
                continue;
            }
            break;
        }

        if (FD_ISSET(g_stAddrState.stStateMonitor.iSockfd, &stFdSet))
        {
            LOG_ERR("Get message form kernel....");
            ProcessKrnlAddrStateMsg(&g_stAddrState);            
            UpdateAddrState(&g_stAddrState);
        }
    }
    

    PutNetlinkSocket(&g_stAddrState.stStateMonitor);

    return 0;
}

#endif
