#ifndef __RTC_H
#define __RTC_H

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
