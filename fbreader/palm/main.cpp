/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004, 2005 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005 Mikhail Sobolev <mss@mawhrin.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

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
