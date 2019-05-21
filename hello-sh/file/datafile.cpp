#include <stdio.h>
#include <datafile.h>

#define DEBUG printf

clsDataFile::clsDataFile()
{
	f_name=NULL;
	f_opened=0;
	f_flag = OFLAG_RO;
	f_mode = OMODE_NO_BLOCK;
}

clsDataFile::~clsDataFile()
{
	fileClose();
}

int clsDataFile::fileOpen(const char *pathname, enum OFLAG flag)
{
	const char *m;

	if (!pathname) {
		DEBUG("null pathname");
	}
	if (f_opened) {
		DEBUG("file already opened");
		return 0;
	}
	if (flag == OFLAG_RO) {
		m = "r";
	} else if (flag == OFLAG_WO) {
		m = "w";
	} else {
		m = "r+";
	}
	f_stream = fopen(pathname, m);
	if (!f_stream) {
		DEBUG("open file %s failed", pathname);
		return -1;
	}
	f_opened = 1;
	f_name = pathname;
	f_flag = flag;

	DEBUG("%s, name:%s, mode:%d, flag:%d",
		__func__, f_name, f_mode, f_flag);

	DEBUG("%s, done", __func__);
	return 0;
}

int clsDataFile::fileClose(void)
{
	int r = 0;

	if (f_opened) {
		r = fclose(f_stream);
		f_opened = 0;
	}
	DEBUG("%s, done", __func__);
	return r;
}

int clsDataFile::fileRead(char *buf, int bytes)
{
	int len = 0;

	if  (!buf) {
		return -1;
	}
	if (!f_opened) {
		return -2;
	}

	len = fread((void *)buf, 1, bytes, f_stream);

	if (len != bytes) {
		DEBUG("%s, read %d bytes while bytes=%d", __func__, len, bytes);
	}
	DEBUG("%s, read %d bytes", __func__, len);
	return len;
}

int clsDataFile::fileWrite(const char *buf, int bytes)
{
	int len = 0;

	if  (!buf) {
		return -1;
	}
	if (!f_opened) {
		return -2;
	}
	len = fwrite((const void *)buf, 1, bytes, f_stream);

	DEBUG("%s, write %d bytes", __func__, len);
	return len;
}

int clsDataFile::fileFlush(void)
{
	int r = 0;

	if (f_opened)
		r = fflush(f_stream);

	DEBUG("%s, done", __func__);
	return r;
}


int clsDataFile::atStart(void)
{
	int r;

	r = fseek(f_stream, 0, SEEK_SET);

	DEBUG("%s, %d, done", __func__, r);
	return r;
}

int clsDataFile::atEnd(void)
{
	int r;

	r = fseek(f_stream, 0, SEEK_END);
	DEBUG("%s, fseek, r=%d", __func__, r);

	r = ftell(f_stream);
	DEBUG("%s, ftell, r=%d", __func__, r);
	return r;
}

int clsDataFile::atPos(int position)
{
	int r;

	r = fseek(f_stream, position, SEEK_SET);
	DEBUG("%s, fseek, r=%d", __func__, r);

	return position;
}

int clsDataFile::getPos(void)
{
	int r;

	r = ftell(f_stream);
	DEBUG("%s, ftell, r=%d", __func__, r);
	return r;
}

int clsDataFile::getSize(void)
{
	return atEnd();
}

int clsDataFile::isError(void)
{
	int r;

	r = ferror(f_stream);
	if (r) {
		clearerr(f_stream);
	}

	DEBUG("%s, r=%d", __func__, r);
	return r;
}

int clsDataFile::isEnd(void)
{
	int r;

	r = feof(f_stream);
	DEBUG("%s, r=%d", __func__, r);
	return r;
}

