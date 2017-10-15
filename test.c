#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

//线程的实现函数
void *pthread_fun(void *arg)
{
	int count = *(int *)arg;
#if 0
	while(count--)
	{
		printf("hello world \n");
		sleep(1);
	}
#endif
	while(count--)
	{
		printf("hello world \n");
		sleep(1);
		if(count == 5)
		{
			pthread_exit("pthread exit");
		}
	}
}

int main(int argc, const char *argv[])
{
	pthread_t tid;  //线程标识符
	int num = 10; //传参方式
	if(pthread_create(&tid, NULL, pthread_fun, &num) != 0)
	{
		perror("fail to pthread_create");
		exit(1);
	}
	
	printf("pthread create success \n");
	void *ret;
	pthread_join(tid, &ret);//等待那个线程

	printf("ret = %s \n", (char *)ret);
	return 0;
}
