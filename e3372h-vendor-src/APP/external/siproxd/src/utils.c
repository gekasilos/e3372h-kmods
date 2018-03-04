/*
    Copyright (C) 2002-2005  Thomas Ries <tries@gmx.net>

    This file is part of Siproxd.
    
    Siproxd is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
    
    Siproxd is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with Siproxd; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
*/
/******************************************************************************
    <���ⵥ��>        <��  ��>  <�޸�ʱ��>   <�汾>      <�޸�����>         
    3082101267  00201037   2013/8/23               ʱ������
	3092906251  s00201037  2013/10/15              ����sipdns��������
	4032002611  l84014339  2013/03/21              ��get_ip_by_host��������ӵ��ʮ���Ƶ�IP�жϹ���
******************************************************************************/
#include "config.h"

#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <net/if.h>
#include <sys/ioctl.h>

#ifdef _SOLARIS2
# include <sys/sockio.h>
#endif

#include <sys/types.h>
#include <pwd.h>

#include <osipparser2/osip_parser.h>

#include "siproxd.h"
#include "log.h"

static char const ident[]="$Id: utils.c,v 1.1 2008/02/19 07:52:01 y42304 Exp $";

#ifndef INET_ADDRSTRLEN
#define INET_ADDRSTRLEN 16
#endif

#ifdef SUPPORT_BRIDGE_BIND
extern int g_lOutIFaceIsProxy;
extern char g_acPPPoEIpAddr[SIP_IPADDR];
extern char g_acPPPoEMask[SIP_IPADDR];
extern char g_acBrIpAddr[SIP_IPADDR];
extern char g_acBrMask[SIP_IPADDR];
#endif

/* configuration storage */
extern struct siproxd_config configuration;

extern int h_errno;
/*BEGIN 3092906251 s00201037 2013-10-15 added*/
extern unsigned int lansrcip;
extern unsigned int br0ip;
extern unsigned int br0mask;
extern int lansrcport;

#define IP_CONNTRACK_FILE "/proc/net/ip_conntrack"
#define BUFLEN 16

/******************************************************************************
  ��������  : GetipInfo
  ��������  : �����Ԫ�������ֵ
  �������  : char *pinput, char *pbuf

  ���ú���  :
  ��������  :
  �������  : ��
  �� �� ֵ  :
******************************************************************************/

static void GetipInfo(char *pinput, char *pbuf)
{
    while (*pinput != ' ')
    {
        *pbuf++ = *pinput++;
    }
    return;
}

/******************************************************************************
  ��������  : CheckSubnet
  ��������  : ���Դip��Ŀ��ip�Ƿ���ͬһ������
  �������  : uint32 src, uint32 dst, uint32 mask

  ���ú���  :
  ��������  :
  �������  : ��
  �� �� ֵ  :
******************************************************************************/

