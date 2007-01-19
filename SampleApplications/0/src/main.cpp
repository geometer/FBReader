#ifdef _WIN32
	#include <windows.h>
	#include <iostream>
#endif

#include <ZLibrary.h>

#include "SampleApplication.h"

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR lpCmdLine, int) {
	int argc = 1;
	// TODO: parse lpCmdLine (argv[0] is missing in win32)
	char **argv = &lpCmdLine;
#else
int main(int argc, char **argv) {
#endif
	ZLibrary::init(argc, argv);
	ZLibrary::run(new SampleApplication());
	ZLibrary::shutdown();
	return 0;
}
