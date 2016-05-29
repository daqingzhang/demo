#include <hal_config.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <misc.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_usart.h>
#include <dev_usart.h>

/* define available usart port*/
#define DBG_USART USART2
#define CMD_USART USART1

typedef USART_TypeDef usart_dev_t;

static int config_usart_nvic(usart_dev_t *usart)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	if(usart == USART1) {
		/* Enable the USART1 Interrupt */
		NVIC_InitStructure.NVIC_IRQChannel                   = USART1_IRQn;	 
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 13;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
		//GROUP 4
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	} else if(usart == USART2) {
		/* Enable the USART2 Interrupt */
		NVIC_InitStructure.NVIC_IRQChannel                   = USART2_IRQn;	 
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 12;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
		//GROUP 4
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	} else {
		return -1;
	}
	return 0;
}

static int config_usart_gpio(usart_dev_t *usart)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	if(usart == USART1) {	
		/* Configure USART & GPIO Clock */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	
		/* Configure GPIO Pins */
		/* Configure USART1 Tx (PA.09) as alternate function push-pull */
		GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_9;
		GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);    
		/* Configure USART1 Rx (PA.10) as input floating */
		GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOA, &GPIO_InitStructure);		
	} else if(usart == USART2) {
		/* Configure USART & GPIO clock */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	
		/* Configure GPIO Pins */
		/* Configure USART2 Tx (PA.02) as alternate function push-pull */
		GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_2;
		GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);    
		/* Configure USART2 Rx (PA.03) as input floating */
		GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_3;
		GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOA, &GPIO_InitStructure);		
	} else {
		return -1;
	}
	return 0;
}

static int config_usart(usart_dev_t *usart,int br,int data,char parity,int stop)
{
	uint16_t databit,paritybit,stopbit;
	USART_InitTypeDef USART_InitStructure;

	if(usart == NULL)
		return -1;

	if(data == 8)
		databit = USART_WordLength_8b;
	else if(data == 9)
		databit = USART_WordLength_9b;
	else
		return -1;

	if((parity == 'N') || (parity == 'n'))
		paritybit = USART_Parity_No;
	else if((parity == 'O') || (parity == 'o'))
		paritybit = USART_Parity_Odd;
	else if((parity == 'E') || (parity == 'e'))
		paritybit = USART_Parity_Even;
	else
		return -1;

	if(stop == 1)
		stopbit = USART_StopBits_1;
	else if(stop == 2)
		stopbit = USART_StopBits_2;
	else
		return -1;

	USART_InitStructure.USART_BaudRate    = (uint16_t)br;
	USART_InitStructure.USART_WordLength  = databit;
	USART_InitStructure.USART_Parity      = paritybit;
	USART_InitStructure.USART_StopBits    = stopbit;
	USART_InitStructure.USART_Mode        = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

	if(usart == USART1) {
		// enable peripheral clock
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
		// configure peripheral
		USART_Init(USART1, &USART_InitStructure);
		// enable usart
		USART_Cmd(USART1, ENABLE);
	} else if(usart == USART2) {
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);		
		USART_Init(USART2, &USART_InitStructure); 
		USART_Cmd(USART2, ENABLE);
	} else {
		return -1;
	}
	return 0;
}

static void reset_usart(usart_dev_t *usart)
{
	USART_DeInit(usart);
}

static int init_usart(usart_dev_t *usart)
{
	// reset usart
	reset_usart(usart);
	// config nvic
	if(config_usart_nvic(usart))
		return -1;
	// config gpio pins
	if(config_usart_gpio(usart))
		return -1;
	// config usart baudrate,data,parity,stop bit
	if(config_usart(usart,115200,8,'N',1))
		return -1;
	return 0;
}

static uint8_t getc_usart(usart_dev_t *usart)
{
	while(USART_GetFlagStatus(usart,USART_FLAG_RXNE) == RESET);
	return ((uint8_t)(USART_ReceiveData(usart)));
}

static void putc_usart(usart_dev_t *usart,char ch)
{
	while(USART_GetFlagStatus(usart, USART_FLAG_TXE) == RESET);	
	USART_SendData(usart,ch);
}

int read_usart(usart_dev_t *usart, uint8_t *pbuf, int len)
{
	int rx_len = 0;

	if((usart == NULL) || (pbuf == NULL) || (len <= 0))
		return 0;

	while(len--) {
		*pbuf++ = getc_usart(usart);
		rx_len++;
	}
	return rx_len;
}

int write_usart(usart_dev_t *usart, uint8_t *pdat, int len)
{
	int tx_len = 0;
	
	if((usart == NULL) || (pdat == NULL) || (len <= 0))
		return 0;

	while(len--) {
		putc_usart(usart,*pdat++);
		tx_len++;
	}
	return tx_len;
}

int config_usart_speed(usart_dev_t *usart, int speed)
{
	uint32_t tmpreg = 0x00, apbclock = 0x00;
	uint32_t integerdivider = 0x00;
	uint32_t fractionaldivider = 0x00;
	RCC_ClocksTypeDef RCC_ClocksStatus;

	RCC_GetClocksFreq(&RCC_ClocksStatus);
	if (usart == USART1)
		apbclock = RCC_ClocksStatus.PCLK2_Frequency;
	else
		apbclock = RCC_ClocksStatus.PCLK1_Frequency;

	/* Determine the integer part */
	if ((usart->CR1 & 0x8000) != 0)
		/* Integer part computing in case Oversampling mode is 8 Samples */
		integerdivider = ((25 * apbclock) / (2 * (speed)));    
	else
		/* Integer part computing in case Oversampling mode is 16 Samples */
		integerdivider = ((25 * apbclock) / (4 * (speed)));

	tmpreg = (integerdivider / 100) << 4;

	/* Determine the fractional part */
	fractionaldivider = integerdivider - (100 * (tmpreg >> 4));

	/* Implement the fractional part in the register */
	if ((usart->CR1 & 0x8000) != 0)
		tmpreg |= ((((fractionaldivider * 8) + 50) / 100)) & ((uint8_t)0x07);
	else
		/* if ((USARTx->CR1 & CR1_OVER8_Set) == 0) */
		tmpreg |= ((((fractionaldivider * 16) + 50) / 100)) & ((uint8_t)0x0F);

	/* Write to USART BRR */
	usart->BRR = (uint16_t)tmpreg;

	return 0;
}

