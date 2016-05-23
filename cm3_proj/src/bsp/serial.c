#include "config.h"
#include "cs_types.h"
#include <string.h>
#include "global_macros.h"
#include "reg_uart.h"
#include "reg_sysctrl.h"
#include <stdarg.h>
#ifdef CONFIG_SERIAL_DMA_TEST
#include "reg_ifc.h"
#include "ifc.h"
#endif /* CONFIG_SERIAL_DMA_TEST */

#ifdef CONFIG_SERIAL
#define SERIAL_BAUD_RATE_DEFAULT     HAL_UART_BAUD_RATE_115200

static HWP_UART_T *hwp_uart = hwp_uart3;

void _serial_set_baudrate(HAL_UART_BAUD_RATE_T rate)
{
    /* use hw default 921600 */
}

void _serial_init(void)
{
    _serial_set_baudrate(SERIAL_BAUD_RATE_DEFAULT);

    hwp_uart->triggers   = UART_AFC_LEVEL(1); //7 ?

    /* Dec. 15, 2009 - Hua Zeng - Donot block UART even if peer is not connected */
    hwp_uart->ctrl       = UART_ENABLE | UART_DATA_BITS_8_BITS |
                           UART_TX_STOP_BITS_1_BIT | UART_PARITY_ENABLE_NO;

    /* Allow reception */
    hwp_uart->CMD_Set             = UART_RTS;
}

#ifdef CONFIG_SERIAL_DMA_TEST
/***************************************************************
   * Function     :hal_UartDmaFifoEmpty
   * Parameter  :HAL_IFC_REQUEST_ID_T requestId
   * Return        :BOOL,TRUE stand for data exist in buffer,FALSE stand for no data in buffer
   * Author        : xiankuiwei
   * Date           : 2012-10-10
   * Description : get the status of data exist in dma buffer
   *
   *Modification: none
***************************************************************/
BOOL hal_UartDmaFifoEmpty(HAL_IFC_REQUEST_ID_T requestId)
{
    UINT8 i;
    BOOL fifoIsEmpty = TRUE;

    // Check the requested id is not currently already used.
    for (i = 0; i < SYS_IFC_STD_CHAN_NB ; i++)
    {
        if (GET_BITFIELD(hwp_sysIfc->std_ch[i].control, SYS_IFC_REQ_SRC) == requestId)
        {
            fifoIsEmpty = hal_IfcChannelIsFifoEmpty(requestId, i);
            return fifoIsEmpty;
        }
    }

    return TRUE;
}

/***************************************************************
   * Function     :hal_UartDmaResetFifoTc
   * Parameter  :HAL_IFC_REQUEST_ID_T requestId
   * Author        : xiankuiwei
   * Date           : 2012-10-10
   * Description :clear data number the dma channel send or received
   *
   *Modification: none
***************************************************************/
void hal_UartDmaResetFifoTc(HAL_IFC_REQUEST_ID_T requestId)
{
    UINT8 i;

    // Check the requested id is not currently already used.
    for (i = 0; i < SYS_IFC_STD_CHAN_NB ; i++)
    {
        if (GET_BITFIELD(hwp_sysIfc->std_ch[i].control, SYS_IFC_REQ_SRC) == requestId)
        {
            hwp_sysIfc->std_ch[i].tc = 0;
            return;
        }
    }

    return;
}

/***************************************************************
   * Function     :hal_UartRxDmaPurgeFifo
   * Parameter  :HAL_IFC_REQUEST_ID_T requestId
   * Author        : xiankuiwei
   * Date           : 2012-10-10
   * Description :clear data in dma channel buffer
   *
   *Modification: None
***************************************************************/
void hal_UartRxDmaPurgeFifo(HAL_IFC_REQUEST_ID_T requestId)
{
    UINT8 i;

    for (i = 0; i < SYS_IFC_STD_CHAN_NB ; i++)

    if (GET_BITFIELD(hwp_sysIfc->std_ch[i].control, SYS_IFC_REQ_SRC) == requestId)
    {
        (void)hal_IfcChannelFlush(requestId, i);
    }
}


