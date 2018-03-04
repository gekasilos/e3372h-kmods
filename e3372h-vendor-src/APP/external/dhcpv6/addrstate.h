#ifndef __ADDR_STATE_H__
#define __ADDR_STATE_H__

#include <netinet/in.h>
//#include <sys/types.h>
#include <asm/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <pthread.h>


#define WAN_FIRST_GET_IP6_ADDR      (1)
#define INTERFACE_NO_G_ADDR         (0)

#define UNINIT                      (0)
#define INIT                        (1)

#define ADDR_MAX_NUM                (16)


#define NIP6(addr) \
	ntohs((addr).s6_addr16[0]), \
	ntohs((addr).s6_addr16[1]), \
	ntohs((addr).s6_addr16[2]), \
	ntohs((addr).s6_addr16[3]), \
	ntohs((addr).s6_addr16[4]), \
	ntohs((addr).s6_addr16[5]), \
	ntohs((addr).s6_addr16[6]), \
	ntohs((addr).s6_addr16[7])
#define NIP6_FMT "%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x"
#define NIP6_SEQFMT "%04x%04x%04x%04x%04x%04x%04x%04x"


typedef enum
{    
    en_item_free = 0,
    en_item_used = 1,
    en_item_new  = (1 << 2),            /* �½ڵ���Ҫ���� */
}ADDR_ITEM_STATUS_EN;

/* ״̬ */
typedef enum
{
    en_no_addr      = 0,
    en_one_addr     = 1,
    en_more_addr    = 2,
}ADDR_STATE_EN;


/* WAN״̬ */
typedef enum
{
    en_wan_down = 0,
    en_wan_up   = 1,
}WAN_STATE_EN;

/* �Ƿ��·� */
typedef enum
{
    en_not_assign   = 0,
    en_do_assign    = 1,
}ADDR_ASSIGN_EN;


typedef struct netlinksock
{
    int                 iSockfd;
    int                 iProtocol;
    int                 iIfIdx;         /* �ӿ�ID */
    uint32_t            iSeq;
    struct sockaddr_nl  stLocalAddr;
    struct sockaddr_nl  stPeerAddr;
}NETLINK_SOCKET_ST;


typedef struct AddrSaveItem
{
    int iFlg;
    int iPrefixLen;
    struct in6_addr stAddr;
}ADDR_ITEM_ST;

typedef struct AddrCnt
{
    int iCnt;
}IPV6_ADDR_CNT_ST;

typedef struct _TagNlReq
{
    struct nlmsghdr     stNlMsgHdr;
    struct ifaddrmsg    stAddr;
    char   acBuff[256]; 
}ADDR_DEL_REQ_ST;

typedef struct _TagNlRouteReq
{
    struct nlmsghdr     stNlMsgHdr;
    struct rtmsg        stRt;
    char                acBuff[256]; 
}ROUTE_DEL_REQ_ST;

typedef struct AddrState
{
    pthread_t tidp;                         /* �߳�ID */
    int     iInit;                          /* �Ƿ���ֱ�Ӷ�ȡ�ļ� */
    int     ifindex;                        /* �ӿ�ID */
    int     iGlobalAddrCnt;                 /* ȫ�ֵ�ַ���� */ 
    int     iWanState;                      /* WAN״̬ */
    int     iReadDialFile;                  /* �Ƿ��ȡdial6�ļ�, ����0��ʾ����ȡ */
    unsigned int  uiDialCmd;                /* ���Ǵ������һ�εĲ���״̬����Ϊ��ÿ��1s��ȥ��ѯWAN�Ĳ���״̬���
                                                                * �����ֵû�иı䣬��ô�Ϳ��Լ�������ǰ������
                                                                */
#if 0
    int     iState;                         /* ״̬ */
#endif
    int     iEnableRA;                      /* ��û��ʹ��RA����Ϊ������Ҫ������������״̬ */
    int     iIANAFlag;                      /* �Ƿ������ַ */
    int     iIAPDFlag;                      /* ��û������IAPD */
    int     iDsliteFlag;                    /* ��û������Dslite */
    int     iAssignPrefix;                  /* �Ƿ��·�ǰ׺ */
    int     iAssignDns;                     /* �Ƿ��·�DNS��Ϣ */
    NETLINK_SOCKET_ST stStateMonitor;       /* ״̬������ */
    ADDR_ITEM_ST astAddr[ADDR_MAX_NUM];     /* ���е�ַ */
    struct AddrState *pstNext;
    //IPV6_ADDR_CNT_ST stAddrCnt;
}ADDRSTATE_ST;

int StartMonitorWanPthread(char *pszIfName, int iEnableRA, 
    int iIANAFlag, int iIAPDFlag, int iDsliteFlag, int iReadDialFile);
int StopMonitorWanPthread(char *pszIfName);
int DelAllGlobalAddrs(ADDRSTATE_ST *pstState);
void StopWanProcessByIndex(int iIfindex);


#endif
