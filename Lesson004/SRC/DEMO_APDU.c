
/*****************************************************
 Copyright(C),CEC Huada Electronic Design Co.,Ltd.
 
 File name: 	DEMO_APDU.c
 Author:    	Wangyun
 Version:		V1.0
 Date:  		2014-01-27
 Description: 
 History:	
				V1.0	2014-01-27	initial version
******************************************************/
#include  "../inc/declare.h" 

#define EOL     0xFF

#define ECB_ENCRYPT             0x00
#define ECB_DECRYPT             0x40
#define CBC_ENCRYPT             0x20
#define CBC_DECRYPT             0x60
#define MAC_EXTPARA             0x84

#define ECB_ENCRYPT_ExtKey      0x04
#define ECB_DECRYPT_ExtKey      0x44

#define APDU_CLA    G_APDUBuf[0]
#define APDU_INS    G_APDUBuf[1]
#define APDU_P1     G_APDUBuf[2]
#define APDU_P2     G_APDUBuf[3]
#define APDU_P3     G_APDUBuf[4]

//command list
static const COMMANDLIST CMD_INSList[]=
{ 
	{0xF0, cmd_Symmetry_ALG},
 	{0xF2, cmd_CRC_Calculate},
  	{0xC6, cmd_GET_INFO},
  	{0xD2, cmd_NVM_Erase},
	{0xD6, cmd_NVM_Write},
	{0xD8, cmd_Loader_Resume},
  	{EOL, cmd_INSError},
};
/************************************************* 
  Function:       Get_APDUResp 
  Description:    
  Input:                        
  Output:         
  Return:         Null
  Others:         Null 
*************************************************/
void Get_APDUResp(void)
{
    *(G_APDUBuf + G_SendLen) = (INT8U)(G_SW1SW2 >> 8);
    *(G_APDUBuf + G_SendLen + 1) = (INT8U)(G_SW1SW2 & 0xFF);
    G_SendLen += 2;
}
/************************************************* 
  Function:      INSError
  Description:    
  Input:           
  Output:         
  Return:        
  Others:      
*************************************************/
void cmd_INSError(void)
{
	G_SW1SW2 = 0x6D00;
}

/************************************************* 
  Function:      DemoCos_APDU_Dispatch
  Description:    
  Input:           
  Output:         
  Return:        
  Others:      
*************************************************/
void DemoCos_APDU_Dispatch(void)
{
    INT8U i;
    
    G_SendLen = 0;
    
    for(i=0;;i++)
    {
    	if((CMD_INSList[i].ins == APDU_INS)||(CMD_INSList[i].ins == EOL))
    	{
    		CMD_INSList[i].func();
            Get_APDUResp();
    		break;
    	}                           			
    }
}

