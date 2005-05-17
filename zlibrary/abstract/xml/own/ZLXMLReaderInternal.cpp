#include "ZLXMLReaderInternal.h"
#include "../ZLXMLReader.h"

ZLXMLReaderInternal::ZLXMLReaderInternal(ZLXMLReader &reader, const char *encoding) : myReader(reader) {
}

ZLXMLReaderInternal::~ZLXMLReaderInternal() {
}

void ZLXMLReaderInternal::init() {
}

bool ZLXMLReaderInternal::parseBuffer(const char *buffer, size_t len) {
	return false;
}
