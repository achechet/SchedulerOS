/*****************************************************
 Copyright(C),CEC Huada Electronic Design Co.,Ltd.
 
 File name: CIU98428F_API_Base_NoNVM.c
 Author:    
 Version:	
 Date:      
 Description:   CIU98428F User API Function
 History:	

******************************************************/
#include "CIU98428F_SFR_C.H"

#define FACTORY_CODE_SADDR 		0x1FFFFE00

#define STDBY_IS_VALID                      (((SCISSR & 0x08) == 0) && ((SCIRXFCR & 0x02) != 0))
/* MACRO DEFINE */
/* 7816 Interface */
#define SCI_START_SETU_COUNTER 				SET_REG_BIT(SCISSR,SCISSR_SETU)
#define SCI_START_SETU_COUNTER_IS_RUNNING 	REG_BIT_IS_SET(SCISSR,SCISSR_SETU)
#define SCI_IS_BUSY 					    (SCISSR & ((1 << SCISSR_RB)|(1 << SCISSR_TB)))
#define CLEAR_SCI_AUTO_RETRY_LIMIT 		    (SCISCON &= ~(3 << SCISCON_RCNT_0))
#define SET_SCI_AUTO_RETRY_LIMIT(num) 	    (SCISCON |= ((num - 2) & 0x3))
#define ENABLE_SCI 						    SET_REG_BIT(SCISCON,SCISCON_SE)
#define DISABLE_SCI 					    CLEAR_REG_BIT(SCISCON,SCISCON_SE) 
/*7816 send data*/
#define SCI_DATA_IS_SENDING 			    REG_BIT_IS_CLEAR(SCISSR,SCISSR_TI)
#define CLEAR_SCI_TI 					    CLEAR_REG_BIT(SCISSR,SCISSR_TI)
#define SCI_PARITY_IS_ERROR 			    REG_BIT_IS_SET(SCISSR,SCISSR_PE)
#define CLEAR_SCI_PARITY_ERROR 			    CLEAR_REG_BIT(SCISSR,SCISSR_PE)
#define FLUSH_SCI_TX_FIFO 				    SET_REG_BIT(SCITXFCR,SCITXFCR_FLUSH_TX_FIFO) 
#define FLUSH_SCI_TX_FIFO_IS_RUNNING 	    REG_BIT_IS_SET(SCITXFCR, SCITXFCR_FLUSH_TX_FIFO)
/*7816 receive data*/
#define CLEAR_SCI_RI 					    CLEAR_REG_BIT(SCISSR,SCISSR_RI)
#define SCIRXFCR_RX_IS_EMPTY  			    REG_BIT_IS_SET(SCIRXFCR,SCIRXFCR_RX_EMPTY)

/* Random */
#define ENABLE_RNG1_CLK 		            SET_REG_BIT(SYSCLKEN,SYSCLKEN_RNG1EN)
#define DISABLE_RNG1_CLK 		            SET_REG_BIT(SYSCLKCLR,SYSCLKEN_RNG1EN)
#define RANDOM_NUM_IS_OK 		            REG_BIT_IS_SET(RNG1STR,RNG1STR_RNGRDY)
#define RANDOM_NUM_IS_NOT_OK 	            REG_BIT_IS_CLEAR(RNG1STR,RNG1STR_RNGRDY) 
#define RANDOM_NUM_IS_TIMEOUT 	            REG_BIT_IS_SET(RNG1STR,RNG1STR_TIMEOUT)

#define SET_REG_BIT(reg,bit) 			    (reg |= (1 << (bit)))
#define CLEAR_REG_BIT(reg,bit) 			    (reg &= ~(1 << (bit)))
#define REG_BIT_IS_SET(reg,bit) 		    (reg & (1 << (bit)))
#define REG_BIT_IS_CLEAR(reg,bit) 		    ((reg & (1 << (bit))) == 0)
#define SET_BIT(bit) 					    (0x1 << (bit))
#define CLEAR_BIT(bit) 					    (~(0x1 << (bit)))

//SCI SFR
//SCISCON bits
typedef enum 
{
	SCISCON_RCNT_0 = 0,
	SCISCON_RCNT_1 = 1,
	SCISCON_PARITY = 2 ,
	SCISCON_IVC = 3,
	SCISCON_EGB = 4,
	SCISCON_PS = 5,
	SCISCON_SE = 6,
    SCISCON_WTC = 7,
    SCISCON_AWTS = 8,
    SCISCON_SAFB = 10,
    SCISCON_RTS = 11,
} SCISCON_BITS;

