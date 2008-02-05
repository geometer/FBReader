/*
 * Copyright (C) 2007-2008 Geometer Plus <contact@geometerplus.com>
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

#include "../../../../core/src/win32/filesystem/ZLWin32FSManager.h"
#include "../time/ZLWin32Time.h"
#include "../dialogs/ZLWin32DialogManager.h"
#include "../image/ZLWin32ImageManager.h"
#include "../view/ZLWin32PaintContext.h"
#include "../message/ZLWin32Message.h"
#include "../../../../core/src/win32/config/ZLWin32Config.h"
#include "../../../../core/src/win32/encoding/ZLWin32EncodingConverter.h"

const std::string ZLibrary::FileNameDelimiter("\\");
const std::string ZLibrary::PathDelimiter(";");
const std::string ZLibrary::EndOfLine("\r\n");

bool ZLibrary::init(int &argc, char **&argv) {
	parseArguments(argc, argv);

	ZLWin32ConfigManager::createInstance();
	ZLWin32FSManager::createInstance();
	ZLWin32TimeManager::createInstance();
	ZLWin32DialogManager::createInstance();
	ZLWin32CommunicationManager::createInstance();
	ZLWin32ImageManager::createInstance();
	ZLEncodingCollection::instance().registerProvider(new ZLWin32EncodingConverterProvider());

	return true;
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
	delete application;
}

std::string ZLibrary::Language() {
	if (!ourLanguage.empty()) {
		return ourLanguage;
	}

	LANGID id = GetUserDefaultUILanguage();
	switch (id & 0xFF) {
		case LANG_ARABIC:
			return "ar";
		case LANG_BULGARIAN:
			return "bg";
		case LANG_CATALAN:
			return "ca";
		case LANG_CHINESE:
			return "zh";
		case LANG_CZECH:
			return "cs";
		case LANG_DANISH:
			return "da";
		case LANG_GERMAN:
			return "de";
		case LANG_GREEK:
			return "el";
		case LANG_ENGLISH:
			return "en";
		case LANG_SPANISH:
			return "es";
		case LANG_FINNISH:
			return "fi";
		case LANG_FRENCH:
			return "fr";
		case LANG_HEBREW:
			return "he";
		case LANG_HUNGARIAN:
			return "hu";
		case LANG_ICELANDIC:
			return "is";
		case LANG_ITALIAN:
			return "it";
		case LANG_JAPANESE:
			return "ja";
		case LANG_KOREAN:
			return "ko";
		case LANG_DUTCH:
			return "nl";
		case LANG_NORWEGIAN:
			return "no";
		case LANG_POLISH:
			return "pl";
		case LANG_PORTUGUESE:
			return "pt";
		case LANG_ROMANIAN:
			return "ro";
		case LANG_RUSSIAN:
			return "ru";
		case LANG_SERBIAN:
			switch ((id >> 16) & 0xFF) {
				case 0x04:
				case 0x10:
					return "hr";
				case 0x14:
				case 0x20:
				case 0x78:
					return "bs";
				default:
					return "sr";
			}
		case LANG_SLOVAK:
			return "sk";
		case LANG_ALBANIAN:
			return "sq";
		case LANG_SWEDISH:
			return "sv";
		case LANG_THAI:
			return "th";
		case LANG_TURKISH:
			return "tr";
		case LANG_URDU:
			return "ur";
		case LANG_INDONESIAN:
			return "id";
		case LANG_UKRAINIAN:
			return "uk";
		case LANG_BELARUSIAN:
			return "be";
		case LANG_SLOVENIAN:
			return "sl";
		case LANG_ESTONIAN:
			return "et";
		case LANG_LATVIAN:
			return "lv";
		case LANG_LITHUANIAN:
			return "lt";
		case LANG_FARSI:
			return "fa";
		case LANG_VIETNAMESE:
			return "vi";
		case LANG_ARMENIAN:
			return "hy";
		case LANG_AZERI:
			return "az";
		case LANG_BASQUE:
			return "eu";
		case LANG_MACEDONIAN:
			return "mk";
		case LANG_AFRIKAANS:
			return "af";
		case LANG_GEORGIAN:
			return "ka";
		case LANG_FAEROESE:
			return "fo";
		case LANG_HINDI:
			return "hi";
		case LANG_MALAY:
			return "ms";
		case LANG_KAZAK:
			return "kk";
		case LANG_KYRGYZ:
			return "ky";
		case LANG_SWAHILI:
			return "sw";
		case LANG_UZBEK:
			return "uz";
		case LANG_TATAR:
			return "tt";
		case LANG_BENGALI:
			return "bn";
		case LANG_PUNJABI:
			return "pa";
		case LANG_GUJARATI:
			return "gu";
		case LANG_ORIYA:
			return "or";
		case LANG_TAMIL:
			return "ta";
		case LANG_TELUGU:
			return "te";
		case LANG_KANNADA:
			return "kn";
		case LANG_MALAYALAM:
			return "ml";
		case LANG_ASSAMESE:
			return "as";
		case LANG_MARATHI:
			return "mr";
		case LANG_SANSKRIT:
			return "sa";
		case LANG_MONGOLIAN:
			return "mn";
		case LANG_GALICIAN:
			return "gl";
		case LANG_KONKANI:
			return "kok";
		//case LANG_MANIPURI:
			//return "";
		//case LANG_SINDHI:
			//return "";
		case LANG_SYRIAC:
			return "syr";
		//case LANG_KASHMIRI:
			//return "";
		case LANG_NEPALI:
			return "ne";
		case LANG_DIVEHI:
			return "dv";
		default:
			return "en";
	}
}
