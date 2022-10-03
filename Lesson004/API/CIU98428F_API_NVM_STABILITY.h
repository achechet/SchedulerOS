/*****************************************************
 Copyright(C),CEC Huada Electronic Design Co.,Ltd.
 
 File name: CIU98428F_API_NVM_STABILITY.h
 Author:    	
 Version:       	
 Date:      	
 Description:   
 History:
                
******************************************************/
#ifndef __CIU98428F_API_NVM_STABILITY_H_
#define __CIU98428F_API_NVM_STABILITY_H_

extern unsigned int	G_fCData1;
extern unsigned int	G_fCData2;
extern unsigned int	G_fCData3;

void openWrSwitch(void);
void closeWrSwitch(void);

unsigned int HED_FlashErasePage_St(unsigned int FlashAddr);
void HED_FlashErasePageNoChk_St(unsigned int FlashAddr);
void HED_FlashEraseSectorNoChk_St(unsigned int FlashAddr);
unsigned int HED_FlashWritePage_St (unsigned int FlashAddr, unsigned int * DataBuf);
unsigned int HED_FlashWriteWord_St (unsigned int FlashAddr, unsigned int DataValue);

#endif   