static int CheckSubnet(unsigned int src, unsigned int dst, unsigned int mask)
{
    if ((src & mask) == (dst & mask))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/******************************************************************************
  ��������  : ParseIpInfo
  ��������  : ����conntrack udp������
  �������  : int8 *pinput, uint32 *psrc,
              uint32 *pdst, int32 *psport, int32 *pdport

  ���ú���  :
  ��������  :
  �������  : ��
  �� �� ֵ  :
******************************************************************************/

static void ParseIpInfo(char *pinput, unsigned int *psrc,
                 unsigned int *pdst, int *psport, int *pdport,
                 char *pcompare)
{
    char buf[BUFLEN] = {0};
    char *p = NULL;

    p = strstr(pinput, "src=");
    if (NULL != p)
    {
        p += strlen("src=");
        GetipInfo(p, buf);
        if (strcmp(buf,pcompare))
        {
            *psrc = 0;
            *pdst = 0;
            return;
        }
        *psrc = htonl(inet_addr(buf));
        memset(buf, 0, BUFLEN);
        p = strstr(p, "dst=");
        if (NULL != p)
        {
            p += strlen("dst=");
            GetipInfo(p, buf);
            *pdst = htonl(inet_addr(buf));
            memset(buf, 0, BUFLEN);
            if (CheckSubnet(*psrc, *pdst,
                            br0mask))
            {
                *psrc = 0;
                *pdst = 0;
                return;
            }

            p = strstr(p, "sport=");
            if (NULL != p)
            {
                p += strlen("sport=");
                GetipInfo(p, buf);
                *psport = atoi(buf);
                memset(buf, 0, BUFLEN);
                p = strstr(p, "dport=");
                if (NULL != p)
                {
                    p += strlen("dport=");
                    GetipInfo(p, buf);
                    *pdport = atoi(buf);
                }
                else
                {
                    *psrc   = 0;
                    *pdst   = 0;
                    *psport = 0;
                }
            }
            else
            {
                *psrc = 0;
                *pdst = 0;
            }
        }
        else
        {
            *psrc = 0;
        }
    }

    return;
}


/******************************************************************************
  ��������  : GetConntrackInfo
  ��������  : �����Ԫ��
  �������  : int8 *p, Ip_Rule_Info_ST *piprunleinfo

  ���ú���  :
  ��������  :
  �������  : ��
  �� �� ֵ  :
******************************************************************************/

static void GetConntrackInfo(char *pbuf,char *sip ,unsigned int *dip)
{
    unsigned int src  = 0;
    unsigned int dst  = 0;
    int sport = 0;
    int dport = 0;

    /*udp*/
    if (NULL != strstr(pbuf, "udp"))
    {
        ParseIpInfo(pbuf, &src, &dst, &sport, &dport,sip);
        if (src && 
            (dport == configuration.sip_listen_port) &&
            (sport == lansrcport))
        {
            //��õ�dip
            *dip = dst;
        }
    }
    return;
}

/******************************************************************************
  ��������  : GetConntrackDstIp
  ��������  : ��/proc/net/ip_conntrack�ļ��������з�����
              ������֮��Ӧ��dstip
  �������  :

  ���ú���  :
  ��������  :
  �������  : ��
  �� �� ֵ  :
******************************************************************************/

static void GetConntrackDstIp(unsigned int sip, unsigned int *dip)
{
    char buf[BUFLENTH] = {0};
    char *psipbuf = NULL;
    FILE *fp = NULL;
    struct in_addr sin_addr = {0};
    sin_addr.s_addr = ntohl(sip);
    psipbuf = inet_ntoa(sin_addr);
    if (NULL == psipbuf)
    {
        return;
    }
    
    fp = fopen(IP_CONNTRACK_FILE, "r");

    if (NULL == fp)
    {
        return;
    }

    while (fgets(buf, BUFLENTH, fp))
    {   
        GetConntrackInfo(buf, psipbuf, dip);
        if (*dip)
        {
            break;
        }
    }
    fclose(fp);
    fp = NULL;
    return;
}
/*END 3092906251 s00201037 2013-10-15 added*/
/*
 * resolve a hostname and return in_addr
 * handles its own little DNS cache.
 *
 * RETURNS
 *	STS_SUCCESS on success
 *	STS_FAILURE on failure
 */
int get_ip_by_host(char *hostname, struct in_addr *addr) {
   int i, j;
   time_t t;
   struct hostent *hostentry;
#if defined(HAVE_GETHOSTBYNAME_R)
   struct hostent result_buffer;
   char tmp[GETHOSTBYNAME_BUFLEN];
#endif
   /*BEGIN 3092906251 s00201037 2013-10-15 added*/
   unsigned int serdstip  = 0; 
   /*END 3092906251 s00201037 2013-10-15 added*/
   int error;
   unsigned long hostip = 0;
   
   static struct {
      time_t timestamp;
      struct in_addr addr;
      char hostname[HOSTNAME_SIZE+1];
   } dns_cache[DNS_CACHE_SIZE];
   static int cache_initialized=0;

   if (hostname == NULL) {
      ERROR("get_ip_by_host: NULL hostname requested");
      return STS_FAILURE;
   }

   if (addr == NULL) {
      ERROR("get_ip_by_host: NULL in_addr passed");
      return STS_FAILURE;
   }
   
   hostip = inet_addr(hostname);
   if (INADDR_NONE != hostip)
   {
       memcpy(addr, &hostip, sizeof(struct in_addr));
       return STS_SUCCESS;
   }

   /* first time: initialize DNS cache */
   if (cache_initialized == 0) {
      DEBUGC(DBCLASS_DNS, "initializing DNS cache (%i entries)", DNS_CACHE_SIZE);
      memset(dns_cache, 0, sizeof(dns_cache));
      cache_initialized=1;
   }
   /*BEGIN 3082101267 00201037 20130823 modified*/
   osip_time(&t);
   /*END 3082101267 00201037 20130823 modified*/

   /* clean expired entries */
   for (i=0; i<DNS_CACHE_SIZE; i++) {
      if (dns_cache[i].hostname[0]=='\0') continue;
      if ( (dns_cache[i].timestamp+DNS_MAX_AGE) < t ) {
         DEBUGC(DBCLASS_DNS, "cleaning DNS cache (entry %i)", i);
         memset (&dns_cache[i], 0, sizeof(dns_cache[0]));
      }
   }

   /*
    * search requested entry in cache
    */
   for (i=0; i<DNS_CACHE_SIZE; i++) {
      if (dns_cache[i].hostname[0]=='\0') continue; /* empty */
      if (strcmp(hostname, dns_cache[i].hostname) == 0) { /* match */
         memcpy(addr, &dns_cache[i].addr, sizeof(struct in_addr));
         DEBUGC(DBCLASS_DNS, "DNS lookup - from cache: %s -> %s",
	        hostname, utils_inet_ntoa(*addr));
         return STS_SUCCESS;
      }
   }
   
   /* did not find it in cache, so I have to resolve it */

   /* need to deal with reentrant versions of gethostbyname_r()
    * as we may use threads... */
#if defined(HAVE_GETHOSTBYNAME_R)

   /* gethostbyname_r() with 3 arguments (e.g. osf/1) */
   #if defined(HAVE_FUNC_GETHOSTBYNAME_R_3)
   gethostbyname_r(hostname,		/* the FQDN */
		   &result_buffer,	/* the result buffer */ 
		   &hostentry
		   );
   if (hostentry == NULL) error = h_errno;

   /* gethostbyname_r() with 5 arguments (e.g. solaris, linux libc5) */
   #elif defined(HAVE_FUNC_GETHOSTBYNAME_R_5)
   hostentry = gethostbyname_r(hostname,        /* the FQDN */
			       &result_buffer,  /* the result buffer */
			       tmp,
			       GETHOSTBYNAME_BUFLEN,
			       &error);

   /* gethostbyname_r() with 6 arguments (e.g. linux glibc) */
   #elif defined(HAVE_FUNC_GETHOSTBYNAME_R_6)
   gethostbyname_r(hostname,        /* the FQDN */
		   &result_buffer,  /* the result buffer */
		   tmp,
		   GETHOSTBYNAME_BUFLEN,
		   &hostentry,
		   &error);
   #else
      #error "gethostbyname_r() with 3, 5 or 6 arguments supported only"
   #endif   
#elif defined(HAVE_GETHOSTBYNAME)
   hostentry=gethostbyname(hostname);
   if (hostentry == NULL) error = h_errno;
#else
   #error "need gethostbyname() or gethostbyname_r()"
#endif

   if (hostentry==NULL) {
      /*
       * Some errors just tell us that there was no IP resolvable.
       * From the manpage:
       *   HOST_NOT_FOUND
       *      The specified host is unknown.
       *   HOST_NOT_FOUND
       *      The specified host is unknown.
       *   NO_ADDRESS or NO_DATA
       *      The requested name is valid but does not have an IP
       *      address.
       */
      if ((error == HOST_NOT_FOUND) ||
          (error == NO_ADDRESS) ||
          (error == NO_DATA)) {
#ifdef HAVE_HSTRERROR
         DEBUGC(DBCLASS_DNS, "gethostbyname(%s) failed: h_errno=%i [%s]",
                hostname, h_errno, hstrerror(error));
#else
         DEBUGC(DBCLASS_DNS, "gethostbyname(%s) failed: h_errno=%i",
                hostname, error);
#endif
      } else {
#ifdef HAVE_HSTRERROR
         ERROR("gethostbyname(%s) failed: h_errno=%i [%s]",
               hostname, h_errno, hstrerror(h_errno));
#else
         ERROR("gethostbyname(%s) failed: h_errno=%i",hostname, h_errno);
#endif
      }
      /*BEGIN 3092906251 s00201037 2013-10-15 added*/
	  if (0 == lansrcip)
	  {
		  return STS_FAILURE;
	  }
	  if ((lansrcip & br0mask) == (br0ip & br0mask))
	  {
		  GetConntrackDstIp(lansrcip,&serdstip);
          if(serdstip)
          {
            addr->s_addr = ntohl(serdstip);
          }
          else
          {
            return STS_FAILURE;
          }
	  }
	  else
	  {
		  return STS_FAILURE;
	  }
   }
   else
   {
	   memcpy(addr, hostentry->h_addr, sizeof(struct in_addr));
   }
   /*END 3092906251 s00201037 2013-10-15 added*/
   DEBUGC(DBCLASS_DNS, "DNS lookup - resolved: %s -> %s",
          hostname, utils_inet_ntoa(*addr));

   /*
    * find an empty slot in the cache
    */
   j=0;
   for (i=0; i<DNS_CACHE_SIZE; i++) {
      if (dns_cache[i].hostname[0]=='\0') break;
      if (dns_cache[i].timestamp < t) {
         /* remember oldest entry */
         t=dns_cache[i].timestamp;
	 j=i;
      }
   }
   /* if no empty slot found, take oldest one */
   if (i >= DNS_CACHE_SIZE) i=j;

   /*
    * store the result in the cache
    */
   DEBUGC(DBCLASS_DNS, "DNS lookup - store into cache, entry %i)", i);
   memset(&dns_cache[i], 0, sizeof(dns_cache[0]));
   strncpy(dns_cache[i].hostname, hostname, HOSTNAME_SIZE);
   /*BEGIN 3082101267 00201037 20130823 modified*/
   osip_time(&dns_cache[i].timestamp);
   /*END 3082101267 00201037 20130823 modified*/
   memcpy(&dns_cache[i].addr, addr, sizeof(struct in_addr));
   printsip("Sip: Func:get_ip_by_host(), hostname:[%s], addr:[%s]\r\n", 
       hostname, inet_ntoa(*addr));
   
   return STS_SUCCESS;
}


/*
 * Secure enviroment:
 * If running as root, put myself into a chroot jail and
 * change UID/GID to user as requested in config file
 */
void secure_enviroment (void) {
   int sts;
   struct passwd *passwd=NULL;

   DEBUGC(DBCLASS_CONFIG,"running w/uid=%i, euid=%i, gid=%i, egid=%i",
          getuid(), geteuid(), getgid(), getegid());

   if ((getuid()==0) || (geteuid()==0)) {
      /*
       * preparation - after chrooting there will be NOTHING more around
       */
      if (configuration.user) passwd=getpwnam(configuration.user);

      /*
       * change root directory into chroot jail
       */
      if (configuration.chrootjail) {
         /* !!!
          * Before chrooting I must at least once trigger the resolver
          * as it loads some dynamic libraries. Once chrootet
          * these libraries will *not* be found and gethostbyname()
          * calls will simply fail (return NULL pointer and h_errno=0).
          * Also (at least for FreeBSD) syslog() needs to be called
          * before chroot()ing - this is done in main() by an INFO().
          * Took me a while to figure THIS one out
          */
         struct in_addr dummy;
         get_ip_by_host("localhost", &dummy);
         DEBUGC(DBCLASS_CONFIG,"chrooting to %s",
                configuration.chrootjail);
         sts = chroot(configuration.chrootjail);
	 if (sts != 0) DEBUGC(DBCLASS_CONFIG,"chroot(%s) failed: %s",
	                      configuration.chrootjail, strerror(errno));
         chdir("/");
      }

      /*
       * change user ID and group ID 
       */
      if (passwd) {
         DEBUGC(DBCLASS_CONFIG,"changing uid/gid to %s",
                configuration.user);
         sts = setgid(passwd->pw_gid);
         DEBUGC(DBCLASS_CONFIG,"changed gid to %i - %s",
	        passwd->pw_gid, (sts==0)?"Ok":"Failed");

         sts = setegid(passwd->pw_gid);
         DEBUGC(DBCLASS_CONFIG,"changed egid to %i - %s",
	        passwd->pw_gid, (sts==0)?"Ok":"Failed");

/* don't set the real user id - as we need to elevate privs
   when setting up an RTP masquerading tunnel */
/*&&& Actually this is no longer true (7-Jul-2004/xar) */
//         sts = setuid(passwd->pw_uid);
//         DEBUGC(DBCLASS_CONFIG,"changed uid to %i - %s",
//	        passwd->pw_uid, (sts==0)?"Ok":"Failed");

         sts = seteuid(passwd->pw_uid);
         DEBUGC(DBCLASS_CONFIG,"changed euid to %i - %s",
	        passwd->pw_uid, (sts==0)?"Ok":"Failed");
      }
   }
}


/*
 * get_ip_by_ifname:
 * fetches own IP address by its interface name
 *
 * STS_SUCCESS on returning a valid IP and interface is UP
 * STS_FAILURE if interface is DOWN or other problem
 */
int get_ip_by_ifname(char *ifname, struct in_addr *retaddr) {
   struct ifreq ifr;
   struct sockaddr_in *sin = (struct sockaddr_in *)&ifr.ifr_addr;
   int sockfd;
   int i, j;
   int ifflags, isup;
   time_t t;
   static struct {
      time_t timestamp;
      struct in_addr ifaddr;	/* IP */
      int isup;			/* interface is UP */
      char ifname[IFNAME_SIZE+1];
   } ifaddr_cache[IFADR_CACHE_SIZE];
   static int cache_initialized=0;

   if (ifname == NULL) {
      WARN("get_ip_by_ifname: got NULL ifname passed - please check config"
           "file ('if_inbound' and 'if_outbound')");
      return STS_FAILURE;
   }

   /* first time: initialize ifaddr cache */
   if (cache_initialized == 0) {
      DEBUGC(DBCLASS_DNS, "initializing ifaddr cache (%i entries)", 
             IFADR_CACHE_SIZE);
      memset(ifaddr_cache, 0, sizeof(ifaddr_cache));
      cache_initialized=1;
   }

   if (retaddr) memset(retaddr, 0, sizeof(struct in_addr));
   /*BEGIN 3082101267 00201037 20130823 modified*/
   osip_time(&t);
   /*END 3082101267 00201037 20130823 modified*/
   /* clean expired entries */
   for (i=0; i<IFADR_CACHE_SIZE; i++) {
      if (ifaddr_cache[i].ifname[0]=='\0') continue;
      if ( (ifaddr_cache[i].timestamp+IFADR_MAX_AGE) < t ) {
         DEBUGC(DBCLASS_DNS, "cleaning ifaddr cache (entry %i)", i);
         memset (&ifaddr_cache[i], 0, sizeof(ifaddr_cache[0]));
      }
   }

   /*
    * search requested entry in cache
    */
   for (i=0; i<IFADR_CACHE_SIZE; i++) {
      if (ifaddr_cache[i].ifname[0]=='\0') continue;
      if (strcmp(ifname, ifaddr_cache[i].ifname) == 0) { /* match */
         if (retaddr) memcpy(retaddr, &ifaddr_cache[i].ifaddr,
                             sizeof(struct in_addr));
         DEBUGC(DBCLASS_DNS, "ifaddr lookup - from cache: %s -> %s %s",
	        ifname, utils_inet_ntoa(ifaddr_cache[i].ifaddr),
                (ifaddr_cache[i].isup)? "UP":"DOWN");
         return (ifaddr_cache[i].isup)? STS_SUCCESS: STS_FAILURE;
      } /* if */
   } /* for i */

   /* not found in cache, go and get it */
   memset(&ifr, 0, sizeof(ifr));

   if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      ERROR("Error in socket: %s\n",strerror(errno));
      return STS_FAILURE;
   }

   strcpy(ifr.ifr_name, ifname);
   sin->sin_family = AF_INET;

   /* get interface flags */
   if(ioctl(sockfd, SIOCGIFFLAGS, &ifr) != 0) {
      ERROR("Error in ioctl SIOCGIFFLAGS: %s [%s]\n",
            strerror(errno), ifname);
      close(sockfd);
      return STS_FAILURE;
   } 
   ifflags=ifr.ifr_flags;

   /* get address */
   if(ioctl(sockfd, SIOCGIFADDR, &ifr) != 0) {
      ERROR("Error in ioctl SIOCGIFADDR: %s (interface %s)\n",
      strerror(errno), ifname);
      close(sockfd);
      return STS_FAILURE;
   } 

   if (ifflags & IFF_UP) isup=1;
   else isup=0;

   DEBUGC(DBCLASS_DNS, "get_ip_by_ifname: if %s has IP:%s (flags=%x) %s",
          ifname, utils_inet_ntoa(sin->sin_addr), ifflags,
          (isup)? "UP":"DOWN");
   
   printsip("Sip:get_ip_by_ifname: if[%s] has IP:[%s] (flags=[%x]) [%s] \r\n",
          ifname, utils_inet_ntoa(sin->sin_addr), ifflags,
          (isup)? "UP":"DOWN");

#ifdef SUPPORT_BRIDGE_BIND
   if ((1 == g_lOutIFaceIsProxy) && (strcmp(ifname, "br0") == 0))
   {
       if (0 == utils_inet_aton(g_acPPPoEIpAddr, &sin->sin_addr))
       {
           printsip("Sip: PPPoEIp:[%s]\r\n", g_acPPPoEIpAddr);
           utils_inet_aton("192.168.239.1", &sin->sin_addr);
       }
       printsip("Sip: The Proxy Br0 Addr:[%s] \r\n", inet_ntoa(sin->sin_addr));
   }
#endif
   /*
    *find an empty slot in the cache
    */
   j=0;
   for (i=0; i<IFADR_CACHE_SIZE; i++) {
      if (ifaddr_cache[i].ifname[0]=='\0') break;
      if (ifaddr_cache[i].timestamp < t) {
         /* remember oldest entry */
         t=ifaddr_cache[i].timestamp;
	 j=i;
      }
   }
   /* if no empty slot found, take oldest one */
   if (i >= IFADR_CACHE_SIZE) i=j;

   /*
    * store the result in the cache
    */
   DEBUGC(DBCLASS_DNS, "ifname lookup - store into cache, entry %i)", i);
   memset(&ifaddr_cache[i], 0, sizeof(ifaddr_cache[0]));
   strncpy(ifaddr_cache[i].ifname, ifname, IFNAME_SIZE);
   ifaddr_cache[i].timestamp=t;
   memcpy(&ifaddr_cache[i].ifaddr, &sin->sin_addr, sizeof(sin->sin_addr));
   ifaddr_cache[i].isup=isup;

   if (retaddr) memcpy(retaddr, &sin->sin_addr, sizeof(sin->sin_addr));

   close(sockfd);
   return (isup)? STS_SUCCESS : STS_FAILURE;
}


