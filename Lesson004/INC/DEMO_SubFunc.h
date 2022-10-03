/*****************************************************
 Copyright(C),CEC Huada Electronic Design Co.,Ltd.
 
 File name: 	DEMO_SCI_SubFunc.h
 Author:    	Wangyun
 Version:			V1.0
 Date:  			2013-03-19
 Description: 
 History:	
							V1.0	2013-04-07	initial version
******************************************************/
#ifndef __DEMO_SCI_SUBFUNC_H_
#define __DEMO_SCI_SUBFUNC_H_

extern INT16U G_SendLen;      //APDU send len 
extern INT16U G_SW1SW2;
extern INT8U G_FiDi;
extern INT8U G_APDUBuf[261];

void DemoCos_DelayNETU(unsigned int nETU);
void DEMOCOS_PPS(void);
void DemoCos_SendINS(unsigned char ins);
void openSend60(void);
void closeSend60(void);

#endif
