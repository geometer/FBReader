#include <iostream>
#include <string>

#include <ZLibrary.h>
#include <ZLFile.h>
#include <ZLInputStream.h>

#include <ZLLanguageDetector.h>


void doIt(const std::string &inputFileName) {
	const size_t BUFFER_SIZE = 50000;
	char *buffer = new char[BUFFER_SIZE];  
	shared_ptr<ZLInputStream> stream = ZLFile(inputFileName).inputStream();
	if (stream.isNull() || !stream->open()) {
		std::cerr << "Couldn't open file " << inputFileName << "\n";
		return;
	}
	size_t length = stream->read(buffer, BUFFER_SIZE);
	std::string out = inputFileName;
	std::cout << "It have been read " << length << " bytes from file " << out.substr(out.find_last_of("/")+1) << "\n";
	stream->close();
	
	shared_ptr<ZLLanguageDetector::LanguageInfo> resultLanguageInfoPtr;
	resultLanguageInfoPtr = ZLLanguageDetector().findInfo(buffer, length);
	if (!resultLanguageInfoPtr.isNull()) {
		std::cout << "Language: " << resultLanguageInfoPtr->Language << "\n";
		std::cout << "Encoding: " << resultLanguageInfoPtr->Encoding << "\n";
	}
	
	delete[] buffer;
}

int main(int argc, char **argv) {
	if (argc == 1) {
		std::cerr << "usage:\n  " << argv[0] << " <file name> [<file name> ...]\n";
	}

	ZLibrary::init(argc, argv);
	
	for (int i = 1; i < argc; ++i) {
		std::cout << argv[i] << ":\n";
		doIt(argv[i]);
	}

	ZLibrary::shutdown();
	return 0;
}
