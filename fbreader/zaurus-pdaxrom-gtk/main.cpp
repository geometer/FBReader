/*
 * FBReader -- electronic book reader
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <gtk/gtk.h>

#include <abstract/ZLXMLReader.h>
#include <unix/ZLUnixFSManager.h>
#include <unix/ZLUnixTime.h>
#include <desktop/XMLOptions.h>
#include <gtk/GtkDialogManager.h>
#include <gtk/GtkDeviceInfo.h>

#include "GtkFBReader.h"
#include "Paths.h"

int main(int argc, char **argv) {
	gtk_disable_setlocale();
	gtk_init(&argc, &argv);

	ZLUnixTimeManager::createInstance();
	ZLUnixFSManager::createInstance();
	ZLXMLReader::setEncodingDescriptionPath(EncodingDescriptionPath);
	XMLOptions::createInstance("FBReader");
	GtkDialogManager::createInstance();
	((GtkDialogManager&)GtkDialogManager::instance()).setPixmapPath(GtkFBReader::ImageDirectory);
	GtkDeviceInfo::createInstance();

	GtkFBReader *reader = new GtkFBReader(argc == 1 ? std::string() : argv[1]);

	((GtkDialogManager&)GtkDialogManager::instance()).setMainWindow(reader->getMainWindow());

	gtk_main();

	GtkDeviceInfo::deleteInstance();
	GtkDialogManager::deleteInstance();
	XMLOptions::deleteInstance();
	ZLUnixFSManager::deleteInstance();
	ZLUnixTimeManager::deleteInstance();

	return 0;
}
