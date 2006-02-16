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

#include <abstract/ZLEncodingConverter.h>
#include <unix/ZLUnixFSManager.h>
#include <unix/ZLUnixTime.h>
#include <desktop/XMLOptions.h>
#include <qt/QDialogManager.h>
#include <qt/QImageManager.h>
#include <qt/QDeviceInfo.h>

#include "QFBReader.h"
#include "Paths.h"

int main(int argc, char **argv) {
	QApplication application(argc, argv);

	ZLUnixTimeManager::createInstance();
	ZLUnixFSManager::createInstance();
	XMLOptions::createInstance("FBReader");
	QDialogManager::createInstance();
	QImageManager::createInstance();
	((QDialogManager&)QDialogManager::instance()).setPixmapPath(QFBReader::ImageDirectory);
	ZLEncodingConverter::setEncodingDescriptionPath(EncodingDescriptionPath);
	QDeviceInfo::createInstance();

	QFBReader *reader = new QFBReader(argc == 1 ? std::string() : argv[1]);
	application.setMainWidget(reader);
	reader->show();
	int code = application.exec();
	delete reader;

	QDeviceInfo::deleteInstance();
	QImageManager::deleteInstance();
	QDialogManager::deleteInstance();
	XMLOptions::deleteInstance();
	ZLUnixFSManager::deleteInstance();
	ZLUnixTimeManager::deleteInstance();

	return code;
}
