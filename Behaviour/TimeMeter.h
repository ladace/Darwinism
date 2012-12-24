#ifndef _TIMEMETER_H_
#define _TIMEMETER_H_

#include <sys/time.h>
class TimeMeter
{
	struct  timeval start;
	struct  timeval end;
	unsigned  long diff;
public:
	TimeMeter()
	{
		gettimeofday(&start,NULL);
	}
	void timeStart()
	{
		gettimeofday(&start,NULL);
	}

	double timeNow()
	{
		gettimeofday(&end,NULL);
		diff = 1000000 * (end.tv_sec-start.tv_sec)+ end.tv_usec-start.tv_usec;
		return diff/1000.0;
	}
};

#endif
