/*************************************************************************
    > File Name: spi.c
    > Author: zhoubo
    > Mail: 1508540876@qq.com
    > Created Time: 2019年03月17日 星期日 17时26分44秒
 ************************************************************************/

#include "spi.h"

extern spi_regs_t *spi_regs;
void Spi_Cs_OutMode(void)
{
    spi_regs->gpbcon &= ~(3 << 10);
    spi_regs->gpbcon |= 1 << 10;
}

void Spi_Ck_OutMode(void)
{
    spi_regs->gpbcon &= ~(3 << 12);
    spi_regs->gpbcon |= 1 << 12;
}

void Spi_Si_OutMode(void)
{
    spi_regs->gpbcon &= ~(3 << 14);
    spi_regs->gpbcon |= 1 << 14;
}
void Spi_So_InMode(void)
{
    spi_regs->gpbcon &= ~(3 << 16);
}
void Spi_Cs_SetPin(u8 sta)
{
    if (sta)
        spi_regs->gpbdat |= 1 << 5;
    else
        spi_regs->gpbdat &= ~(1 << 5);
}

void Spi_Ck_SetPin(u8 sta)
{
    if (sta)
        spi_regs->gpbdat |= 1 << 6;
    else
        spi_regs->gpbdat &= ~(1 << 6);
}

void Spi_Si_SetPin(u8 sta)
{
    if (sta)
        spi_regs->gpbdat |= 1 << 7;
    else
        spi_regs->gpbdat &= ~(1 << 7);
}
u8 Spi_So_GetPin(void)
{
    if (spi_regs->gpbdat & (1 << 8))
        return 1;
    return 0;
}

void Spi_Init(void)
{
    Spi_Cs_OutMode();
    Spi_Ck_OutMode();
    Spi_Si_OutMode();
    Spi_So_InMode();
    Spi_Cs_SetPin(1);
    Spi_Ck_SetPin(1);
    Spi_Si_SetPin(1);


}

void Spi_WriteOneByte(u8 data)
{
    u8 i;
    for (i = 0; i < 8; i++)
    {
        Spi_Ck_SetPin(0);
        udelay(1);
        if (data & 0x80)
            Spi_Si_SetPin(1);
        else
            Spi_Si_SetPin(0);
        data <<= 1;
        udelay(1);
        Spi_Ck_SetPin(1);
        udelay(1);
    }
}

u8 Spi_ReadOneByte(void)
{
    u8 i, data = 0;
    for (i = 0; i < 8; i++)
    {
        Spi_Ck_SetPin(0);
        udelay(1);
        data <<= 1;
        if (Spi_So_GetPin())
            data |= 0x01;
        Spi_Ck_SetPin(1);
        udelay(1);
    }
    return data;
}
