/* < DTS2013093005579 xiaohui 20131009 begin */
/* < DTS2013112300820 xiaohui 20131123 begin */

/*
 * Very simple but very effective user-space memory tester.
 * Originally by Simon Kirby <sim@stormix.com> <sim@neato.org>
 * Version 2 by Charles Cazabon <charlesc-memtester@pyropus.ca>
 * Version 3 not publicly released.
 * Version 4 rewrite:
 * Copyright (C) 2004-2012 Charles Cazabon <charlesc-memtester@pyropus.ca>
 * Licensed under the terms of the GNU General Public License version 2 (only).
 * See the file COPYING for details.
 *
 * This file contains the functions for the actual tests, called from the
 * main routine in memtester.c.  See other comments in that file.
 *
 */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "log.h"
#include "types.h"
#include "sizes.h"
#include "memtester.h"
#include "common.h"
#include "tests.h"

/*lint -e613 -e620*/
char progress[] = "-\\|/";
//#define PROGRESSLEN 4
#define PROGRESSOFTEN 2500
#define ONE 0x00000001uL

//#define DBG printf

ul err_total = 0 ;
ul ERR_COUNT  = 2 ;
extern int g_log_buffered;
/* Function definitions. */

int compare_regions(ulv *bufa, ulv *bufb, size_t count , char* log_buf, int thread_id )
{
    int r = 0;
    size_t i;
    ulv *p1 = bufa;
    ulv *p2 = bufb;
    off_t physaddr;

    thread_id = thread_id;

    for (i = 0; i < count; i++, p1++, p2++)
    {
        if (*p1 != *p2)
        {
            if (use_phys)
            {
                physaddr = physaddrbase + (long)(i * sizeof(ul));

                log_put( g_log_buffered, log_buf, 0,
                         "[%d]FAILURE: 0x%08lx != 0x%08lx at physical address "
                         "0x%08lx.\n",
                         thread_id, (ul) *p1, (ul) *p2, physaddr);
                log_put( g_log_buffered, log_buf, 1,
                         "                         " );

                show_current_freq_buf( log_buf , -1 );/*lint !e718*/
            }
            else
            {

                log_put( g_log_buffered, log_buf, 0,
                         "[%d]FAILURE: 0x%08lx != 0x%08lx at offset 0x%08lx.\n",
                         thread_id, (ul) *p1, (ul) *p2, (ul) (i * sizeof(ul)));
                log_put( g_log_buffered, log_buf, 1,
                         "                         " );

                show_current_freq_buf( log_buf , -1 );
            }
            /* printf("Skipping to next test..."); */
            r = -1;
            return r;
        }
    }
    return r;
}

int test_stuck_address(ulv *bufa, size_t count, char* log_buf, int thread_id)
{
    ulv *p1 = bufa;
    unsigned int j;
    size_t i;
    off_t physaddr;

    thread_id = thread_id;

    for (j = 0; j < 16; j++)
    {
        p1 = (ulv *) bufa;
        for (i = 0; i < count; i++)
        {
            *p1 = ((j + i) % 2) == 0 ? (ul) p1 : ~((ul) p1);
            p1++;
        }
        p1 = (ulv *) bufa;
        for (i = 0; i < count; i++, p1++)
        {
            if (*p1 != (((j + i) % 2) == 0 ? (ul) p1 : ~((ul) p1)))
            {
                if (use_phys)
                {
                    physaddr = physaddrbase + (long)(i * sizeof(ul));

                    log_put( g_log_buffered, log_buf, 0,
                             "[%d]FAILURE: possible bad address line at physical "
                             "address 0x%08lx.\n",
                             thread_id, physaddr);

                    log_put( g_log_buffered, log_buf, 1,
                             "                         " );

                    show_current_freq_buf( log_buf , -1 );
                }
                else
                {

                    log_put( g_log_buffered, log_buf, 0,
                             "[%d]FAILURE: possible bad address line at offset "
                             "0x%08lx.\n",
                             thread_id, (ul) (i * sizeof(ul)));

                    log_put( g_log_buffered, log_buf, 1,
                             "                         " );

                    show_current_freq_buf( log_buf, -1 );
                }

                log_put( g_log_buffered, log_buf, 1,
                         "Skipping to next test...\n");

                return -1;
            }
        }
    }
    return 0;
}

int test_random_value(ulv *bufa, ulv *bufb, size_t count, char* log_buf, int thread_id)
{
    ulv *p1 = bufa;
    ulv *p2 = bufb;
    size_t i;

    for (i = 0; i < count; i++)
    {
        *p1++ = *p2++ = rand_ul();
        if (!(i % PROGRESSOFTEN))
        {
            //putchar('\b');
            //putchar(progress[++j % PROGRESSLEN]);
            //fflush(stdout);
        }
    }
    return compare_regions(bufa, bufb, count, log_buf, thread_id);
}

