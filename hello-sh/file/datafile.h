#ifndef __DATAFILE_H__
#define __DATAFILE_H__

#include <stdio.h>

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;

enum OMODE {
	OMODE_NO_BLOCK,
	OMODE_BLOCK,
};

enum OFLAG {
	OFLAG_RO, //read only
	OFLAG_WO, //write only
	OFLAG_WR, //write and read
};

class clsDataFile {
private:
	const char *f_name;
	int f_opened;
	enum OFLAG f_flag;
	enum OMODE f_mode;
	FILE *f_stream;
public:
	clsDataFile();
	~clsDataFile();
public:
	int fileOpen(const char *pathname, enum OFLAG flag);
	int fileClose(void);
	int fileRead(char *buf, int bytes);
	int fileWrite(const char *buf, int bytes);
	int fileFlush(void);
	int atStart(void); // return start postion
	int atEnd(void);   // return end position
	int atPos(int position); // return new postion
	int getPos(void);  // return current postion
	int getSize(void); // return file size
	int isError(void); // 1: error, 0: no error
	int isEnd(void);   // 1: is end positioin
};

#endif
