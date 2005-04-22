#ifndef __MYZLIB_H__
#define __MYZLIB_H__

#include <PalmOS.h>
#include <PalmTypes.h>

#define NOZLIBDEFS

extern "C" {

#include <SysZLib.h>

};

#ifndef MAX_WBITS
	#define MAX_WBITS   15
#endif

#endif /* __MYZLIB_H__ */
