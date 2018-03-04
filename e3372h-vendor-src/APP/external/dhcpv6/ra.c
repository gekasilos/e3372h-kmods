#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/select.h>

#include "ra.h"
#include "ratlv.h"
#include "sockutils.h"
#include "misc.h"
#include "raopt.h"

/*
  <���ⵥ��>     <��  ��>    <�޸�ʱ��>   <�汾> <�޸�����>
    2032203980  m00182044   2012/6/10 DHCP6c����ͨ��RA������״̬IPv6��ַ���� 
 */


INTERFACE_RA_ST *g_pstIfList = NULL;

int InitInterface(INTERFACE_RA_ST **p)
{
    if (!(*p))
    {
        *p = malloc(sizeof(INTERFACE_RA_ST));
        if (!(*p))
        {
            loginfo(FUNC, "malloc error");
            return (-1);
        }
    }
    
    memset(*p, 0, sizeof(INTERFACE_RA_ST));

    return (0);
}


INTERFACE_RA_ST *FindInterfaceByIfindex(int iIfindex)
{
    INTERFACE_RA_ST *p = NULL;

    FUNCBEGIN();
    
    if (!g_pstIfList)
    {
        loginfo(FUNC, "all list is NULL");
        return (NULL);
    }

    p = g_pstIfList;
    for (; p; p = p->pstNext)
    {
        if (iIfindex == p->iIfindex)
        {
            return (p);
        }
    }

    FUNCEND();

    return (NULL);
}


int AddInterfaceIntoList(INTERFACE_RA_ST **pp)
{
    if (!(*pp))
    {
        loginfo(FUNC, "args NULL");
        return (-1);
    }

    (*pp)->pstNext = g_pstIfList;
    g_pstIfList = *pp;

    return (0);
}

INTERFACE_RA_ST *ReomveInterfaceFromListByIdx(int iIfindex)
{
    INTERFACE_RA_ST *pstCur, *pstPrev;

    FUNCBEGIN();

    pstPrev = NULL;
    for (pstCur = g_pstIfList; pstCur; 
        pstPrev = pstCur, pstCur = pstCur->pstNext)
    {
        if (iIfindex == pstCur->iIfindex)
        {
            if (NULL == pstPrev)
            {
                g_pstIfList = pstCur->pstNext;
            }
            else
            {
                pstPrev->pstNext = pstCur->pstNext;
            }
            
            FUNCEND();
            return (pstCur);
        }
    }

    FUNCEND();

    return (NULL);
}


int DelInterfaceByIndex(int iIfIndex)
{
    INTERFACE_RA_ST *p = NULL;

    FUNCBEGIN();

    p = ReomveInterfaceFromListByIdx(iIfIndex);
    if (!p)
    {
        loginfo(FUNC, "Error: can not found the index [%d]", iIfIndex);
        return (-1);
    }

    if ((p->stRAData.iDhcpStatus& enMDHCP)
        || (p->stRAData.iDhcpStatus & enODHCP))
    {
        loginfo(FUNC, "now, begin to stop dhcp6c....");
        StopDhcp6c(p->iIfindex, 0);
        p->stRAData.iDhcpStatus = enNothing;
    }

    ClearDftRoute(&p->stRAData);
    ClearRAPrefix(&p->stRAData);
    ClearDNSInfo(&p->stRAData);

    if (0 <= p->iSockfd)
    {
        close(p->iSockfd);
    }

    free(p);

    FUNCEND();

    return (0);
}


