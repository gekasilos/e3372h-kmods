/* < DTS2013093005579 xiaohui 20131009 begin */
/* < DTS2013112300820 xiaohui 20131123 begin */

#ifndef __ADD_OTA_TEST__
#define __ADD_OTA_TEST__    /*lint !e750*/
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

typedef struct _FileContents
{
    unsigned char* data;
    size_t size;
    struct stat st;
} FileContents;/*lint !e751*/

#define Memtester_DBG(fmt, args...) upgrade_print("%s,line=%d: "fmt, __func__ , __LINE__ , ##args)/*lint !e750*/

int get_args(int *argc, char ***argv) ;/*lint !e752*/

//int timestamp_print(FILE *stream,char* fmt, ...) ;       //һ���򵥵�������printf��ʵ��


//int memtester_print( char*fmt,...);

int timestamp_print_time(FILE *stream, int thread_id );  /*lint !e752 !e10 !e101 !e132*/     //һ���򵥵�������printf��ʵ��

#endif

/* DTS2013112300820 xiaohui 20131009 end > */
/* DTS2013093005579 xiaohui 20131009 end > */

