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
#include <desktop/XMLOptions.h>
#include <gtk/GtkDialogManager.h>
#include <gtk/GtkScreenSize.h>

#include "GtkFBReader.h"
#include "GtkWord.h"

int main(int argc, char **argv) {
	gtk_disable_setlocale();
	gtk_init(&argc, &argv);

	ZLUnixFSManager::createInstance();
	ZLXMLReader::setEncodingDescriptionPath("share/FBReader/encodings");
	XMLOptions::createInstance("FBReader");
	GtkDialogManager::createInstance();
	GtkWordBuilder::createInstance();
	GtkScreenSize::createInstance();

	new GtkFBReader();
	gtk_main();

	GtkScreenSize::deleteInstance();
	GtkWordBuilder::deleteInstance();
	GtkDialogManager::deleteInstance();
	XMLOptions::deleteInstance();
	ZLUnixFSManager::deleteInstance();

	return 0;
}
