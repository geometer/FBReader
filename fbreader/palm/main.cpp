#include <zlib.h>

#include <palm/PalmOptions.h>
#include <palm/ZLPalmFSManager.h>
#include <palm/PalmDialogManager.h>

#include "PalmFBReader.h"
#include "PalmFBReader-resources.h"

static Err StartApplication() {
	ZLSetup;
	ZLPalmFSManager::createInstance();
	PalmOptions::createInstance("FBReader");
	PalmDialogManager::createInstance();
	FrmGotoForm(MainFBReaderForm);
	return 0;
}

static void StopApplication() {
	PalmDialogManager::deleteInstance();
	PalmOptions::deleteInstance();
	ZLPalmFSManager::deleteInstance();
	ZLTeardown;

	FrmSaveAllForms();
	FrmCloseAllForms();
}

UInt32 PilotMain(UInt16 cmd, MemPtr /*cmdPBP*/, UInt16 /*launchFlags*/) {
	Err err = 0;
	
	if ((cmd == sysAppLaunchCmdNormalLaunch) && ((err = StartApplication()) == 0)) {
		EventLoop();
		StopApplication();
	}

	return err;
}
