#include <string.h>

#include "ZLStringInputStream.h"

int ZLStringInputStream::read(char *buffer, int maxSize) {
	int size = myString.length() - myPosition;
	if (size > maxSize) {
		size = maxSize;
	}
	strncpy(buffer, myString.c_str() + myPosition, size);
	return size;
}
