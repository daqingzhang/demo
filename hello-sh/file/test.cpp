#include <stdio.h>
#include <unistd.h>
#include <basefile.h>
#include <string.h>
#include <datafile.h>

static char databuf[1024 * 10];
//static char databuf2[1024 * 10];

class clsDataFile dfile;
const char file_name[]="abc.bin";

int test_read(void)
{
	int r = 0;
	unsigned int len, rlen, i;
	unsigned char *pbuf = (unsigned char *)databuf;
	class clsDataFile *f = &dfile;
	enum OFLAG mode=OFLAG_RO;


	printf("\n==============> %s start ==================>\n", __func__);
	r = f->fileOpen(file_name, mode);
	if (r) {
		printf("open file failed %d\n", r);
		return -1;;
	}

	len = f->getSize();
	printf("getSize: len=%d\n", len);
	if (len == 0) {
		printf("len is 0, exit\n");
		goto _exit;
	}

	rlen = f->fileRead((char *)pbuf, len);
	if (rlen != len) {
		printf("read file failed, rlen=%d, len=%d\n", rlen, len);
		r = -1;
		goto _exit;
	}

	printf("\nread data:\n");
	for(i = 0; i < len; i += 16) {
		printf( "%02X %02X %02X %02X %02X %02X %02X %02X  "
			"%02X %02X %02X %02X %02X %02X %02X %02X\n",
		pbuf[i+0], pbuf[i+1], pbuf[i+2], pbuf[i+3],
		pbuf[i+4], pbuf[i+5], pbuf[i+6], pbuf[i+7],
		pbuf[i+8], pbuf[i+9], pbuf[i+10], pbuf[i+11],
		pbuf[i+12], pbuf[i+13], pbuf[i+14], pbuf[i+15]);
	}

_exit:
	f->fileClose();
	printf("\n==============> %s end ==================>\n", __func__);
	return r;
}

int test_write(void)
{
	int r = 0;
	unsigned int len, wlen, i;
	char *pbuf = databuf;
	class clsDataFile *f = &dfile;
	enum OFLAG mode=OFLAG_WO;

	printf("\n==============> %s start ==================>\n", __func__);
	r = f->fileOpen(file_name, mode);
	if (r) {
		printf("open file failed %d\n", r);
		return -1;;
	}

	len = f->getSize();
	printf("getSize: len=%d\n", len);

	f->atEnd();
	printf("after atEnd, getPos: %d\n", f->getPos());

	len = 256;
	for(i = 0; i < len; i++) {
		pbuf[i] = i;
	}

	wlen = f->fileWrite((const char *)pbuf, len);
	if (wlen != len) {
		printf("read file failed, wlen=%d, len=%d\n", wlen, len);
		r = -1;
		goto _exit;
	}

	len = f->getSize();
	printf("getSize: len=%d\n", len);
_exit:
	f->fileClose();
	printf("\n==============> %s end ==================>\n", __func__);
	return r;
}

int test_read_write(void)
{
	int r = 0;
	unsigned int len, rlen, wlen, i;
	char *pbuf = databuf;
	class clsDataFile *f = &dfile;
	enum OFLAG mode=OFLAG_WR;

	printf("\n==============> %s start ==================>\n", __func__);
	r = f->fileOpen(file_name, mode);
	if (r) {
		printf("open file failed %d\n", r);
		return -1;;
	}

	len = f->getSize();
	printf("getSize: len=%d\n", len);

	f->atEnd();
	printf("after atEnd,   getPos: %d\n", f->getPos());

	f->atStart();
	printf("after atStart, getPos: %d\n", f->getPos());

	i = 8;
	f->atPos(i);
	printf("after atPos(%d),   getPos: %d\n", i, f->getPos());

	rlen = f->fileRead(pbuf, 1);
	printf("AA: pbuf[%d]=%2x\n", i, pbuf[0]);

	pbuf[0] = 0xde;
	len = 1;
	wlen = f->fileWrite((const char *)pbuf, len);
	if (wlen != len) {
		printf("read file failed, wlen=%d, len=%d\n", wlen, len);
		r = -1;
		goto _exit;
	}

	f->atPos(i);
	pbuf[0] = 0;
	rlen = f->fileRead(pbuf, 1);
	printf("BB: pbuf[%d]=%2x\n", i, pbuf[0]);

	len = f->getSize();
	printf("getSize: len=%d\n", len);
_exit:
	f->fileClose();
	printf("\n==============> %s end ==================>\n", __func__);
	return r;
}

int test_datafile(int argc, const char *argv[])
{
	int r;
	const char *s = NULL;

	if (argc > 0) {
		printf("argv[0]=%s\n", argv[0]);
	}

	if (argc > 1) {
		s = argv[1];
	}

	if (!s) {
		printf("null cmd\n");
		return 0;
	}
	printf("%s, s=%s\n", __func__, s);

	if (!strcmp(s, "r")) {
		r =  test_read();
	} else if (!strcmp(s, "w")) {
		r = test_write();
	} else if (!strcmp(s, "rw")) {
		r = test_read_write();
	} else {
		printf("not support: %s\n", s);
	}
	return r;
}

