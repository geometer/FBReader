/*
 * Copyright (C) 2004-2010 Geometer Plus <contact@geometerplus.com>
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
#include <stdlib.h>

#include <ZLApplication.h>
#include <ZLibrary.h>
#include <ZLLanguageUtil.h>
#include <ZLFile.h>
#include <ZLDir.h>

#include "../../../../core/src/unix/library/ZLibraryImplementation.h"

#include "../../gtk/filesystem/ZLGtkFSManager.h"
#include "../time/ZLGtkTime.h"
#include "../dialogs/ZLGtkDialogManager.h"
#include "../../gtk/image/ZLGtkImageManager.h"
#include "../view/ZLGtkPaintContext.h"
#include "../message/ZLMaemoMessage.h"
#include "../../../../core/src/unix/xmlconfig/XMLConfig.h"
#include "../../../../core/src/unix/iconv/IConvEncodingConverter.h"
#if MAEMO_VERSION > 2
#include "../network/ZLMaemoNetworkManager.h"
#endif

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
	ZLMaemoCommunicationManager::createInstance();
	ZLGtkImageManager::createInstance();
#if MAEMO_VERSION > 2
	ZLMaemoNetworkManager::createInstance();
#endif

	if (!ZLFile("/usr/lib/more-gconv").directory(false).isNull()) {
		setenv("GCONV_PATH", "/usr/lib/more-gconv", 1);
	}
	ZLEncodingCollection::Instance().registerProvider(new IConvEncodingConverterProvider());
}

ZLPaintContext *ZLGtkLibraryImplementation::createContext() {
	return new ZLGtkPaintContext();
}

void ZLGtkLibraryImplementation::run(ZLApplication *application) {
	ZLDialogManager::Instance().createApplicationWindow(application);
	application->initWindow();
	gtk_widget_set_default_direction(ZLLanguageUtil::isRTLLanguage(ZLibrary::Language()) ? GTK_TEXT_DIR_RTL : GTK_TEXT_DIR_LTR);
	gtk_main();
	delete application;
}
