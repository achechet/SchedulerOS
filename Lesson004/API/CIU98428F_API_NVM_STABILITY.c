/*****************************************************
 Copyright(C),CEC Huada Electronic Design Co.,Ltd.
 
 File name: CIU98428F_API_NVM_STABILITY.c
 Author:    	
 Version:       
 Date:      	
 Description:   NVM operation function
 History:	
                
******************************************************/
#include "CIU98428F_SFR_C.H"

#define PAGE_SIZE 		        0x200

#define NVMEPKEY	 			0x57AF6C00
#define NVM_SECTOR_ERASE		0x09		//block erase
#define NVM_PAGE_ERASE			0x0A		//page erase
#define NVM_WORD_WRITE			0x0E		//word write
#define NVM_PAGE_WRITE			0x0F		//page write

#define SWITCHTAG1	0xAA55AA55
#define SWITCHTAG2 	0x55AA55AA
#define SWITCHTAG3 	0xA5A55A5A

#define NVM_SUCCESS 	0
#define NVM_FAILED		0xFFFFFFFF

unsigned int	G_fCData1;
unsigned int	G_fCData2;
unsigned int	G_fCData3;

extern void Reset_Handler(void);
extern void setWrMode(unsigned int wrMode);

/************************************************* 
  Function:       closeWrSwitch() 
  Description:   close NVM operation's switch
  Calls:          
  Called By:      
  Input:          none
  Output:         none 
  Return:         none
  Others:         none 
*************************************************/ 
void closeWrSwitch(void)
{ 	// close NVM operation's switch
	G_fCData1 = 0;
	G_fCData2 = 0;
	G_fCData3 = 0;	
}
/************************************************* 
  Function:       openWrSwitch() 
  Description:     open NVM operation's switch
  Calls:          
  Called By:      
  Input:          none
  Output:         none 
  Return:         none
  Others:         none 
*************************************************/ 
void openWrSwitch(void)
{ 	// open NVM operation's switch
	
	G_fCData1 = SWITCHTAG1;	
	G_fCData2 = SWITCHTAG2;
	G_fCData3 = SWITCHTAG3;	
}
/************************************************* 
  Function:       chkWrSwitch() 
  Description:     check NVM operation's switch
  Calls:          
  Called By:      
  Input:          none
  Output:         none 
  Return:         none
  Others:         none 
*************************************************/ 
void chkWrSwitch(void)
{ 	// check NVM operation's switch
	
	if ((G_fCData1 == SWITCHTAG1) && (G_fCData2 == SWITCHTAG2)
	        && (G_fCData3 == SWITCHTAG3))
	{		
		;//none
	}
	else
	{
		Reset_Handler();				
	}	
}

/************************************************* 
  Function:   	void wr_SDPKey(void) 
  Description:	write SDP Key of NVM
  Calls:          
  Called By:      
  Input:      	none
  Output:    	none 
  Return:     	none
  Others:     	none 
*************************************************/ 
void wr_SDPKey(void)
{
	SYSNVMSDPK1 = SWITCHTAG1;
	SYSNVMSDPK2 = SWITCHTAG2;
	SYSNVMSDPK3 = SWITCHTAG3;
	return;
}

/************************************************* 
  Function:   	unsigned int HED_FlashErasePage_St(unsigned int FlashAddr)
  Description:	Flash page erase with check
  Calls:          
  Input:       	FlashAddr: Flash address in erase page.
  Output:		none          
  Return:		0: sucess
				0xffffffff: failed
  Others:          
*************************************************/ 
unsigned int HED_FlashErasePage_St(unsigned int FlashAddr)
{
	unsigned int i;
	
	FlashAddr &= ~0x1FF;//page aligned

	__disable_irq();
	
	wr_SDPKey();
	*(unsigned int *)FlashAddr = 0xFFFFFFFF;
	setWrMode(NVMEPKEY | NVM_PAGE_ERASE);

	__enable_irq();
	
	for(i = 0; i < PAGE_SIZE/4; i++)
	{
		if(*((unsigned int*)FlashAddr + i) != 0xFFFFFFFF)
		{
			return NVM_FAILED;
		}
	}
	return NVM_SUCCESS;
}

