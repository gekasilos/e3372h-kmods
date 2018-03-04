
#include <product_config.h>
#include <boot/boot.h>
#include <balongv7r2/spi.h>
#include <balongv7r2/parameters.h>
#include <hi_syscrg_interface.h>
#include "hi_syssc_interface.h"
#include <soc_memmap.h>
#include "hi_spi.h"

const u32 spiBase[2] = {ECS_SPI0_BASE, ECS_SPI1_BASE};

void spi_clk_enable(u32 spiNo)
{
    if ((0 != spiNo) && (1 != spiNo))
    {
        return ;
    }

    if(spiNo)
        hi_syscrg_spi1_clk_enable();
    else
        hi_syscrg_spi0_clk_enable();
}

s32 spi_init(u32 spiNo)
{
    if ((0 != spiNo) && (1 != spiNo))
    {
        return ERROR;
    }

    /*��clk*/
    spi_clk_enable(spiNo);
    
    /* ��ֹSPI Slave*/
    writel(0,SPI_SLAVE_EN(spiNo));

    /* ��ֹSPI Master*/
    writel(0,SPI_EN(spiNo));

    if(spiNo == SPI_LCD_NUM)
    {   
#if(FEATURE_ON == MBB_COMMON)
        writel((0x07<<SPI_CTRL0_DFS_BITPOS)    \
                | (1<<SPI_CTRL0_SCPH_BITPOS)    \
                | (1<<SPI_CTRL0_SCPOL_BITPOS)   \
                | (0 << SPI_CTRL0_FRF_BITPOS)   \
                | (0x00<<SPI_CFS_BITPOS),SPI_CTRL0(spiNo));
         /* ����SPI������ΪSSI CLK��1/24����48/6=8MHz */
        writel(24,SPI_BAUD(spiNo));
#else
        writel((0x08<<SPI_CTRL0_DFS_BITPOS)    \
                | (0<<SPI_CTRL0_SCPH_BITPOS)    \
                | (0<<SPI_CTRL0_SCPOL_BITPOS)   \
                | (0 << SPI_CTRL0_FRF_BITPOS)   \
                | (0x00<<SPI_CFS_BITPOS),SPI_CTRL0(spiNo));
         /* ����SPI������ΪSSI CLK��1/24����48/6=8MHz */
        writel(6,SPI_BAUD(spiNo));
#endif
    }
    else
    {
        /* ����ctrl0�Ĵ����������ֳ���Ϊ1�����ݿ��Ϊ8,�����ش���,�͵�ƽ��Ч */
        writel((0x0f<<SPI_CTRL0_DFS_BITPOS)    \
                | (1<<SPI_CTRL0_SCPH_BITPOS)    \
                | (1<<SPI_CTRL0_SCPOL_BITPOS)   \
                | (0 << SPI_CTRL0_FRF_BITPOS)   \
                | (0x00<<SPI_CFS_BITPOS),SPI_CTRL0(spiNo));

    /* ����SPI������ΪSSI CLK��1/24����48/24=2MHz */
    writel(24,SPI_BAUD(spiNo));
    }

    /*��ֹ�����ж��ź�*/
    writel(0,SPI_IMR(spiNo));

    /*����DMA����*/
    writel(0, SPI_DMAC(spiNo));

    return OK;
}

