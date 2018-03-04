#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/icmp6.h>
#include <linux/if.h>
#include "ra.h"
#include "raopt.h"
#include "misc.h"
/*
  <���ⵥ��>     <��  ��>    <�޸�ʱ��>   <�汾> <�޸�����>
    2121101411  s00201037  2012/12/25        fortify����
    2032203980  m00182044   2012/6/10 DHCP6c����ͨ��RA������״̬IPv6��ַ���� 
    3123107867  y00188255    2014/1/15        ǰ׺���ں�ɾ��wan0�϶�Ӧ��ipv6��ַ
    4012701909  y00188255    2014/1/27        ����coverity����
    3121105278  s00216562    2014/2/22        ����PPPoE IPv6���Ų�֧��M=0��O=0��M=0��O=1��������Ĳ���

 */

#define BUFFER_LENTH_64  64
#define BUFFER_LENTH_256 256 
#define BYTE_LENTH  8
#define WANNAME "wan0"

#define LONG_MAX 4
extern int g_iEnableRA;
static int GetPrefixNum(RA_INFO_LIST_ST *pstList);

static int GetDNSNum(RA_INFO_LIST_ST *pstList);

static int GetDftRtNum(RA_INFO_LIST_ST *pstList);


static RA_PREFIX_ST *FindPrefix(RA_INFO_LIST_ST *pstList,
    struct in6_addr stAddr, unsigned char ucPrefixLen);

static RA_DNS_INFO_ST *FindDNS(RA_INFO_LIST_ST *pstList,
    struct in6_addr stAddr);

static long TimeSub(long lCurTime, long lOldTime);


static int GetPrefixNum(RA_INFO_LIST_ST *pstList)
{
    if (!pstList)
    {
        loginfo(FUNC, "args NULL");
        return (0);
    }
    return pstList->iPrefixNum;
}

static int GetDNSNum(RA_INFO_LIST_ST *pstList)
{
    if (!pstList)
    {
        loginfo(FUNC, "args NULL");
        return (0);
    }
    return pstList->iDnsNum;
}

static int GetDftRtNum(RA_INFO_LIST_ST *pstList)
{
    if (!pstList)
    {
        loginfo(FUNC, "args NULL");
        return (0);
    }
    return pstList->iDftRtNum;
}




/* ��ǰ׺�б��л�ȡ��Ӧ��ǰ׺�Ƿ���� */
static RA_PREFIX_ST *FindPrefix(RA_INFO_LIST_ST *pstList,
    struct in6_addr stAddr, uint8_t ucPrefixLen)
{
    RA_PREFIX_ST *p = NULL;
    if ((!pstList) || (0 >= GetPrefixNum(pstList)))
    {
        return (NULL);
    }

    p = pstList->pstPrexList;
    while (p)
    {
        if (0 == ComparePrefix(stAddr, ucPrefixLen, 
                            p->stPrefix, p->iPrefixLen))
        {
            return p;
        }
        p = p->pstNext;
    }

    return (NULL);
}

/* ��DNS�б��л�ȡ�Ƿ�DNS���� */
static RA_DNS_INFO_ST *FindDNS(RA_INFO_LIST_ST *pstList, struct in6_addr stAddr)
{
    RA_DNS_INFO_ST *p = NULL;
    if ((!pstList) || (0 >= GetDNSNum(pstList)))
    {
        return (NULL);
    }

    p = pstList->pstDNSList;
    while (p)
    {
        if (!memcmp(&stAddr, &p->stDnsAddr, sizeof(struct in6_addr)))
        {
            return (p);
        }
        p = p->pstNext;
    }

    return (NULL);
}

static RA_DEFAULT_ROUTE_ST *FindDftRoute(RA_INFO_LIST_ST *pstList, 
        struct in6_addr stAddr)
{
    RA_DEFAULT_ROUTE_ST *p = NULL;
    if ((!pstList) || (0 >= GetDftRtNum(pstList)))
    {
        return (NULL);
    }

    p = pstList->pstDftRtList;
    while (p)
    {
        if (!memcmp(&stAddr, &p->stDftRouteAddr, sizeof(struct in6_addr)))
        {
            return (p);
        }
        p = p->pstNext;
    }

    return (NULL);
}


/* ��������ʱ��Ĳ�ֵ������lCurTimeΪ��ǰ��sysinfo��ȡ���ģ�lOldTimeΪ֮ǰ�����ʱ�� */
static long TimeSub(long lCurTime, long lOldTime)
{
    long x = 0;
    if (lCurTime < lOldTime)
    {
        x = LONG_MAX - lOldTime + lCurTime;
        fprintf(stderr, "Notice: time overflow...");
    }
    else
    {
        x = lCurTime - lOldTime;
    }

    return (x);
}


/*****************************************************************************************************************/

static int WriteAllRADft(RA_INFO_LIST_ST *pstList);
static int FlushAllRADftFile(RA_INFO_LIST_ST *pstList);

// TODO: ���Ĭ��·�ɵ�ʱ�䷢���˸ı䣬��ôҲ��Ҫ��־����
int ProcessRADefaultRoute(RA_INFO_LIST_ST *pstList,
    void *pvNDHdr, struct sockaddr_in6 stFrom)
{
    RA_DEFAULT_ROUTE_ST *p = NULL;
    struct nd_router_advert *pstNDHdr;
    uint32_t         uiDftLft = 0;
    int              iNew = 0;
    
    if (!pvNDHdr || !pstList)
    {
        loginfo(FUNC, "Args NULL");
        return (-1);
    }

    pstNDHdr = (struct nd_router_advert *)pvNDHdr;
    uiDftLft = ntohs(pstNDHdr->nd_ra_router_lifetime);

    p = FindDftRoute(pstList, stFrom.sin6_addr);
    /* �½ڵ㣬��Ҫ��ʼ�� */
    if (!p && uiDftLft)
    {
        if (MAX_DFTRT_NUM <= GetDftRtNum(pstList))
        {
            loginfo(FUNC, "too many default route");
            return (-1);
        }

        loginfo(FUNC, "New default route node [" NIP6_FMT "] lifetime: %d", 
            NIP6(stFrom.sin6_addr), uiDftLft);
        
        p = malloc(sizeof(RA_DEFAULT_ROUTE_ST)); 
        if (!p)
        {
            loginfo(FUNC, "malloc error");
            return (-1);
        }
        memset(p, 0, sizeof(RA_DEFAULT_ROUTE_ST));
        iNew = 1;
    }

    if (p)
    {
        p->lUpTime = SysUpTime();
        memcpy(&p->stDftRouteAddr, &stFrom.sin6_addr, sizeof(struct in6_addr));
        p->uiLft = uiDftLft;
        // TODO: ����linux/icmpv6.hͷ�ļ�����Ĭ��·�����ȼ���¼����
        // TODO: ����Ŀǰ��uclib���linux/icmpv6.h����֧��icmpv6ͷ�ļ�
        //p->uiPrio = 0;
        if (iNew)
        {
            p->pstNext = pstList->pstDftRtList;
            pstList->pstDftRtList = p;
            pstList->iDftRtNum += 1;
            p->iFlg = EN_NEW;
        }
        else
        {
            p->iFlg |= EN_MODIFY;
        }
    }

    return (0);
}

