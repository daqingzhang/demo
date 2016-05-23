#include <stdio.h>
#include <lib_base.h>
#include <lib.h>

int add(int a,int b)
{
	return(caculate(a,b,'+'));
}

int sub(int a,int b)
{
	return(caculate(a,b,'-'));
}

int mul(int a,int b)
{
	return(caculate(a,b,'*'));
}

int div(int a,int b)
{
	if(!b)
		return 0xdeaddead;
	return(caculate(a,b,'/'));
}