/***************************************************************
   * Function     :_serial_dma_init
   * Parameter  : none
   * Author        : xiankuiwei
   * Date           : 2012-10-10
   * Description : when using uart in dma mode , set the uart register as follows.
   *
   *Modification: None
***************************************************************/
void _serial_dma_init(void)
{
    _serial_set_baudrate(SERIAL_BAUD_RATE_DEFAULT);

    hwp_uart->triggers  = UART_AFC_LEVEL(1); //7 ?

    hwp_uart->ctrl      = UART_ENABLE | UART_DATA_BITS_8_BITS |
                            UART_TX_STOP_BITS_1_BIT | UART_PARITY_ENABLE_NO | UART_DMA_MODE_ENABLE;

    /* Allow reception */
   hwp_uart->CMD_Set    = UART_RTS;

   hwp_uart->irq_mask   = UART_RX_DMA_DONE | UART_RX_DMA_TIMEOUT;
}

/***************************************************************
   * Function     :hal_UartSendData
   * Parameter  :CONST UINT8* data  uart dma channel bufferaddress
                          UINT32 length   uart dma channel buffer length
   * Author        : xiankuiwei
   * Date           : 2012-10-10
   * Description : Before sending data , assign the dma channel buffer address and buffer
                           length.
   *
   *Modification: None
***************************************************************/
UINT32 hal_UartSendData(CONST UINT8* data, UINT32 length)
{
    UINT8  TxChannelNum = 0;

    TxChannelNum = hal_IfcTransferStart(HAL_IFC_UART3_TX, (UINT8 *)data, length, HAL_IFC_SIZE_8_MODE_AUTO);

    if (HAL_UNKNOWN_CHANNEL == TxChannelNum)
    {
        // No channel available
        return 0;
    }
    else
    {
        // all data will be send
        return length;
    }
}

/***************************************************************
   * Function     :_serial_dma_puts
   * Parameter  :UINT8* destAddress  uart dma sending buffer address
   * Return        : UINT32
   * Author        : xiankuiwei
   * Date           : 2012-10-10
   * Description :Before sending data , assign the dma channel buffer address and buffer
                           length.
   *
   *Modification: None
***************************************************************/
void _serial_dma_puts(UINT8 *s)
{
    UINT32 length = 0;

    length = (UINT32)strlen((const char *)s);

    /* wait until fifo is empty, to prevent overlap original data */
    while(!hal_UartDmaFifoEmpty(HAL_IFC_UART3_TX));

    (void)hal_UartSendData(s, length);
}

/***************************************************************
   * Function     :hal_UartGetData
   * Parameter  :UINT8* destAddress  uart dma receiving buffer address
                          UINT32 length         uat dma receiving buffer length,when data num
                          received is up to the length , a uart_rx_dmadone interrupt generate.
   * Return        : UINT32
   * Author        : xiankuiwei
   * Date           : 2012-10-10
   * Description :Before receiving data , assign the dma receiving buffer address and buffer length.
   *
   *Modification: None
***************************************************************/
UINT32 hal_UartGetData(UINT8* destAddress, UINT32 length)
{
    UINT8  RxChannelNum = 0;

    // (Re)Start transfert, this will reset the timeout counter.
    // Do this before clearing the mask and cause, to prevent
    // a previous unwanted timeout interrupt to occur right between the
    // two clears and then the restart transfert.

    RxChannelNum = hal_IfcTransferStart(HAL_IFC_UART3_RX, destAddress, length, HAL_IFC_SIZE_8_MODE_AUTO);

    // check if we got an IFC channel
    if (HAL_UNKNOWN_CHANNEL == RxChannelNum)
    {
        // No channel available
        // No data received
        return 0;
    }
    else
    {
        // all data will be fetched
        return length;
    }
}

