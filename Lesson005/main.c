#define NVIC_ISER 	(*(volatile long *) 0xE000E100) 		// Interrupt enable set register
#define NVIC_ICER 	(*(volatile long *) 0xE000E180) 		// Interrupt enable clear register	

int main(void) 
{
				NVIC_ISER |= (1<<0);					// 
				NVIC_ICER |= (1<<0);
	while(1)
	{
	}
}
