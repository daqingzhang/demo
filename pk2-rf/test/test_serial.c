#include <serial.h>
#include <util.h>
#include <string.h>

#define UART_IRQ_MASK_OVR (1 << 30)
#define UART_IRQ_MASK_TX  (1 << 29)
#define UART_IRQ_MASK_RX  (1 << 28)

volatile static int serial_callback_done = 0;
typedef void (callback_t)(void *parm);

struct serial_channel {
	int active;
	u8 *pbuf;
	u32 len;
	callback_t *callback;
};

volatile static struct serial_channel *gptxchan = 0;
volatile static struct serial_channel *gprxchan = 0;

void serial_callback(unsigned int irqs)
{
	if(irqs & UART_IRQ_MASK_OVR) {
		u32 intclr = HWP_UART->intclr;

		serial_callback_done = 0x10;
		if(intclr & UART_INTCLR_RX_OVR) { //rx overrun
			serial_callback_done |= 0x01;
			HWP_UART->intclr = UART_INTCLR_RX_OVR;
		}
		if(intclr & UART_INTCLR_TX_OVR) {
			serial_callback_done |= 0x02;
			HWP_UART->intclr = UART_INTCLR_TX_OVR;
		}
	}
	if(irqs & UART_IRQ_MASK_TX) {
		u32 intclr = HWP_UART->intclr;

		serial_callback_done = 0x20;
		if(intclr & UART_INTCLR_TX)
			serial_callback_done |= 0x04;

		HWP_UART->intclr = UART_INTCLR_TX;

		// send data via interrupts
		if(gptxchan) {
			if((gptxchan->len != 0) & (gptxchan->pbuf != NULL)) {
				HWP_UART->data = *(gptxchan->pbuf++);
				gptxchan->len--;
			}
			if(gptxchan->callback)
				gptxchan->callback((void *)&(gptxchan->len));
		}
	}
	if(irqs & UART_IRQ_MASK_RX) {
		u32 intclr = HWP_UART->intclr;

		serial_callback_done = 0x40;
		if(intclr & UART_INTCLR_RX)
			serial_callback_done |= 0x08;

		HWP_UART->intclr = UART_INTCLR_RX;
		//serial_puts("serial callback,rx irqs\n");
		// receive data via interrupts
		if(gprxchan) {
			if((gprxchan->len != 0) & (gprxchan->pbuf != NULL)) {
				*(gprxchan->pbuf++) = HWP_UART->data;
				gprxchan->len--;
			}
			if(gprxchan->callback)
				gprxchan->callback((void *)&(gprxchan->len));
		}
	}
}

static int invalid_callback(u32 key, u32 value)
{
	serial_puts("\n");
	switch(key) {
	case 0x11:
		serial_puts("intclr rx ovr set 1\n");
		break;
	case 0x12:
		serial_puts("intclr tx ovr set 1\n");
		break;
	case 0x14:
		serial_puts("status tx ovr set 1\n");
		break;
	case 0x18:
		serial_puts("status rx ovr set 1\n");
		break;
	case 0x16:
		serial_puts("status & intclr tx ovr set 1\n");
		break;
	case 0x19:
		serial_puts("status & intclr rx ovr set 1\n");
		break;
	case 0x24:
		serial_puts("intclr tx set 1\n");
		break;
	case 0x48:
		serial_puts("intclr rx set 1\n");
		break;
	default:
		serial_puts("invalid_callback, key is ");
		print_u32(key);
		serial_puts("\n");
		break;
	}
	if(key == value)
		return 0;
	return 1;
}

static int serial_irq_rxovr(void)
{
	u8 t;

	serial_init();

	// send message
	serial_puts("serial_irq_rxovr, input 2 bytes\n");
	while(HWP_UART->state & UART_STATE_TX_FUL)
		;
	mdelay(10);

	// if rx fifo is not full, read rxfifo
	while(HWP_UART->state & UART_STATE_RX_FUL)
		t = HWP_UART->data;
	mdelay(10);

	// clear rx ovr state
	HWP_UART->intclr = UART_INTCLR_RX_OVR;

	// enable global irq
	irq_clr_pending(UART_IRQ_MASK_OVR);
	irq_enable(UART_IRQ_MASK_OVR);

	// enable rx ovr irq
	HWP_UART->ctrl |= UART_CTRL_RX_OVR_INTEN;
	mdelay(10);
	serial_callback_done = 0;
	while(serial_callback_done == 0);

	// disable irq
	HWP_UART->ctrl &= ~UART_CTRL_RX_OVR_INTEN;
	irq_disable(UART_IRQ_MASK_OVR);

	// get data
	while(HWP_UART->state & UART_STATE_RX_FUL) {
		t = HWP_UART->data;
		serial_puts("serial_irq_rxovr, get 1 byte: ");
		print_u32(t);
		serial_puts("\n");
	}
	if(invalid_callback(serial_callback_done, 0x11)) {
		serial_puts("serial_irq_rxovr, error code ");
		print_u32(serial_callback_done);
		serial_puts("\n");
		return -1;
	}
	serial_puts("serial_irq_rxovr, test success !\n");
	return 0;
}

