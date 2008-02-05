/*
 * Copyright (C) 2004-2008 Geometer Plus <contact@geometerplus.com>
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

#include <gtk/gtk.h>

#include <ZLApplication.h>
#include <ZLibrary.h>

#include "../../../../core/src/unix/library/ZLibraryImplementation.h"

#include "../filesystem/ZLGtkFSManager.h"
#include "../time/ZLGtkTime.h"
#include "../dialogs/ZLGtkDialogManager.h"
#include "../image/ZLGtkImageManager.h"
#include "../view/ZLGtkPaintContext.h"
#include "../../unix/message/ZLUnixMessage.h"
#include "../../../../core/src/util/ZLKeyUtil.h"
#include "../../../../core/src/unix/xmlconfig/XMLConfig.h"
#include "../../../../core/src/unix/iconv/IConvEncodingConverter.h"

class ZLGtkLibraryImplementation : public ZLibraryImplementation {

private:
	void init(int &argc, char **&argv);
	ZLPaintContext *createContext();
	void run(ZLApplication *application);
};

void initLibrary() {
	new ZLGtkLibraryImplementation();
}

void ZLGtkLibraryImplementation::init(int &argc, char **&argv) {
	gtk_init(&argc, &argv);

	ZLibrary::parseArguments(argc, argv);

	XMLConfigManager::createInstance();
	ZLGtkFSManager::createInstance();
	ZLGtkTimeManager::createInstance();
	ZLGtkDialogManager::createInstance();
	ZLUnixCommunicationManager::createInstance();
	ZLGtkImageManager::createInstance();
	ZLEncodingCollection::instance().registerProvider(new IConvEncodingConverterProvider());

	ZLKeyUtil::setKeyNamesFileName("keynames-gtk.xml");
}

ZLPaintContext *ZLGtkLibraryImplementation::createContext() {
	return new ZLGtkPaintContext();
}

void ZLGtkLibraryImplementation::run(ZLApplication *application) {
	ZLDialogManager::instance().createApplicationWindow(application);
	application->initWindow();
	gtk_main();
	delete application;
}
