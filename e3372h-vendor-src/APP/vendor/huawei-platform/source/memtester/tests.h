/* < DTS2013093005579 xiaohui 20131009 begin */
/* < DTS2013112300820 xiaohui 20131123 begin */

#ifndef DDR_TESTS_H
#define DDR_TESTS_H
/*
 * Very simple yet very effective memory tester.
 * Originally by Simon Kirby <sim@stormix.com> <sim@neato.org>
 * Version 2 by Charles Cazabon <charlesc-memtester@pyropus.ca>
 * Version 3 not publicly released.
 * Version 4 rewrite:
 * Copyright (C) 2004-2012 Charles Cazabon <charlesc-memtester@pyropus.ca>
 * Licensed under the terms of the GNU General Public License version 2 (only).
 * See the file COPYING for details.
 *
 * This file contains the declarations for the functions for the actual tests,
 * called from the main routine in memtester.c.  See other comments in that
 * file.
 *
 */

/* Function declaration. */


/* PRBS������ */

/*lint -e10 -e49 -e101 -e132 -e620 -e749 -e750 -e752*/

#define  SEED_LEN_7    7
#define  PRBS_LEN_7   (1<<SEED_LEN_7)-1  /*lint !e620*/  /* ���ظ���PRBS���� */
#define  MASK_1         (0x00000001ul)/*lint !e620*/
#define MTEST_PRBS_DATA_7                0x00000004
#define DATA_WIDTH  32  /* ������λ���ֽ��� */

#define SEQUNCE_INC 0x11111111   /* ����������������������� */
/* ��4bit��8bit���ݽ�������λƴװ��32bitλ�����ݣ������ڴ汳�����ݹ��졣2013-2-25 / z00157998 */
#define ULL_BYTE(x) (ul)((ul)x | (ul)x<<8 | (ul)x<<16 | (ul)x<<24)
#define CPU_BITS_WIDE 32


/**********************************************************************************

��������: ��PRBS�������ɵ�ַ���ϵ����ݻ����������ϵ����ݣ�����align��ʽ

��ڲ���:
        UINT32 *prbs_array      ��������
        UINT32 *paddrdata       ���ɵ�λ������
        UINT32 addrdata_wide    ����λ�� (��ַ���Ǵ�0��n-1,���������ַ�߿�Ϊn,ͬ��������)
        UINT32 ilocation        ѡȡ��������ָ����bitλ(��Ҫ��prbs sequence�еĵڼ���bit��ȡ���� ������Χ 1-->n)

�� �� ֵ:
************************************************************************************/

#define make_prbs_addrdata_align(prbs_array,paddrdata,ilocation)  \
    if(((prbs_array[ilocation>>5]>>(ilocation & 0x1f))&MASK_1) == 1)\
    {\
        paddrdata = 0xffffffff;\
    }\
    else\
    {\
        paddrdata = 0;\
    }


/**********************************************************************************

��������: ��PRBS�������ɵ�ַ���ϵ����ݻ����������ϵ����ݣ�����flip��ʽ

��ڲ���:
        UINT32 *prbs_array      ��������
        UINT32 *paddrdata       ���ɵ�λ������
        UINT32 addrdata_wide    ����λ�� (��ַ���Ǵ�0��n-1,���������ַ�߿�Ϊn,ͬ��������)
        UINT32 ilocation        ѡȡ��������ָ����bitλ(��Ҫ��prbs sequence�еĵڼ���bit��ȡ���� ������Χ 1-->n)
        UINT32 index_line       ��������λ(������ַ�ߣ������ߣ�ȷ���ĸ���ַ�߻�������ȡ�� ������Χ 0-->n-1)

�� �� ֵ:
************************************************************************************/

#define make_prbs_addrdata_alignflip(prbs_array, paddrdata, ilocation, index_line) \
    if(((prbs_array[ilocation>>5]>>(ilocation & 0x1f))&MASK_1) == 1)\
    {\
        paddrdata = 0xffffffff;\
    }\
    else\
    {\
        paddrdata = 0;\
    }\
    paddrdata = (paddrdata)^(MASK_1<<(index_line & 0x1f));