int UpdateDftRt(RA_INFO_LIST_ST *pstList)
{
    RA_DEFAULT_ROUTE_ST *pstCur = NULL;
    RA_DEFAULT_ROUTE_ST *pstPrev = NULL;
    RA_DEFAULT_ROUTE_ST *pstTmpList = NULL;
    long ulCurrTime = SysUpTime();
    int iCnt = 0;

    //if (0 != GetDftRtNum(pstList))
    {
        pstCur = pstList->pstDftRtList;
        pstPrev = NULL;
        for (; pstCur; )
        {
            /* ���ڵĽڵ� */
            ulCurrTime = TimeSub(ulCurrTime, pstCur->lUpTime);
            
            if ((LIFETIME_TIMEOUT == pstCur->uiLft)
                || (ulCurrTime >= pstCur->uiLft))
            {
                if (NULL == pstPrev)
                {
                    pstList->pstDftRtList = pstCur->pstNext;
                    if (!(pstCur->iFlg & EN_SEND))
                    {
                        free(pstCur);
                    }
                    else
                    {
                        pstCur->pstNext = pstTmpList;
                        pstTmpList = pstCur;
                    }
                    pstPrev = NULL;
                    pstCur = pstList->pstDftRtList;
                }
                else
                {
                    pstPrev->pstNext = pstCur->pstNext;
                    if (!(pstCur->iFlg & EN_SEND))
                    {
                        free(pstCur);
                    }
                    else
                    {
                        pstCur->pstNext = pstTmpList;
                        pstTmpList = pstCur;
                    }
                    pstCur = pstPrev->pstNext;
                }

                pstList->iDftRtNum -= 1;
            }
            else
            {
                pstPrev = pstCur;
                pstCur = pstCur->pstNext;
            }
        }
    }

    // TODO: ֪ͨĬ��·��ʧЧ
    if (pstTmpList)
    {
        iCnt = 1;
    }

    if (pstTmpList)
    {
        while (pstTmpList)
        {
            pstPrev = pstTmpList;
            pstTmpList = pstTmpList->pstNext;
            free(pstPrev);
        }
    }

    // TODO:��֪���е�û��֪ͨ��Ĭ��·��
    /* �����Щ�ڵ�����ˣ���ô��Ҫˢ�£��Ӷ���֪��Ч�� */
    /* �ȼ����û���½ڵ� */
    // TODO: ����ڵ��־ΪEN_MODIFY ��ôҲ��Ҫ֪ͨ
    for (pstCur = pstList->pstDftRtList; pstCur; pstCur = pstCur->pstNext)
    {
        if (pstCur->iFlg & EN_NEW)
        {
            iCnt = 1;
            break;
        }
    }

    /* �ߵ��������м��ֿ���: 1 �нڵ�����ˣ�2 �����˽ڵ㣬����1��2������ */
    if (iCnt)
    {
        WriteAllRADft(pstList);
        return (1);
    }

    return (0);
}


/* ������е�Ĭ��·����Ϣ */
int ClearDftRoute(RA_INFO_LIST_ST *pstList)
{
    RA_DEFAULT_ROUTE_ST *p = NULL;

    if (!pstList)
    {
        loginfo(FUNC, "dft list is NULL");
        return (0);
    }

#if 0
    p = pstList->pstDftRtList;
    for (; p; p = p->pstNext)
    {
        if (p->iFlg & EN_SEND)
        {
            // TODO: ֪ͨLAN�࣬���߷���Ϣ �����Ӧ�ļ�
        }
    }
#endif

    FlushAllRADftFile(pstList);
    // TODO: ֪ͨLAN�࣬���߷���Ϣ �����Ӧ�ļ�
    
    p = pstList->pstDftRtList;
    while (pstList->pstDftRtList)
    {
        p = pstList->pstDftRtList;
        pstList->pstDftRtList = pstList->pstDftRtList->pstNext;
        free(p);
    }

    pstList->pstDftRtList = NULL;

    return 0;
}

/* ������Ĭ��·��д���ļ���ȥ */
// TODO: ��ʱ��Ҳд��
static int WriteAllRADft(RA_INFO_LIST_ST *pstList)
{
    FILE *fp = NULL;
    char acFilePath[64] = {0};
    char acIfname[IFNAMSIZ] = {0};
    RA_DEFAULT_ROUTE_ST *pstCur = NULL;

    if (!pstList)
    {
        loginfo(FUNC, "args NULL");
        return (-1);
    }
    
    if (!if_indextoname(pstList->iIfindex, acIfname))
    {
        loginfo(FUNC, "if_indextoname error");
        return (-1);
    }
    
    snprintf(acFilePath, 63, WAN_GW6_FILE, acIfname);
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

    for (pstCur = pstList->pstDftRtList; pstCur; pstCur = pstCur->pstNext)
    {
        WriteIPIntoFile(fp, pstCur->stDftRouteAddr, -1);
        
        if (pstCur->iFlg & EN_NEW)
        {
            pstCur->iFlg &= ~EN_NEW;
        }
        if (pstCur->iFlg & EN_MODIFY)
        {
            pstCur->iFlg &= ~EN_MODIFY;
        }
        pstCur->iFlg |= EN_PROCESSED;
        pstCur->iFlg |= EN_SEND;
    }

    fclose(fp);

    return (0);
}