/*****************************************************************************
* �� �� ��  : spiRecv
*
* ��������  : SPI������ѯ��������
*
* �������  : spiNo SPI��������
*                  cs:   Ƭѡ��
*                  prevData:   ָ��Ҫ��ȡ���ݵ�ַ��ָ��
*                  recvSize��Ҫ��ȡ�����ݵĴ�С
*                  psendData: ָ��Ҫ���͵�����͵�ַ��ָ��
*                  sendSize:  Ҫ��������͵�ַ�ĳ���(3 ����4)
* �������  :
*
* �� �� ֵ  : OK or ERROR
*
* ����˵��  :
*
*****************************************************************************/
s32 spi_recv (u32 spiNo, u32 cs, u16* prevData, u32 recvSize,u16* psendData,u32 sendSize )
{
    u16 *pRh;
    u16 *pSh;
    u32  i;
    u32 ulLoop = SPI_MAX_DELAY_TIMES;

    /*��2��spi��������ÿ����������2��Ƭѡ*/
    if (((0 != spiNo) && (1 != spiNo))
        || (0 != cs && 1 != cs)
        || (NULL == psendData) || (NULL == prevData) || (0 == recvSize) || (0 == sendSize))
    {
        return ERROR;
    }

    pRh = prevData;
    pSh = psendData;

    /* ��ֹSPI Slave*/
    writel(0,SPI_SLAVE_EN(spiNo));

    /* ��ֹSPI Master*/
    writel(0, SPI_EN(spiNo));

    /* ���ó�EEPROM��ģʽ */
    /*writel((readl(SPI_CTRL0(spiNo)) | SPI_CTRL0_TMOD_EEPROM_READ),SPI_CTRL0(spiNo));*/
    writel(((readl(SPI_CTRL0(spiNo)) & ~SPI_CTRL0_TMOD_BITMASK) | SPI_CTRL0_TMOD_SEND_RECV),SPI_CTRL0(spiNo));

    /* ���ý������ݵ���Ŀ*/
    writel((recvSize-1),SPI_CTRL1(spiNo));

    /*ʹ��SPI Master*/
    writel(1, SPI_EN(spiNo));

    /*ʹ��SPI Slave*/
    writel((1 << cs), SPI_SLAVE_EN(spiNo));

    /* �������� */
    for(i = 0; i < sendSize; i++)
    {
        /* �ȴ�����FIFO���� */
        while(!(readl(SPI_STATUS(spiNo)) & SPI_STATUS_TXNOTFULL)
            && (0 != --ulLoop))
        {
        }

        if(0 == ulLoop)
        {
            return ERROR;
        }

        writel(*pSh++, SPI_DR(spiNo));
    }

    /*������FIFO�е�����ȫ������*/
    while(!(readl(SPI_STATUS(spiNo)) & SPI_STATUS_TXEMPTY)
        && (0 != --ulLoop))
    {
    }

    if(0 == ulLoop)
    {
        return ERROR;
    }

    /* �������� */
    for(i = 0; i < recvSize; i++)
    {
        ulLoop = SPI_MAX_DELAY_TIMES;
        /* �ȴ���ȡ������ */
        while(!(readl(SPI_STATUS(spiNo)) & SPI_STATUS_RXNOTEMPTY)
            && (0 != --ulLoop))
        {
        }

        if(0 == ulLoop)
        {
            return ERROR;
        }

        *pRh++ = readl(SPI_DR(spiNo));

    }

/* SPI���������Զ���ʹ��,��������������� */
#if 0
    /* ��ֹSPI Slave*/
    writel(0, SPI_SLAVE_EN(spiNo));

    /* ��ֹSPI Master*/
    writel(0, SPI_EN(spiNo));
#endif
    return OK;
}

