/*****************************************************
 Copyright(C),CEC Huada Electronic Design Co.,Ltd.
 
 File name: 	DEMO_SCI_SubFunc.h
 Author:    	Xuews
 Version:			V1.0
 Date:  			2013-04-07
 Description: 
 History:	
							V1.0	2013-04-07	initial version
******************************************************/
#ifndef __DEMO_APDU_H_
#define __DEMO_APDU_H_

#define LOADER_START_ADDR	0x69000

void cmd_INSError(void);
void Get_APDUResp(void);
void DemoCos_APDU_Dispatch(void);
void cmd_Symmetry_ALG(void);
void cmd_CRC_Calculate(void);
void cmd_GET_INFO(void);
void cmd_NVM_Erase(void);
void cmd_NVM_Write(void);
void cmd_Loader_Resume(void);

#endif