int ReInitInterface(int iIfIndex)
{
    INTERFACE_RA_ST *p = NULL;

    FUNCBEGIN();
    
    p = FindInterfaceByIfindex(iIfIndex);
    if (!p)
    {
        loginfo(FUNC, "args NULL");
        return (-1);
    }

    ClearDNSInfo(&p->stRAData);
    ClearRAPrefix(&p->stRAData);
    ClearDftRoute(&p->stRAData);

    //p->iState = 

    loginfo(FUNC, "begin to stop dhcp6c......");
    
    if ((p->stRAData.iDhcpStatus & enMDHCP)
        || (p->stRAData.iDhcpStatus & enODHCP))
    {
        loginfo(FUNC, "now, begin to stop dhcp6c........");
        StopDhcp6c(p->iIfindex, 0);
    }
    
    loginfo(FUNC, "over................");

    /* ����ͳ����Ϣ��ʼ�� */
    p->iFlags = 0;
    p->iRSNum = 0;
    p->stRAData.iDftRtNum = 0;
    p->stRAData.iDnsNum = 0;
    p->stRAData.iPrefixNum = 0;
    p->stRAData.iManaged = 0;
    p->stRAData.iOtherInfo = 0;
    p->stRAData.iMOChg = 0;
    p->stRAData.iDhcpStatus = enNothing;
    p->stRAData.pstDftRtList = NULL;
    p->stRAData.pstDNSList = NULL;
    p->stRAData.pstPrexList = NULL;

    FUNCEND();

    return (0);
}

int StopRunningRA(int iIfindex)
{
    char acIfname[IFNAMSIZ] = {0};
    INTERFACE_RA_ST *p = NULL;

    FUNCBEGIN();

    loginfo(FUNC, "now, stop the running ra....");

    /* ��ֹͣ�ں˵�RA���� */
    if (NULL != if_indextoname(iIfindex, acIfname))
    {
        loginfo(FUNC, "Now, begin to stop ra config......\r\n\r\n\r\n\r\n");
        SetAcceptRA(acIfname, 0);        
        SetAutoConf(acIfname, 0);
    }
    
    p = FindInterfaceByIfindex(iIfindex);
    if (!p)
    {
        loginfo(FUNC, "args NULL");
        return (-1);
    }

    /* start of by d00107688 2010-01-26 ����֧���Ƿ��������Ĭ��·�� */
    if (!p->iDftRt)
    {
        SetAcceptDftRoute(acIfname, 1);
    }
    /* end of by d00107688 2010-01-26 ����֧���Ƿ��������Ĭ��·�� */

    
    loginfo(FUNC, "notice: dhcp status:��[%0x]",
        p->stRAData.iDhcpStatus);

    /* ����ӿ� */
    ReInitInterface(iIfindex);

    FUNCEND();

    return (0);
}


int StartRunningRA(int iIfindex)
{
    char acIfname[IFNAMSIZ] = {0};
    INTERFACE_RA_ST *p;

    FUNCBEGIN();

    /* Ӧ���ȳ�ʼ��Ȼ��������������Ϊ1, ������Ա��Ƴ� */    
   // ReInitInterface(iIfindex);
    
    p = FindInterfaceByIfindex(iIfindex);
    if (!p)
    {
        loginfo(FUNC, "args NULL");
        return (-1);
    }

    if (NULL != if_indextoname(iIfindex, acIfname))
    {
        loginfo(FUNC, "Now, begin to start ra config......\r\n\r\n\r\n\r\n");
        SetAcceptRA(acIfname, 1);
        p->iFlags |= en_st_start;

        /* start of by d00107688 2010-01-26 ����֧���Ƿ��������Ĭ��·�� */
        if (p->iDftRt)
        {
            SetAcceptDftRoute(acIfname, 1);
        }
        else
        {
            SetAcceptDftRoute(acIfname, 0);
        }
        /* end of by d00107688 2010-01-26 ����֧���Ƿ��������Ĭ��·�� */
        
        SetAutoConf(acIfname, 1);
    }

    loginfo(FUNC, "start reinit interface [%d]", iIfindex);

#if 0
    /* �Է�ֹ��start֮ǰ�Ѿ������ˣ�������³�ʼ��һ�� */
    ReInitInterface(iIfindex);
    p->iFlags |= en_st_start;
#endif

    FUNCEND();

    return (0);
}


