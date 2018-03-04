#ifndef __OPT_H__
#define __OPT_H__

#include <netinet/in.h>
#include <sys/types.h>
#include "ratlv.h"

#define PREFIX_TO_LAN       (1)
#define PREFIX_NO_TO_LAN    (0)

#define MAX_DFTRT_NUM       (16)
#define MAX_PREFIX_NUM      (16)
#define MAX_RDDNS_NUM       (16)

#define ND_OPT_RIO          (24)        /* rfc 4191 */
#define ND_OPT_RDNSS        (25)
#define INFINITY_LFT        (0xffffffff)

#define LIFETIME_TIMEOUT    (0)

/* DNSѡ�����С���� */
#define RA_RDDNS_LEN_MIN    (3)

#define IPV6_ADDR_LEN       (16)

typedef enum 
{
    EN_NEW          = (0x1),            /* �½ڵ� */
    EN_MODIFY       = (0x1 << 1),       /* �ýڵ㲻���µģ����޸� */
    EN_NO_MODIFY    = (0x1 << 2),       /* �ڵ�����û�� */
    EN_PROCESSED    = (0x1 << 3),       /* �ڵ㱻������� */
    EN_SEND         = (0x1 << 4)        /* �ýڵ㱻���͸�LAN���� */
}EN_NODE_FLAGS;

typedef enum
{
    EN_M_CHANGED = 1,
    EN_O_CHANGED = 2,
}EN_M_O_CHG_FLG;


typedef enum
{
    enNothing           = 0,            /* ��ʼ��״̬ */
    enMDHCP             = (1 << 1),     /* ͨ��M������DHCP */
    enODHCP             = (1 << 2),     /* ͨ��O������DHCP */
}EN_STATUS;


/* ǰ׺��Ϣ
 * stSrc ����������option���ó�������ÿһ���ṹ����ǰ׺,DNS��һ������Ϊ���ǵ����ܶ��·�����·�ͬһ��ǰ׺����DNS��Ϣ
 * ͬʱҲΪ�˷����ÿһ��ǰ׺����DNS��Ϣ���в���
 */
typedef struct __tagPrefix
{
    struct __tagPrefix  *pstNext;
    int                 bOnLink;            /* ���ֵʵ����û��ʲô�� */
    int                 bAutoConf;          /* ���ֵʵ����Ҳ���Բ���Ҫ�������������Ǳ�����չ */
    uint32_t            uiLft;
    uint32_t            uiVlft;    
    struct in6_addr     stPrefix;           /* ǰ׺�������ֽ��� */
    uint8_t             iPrefixLen;         
#if 0
    struct timeval      stUpdateT;          /* ��option��󱻸���ʱ�� */
#else
    long       lUpTime;           /* ��option��󱻸���ʱ�� */
#endif
#if 0
    int                 iIfIndex;           /* ���ĸ��ӿ��յ��� */
#endif
    int                 iGiveLan;           /* �Ƿ��·�����LAN�� */
    struct sockaddr_in6 stSrc;              /* ��ǰ׺����Դ(·������Դ��ַ) �Ѿ��������ֽ���� */
    int                 iFlg;               /* �ýڵ�ı�־������µ�RA�����˸ýڵ㵫�ǻ�û�б�������ô�����µ� */
}RA_PREFIX_ST;



/*
 * 0                   1                   2                   3
 * 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |     Type      |     Length    |           Reserved            |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                           Lifetime                            |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                                                               |
 * :            Addresses of IPv6 Recursive DNS Servers            :
 * |                                                               |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */
struct nd_opt_dns_info     /* dns information */
{
    uint8_t   nd_opt_dns_type;
    uint8_t   nd_opt_dns_len;    
    uint16_t  nd_opt_dns_reserved;
    uint32_t  nd_opt_dns_lifetime;
};


/* DNS��Ϣ */
typedef struct __tagDnsInfo
{
    struct  __tagDnsInfo    *pstNext;
    struct  in6_addr        stDnsAddr;
    uint32_t                uiLft;
#if 0
        struct timeval      stUpdateT;          /* ��option��󱻸���ʱ�� */
#else
        long       lUpTime;           /* ��option��󱻸���ʱ�� */
#endif
#if 0
        int                 iIfIndex;           /* ���ĸ��ӿ��յ��� */
#endif
    struct sockaddr_in6     stSrc;              /* ��ǰ׺����Դ(·������Դ��ַ) �Ѿ��������ֽ���� */
    int                     iFlg;               /* �ýڵ�ı�־����������˸ýڵ㵫�ǻ�û�б�������ô�����µ� */

}RA_DNS_INFO_ST;


/* Ĭ��·����Ϣ */
typedef struct _TagDefaultRoute
{
    struct _TagDefaultRoute *pstNext;
    struct in6_addr         stDftRouteAddr;     /* Ĭ��·�ɵ�ַ */
    uint32_t                uiLft;              /* Ĭ��·�ɴ��ʱ�� */
    uint8_t                 uiPrio;             /* Ĭ��·�����ȼ� */
    long                    lUpTime;           /* ��option��󱻸���ʱ�� */
    int                     iFlg;               /* ��·�ɱ�־ */
}RA_DEFAULT_ROUTE_ST;

/*iAssignPrefix, iAssignDNS����Ԥ��������������Ҫ������WAN��ʱ���ָ���Ƿ����WAN����Ϣ�·���LAN��  */
typedef struct __tagRAInfoList
{
    int             iPrefixNum;             /* ǰ׺����Ŀ */
    int             iDnsNum;                /* DNS����Ŀ */
    int             iDftRtNum;              /* Ĭ��·������ */
    int             iManaged;               /* M ��־ */
    int             iOtherInfo;             /* O ��־����ŵ�������յ������һ��RA�еĶ�Ӧ�ֶ� */
    int             iMOChg;                 /* M, O����Ƿ�ı��� */
    int             iDhcpStatus;            /* DHCP״̬��־ */
    int             iAssignPrefix;          /* ��ȡ��ǰ׺��Ϣ�Ƿ��·� */
    int             iAssignDNS;             /* ��ȡ��DNS��Ϣ�Ƿ��·� */
    int             iIfindex;               /* ��Щ��Ϣ�����ĸ��ӿ����յ��� */
    int             iMtu;    /*�·���MTUֵ*/
    RA_PREFIX_ST    *pstPrexList;
    RA_DNS_INFO_ST  *pstDNSList;
    RA_DEFAULT_ROUTE_ST *pstDftRtList; 
}RA_INFO_LIST_ST;

int ClearRAPrefix(RA_INFO_LIST_ST *pstList);
int ClearDNSInfo(RA_INFO_LIST_ST *pstList);
int ClearDftRoute(RA_INFO_LIST_ST *pstList);
int ProcessRAOption(ND_OPT_FMT_LINK_ST *pstOptList, RA_INFO_LIST_ST *pstList);


#endif
