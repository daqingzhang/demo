#include <stdio.h>
#include <lib_base.h>

int caculate(int a,int b,const char op)
{
	int r = 0;

	switch(op) {
	case '+':
		r = a + b;
		break;
	case '-':
		r = a - b;
		break;
	case '*':
		r = a * b;
		break;
	case '/':
		r = a / b;
		break;
	default:
		r = 0xdeaddead;
		break;
	}
	return r;
}
