#include <string.h>

#include "ZLStringInputStream.h"

int ZLStringInputStream::read(char *buffer, int maxSize) {
	int size = myString.length() - myPosition;
	if (size > maxSize) {
		size = maxSize;
	}
	strncpy(buffer, myString.data() + myPosition, size);
	myPosition += size;
	return size;
}