static int FlushAllRADftFile(RA_INFO_LIST_ST *pstList)
{
    FILE *fp = NULL;
    char acFilePath[64] = {0};
    char acIfname[IFNAMSIZ] = {0};

    if (!pstList)
    {
        loginfo(FUNC, "args NULL");
        return (-1);
    }
    
    if (!if_indextoname(pstList->iIfindex, acIfname))
    {
        loginfo(FUNC, "if_indextoname error");
        return (-1);
    }
    
    snprintf(acFilePath, 63, WAN_IPADDR6_FILE, acIfname);
    fp = fopen(acFilePath, "w");
    if (!fp)
    {
        loginfo(FUNC, "open file [%s] error", acFilePath);
        return (-1);
    }

    fclose(fp);

    return (0);
}


/*****************************************************************************************************************/

static int WriteAllRAPrefix(RA_INFO_LIST_ST *pstList);
static int FlushAllRAPrefixFile(RA_INFO_LIST_ST *pstList);


int ProcessRAPrefixOpt(RA_INFO_LIST_ST *pstList,
            void *pvPrefixOpt, struct sockaddr_in6 stFrom)
{
    struct nd_opt_prefix_info *pstPrefix = NULL;
    RA_PREFIX_ST *pstPrefixNode = NULL; 
    int iNew = 0;
    uint32_t uiPerTime, uiValidTime;
    struct in6_addr stAddr;
    uint32_t uiLocalPrefix;

    if ((!pvPrefixOpt) || (!pstList))
    {
        loginfo(FUNC, "args NULL");
        return (-1);
    }

    pstPrefix = (struct nd_opt_prefix_info *)pvPrefixOpt;

    
    /* �����ǰ׺���������Զ���ַ���ã���ô����Ҫ���� */
    if (!(pstPrefix->nd_opt_pi_flags_reserved & ND_OPT_PI_FLAG_AUTO))
    {
        loginfo(FUNC, "prefix can not used to autoconf [%0x]", 
            pstPrefix->nd_opt_pi_flags_reserved);
        return (-1);
    }

    uiPerTime = ntohl(pstPrefix->nd_opt_pi_preferred_time);
    uiValidTime = ntohl(pstPrefix->nd_opt_pi_valid_time);

    /* ���perferred lifetime����valid lifetime����ôҲ�ǷǷ���������ǰ׺ */
    if (uiPerTime > uiValidTime)
    {
        loginfo(FUNC, "lifetime error");
        return (-1);
    }
    memcpy(&stAddr, &pstPrefix->nd_opt_pi_prefix, sizeof(struct in6_addr));

    

    /* ǰ׺�������Ҫ�� */
    #if 1
    if (IN6_IS_ADDR_LINKLOCAL(&stAddr)
        || IN6_IS_ADDR_MULTICAST(&stAddr))
    {
        loginfo(FUNC, "error: address type error");
        return (-1);
    }
    #else
    if (IPV6_ADDR_SCOPE_GLOBAL != IPv6AddrType(stAddr))
    {
        LOG_ERR("Error: address type error");
        return (-1);
    }
    #endif

    pstPrefixNode = FindPrefix(pstList,
        pstPrefix->nd_opt_pi_prefix, pstPrefix->nd_opt_pi_prefix_len);
    /* �½ڵ� */
    if (!pstPrefixNode)
    {
        if (MAX_PREFIX_NUM <= GetPrefixNum(pstList))
        {
            loginfo(FUNC, "prefix list is full");
            return (-1);
        }
        
        loginfo(FUNC, "New prefix ["NIP6_FMT"] perfer lifetime: %d  valid lifetime: %d", 
            NIP6(stAddr), uiPerTime, uiValidTime);

        /* �����½ڵ���˵���ʱ��Ϊ0����ô������ǰ׺ */
        if ((LIFETIME_TIMEOUT == uiPerTime) || (LIFETIME_TIMEOUT == uiValidTime))
        {
            loginfo(FUNC, "New node, lifetime is 0");
            return (0);
        }
        
        pstPrefixNode = (RA_PREFIX_ST *)malloc(sizeof(RA_PREFIX_ST));
        if (!pstPrefixNode)
        {
            loginfo(FUNC, "malloc error");
            return (-1);
        }
        memset(pstPrefixNode, 0, sizeof(RA_PREFIX_ST));
        pstPrefixNode->pstNext = NULL;
        iNew = 1;
        pstPrefixNode->iFlg |= EN_NEW;
    }
    else            // TODO: �ж��Ƿ�ýڵ��ʱ�䷢���˸ı䣬����ı�Ҳ��Ҫ֪ͨWAN
    {
        pstPrefixNode->iFlg |= EN_MODIFY;
    }

    /* ��ֵ */
    {
       pstPrefixNode->bAutoConf = (pstPrefix->nd_opt_pi_flags_reserved & ND_OPT_PI_FLAG_AUTO);
       pstPrefixNode->bOnLink = (pstPrefix->nd_opt_pi_flags_reserved & ND_OPT_PI_FLAG_ONLINK);
       pstPrefixNode->uiLft = uiPerTime;
       pstPrefixNode->uiVlft = uiValidTime;
       memcpy(&pstPrefixNode->stPrefix, &pstPrefix->nd_opt_pi_prefix, sizeof(struct in6_addr));
       pstPrefixNode->iPrefixLen = pstPrefix->nd_opt_pi_prefix_len;
       memcpy(&pstPrefixNode->stSrc, &stFrom, sizeof(struct sockaddr_in6));
       pstPrefixNode->lUpTime = SysUpTime();
    }

    /* ������½ڵ㣬��ô��Ҫ�������� */
    if (iNew)
    {
        pstPrefixNode->pstNext = pstList->pstPrexList;
        pstList->pstPrexList = pstPrefixNode;
        pstList->iPrefixNum += 1;
    }

    return 0;    
}

