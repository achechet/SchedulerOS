/*******************************************************************************
 Copyright(C),CEC Huada Electronic Design Co.,Ltd.
 File name:     CIU98428F_API_DES.h
 Author:        Wangyun
 Version:       V1.0 
 Date:          2014-12-08
 Description:  
 History:       
                V1.0	2014-12-08	initial version
*******************************************************************************/
#ifndef __CIU98428F_API_DES_H__
#define __CIU98428F_API_DES_H__

#define DES_SUCCESS 			0xB7C8D9EA
#define DES_LEN_ERR				0x932057CE
#define DES_CHK_ERR				0x48372615

#define ECB_ENCRYPT                         0x00
#define ECB_DECRYPT                         0x40
#define CBC_ENCRYPT                         0x20
#define CBC_DECRYPT                         0x60

typedef struct 
{
	unsigned int *iDataPtr;
	unsigned int DataLen;
	unsigned int *iKeyPtr;
	unsigned int *IVPtr;
	unsigned int *oDataPtr;
	unsigned char Type;
	unsigned int *paraChkSum;
}DES_Parameters, DES3_Parameters;

unsigned int HED_DESBlock(DES_Parameters * DES_Param);
unsigned int HED_DES3Block(DES3_Parameters * DES3_Param);

#endif
