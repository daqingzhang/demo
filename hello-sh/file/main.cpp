#include <stdio.h>
#include <unistd.h>
#include <a.h>
#include <basefile.h>
#include <string.h>
#include <datafile.h>

int test_basefile(void)
{
	char dat[]="helloworld\n";
	char buf[100];
	class clsBaseFile bf;
	int r;

	r = bf.createFile("abc.txt", 0x664);
	if (r < 0) {
		printf("create error %d\n", r);
		return -1;
	}

	r = bf.openFile("abc.txt",FIL_FLAG_RW);
	if (r < 0) {
		printf("open error %d\n", r);
		return -1;
	}

	r = bf.writeFile(dat,10);
	if (r < 0) {
		printf("write error %d\n", r);
		return -1;
	}
	bf.closeFile();
	r = bf.openFile("abc.txt",FIL_FLAG_RW);

	r = bf.readFile(buf, 5, 1000);
	if (r < 0) {
		printf("read error %d\n", r);
		return -1;
	}
	printf("read %d bytes\n", r);
	for(int i = 0; i < r; i++) {
		printf("%c", buf[i]);
	}
	printf("\n");

	r = bf.closeFile();
	if (r < 0) {
		printf("close error %d\n", r);
		return -1;
	}
}

extern int test_datafile(int argc, const char *argv[]);

int main(int argc, const char *argv[])
{
	int a=10, b=20;

	printf("hello world\n");
	printf("a+b=%d\n", add(a,b));

	test_datafile(argc, argv);
#if 0
	int fdin=0, fdout=1, ret;
	char dat[]="helloworld\n";
	char buf[100];

	ret = write(fdin, dat, sizeof(dat));
	printf("write ret=%d\n", ret);
	ret = read(fdout, buf, sizeof(dat));
	printf("read ret=%d\n", ret);
#endif

	return 0;
}
