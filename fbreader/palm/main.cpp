#include <zlib.h>

#include <palm/PalmOptions.h>
#include <palm/ZLPalmFSManager.h>
#include <palm/PalmDialogManager.h>
#include <palm/PalmScreenSize.h>

#include "PalmFBReader.h"
#include "PalmFBReader-resources.h"

UInt32 PilotMain(UInt16 cmd, MemPtr /*cmdPBP*/, UInt16 /*launchFlags*/) {
	if (cmd == sysAppLaunchCmdNormalLaunch) {
		ZLSetup;
		ZLPalmFSManager::createInstance();
		PalmOptions::createInstance("FBReader");
		PalmDialogManager::createInstance();
		PalmScreenSize::createInstance();

		FrmGotoForm(MainFBReaderForm);
		EventLoop();

		PalmScreenSize::deleteInstance();
		PalmDialogManager::deleteInstance();
		PalmOptions::deleteInstance();
		ZLPalmFSManager::deleteInstance();
		ZLTeardown;

		FrmSaveAllForms();
		FrmCloseAllForms();
	}

	return 0;
}