//SCISSR bits
typedef enum 
{
	SCISSR_OE = 0,
	SCISSR_PE = 1,
	SCISSR_RI = 2 ,
	SCISSR_RB = 3,
	SCISSR_TI = 4,
	SCISSR_TB = 5,
	SCISSR_SETU = 6,
} SCISSR_BITS;

typedef enum 
{
	SCIRXFCR_FLUSH_RX_FIFO = 0,
	SCIRXFCR_RX_EMPTY = 1,
	SCIRXFCR_WATER_LEVEL = 2 ,
} SCIRXFCR_BITS;

typedef enum 
{
	SCITXFCR_FLUSH_TX_FIFO = 0,
	SCITXFCR_TX_FULL = 1,
} SCITXFCR_BITS;

//RNG
typedef enum 
{
	RNG1STR_RNGRDY = 0,
	RNG1STR_TIMEOUT = 1,
}  RNG1STR_BITS;

//CRC
typedef enum
{
	CRCCSR_CRC_ST = 0,
	CRCCSR_INPUT_REV = 1,
	CRCCSR_OUTPUT_REV = 2,
	CRCCSR_OUTPUT_NOT = 3,
	CRCCSR_WIDTH_0 = 4,
	CRCCSR_WIDTH_1 = 5,
} CRCCSR_BITS;	

/* 7816 Interface */
/************************************************* 
  Function:      HED_T0_SendNBytes (unsigned char * DataAddr , unsigned int DataLen)
  Description:    
		send N bytes by 7816 interface(T = 0 protocol)			
  Calls: none    
  Input: 
		DataAddr:address of the data to be sent
		DataLen:lengtn of the data to be sent
  Output:нч
  Return:
		0x00:data sent successfully
		0x01:data sent fail
		0x02:API's para illegal  
  Others:none
*************************************************/ 
unsigned int HED_T0_SendNBytes (unsigned char * DataAddr , unsigned int DataLen)
{
	if(DataLen == 0) 
	{
		return (2);
	}

	//wait for idle time
	while(SCI_IS_BUSY); 

	//set auto retry time = 3
	CLEAR_SCI_AUTO_RETRY_LIMIT; 
	SET_SCI_AUTO_RETRY_LIMIT(3);

	while(DataLen--)
	{
		//write one byte to buffer a time
		SCISBUF = (*DataAddr++);
		while(SCI_DATA_IS_SENDING);

		if(SCI_PARITY_IS_ERROR == 0 )
		{
			SCISSR = 0;							//clr SCI status
		}
		else
		{
			SCITXFCR = 0x01;					//clr trans FIFO control
			while ((SCITXFCR & 0x01) == 0x01);
			SCISSR = 0;							//clr SCI status
			return (1);    	
		}
	}
	return (0);
}
/************************************************* 
  Function:     HED_T0_ReceiveNBytesRam(unsigned char *DataAddr, unsigned int DataLen)
  Description:    
		receive N bytes to RAM by 7816 interface(T = 0 protocol)			
  Calls: none              
  Input: 
		DataAddr:address to store the data received
		DataLen:bytes number of the data received
  Output:data received
  Return:none
  Others:none
*************************************************/ 
void HED_T0_ReceiveNBytesRam(unsigned char *DataAddr, unsigned int DataLen)
{
	while(SCI_IS_BUSY);

	while(DataLen--)
	{	 	      
		//wait for data
		while(SCIRXFCR_RX_IS_EMPTY);
		(*DataAddr++) = (unsigned char)SCISBUF; 
	}
}

