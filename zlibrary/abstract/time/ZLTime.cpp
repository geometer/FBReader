#include "ZLTime.h"

ZLTime::ZLTime() {
	ftime(&myTime);
}

long ZLTime::millisecondsTo(const ZLTime &time) const {
	return (time.myTime.time - myTime.time) * 1000 + time.myTime.millitm - myTime.millitm;
}