void *RAMainLoop(void *arg)
{
    int     err = 0;
    int     iRet = 0;
    fd_set   rdset;
    ND_OPT_FMT_LINK_ST *pstMsg = NULL;
    unsigned char buff[MAX_BUFFER];
    int iRcvLen = -1;
    struct sockaddr_in6 stFrom;
    INTERFACE_RA_ST *p = (INTERFACE_RA_ST *)arg;
    struct timeval t;
/* start of AU4D02338 by d00107688 2010-01-21 ��������ΪSLAAC��ʽ��ȡIPv6�ĵ�ַ������������RS��δ�յ�RA��������Ҳ������RS���� */
    static long lTimeLft;          /* selectʣ���ʱ�� */
    long lTime;

    lTimeLft = SysUpTime();
/* end of AU4D02338 by d00107688 2010-01-21 ��������ΪSLAAC��ʽ��ȡIPv6�ĵ�ַ������������RS��δ�յ�RA��������Ҳ������RS���� */

	int len;
    
	loginfo(FUNC, "begin to loop......");

    fprintf(stderr, "RAMainLoop pid: %d\r\n\r\n\r\n", getpid());
    
    sleep(1);
 
    while (!exit_ok)
    {
        iRet = 0;
        if (p->iFlags & en_st_start)
        {
            iRet = 1;
        }

        if (!iRet)
        {
            loginfo(FUNC, "ra will not eff....... %0x\r\n\r\n", p->iFlags & en_st_start ? 1:0);
        }
        
        /* ����Ҫ��Ҫ����RS��Ϣ */
       
/* start of AU4D02338 by d00107688 2010-01-21 ��������ΪSLAAC��ʽ��ȡIPv6�ĵ�ַ������������RS��δ�յ�RA��������Ҳ������RS���� */	    
#if 0
        if ( iRet
             && (MAX_RS_SEND_NUM > p->iRSNum)
             && (!(en_ra_rcv & p->iFlags)))
#else
        if (iRet && (!(en_ra_rcv & p->iFlags)))
#endif
        {
            len = SendRSPacket(p->iSockfd, p->iIfindex);
            /*BEGIN 2032203980  m00182044 2012-06-10 modify*/
			loginfo(FUNC, "send rs len = %d \n", len);
            /*END 2032203980  m00182044 2012-06-10 modify*/
            
            if (RS_NUM_MAX_VALUE < p->iRSNum)
            {
                p->iRSNum = 1;
            }

            if (0 == p->iRSNum)
            {
                t.tv_sec = 0;
            }
            else
            {
                t.tv_sec = (1 << p->iRSNum);
            }
            t.tv_usec = 0;

            p->iRSNum++;

            loginfo(FUNC, "the %d times to send, time: %0x\r\n", p->iRSNum, t.tv_sec);
        }
        else
        {
            t.tv_sec = PREFIX_UPDATE_TIME;
            t.tv_usec = 0;
        }

        if (!iRet)
        {
            t.tv_sec = PREFIX_UPDATE_TIME;
        }
/* end of AU4D02338 by d00107688 2010-01-21 ��������ΪSLAAC��ʽ��ȡIPv6�ĵ�ַ������������RS��δ�յ�RA��������Ҳ������RS���� */		
        
        FD_ZERO(&rdset);
        FD_SET(p->iSockfd, &rdset);
        err = select((p->iSockfd + 1), &rdset, NULL, NULL, &t);
        loginfo(FUNC, "select return err %d", err);
        if (0 > err)
        {
            loginfo(FUNC, "select error %s", strerror(errno));
            if (EINTR == err)
            {
                loginfo(FUNC, "because interrupt........");
            }
            continue;
        }
        else if (FD_ISSET(p->iSockfd, &rdset))
        {
            iRcvLen = RecvRAPacket(p->iSockfd, &stFrom, p->iIfindex,  
                                    buff, MAX_BUFFER, iRet);
            if ((!iRet) || (0 >= iRcvLen))
            {
                loginfo(FUNC, "receive msg error");
                continue;
            }
            
            pstMsg = BuildTLVList((void *)buff, stFrom, iRcvLen);
            if (!pstMsg)
            {
                loginfo(FUNC, "Build TLV list error");
                continue;
            }

#if 0
            DumpWholeTLVList(pstMsg);
#endif
            ProcessRAOption(pstMsg, &p->stRAData);  
            FreeTLVList(&pstMsg);
            UpdateRAInfo(p);

            p->iFlags |= en_ra_rcv;
        }

/* start of AU4D02338 by d00107688 2010-01-21 ��������ΪSLAAC��ʽ��ȡIPv6�ĵ�ַ������������RS��δ�յ�RA��������Ҳ������RS���� */
        if (iRet)
        {
            lTime = SysUpTime();
            
            /* ���lTime < lTimeLft ��ô��ʾ����� */
            if ((lTime < lTimeLft) 
                || (PREFIX_UPDATE_TIME <= (lTime - lTimeLft)))
            {
                UpdateRAInfo(p); 
                lTimeLft = SysUpTime();
            }
        }
/* end of AU4D02338 by d00107688 2010-01-21 ��������ΪSLAAC��ʽ��ȡIPv6�ĵ�ַ������������RS��δ�յ�RA��������Ҳ������RS���� */
    }

    DelInterfaceByIndex(p->iIfindex);

    pthread_exit(0);

    return (0);
}

