

;;/*****************************************************
;; Copyright(C),CEC Huada Electronic Design Co.,Ltd.
 
;; File name: 	CIU98428F_API_NVM_STABILITY.s
;; Author: 		wangyun   
;; Version:		V1.0
;; Date:      	2014-12-04
;; Description: NVM Stability function
;; History:	
;;				V1.0	2014-12-04	initial version
;;******************************************************/

SYSNVMMODE      EQU     0x50007084

                PRESERVE8
                THUMB
                
                IMPORT	chkWrSwitch
                AREA    SETSYSNVMMODE, CODE, READONLY
                EXPORT  setWrMode

setWrMode
                PUSH {LR}
                
                MOVS R3,#0
                MSR APSR,R3
                MOVS R2,R0
                LDR R1,=SYSNVMMODE
                
                PUSH {R1-R2}
                BL chkWrSwitch
                POP {R1-R2}
                
                BNE no_opt
                STR R2,[R1];STR is not conditional instruction on the Cortex-M0 processor.
no_opt                
                POP {R3}
                
                BX R3                
                END
