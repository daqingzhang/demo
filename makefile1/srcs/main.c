#include <stdio.h>
#include <lib.h>

int main(void)
{
	int i=0;
	int a = 0,b = 1,t = 0;
	int sum = 10;

	// to print fibocci number
	for(i = 0; i < sum; i++)
	{
		t = a + b;
		a = b;
		b = t;
		printf("%d\n",t);
	}
	print("hello world\n");
	printf("%d + %d = %d\n",a,b,add(a,b));
	printf("%d - %d = %d\n",a,b,sub(a,b));
	printf("%d * %d = %d\n",a,b,mul(a,b));
	printf("%d / %d = %d\n",a,b,div(a,b));
	return 0;
}