/***************************************************************
   * Function     :_serial_dma_gets
   * Parameter  :UINT8 *s dma   uart dma receiving buffer address
                          UINT32 length  uart dma receiving buffer len
   * Return        : None
   * Author        : xiankuiwei
   * Date           : 2012-10-10
   * Description :Before receiving data , assign the dma receiving buffer address and buffer length.
   *
   *Modification: None
***************************************************************/
void _serial_dma_gets(UINT8 *s, UINT32 length)
{
    if (hwp_uart->irq_cause & UART_RX_DMA_TIMEOUT)
    {
        hal_UartRxDmaPurgeFifo(HAL_IFC_UART3_RX);

        while(!hal_UartDmaFifoEmpty(HAL_IFC_UART3_RX));

        hwp_uart->irq_cause |= UART_RX_DMA_TIMEOUT;
    }

    hal_UartDmaResetFifoTc(HAL_IFC_UART3_RX);

    (void)hal_UartGetData(s, length);

}
#endif /* CONFIG_SERIAL_DMA_TEST */

void _serial_deinit(void)
{
    hwp_uart->ctrl    = 0;
    hwp_uart->CMD_Clr = UART_RTS;
}

void _serial_enable_rtscts(void)
{
     hwp_uart->ctrl |= UART_AUTO_FLOW_CONTROL;
}

/*
 * Test whether a character is in the RX buffer
 */
int _serial_tstc(void)
{
    return (GET_BITFIELD(hwp_uart->status, UART_RX_FIFO_LEVEL));
}

int _serial_getc(void)
{
    /* wait for character to arrive */ ;
    while (!(GET_BITFIELD(hwp_uart->status, UART_RX_FIFO_LEVEL)))
        ;

    return (hwp_uart->rxtx_buffer & 0xff);
}

void _serial_putc_hw(const char c)
{
    // Place in the TX Fifo ?
    while (!(GET_BITFIELD(hwp_uart->status, UART_TX_FIFO_SPACE)))
        ;

    hwp_uart->rxtx_buffer = (UINT32)c;
}

void _serial_putc(const char c)
{
    if (c == '\n') {
        _serial_putc_hw('\r');
    }
    _serial_putc_hw(c);
}

void _serial_puts(const char *s)
{
    while (*s) {
        _serial_putc(*s++);
    }
}

void serial_init(void)
{
    _serial_init();
}

void serial_enable_rtscts(void)
{
    _serial_enable_rtscts();
}

int serial_getc(void)
{
    return _serial_getc();
}

int serial_tstc(void)
{
    return _serial_tstc();
}

void serial_putc(const char c)
{
    _serial_putc(c);
}

void serial_puts(const char *s)
{
    _serial_puts(s);
}

#ifdef CONFIG_SERIAL_DMA_TEST
/**************************************************
   * Function     :serial_init
   * Parameter  :void
   * Author        : xiankuiwei
   * Date           : 2012-10-10
   * Description : uart register initialization.
   * Modification: None
   *
**************************************************/
void serial_dma_init(void)
{
    _serial_dma_init();
}

/***************************************************************
   * Function     :serial_dma_puts
   * Parameter  :const char *s  uart dma sending buffer address
   * Return        : void
   * Author        : xiankuiwei
   * Date           : 2012-10-10
   * Description : Sending characters by using uart in dma mode.
   *
   *Modification: None
***************************************************************/
void serial_dma_puts(const char *s)
{
    _serial_dma_puts((UINT8 *)s);
}

/***************************************************************
   * Function     :serial_dma_getc
   * Parameter  :UINT8  *s dma uart dma receiving buffer address
   * Author        : xiankuiwei
   * Date           : 2012-10-10
   * Description :Initializing the uart dma receing buffer address and buffer length.
   * Modification: None
   *
***************************************************************/
void serial_dma_getc(UINT8 *s)
{
    _serial_dma_gets(s, 1);
}
#endif /* CONFIG_SERIAL_DMA_TEST */

static void backspace(int cnts)
{
    int i;

    for(i=0; i<cnts; i++) {
        serial_putc('\b');
        serial_putc(' ');
        serial_putc('\b');
    }
}

int serial_gets(UINT8 *pstr)
{
    UINT32 length;

    length = 0;
    while(1) {
        pstr[length] = serial_getc();
        if(pstr[length] == '\r') {
            pstr[length] = 0x00;
            break;
        }
        else if( pstr[length] == '\b' ) {
            if(length>0) {
                length --;
                backspace(1);
            }
        }
        else {
            serial_putc(pstr[length]);
            length ++;
        }

        if(length > 32)
            return -1;
    }
    return 0;
}

