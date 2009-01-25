/*
 * Copyright (C) 2009 Geometer Plus <contact@geometerplus.com>
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
#include <ZLDialogManager.h>
#import "cocoaMain.h"


#include "../../../../core/src/unix/xmlconfig/XMLConfig.h"
#include "../../unix/message/ZLUnixMessage.h"
#include "../../../../core/src/unix/library/ZLibraryImplementation.h"
//#include "../../../../core/src/unix/iconv/IConvEncodingConverter.h"

#include "ZLCocoaFSManager.h"
#include "ZLCocoaTimeManager.h"
#include "ZLCocoaDialogManager.h"

class ZLCocoaLibraryImplementation : public ZLibraryImplementation {
	
private:
	void init(int &argc, char **&argv);
	ZLPaintContext *createContext();
	void run(ZLApplication *application);
};

void initLibrary() {
	NSLog(@"initLibrary");
	new ZLCocoaLibraryImplementation();
}

void ZLCocoaLibraryImplementation::init(int &argc, char **&argv) {
	// FIXME: argc, argv
	NSLog(@"init");
	ZLibrary::parseArguments(argc, argv);
	NSLog(@"init2");
	XMLConfigManager::createInstance();
	ZLCocoaFSManager::createInstance();
	ZLCocoaTimeManager::createInstance();
	ZLCocoaDialogManager::createInstance();
	ZLUnixCommunicationManager::createInstance();
	NSLog(@"init3");
	// ZLCocoaImageManager::createInstance();
	// ZLEncodingCollection::instance().registerProvider(new IConvEncodingConverterProvider());
	// ZLKeyUtil::setKeyNamesFileName("keynames-cocoa.xml");
}

ZLPaintContext *ZLCocoaLibraryImplementation::createContext() {
	//return new ZLCocoaPaintContext();
	NSLog(@"createContext");
	return NULL;
}

void ZLCocoaLibraryImplementation::run(ZLApplication *application) {
	NSLog(@"run");
	//ZLDialogManager::instance().createApplicationWindow(application);
	//application->initWindow();
	NSLog(@"run2");
	//cocoaMain(0, NULL);
	delete application;
}