#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef unsigned int uint32_t;

int tmpfile_open(void)
{
	char fname[] = "/tmp/temp2_file.XXXXXX";
	int fd;

	fd = mkstemp(fname);
	unlink(fname);

	printf("%s: fd=%d, file name=%s\n", __func__, fd, fname);
	return fd;
}

int tmpfile_close(int fd)
{
	close(fd);
	printf("%s: fd=%d\n", __func__, fd);
	return 0;
}

int tmpfile_set_pos(int fd, int pos)
{
	lseek(fd, pos, SEEK_SET);
	printf("%s: pos=%d\n", __func__, pos);
	return 0;
}

int tmpfile_write(int fd, char *data, uint32_t len)
{
	int r;

	printf("%s: fd=%d, len=%d\n", __func__, fd, (int)len);

	if (data) {
		r = write(fd, data, len);
	}
	printf("%s: done, fd=%d, r=%d\n", __func__, fd, r);
	return fd;
}

int tmpfile_read(int fd, char *buf, uint32_t len)
{
	int r;

	printf("%s: len=%d\n", __func__, len);
	if (len > 0) {
		r = read(fd, buf, len);
		printf("%s: r=%d\n", __func__, r);
		printf("%s: data=%s\n", __func__, buf);
	}
	return r;
}

int main(void)
{
	int fd, rlen;
	char *text = "123456789ABCDEFGHIJK";
	uint32_t len;
	char buf[64] = {0};

	fd = tmpfile_open();

	len = strlen(text);
	tmpfile_write(fd, text, len);

	memset(buf, 0, sizeof(buf));
	tmpfile_set_pos(fd, 0);
	rlen = tmpfile_read(fd, buf, len);
	printf("read %d bytes from fd %d\n", rlen, fd);

	memset(buf, 0, sizeof(buf));
	tmpfile_set_pos(fd, len-5);
	rlen = tmpfile_read(fd, buf, 5);
	printf("read %d bytes from fd %d\n", rlen, fd);

	memset(buf, 0, sizeof(buf));
	tmpfile_set_pos(fd, len-8);
	rlen = tmpfile_read(fd, buf, 8);
	printf("read %d bytes from fd %d\n", rlen, fd);

	memset(buf, 0, sizeof(buf));
	tmpfile_set_pos(fd, len);
	rlen = tmpfile_read(fd, buf, 1);
	printf("read %d bytes from fd %d\n", rlen, fd);
	tmpfile_close(fd);

	printf("test done\n");
	return 0;
}

