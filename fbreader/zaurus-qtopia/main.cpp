/*
 * FBReader -- electronic book reader
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <qpe/qpeapplication.h>

#include <abstract/ZLXMLReader.h>
#include <unix/ZLUnixTime.h>
#include <unix/ZLUnixFSManager.h>
#include <qtopia/QOptions.h>
#include <qtopia/QDialogManager.h>
#include <qtopia/QScreenSize.h>

#include "QFBReader.h"
#include "Paths.h"

int main(int argc, char **argv) {
	QPEApplication application(argc, argv);

	ZLUnixTimeManager::createInstance();
	ZLUnixFSManager::createInstance();
	ZLXMLReader::setEncodingDescriptionPath(EncodingDescriptionPath);
	QOptions::createInstance("FBReader");
	QDialogManager::createInstance();
	((QDialogManager&)QDialogManager::instance()).setPixmapPath(QFBReader::ImageDirectory);
	QScreenSize::createInstance();

	QFBReader *reader = new QFBReader();
	application.showMainWidget(reader);
	int code = application.exec();
	delete reader;

	QScreenSize::deleteInstance();
	QDialogManager::deleteInstance();
	QOptions::deleteInstance();
	ZLUnixFSManager::deleteInstance();
	ZLUnixTimeManager::deleteInstance();

	return code;
}