/* ɾ��wan0�ӿ��϶�Ӧǰ׺���ɵ�ipv6��ַ */
static void delete_address_by_prefix(struct in6_addr *prefix, int prefixlen)
{
    char tmpbuf[BUFFER_LENTH_64] = {0};
    char addr6[BUFFER_LENTH_64] = {0};
    char buff[BUFFER_LENTH_256] = {0};
    char ifname[BUFFER_LENTH_64] = {0};
    struct in6_addr staddr6;
    FILE* pstfile = NULL;
    int addrlen = 0;
    int i = 0;
    int j = 0;

    if (NULL == prefix)
    {
        loginfo(FUNC, "prefix is NULL");
        return;
    }
    
    pstfile = fopen("/proc/net/if_inet6", "r");
    if (NULL != pstfile)
    {
        while (fgets(buff, BUFFER_LENTH_256, pstfile))
        {
            memset(tmpbuf, 0, BUFFER_LENTH_64);
            memset(ifname, 0, BUFFER_LENTH_64);
            // coverity[secure_coding]
            sscanf(buff, "%s %s %s %s %s %s\n", tmpbuf, ifname, ifname, ifname, ifname, ifname);
            if (0 == strcmp(ifname, WANNAME))
            {
                addrlen = strlen(tmpbuf);
                /* ΪIPV6��ַ����:�ָ��� */
                for (i = 0, j = 0; i < addrlen; i++, j++)
                {
                    if ((i != 0) && (i % 4 == 0))
                    {
                        addr6[j] = ':';
                        j++;
                    }
                    addr6[j] = tmpbuf[i];
                }
                if (inet_pton(AF_INET6, addr6, &staddr6) > 0)
                {
                    /* ��ǰ׺λ�����Ȼ���Ϊ�ֽ��� */
                    addrlen = prefixlen / BYTE_LENTH;
                    if (0 == memcmp(&staddr6, prefix, addrlen))
                    {
                        /* �ҵ�wan0��ָ��ǰ׺��Ӧ��ipv6��ַ��ɾ�� */
                        memset(buff, 0, BUFFER_LENTH_256);
                        memset(addr6, 0, BUFFER_LENTH_64);
                        inet_ntop(AF_INET6, &staddr6, addr6, INET6_ADDRSTRLEN);
                        snprintf(buff, BUFFER_LENTH_256, "busybox ifconfig %s del %s/%d", WANNAME, addr6, prefixlen);
                        loginfo(FUNC, " cmd is %s ", buff);
                        // coverity[tainted_string]
                        system(buff);
                        break;                       
                    }
                }
            }
        }
        fclose(pstfile);
    }
}

int UpdateRAPrefix(RA_INFO_LIST_ST *pstList)
{
    RA_PREFIX_ST *pstCurPrefixNode = NULL;
    RA_PREFIX_ST *pstPrePrefixNode = NULL;
    RA_PREFIX_ST *pstTmpPrefixList = NULL;
    long   ulCurrTime = SysUpTime();
    int iCnt = 0;
    
    if (0 != GetPrefixNum(pstList))
    {
        pstCurPrefixNode = pstList->pstPrexList;
        pstPrePrefixNode = NULL;
        while (NULL != pstCurPrefixNode)
        {
            ulCurrTime = TimeSub(ulCurrTime, pstCurPrefixNode->lUpTime);
            /* �����������Ϊ0�����Ѿ��ϻ�����,��ô����Ҫ��������ɾ������Щ�ڵ���Ϣ */
            if ((LIFETIME_TIMEOUT == pstCurPrefixNode->uiLft)
                || ((INFINITY_LFT != pstCurPrefixNode->uiLft)
                    && (ulCurrTime >= pstCurPrefixNode->uiVlft)))
            {
                if (pstList->pstPrexList == pstCurPrefixNode)
                {
                    pstList->pstPrexList = pstCurPrefixNode->pstNext;
                    
                    if (!(pstCurPrefixNode->iFlg & EN_SEND))
                    {
                        free(pstCurPrefixNode);
                    }
                    else
                    {
                        /* ������ʱ���� */
                        pstCurPrefixNode->pstNext = pstTmpPrefixList;
                        pstTmpPrefixList = pstCurPrefixNode;
                    }
                    
                    pstCurPrefixNode = pstList->pstPrexList;
                    pstPrePrefixNode = pstCurPrefixNode;
                }
                else
                {
                   pstPrePrefixNode->pstNext = pstCurPrefixNode->pstNext;
                   if (!(pstCurPrefixNode->iFlg & EN_SEND))
                   {
                       free(pstCurPrefixNode);
                   }
                   else     /* �·�����LAN�����Ҫ֪ͨ��LAN��֪��Щ�Ѿ����� */
                   {
                        /* ������ʱ���� */
                       pstCurPrefixNode->pstNext = pstTmpPrefixList;
                       pstTmpPrefixList = pstCurPrefixNode;
                   }
                   
                   pstCurPrefixNode = pstPrePrefixNode->pstNext;
                }

                /* �ڵ��������� */
                pstList->iPrefixNum -= 1;
            }            
            else
            {
                pstPrePrefixNode = pstCurPrefixNode;
                pstCurPrefixNode = pstCurPrefixNode->pstNext;
            }
        }
    }

    /* ��������ж��ǹ��ڵ����Ҫ��֪LAN�� */
    if (pstTmpPrefixList)
    {
        iCnt = 1;
        // TODO: ��LAN��ͨѶ��֪��Щǰ׺ȫ������
    }

    /* �ͷ���ʱ���� */
    if (pstTmpPrefixList)
    {
        for (; NULL != pstTmpPrefixList;)
        {
            /* �ͷŹ��ڽڵ㲢ɾ��wan0�����ɵĶ�Ӧipv6��ַ */
            delete_address_by_prefix(&pstTmpPrefixList->stPrefix, pstTmpPrefixList->iPrefixLen);
            pstPrePrefixNode = pstTmpPrefixList;
            pstTmpPrefixList = pstTmpPrefixList->pstNext;
            free(pstPrePrefixNode);
            
        }
    }
    
    /* �����Щ�ڵ�����ˣ���ô��Ҫˢ�£��Ӷ���֪��Ч�� */
    /* �ȼ����û���½ڵ� */
    for (pstCurPrefixNode = pstList->pstPrexList; 
                pstCurPrefixNode; pstCurPrefixNode = pstCurPrefixNode->pstNext)
    {
        // TODO: ����ڵ㲻���½ڵ㵫��ʱ��ı���Ҳ��Ҫ֪ͨ
        if (pstCurPrefixNode->iFlg & EN_NEW)
        {
            iCnt = 1;
            break;
        }
    }
                
    // TODO: ��ʣ���û���·���LAN�Ĳ����·���LAN
    /* �ߵ��������м��ֿ���: 1 �нڵ�����ˣ�2 �����˽ڵ㣬����1��2������ */
    if (iCnt)
    {
        WriteAllRAPrefix(pstList);
        // TODO: ֪ͨLAN��
        return (1);
    }

    return (0);
}


