// CKMU
#define SYSCLKEN             (*((volatile unsigned long *)(0x50007000 + 0x200)))
#define SYSCLKCLR            (*((volatile unsigned long *)(0x50007000 + 0x204)))
#define SYSCLKCFG            (*((volatile unsigned long *)(0x50007000 + 0x208)))
/********************************************************************************
                             TIMER Regsiter
********************************************************************************/
#define TIMER0LOAD           (*((volatile unsigned long *)(0x40000800 + 0x00 )))
#define TIMER0VALUE          (*((volatile unsigned long *)(0x40000800 + 0x04 )))
#define TIMER0CONTROL        (*((volatile unsigned long *)(0x40000800 + 0x08 )))
#define TIMER0IS             (*((volatile unsigned long *)(0x40000800 + 0x0C )))
#define TIMER1LOAD           (*((volatile unsigned long *)(0x40000800 + 0x20 )))
#define TIMER1VALUE          (*((volatile unsigned long *)(0x40000800 + 0x24 )))
#define TIMER1CONTROL        (*((volatile unsigned long *)(0x40000800 + 0x28 )))
#define TIMER1IS             (*((volatile unsigned long *)(0x40000800 + 0x2C )))

int main(void) 
	{
		SYSCLKCFG = (0x01<<12) | (0x04 << 4);	//APB = AHB div 2; cpu = 15MHz
		SYSCLKEN |= (1<<9);
		TIMER0LOAD = 11;
		TIMER0CONTROL = 0x82; // 1000 0010

		while(1) 
			{
				unsigned int result2=TIMER0VALUE;
			}
	}