/************************************************* 
  Function:   	void HED_FlashErasePageNoChk_St(unsigned int FlashAddr)
  Description:	Flash page erase without check
  Calls:          
  Input:       	FlashAddr: Flash address in erase page.
  Output:		none          
  Return:		none
  Others:          
*************************************************/ 
void HED_FlashErasePageNoChk_St(unsigned int FlashAddr)
{
	FlashAddr &= ~0x1FF;//page aligned
	__disable_irq();
	wr_SDPKey();
	*((unsigned int *)FlashAddr) = 0xFFFFFFFF;
	setWrMode(NVMEPKEY | NVM_PAGE_ERASE);
	__enable_irq();
}

/************************************************* 
  Function:   	void HED_FlashEraseSectorNoChk_St(unsigned int FlashAddr)
  Description:	Flash sector erase without check
  Calls:          
  Input:       	FlashAddr: Flash address in erase sector.
  Output:		none          
  Return:		none
  Others:          
*************************************************/ 
void HED_FlashEraseSectorNoChk_St(unsigned int FlashAddr)
{
	FlashAddr &= ~0xFFF;//sector aligned
	__disable_irq();
	wr_SDPKey();
	*((unsigned int *)FlashAddr) = 0xFFFFFFFF;
	setWrMode(NVMEPKEY | NVM_SECTOR_ERASE);
	__enable_irq();
}

/************************************************* 
  Function:   	unsigned int FlashWriteNWord_St (unsigned int FlashAddr, unsigned int * DataBuf, unsigned int DataLen)
  Description:	Flash write N words in half page
  Calls:          
  Input:       	FlashAddr: Flash write address, must be 4bytes align;
				DataBuf: data address which would be write to Flash, must be 4bytes align;
				DataLen: word length of write data
  Output:		none          
  Return:		0: sucess
				0xffffffff: failed
  Others:          
*************************************************/  
unsigned int FlashWriteNWord_St (unsigned int FlashAddr, unsigned int * DataBuf, unsigned int DataLen)
{
	unsigned int i;
	
	FlashAddr &= ~0xFF;	//half page aligned

	__disable_irq();
	
	wr_SDPKey();
	SYSNVMPBSTADDR = (unsigned int)DataBuf;
	SYSNVMPBLEN = DataLen - 1;
	*((unsigned int *)FlashAddr) = 0xFFFFFFFF;
	setWrMode(NVMEPKEY | NVM_PAGE_WRITE);
	
	__enable_irq();
	
	for(i = 0; i < DataLen; i++)
	{
		if(*((unsigned int*)FlashAddr + i) != *(DataBuf + i))
		{
			return NVM_FAILED;
		}
	}
	return NVM_SUCCESS;
}

/************************************************* 
  Function:   	unsigned int HED_FlashWritePage_St (unsigned int FlashAddr, unsigned int * DataBuf)
  Description:	Flash page write
  Calls:       	HED_FlashWriteNWord_St
  Input:       	FlashAddr: Flash write address, must be 4bytes align;
				DataBuf: data address which would be write to Flash, must be 4bytes align.	
  Output:		none          
  Return:		0: sucess
				0xffffffff: failed
  Others:          
*************************************************/  
unsigned int HED_FlashWritePage_St (unsigned int FlashAddr, unsigned int * DataBuf)
{
	FlashAddr &= ~0x1FF;//page aligned
	
	if(NVM_SUCCESS != FlashWriteNWord_St(FlashAddr, DataBuf, PAGE_SIZE/8))
		return NVM_FAILED;
	if(NVM_SUCCESS != FlashWriteNWord_St(FlashAddr+PAGE_SIZE/2, DataBuf+PAGE_SIZE/8, PAGE_SIZE/8))
		return NVM_FAILED;
	
	return NVM_SUCCESS;
}


/************************************************* 
  Function:   	unsigned int HED_FlashWriteWord_St (unsigned int FlashAddr, unsigned int DataValue)
  Description:	Flash write one word
  Calls:          
  Input:       	FlashAddr: Flash write address, must be 4bytes align;
				DataValue: one word data;
  Output:		none          
  Return:		0: sucess
				0xffffffff: failed
  Others:          
*************************************************/  
unsigned int HED_FlashWriteWord_St (unsigned int FlashAddr, unsigned int DataValue)
{
	__disable_irq();
 	wr_SDPKey();
	*(unsigned int *)FlashAddr = DataValue;
	setWrMode(NVMEPKEY | NVM_WORD_WRITE);
	__enable_irq();

    if (*((unsigned int *)FlashAddr) != DataValue)
    		return NVM_FAILED;
	return NVM_SUCCESS;  
}
