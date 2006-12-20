#include "Number.h"
#include <stdio.h>

unsigned int Number::strlen() const {
	if (myValue == 0) {
		return 1;
	}

	int value = myValue;
	unsigned int len = 0;
	if (value < 0) {
		value = -value;
		len = 1;
	}

	while (value > 0) {
		len++;
		value /= 10;
	}

	return len;
}

const char* Number::sprintf(char *buffer) const {
	::sprintf(buffer, "%d", myValue);
	return buffer;
}