/**************************************************************
 *
 *						Serial Device 
 *
 **************************************************************
 */

static usart_dev_t *hwp_usart = DBG_USART;

void serial_init(void)
{
	init_usart(hwp_usart);
}

char serial_getc(void)
{
	return getc_usart(hwp_usart);
}

void serial_putc(char ch)
{
	putc_usart(hwp_usart,ch);
}

int serial_tstc(void)
{
	return (USART_GetFlagStatus(hwp_usart,USART_FLAG_RXNE) == SET) ? 1 : 0;
}

void serial_puts(const char *pstr)
{
	while(*pstr != '\0')
		serial_putc(*pstr++);
}

static void backspace(int cnts)
{
    int i;

    for(i=0; i<cnts; i++) {
        serial_putc('\b');
        serial_putc(' ');
        serial_putc('\b');
    }
}

int serial_gets(char *pstr)
{
    int length;

    length = 0;
    while(1) {
        pstr[length] = serial_getc();
        if(pstr[length] == '\r') {
            pstr[length] = 0x00;
            break;
        } else if( pstr[length] == '\b' ) {
            if(length>0) {
                length--;
                backspace(1);
            }
        } else {
            serial_putc(pstr[length]);
            length++;
        }

        if(length > 32)
            return -1;
    }
    return 0;
}

int serial_read(uint8_t *buf,int len)
{
	return read_usart(hwp_usart,buf,len);
}

int serial_write(uint8_t *data,int len)
{
	return write_usart(hwp_usart,data,len);
}

void oct2asc(uint8_t *pval,char *pstr,int len)
{
	char str[64];
	int i = 0;
	uint8_t ch = 0;
	uint64_t mask = 0,n,num = *(uint64_t *)pval;

	len = (len > 8) ? (8):len;
	while(i < len) {
		mask |= (0xFF << (i * 8));
		i++;
	}
	num &= mask;
	i = 0;
	while(num != 0) {
		n = num / 10;
		ch = num - n *10;
		num = n;
		ch += (ch < 10)?('0'):('A'-10);
		str[i] = ch;
		i++;
	}
	while(i > 0) {
		--i;
		*pstr++ = str[i];
	}
	*pstr = '\0';
}

void hex2asc(uint8_t *phex,char *pasc,int hexlen)
{
	uint8_t temp;
	char ch;
	*(pasc+hexlen) = '\0';
	phex += hexlen;
	while(hexlen--) {
		temp = *--phex;
		ch = (temp >> 4) & 0xF;
		ch += (ch < 10)?(0x30):(0x37);
		*pasc++ = ch;
		ch = temp & 0xF;
		ch += (ch < 10)?(0x30):(0x37);
		*pasc++ = ch;
	}
}

uint64_t asc2hex(uint8_t *pstr,int len)
{
	int i = 0;
	uint8_t *p,temp;
	uint64_t val = 0;
	
	p = pstr;
	while(*p != '\0'){
		i++;
		p++;
	}
	if(len > 16)
		len = 16;
	if(i > len)
		i = len;
	
	if((pstr[0] == '0')&&(pstr[1] == 'x'))
		p = pstr + 2;
	else if((pstr[0] == '0')&&(pstr[1] == 'X'))
		p = pstr + 2;
	else
		p = pstr;
	
	while(i--) {
		temp = *p++;
		if((temp >= '0')&&(temp <= '9'))
			temp = temp - 0x30;
		else if((temp >= 'A')&&(temp <= 'F'))
			temp = temp - 0x37;
		else if((temp > 'a')&&(temp <= 'f'))
			temp = temp - 0x20 - 0x37;
		else
			return val;
		val |= temp;
		val <<= 4;
	}
	return val;
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
	lu	 - decimal number, unsigned long
	llu	 - decimal number, unsigned long long
	llx	 - hexadecimal number, unsigned long long
    %    - literal '%'
	c	 - character
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

	char str[64];
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
				serial_putc('%');
				break;
			case 'l':
				if((*(s+1) == 'l') && (*(s+2) == 'x')) {
					uint64_t num = 0;
					num = va_arg(ap,unsigned long long);
					hex2asc((uint8_t *)(&num), str,sizeof(num));
					p = str;
					s += 2;
				}
				else if((*(s+1) == 'l') && (*(s+2) == 'u')) {
					uint64_t num = 0;
					num = va_arg(ap,unsigned long long);
					oct2asc((uint8_t *)(&num), str,sizeof(num));
					p = str;
					s += 2;
				}
				else if(*(s+1) == 'u') {
					iarg = va_arg(ap,int);
					radix = 16;
					p = flex_i2a(iarg, str, radix, sizeof(str), size);
					s += 1;
				} else {
					str[0] = 'l';
					str[1] = 0;
					p = str;
				}
				break;
			case 'c':
				iarg = va_arg(ap, int);
				str[0] = iarg;
				str[1] = 0;
				p = str;
			default: ;
			}

			if (p) serial_puts(p);
		}
		else {
			serial_putc(*s);
		}
		++s;
	}

	va_end(ap);
}
