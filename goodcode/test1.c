int checksum1(int *data)
{
	char i;
	int sum = 0;
	for(i = 0;i < 64;i++)
		sum += data[i];
	return sum;
}

short checksum2(short *data)
{
	unsigned int i;
	short sum = 0;
	for(i = 0;i < 64;i++)
		sum = (short)(sum + data[i]);
	return sum;
}

short checksum3(short *data)
{
	unsigned int i;
	short sum = 0;
	for(i = 0;i < 64;i++)
		sum += data[i];
	return sum;
}

short checksum4(short *data)
{
	unsigned int i;
	int sum = 0;
	for(i = 0;i < 64;i++)
		sum += *(data++);
	return sum;
}

short checksum5(short *data)
{
	unsigned int i;
	int sum = 0;

	for(i = 64;i != 0;i--)
		sum += *(data++);
	return sum;
}

int checksum6_1(int *data)
{
	unsigned int i;
	int sum = 0;

	for(i = 64;i != 0;i--)
		sum += *(data++);
	return sum;
}

int checksum6_2(int *data)
{
	unsigned int i;
	int sum = 0;

	for(i = 0;i < 64;i++)
		sum += *(data++);
	return sum;
}

int checksum7_1(int *data)
{
	unsigned int i = 64;
	int sum = 0;
	while(i--) {
		sum += *(data++);
	}
	return sum;
}

int checksum7_2(int *data)
{
	unsigned int i = 64;
	int sum = 0;

	do {
		sum += *(data++);
	}while(--i);
	return sum;
}

/*
 * convert u32 to hexadecimal number
 * example: 0x1234ABCD --> {0x31,0x32,0x33,0x34,0x41,0x42,0x43,0x44}
 */
void u32_to_hex(unsigned char *phex, unsigned int dat)
{
	int i;
	unsigned int t;

	for(i = 8;i != 0;i--) {
		dat = (dat << 4) | (dat >> 28);//LSL 4 bit
		t = dat & 0xF;
		if(t < 0xA)
			t += 0x30;
		else
			t += 0x37;
		*(phex++) = (unsigned char)t;
	}
}

int add8_1(int a, int b)
{
	return(a+b);
}

char add8_2(char a, char b)
{
	return(a+b);
}

short add8_3(short a,short b)
{
	return(a+b);
}

int add9_1(int a,int b,int c, int d)
{
	a += b;
	a += c;
	a += d;
	return a;
}

int add9_2(int a,int b,int c, int d,int e)
{
	a += b;
	a += c;
	a += d;
	a += e;
	return a;
}

int add9_3(int a,int b,int c, int d,int e,int f)
{
	a += b;
	a += c;
	a += d;
	a += e;
	a += f;
	return a;
}

struct abc
{
	int a;
	int b;
	int c;
	int d;
	int e;
	int f;
};

int add9_4(struct abc *t)
{
	int n;
	n += t->a;
	n += t->b;
	n += t->c;
	n += t->d;
	n += t->e;
	n += t->f;
	return n;
}
