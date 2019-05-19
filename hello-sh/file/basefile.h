#ifndef __BASEFILE_H__
#define __BASEFILE_H__

enum {
	FIL_FLAG_RO,
	FIL_FLAG_WO,
	FIL_FLAG_RW,
	FIL_FLAG_APPEND,
};

enum {
	FILE_NOBLOCK,
	FILE_BLOCK,
};

typedef unsigned int uint32_t;

class clsBaseFile {
private:
	const char *file_name;
	int file_fd;
	int file_opened;
	int file_flag;
	int file_mode;
public:
	clsBaseFile();
	~clsBaseFile();
public:
	int createFile(const char *pathname, int mode);
	int openFile(const char *pathname, int flag);
	int readFile(char *buf, uint32_t bytes, int tm);
	int writeFile(const char *buf, uint32_t bytes);
	int flushFile(void);
	int closeFile(void);
};

#endif
