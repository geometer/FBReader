#include <ZLibrary.h>

#include "SampleApplication.h"

int main(int argc, char **argv) {
	ZLibrary::init(argc, argv);
	ZLibrary::run(new SampleApplication());
	ZLibrary::shutdown();
	return 0;
}