int test_xor_comparison(ulv *bufa, ulv *bufb, size_t count , char* log_buf, int thread_id)
{
    ulv *p1 = bufa;
    ulv *p2 = bufb;
    size_t i;
    ul q = rand_ul();

    for (i = 0; i < count; i++)
    {
        *p1++ ^= q;
        *p2++ ^= q;
    }
    return compare_regions(bufa, bufb, count, log_buf, thread_id);
}

int test_sub_comparison(ulv *bufa, ulv *bufb, size_t count, char* log_buf, int thread_id)
{
    ulv *p1 = bufa;
    ulv *p2 = bufb;
    size_t i;
    ul q = rand_ul();

    for (i = 0; i < count; i++)
    {
        *p1++ -= q;
        *p2++ -= q;
    }
    return compare_regions(bufa, bufb, count, log_buf, thread_id);
}

int test_mul_comparison(ulv *bufa, ulv *bufb, size_t count, char* log_buf, int thread_id)
{
    ulv *p1 = bufa;
    ulv *p2 = bufb;
    size_t i;
    ul q = rand_ul();

    for (i = 0; i < count; i++)
    {
        *p1++ *= q;
        *p2++ *= q;
    }
    return compare_regions(bufa, bufb, count, log_buf, thread_id);
}

int test_div_comparison(ulv *bufa, ulv *bufb, size_t count, char* log_buf, int thread_id)
{
    ulv *p1 = bufa;
    ulv *p2 = bufb;
    size_t i;
    ul q = rand_ul();

    for (i = 0; i < count; i++)
    {
        if (!q)
        {
            q++;
        }
        *p1++ /= q;
        *p2++ /= q;
    }
    return compare_regions(bufa, bufb, count, log_buf, thread_id);
}

int test_or_comparison(ulv *bufa, ulv *bufb, size_t count , char* log_buf, int thread_id)
{
    ulv *p1 = bufa;
    ulv *p2 = bufb;
    size_t i;
    ul q = rand_ul();

    for (i = 0; i < count; i++)
    {
        *p1++ |= q;
        *p2++ |= q;
    }
    return compare_regions(bufa, bufb, count, log_buf, thread_id);
}

int test_and_comparison(ulv *bufa, ulv *bufb, size_t count, char* log_buf, int thread_id)
{
    ulv *p1 = bufa;
    ulv *p2 = bufb;
    size_t i;
    ul q = rand_ul();

    for (i = 0; i < count; i++)
    {
        *p1++ &= q;
        *p2++ &= q;
    }
    return compare_regions(bufa, bufb, count, log_buf, thread_id);
}

int test_seqinc_comparison(ulv *bufa, ulv *bufb, size_t count, char* log_buf, int thread_id)
{
    ulv *p1 = bufa;
    ulv *p2 = bufb;
    size_t i;
    ul q = rand_ul();

    for (i = 0; i < count; i++)
    {
        *p1++ = *p2++ = (i + q);
    }
    return compare_regions(bufa, bufb, count, log_buf, thread_id);
}

int test_solidbits_comparison(ulv *bufa, ulv *bufb, size_t count, char* log_buf, int thread_id)
{
    ulv *p1 = bufa;
    ulv *p2 = bufb;
    unsigned int j;
    ul q;
    size_t i;

    for (j = 0; j < 64; j++)
    {
        q = (j % 2) == 0 ? UL_ONEBITS : 0;
        p1 = (ulv *) bufa;
        p2 = (ulv *) bufb;
        for (i = 0; i < count; i++)
        {
            *p1++ = *p2++ = ((i % 2) == 0 ? q : ~q);
        }
        if (compare_regions(bufa, bufb, count, log_buf, thread_id))
        {
            return -1;
        }
    }
    return 0;
}

int test_checkerboard_comparison(ulv *bufa, ulv *bufb, size_t count, char* log_buf, int thread_id)
{
    ulv *p1 = bufa;
    ulv *p2 = bufb;
    unsigned int j;
    ul q;
    size_t i;

    for (j = 0; j < 64; j++)
    {
        q = (j % 2) == 0 ? CHECKERBOARD1 : CHECKERBOARD2;
        p1 = (ulv *) bufa;
        p2 = (ulv *) bufb;
        for (i = 0; i < count; i++)
        {
            *p1++ = *p2++ = ((i % 2) == 0 ? q : ~q);
        }
        if (compare_regions(bufa, bufb, count, log_buf, thread_id))
        {
            return -1;
        }
    }
    return 0;
}

