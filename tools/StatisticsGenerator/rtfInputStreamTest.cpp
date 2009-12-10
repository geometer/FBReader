#include "RtfReaderStream.h"
#include <shared_ptr.h>
#include <iostream>
#include <string>
#include <ZLibrary.h>
#include <ZLFile.h>
#include <ZLInputStream.h>
#include <ZLOutputStream.h>

int main (int argc, char **argv) {
	ZLibrary::init(argc, argv);
	const size_t len = 4096;
	shared_ptr<ZLInputStream> newStream = new RtfReaderStream(argv[1], len);
	char* buffer = new char[len];
	newStream->open();
	std::cerr << "open" << '\n';
	newStream->read(buffer, len);
	newStream->close();
	std::cerr << "reading done" << '\n';
	shared_ptr<ZLOutputStream> stream = ZLFile("test.txt").outputStream();
	stream->open();
	std::cerr << "open" << '\n';
	stream->write(buffer, len);
	std::cerr << "writing done" << '\n';
	stream->close();
	delete[] buffer;
	ZLibrary::shutdown();
	return 0;
}
