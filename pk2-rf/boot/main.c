#include <register.h>
#include <tmcu_irq.h>

#if 0
void test_do_loop(void) property(loop_levels_2)
{
	int i,j,k;
	int var_i=0,var_j=0,var_k=0;
	for(i=0;i<4;i++){  
		write_register(0xe8,0x00);
		delay_calib(500);  
		write_register(0xe8,0x80);
		delay_calib(500);
		write_register(0xcc,i+1);
		for(j=0;j<5;j++){
			write_register(0xe8,0x00);
			delay_calib(330);
			write_register(0xe8,0x80);
			delay_calib(330);
			var_j++;
			write_register(0xcd,var_j);
			for(k=0;k<6;k++){
				write_register(0xe8,0x00);
				delay_calib(165);
				write_register(0xe8,0x80);
				delay_calib(165);
				var_k++;
				write_register(0xce,var_k);
			}
		}
	}
	write_register(0xe8,0x00);  
}
#endif

//#define TMCU_INT_MASK 0xF0FE0000
#define TMCU_INT_MASK 0xF0FEFFFF//0xFCFFFFFF//0xf0fff0fe

volatile unsigned short clock_source = 0;//26M or 156M
volatile long long time_trace = 0;

int main(void)
{
	unsigned int data_b8h;

	write_register(0xfe,0x00); // GSM_Spi chaos debug
	// interrupt
	set_interrupt_mask(TMCU_INT_MASK);
	enable_interrupts(); 

	// 1. judge the clock source, 26 or 156?
	data_b8h = read_register(0xb8);
	clock_source = (data_b8h&0x18)? 156 : 26;

	// 2. mailbox for response

	// 3. flag
	//write_register(0x24,0xaa);
	//write_register(0x26,0x11);	

	//////////////
	//test_do_loop(); 
	////////////

	// 4. software version
	// --------------
	//version 01: first merge version, 2015,07,16
	//version 02: do-loop in main() is limited to 1; close 0x2fh, 2015,07,28
	//version 04: 8850E U02
	//version 05: add band8
	//---------------
	write_register(0xcb,0x05);//0XcaH+0XcbH:version control

	// 5. halt 
	while(1) {
		// main thread
		int mocro_u = 0;
		while(mocro_u<65535){
			delay_calib(33);//double do--loop, worked,not sure yet
			mocro_u++;
			write_register(0xc6,((mocro_u>>8)&0xff));
			write_register(0xc7,(mocro_u&0xff));
			time_trace++;
		}
	}
	return 0;
}
