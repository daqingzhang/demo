#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/syscall.h>

static pid_t gettid(void)
{
	return syscall(__NR_gettid);
}

static int gdat = 100;

enum {
	CMD_STOP,
	CMD_START,
};

volatile static int cmd = 0;

void *subthread1(void *data)
{
	int *count = data;

	printf("%s, thread id=%d\n", __func__, (int)(pthread_self()));
	printf("%s, pid=%d, tid=%d\n", __func__, (int)getpid(), (int)gettid());

	while(1) {
		printf("hello world, %d\n", *count);
		*count = *count + 1;
		sleep(1);

		if(cmd == CMD_STOP)
			break;
	}
	printf("exit from subthread\n");
	return count;
}

void *subthread2(void *data)
{
	int *exitval = &gdat;

	printf("%s, thread id=%d\n", __func__, (int)(pthread_self()));
	printf("%s, pid=%d, tid=%d\n", __func__, (int)getpid(), (int)gettid());

	while(1) {
		char ch;

		printf(">\n");
		ch = getchar();
	//	ch = getchar_unlocked();
		printf("%c\n", ch);

		if(ch == 'q')
			cmd = CMD_STOP;
		else
			cmd = CMD_START;

		usleep(10);
		if(cmd == CMD_STOP)
			break;
	}
	pthread_exit((void *)exitval);

	return NULL;
}

int main(int argc, const char *argv[])
{
	pthread_t tid1,tid2;
	int ret, *subret1, *subret2;
	static int sec = 0;

	cmd = CMD_START;

	printf("this is main thread %d\n", (int)(pthread_self()));

	ret = pthread_create(&tid1, NULL, subthread1, (void *)&sec);
	if(ret) {
		printf("create thread failed, %d\n", ret);
		return ret;
	}
	printf("tid1 = %d\n", (int)tid1);

	ret = pthread_create(&tid2, NULL, subthread2, NULL);
	if(ret) {
		printf("create thread failed, %d\n", ret);
		return ret;
	}
	printf("tid2 = %d\n", (int)tid2);

	printf("wait subthread exit\n");
	ret = pthread_join(tid1, (void **)&subret1);
	if(ret) {
		printf("thread_join failed %d\n", ret);
		return ret;
	}

	ret = pthread_join(tid2, (void **)&subret2);
	if(ret) {
		printf("thread_join failed %d\n", ret);
		return ret;
	}

	printf("subret1 = %d, subret2 = %d, sec = %d\n", *subret1, *subret2, sec);
	return 0;
}
