#include <serial.h>

int serial_test(void)
{
	char ch;

	serial_init();

	serial_puts("hello world !\n");
	serial_puts("input data,press q to exit\n");

	while(1) {
		ch = serial_getc();
		serial_putc(ch);
		if((ch == 'q') || (ch == 'Q'))
			break;
	}

	putchar('\r');
	putchar('\n');
	putchar('e');
	putchar('n');
	putchar('d');

	putchar('\r');
	putchar('\n');
	print_u32(0xabcdef12);

	putchar('\r');
	putchar('\n');
	print_u32(0x12345678);

	putchar('\r');
	putchar('\n');
	serial_puts("serial test done !\n");
	return 0;
}
