#ifndef __RA_H__
#define __RA_H__

#include <pthread.h>
#include "raopt.h"

#if 0
#define     log_error(str, args ...)                                  \
{                                                                   \
    printf("\r\n[%-16s : %-5d %8x] ", __FUNCTION__, __LINE__, pthread_self());          \
    printf(str, ## args);                                           \
    printf("\r\n\r\n");                                             \
}
#endif



#define MAX_RS_SEND_NUM             (3)

#define PREFIX_UPDATE_TIME          (4)

#define RS_LOOP_SEND_TIME           (2)

#define RS_NUM_MAX_VALUE            (4)

typedef enum
{
    en_rs_send      = 1,               /* �з���RS��ȥ */
    en_ra_rcv       = (1 << 1),        /* �յ���RA����ʱ�Ͳ���Ҫ����RS�� */
    en_st_start     = (1 << 2),         /* �Ƿ�������� */
}EN_FLAGS;


typedef struct _TagInterfaceRA
{
    struct  _TagInterfaceRA *pstNext;
    pthread_t               tidp;                /* �߳�ID */
    int                     iSockfd;
    int                     iIfindex;
    int                     iIANAFlag;        /* �Ƿ���IANA */
    int                     iIAPDFlag;        /* ��û�п���IAPD */
    int                     iDsliteFlag;      /* ��û�п���Dslite */
    /* start of by d00107688 2010-01-26 ����֧���Ƿ��������Ĭ��·�� */
    int                     iDftRt;           /* �Ƿ���RAʱ���Ĭ��·��ѧϰ */    
    /* start of by d00107688 2010-01-26 ����֧���Ƿ��������Ĭ��·�� */
    int                     iState;              /* ��ǰRA��״̬ */
    //int                     iDhcpState;          /* ��ǰ��DHCPC״̬ */
    int                     iRSNum;              /* ���͵�RS��Ŀ */
    int                     iFlags;              /* ��־ */
    RA_INFO_LIST_ST         stRAData;            /* ʵ���Ͼ���RA_INFO_LIST_ST */
}INTERFACE_RA_ST;

int StartRAPthread(char *pszIfName, int iIANAFlag, int iIAPDFlag, int iDsliteFlag, int iDftRT);
int ReInitInterfacebyIfIndex(int iIfIndex);
int StopRunningRA(int iIfindex);
int StartRunningRA(int iIfindex);
int UpdateRAInfoByWanMonitor(int iIfindex);

#endif
