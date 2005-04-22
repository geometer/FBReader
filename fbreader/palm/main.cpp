#include <Extensions/ExpansionMgr/VFSMgr.h>
#include <zlib.h>

#include <palm/PalmOptions.h>
#include <palm/ZLPalmFSManager.h>

#include <abstract/ZLOutputStream.h>

#include "PalmFBReader.h"
#include "PalmFBReader-resources.h"

UInt16 ZLibRef = 0;

static Err StartApplication() {
	ZLSetup;
	ZLPalmFSManager::createInstance();
	PalmOptions::createInstance("FBReader");
	FrmGotoForm(MainFBReaderForm);
	return 0;
}

static void StopApplication() {
	PalmOptions::deleteInstance();
	ZLPalmFSManager::deleteInstance();
}

DWord PilotMain(Word cmd, Ptr /*cmdPBP*/, Word /*launchFlags*/) {
	Err err = 0;
	
	if ((cmd == sysAppLaunchCmdNormalLaunch) && ((err = StartApplication()) == 0)) {
		EventLoop();

		ZLOutputStream *stream = ZLPalmFSManager::instance().createOutputStream("/test1");
		if (stream->open()) {
			stream->write("HELLO1");
			stream->close();
		}
		delete stream;
		/*
		if (code != 0) {
			char txt[4];
			txt[0] = '0' + code / 100;
			txt[1] = '0' + code / 10 % 10;
			txt[2] = '0' + code % 10;
			txt[3] = '\0';
			FrmCustomAlert(GoodnightMoonAlert, txt, 0, 0);
		}
		*/
		StopApplication();
	}

	return err;
}