int test_blockseq_comparison(ulv *bufa, ulv *bufb, size_t count, char* log_buf, int thread_id)
{
    ulv *p1 = bufa;
    ulv *p2 = bufb;
    unsigned int j;
    size_t i;

    for (j = 0; j < 256; j++)
    {
        p1 = (ulv *) bufa;
        p2 = (ulv *) bufb;
        for (i = 0; i < count; i++)
        {
            *p1++ = *p2++ = (ul) UL_BYTE(j);
        }
        if (compare_regions(bufa, bufb, count, log_buf, thread_id))
        {
            return -1;
        }
    }
    return 0;
}

int test_walkbits0_comparison(ulv *bufa, ulv *bufb, size_t count, char* log_buf, int thread_id)
{
    ulv *p1 = bufa;
    ulv *p2 = bufb;
    unsigned int j;
    size_t i;

    for (j = 0; j < UL_LEN * 2; j++)
    {
        p1 = (ulv *) bufa;
        p2 = (ulv *) bufb;
        for (i = 0; i < count; i++)
        {
            if (j < UL_LEN)   /* Walk it up. */
            {
                *p1++ = *p2++ = ONE << j;
            }
            else   /* Walk it back down. */
            {
                *p1++ = *p2++ = ( ONE << ((UL_LEN * 2 - j) - 1));
            }
        }
        if (compare_regions(bufa, bufb, count, log_buf, thread_id))
        {
            return -1;
        }
    }
    return 0;
}

int test_walkbits1_comparison(ulv *bufa, ulv *bufb, size_t count, char* log_buf, int thread_id)
{
    ulv *p1 = bufa;
    ulv *p2 = bufb;
    unsigned int j;
    size_t i;

    for (j = 0; j < UL_LEN * 2; j++)
    {
        p1 = (ulv *) bufa;
        p2 = (ulv *) bufb;
        for (i = 0; i < count; i++)
        {
            if (j < UL_LEN)   /* Walk it up. */
            {
                *p1++ = *p2++ = ( UL_ONEBITS ^ (ONE << j));
            }
            else   /* Walk it back down. */
            {
                *p1++ = *p2++ = ( UL_ONEBITS ^ (ONE << ((UL_LEN * 2 - j) - 1)));
            }
        }
        if (compare_regions(bufa, bufb, count, log_buf, thread_id))
        {
            return -1;
        }
    }
    return 0;
}

int test_bitspread_comparison(ulv *bufa, ulv *bufb, size_t count, char* log_buf, int thread_id)
{
    ulv *p1 = bufa;
    ulv *p2 = bufb;
    unsigned int j;
    size_t i;

    for (j = 0; j < UL_LEN * 2; j++)
    {
        p1 = (ulv *) bufa;
        p2 = (ulv *) bufb;
        for (i = 0; i < count; i++)
        {
            if (j < UL_LEN)   /* Walk it up. */
            {
                *p1++ = *p2++ = ((i % 2 == 0)
                                 ? (ONE << j) | (ONE << (j + 2))
                                 : UL_ONEBITS ^ ((ONE << j)
                                                 | (ONE << (j + 2))));
            }
            else   /* Walk it back down. */
            {
                *p1++ = *p2++ = ((i % 2 == 0)
                                 ? (ONE << ((UL_LEN * 2 - 1) - j)) | (ONE << ((UL_LEN * 2 + 1) - j))
                                 : UL_ONEBITS ^ (ONE << ((UL_LEN * 2 - 1) - j)
                                                 | (ONE << ((UL_LEN * 2 + 1) - j))));
            }
        }
        if (compare_regions(bufa, bufb, count, log_buf, thread_id))
        {
            return -1;
        }
    }
    return 0;
}

int test_bitflip_comparison(ulv *bufa, ulv *bufb, size_t count, char* log_buf, int thread_id)
{
    ulv *p1 = bufa;
    ulv *p2 = bufb;
    unsigned int j, k;
    ul q;
    size_t i;

    for (k = 0; k < UL_LEN; k++)
    {
        q = ONE << k;
        for (j = 0; j < 8; j++)
        {
            q = ~q;
            p1 = (ulv *) bufa;
            p2 = (ulv *) bufb;
            for (i = 0; i < count; i++)
            {
                *p1++ = *p2++ = ((i % 2) == 0 ? q : ~q);
            }
            if (compare_regions(bufa, bufb, count, log_buf, thread_id))
            {
                return -1;
            }
        }
    }
    return 0;
}

