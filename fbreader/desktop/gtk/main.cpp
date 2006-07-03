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

#include <gtk/gtk.h>

#include <unix/ZLUnixFSManager.h>
#include <unix/ZLUnixTime.h>
#include <abstract/XMLOptions.h>
#include <gtk/GtkDialogManager.h>
#include <gtk/GtkImageManager.h>
#include <gtk/GtkDeviceInfo.h>
#include <gtk-desktop/GtkPaintContext.h>
#include <gtk-desktop/GtkApplicationWindow.h>

#include "../../common/fbreader/CollectionView.h"
#include "../../common/fbreader/FBReader.h"

const std::string ImageDirectory = ZLApplication::BaseDirectory + ZLApplication::PathDelimiter + "FBReader" + ZLApplication::PathDelimiter + "icons";
const std::string CollectionView::DeleteBookImageFile = ImageDirectory + "/FBReader/remove.png";

int main(int argc, char **argv) {
	gtk_init(&argc, &argv);

	ZLUnixTimeManager::createInstance();
	ZLUnixFSManager::createInstance();
	GtkDialogManager::createInstance();
	GtkImageManager::createInstance();
	((GtkDialogManager&)GtkDialogManager::instance()).setPixmapPath(ImageDirectory);
	XMLOptions::createInstance("FBReader");
	GtkDeviceInfo::createInstance();

	// MSS: use the first argument that gtk did not consume
	FBReader *reader = new FBReader(new GtkPaintContext(), argc == 1 ? std::string() : argv[1]);
	ZLDialogManager::instance().createApplicationWindow(reader);
	reader->initWindow();
	gtk_main();
	delete reader;

	GtkDeviceInfo::deleteInstance();
	XMLOptions::deleteInstance();
	GtkImageManager::deleteInstance();
	GtkDialogManager::deleteInstance();
	ZLUnixFSManager::deleteInstance();
	ZLUnixTimeManager::deleteInstance();

	return 0;
}

// vim:ts=2:sw=2:noet
