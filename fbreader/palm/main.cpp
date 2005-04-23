#include <zlib.h>

#include <palm/PalmOptions.h>
#include <palm/ZLPalmFSManager.h>
#include <palm/PalmDialogManager.h>

#include <abstract/ZLInputStream.h>
#include <abstract/ZLOutputStream.h>

#include "PalmFBReader.h"
#include "PalmFBReader-resources.h"

UInt16 ZLibRef = 0;

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
}

DWord PilotMain(Word cmd, Ptr /*cmdPBP*/, Word /*launchFlags*/) {
	Err err = 0;
	
	if ((cmd == sysAppLaunchCmdNormalLaunch) && ((err = StartApplication()) == 0)) {
		EventLoop();

		std::string fileName = "/test1.zip:test1";
		ZLInputStream *istream = ZLPalmFSManager::instance().createInputStream(fileName);
		if (istream != 0) {
			if (istream->open()) {
				char txt[10];
				int size = istream->read(txt, 6);
				txt[size] = '\0';
				ZLOutputStream *ostream = ZLPalmFSManager::instance().createOutputStream("/test3");
				if (ostream->open()) {
					ostream->write(txt);
					ostream->close();
				}
				delete ostream;
				istream->close();
			}
			delete istream;
		}
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
