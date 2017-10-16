#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int wdog_main(int argc, char *argv[])
{
	int fd;
	int ret = 0;

	printf("%s hello\n", __func__);

	fd = open("/dev/watchdog", O_WRONLY);
	if (fd == -1) {
		perror("watchdog");
		exit(EXIT_FAILURE);
	}
	while (1) {
		ret = write(fd, "\0", 1);
		if (ret != 1) {
			ret = -1;
			break;
		}
		sleep(1);
		break;
	}
	close(fd);
	printf("%s finish\n", __func__);
	return ret;
}
