/*
 * Copyright (C) 2007-2010 Geometer Plus <contact@geometerplus.com>
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

#include "../../../../core/src/win32/util/W32WCHARUtil.h"
#include "ZLWin32DialogManager.h"
#include "ZLWin32ProgressDialog.h"
#include "ZLWin32Dialog.h"
#include "../application/ZLWin32ApplicationWindow.h"
#include "../image/ZLWin32ImageManager.h"
#include "ZLWin32OptionsDialog.h"
#include "ZLWin32SelectionDialog.h"
#include "ZLWin32MessageBox.h"

void ZLWin32DialogManager::createApplicationWindow(ZLApplication *application) const {
	myApplicationWindow = new ZLWin32ApplicationWindow(application);
}

void ZLWin32DialogManager::removeApplicationWindow() const {
	myApplicationWindow = 0;
}

shared_ptr<ZLDialog> ZLWin32DialogManager::createDialog(const ZLResourceKey &key) const {
	return new ZLWin32Dialog(*myApplicationWindow, resource()[key]);
}

shared_ptr<ZLOptionsDialog> ZLWin32DialogManager::createOptionsDialog(const ZLResourceKey &key, shared_ptr<ZLRunnable> applyAction, bool showApplyButton) const {
	return new ZLWin32OptionsDialog(*myApplicationWindow, resource()[key], applyAction, showApplyButton);
}

void ZLWin32DialogManager::informationBox(const std::string &title, const std::string &message) const {
	ZLWin32MessageBox box(*myApplicationWindow, W32StandardIcon::ID_INFORMATION, title, message);
	box.addButton(buttonName(OK_BUTTON));
	box.run();
}

void ZLWin32DialogManager::errorBox(const ZLResourceKey &key, const std::string &message) const {
	ZLWin32MessageBox box(*myApplicationWindow, W32StandardIcon::ID_ERROR, dialogTitle(key), message);
	box.addButton(buttonName(OK_BUTTON));
	box.run();
}

int ZLWin32DialogManager::questionBox(const ZLResourceKey &key, const std::string &message, const ZLResourceKey &button0, const ZLResourceKey &button1, const ZLResourceKey &button2) const {
	ZLWin32MessageBox box(*myApplicationWindow, W32StandardIcon::ID_QUESTION, dialogTitle(key), message);
	if (!button0.Name.empty()) {
		box.addButton(buttonName(button0));
	}
	if (!button1.Name.empty()) {
		box.addButton(buttonName(button1));
	}
	if (!button2.Name.empty()) {
		box.addButton(buttonName(button2));
	}
	return box.run();
}

bool ZLWin32DialogManager::selectionDialog(const ZLResourceKey &key, ZLTreeHandler &handler) const {
	return ZLWin32SelectionDialog(*myApplicationWindow, dialogTitle(key), handler).runWithSize();
}

shared_ptr<ZLProgressDialog> ZLWin32DialogManager::createProgressDialog(const ZLResourceKey &key) const {
	return new ZLWin32ProgressDialog(myApplicationWindow, key);
}

bool ZLWin32DialogManager::isClipboardSupported(ClipboardType type) const {
	return type == CLIPBOARD_MAIN;
}

void ZLWin32DialogManager::setClipboardText(const std::string &text, ClipboardType type) const {
	if ((type == CLIPBOARD_MAIN) && !text.empty()) {
		if (OpenClipboard(myApplicationWindow->mainWindow())) {
			ZLUnicodeUtil::Ucs2String ucs2String;
			::createNTWCHARString(ucs2String, text);
			WCHAR *hData = (WCHAR*)GlobalAlloc(GMEM_MOVEABLE, ucs2String.size() * 2);
			WCHAR *copy = (WCHAR*)GlobalLock(hData);
			memcpy(copy, &ucs2String[0], ucs2String.size() * 2);
			GlobalUnlock(hData);
			EmptyClipboard();
			SetClipboardData(CF_UNICODETEXT, hData);
			CloseClipboard();
		}
	}
}

void ZLWin32DialogManager::setClipboardImage(const ZLImageData &image, ClipboardType type) const {
	if (type == CLIPBOARD_MAIN) {
		ZLWin32ImageData &win32Image = (ZLWin32ImageData&)image;
		const BYTE *pixels = win32Image.pixels(ZLColor(255, 255, 255));
		if (pixels != 0) {
			if (OpenClipboard(myApplicationWindow->mainWindow())) {
				EmptyClipboard();
				HDC dc = GetDC(0);
				BITMAPINFOHEADER header;
				ZeroMemory(&header, sizeof(header));
				header.biSize = sizeof(header);
				header.biWidth = image.width();
				header.biHeight = image.height();
				header.biPlanes = 1;
				header.biBitCount = 24;
				header.biCompression = BI_RGB;
				header.biSizeImage = 0;
				HBITMAP bitmap = CreateDIBitmap(
					dc, &header,
					CBM_INIT,
					pixels, win32Image.info(),
					DIB_RGB_COLORS
				);
				SetClipboardData(CF_BITMAP, bitmap);
				ReleaseDC(0, dc);
				CloseClipboard();
			}
		}
	}
}
