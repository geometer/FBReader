/*
 * Copyright (C) 2010 Geometer Plus <contact@geometerplus.com>
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

#include <ZLApplication.h>
#include <ZLibrary.h>
#include <ZLLanguageUtil.h>

#include "../../../../core/src/unix/library/ZLibraryImplementation.h"

#include "../application/ZLCocoaApplicationWindow.h"
#include "../filesystem/ZLCocoaFSManager.h"
#include "../time/ZLCocoaTimeManager.h"
#include "../dialogs/ZLCocoaDialogManager.h"
#include "../image/ZLCocoaImageManager.h"
#include "../view/ZLCocoaPaintContext.h"
//#include "../../unix/message/ZLUnixMessage.h"
//#include "../../../../core/src/util/ZLKeyUtil.h"
#include "../../../../core/src/unix/xmlconfig/XMLConfig.h"
#include "../../../../core/src/unix/iconv/IConvEncodingConverter.h"
//#include "../../../../core/src/unix/curl/ZLCurlNetworkManager.h"

class ZLCocoaLibraryImplementation : public ZLibraryImplementation {

private:
	void init(int &argc, char **&argv);
	ZLPaintContext *createContext();
	void run(ZLApplication *application);
	
private:
	int myArgc;
	const char **myArgv;
	// NSAutoreleasePool*
	void *myPool;
};

void initLibrary() {
	new ZLCocoaLibraryImplementation();
}

void ZLCocoaLibraryImplementation::init(int &argc, char **&argv) {
	myPool = [[NSAutoreleasePool alloc] init];

	ZLibrary::parseArguments(argc, argv);

	myArgc = argc;
	myArgv = (const char**)argv;

	XMLConfigManager::createInstance();
	ZLCocoaFSManager::createInstance();
	ZLCocoaTimeManager::createInstance();
	ZLCocoaDialogManager::createInstance();

	//ZLUnixCommunicationManager::createInstance();
	ZLCocoaImageManager::createInstance();
	ZLEncodingCollection::Instance().registerProvider(new IConvEncodingConverterProvider());
	//ZLCurlNetworkManager::createInstance();

	//ZLKeyUtil::setKeyNamesFileName("keynames-gtk.xml");
}

ZLPaintContext *ZLCocoaLibraryImplementation::createContext() {
	return new ZLCocoaPaintContext();
}

void ZLCocoaLibraryImplementation::run(ZLApplication *application) {
	new ZLCocoaApplicationWindow(application);
	UIApplicationMain(myArgc, (char**)myArgv, nil, nil);
	delete application;
	[(NSAutoreleasePool*)myPool release];
}