/* ������е�RA��Ϣ������pstList������������ */
int ClearRAPrefix(RA_INFO_LIST_ST *pstList)
{
    RA_PREFIX_ST *p = NULL;

    if (!pstList)
    {
        loginfo(FUNC, "list is NULL");
        return (0);
    }

#if 0
    p = pstList->pstPrexList;
    for (; p; p = p->pstNext)
    {
        if (p->iFlg & EN_SEND)
        {
            // TODO: ֪ͨLAN�࣬���߷���Ϣ
        }
    }
#endif

    FlushAllRAPrefixFile(pstList);
    // TODO: ֪ͨLAN�࣬���߷���Ϣ


    p = pstList->pstPrexList;
    while (pstList->pstPrexList)
    {
        p = pstList->pstPrexList;
        pstList->pstPrexList = pstList->pstPrexList->pstNext;
        free(p);
    }

    pstList->pstPrexList = NULL;

    return 0;
}

static int WriteAllRAPrefix(RA_INFO_LIST_ST *pstList)
{
    FILE *fp = NULL;
    char acFilePath[64] = {0};
    char acIfname[IFNAMSIZ] = {0};
    RA_PREFIX_ST *pstCur = NULL;

    if (!pstList)
    {
        loginfo(FUNC, "args NULL");
        return (-1);
    }
    
    if (!if_indextoname(pstList->iIfindex, acIfname))
    {
        loginfo(FUNC, "if_indextoname error");
        return (-1);
    }
    
    snprintf(acFilePath, 63, WAN_PREFIX_FILE, acIfname);
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

    for (pstCur = pstList->pstPrexList; 
            pstCur; pstCur = pstCur->pstNext)
    {
        /*��ra prefix��onlink �����ļ�, ��cms��ȡ, ����wan��ԴIP·�������raǰ׺����Ŀ*/
        WriteIPLinkFlagIntoFile(fp, pstCur->stPrefix, pstCur->iPrefixLen, pstCur->bOnLink);
        
        if (pstCur->iFlg & EN_NEW)
        {
            pstCur->iFlg &= ~EN_NEW;
        }
        if (pstCur->iFlg & EN_MODIFY)
        {
            pstCur->iFlg &= ~EN_MODIFY;
        }
        pstCur->iFlg |= EN_PROCESSED;
        pstCur->iFlg |= EN_SEND;
    }

    fclose(fp);

    return (0);
}

static int FlushAllRAPrefixFile(RA_INFO_LIST_ST *pstList)
{
    FILE *fp = NULL;
    char acFilePath[64] = {0};
    char acIfname[IFNAMSIZ] = {0};

    if (!pstList)
    {
        loginfo(FUNC, "args NULL");
        return (-1);
    }
    
    if (!if_indextoname(pstList->iIfindex, acIfname))
    {
        loginfo(FUNC, "if_indextoname error");
        return (-1);
    }
    
    snprintf(acFilePath, 63, WAN_PREFIX_FILE, acIfname);
    fp = fopen(acFilePath, "w");
    if (!fp)
    {
        loginfo(FUNC, "open file [%s] error", acFilePath);
        return (-1);
    }

    fclose(fp);

    return (0);
}


/*****************************************************************************************************************/

static int WriteAllRADNS(RA_INFO_LIST_ST *pstList);
static int FlushAllRDNSFile(RA_INFO_LIST_ST *pstList);


// TODO: ע�⣬rfc5006��˵������ΪDNS����ʱ�����Ƶģ��������������������RS����Ӷ�����RAʹ��DNS�������
// TODO: Ŀǰ�ݲ�֧��
// TODO: ���DNS���ϻ�ʱ��ı���Ҳ��Ҫ֪ͨWAN
int ProcessRADNS(RA_INFO_LIST_ST *pstList,
            void *pvPrefixOpt, struct sockaddr_in6 stFrom)
{
    struct nd_opt_dns_info *pstDNSOptHdr = (struct nd_opt_dns_info *)pvPrefixOpt;
    RA_DNS_INFO_ST *pstDNSInfo = NULL; 
    RA_DNS_INFO_ST *pstPrev = NULL; 
    struct in6_addr stAddr;
    uint32_t    uiTime = 0;
    unsigned char  *p = NULL;
    int i = 0;
    int iNew = 0;

    if ((!pvPrefixOpt) || (!pstList))
    {
        loginfo(FUNC, "args NULL");
        return (-1);
    }

    /* ��С����Ϊ3 */
    if (RA_RDDNS_LEN_MIN > pstDNSOptHdr->nd_opt_dns_len)
    {
        loginfo(FUNC, "error dns option len must longger than 3");
        return (-1);
    }

    uiTime = ntohl(pstDNSOptHdr->nd_opt_dns_lifetime);


    p = (unsigned char *)(pstDNSOptHdr + 1);
    /* ֮������Ҫ��ȥ1����Ϊ��Ҫ��ȥoptionͷ���Ĵ�СΪ1 */
    for (i = 0; i < (pstDNSOptHdr->nd_opt_dns_len - 1); )
    {
        memcpy(&stAddr, p, sizeof(struct in6_addr));
        pstDNSInfo = FindDNS(pstList, stAddr);
        if (!pstDNSInfo)
        {
            /* ���ʱ��Ϊ0���������½ڵ���ô���ܸõ�ַ�����ܲ�ͬ��RAͨ���˲�ͬ��DNS */
            if (LIFETIME_TIMEOUT == uiTime)
            {
                loginfo(FUNC, "new node, lifetime is 0");
                continue;
            }

            /* �ڵ��������ܳ������ֵ */
            if (MAX_RDDNS_NUM <= GetDNSNum(pstList))
            {
                i += 2;
                p += (2 * IPV6_ADDR_LEN);
                continue;
            }
                        
            pstDNSInfo = (RA_DNS_INFO_ST *)malloc(sizeof(RA_DNS_INFO_ST));
            if (!pstDNSInfo)
            {
                loginfo(FUNC, "malloc error");
                return (-1);
            }
            memset(pstDNSInfo, 0, sizeof(RA_DNS_INFO_ST));            
            pstDNSInfo->iFlg |= EN_NEW;            
            iNew = 1;
        }
        else
        {            
            pstDNSInfo->iFlg |= EN_MODIFY;
        }

        /* �������� */
        {
            memcpy(&pstDNSInfo->stDnsAddr, &stAddr, sizeof(struct in6_addr));
            pstDNSInfo->lUpTime = SysUpTime();
            pstDNSInfo->uiLft = uiTime;
            memcpy(&pstDNSInfo->stSrc, &stFrom, sizeof(struct sockaddr_in6));
        }

        if (iNew)
        {
#if 0
            pstDNSInfo->pstNext = pstList->pstDNSList;
            pstList->pstDNSList = pstDNSInfo;
#else
            pstPrev = pstList->pstDNSList;
            if(pstPrev != NULL)
            {
                for (pstPrev = pstList->pstDNSList; NULL != pstPrev->pstNext;
                    pstPrev = pstPrev->pstNext)
                {
                    continue;
                }
            }
            if (NULL == pstPrev)
            {
                pstDNSInfo->pstNext = pstList->pstDNSList;
                pstList->pstDNSList = pstDNSInfo;
            }
            else
            {
                pstPrev->pstNext = pstDNSInfo;
                pstDNSInfo->pstNext = NULL;
            }
#endif
            pstList->iDnsNum += 1;
        }

        i += 2;
        p += IPV6_ADDR_LEN;
    }

    return (0);
}


