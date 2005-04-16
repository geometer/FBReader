#include <palm/PalmOptions.h>

#include "PalmFBReader.h"
#include "PalmFBReader-resources.h"

static Err StartApplication() {
	PalmOptions::createInstance("FBReader");
	FrmGotoForm(MainFBReaderForm);
	return 0;
}

static void StopApplication() {
	PalmOptions::deleteInstance();
}

DWord PilotMain(Word cmd, Ptr /*cmdPBP*/, Word /*launchFlags*/) {
	Err err = 0;
	
	if ((cmd == sysAppLaunchCmdNormalLaunch) && ((err = StartApplication()) == 0)) {
		EventLoop();
		StopApplication();
	}

	return err;
}
