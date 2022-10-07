Stack_Size    EQU   0x400
top_of_stack  EQU 0x20000000 + Stack_Size

  PRESERVE8
  THUMB
  
  AREA	RESET,	DATA, READONLY
  DCD   top_of_stack  ;Top of Stack
  DCD   Start

  AREA  |.text|, CODE, READONLY

  ENTRY
 
Start    PROC
	
  MOV   R0, #0
  
  ENDP

  ALIGN
  END