static int serial_irq_rx(void)
{
	u8 t;

	serial_init();

	serial_puts("serial_irq_rx, intput 1 byte\n");
	while(HWP_UART->state & UART_STATE_TX_FUL)
		;

	// if rx fifo is not full, read rxfifo
	while(HWP_UART->state & UART_STATE_RX_FUL)
		t = HWP_UART->data;
	mdelay(10);

	// clear rx irq state
	HWP_UART->intclr = UART_INTCLR_RX;

	// enable global irq
	irq_clr_pending(UART_IRQ_MASK_RX);
	irq_enable(UART_IRQ_MASK_RX);

	serial_callback_done = 0;
	// enable rx irq
	HWP_UART->ctrl |= (UART_CTRL_RX_INTEN | UART_CTRL_RX_EN);
	//HWP_UART->ctrl |= (UART_CTRL_RX_EN);
	mdelay(10);

	while(serial_callback_done == 0) {
#if 0
		if(HWP_UART->state & UART_STATE_RX_FUL) {
			dump_reg((void *)HWP_UART,5);
			serial_puts("serial_irq_rx, break waiting\n");
			break;
		}
#endif
	};

	// disable irq
	HWP_UART->ctrl &= ~UART_CTRL_RX_INTEN;
	irq_disable(UART_IRQ_MASK_RX);

	// get data
	while(HWP_UART->state & UART_STATE_RX_FUL) {
		t = HWP_UART->data;
		serial_puts("serial_irq_rx, get 1 byte: ");
		print_u32(t);
		serial_puts("\n");
	}

	if(invalid_callback(serial_callback_done, 0x48)) {
		serial_puts("\nserial_irq_rx, error code ");
		print_u32(serial_callback_done);
		serial_puts("\n");
		return -1;
	}
	serial_puts("\nserial_irq_rx, test success !\n");
	return 0;
}

static int serial_irq_txovr(void)
{
	serial_init();
	serial_puts("serial_irq_txovr, send 3 bytes ...\n");

	// if txfifo is full, waitting ...
	while(HWP_UART->state & UART_STATE_TX_FUL)
		;
	mdelay(10);

	// clear tx ovr state
	HWP_UART->intclr = UART_INTCLR_TX_OVR;
	irq_clr_pending(UART_IRQ_MASK_OVR);

	// enable global irq
	irq_enable(UART_IRQ_MASK_OVR);
	HWP_UART->ctrl |= UART_CTRL_TX_OVR_INTEN;

	serial_callback_done = 0;
	mdelay(10);

	HWP_UART->data = 0x42;
	HWP_UART->data = 0x43;
	HWP_UART->data = 0x44;
	while(serial_callback_done == 0);

	// disable global irq
	HWP_UART->ctrl &= ~UART_CTRL_TX_OVR_INTEN;
	irq_disable(UART_IRQ_MASK_OVR);

	if(invalid_callback(serial_callback_done, 0x12)) {
		serial_puts("\nserial_irq_txovr, error code ");
		print_u32(serial_callback_done);
		serial_puts("\n");
		return -1;
	}
	serial_puts("\nserial_irq_txovr, test success !\n");
	return 0;
}

static int serial_irq_tx(void)
{
	serial_init();
	serial_puts("serial_irq_tx, send 1 bytes ...\n");

	// if txfifo is full, waitting ...
	while(HWP_UART->state & UART_STATE_TX_FUL)
		;
	mdelay(10);

	// clear tx state
	HWP_UART->intclr = UART_INTCLR_TX;

	// enable global irq
	irq_clr_pending(UART_IRQ_MASK_TX);
	irq_enable(UART_IRQ_MASK_TX);

	// enable tx ovr irq
	HWP_UART->ctrl |= UART_CTRL_TX_INTEN;
	mdelay(10);
	serial_callback_done = 0;
	HWP_UART->data = 0x42;
	while(serial_callback_done == 0);

	// disable global irq
	HWP_UART->ctrl &= ~UART_CTRL_TX_INTEN;
	irq_disable(UART_IRQ_MASK_TX);

	if(invalid_callback(serial_callback_done, 0x24)) {
		serial_puts("\nserial_irq_tx, error code ");
		print_u32(serial_callback_done);
		serial_puts("\n");
		return -1;
	}
	serial_puts("\nserial_irq_tx, test success !\n");
	return 0;
}

static struct serial_channel txchan;
static struct serial_channel rxchan;

volatile static int tx_callback_done = 0;
volatile static int rx_callback_done = 0;

//static u8 serial_txbuf[80];
static u8 serial_rxbuf[80];