int UpdateDNSInfo(RA_INFO_LIST_ST *pstList)
{
    RA_DNS_INFO_ST *pstCurDNSNode = NULL;
    RA_DNS_INFO_ST *pstPreDNSNode = NULL;
    RA_DNS_INFO_ST *pstTmpDNSList = NULL;
    long   ulCurrTime = SysUpTime();
    int iCnt = 0;
    
    if (0 != GetDNSNum(pstList))
    {
        pstCurDNSNode = pstList->pstDNSList;
        pstPreDNSNode = NULL;
        while (NULL != pstCurDNSNode)
        {
            ulCurrTime = TimeSub(ulCurrTime, pstCurDNSNode->lUpTime);
            /* �����������Ϊ0�����Ѿ��ϻ�����,��ô����Ҫ��������ɾ������Щ�ڵ���Ϣ */
            if ((LIFETIME_TIMEOUT == pstCurDNSNode->uiLft)
                || ((INFINITY_LFT != pstCurDNSNode->uiLft)
                    && (ulCurrTime >= pstCurDNSNode->uiLft)))
            {
                if (pstList->pstDNSList == pstCurDNSNode)
                {
                    pstList->pstDNSList = pstCurDNSNode->pstNext;
                    
                    if (!(pstCurDNSNode->iFlg & EN_SEND))
                    {
                        free(pstCurDNSNode);
                    }
                    else            /* ��Щ��Ϣ�����͸���LAN����߸���DNS����ô�����������Ҫ֪ͨLAN�����DNSɾ����Щ��Ϣ */
                    {
                        if (NULL != pstPreDNSNode)
                        {
                        /* ������ʱ���� */
                        pstPreDNSNode->pstNext = pstTmpDNSList;
                        pstTmpDNSList = pstCurDNSNode;
                        }
                    }
                    
                    pstCurDNSNode = pstList->pstDNSList;
                    pstPreDNSNode = pstCurDNSNode;
                }
                else
                {
                    if (NULL != pstPreDNSNode)
                {
                   pstPreDNSNode->pstNext = pstCurDNSNode->pstNext;
                   if (!(pstCurDNSNode->iFlg & EN_SEND))
                   {
                       free(pstCurDNSNode);
                   }
                   else     /* �·�����LAN,�����Ҫ֪ͨLAN��Щ�Ѿ����� */
                   {
                       /* ������ʱ���� */
                       pstCurDNSNode->pstNext = pstTmpDNSList;
                       pstTmpDNSList = pstCurDNSNode;
                   }
                   pstCurDNSNode = pstPreDNSNode->pstNext;
                    }
                }

                /* �ڵ��������� */
                pstList->iDnsNum -= 1;
            }            
            else
            {
                pstPreDNSNode = pstCurDNSNode;
                pstCurDNSNode = pstCurDNSNode->pstNext;
            }
        }
    }

    /* ��������ж��ǹ��ڵ����Ҫ��֪LAN�� */
    if (pstTmpDNSList)
    {
        iCnt = 1;
        // TODO: ��LAN��ͨѶ��֪��Щǰ׺ȫ������
    }
    
    /* �ͷ���ʱ���� */
    if (pstTmpDNSList)
    {
        for (; NULL != pstTmpDNSList;)
        {
            pstPreDNSNode = pstTmpDNSList;
            pstTmpDNSList = pstTmpDNSList->pstNext;
            free(pstPreDNSNode);
        }
    }

/* �����Щ�ڵ�����ˣ���ô��Ҫˢ�£��Ӷ���֪��Ч�� */
/* �ȼ����û���½ڵ� */
    if (0 != GetDNSNum(pstList))
    {
        pstCurDNSNode = pstList->pstDNSList;
        pstPreDNSNode = NULL;
        while (NULL != pstCurDNSNode)
        {
            // TODO: ����ڵ㲻���½ڵ㵫��ʱ��ı���Ҳ��Ҫ֪ͨ
            if (pstCurDNSNode->iFlg & EN_NEW)
            {
                iCnt = 1;
                break;
            }
            pstPreDNSNode = pstCurDNSNode;
            pstCurDNSNode = pstCurDNSNode->pstNext;
        }
    }

    // TODO: ��ʣ���û���·���LAN�Ĳ����·���LAN
    if (iCnt)
    {
        WriteAllRADNS(pstList);
        return (1);
    }

    return (0);
}