/* CRC */
#define CHANGE_CRC_WIDTH_FROM_32_TO_8  (CRCCSR |= (1<<5))
/************************************************* 
  Function:     unsigned short HED_CRC16(unsigned short IV, unsigned int DataLen, unsigned char *DataBuf, unsigned char type)
  Description:    
		Calculate the CRC-16 of a block of bytes in RAM/EE			
  Calls:              
  Input: 
		DataLen:bytes number of the block of
		IV: initial value
		DataBuf: the address of the block of data to be calculate 
		type: input and output data order type
  Output:none
  Return:2 bytes CRC result (big endian)
  Others:none 
*************************************************/ 
unsigned short HED_CRC16(unsigned short IV, unsigned int DataLen, unsigned char *DataBuf, unsigned char type)
{
	unsigned int tmp_cnt = (unsigned int)DataBuf & 0x3; //distance to previous word border
	unsigned int div4_quotient, div4_mod;   //

	if(DataLen == 0)
	{
		return IV;
	}
	
	CRCIV = IV;
	CRCCSR = ((type&0x3)<<1) | SET_BIT(CRCCSR_CRC_ST);

	if (DataLen <= 0x07)
	{
		div4_quotient = 0;
		div4_mod = DataLen;   
	}
	else
	{
		unsigned int curr_crccsr = CRCCSR;
		if (tmp_cnt)
		{
			tmp_cnt = 0x4 - tmp_cnt;
			DataLen -= tmp_cnt;
			CHANGE_CRC_WIDTH_FROM_32_TO_8;   //CRCCSR[5:4] = 10 ->8
			while (tmp_cnt--)
			{
				CRCDATA = *DataBuf++;
			}
		}
		div4_quotient = DataLen >> 0x02; 
		div4_mod = DataLen & 0x3;   //word numbers
		CRCCSR = curr_crccsr;

		if (div4_quotient)
		{
			unsigned int * tmp_databuf = (unsigned int *)DataBuf;
			while (div4_quotient--)
			{
				CRCDATA = *(unsigned int *)tmp_databuf++;//4 bytes one time
			}
			DataBuf = (unsigned char *)tmp_databuf;
		}
	}

	if (div4_mod)	
	{
		CHANGE_CRC_WIDTH_FROM_32_TO_8;   //CRCCSR[5:4] = 10 ->8
		while (div4_mod--)
		{
			CRCDATA = *DataBuf++;
		}

	}

	return (unsigned short)CRCDATA;
}

/* Serial Num */
void HED_GetChipInfo(unsigned char *DataBuf, unsigned int FlashAddr, unsigned int DataLen)
{
	while(DataLen--)
	{
		*DataBuf++ = *(unsigned char *)FlashAddr;
		FlashAddr += 0x00000001;
	}
} 
/* Chip ID */
/************************************************* 
  Function:     void HED_GetChipId(unsigned char *ChipId)
  Description:  to get chip ID		
  Calls:              
  Input: ChipId:address to store the chip ID
  Output:none
  Return:
  Others:none
*************************************************/ 
void HED_GetChipId(unsigned char *ChipId)
{
    unsigned char die_YX_position;
    unsigned char test_month;
    
    HED_GetChipInfo(ChipId,  FACTORY_CODE_SADDR+0x09, 1);	//Test year
    
    /*get 2nd sequence number*/
    HED_GetChipInfo(&die_YX_position,  FACTORY_CODE_SADDR+0x17, 1);	//get die X/Y position on wafer
    HED_GetChipInfo(&test_month,  FACTORY_CODE_SADDR+0x0A, 1);	//get test_month
    die_YX_position = ((die_YX_position & 0x10) << 3) | ((die_YX_position & 0x01) << 6);    
    test_month = (test_month >> 4)*10 + (test_month & 0x0F);
    test_month &= 0x3F;
    ChipId[1] = die_YX_position | test_month;
    /*get 2nd sequence number*/
    
    HED_GetChipInfo(ChipId+2,  FACTORY_CODE_SADDR+0x1C, 1);	//lot number (highest byte)
    HED_GetChipInfo(ChipId+3,  FACTORY_CODE_SADDR+0x1D, 1);	//lot number (second byte)
    HED_GetChipInfo(ChipId+4,  FACTORY_CODE_SADDR+0x1E, 1);	//lot number (third byte)
    HED_GetChipInfo(ChipId+5,  FACTORY_CODE_SADDR+0x1F, 1);	//lot number (fourth byte)
    HED_GetChipInfo(ChipId+6,  FACTORY_CODE_SADDR+0x0B, 1);	//lot number (fifth byte)
    HED_GetChipInfo(ChipId+7,  FACTORY_CODE_SADDR+0x0C, 1);	//lot number (lowest byte)
    HED_GetChipInfo(ChipId+8,  FACTORY_CODE_SADDR+0x1A, 1);	//lot subsidiary number (high byte)
    HED_GetChipInfo(ChipId+9,  FACTORY_CODE_SADDR+0x1B, 1);	//lot subsidiary number (low byte)
    HED_GetChipInfo(ChipId+10,  FACTORY_CODE_SADDR+0x0D, 1); //wafer number
    HED_GetChipInfo(ChipId+11,  FACTORY_CODE_SADDR+0x0E, 1); //die X position on wafer
    HED_GetChipInfo(ChipId+12,  FACTORY_CODE_SADDR+0x0F, 1); //die Y position on wafer
}