#ifdef TEST_NARROW_WRITES
int test_8bit_wide_random(ulv* bufa, ulv* bufb, size_t count, char* log_buf, int thread_id)
{
    u8v *p1, *t;
    ulv *p2;
    int attempt;
    unsigned int b, j = 0;
    size_t i;

    for (attempt = 0; attempt < 2;  attempt++)
    {
        if (attempt & 1)
        {
            p1 = (u8v *) bufa;
            p2 = bufb;
        }
        else
        {
            p1 = (u8v *) bufb;
            p2 = bufa;
        }
        for (i = 0; i < count; i++)
        {
            t = mword8.bytes;
            *p2++ = mword8.val = rand_ul();
            for (b = 0; b < UL_LEN / 8; b++)
            {
                *p1++ = *t++;
            }
            if (!(i % PROGRESSOFTEN))
            {
                //putchar('\b');
                //putchar(progress[++j % PROGRESSLEN]);
                //fflush(stdout);
            }
        }
        if (compare_regions(bufa, bufb, count , log_buf, thread_id))
        {
            return -1;
        }
    }
    return 0;
}

int test_16bit_wide_random(ulv* bufa, ulv* bufb, size_t count, char* log_buf, int thread_id)
{
    u16v *p1, *t;
    ulv *p2;
    int attempt;
    unsigned int b, j = 0;
    size_t i;

    for (attempt = 0; attempt < 2; attempt++)
    {
        if (attempt & 1)
        {
            p1 = (u16v *) bufa;
            p2 = bufb;
        }
        else
        {
            p1 = (u16v *) bufb;
            p2 = bufa;
        }
        for (i = 0; i < count; i++)
        {
            t = mword16.u16s;
            *p2++ = mword16.val = rand_ul();
            for (b = 0; b < UL_LEN / 16; b++)
            {
                *p1++ = *t++;
            }
            if (!(i % PROGRESSOFTEN))
            {
                //putchar('\b');
                //putchar(progress[++j % PROGRESSLEN]);
                //fflush(stdout);
            }
        }
        if (compare_regions(bufa, bufb, count, log_buf, thread_id))
        {
            return -1;
        }
    }
    return 0;
}
#endif

int test_assist(ulv* bufa, ulv* bufb, size_t count)
{
    ulv *p1;
    ulv *p2;
    ul temp;
    size_t i;
    extern int jump_freq_exit;
    extern int tests_finished;
    p1 = (ulv *) bufa;
    p2 = (ulv *) bufb;
    for (i = 0; i < count; i++)
    {
        if (jump_freq_exit || tests_finished )
        {
            return 0;
        }

        temp = *p1;
        *p2++ = ~temp + i;
        temp = *p2;
        *p1++ = ~temp + i;
    }
    return 0;
}

/* prbs ����cache line ���в���die0��die1 */

/**********************************************************************************
��������int test_prbs_data_align_test_7(ulv* bufa, ulv* bufb, size_t count)

��������: ��PRBS align�㷨�����������

                ����: PASS
 Create By    : z00157998 2013.3.25
 Modification :
 Restriction  : ���⺯��

***********************************************************************************/
int test_prbs_data_align_test_7(ulv* bufa, ulv* bufb, size_t count, char* log_buf, int thread_id )
{
    ulv *p1;
    ulv *p2;
    ulv *end;
    ul    backdata;
    ul    i = 0;        /* ���ڱ�������ѭ�� */
    int   r = 0;      /* return ״̬ */
    ul    prbs_array[4] = {0x86081fd5, 0xcae689e2, 0x6b648e17, 0x19dd2c6f}; /* PRBS�����ѹ̶� */
    ul    test_type = DDR_TEST_ERR_PRBS_DATA_ALIGN;
    ul    step = 0;
    ul    err_count = 0;
    p1 = (ulv*)bufa;
    p2 = (ulv*)bufb;
    end = (ulv *)((ul)bufa + ((ul)count * (CPU_BITS_WIDE / 8)) - 1);
    //timestamp_print_time(stdout);
    //printf("p1 = 0x%lx,p2 = 0x%lx,end = 0x%lx\n",p1,p2,end);
    //fflush(stdout);

    /*���յ�ַ���� w*/
    while (p1 < end)
    {
        //printf("in while bufa < end ");
        for (i = 0; i < PRBS_LEN_7; i++)
        {
            make_prbs_addrdata_align(prbs_array, backdata, i);

            /*���յ�ַ���� w*/
            *p1 = backdata;
            *p2 = ~backdata;

            p1 ++;
            p2 ++;

            if (p1 > end)
            {
                break;
            }
        }
    }

    p1 = (ulv*)bufa;
    p2 = (ulv*)bufb;
    end = (ulv *)((ul)bufa + ((ul)count * (CPU_BITS_WIDE / 8)) - 1);
    //printf("p1 = 0x%lx,p2 = 0x%lx,end = 0x%lx\n",p1,p2,end);
    //fflush(stdout);

    /*���յ�ַ���� r*/
    while (p1 < end)
    {
        for (i = 0; i < PRBS_LEN_7; i++)
        {
            make_prbs_addrdata_align(prbs_array, backdata, i);

            if (backdata != *p1)
            {
                step = 1;
                r = -1;
                err_count    ++;
                err_log_printf(p1, (ul)*p1, backdata, test_type , step , log_buf, thread_id );
            }

            if (~backdata != *p2)
            {
                step = 2;
                r = -1;
                err_count    ++;
                err_log_printf(p2, (ul)*p2, ~backdata, test_type , step, log_buf, thread_id );
            }

            p1 ++;
            p2 ++;

            if (err_count > ERR_COUNT)
            {
                return r;        /* ������������ֵ�˳����� */
            }
            if (p1 > end)
            {
                break;                    /* ���Ե�ַ���ڽ�����ַ��ʱ���˳�ѭ�� */
            }

        }

    }
    //timestamp_print_time(stdout);
    return r;

}