/*****************************************************************************
* �� �� ��  : spiSend
*
* ��������  : SPI������ѯ����
*
* �������  : spiNo SPI��������
*                  cs:   Ƭѡ��
*                  pData:   ָ��Ҫ�������ݵ�ַ��ָ��
*                  ulLen��Ҫ���͵����ݵĴ�С
* �������  :
*
* �� �� ֵ  : OK or ERROR
*
* ����˵��  :
*
*****************************************************************************/
s32 spi_send (u32 spiNo, u32 cs, u16* pData, u32 ulLen)
{
    u16 *pSh;
    u32  i;
    u32 ulLoop = SPI_MAX_DELAY_TIMES;
    u32 ulVal;

    if (((0 != spiNo) && (1 != spiNo))
        || (0 != cs && 1 != cs)
        || (NULL == pData) || (0 == ulLen))
    {
        return ERROR;
    }

    pSh = (u16*)pData;

    /* ��ֹSPI Slave*/
    writel(0, SPI_SLAVE_EN(spiNo));

    /* ��ֹSPI Master*/
    writel(0, SPI_EN(spiNo));

    /* ���óɷ���ģʽ */
    writel(((readl(SPI_CTRL0(spiNo)) & ~SPI_CTRL0_TMOD_BITMASK) | SPI_CTRL0_TMOD_SEND),SPI_CTRL0(spiNo));

    /*ʹ��SPI Master*/
    writel(1, SPI_EN(spiNo));

    /*ʹ��SPI Slave*/
    writel((1 << cs), SPI_SLAVE_EN(spiNo));

    /* �������� */
    for(i = 0; i < ulLen; i++)
    {
        /* �ȴ�����FIFO���� */
        while(!(readl(SPI_STATUS(spiNo)) & SPI_STATUS_TXNOTFULL)
            && (0 != --ulLoop))
        {
        }

        if(0 == ulLoop)
        {
            return ERROR;
        }
        writel(*pSh++, SPI_DR(spiNo));
    }

    /*������FIFO�е�����ȫ������,�Ҳ�BUSY*/
    ulLoop = SPI_MAX_DELAY_TIMES;
    ulVal = readl(SPI_STATUS(spiNo));
    while(((!(ulVal & SPI_STATUS_TXEMPTY)) || (ulVal & SPI_STATUS_BUSY))
        && (0 != --ulLoop))
    {
        ulVal = readl(SPI_STATUS(spiNo));
    }

    if(0 == ulLoop)
    {
        return ERROR;
    }

    return OK;
}
/*****************************************************************************
* �� �� ��  : spi_three_wire_set
*
* ��������  : ����3��ģʽ
*
* �������  :
* �������  :
*
* �� �� ֵ  :
*
* ����˵��  :
*
*****************************************************************************/
void spi_three_wire_set(u32 spiNo)
{
    hi_spi_three_wire_set(spiNo);
}

/*****************************************************************************
* �� �� ��  : spiDevDetect
*
* ��������  : ��̬̽��SPI�����Ͻ�����豸���ͣ���֤SPI��������
*
* �������  :
* �������  :
*
* �� �� ֵ  :
*
* ����˵��  :
*
*****************************************************************************/
s32 spi_dev_detect( eSpiDevType *devType )
{
    u16 ucCmdRDID = SPI_DEV_CMD_RDID;
    u32 ulRecv = 0;

    /*cprintf("\r\nspiDevDetect begin!" );*/
    if(NULL == devType)
    {
        cprintf("\r\nPARA *devType is NULL!");
        return ERROR;
    }

    /* EEPROM�Ը����֧�֣����ղ������ݳ�ʱ�˳� */
    if(OK != spi_recv(SPI_NO_DEV, SPI_CS_DEV, (u16*)&ulRecv, 3, &ucCmdRDID, 1))
    {
        cprintf("\r\nread ID err");
        *devType = E_SPI_DEV_TYPE_EEPROM;

        return OK;
    }

    cprintf("\r\nID: %x", ulRecv);

    /* EEPROM��֧�ָ�����᷵��ȫ0��ȫ1 */
    if(0 != ulRecv && 0xFFFFFF != ulRecv)
    {
        *devType = E_SPI_DEV_TYPE_SFLASH;
    }
    else
    {
        *devType = E_SPI_DEV_TYPE_EEPROM;
    }
    cprintf("\r\ndevType is :  @ %x", *devType);

    return OK;

}
s32 spitest()
{
    eSpiDevType edevType;

    (void)spi_init(SPI_NO_DEV);

    (void)spi_dev_detect(&edevType);

    if(E_SPI_DEV_TYPE_EEPROM == edevType)
    {
        cprintf("\r\nE2PROM_boot!" );
    }
    else
    {
        cprintf("\r\nSF_boot!" );
    }
    return 0;
}
