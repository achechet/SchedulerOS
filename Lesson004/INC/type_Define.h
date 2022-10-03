/*****************************************************
 Copyright(C),CEC Huada Electronic Design Co.,Ltd.
 
 File name: 	type_define.h
 Author:    	Xuews
 Version:			V1.0
 Date:  			2013-04-07
 Description: 
 History:	
							V1.0	2013-04-07	initial version
******************************************************/

#ifndef    _TYPE_DEFINE_H_
#define	   _TYPE_DEFINE_H_

/* 1. define data type  */

typedef     unsigned char           INT8U;
typedef     unsigned short          INT16U;
typedef     unsigned int            INT32U;
/* Type Definition */
#define INT8U   unsigned char
#define INT16U  unsigned short
#define INT32U  unsigned int

typedef     unsigned char   bool;

#define BIT0    (1ul<<0)
#define BIT1    (1ul<<1)
#define BIT2    (1ul<<2)
#define BIT3    (1ul<<3)
#define BIT4    (1ul<<4)
#define BIT5    (1ul<<5)
#define BIT6    (1ul<<6)
#define BIT7    (1ul<<7)
#define BIT8    (1ul<<8)
#define BIT9    (1ul<<9)
#define BIT10   (1ul<<10)
#define BIT11   (1ul<<11)
#define BIT12   (1ul<<12)
#define BIT13   (1ul<<13)
#define BIT14   (1ul<<14)
#define BIT15   (1ul<<15)
#define BIT16   (1ul<<16)
#define BIT17   (1ul<<17)
#define BIT18   (1ul<<18)
#define BIT19   (1ul<<19)
#define BIT20   (1ul<<20)
#define BIT21   (1ul<<21)
#define BIT24   (1ul<<24)
#define BIT26   (1ul<<26)
#define BIT30   (1ul<<30)
#define BIT31   (1ul<<31)  

#define 	DEMO_SET_BIT(x,y)  		(x |= (0x01 <<y))    
#define 	DEMO_CLR_BIT(x,y)  		(x &= (~(0x01 <<y))) 
#define 	DEMO_CHECK_BIT(x,y) 	(x & (0x01 <<y))   


/* 2. definedata structure */
/* 2.2 definedata structure of function pointer */
//define the function pointer structure for the APDU command
typedef struct 
{
    INT8U ins;
    void (*func)(void);
}COMMANDLIST;

typedef void (*DEMO_FUNCPTR_VOID)(void);
typedef void (*DEMO_FUNCPTR_PTR)(void *); 

#endif