/************************************************* 
  Function:      cmd_Symmetry_ALG
  Description:    
  Input:           
  Output:         
  Return:        
  Others:      
*************************************************/
void cmd_Symmetry_ALG(void)
{
    INT32U ret;
    
    DES_Parameters * Demo_DES_Param;
    DES_Parameters  Demo_DES_ParamVar;
    DES3_Parameters  Demo_3DES_Param;


    /************************************************* 
        DES/3DES/SSF33 standard data
    *************************************************/
    INT32U  std_DES_Key[4] = {0x53D6A7A9,0x0D38BC86,\
                                0x13B88389,0x963E20CB};
    INT32U  std_DES_IV[2] = {0x3DCD8D1E,0x76F1233B};
    INT32U  std_DES_InData[8] = {0xF0E5AC7F,0x803571B0,0xA7408E0C,0x378C05AE,\
                                   0xE58732C7,0xCC28A34E,0x0B52EE7A,0x8214C162};
    
    INT32U  std_DES_ECB_EncData[8] = {0x1D1A9D5A,0x7BE7C2F0,0xDD3BFC0A,0x74DBA3C3,0x5DAFFB8C,0x87286F17,0xE36194F3,0x307DFAA3};
    INT32U  std_2Key3DES_CBC_EncData[8] = {0x8E507B1D,0x81EF13C6,0x56BCF427,0x8A35434D,0xA5DB5C40,0x04B9FE12,0x5A81FED3,0x82AC8DF7};                                          

	INT32U	chkSumVar;
	INT32U 	output[8];
     
    /*1. run DES ECB_ENC */
    //1-1.config DES struct para
    Demo_DES_Param = &Demo_DES_ParamVar;
    Demo_DES_Param->iDataPtr = std_DES_InData;
    Demo_DES_Param->oDataPtr = output;
    Demo_DES_Param->DataLen = sizeof(std_DES_InData);
    //Demo_DES_Param->IVPtr = std_DES_IV;
    Demo_DES_Param->iKeyPtr = std_DES_Key;
    Demo_DES_Param->Type = ECB_ENCRYPT;
	Demo_DES_Param->paraChkSum = &chkSumVar;
    
    //1-2.run DES 
    ret = HED_DESBlock(Demo_DES_Param);
    if((ret != DES_SUCCESS) || (chkSumVar != ((unsigned int)Demo_DES_Param->oDataPtr+Demo_DES_Param->DataLen)))
    {
        G_SW1SW2 = 0x6F01;
        return;
    }
    
    //1-3.check DES data
    ret = memcmp(Demo_DES_Param->oDataPtr, std_DES_ECB_EncData, sizeof(std_DES_ECB_EncData));
    if(ret != 0)
    {
        G_SW1SW2 = 0x6F02;
        return;
    } 
    
    /*2. run 3DES 2KEY CBC ENC /DEC */
    //2-1.config 3DES struct para
    Demo_3DES_Param.iDataPtr = std_DES_InData;
    Demo_3DES_Param.oDataPtr = output;
    Demo_3DES_Param.DataLen = sizeof(std_DES_InData);
    Demo_3DES_Param.IVPtr = std_DES_IV;
    Demo_3DES_Param.iKeyPtr = std_DES_Key;
    Demo_3DES_Param.Type = CBC_ENCRYPT;
	Demo_3DES_Param.paraChkSum = &chkSumVar;
    
    //2-2.run 3DES 
    ret = HED_DES3Block(&Demo_3DES_Param);
    if((ret != DES_SUCCESS) || (chkSumVar != ((unsigned int)Demo_3DES_Param.oDataPtr+Demo_3DES_Param.DataLen)))
    {
        G_SW1SW2 = 0x6F03;
        return;
    }
    
    //2-3.check 3DES data
    ret = memcmp(Demo_3DES_Param.oDataPtr, std_2Key3DES_CBC_EncData, sizeof(std_2Key3DES_CBC_EncData));
    if(ret != 0)
    {
        G_SW1SW2 = 0x6F04;
        return;
    } 
    
    //2-4.check 3DES data Dec
    Demo_3DES_Param.iDataPtr = std_2Key3DES_CBC_EncData;
    Demo_3DES_Param.oDataPtr = output;
    Demo_3DES_Param.DataLen = sizeof(std_2Key3DES_CBC_EncData);
    Demo_3DES_Param.IVPtr = std_DES_IV;
    Demo_3DES_Param.iKeyPtr = std_DES_Key;
    Demo_3DES_Param.Type = CBC_DECRYPT;
	Demo_3DES_Param.paraChkSum = &chkSumVar;
    
    ret = HED_DES3Block(&Demo_3DES_Param);
	if((ret != DES_SUCCESS) || (chkSumVar != ((unsigned int)Demo_3DES_Param.oDataPtr+Demo_3DES_Param.DataLen)))
    {
        G_SW1SW2 = 0x6F05;
        return;
    }
    ret = memcmp(Demo_3DES_Param.oDataPtr, std_DES_InData, sizeof(std_DES_InData));
    if(ret != 0)
    {
        G_SW1SW2 = 0x6F06;
        return;
    }    
    
    //return
    G_SW1SW2 = 0x9000;       
}
/************************************************* 
  Function:      cmd_CRC_Calculate
  Description:    
  Input:           
  Output:         
  Return:        
  Others:      
*************************************************/
void cmd_CRC_Calculate(void)
{
    INT16U ret;
    INT8U crcXInput[261];

    /************************************************* 
        CRC16 standard data
    *************************************************/
    const INT8U crcFInput[261] = {0xBA,0x37,0xB1,0x5A,0xCF,0xF7,0x7C,0x7D,0x0C,0xFD,0xA4,0x06,0xC2,0xA6,0x6D,0xD8,0x35,0x02,0x8D,0x5C,0x28,0x4A,0x57,0x1B,0xA7,0xA0,0x25,0x9D,0x16,0x08,0x06,0x83,\
                              0x79,0xA5,0xA7,0xE3,0x04,0x37,0x85,0xE2,0xE4,0xD3,0x52,0x96,0x4E,0xE3,0xA6,0x1D,0xC8,0x60,0xE9,0xF7,0xB6,0x4C,0x7F,0xAB,0x26,0x74,0x34,0x98,0xDF,0x66,0xE9,0x1A,\
                              0xA6,0xB0,0x7F,0xE0,0xD1,0x59,0x02,0x8B,0x10,0xA3,0x18,0x8A,0x7A,0x01,0xAA,0x33,0xD5,0x54,0xD3,0x24,0x29,0x6D,0x08,0xDB,0x86,0xBE,0x87,0x92,0x13,0x61,0x04,0x5E,\
                              0x59,0x4D,0x90,0x8A,0xD0,0xD6,0xCC,0x30,0xA9,0x64,0x4D,0x18,0xDC,0x76,0x51,0xD3,0x74,0xD9,0xA2,0x19,0x19,0x23,0xC9,0x62,0xDD,0x75,0x75,0xC2,0x4A,0x70,0x2D,0x09,\
                              0xAA,0x76,0x33,0x19,0x97,0x24,0xBA,0x88,0xC5,0x0E,0x4A,0x79,0x0E,0xBC,0x72,0xB5,0xBD,0xE4,0xAF,0x10,0x1E,0xE8,0x9A,0xF9,0x44,0x91,0x57,0x61,0x1C,0x0B,0x3C,0xD1,\
                              0xB1,0x22,0xC0,0xC5,0xC0,0xBD,0xA5,0x4C,0x7E,0x9A,0x67,0xE4,0xA6,0x4B,0xE6,0x91,0xC8,0x6F,0x51,0x3F,0xD0,0x33,0x54,0x58,0xD4,0x0A,0x85,0xA8,0x21,0xAA,0x0A,0x6F,\
                              0x86,0x49,0x8E,0xC5,0xE1,0x18,0x64,0x34,0xEA,0xFF,0xFB,0x93,0x3E,0x99,0x84,0x1F,0xAE,0x71,0xE1,0xDF,0xC7,0x7C,0xCF,0x37,0xA3,0xD9,0x56,0xCC,0xF1,0xC6,0x6F,0x9A,\
                              0x42,0xE4,0xF5,0x52,0x94,0xAE,0xD0,0xF8,0x23,0x35,0x5F,0xBC,0x6D,0x20,0x25,0x17,0x85,0xE2,0xB6,0x27,0x9B,0x3C,0xE2,0x4E,0xCB,0xF5,0x22,0x08,0x24,0xD6,0x42,0x0C,\
                              0xFB,0xEA,0x4E,0xA5,0x70}; 
    const INT8U crcFInput2[13] = {0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x11,0x22,0x33,0x44};                                
    
    #define std_CRCOutput_6363          0x8AA0
    #define std_CRCOutput_FFFF          0x2152
    #define std_CRCOutput_JAVA_TCK      0x3A2F
    
    SYSCLKEN |= BIT0;//enable CRC IP's CLK
    memcpy(crcXInput, crcFInput, sizeof(crcXInput));
        
    ret = HED_CRC16(0x6363, sizeof(crcXInput), crcXInput,2);
    if (ret != std_CRCOutput_6363)
    {
        G_SW1SW2 = 0x6F01;
        return;
    }
        
    ret = HED_CRC16(0xFFFF, sizeof(crcFInput), (INT8U *)crcFInput,2);
    if ((INT16U)(~ret) != std_CRCOutput_FFFF)
    {
        G_SW1SW2 = 0x6F02;
        return;
    }
    
    ret = HED_CRC16(0xFFFF, sizeof(crcFInput2), (INT8U *)crcFInput2,1);//fit to JC API's requirement,can pass TCK verify
    if ((INT16U)(~ret) != std_CRCOutput_JAVA_TCK)
    {
        G_SW1SW2 = 0x6F03;
        return;
    }
    
    G_SW1SW2 = 0x9000;
}