/**********************************************************************************
��������int test_prbs_data_flip_test_7(ulv* bufa, ulv* bufb, size_t count)

��������: ��PRBS flip�㷨�����������

 Create By    : z00157998 2013.3.25
 Modification :
 Restriction  : ���⺯��
***********************************************************************************/
int test_prbs_data_flip_test_7(ulv* bufa, ulv* bufb, size_t count , char* log_buf, int thread_id)
{
    ulv *p1;
    ulv *p2;
    ulv *end;
    ul    backdata;
    ul    i = 0;
    ul    k = 0;
    ul    j = 1;
    int   r = 0;
    ul    step;
    ul    err_count = 0;
    ul    prbs_array[4] = {0x86081fd5, 0xcae689e2, 0x6b648e17, 0x19dd2c6f}; /* PRBS�����ѹ̶� */
    ul    test_type = DDR_TEST_ERR_PRBS_DATA_FLIP;


    p1 = (ulv*)bufa;
    p2 = (ulv*)bufb;
    end = (ulv *)((ul)bufa + ((ul)count * (CPU_BITS_WIDE / 8)) - 1);
    //timestamp_print_time(stdout);
    //printf("p1 = 0x%lx,p2 = 0x%lx,end = 0x%lx\n",p1,p2,end);
    //fflush(stdout);
    /*���յ�ַ���� w*/
    while (p1 < end)
    {
        for (i = 0; i < PRBS_LEN_7; i++)
        {
            j <<= 1;
            k += 1;
            if (j == 0x0)
            {
                j = 0x1;
                k = 0;
            }

            make_prbs_addrdata_alignflip(prbs_array, backdata, i, k);

            /*���յ�ַ���� w*/
            *p1 = backdata;
            *p2 = ~backdata;

            p1 ++;
            p2 ++;

            if (p1 > end)
            {
                break;
            }
        }
    }


    k = 0;
    j = 1;
    p1 = (ulv*)bufa;
    p2 = (ulv*)bufb;
    end = (ulv *)((ul)bufa + ((ul)count * (CPU_BITS_WIDE / 8)) - 1);
    //printf("p1 = 0x%lx,p2 = 0x%lx,end = 0x%lx\n",p1,p2,end);
    //fflush(stdout);
    /*���յ�ַ���� r*/
    while (p1 < end)
    {
        for (i = 0; i < PRBS_LEN_7; i++)
        {
            j <<= 1;
            k += 1;
            if (j == 0x0)
            {
                j = 0x1;
                k = 0;
            }

            make_prbs_addrdata_alignflip(prbs_array, backdata, i, k);
            if (backdata != *p1)
            {
                step = 1;
                r = -1;
                err_count    ++;
                err_log_printf(p1, (ul)*p1, backdata, test_type , step, log_buf, thread_id );
            }

            if (~backdata != *p2)
            {
                step = 2;
                r = -1;
                err_count    ++;
                err_log_printf(p2, (ul)*p2, ~backdata, test_type , step, log_buf, thread_id );
            }

            p1 ++;
            p2 ++;

            if (err_count > ERR_COUNT)
            {
                return r;        /* ������������ֵ�˳����� */
            }
            if (p1 > end)
            {
                break;                    /* ���Ե�ַ���ڽ�����ַ��ʱ���˳�ѭ�� */
            }

        }
    }
    //timestamp_print_time(stdout);
    return r;

}




