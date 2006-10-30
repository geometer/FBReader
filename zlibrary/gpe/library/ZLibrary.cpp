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

#include <gtk/gtk.h>
#include <gpe/init.h>

#include <ZLApplication.h>
#include <ZLibrary.h>

#include "../../unix/filesystem/ZLUnixFSManager.h"
#include "../../gtk/time/ZLGtkTime.h"
#include "../dialogs/GtkDialogManager.h"
#include "../../gtk/image/GtkImageManager.h"
#include "../view/GtkPaintContext.h"

void ZLibrary::init(int &argc, char **&argv) {
	gpe_application_init (&argc, &argv);

	ZLUnixFSManager::createInstance();
	ZLGtkTimeManager::createInstance();
	GtkDialogManager::createInstance();
	GtkImageManager::createInstance();
}

ZLPaintContext *ZLibrary::createContext() {
	return new GtkPaintContext();
}

void ZLibrary::run(ZLApplication *application) {
	ZLDialogManager::instance().createApplicationWindow(application);
	application->initWindow();
	gtk_main();
	delete application;
}