void hex2asc(UINT8 *pdata, UINT8 *pstr, UINT8 len)
{
	UINT8 i, ch, mylen;

	if(len>16)
		mylen = 16;
	else
		mylen = len;

	for(i=mylen; i>0; i--)
	{
		ch = pdata[(i-1)>>1];
		if( i%2 )
			ch &= 0xF;
		else
			ch >>= 4;
		if(ch<10)
			ch += '0';
		else
			ch += ('A'-10);
		pstr[mylen-i] = ch;

	}
	pstr[mylen] = 0;
}

UINT64 asc2hex(UINT8 *pstr, UINT8 len)
{
	UINT8 i,ch,mylen;
	UINT64 hexvalue;

	for(mylen=0,i=0; i<16; i++)
	{
		if( pstr[i] == 0 )
			break;
		mylen ++;
	}
	if( len != 0 )
	{
		if(mylen>len)
			mylen = len;
	}
	if(mylen>16)
		mylen = 16;

	hexvalue = 0;
	for (i = 0; i < mylen; i++)
	{
		hexvalue <<= 4;
		ch = *(pstr+i);
		if((ch>='0') && (ch<='9'))
			hexvalue |= ch - '0';
		else if((ch>='A') && (ch<='F'))
			hexvalue |= ch - ('A' - 10);
		else if((ch>='a') && (ch<='f'))
			hexvalue |= ch - ('a' - 10);
		else
			;
	}
	return(hexvalue);
}

static char *flex_i2a(unsigned num, char *pstr, int radix, int len, int digits)
{
	char ch;
	char *p;
	int sign = 0;

	if (radix < 0) {
		radix = -radix;
		sign = 1;
	}

	// limit the string and number of displayed digits
	if (digits > len-1) digits = len-1;
	p = pstr + len;
	*--p = 0;

	// print the number including sign
	while (pstr != p)
	{
		unsigned int n = num/radix;
		ch = num - n*radix;
		num = n;
		ch += (ch < 10)? '0': 'A'-10;
		*--p = ch;
		if (num == 0) break;
	}

	// output the sign
	if (sign && pstr != p) {
		*--p = '-';
	}

	if (digits == 0)
		return p;

	ch = (radix == 16)? '0': ' ';
	len = digits - (pstr+len-p-1); // target_digits - current_chars
	for (; len > 0; len--)
		*--p = ch;

	return p;
}

/*
 rprintf() is a tiny version of printf() for use in the boot_rom.
 The format-string is very limited subset of printf():
 	%<size><format>
 where <size> is a single digit size specifier between 0 and 9. <size> is
 accepted for all <formats> but ignored for strings.

 The following <format> codes is supported:
    s    - string
    d    - decimal number, signed
    u    - decimal number, unsigned
    x    - hexadecimal number, unsigned
    %    - literal '%'

 Examples:
        rprintf("The answer to life, universe and %s is %d!\n", "everything", 42);
        rprintf("%d is %u in unsigned (hex: 0x%8x)\n", -42, -42, -42);
    Output:
        "The answer to life, universe and everything is 42!"
        "-42 is 4294967254 in unsigned (hex: 0xFFFFFFD6)"

*/

void rprintf(const char *fmt, ...)
{
	va_list ap;

	char str[16];
	int iarg;

	va_start(ap, fmt);

	const char *s = fmt;
	while(*s) {
		if (*s == '%')
		{
			int size = 0;
			int radix = 10;
			const char *p = 0;

			++s;
			if (*s == '0') s++;
			if (*s >= '0' && *s <= '9') { size = *s++ - '0'; };

			switch(*s) {
			case 's':
				p = va_arg(ap, const char *);
				break;
			case 'x':
				radix = 16;
			case 'd':
			case 'u':
				iarg = va_arg(ap, int);
				if (*s == 'd' && iarg < 0) {
					radix = -10;
					iarg = -iarg;
				}
				p = flex_i2a(iarg, str, radix, sizeof(str), size);
				break;
			case '%':
				_serial_putc('%');
				break;
			case 'c':
				iarg = va_arg(ap, int);
				str[0] = iarg;
				str[1] = 0;
				p = str;
			default: ;
			}

			if (p) _serial_puts(p);
		}
		else {
			_serial_putc(*s);
		}
		++s;
	}

	va_end(ap);
}

