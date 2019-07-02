#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>


#include <linux/input.h>
static int event0_fd = -1;
struct input_event ev0[64];

static int handle_event0()
{
	int button = 0, realx=0, realy=0, i, rd;
	rd = read(event0_fd, ev0, sizeof(struct input_event)* 64);
	if(rd < sizeof(struct input_event)) return 0;
	for(i=0;i<rd/sizeof(struct input_event); i++)
	{
		if(EV_ABS == ev0[i].type)
		{
			if(ev0[i].code == 0) {
				realx = ev0[i].value;
			} else if(ev0[i].code == 1) {
				realy = ev0[i].value;
			}
		}
		printf("event(%d):type:%d; code:%3d; value:%3d; realx:%3d; realy:%3d\n",i,ev0[i].type,ev0[i].
code,ev0[i].value,realx,realy);
	}

	return 1;
}

int main(void)
{
	int done = 1;
	event0_fd = open("/dev/input/event0",O_RDWR);
	if(event0_fd <0) {
		printf("open input device error\n");
		return -1;
	}
	while (done)
	{
		printf("begin handle_event0...\n");
		done = handle_event0();
		printf("end handle_event0...\n");
	}
	if(event0_fd > 0)
	{
		close(event0_fd);
		event0_fd = -1;
	}
	return 0;
}

