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


void SysTick_Handler(void);           // SysTick Handler

// External Interrupts
void Timer0_IRQHandler(void);         // 0: Default
void Timer1_IRQHandler(void);         // 

void delay_ms(unsigned int delay_temp);

unsigned int delay_count = 0;

void delay_ms(unsigned int delay_temp)
	{
		delay_count = delay_temp;
		while(delay_count) {}		
	}


void SysTick_Handler(void) 
	{
		if(delay_count > 0)
		{
			delay_count--;
		}
	};

int main(void) 
	{
		SYSCLKCFG = (0x01<<12) | (0x04 << 4);	//APB = AHB div 2; cpu = 15MHz
		SYSCLKEN |= (1<<9);
		TIMER0LOAD = 11;
		TIMER0CONTROL = 0x82; // 1000 0010
		while(1) 
			{
			delay_ms(1000);
			}
	}
