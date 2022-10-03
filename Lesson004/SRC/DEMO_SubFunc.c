
/*****************************************************
 Copyright(C),CEC Huada Electronic Design Co.,Ltd.
 
 File name: 	DEMO_SubFunc.c
 Author:    	Wangyun
 Version:		V1.0
 Date:  		2014-01-27
 Description: 
 History:	
				V1.0	2014-01-27	initial version
******************************************************/
#include  "../inc/declare.h" 

#define		b_SE_rd				(SCISCON & (1<<6))
#define 	MACRO_bSE_ENABLE	SCISCON |= (1<<6)
#define 	MACRO_bSE_DISABLE	SCISCON &= ~(1<<6)

#define PPS_PPSS G_APDUBuf[0]
#define PPS_PPS0 G_APDUBuf[1]
#define PPS_PPS1 G_APDUBuf[2]
#define PPS_PPS2 G_APDUBuf[3]
#define PPS_PPS3 G_APDUBuf[4]
#define PPS_PCK  G_APDUBuf[5]
#define PPS_NUM 5

static const  INT16U FD_table[PPS_NUM][3] = {{0x97,7,0x78},{0x11,371,0xfe},{0x18,30,0xf7},
						{0x94,63,0x7B},{0x96,15,0x79}}; //{FD,SFR_Value,xor_value}

/****************************************************************************
  Function:      openSend60(void)
  Description:    
  Input:           
  Output:         
  Return:        
  Others:                            
****************************************************************************/
void openSend60(void)
{
	DEMO_SET_BIT(SCIS60CON, 0);
}

/****************************************************************************
  Function:      closeSend60(void)
  Description:    
  Input:           
  Output:         
  Return:        
  Others:                            
****************************************************************************/
void closeSend60(void)
{
	DEMO_CLR_BIT(SCIS60CON, 0);
	while (SCISSR & (1<<5));
}

/****************************************************************************
  Function:      DemoCos_DelayNETU
  Description:    
  Input:           
  Output:         
  Return:        
  Others:                            
****************************************************************************/
void DemoCos_DelayNETU(unsigned int nETU)
{
	while(nETU)
	{
    	SCISSR |= BIT6;/* Begin 1ETU counter */
    	while((SCISSR & BIT6) != 0)
    	{
			;
    	}
		nETU--;
	}
}
/*************************************************
  Function:      DEMOCOS_PPS
  Description:    
  Input:           
  Output:         
  Return:        
  Others:                  
*************************************************/ 
void DEMOCOS_PPS(void)
{
	
	register INT32U len,pck;
	register INT32U i;
    INT8U  PPSFailRES[3] = {0xFF,0x00,0xFF};
    

	PPS_PPSS = 0xff;
	len = 1; 
    HED_T0_ReceiveNBytesRam(G_APDUBuf+1,1);
	for (i = 4; i<7; i++)
	{
		if (((PPS_PPS0 >> i) & 1) == 1)
			len++;
	}
	
	HED_T0_ReceiveNBytesRam(G_APDUBuf+2,len);

	/*get PCK */
	pck = 0x00;
	for (i = 0; i<(len+2); i++)
	{
		pck = pck ^ G_APDUBuf[i];
	}
	if (pck == 0)
	{
		DemoCos_DelayNETU(4);
		if ((PPS_PPS0 & 0x9F) == 0x10) 
		{		
			/* find FD */
			for (i = 0; i < PPS_NUM; i++)
			{
				if (PPS_PPS1 == FD_table[i][0])
				{
					/* send pps response */
					PPS_PPS0 = 0x10;
					PPS_PPS2 = FD_table[i][2];
					HED_T0_SendNBytes(G_APDUBuf,4);
					SCITA1 = FD_table[i][0];	
					return;
				}
			}
		}
		/* send pps err response */
        HED_T0_SendNBytes(&PPSFailRES[0],1);
        HED_T0_SendNBytes(&PPSFailRES[1],1);
        HED_T0_SendNBytes(&PPSFailRES[2],1);
	}
	else
	{
		/*means pps pck is err ,do nothing */
	}
}
/****************************************************************************
  Function:      DemoCos_SendINS(unsigned char ins)
  Description:    
  Input:           
  Output:         
  Return:        
  Others:                            
****************************************************************************/
void DemoCos_SendINS(unsigned char ins)
{
	HED_T0_SendNBytes(&ins,1);//send INS
}
