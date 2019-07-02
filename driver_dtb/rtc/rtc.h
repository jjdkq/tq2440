#ifndef __RTC_H
#define __RTC_H

#define DEVICE_NAME "rtc"       /*注册驱动时建立的设备名称*/ 
#define RDTIME		1
#define WRTIME		0
	
typedef struct
{
	unsigned short year;
	unsigned mon;
	unsigned day;
	unsigned date;
	unsigned hour;
	unsigned min;
	unsigned sec;
}time_t_;

#endif
