/*
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
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

#include <qpe/qpeapplication.h>

#include <ZLApplication.h>
#include <ZLibrary.h>

#include "../../unix/filesystem/ZLUnixFSManager.h"
#include "../../qt/time/ZLQtTime.h"
#include "../dialogs/QDialogManager.h"
#include "../../qt/image/QImageManager.h"
#include "../view/QPaintContext.h"
#include "../message/ZLQtopiaMessage.h"

void ZLibrary::init(int &argc, char **&argv) {
	new QPEApplication(argc, argv);

	ZLQtTimeManager::createInstance();
	ZLUnixFSManager::createInstance();
	QDialogManager::createInstance();
	ZLQtopiaCommunicationManager::createInstance();
	QImageManager::createInstance();
}

ZLPaintContext *ZLibrary::createContext() {
	return new QPaintContext();
}

void ZLibrary::run(ZLApplication *application) {
	ZLDialogManager::instance().createApplicationWindow(application);
	application->initWindow();
	qApp->exec();
	delete application;
}