void    err_log_printf(ulv *err_addr, ul err_data, ul b_data, ul t_type , ul step, char* log_buf, int thread_id )
{
    off_t physaddr;

    thread_id = thread_id;

    if (use_phys)
    {
        physaddr = (physaddrbase + (off_t)err_addr);
    }
    else
    {
        physaddr = (off_t)err_addr;
    }

    log_put( g_log_buffered, log_buf, 0,
             "[%d]FAILURE: 0x%08lX,0x%08lX,0x%08lX,0x%lX,0x%lX.\n",
             thread_id, (ul)physaddr, err_data, b_data, t_type, step);

    log_put( g_log_buffered, log_buf, 1, "                         " );

    show_current_freq_buf( log_buf , -1 );

    /* printf("Skipping to next test..."); */

    err_total ++ ;    /* �����ַ���� */
}

int test_walking_ones(ulv* bufa, ulv* bufb, size_t count , char* log_buf, int thread_id)
{
    ulv *p1;
    ulv *p2;
    ulv *end;
    ul    pattern, pattern_a, pattern_b;
    ul    i;        /* ѭ�����Դ��� */
    int    r = 0;
    ul    step;
    ul    err_count = 0;
    ul start_pattern = 0;
    ul    test_type = DDR_TEST_ERR_VARIATION_WALKING_ONES;
    ul idx;
    //timestamp_print_time(stdout);

    for (i = 0;i < 2;i++)
    {
        p1 = (ulv*)bufa;
        p2 = (ulv*)bufb;
        end = (ulv *)((ul)bufa + ((ul)count * (CPU_BITS_WIDE / 8)) - 1);

        /*���յ�ַ���� w*/
        while (p1 < end)
        {
            idx = 0;

            for (start_pattern = 0x55555555; start_pattern != 0x0; start_pattern <<= 1)
            {
                if (i != 0)
                {
                    pattern = ~ start_pattern;
                }
                else
                {
                    pattern = start_pattern;
                }

                /*���յ�ַ���� w*/

                if (idx % 2)
                {
                    pattern_a = pattern;
                    pattern_b  = ~pattern;
                }
                else
                {
                    pattern_a = ~pattern;
                    pattern_b  = pattern;
                }

                *p1 = pattern_a;
                *p2 = pattern_b;
                p1 ++;
                p2 ++;

                pattern <<= 1;
                if (pattern == 0x0)
                {
                    pattern = 0x55555555;
                }

                idx++;

                if (p1 > end)
                {
                    break;
                }

            }
        }


        /*���յ�ַ���� r*/
        p1 = (ulv*)bufa;
        p2 = (ulv*)bufb;
        end = (ulv *)((ul)bufa + ((ul)count * (CPU_BITS_WIDE / 8)) - 1);

        while (p1 < end)
        {
            idx = 0;

            for (start_pattern = 0x55555555; start_pattern != 0x0; start_pattern <<= 1)
            {
                if (i != 0)
                {
                    pattern = ~ start_pattern;
                }
                else
                {
                    pattern = start_pattern;
                }

                if (idx % 2)
                {
                    pattern_a = pattern;
                    pattern_b  = ~pattern;
                }
                else
                {
                    pattern_a = ~pattern;
                    pattern_b  = pattern;
                }

                if (pattern_a != (*p1))
                {
                    step = 1;
                    r = -1;
                    err_count    ++;
                    err_log_printf(p1, (ul)*p1, pattern_a, test_type , step, log_buf, thread_id );
                }

                if (pattern_b != (*p2))
                {
                    step = 2;
                    r = -1;
                    err_count    ++;
                    err_log_printf(p2, (ul)*p2, pattern_b, test_type , step, log_buf, thread_id );
                }

                p1 ++;
                p2 ++;
                pattern <<= 1;
                if (pattern == 0x0)
                {
                    pattern = 0x55555555;
                }

                idx++;

                if (err_count > ERR_COUNT)
                {
                    return    r;        /* ������������ֵ�˳����� */
                }
                if (p1 > end)
                {
                    break;                    /* ���Ե�ַ���ڽ�����ַ��ʱ���˳�ѭ�� */
                }
            }
        }
    }
    //timestamp_print_time(stdout);
    return r;
}





