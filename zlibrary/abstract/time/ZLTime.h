#ifndef __ZLTIME_H__
#define __ZLTIME_H__

#include <sys/timeb.h>

class ZLTime {

public:
	ZLTime();
	long millisecondsTo(const ZLTime &time) const;
	long millisecondsFrom(const ZLTime &time) const { return - millisecondsTo(time); }

private:
	struct timeb myTime;	
};

#endif /* __ZLTIME_H__ */
