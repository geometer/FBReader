#include <iostream>
#include <string>

#include <ZLibrary.h>
#include <ZLFile.h>
#include <ZLDir.h>
#include <ZLInputStream.h>
#include <ZLOutputStream.h>

#include <ZLStatistics.h>
#include <ZLStatisticsGenerator.h>
#include <ZLStatisticsXMLWriter.h>

const size_t CHAR_SEQUENCE_SIZE = 3;
const size_t TOP_AMOUNT = 300;

int main(int argc, char **argv) {
	if (argc != 2) {
		std::cerr << "usage:\n  " << argv[0] << " <directory name with language examples in txt format>\n";
		return 0;
	}
	ZLibrary::init(argc, argv);

	std::string directoryName = argv[1];
	shared_ptr<ZLDir> directory = ZLFile(directoryName).directory(false);
	if (directory.isNull()) { 
		std::cerr << "cannot open directory  " << directoryName << "\n";
		return 0;
	}
	std::vector<std::string> fileNames;
	directory->collectFiles(fileNames, false);
	ZLMapBasedStatistics tempStatistics, resultStatistics;
	int counter = 0;
	for (std::vector<std::string>::const_iterator it = fileNames.begin(); it != fileNames.end(); ++it) {
		ZLFile file(directory->itemPath(*it));
		if (file.extension() != "txt") {
			continue;
		} else {
			++counter;
		}
		ZLStatisticsGenerator("\n\r ").generate(file.path(), CHAR_SEQUENCE_SIZE, tempStatistics);
		if (counter == 1) {
			resultStatistics = tempStatistics;
			continue;
		}
		resultStatistics.retain(tempStatistics);
	}
	std::string outName (argv[1]);
	outName += "pattern.stat";
	shared_ptr<ZLOutputStream> streamOut = ZLFile(outName).outputStream();
	streamOut->open();
	ZLMapBasedStatistics restrictedResultStatistics = resultStatistics.top(TOP_AMOUNT);
	restrictedResultStatistics.scaleToShort();
	ZLStatisticsXMLWriter(*streamOut).writeStatistics(restrictedResultStatistics);
	streamOut->close();
	ZLibrary::shutdown();
	return 0;
}