/* ������е�DNS��Ϣ */
int ClearDNSInfo(RA_INFO_LIST_ST *pstList)
{
    RA_DNS_INFO_ST *p = NULL;

    if (!pstList)
    {
        loginfo(FUNC, "dns list is NULL");
        return (0);
    }

#if 0
    for (; p; p = p->pstNext)
    {
        if (p->iFlg & EN_SEND)
        {
            // TODO: ֪ͨLAN�࣬���߷���Ϣ
        }
    }
#endif
 
    p = pstList->pstDNSList;
    while (pstList->pstDNSList)
    {
        p = pstList->pstDNSList;
        pstList->pstDNSList = pstList->pstDNSList->pstNext;
        free(p);
    }

    pstList->pstDNSList = NULL;

    return 0;
}

static int WriteAllRADNS(RA_INFO_LIST_ST *pstList)
{
    FILE *fp = NULL;
    char acFilePath[64] = {0};
    char acIfname[IFNAMSIZ] = {0};
    RA_DNS_INFO_ST *pstCur = NULL;

    FUNCBEGIN();

    if (!pstList)
    {
        loginfo(FUNC, "args NULL");
        return (-1);
    }
    
    if (!if_indextoname(pstList->iIfindex, acIfname))
    {
        loginfo(FUNC, "if_indextoname error");
        return (-1);
    }
    
    snprintf(acFilePath, 63, WAN_DNS6_FILE, acIfname);
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

    for (pstCur = pstList->pstDNSList; 
            pstCur; pstCur = pstCur->pstNext)
    {
        WriteIPIntoFile(fp, pstCur->stDnsAddr, -1);
        
        if (pstCur->iFlg & EN_NEW)
        {
            pstCur->iFlg &= ~EN_NEW;
        }
        if (pstCur->iFlg & EN_MODIFY)
        {
            pstCur->iFlg &= ~EN_MODIFY;
        }
        
        pstCur->iFlg |= EN_PROCESSED;
        pstCur->iFlg |= EN_SEND;
    }

    fclose(fp);

    FUNCEND();

    return (0);
}

static int FlushAllRDNSFile(RA_INFO_LIST_ST *pstList)
{
    FILE *fp = NULL;
    char acFilePath[64] = {0};
    char acIfname[IFNAMSIZ] = {0};

    FUNCBEGIN();

    if (!pstList)
    {
        loginfo(FUNC, "args NULL");
        return (-1);
    }
    
    if (!if_indextoname(pstList->iIfindex, acIfname))
    {
        loginfo(FUNC, "if_indextoname error");
        return (-1);
    }
    
    snprintf(acFilePath, 63, WAN_DNS6_FILE, acIfname);
    fp = fopen(acFilePath, "w");
    if (!fp)
    {
        loginfo(FUNC, "open file [%s] error", acFilePath);
        return (-1);
    }

    fclose(fp);

    FUNCEND();

    return (0);
}

static int ProcessRaMtu(RA_INFO_LIST_ST *pstList, void *pvPrefixOpt)
{
    struct nd_opt_mtu *pstRaMtu = pvPrefixOpt;
    FILE *fp = NULL;
    char acIfname[IFNAMSIZ] = {0};

    if (!pstList || !pvPrefixOpt)
    {
        loginfo(FUNC, "NULL parameter");
        return -1;
    }
    
    pstList->iMtu = ntohl(pstRaMtu->nd_opt_mtu_mtu);
    loginfo(FUNC, "RA options contains MTU=%d", pstList->iMtu);

    if (!if_indextoname(pstList->iIfindex, acIfname))
    {
        loginfo(FUNC, "if_indextoname error");
        return (-1);
    }
    
    SetFileValue(WAN_RA_MTU_FILE, acIfname, pstList->iMtu);
    return 0;
}

static int ProcessRaMOFlags(RA_INFO_LIST_ST *pstList)
{
    FILE *fp = NULL;
    char acIfname[IFNAMSIZ] = {0};

    if (!pstList)
    {
        loginfo(FUNC, "NULL parameter");
        return -1;
    }
    
    loginfo(FUNC, "RA options contains M=%d, O=%d", pstList->iManaged, pstList->iOtherInfo);

    if (!if_indextoname(pstList->iIfindex, acIfname))
    {
        loginfo(FUNC, "if_indextoname error");
        return (-1);
    }
    
    SetFileValue(WAN_RA_M_FLAG_FILE, acIfname, pstList->iManaged==ND_RA_FLAG_MANAGED);
    SetFileValue(WAN_RA_O_FLAG_FILE, acIfname, pstList->iOtherInfo==ND_RA_FLAG_OTHER);
    return 0;
}

/*****************************************************************************************************************/