int StartRAPthread(char *pszIfName, int iIANAFlag, int iIAPDFlag, int iDsliteFlag, int iDftRT)
{
    int     iIfx = -1;
    int     iSockfd = -1;
    INTERFACE_RA_ST *p = NULL;

    if (!pszIfName)
    {
        loginfo(FUNC, "args NULL");
        return (-1);
    }
    
    iIfx = if_nametoindex(pszIfName);
    if (0 >= iIfx)
    {
        loginfo(FUNC, "if_nametoindex error %s", strerror(errno));
        return (-1);
    }
    p = FindInterfaceByIfindex(iIfx);
    if (p)
    {
        loginfo(FUNC, "Already exist the interface");
        return (-1);
    }
    
    if (0 != InitInterface(&p))
    {
        loginfo(FUNC, "init the interface error");
        return (-1);
    }
    
    p->iIfindex = iIfx;
    p->stRAData.iIfindex = iIfx;
    p->iIANAFlag = iIANAFlag;
    p->iIAPDFlag = iIAPDFlag;
    p->iDsliteFlag = iDsliteFlag;

    /* start of by d00107688 2010-01-26 ����֧���Ƿ��������Ĭ��·�� */
    p->iDftRt = iDftRT;
    /* end of by d00107688 2010-01-26 ����֧���Ƿ��������Ĭ��·�� */

    p->iSockfd = CreateICMPv6Socket();
	loginfo(FUNC, "create icmp socket %d\r\n", p->iSockfd);
    if (0 > p->iSockfd)
    {
        loginfo(FUNC, "error: create icmp socket error\r\n");
        free(p);
        return (-1);
    }

    if (0 > SetICMPv6SockOpt(p->iSockfd))
    {
        loginfo(FUNC, "error: setsockopt error");
        close(p->iSockfd);
        free(p);
        return (-1);
    }

    if (0 > BindICMPSocket(p->iSockfd, p->iIfindex))
    {
        loginfo(FUNC, "Err: bind error");
        close(p->iSockfd);
        free(p);
        return (-1);
    }

    if (0 != AddInterfaceIntoList(&p))
    {
        loginfo(FUNC, "add into list error");
        close(p->iSockfd);
        free(p);
        return (-1);
    }

    if (0 != pthread_create(&p->tidp, NULL, RAMainLoop, (void *)p))
    {
        loginfo(FUNC, "crate pthread for pthread error");
        return (-1);
    }

    return (0);
}


int UpdateRAInfoByWanMonitor(int iIfindex)
{
    return UpdateByIfindex(iIfindex);
}


#if 0
int main(int argc, char **argv)
{
    int iIfx = -1;
    INTERFACE_RA_ST *p = NULL;
    
    if (2 != argc)
    {
        LOG_ERR("usage %s dev\r\n", argv[0]);
        return (-1);
    }

    iIfx = if_nametoindex(argv[1]);
    if (0 >= iIfx)
    {
        LOG_ERR("if_nametoindex error");
        return (-1);
    }

    if (0 != StartRAPthread(argv[1]))
    {
        LOG_ERR("start ra process thread error");
        return (-1);
    }

    p = FindInterfaceByIfindex(iIfx);

    if (!p)
    {
        LOG_ERR("Found error");
        return (-1);
    }
    
    pthread_join(p->tidp, NULL);

    return 0;
}

#endif