typedef enum
{

    T_FAIL          = 0x0, /* 0 ��ʾ����ʧ��*/
    T_PASS           = 0x1, /* 1 ��ʾ���Գɹ� */

    DDR_TEST_ERR_MAX = 0x7FFFFFFF,  /**< Force enum to 32 bits */
    DDR_TEST_ERR_NONE        = 0x0F   , /**< No error */



    DDR_LOG_AREA_TEST     = 0x88 ,


    /* ������㷨��֧�����������Ƿ�Ϊ�ڴ浥Ԫ���ϻ������߹���
    �ڴ浥Ԫ����ʱ���������͸�������м���ЧӦ�����߹��Ͻ�����Ϊ��һ��ַ�ռ���ϡ�
    */
    DDR_TEST_ERR_RANDOM_XOR   = 0xF0 , /*< RANDOM_XOR */
    DDR_TEST_ERR_RANDOM_SUB   = 0xF1 , /*< RANDOM_SUB */
    DDR_TEST_ERR_RANDOM_MUL   = 0xF2 , /*< RANDOM_MUL */
    DDR_TEST_ERR_RANDOM_OR   = 0xF3 , /*< RANDOM_OR */
    DDR_TEST_ERR_RANDOM_AND   = 0xF4 , /*< RANDOM_AND */
    DDR_TEST_ERR_RANDOM_INC   = 0xF5 , /*< RANDOM_INC */


    DDR_TEST_ERR_UNATTACHED       = 0x1F   , /*< No DDR attached */


    /* ע���㷨λ�õı��ţ�Ҫ��gastrMemTesters[]��Ӧ */
    /* �����׶��Ƽ��㷨 */
    DDR_TEST_ERR_PRBS_DATA_ALIGN   = 0x0, /*< PRBS_DATA_ALIGN  */
    DDR_TEST_ERR_PRBS_DATA_FLIP   = 0x1, /*< DATA_FLIP  */

    DDR_TEST_ERR_BIT_EQUALIZING      = 0x2,  /*< bit������� */
    DDR_TEST_ERR_MARCH_IC    = 0x3 , /*< MARCH_IC  */
    DDR_TEST_ERR_RANDOM    = 0x4 , /*< RANDOM */


    /* ǿ���׶ε�ɸѡ�㷨�����������з������ƹ��϶�λ */
    DDR_TEST_ERR_MARCH_LA       = 0x5 ,  /*< MARCH_LA */
    DDR_TEST_ERR_MARCH_LR       = 0x6 ,  /*< MARCH_LR */
    DDR_TEST_ERR_MARCH_RAW       = 0x7 ,  /*< MARCH_RAW */
    DDR_TEST_ERR_MARCH_SL       = 0x8,  /*< MARCH_SL */
    DDR_TEST_ERR_MARCH_SR    = 0x9 , /*< MARCH_SR */
    DDR_TEST_ERR_MARCH_SS    = 0xA , /*< MARCH_SS */
    DDR_TEST_ERR_MARCH_G    = 0xB , /*< MARCH_G */
    DDR_TEST_ERR_WALKING_ONES     = 0xC ,  /*< Walking ones error */
    DDR_TEST_ERR_VARIATION_WALKING_ONES = 0xD, /*< VARIATION_WALKING_ONES */
    DDR_TEST_ERR_DEVIL_ARITHMETIC  = 0xE, /*< DEVIL_ARITHMETIC */
    DDR_TEST_ERR_MODEL_DATA   = 0xF , /*< DDR_TEST_ERR_MODEL_DATA */

    TEST_OWN_ADDR = 0x10,

} ddr_test_error_type;/*lint !e751*/


int test_prbs_data_align_test_7(ulv* bufa, ulv* bufb, size_t count, char* log_buf, int thread_id);
int test_prbs_data_flip_test_7(ulv* bufa, ulv* bufb, size_t count, char* log_buf, int thread_id);
void err_log_printf(ulv *err_addr, ul err_data, ul b_data, ul t_type , ul step , char* log_buf, int thread_id);
int test_walking_ones(ulv* bufa, ulv* bufb, size_t count, char* log_buf, int thread_id);
int test_variation_walking_ones(ulv* bufa, ulv* bufb, size_t count, char* log_buf, int thread_id);
int test_own_addr(ulv* bufa, ulv* bufb, size_t count, char* log_buf, int thread_id);













int test_stuck_address(unsigned long volatile *bufa, size_t count, char* log_buf, int thread_id) ;
int test_random_value(unsigned long volatile *bufa, unsigned long volatile *bufb, size_t count , char* log_buf, int thread_id);
int test_xor_comparison(unsigned long volatile *bufa, unsigned long volatile *bufb, size_t count , char* log_buf, int thread_id);
int test_sub_comparison(unsigned long volatile *bufa, unsigned long volatile *bufb, size_t count , char* log_buf, int thread_id);
int test_mul_comparison(unsigned long volatile *bufa, unsigned long volatile *bufb, size_t count , char* log_buf, int thread_id);
int test_div_comparison(unsigned long volatile *bufa, unsigned long volatile *bufb, size_t count , char* log_buf, int thread_id);
int test_or_comparison(unsigned long volatile *bufa, unsigned long volatile *bufb, size_t count , char* log_buf, int thread_id);
int test_and_comparison(unsigned long volatile *bufa, unsigned long volatile *bufb, size_t count , char* log_buf, int thread_id);
int test_seqinc_comparison(unsigned long volatile *bufa, unsigned long volatile *bufb, size_t count , char* log_buf, int thread_id);
int test_solidbits_comparison(unsigned long volatile *bufa, unsigned long volatile *bufb, size_t count , char* log_buf, int thread_id);
int test_checkerboard_comparison(unsigned long volatile *bufa, unsigned long volatile *bufb, size_t count , char* log_buf, int thread_id);
int test_blockseq_comparison(unsigned long volatile *bufa, unsigned long volatile *bufb, size_t count , char* log_buf, int thread_id);
int test_walkbits0_comparison(unsigned long volatile *bufa, unsigned long volatile *bufb, size_t count , char* log_buf, int thread_id);
int test_walkbits1_comparison(unsigned long volatile *bufa, unsigned long volatile *bufb, size_t count , char* log_buf, int thread_id);
int test_bitspread_comparison(unsigned long volatile *bufa, unsigned long volatile *bufb, size_t count , char* log_buf, int thread_id);
int test_bitflip_comparison(unsigned long volatile *bufa, unsigned long volatile *bufb, size_t count , char* log_buf, int thread_id);
#ifdef TEST_NARROW_WRITES
int test_8bit_wide_random(unsigned long volatile *bufa, unsigned long volatile *bufb, size_t count , char* log_buf, int thread_id);
int test_16bit_wide_random(unsigned long volatile *bufa, unsigned long volatile *bufb, size_t count , char* log_buf, int thread_id);
#endif
int test_assist(unsigned long volatile * bufa, unsigned long volatile * bufb, size_t count);

#endif
/* DTS2013112300820 xiaohui 20131009 end > */
/* DTS2013093005579 xiaohui 20131009 end > */