/************************************************* 
  Function:      cmd_GET_INFO
  Description:    
  Input:           
  Output:         
  Return:        
  Others:      
*************************************************/
void cmd_GET_INFO(void)
{
    INT8U p3,ins;
    
    /*no check P3 ,user can add it */    
    p3 = APDU_P3;
	ins = APDU_INS;
        
    /* according to P1, do*/
    switch (APDU_P1)
    {           
        case 0x01:
            HED_GetRandom(G_APDUBuf,8);
            break;
        
        case 0x02:
            HED_GetFactoryCode(G_APDUBuf);
            break;        
            
        case 0x03:
            HED_GetChipId(G_APDUBuf);
            break;
            
        default:
            G_SW1SW2 = 0x6F00;
            return;    
    }	
	
	DemoCos_SendINS(ins);//send INS       
    G_SendLen = (p3 == 0 ? 256 : p3);
    G_SW1SW2 = 0x9000;    
}
/************************************************* 
  Function:      cmd_NVM_Erase
  Description:    
  Input:           
  Output:         
  Return:        
  Others:      
*************************************************/
#define DEMO_ERWR_START_ADDR	0x00060000
#define PAGE_SIZE				0x200
#define SECTOR_SIZE				(PAGE_SIZE*8)
void cmd_NVM_Erase(void)
{   
	
	openSend60();
	/* according to P1, do*/
    switch (APDU_P1)
    {           
        case 0x01:
            if(HED_FlashErasePage_St(DEMO_ERWR_START_ADDR + APDU_P2 * PAGE_SIZE) != 0)
			{
				G_SW1SW2 = 0x6A80;
				closeSend60();
				return;
			}
            break;       
        case 0x02:
            HED_FlashEraseSectorNoChk_St(DEMO_ERWR_START_ADDR + APDU_P2 * SECTOR_SIZE);
            break;                  
        default:
            G_SW1SW2 = 0x6F00;
			closeSend60();
            return;    
    }
    G_SW1SW2 = 0x9000;
	closeSend60();
}

