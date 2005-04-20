#include <Extensions/ExpansionMgr/VFSMgr.h>

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

		UInt16  unVol;
		UInt32  ulIter;
		UInt32  ulVFSMgrVer;
		UInt32  ulBytesWritten;
		UInt32  fRef;

		FtrGet(sysFileCVFSMgr, vfsFtrIDVersion, &ulVFSMgrVer);
	  ulIter = vfsIteratorStart;
    VFSVolumeEnumerate(&unVol, &ulIter);

		VFSFileOpen(unVol, "/test", vfsModeWrite | vfsModeCreate | vfsModeTruncate, &fRef);
		VFSFileWrite(fRef, 5, "hello", &ulBytesWritten);
		VFSFileClose(fRef);
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