static void init_channel(struct serial_channel *chan,int act, u8 *buf, u32 len, callback_t *cb)
{
	chan->active 	= act;
	chan->pbuf 	= buf;
	chan->len 	= len;
	chan->callback 	= cb;
}

static void register_channel(struct serial_channel *txch, struct serial_channel *rxch)
{
	gptxchan = txch;
	gprxchan = rxch;
}

static void rx_callback(void *param)
{
	u32 len = *(u32 *)param;

	if(!len)
		rx_callback_done = 1;
}

static void tx_callback(void *param)
{
	u32 len  = *(u32 *)param;

	if(!len)
		tx_callback_done = 1;
}

static int serial_irq_rxtx(void)
{
	int len = 5;

	serial_init();
	serial_puts("serial_irq_rxtx, input 5 bytes ...\n");

	while(HWP_UART->state & UART_STATE_TX_FUL);
	mdelay(10);

	rx_callback_done = 0;
	tx_callback_done = 0;

	init_channel(&rxchan, 1, serial_rxbuf,len,rx_callback);
	init_channel(&txchan, 1, &serial_rxbuf[1],len,tx_callback);
	register_channel(&txchan,&rxchan);

	HWP_UART->intclr = UART_INTCLR_TX | UART_INTCLR_RX;
	irq_clr_pending(UART_IRQ_MASK_RX | UART_IRQ_MASK_TX);

	irq_enable(UART_IRQ_MASK_RX | UART_IRQ_MASK_TX);
	HWP_UART->ctrl |= UART_CTRL_TX_INTEN | UART_CTRL_RX_INTEN;

	while(rx_callback_done == 0);

	HWP_UART->data = serial_rxbuf[0];
	while(tx_callback_done == 0);

	HWP_UART->ctrl &= ~(UART_CTRL_TX_INTEN | UART_CTRL_RX_INTEN);
	irq_disable(UART_IRQ_MASK_RX | UART_IRQ_MASK_TX);

	serial_puts("\nserial_irq_rxtx, test success !\n");
	return 0;
}

static int serial_irq_send(void)
{
	char *str = "hello world\r\n";

	serial_init();
	serial_puts("serial_irq_send, start ...\n");
	mdelay(10);

	init_channel(&txchan, 1, (u8 *)str,strlen(str),tx_callback);
	register_channel(&txchan,0);
	tx_callback_done = 0;

	HWP_UART->intclr = UART_INTCLR_TX;
	irq_clr_pending(UART_IRQ_MASK_TX);

	irq_enable(UART_IRQ_MASK_TX);
	HWP_UART->ctrl |= UART_CTRL_TX_INTEN;

	HWP_UART->data = 0x38;
	while(tx_callback_done == 0);

	HWP_UART->ctrl &= ~UART_CTRL_TX_INTEN;
	irq_enable(UART_IRQ_MASK_TX);

	serial_puts("serial_irq_send, test success !\n");
	return 0;
}

static int serial_irq_receive(void)
{
	char rxbuf[30];
	int i,len = 1;

	serial_init();
	serial_puts("serial_irq_receive, start ...\n");

	init_channel(&rxchan, 1, (u8 *)rxbuf, len, rx_callback);
	register_channel(0,&rxchan);
	rx_callback_done = 0;

	HWP_UART->intclr = UART_INTCLR_RX;
	irq_clr_pending(UART_IRQ_MASK_RX);

	irq_enable(UART_IRQ_MASK_RX);
	HWP_UART->ctrl |= UART_CTRL_RX_INTEN;

	while(!rx_callback_done);

	HWP_UART->ctrl &= ~UART_CTRL_RX_INTEN;
	irq_disable(UART_IRQ_MASK_RX);

	for(i = 0;i < len;i++) {
		serial_puts("get data: ");
		print_u32(rxbuf[i]);
		serial_puts("\n");
	}

	serial_puts("serial_irq_receive, test success !\n");
	return 0;
}

static int serial_loopback(void)
{
	u8 ch;

	serial_init();

	serial_puts("hello world !\n");
	serial_puts("input data,press q to exit\n");

	while(1) {
		serial_puts(">");
		ch = serial_getc();
		serial_putc(ch);
		serial_puts("\n");
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
	serial_puts("serial, loop back test success !\n");
	return 0;
}

int serial_test(void)
{
	int r = 0;
	r += serial_loopback(); // loopback pooling
//	r += serial_irq_txovr();// tx overrun interrupts test
#if 1
	r += serial_irq_tx(); 	// send data via interrupts
	r += serial_irq_rxovr();// rx overrun interrupts test
	r += serial_irq_rx(); 	// received data via interrupts
	r += serial_irq_rxtx(); // received & send data via interrupts
	r += serial_irq_send();	// send string via interrupts
	r += serial_irq_receive(); // receive string via interrupts
#endif
	return r;
}