void print_u8(UINT8 u8)
{
    char str[2+1];

    hex2asc((UINT8 *)&u8, (UINT8 *)str, 2);
    serial_puts((char *)str);
}

void print_u16(UINT16 u16)
{
    char str[4+1];

    hex2asc((UINT8 *)&u16, (UINT8 *)str, 4);
    serial_puts(str);
}

void print_u32(UINT32 u32)
{
    char str[8+1];

    hex2asc((UINT8 *)&u32, (UINT8 *)str, 8);
    serial_puts(str);
}

UINT32 serial_get_u32(void)
{
    UINT8 str[32];

    serial_gets(str);
    return (UINT32)asc2hex(str, 0);
}

void dump_buf(UINT8 *buf, UINT32 size)
{
    UINT32 i, j;

    size += (size%16);
    for (i=0;i<size;i+=16) {
        for (j=0;j<16;j++) {
            print_u8(buf[i+j]);
            serial_puts(" ");
        }
        serial_puts("\n");
    }
}

#ifdef CONFIG_SERIAL_TEST
void serial_test(void)
{
	char data, reply;

	//serial_init();

	serial_puts("Serial Test\n");
	print_u8(0x5a);
	serial_puts("\n");
	print_u16(0xabcd);
	serial_puts("\n");
	print_u32(0x12345678);
	serial_puts("\n");

	while(1)
	{
		serial_puts("Check input: ");

		data = (char)serial_getc();
		if(data >= 'A' && data <='Z')
		{
			reply = data + 'a' - 'A';
		}
		else if(data >= 'a' && data <= 'z')
		{
			reply = data + 'A' - 'a';
		}
		else if(data >= '0' && data <= '9')
		{
			reply = '0' + ((data - '0' + 5) % 10);
		}
		else
		{
			reply = data;
		}

		serial_putc(reply);
		serial_puts("\n");
		if (data == 'q' || data== 'Q')
			break;
	}
	serial_puts("Serial Test Done\n");
}


#ifdef CONFIG_SERIAL_DMA_TEST
/***************************************************************
   * Function     : serial_dma_test
   * Parameter  : void
   * Return        : void
   * Author        : xiankuiwei
   * Date           : 2012-10-10
   * Description : testing the function of uart in dma mode .
   *
   *Modification: None
***************************************************************/
void serial_dma_test(void)
{
    char buf[512] = "Serial Test\r\n";
    char bufin;

    serial_dma_init();

    serial_dma_puts(buf);


    (void)strcpy(buf, "Check input-1:\r\n");
    serial_dma_puts(buf);

    serial_dma_getc((UINT8 *)&bufin);

    while(1)
    {
        while(!(hwp_uart->irq_cause & UART_RX_DMA_DONE));
        buf[0] = bufin;
        buf[1] = 0x00;

        serial_dma_puts(buf);
        hwp_uart->irq_cause |= UART_RX_DMA_DONE;

        if (('\r' == buf[0]) || ('\n' == buf[0]))
        {
            break;
        }

        serial_gets((UINT8 *)&bufin);
   }

   serial_dma_puts("Serial Test Done\r\n");
}
#endif /* CONFIG_SERIAL_DMA_TEST */
#endif /* CONFIG_SERIAL_TEST */

#else /* CONFIG_SERIAL */
void serial_init(void) {}
void serial_enable_rtscts(void) {}
int serial_getc(void) {return 0;}
int serial_tstc(void) {return 0;}
void serial_putc(const char c) {}
void serial_puts(UINT8 *s) {}
int serial_gets(UINT8 *pstr) {return 0;}

void print_u8(UINT8 u8) {}
void print_u16(UINT16 u16) {}
void print_u32(UINT32 u32) {}
UINT32 serial_get_u32(void) {return 0;}
void dump_buf(UINT8 *buf, UINT32 size) {}
#endif /* CONFIG_SERIAL */