/* Factory Code */
/************************************************* 
  Function:     HED_GetFactoryCode(unsigned char *FactoryCode)
  Description: to get 32 bytes long FactoryCode	
  Calls:               
  Input: address to store the FactoryCode
  Output:none
  Return:
  Others: 
*************************************************/ 
void HED_GetFactoryCode(unsigned char *FactoryCode)
{
	HED_GetChipInfo(FactoryCode, FACTORY_CODE_SADDR, 32);
}


/************************************************* 
  Function:     HED_GetRandom(unsigned char *RandomData,unsigned int DataLen)
  Description:	to get real random data 			
  Calls:               
  Input: 
		RandomData:the address in RAM to store the generated random data 
		DataLen:bytes number of the random data
  Output:real random data in RAM
  Return:none
  Others:none
*************************************************/ 
void HED_GetRandom(unsigned char *RandomData,unsigned int DataLen)
{
	unsigned int div4_quotient, div4_mod;
	unsigned int *tmp_databuf;	
	unsigned int bakSYSCLKEN;
	unsigned int tmp_cnt = (unsigned int)RandomData & 0x3; //distance to previous word border

	bakSYSCLKEN = SYSCLKEN;    
	SYSCLKEN |= (1<<8); 
	
	if(RANDOM_NUM_IS_TIMEOUT)
	{
		CLEAR_REG_BIT(RNG1STR,RNG1STR_TIMEOUT);
	}

	if (DataLen <= 0x07)
	{
		div4_quotient = 0;
		div4_mod = DataLen;
	}
	else
	{
		if (tmp_cnt)
		{
			tmp_cnt = 0x4 - tmp_cnt;
			DataLen -= tmp_cnt;
			while (tmp_cnt--)
			{
				while(RANDOM_NUM_IS_NOT_OK);
				*(RandomData++) = (unsigned char)RNG1OUT;
			}
		}
		div4_quotient = DataLen >> 0x02; 
		div4_mod = DataLen & 0x3;//word numbers

		if (div4_quotient)
		{
			tmp_databuf = (unsigned int *)RandomData;
			while (div4_quotient--)
			{
				while(RANDOM_NUM_IS_NOT_OK);
				*(tmp_databuf++) = RNG1OUT;
			}
			RandomData = (unsigned char *)tmp_databuf;
		}
	}

	if (div4_mod)	
	{
		while (div4_mod--)
		{
			while(RANDOM_NUM_IS_NOT_OK);
			*(RandomData++) = (unsigned char)RNG1OUT; 
		}
	}
	SYSCLKCLR = (~bakSYSCLKEN);
}


/*delay function
PreFunction Instruction:

MOVS    r0,0xxx;    //1 cycle
BL      delay;      //4 cycle

InFunction Instruction:
loopStart
SUBS     r0,r0,#1;  //1 cycle
BCS      loopStart; //1 or 3 cycle
BX		    lr;     //3 cycle

LoopNum == 0:   delay == 10 cycle
LoopNum == 1:   delay == 14 cycle
LoopNum == 2:   delay == 18 cycle
.........
LoopNum == n:   delay == (10 + 4*n) cycle
*/
__asm void delay(unsigned int LoopNum)
{
loopStart
    SUBS     r0,r0,#1
    BCS      loopStart
	BX		    lr
}

/* Set standby Mode */
/************************************************* 
  Function:     HED_SetToStandby(void)
  Description:  set chip to standby mode		
  Calls:              
  Input: none
  Output:none
  Return:
  Others: both SYSCLKCFG and SYSCLKEN remain the same before and after the execution of this API
*************************************************/ 
void HED_SetToStandby(void)
{
	unsigned int clkBak;
	
    /* 1. change clock to 7.5MHz */
	clkBak = SYSCLKCFG;
	if((clkBak & 0x70) == (0x30))
	{	
		SYSCLKCFG = (0x04 << 4);	//cpu = 15MHz
	}
	SYSCLKCFG = (0x05 << 4);	//cpu = 7.5MHz
	
	/* 2. entry standby */
	if(STDBY_IS_VALID)
    {
		__sev();	//set event reg
		__wfe();	//clr event reg	
		__wfe();	//set standby
    }
	
	/* 3. retore clock config */
	if((clkBak & 0x70) == (0x30))
	{	
		SYSCLKCFG = (0x04 << 4);	//cpu = 15MHz
	}
	SYSCLKCFG = clkBak;		
}