/*****************************************************************************
 Description  : test_variation_walking_ones�ڴ���Ժ���
             ��1 �߲�����Ϊ��������)����ڴ棬����4*4=16n��
 Create By    : z00157998 2013.2.25 (z00157998ԭ���㷨)
 Modification :
 Restriction  : �豣֤�ڿ�����Χ����������ǰ���¶Դ洢��Ԫ���в���
*****************************************************************************/
/*aulBackDataArray[]����Ԫ���������˳��ɾ����BitMap���£�����1����λ����
bit    bit    bit    bit    bit    bit    bit    bit    bit    bit    bit    bit    bit    bit    bit    bit    bit    bit    bit    bit    bit    bit    bit    bit    bit    bit    bit    bit    bit    bit    bit    bit
31    30    29    28    27    26    25    24    23    22    21    20    19    18    17    16    15    14    13    12    11    10    9    8    7    6    5    4    3    2    1    0
3    2    1    0    3    2    1    0    3    2    1    0    3    2    1    0    3    2    1    0    3    2    1    0    3    2    1    0    3    2    1    0

0    0    0    1    1    1    1    0    0    0    0    1    1    1    1    0    0    0    0    1    1    1    1    0    0    0    0    1    1    1    1    0
0    0    1    0    1    1    0    1    0    0    1    0    1    1    0    1    0    0    1    0    1    1    0    1    0    0    1    0    1    1    0    1
0    1    0    0    1    0    1    1    0    1    0    0    1    0    1    1    0    1    0    0    1    0    1    1    0    1    0    0    1    0    1    1
1    0    0    0    0    1    1    1    1    0    0    0    0    1    1    1    1    0    0    0    0    1    1    1    1    0    0    0    0    1    1    1
1    1    1    0    0    0    0    1    1    1    1    0    0    0    0    1    1    1    1    0    0    0    0    1    1    1    1    0    0    0    0    1
1    1    0    1    0    0    1    0    1    1    0    1    0    0    1    0    1    1    0    1    0    0    1    0    1    1    0    1    0    0    1    0
1    0    1    1    0    1    0    0    1    0    1    1    0    1    0    0    1    0    1    1    0    1    0    0    1    0    1    1    0    1    0    0
0    1    1    1    1    0    0    0    0    1    1    1    1    0    0    0    0    1    1    1    1    0    0    0    0    1    1    1    1    0    0    0

END*/


int test_variation_walking_ones(ulv* bufa, ulv* bufb, size_t count , char* log_buf, int thread_id)
{
    ulv *p1;
    ulv *p2;
    //ul aulBackDataArray[] ={ 0xF0,0xE1,0xD2,0xC3,0xB4,0xA5,0x96,0x87,0x78,0x69,0x5A,0x4B,0x3C,0x2D,0x1E,0x0F };
    ul    pattern_array[] = { 0x1E  , 0x2D  , 0x4B  , 0x87, 0xF0, 0xE1, 0xD2, 0xC3, 0xB4, 0xA5, 0x96, 0x87, 0x78, 0x69, 0x5A, 0x4B, 0x3C, 0x2D, 0x1E, 0x0F };
    ul    array_num;
    ul    pattern_a, pattern_b;
    ul    idx;                                                                            /* ��������������ѭ�������ı��� */
    ul    offset;                                                                            /* �ڴ�����ƫ�Ʊ��� */
    ul    step = 0;                                                                       /* �ڼ��ζ� */
    ul    err_count = 0;                                                                  /* �����ַ�� */
    ul    test_type = DDR_TEST_ERR_VARIATION_WALKING_ONES;                                /* �����㷨��� */
    int    r = 0;
    ul    i;
    ul    j = 2;
//    ul    j = 1;
//timestamp_print_time(stdout);

    /* ������������ */
    array_num = sizeof(pattern_array) / sizeof(pattern_array[0]);

    for (i = 0;i < j;i++)
    {
        for (idx = 0; idx < array_num ; idx++)                /* ��������ѭ�� */
        {
            if (i != 0)
            {
                pattern_a = ~ ULL_BYTE(pattern_array[idx]);           /* ��������ƴװ��32bit���� */
                pattern_b = ULL_BYTE(pattern_array[idx]);            /* ��������ƴװ��32bit���� */
            }
            else
            {
                pattern_a = ULL_BYTE(pattern_array[idx]);            /* ��������ƴװ��32bit���� */
                pattern_b = ~ ULL_BYTE(pattern_array[idx]);           /* ��������ƴװ��32bit���� */
            }

            /*�� (wx)*/
            p1 = (ulv*)bufa;
            p2 = (ulv*)bufb;
            for (offset = 0; offset < count ; offset ++)
            {
                *p1 = pattern_a;
                *p2 = pattern_b;
                p1 ++;
                p2 ++;
            }

            /*��(rx) */
            p1 = (ulv*)bufa;
            p2 = (ulv*)bufb;
            for (offset = 0; offset < count ; offset ++)
            {

                if (pattern_a != *p1)
                {
                    step = 1;
                    r = -1;
                    err_count    ++;
                    err_log_printf(p1, (ul)*p1, pattern_a, test_type , step, log_buf, thread_id );
                }

                if (pattern_b != *p2)
                {
                    step = 2;
                    r = -1;
                    err_count    ++;
                    err_log_printf(p2, (ul)*p2, pattern_b, test_type , step, log_buf, thread_id );
                }

                p1 ++;
                p2 ++;

            }
            if (err_count > ERR_COUNT)
            {
                return    r;        /* ������������ֵ�˳����� */
            }
        }
    }
    //timestamp_print_time(stdout);
    return r;
}


