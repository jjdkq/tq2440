#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
extern void led_thread(void);
extern void lcd_thread(void);
int main(int argc,char *argv[])
{
    pthread_t id_1,id_2;
	int ret;
	printf("Led thread and lcd thread test!\n");
 
/*创建led线程*/
   	ret=pthread_create(&id_1,NULL,(void  *) led_thread,NULL);
	if(ret)
    {
        printf("Led thread create error!\n");
		exit(-1);
    }
	printf("Led thread OK!\n");
 
/*创建lcd线程*/
    ret=pthread_create(&id_2,NULL,(void  *) lcd_thread,NULL);
    if(ret)
   	{
        printf("Lcd thread create error!\n");
    	exit(-1);
    }
	printf("Lcd thread OK!\n");
	while(1)
	{
		sleep(1);		
		printf("Main thread is running!\n");
	}
    return 0;
}