/*
 * utils_inet_ntoa:
 * implements an inet_ntoa()
 *
 * Returns pointer to a static character string.
 */
char *utils_inet_ntoa(struct in_addr in) {
#if defined(HAVE_INET_NTOP)
   static char string[INET_ADDRSTRLEN];
   if ((inet_ntop(AF_INET, &in, string, INET_ADDRSTRLEN)) == NULL) {
      ERROR("inet_ntop() failed: %s\n",strerror(errno));
      string[0]='\0';
   }
   return string;
#elif defined(HAVE_INET_NTOA)
   return inet_ntoa(in);
#else
#error "need inet_ntop() or inet_ntoa()"
#endif
}


/*
 * utils_inet_aton:
 * implements an inet_aton()
 *
 * converts the string in *cp and stores it into inp
 * Returns != 0 on success
 */
int  utils_inet_aton(const char *cp, struct in_addr *inp) {
#if defined(HAVE_INET_PTON)
   return inet_pton (AF_INET, cp, inp);
#elif defined(HAVE_INET_ATON)
   return inet_aton(cp, inp);
#else
#error "need inet_pton() or inet_aton()"
#endif
}

void get_local_ip(char *ip, struct in_addr *local_ip) {
   /* Returns the local IP address used to reach ip. */
   int sock;
   struct sockaddr_in local;
   struct sockaddr_in remote;
#ifdef SUPPORT_BRIDGE_BIND
   struct sockaddr_in stLocalAddr;
#endif
   int namelen;
   struct in_addr temip;
   int i;

   sock = socket(AF_INET, SOCK_DGRAM, 0);
   if (sock < 0) {
      perror("socket");
   }

   for (i = 0; i < strlen(ip); i++)
   {
       if (((ip[i] < '0') || (ip[i] > '9')) && (ip[i] != '.'))
       {
           break;
       }
   }

   memset((char *) &remote, 0, sizeof(remote));
   remote.sin_family = AF_INET;
   if (i < strlen(ip))
   {
       /* ����������� */
       if (STS_FAILURE == get_ip_by_host(ip, &temip))
       {
           printf("resolve failed.\n");
       }
       remote.sin_addr.s_addr = temip.s_addr;
   }
   else
   {
       remote.sin_addr.s_addr = inet_addr(ip);
   }
#ifdef SUPPORT_BRIDGE_BIND
   /* ���ط���Դ */
    bzero(&stLocalAddr, 0);

    /* Ŀ��ipΪLAN��ip */
    if ((1 == IpisInNet(ip, g_acBrIpAddr, g_acBrMask))
        || (1 == IpisInNet(ip, g_acPPPoEIpAddr, g_acPPPoEMask)))
    {
        get_ip_by_ifname(configuration.inbound_if, &stLocalAddr.sin_addr);
    }
    else
    {
        get_ip_by_ifname(configuration.outbound_if, &stLocalAddr.sin_addr);
    }
/*    
    if (g_lPacketDir == 0)
    {
        get_ip_by_ifname(configuration.outbound_if, &stLocalAddr.sin_addr);
//    stLocalAddr.sin_port = htons(atoi(pszLocalPort));
    }
    else
    {
        get_ip_by_ifname(configuration.inbound_if, &stLocalAddr.sin_addr);
    }
*/    
    stLocalAddr.sin_family = AF_INET;
    
    if (bind(sock, (struct sockaddr *)&stLocalAddr, sizeof(stLocalAddr)) < 0)
    {
    	perror("bind");
    }
    else
    {
        printsip("Sip:get_local_ip() bind src ip:[%s]\r\n", inet_ntoa(stLocalAddr.sin_addr));
    }
#endif
   /* �˴������Զ�Ѱ��·�ɣ�Ȼ��ͨ��getsockname���Ի����һ��·�ɵ�ַ */
   if (connect(sock, (struct sockaddr *) &remote, sizeof(remote))) {
      perror("connect");
   }
   else
    {
        printsip("Sip:get_local_ip() connect Remoteip:[%s]\r\n", inet_ntoa(remote.sin_addr));
    }

   namelen = sizeof(local);
   if (getsockname(sock, (struct sockaddr *) &local,
		   (socklen_t *) &namelen) < 0) {
      perror("getsockname");
   }

   if (close(sock)) {
      perror("close");
   }   
   memcpy(local_ip, &local.sin_addr, sizeof(struct in_addr));
   printsip("Sip: ip:[%s], local_ip[%s] \r\n", ip, inet_ntoa(*local_ip));
}
