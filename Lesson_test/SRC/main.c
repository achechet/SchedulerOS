/*****************************************************
 Copyright(C),CEC Huada Electronic Design Co.,Ltd.
 
 File name: 	main.c
 Author:    	Wangyun
 Version:		V1.0
 Date:  		2014-03-19
 Description: 
 History:	
				V1.0	2014-01-27	initial version
******************************************************/
#include  "../inc/declare.h" 

INT8U 	G_ATR[] = {0x09,0x3B,0x16,0x96,0x81,0x18,0x03,0xDD,0xB0,0x10};//LV Format:Length + Value
INT16U 	G_SendLen;      //APDU send len 
INT16U 	G_SW1SW2;
INT8U	G_APDUBuf[261];
INT8U 	G_FiDi;
/************************************************* 
  Function:      main
  Description:    
  Input:           
  Output:         
  Return:        
  Others:      
*************************************************/
int main(void)
{ 
    closeWrSwitch();//disable EE operation
    
	SYSCLKCFG = (0x01<<12) | (0x04 << 4);	//APB = AHB div 2; cpu = 15MHz
     
	G_FiDi = 0x11;//user can init some global value here,or enable some SECURITY MODULE
	SYSCLKEN |= (1<<17) | (1<<0);//open SCI/CRC IP's clk

	SCISCON = (1<<6) | (1<<4);//SCI Enable,12etu
	HED_T0_SendNBytes(G_ATR+1,G_ATR[0]);//send ATR during(400clk,40000clk) after rst
        
	while(1)
	{
		HED_SetToStandby();
		HED_T0_ReceiveNBytesRam(G_APDUBuf,1);
		if (G_APDUBuf[0] == 0xFF)
		{
			DEMOCOS_PPS();
		}
		else
		{
			HED_T0_ReceiveNBytesRam (G_APDUBuf+1, 4);             
			
			openWrSwitch();//enable EE operation
            G_SW1SW2 = 0x6D00;    
			DemoCos_APDU_Dispatch();            
			closeWrSwitch();//disable EE operation
                
			DemoCos_DelayNETU(2);
			HED_T0_SendNBytes(G_APDUBuf,G_SendLen);
		}            
	}
}
