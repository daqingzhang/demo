int checksum1(int *data)
{
	char i;
	int sum = 0;
	for(i = 0;i < 64;i++)
		sum += data[i];
	return sum;
}

short checksum3(short *data)
{
	unsigned int i;
	short sum = 0;
	for(i = 0;i < 64;i++)
		sum = (short)(sum + data[i]);
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