/************************************************* 
  Function:      cmd_NVM_Write
  Description:    
  Input:           
  Output:         
  Return:        
  Others:      
*************************************************/
void cmd_NVM_Write(void)
{   
	INT32U writeDataBuf[128];
	INT32U data, i;
	
	openSend60();
	/* according to P1, do*/
    switch (APDU_P1)
    {           
        case 0x01:
			for(i = 0; i < 128; i++)
			writeDataBuf[i] = i;
            if(HED_FlashWritePage_St (DEMO_ERWR_START_ADDR + APDU_P2 * PAGE_SIZE, writeDataBuf) != 0)
			{
				G_SW1SW2 = 0x6A80;	
				closeSend60();
				return;
			}
			break;
        case 0x02:
			DemoCos_SendINS(APDU_INS);//send INS
			HED_T0_ReceiveNBytesRam(G_APDUBuf+5, APDU_P3);
			data = (G_APDUBuf[7]<<24) + (G_APDUBuf[8]<<16) + (G_APDUBuf[9]<<8) + G_APDUBuf[10];
            if(HED_FlashWriteWord_St (DEMO_ERWR_START_ADDR+(G_APDUBuf[5]<<8)+G_APDUBuf[6], data) != 0)
			{
				G_SW1SW2 = 0x6A80;
				closeSend60();
				return;
			}
			break;
        default:
            G_SW1SW2 = 0x6F00;
			closeSend60();
            return;    
    }
    G_SW1SW2 = 0x9000;
	closeSend60();
}

/************************************************* 
  Function:      cmd_Loader_Resume
  Description:    
  Input:           
  Output:         
  Return:        
  Others:      
*************************************************/
void cmd_Loader_Resume(void)
{ 
	INT32U i, *loaderVecPtr;

	if(HED_FlashErasePage_St(0) != 0)
	{
		G_SW1SW2 = 0x6A80;
		return;
	}
	
	loaderVecPtr = (INT32U *)LOADER_START_ADDR;
	for(i = 0; i < 28; i++)
	{
		if(HED_FlashWriteWord_St (i*4, loaderVecPtr[i]) != 0)
		{
			G_SW1SW2 = 0x6A80;
			return;
		}
	}
	G_SW1SW2 = 0x9000;
}

