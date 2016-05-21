#include <stdio.h>
#include <../inc/lib.h>

int add(int a,int b)
{
	return (a+b);
}

int sub(int a,int b)
{
	return (a-b);
}

int mul(int a,int c)
{
	return (a + c);
}

int div(int a,int b)
{
	if(!b)
		return 0;
	return(a/b);
}
