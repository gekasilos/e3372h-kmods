/*************************************************************************
*   ��Ȩ����(C) 1987-2020, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  pmu.h
*
*   ��    �� :  xuwenfang
*
*   ��    �� :  pmu.c ��ͷ�ļ�
*
*
*************************************************************************/

#ifndef __PMU_H__
#define __PMU_H__

#include <balongv7r2/types.h>
#include <boot/boot.h>
#include <bsp_pmu.h>

#define  pmic_print_error(fmt, ...)    (cprintf("[pmu]: <%s> "fmt"\n", __FUNCTION__, ##__VA_ARGS__))
#define  pmic_print_info(fmt, ...) \
do {                               \
       /* pmic_print_error(fmt, ##__VA_ARGS__);*/\
} while (0)

/*��������, ����bsp_pmu.h��*/
#endif