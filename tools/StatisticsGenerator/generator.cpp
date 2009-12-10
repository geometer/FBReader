#include <iostream>
#include <string>

#include <ZLibrary.h>
#include <ZLFile.h>
#include <ZLOutputStream.h>

#include <ZLStatistics.h>
#include <ZLStatisticsGenerator.h>
#include <ZLStatisticsXMLWriter.h>

int main(int argc, char **argv) {
    if (argc == 1) {
		std::cerr << "usage:\n  " << argv[0] << " <file name>\n";
		return 1;
    }
    ZLibrary::init(argc, argv);
    std::string outputFileName = argv[1];
	outputFileName += ".stat";
	shared_ptr<ZLOutputStream> stream = ZLFile(outputFileName).outputStream();
	ZLMapBasedStatistics s1,s2;
	ZLStatisticsGenerator("\n\r ").generate(argv[1], 2, s1); 	// Here are available to change char sequence length [second method arg]
	s2 = s1.top(500);	 										// Here are available to change amount of top statistics items
	stream->open();
	ZLStatisticsXMLWriter(*stream).writeStatistics(s2);
	stream->close();
    ZLibrary::shutdown();
    return 0;
}

// vim: ts=4
