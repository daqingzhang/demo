#include <basefile.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define DEBUG printf

clsBaseFile:: clsBaseFile(void)
{
	file_fd=0;
	file_name=NULL;
	file_flag=0;
	file_mode=0;
	file_opened=0;

	DEBUG("%s\n", __func__);
}

clsBaseFile:: ~clsBaseFile(void)
{
	if (file_opened) {
		closeFile();
	}
	DEBUG("%s\n", __func__);
}

int clsBaseFile::createFile(const char *pathname, int mode)
{
	mode_t m;
	int fd;

	switch(mode) {
	case 0x777:
		m = S_IRUSR | S_IWUSR | S_IXUSR
		  | S_IRGRP | S_IWGRP | S_IXGRP
		  | S_IROTH | S_IWOTH | S_IXOTH;
		break;
	case 0x775:
		m = S_IRUSR | S_IWUSR | S_IXUSR
		  | S_IRGRP | S_IWGRP | S_IXGRP
		  | S_IROTH | S_IXOTH;
		break;
	case 0x664:
	default:
		m = S_IRUSR | S_IWUSR
		  | S_IRGRP | S_IWGRP
		  | S_IROTH;
		break;
	}
	fd = open(pathname, O_CREAT, m);
	if (fd < 0) {
		DEBUG("%s, error %d", __func__, fd);
		return -1;
	}
	close(fd);
	DEBUG("%s, fd=%d done\n", __func__, fd);
	return 0;
}

int clsBaseFile::openFile(const char *pathname, int flag)
{
	int fd, f;

	DEBUG("%s\n", __func__);

	switch(flag) {
	case FIL_FLAG_RO:
		f = O_RDONLY;
		break;
	case FIL_FLAG_WO:
		f = O_WRONLY;
		break;
	case FIL_FLAG_RW:
	default:
		f = O_RDWR;
		break;
	}
	fd = open(pathname, f);
	if (fd < 0) {
		DEBUG("%s, error %d", __func__, fd);
		return -1;
	}

	file_name   = pathname;
	file_flag   = f;
	file_fd     = fd;
	file_opened = 1;

	DEBUG("%s, fd=%d done\n", __func__, fd);
	return 0;
}

int clsBaseFile::readFile(char *buf, uint32_t bytes, int tm)
{
	int ret;
	uint32_t len = 0;
	int fd = file_fd;

	DEBUG("%s, bytes=%d\n", __func__, bytes);

	if (!buf) {
		return -1;
	}
	if (!file_opened) {
		return -2;
	}

	while (1) {
		int rlen = read(fd, buf, bytes);

		if (rlen < 0) {
			DEBUG("%s, error %d\n", __func__, rlen);
			break;
		}
		if (rlen > 0) {
			len += rlen;
		}
		if (len >= bytes) {
			break;
		}
		if (tm > 0)
			tm--;
		if (tm == 0) {
			break;
		}
		usleep(1000);
	}

	return len;
}

int clsBaseFile::writeFile(const char *buf, uint32_t bytes)
{
	int ret;

	DEBUG("%s, count=%d\n", __func__, bytes);

	if (!buf) {
		return -1;
	}
	if (!file_opened) {
		return -2;
	}

	ret = write(file_fd, buf, bytes);
	if (ret != bytes) {
		DEBUG("%s, error %d\n", __func__, ret);
	}
	syncfs(file_fd);
	return ret;
}

int clsBaseFile::flushFile(void)
{
	if (file_fd) {
		int fd = file_fd;
		closeFile();
		file_fd = openFile(file_name, file_flag);
	}
	DEBUG("%s\n", __func__);
	return 0;
}

int clsBaseFile::closeFile(void)
{

	int r = 0;

	if (file_fd) {
		r = close(file_fd);
		file_fd = 0;
		DEBUG("%s, ret=%d\n", __func__, r);
	}
	file_opened = 0;

	return r;
}
