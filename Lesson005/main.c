typedef unsigned long int uint32_t;

#define NVIC_ISER 	(*(volatile long *) 0xE000E100) 		// Interrupt enable set register
#define NVIC_ICER 	(*(volatile long *) 0xE000E180) 		// Interrupt enable clear register	

void _delay(uint32_t);

int main(void) 
{
					uint32_t a = 0;
	while(1)
	{
				NVIC_ISER |= (1<<0);					// 

				a = a + 1;
				_delay(10000);
				NVIC_ICER |= (1<<0);
				_delay(100);
	}
}
void _delay(uint32_t index) {
	while(index--);
	}
