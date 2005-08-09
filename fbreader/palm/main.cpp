#include <zlib.h>

#include <abstract/ZLXMLReader.h>
#include <palm/PalmOptions.h>
#include <palm/ZLPalmTime.h>
#include <palm/ZLPalmFSManager.h>
#include <palm/PalmDialogManager.h>
#include <palm/PalmDeviceInfo.h>

#include "PalmFBReader.h"
#include "PalmFBReader-resources.h"

#include "Paths.h"

#include <vector>
#include <string>

PalmFBReader *READER;

UInt32 PilotMain(UInt16 cmd, MemPtr /*cmdPBP*/, UInt16 /*launchFlags*/) {
	if (cmd == sysAppLaunchCmdNormalLaunch) {
		ZLSetup;
		ZLPalmTimeManager::createInstance();
		ZLPalmFSManager::createInstance();
		ZLXMLReader::setEncodingDescriptionPath(EncodingDescriptionPath);
		PalmOptions::createInstance("FBReader");
		PalmDialogManager::createInstance();
		PalmDeviceInfo::createInstance();

		READER = new PalmFBReader();
		FrmGotoForm(MainFBReaderForm);
		EventLoop();
		delete READER;

		PalmDeviceInfo::deleteInstance();
		PalmDialogManager::deleteInstance();
		PalmOptions::deleteInstance();
		ZLPalmFSManager::deleteInstance();
		ZLPalmTimeManager::deleteInstance();
		ZLTeardown;

		FrmSaveAllForms();
		FrmCloseAllForms();
	}

	return 0;
}
