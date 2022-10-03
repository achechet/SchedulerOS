/******************** (C) COPYRIGHT 2015 HED *************************************
* File Name          : CIU98428F_SFR_C.h
* Author             : zhangyj
* Date First Issued  : 2015-01-16
* Description        : 
// ------------------------------------------------------------
// Modification History:
// Version    Date       Author    Modification Description
// V0.0      2015-01-16  zhangyj   
*********************************************************************************/

#ifndef __IO_CIU98428F_H
#define __IO_CIU98428F_H

/********************************************************************************
                      AMBA IP AHB Peripherals Addresse
********************************************************************************/
/********************************************************************************
                             DES Regsiter
********************************************************************************/
#define DESCON               (*((volatile unsigned long *)(0x50000000 + 0x00 )))
#define DESMOD               (*((volatile unsigned long *)(0x50000000 + 0x04 )))
#define DESIV                ((volatile unsigned long *)(0x50000000 + 0x08 )) //64bit
#define DESDATA              ((volatile unsigned long *)(0x50000000 + 0x10 )) //64bit
#define DESKEY1              ((volatile unsigned long *)(0x50000000 + 0x18 )) //64bit
#define DESKEY2              ((volatile unsigned long *)(0x50000000 + 0x20 )) //64bit


/********************************************************************************
                              RNG2 Regsiter
********************************************************************************/
#define RNG2STR              (*((volatile unsigned long *)(0x50003800 + 0x04 )))
#define RNG2OUT              (*((volatile unsigned long *)(0x50003800 + 0x08 )))


/********************************************************************************
                             CRC Regsiter
********************************************************************************/
#define CRCCSR               (*((volatile unsigned long *)(0x50005000 + 0x00 )))
#define CRCDATA              (*((volatile unsigned long *)(0x50005000 + 0x04 )))
#define CRCIV                (*((volatile unsigned long *)(0x50005000 + 0x08 )))


/********************************************************************************
                             SystemController Regsiter
********************************************************************************/
// NVM
#define SYSNVMSTATUS         (*((volatile unsigned long *)(0x50007000 + 0x80 )))
#define SYSNVMMODE           (*((volatile unsigned long *)(0x50007000 + 0x84 )))
#define SYSNVMCON            (*((volatile unsigned long *)(0x50007000 + 0x88 )))
#define SYSNVMPBSTADDR       (*((volatile unsigned long *)(0x50007000 + 0x98 )))
#define SYSNVMPBLEN          (*((volatile unsigned long *)(0x50007000 + 0x9C )))
#define SYSNVMSDPK1          (*((volatile unsigned long *)(0x50007000 + 0xA0 )))
#define SYSNVMSDPK2          (*((volatile unsigned long *)(0x50007000 + 0xA4 )))
#define SYSNVMSDPK3          (*((volatile unsigned long *)(0x50007000 + 0xA8 )))


// RSTMU
#define SYSRSTFLAG           (*((volatile unsigned long *)(0x50007000 + 0x184)))
#define SYSRSTCFG            (*((volatile unsigned long *)(0x50007000 + 0x188)))


// CKMU
#define SYSCLKEN             (*((volatile unsigned long *)(0x50007000 + 0x200)))
#define SYSCLKCLR            (*((volatile unsigned long *)(0x50007000 + 0x204)))
#define SYSCLKCFG            (*((volatile unsigned long *)(0x50007000 + 0x208)))


// POWMU
#define SYSPCON1             (*((volatile unsigned long *)(0x50007000 + 0x280)))


// INTC
#define SYSIRQLR             (*((volatile unsigned long *)(0x50007000 + 0x380)))
#define SYSNMICON            (*((volatile unsigned long *)(0x50007000 + 0x384)))


// SEC
#define SECCTR               (*((volatile unsigned long *)(0x50007000 + 0x400)))
#define SECHWRSTFLG          (*((volatile unsigned long *)(0x50007000 + 0x41C)))
#define SECBUSY              (*((volatile unsigned long *)(0x50007000 + 0x420)))
#define SECRBCLR             (*((volatile unsigned long *)(0x50007000 + 0x424)))
#define SECREGPRT            (*((volatile unsigned long *)(0x50007000 + 0x428)))
#define SECRNDSTOP           (*((volatile unsigned long *)(0x50007000 + 0x434)))