int ProcessRAOption(ND_OPT_FMT_LINK_ST *pstOptList, RA_INFO_LIST_ST *pstList)
{
    static  int         iInitFlg = 0;
    ND_OPT_FMT_ST       *pstTLV = NULL;
    int                 err = 1;    
    static int iOldMflg, iOldOflg;

    FUNCBEGIN();
    
    if ((!pstOptList) || (!pstList))
    {
        loginfo(FUNC, "args NULL");
        return (-1);
    }

    if (!IN6_IS_ADDR_LINKLOCAL(&pstOptList->stFrom.sin6_addr))
    {
        loginfo(FUNC, "RA doesn't come from link-local address");
        return (-1);
    }

    pstTLV = pstOptList->pstOpt;
    if (!pstTLV)
    {
        loginfo(FUNC, "no option for this message");
        return (0);
    }

    ProcessRADefaultRoute(pstList, &pstOptList->stNDRAHdr,  pstOptList->stFrom);

    /* ��������option 
       * ע��: ���ȫ���������ˣ���ô��������¾Ͳ���������������˶�ֻ������¡�
       */
    for (; NULL != pstTLV; pstTLV = pstTLV->pstNext)
    {        
        switch(pstTLV->ucType)
        {
            case ND_OPT_PREFIX_INFORMATION:
                ProcessRAPrefixOpt(pstList, (void *)(pstTLV->stTLVData.pucData), pstOptList->stFrom);
                break;
            case ND_OPT_RDNSS:
                ProcessRADNS(pstList, (void *)(pstTLV->stTLVData.pucData), pstOptList->stFrom);
                break;
            case ND_OPT_SOURCE_LINKADDR:
                break;
            case ND_OPT_MTU:
                ProcessRaMtu(pstList, (void *)(pstTLV->stTLVData.pucData));
                break;
            case ND_OPT_RIO:
            default:
                continue;
        }
    }

    /* ������һ�ε�״̬ */
    iOldMflg = pstList->iManaged;
    iOldOflg = pstList->iOtherInfo;
    
    /* ����M,O��״̬ */
    pstList->iManaged = (pstOptList->stNDRAHdr.nd_ra_flags_reserved 
                                & ND_RA_FLAG_MANAGED );    
    pstList->iOtherInfo = (pstOptList->stNDRAHdr.nd_ra_flags_reserved 
                                    & ND_RA_FLAG_OTHER);

    /*��¼��ǰM, O��ǵ��ļ�����cms�ȶ�ȡ*/
    ProcessRaMOFlags(pstList);
    
    if (iOldMflg != pstList->iManaged)
    {
        pstList->iMOChg |= EN_M_CHANGED;
    }
    if (iOldOflg != pstList->iOtherInfo)
    {
        pstList->iMOChg |= EN_O_CHANGED;
    }

    FUNCEND();

    return (0);
}




/* ע�⣬����DHCP����M����O��־��ʼȡ�����յ��ĵ�һ��RA */
int UpdateStatus(INTERFACE_RA_ST *pstList)
{
    int iCon = 0;
    
    FUNCBEGIN();
    /*BEGIN 2032203980  m00182044 2012-06-10 modify*/

    /*���û�յ�ra, ������dhcp����*/
    if (!(pstList->iFlags & en_ra_rcv))
    {
        return 0;
    }

    if ((!(pstList->stRAData.iDhcpStatus & enMDHCP))
        && (!(pstList->stRAData.iDhcpStatus & enODHCP)))
    {
    /*�Ѿ�����M,O����ˣ���������;
    ��һ��λ���ú������ٺ������ͷ���Դ
    Ҳ�ú����ٽ�����������������ظ�ȥ����StartDhcp6c*/
    pstList->stRAData.iDhcpStatus |= enMDHCP;
    iCon = 1;
    #if 0
        if(pstList->stRAData.iManaged)
        {
            pstList->stRAData.iDhcpStatus |= enMDHCP;
            iCon = 1;
        }
        else if (pstList->stRAData.iOtherInfo)
        {
            pstList->stRAData.iDhcpStatus |= enODHCP;
            iCon = 1;
        }
    #endif
    
        loginfo(FUNC, "notice: dhcp status:��[%0x]",
            pstList->stRAData.iDhcpStatus);
    }

    // TODO: ����DHCP����
        loginfo(FUNC, "now, start dhcp client iCon:%d.......",iCon);

/* start of AU4D02274 by d00107688 2009-12-02 PPPoe+SLAAC��ʽ�޷���ȡDNS��Ϣ */

/*����ͨME60�ԽӸĶ�:
��ʹWAN��������IANA ��IAPD������
����Ҫ��RA��M���λ, ��M=0��ֻ����IAPD, 
��M=1, ������IANA + IAPD
Ҳ���ٿ�O��ǣ�����ȥ����DNS
*/
    if (iCon)
    {
	loginfo(FUNC, "pstList->stRAData.iManaged:%d.......",pstList->stRAData.iManaged);
    /*END 2032203980  m00182044 2012-06-10 modify*/
    /* BEGIN: Added by z67728, 2011/4/20 ���������޸�. */
#ifdef SUPPORT_ATP_IPV6_ALWAYS_SEND_RS
        if ( g_iEnableRA )
        {
            if (pstList->stRAData.iManaged)
            {
                StartDhcp6c(pstList->iIfindex, 
                    pstList->iIANAFlag, pstList->iIAPDFlag, pstList->iDsliteFlag);
            }
            else
            {
                StartDhcp6c(pstList->iIfindex, 0, pstList->iIAPDFlag, pstList->iDsliteFlag);
            }
        }
#else
        if (pstList->stRAData.iManaged)
        {
            StartDhcp6c(pstList->iIfindex, 
                pstList->iIANAFlag, pstList->iIAPDFlag, pstList->iDsliteFlag);
        }
        else if(pstList->stRAData.iOtherInfo)
        {
            StartDhcp6c(pstList->iIfindex, 0, 0, pstList->iDsliteFlag);
        }
        else
        {
            loginfo(FUNC, "Don't start dhcp6c!!");
        }
#endif
    /* END:   Added by z67728, 2011/4/20 */
    }
/* start of AU4D02274 by d00107688 2009-12-02 PPPoe+SLAAC��ʽ�޷���ȡDNS��Ϣ */

    FUNCEND();

    return (0);
}


int UpdateByIfindex(int iIfindex)
{
    INTERFACE_RA_ST *p = NULL;

    FUNCBEGIN();
    
    p = FindInterfaceByIfindex(iIfindex);
    if (p)
    {
        UpdateDftRt(&p->stRAData);
        UpdateRAPrefix(&p->stRAData);
        UpdateDNSInfo(&p->stRAData);
        //UpdateStatus(p);
    }

    FUNCEND();
    
    return (0);
}

/* ������Ϣ���� */
int UpdateRAInfo(INTERFACE_RA_ST *pstList)
{
    int iNum = 0;

    FUNCBEGIN();
    
    iNum += UpdateDftRt(&pstList->stRAData);
    
    //if (g_stRAinfoList.iAssignPrefix)
    {   
        iNum += UpdateRAPrefix(&pstList->stRAData);
    }
    
    //if (g_stRAinfoList.iAssignDNS)
    {
        iNum += UpdateDNSInfo(&pstList->stRAData);
    }
    
    UpdateStatus(pstList);

    // TODO: ���WANû��UP����ô��Ҫ����
    if (iNum)
    {
        NotifyWANStatus(pstList->iIfindex);
        BSP_SYS_WakeupMonitorTask();
    }

    FUNCEND();

    return (0);
}


