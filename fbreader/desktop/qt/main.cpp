/*
 * FBReader -- electronic book reader
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

#include <qapplication.h>

#include <unix/ZLUnixFSManager.h>
#include <qt/ZLQtTime.h>
#include <qt/QDialogManager.h>
#include <qt/QImageManager.h>
#include <qt/QDeviceInfo.h>
#include <qt-desktop/QApplicationWindow.h>
#include <qt/QPaintContext.h>

#include "../../common/fbreader/FBReader.h"

int main(int argc, char **argv) {
	QApplication application(argc, argv);

	ZLQtTimeManager::createInstance();
	ZLUnixFSManager::createInstance();
	QDialogManager::createInstance();
	QImageManager::createInstance();
	((QDialogManager&)QDialogManager::instance()).setPixmapPath(QApplicationWindow::ImageDirectory);
	QDeviceInfo::createInstance();

	FBReader *reader = new FBReader(new QPaintContext(), argc == 1 ? std::string() : argv[1]);
	ZLDialogManager::instance().createApplicationWindow(reader);
	reader->initWindow();
	int code = application.exec();
	delete reader;

	QDeviceInfo::deleteInstance();
	QImageManager::deleteInstance();
	QDialogManager::deleteInstance();
	ZLUnixFSManager::deleteInstance();
	ZLQtTimeManager::deleteInstance();

	return code;
}
