/*****************************************************
 Copyright(C),CEC Huada Electronic Design Co.,Ltd.
 
 File name: CIU98428F_API_Base_NoNVM.h
 Author:    
 Version:
 Date:  
 Description:   CIU98428F User API Function header file
 History:

******************************************************/


#ifndef __CIU98428F_API_BASE_NONVM_H_
#define __CIU98428F_API_BASE_NONVM_H_

/* API FUCTION DEFINE */
/* 7816 Interface */
unsigned int HED_T0_SendNBytes(unsigned char * DataAddr, unsigned int DataLen);
void HED_T0_ReceiveNBytesRam(unsigned char * DataAddr, unsigned int DataLen); 
/* CRC */
unsigned short HED_CRC16(unsigned short IV, unsigned int DataLen, unsigned char *DataBuf, unsigned char type);
/* Chip ID */
void HED_GetChipId(unsigned char *ChipId);
/* Factory Code */
void HED_GetFactoryCode(unsigned char * FactoryCode);
/* Set standby Mode */
void HED_SetToStandby(void);
/* get random number */
void HED_GetRandom(unsigned char *RandomData,unsigned int DataLen);

#endif   /* End of "__CIU98428F_API_BASE_NONVM_H_" */