/********************************************************************************
                             TIMER Regsiter
********************************************************************************/
#define TIMER0LOAD           (*((volatile unsigned long *)(0x40000800 + 0x00 )))
#define TIMER0VALUE          (*((volatile unsigned long *)(0x40000800 + 0x04 )))
#define TIMER0CONTROL        (*((volatile unsigned long *)(0x40000800 + 0x08 )))
#define TIMER0IS             (*((volatile unsigned long *)(0x40000800 + 0x0C )))
#define TIMER1LOAD           (*((volatile unsigned long *)(0x40000800 + 0x20 )))
#define TIMER1VALUE          (*((volatile unsigned long *)(0x40000800 + 0x24 )))
#define TIMER1CONTROL        (*((volatile unsigned long *)(0x40000800 + 0x28 )))
#define TIMER1IS             (*((volatile unsigned long *)(0x40000800 + 0x2C )))

/********************************************************************************
                             SCI Regsiter
********************************************************************************/
#define SCISBUF              (*((volatile unsigned long *)(0x40001000 + 0x00 )))
#define SCISCON              (*((volatile unsigned long *)(0x40001000 + 0x04 )))
#define SCISSR               (*((volatile unsigned long *)(0x40001000 + 0x08 )))
#define SCISCNT              (*((volatile unsigned long *)(0x40001000 + 0x0C )))
#define SCITXFCR             (*((volatile unsigned long *)(0x40001000 + 0x10 )))
#define SCIRXFCR             (*((volatile unsigned long *)(0x40001000 + 0x14 )))
#define SCITA1               (*((volatile unsigned long *)(0x40001000 + 0x18 )))
#define SCIS60CON            (*((volatile unsigned long *)(0x40001000 + 0x1C )))


/********************************************************************************
                             RNG Regsiter
********************************************************************************/
#define RNG1STR              (*((volatile unsigned long *)(0x40002800 + 0x30 )))
#define RNG1OUT              (*((volatile unsigned long *)(0x40002800 + 0x34 )))
#define RNG1TRNGTST          (*((volatile unsigned long *)(0x40002800 + 0x100)))
#define RNG1TRNGTDO          (*((volatile unsigned long *)(0x40002800 + 0x104)))

/********************************************************************************
                             GPIO Regsiter
********************************************************************************/
#define GPIODATA              (*((volatile unsigned long *)(0x40003000 + 0x00 )))
#define GPIODIR               (*((volatile unsigned long *)(0x40003000 + 0x04 )))


/********************************************************************************
                             PPB Regsiter
********************************************************************************/
#define ACTLR                 (*((volatile unsigned long *)( 0xE000E008	)))
#define NVIC_ISER             (*((volatile unsigned long *)( 0xE000E100	)))
#define NVIC_ICER             (*((volatile unsigned long *)( 0xE000E180	)))
#define NVIC_ISPR             (*((volatile unsigned long *)( 0xE000E200	)))
#define NVIC_ICPR             (*((volatile unsigned long *)( 0xE000E280	)))
#define NVIC_IPR0             (*((volatile unsigned long *)( 0xE000E400	)))
#define NVIC_IPR1             (*((volatile unsigned long *)( 0xE000E404	)))
#define NVIC_IPR2             (*((volatile unsigned long *)( 0xE000E408	)))
#define NVIC_IPR3             (*((volatile unsigned long *)( 0xE000E40C	)))
#define NVIC_IPR4             (*((volatile unsigned long *)( 0xE000E410	)))
#define NVIC_IPR5             (*((volatile unsigned long *)( 0xE000E414	)))
#define NVIC_IPR6             (*((volatile unsigned long *)( 0xE000E418	)))
#define NVIC_IPR7             (*((volatile unsigned long *)( 0xE000E41C	)))
#define CPUID                 (*((volatile unsigned long *)( 0xE000ED00	)))
#define ICSR                  (*((volatile unsigned long *)( 0xE000ED04	)))
#define VTOR                  (*((volatile unsigned long *)( 0xE000ED08	)))
#define AIRCR                 (*((volatile unsigned long *)( 0xE000ED0C	)))
#define SCR                   (*((volatile unsigned long *)( 0xE000ED10	)))
#define CCR                   (*((volatile unsigned long *)( 0xE000ED14	)))
#define SHPR2                 (*((volatile unsigned long *)( 0xE000ED1C	)))
#define SHPR3                 (*((volatile unsigned long *)( 0xE000ED20	)))
#define SFCR                  (*((volatile unsigned long *)( 0xE000EF90	)))

/********************************************************************************
                             Interrupt Source
********************************************************************************/
#define     IRQ_TMR0                0
#define     IRQ_TMR1                1
#define     IRQ_RNG1                7
#define     IRQ_7816TX              9
#define     IRQ_7816RX              10
#define     IRQ_7816FIFO            11

#endif  /* End of __IO_CIU98428F_H */
