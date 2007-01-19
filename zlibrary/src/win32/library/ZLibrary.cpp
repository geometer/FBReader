/*
 * Copyright (C) 2007 Nikolay Pultsin <geometer@mawhrin.net>
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

#include <windows.h>

#include <ZLApplication.h>
#include <ZLibrary.h>

#include "../filesystem/ZLWin32FSManager.h"
#include "../time/ZLWin32Time.h"
#include "../dialogs/ZLWin32DialogManager.h"
/*
#include "../../gtk/image/ZLGtkImageManager.h"
*/
#include "../view/ZLWin32PaintContext.h"

void ZLibrary::init(int &argc, char **&argv) {
	//gtk_init(&argc, &argv);

	ZLWin32FSManager::createInstance();
	ZLWin32TimeManager::createInstance();
	ZLWin32DialogManager::createInstance();
	/*
	ZLGtkImageManager::createInstance();
	*/
}

ZLPaintContext *ZLibrary::createContext() {
	return new ZLWin32PaintContext();
}

void ZLibrary::run(ZLApplication *application) {
	ZLDialogManager::instance().createApplicationWindow(application);
	application->initWindow();

	MSG msg;
	while (GetMessage(&msg, 0, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	/*
	delete application;
	pthread_exit(0);
	*/
}