/*****************************************************************************
 Description  : test_variation_walking_ones�ڴ���Ժ���
             ��1 �߲�����Ϊ��������)����ڴ棬����4*4=16n��
 Create By    : z00157998 2013.2.25 (z00157998ԭ���㷨)
 Modification :
 Restriction  : �豣֤�ڿ�����Χ����������ǰ���¶Դ洢��Ԫ���в���
*****************************************************************************/

int test_own_addr(ulv* bufa, ulv* bufb, size_t count , char* log_buf, int thread_id)
{
    ulv *p1;
    ulv *p2;
    ul    offset;                                                                            /* �ڴ�����ƫ�Ʊ��� */
    ul    step = 0;                                                                       /* �ڼ��ζ� */
    ul    err_count = 0;                                                                  /* �����ַ�� */
    ul    test_type = TEST_OWN_ADDR;                                                        /* �����㷨��� */
    int   r = 0;
    ul    pattern_a, pattern_b;

//timestamp_print_time(stdout);
    /*�� (wx)*/
    p1 = (ulv*)bufa;
    p2 = (ulv*)bufb;
    for (offset = 0; offset < count ; offset ++)
    {
        *p1 = (ul)p1;
        *p2 = (ul)p2;
        p1 ++;
        p2 ++;
    }

    /*��(rx) */
    p1 = (ulv*)bufa;
    p2 = (ulv*)bufb;
    for (offset = 0; offset < count ; offset ++)
    {

        if ((ul)p1 != *p1)
        {
            step = 1;
            r = -1;
            err_count    ++;
            err_log_printf(p1, (ul)*p1, (ul)p1, test_type , step, log_buf, thread_id );
        }

        if ((ul)p2 != *p2)
        {
            step = 2;
            r = -1;
            err_count    ++;
            err_log_printf(p2, (ul)*p2, (ul)p2, test_type , step, log_buf, thread_id );
        }

        p1 ++;
        p2 ++;

    }

    if (err_count > ERR_COUNT)
    {
        return    r;        /* ������������ֵ�˳����� */
    }

    /*�� (wx)*/
    p1 = (ulv*)bufa;
    p2 = (ulv*)bufb;
    for (offset = 0; offset < count ; offset ++)
    {
        pattern_a = (ul)(((ul)p1 & 0xffff) << (CPU_BITS_WIDE / 2) | ((ul)p1 & 0xffff0000) >> (CPU_BITS_WIDE / 2) )  ;
        pattern_b = (ul)(((ul)p2 & 0xffff) << (CPU_BITS_WIDE / 2) | ((ul)p2 & 0xffff0000) >> (CPU_BITS_WIDE / 2) )  ;
        *p1 = pattern_a;
        *p2 = pattern_b;
        p1 ++;
        p2 ++;
    }

    /*��(rx) */
    p1 = (ulv*)bufa;
    p2 = (ulv*)bufb;
    for (offset = 0; offset < count ; offset ++)
    {
        pattern_a = (ul)(((ul)p1 & 0xffff) << (CPU_BITS_WIDE / 2) | ((ul)p1 & 0xffff0000) >> (CPU_BITS_WIDE / 2) )  ;
        pattern_b = (ul)(((ul)p2 & 0xffff) << (CPU_BITS_WIDE / 2) | ((ul)p2 & 0xffff0000) >> (CPU_BITS_WIDE / 2) )  ;

        if (pattern_a != *p1)
        {
            step = 3;
            r = -1;
            err_count    ++;
            err_log_printf(p1, (ul)*p1, pattern_a, test_type , step, log_buf, thread_id );
        }

        if (pattern_b != *p2)
        {
            step = 4;
            r = -1;
            err_count    ++;
            err_log_printf(p2, (ul)*p2, pattern_b, test_type , step , log_buf, thread_id);
        }

        p1 ++;
        p2 ++;
    }
    //timestamp_print_time(stdout);
    return r;
}


/* DTS2013112300820 xiaohui 20131009 end > */
/* DTS2013093005579 xiaohui 20131009 end > */

