#include <cctype>

#include "ZLXMLReaderInternal.h"
#include "../ZLXMLReader.h"

ZLXMLReaderInternal::ZLXMLReaderInternal(ZLXMLReader &reader, const char *encoding) : myReader(reader) {
}

ZLXMLReaderInternal::~ZLXMLReaderInternal() {
}

void ZLXMLReaderInternal::init() {
	myState = XML_PARSE_DATA;
}

static const char *ATTRS = { 0 }; 

bool ZLXMLReaderInternal::parseBuffer(const char *buffer, size_t len) {
	const char *start = buffer;
	const char *end = buffer + len;
	for (const char *ptr = start; ptr != end; ptr++) {
		switch (myState) {
			case XML_PARSE_DATA:
				switch (*ptr) {
					case '<':
						if (ptr != start) {
							myReader.characterDataHandler(start, ptr - start);
						}
						start = ptr + 1;
						myState = XML_PARSE_TAG;
						break;
					case '>':
						return false;
					default:
						break;
				}
				break;
			case XML_PARSE_TAG:
				switch (*ptr) {
					case '<':
						return false;
					case '>':
						myTagContents.append(start, ptr - start);
						if (myTagContents.length() > 0) {
							if (myTagContents[0] != '/') {
								size_t tagLen = myTagContents.length();
								size_t nameLen;
								for (nameLen = 0; nameLen < tagLen; nameLen++) {
									if (isspace(myTagContents[nameLen]) || myTagContents[nameLen] == '/') {
										break;
									}
								}
								myReader.startElementHandler(myReader.tag(myTagContents.substr(0, nameLen).c_str()), &ATTRS);
								if (myTagContents[tagLen - 1] == '/') {
									myReader.endElementHandler(myReader.tag(myTagContents.substr(0, nameLen).c_str()));
								}
							} else {
								myReader.endElementHandler(myReader.tag(myTagContents.substr(1).c_str()));
							}
						}
						myTagContents.erase();
						start = ptr + 1;
						myState = XML_PARSE_DATA;
						break;
					default:
						break;
				}
		}
	}
	if (end != start) {
		switch (myState) {
			case XML_PARSE_DATA:
				myReader.characterDataHandler(start, end - start);
				break;
			case XML_PARSE_TAG:
				myTagContents.append(start, end - start);
				break;
		}
	}
	return true;
}
