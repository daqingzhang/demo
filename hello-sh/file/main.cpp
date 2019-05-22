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

char databuf[1024 * 10];

int test_datafile(int argc, const char *argv[])
{
	class clsDataFile dfile, *f;
	int r;
	const char *fn = "a.txt";
	char *pbuf = databuf;
	int buflen=128;
	int datlen, i;

	f = &dfile;

	if (argc > 0) {
		printf("argv[0]=%s\n", argv[0]);
	}

	if (argc > 1) {
		fn = argv[1];
	}

	printf("fn: %s\n", fn);

	r = f->fileOpen(fn, OFLAG_WR);
	if (r) {
		goto err;
	}

	for (i = 0; i < buflen; i++)
		pbuf[i] = i;

#if 0
	r = f->fileWrite(pbuf, buflen);
	printf("fileWRite, r=%d\n", r);
#endif
	memset(pbuf, 0x0, buflen);

	datlen = f->atEnd();
	printf("atEnd: %d\n", datlen);

	datlen = f->atStart();
	printf("atStart: %d\n", datlen);

	datlen = f->getSize();
	printf("getSize: %d\n", datlen);

	datlen = f->atPos(8);
	datlen = f->getPos();
	printf("setPos, getPos: %d\n", datlen);

	datlen = f->getSize() - f->getPos();
	r = f->fileRead(pbuf, datlen);
	printf("read %d bytes, while datlen=%d\n", r, datlen);

	printf("\n");
	for(i = 0; i < r; i += 8) {
		printf("%02x %02x %02x %02x %02x %02x %02x %02x\n",
		pbuf[i+0], pbuf[i+1], pbuf[i+2], pbuf[i+3],
		pbuf[i+4], pbuf[i+5], pbuf[i+6], pbuf[i+7]);
	}
	printf("%s, done\n", __func__);
	return 0;
err:
	printf("%s, error %d\n", __func__, r);
	return 0;
}


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
