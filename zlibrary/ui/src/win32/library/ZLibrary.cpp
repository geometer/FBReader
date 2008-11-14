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

void ZLibrary::initLocale() {
	LANGID id = GetUserDefaultUILanguage();
	switch (id & 0xFF) {
		case LANG_ARABIC:
			ourLanguage = "ar";
		case LANG_BULGARIAN:
			ourLanguage = "bg";
		case LANG_CATALAN:
			ourLanguage = "ca";
		case LANG_CHINESE:
			ourLanguage = "zh";
		case LANG_CZECH:
			ourLanguage = "cs";
		case LANG_DANISH:
			ourLanguage = "da";
		case LANG_GERMAN:
			ourLanguage = "de";
		case LANG_GREEK:
			ourLanguage = "el";
		case LANG_ENGLISH:
			ourLanguage = "en";
		case LANG_SPANISH:
			ourLanguage = "es";
			switch ((id >> 16) & 0xFF) {
				case 0x01:
					ourCountry = "ES";
				default:
					ourCountry = "LA";
			}
		case LANG_FINNISH:
			ourLanguage = "fi";
		case LANG_FRENCH:
			ourLanguage = "fr";
		case LANG_HEBREW:
			ourLanguage = "he";
		case LANG_HUNGARIAN:
			ourLanguage = "hu";
		case LANG_ICELANDIC:
			ourLanguage = "is";
		case LANG_ITALIAN:
			ourLanguage = "it";
		case LANG_JAPANESE:
			ourLanguage = "ja";
		case LANG_KOREAN:
			ourLanguage = "ko";
		case LANG_DUTCH:
			ourLanguage = "nl";
		case LANG_NORWEGIAN:
			ourLanguage = "no";
		case LANG_POLISH:
			ourLanguage = "pl";
		case LANG_PORTUGUESE:
			ourLanguage = "pt";
			switch ((id >> 16) & 0xFF) {
				case 0x01:
					ourCountry = "BR";
				default:
					ourCountry = "PT";
			}
		case LANG_ROMANIAN:
			ourLanguage = "ro";
		case LANG_RUSSIAN:
			ourLanguage = "ru";
		case LANG_SERBIAN:
			switch ((id >> 16) & 0xFF) {
				case 0x04:
				case 0x10:
					ourLanguage = "hr";
				case 0x14:
				case 0x20:
				case 0x78:
					ourLanguage = "bs";
				default:
					ourLanguage = "sr";
			}
		case LANG_SLOVAK:
			ourLanguage = "sk";
		case LANG_ALBANIAN:
			ourLanguage = "sq";
		case LANG_SWEDISH:
			ourLanguage = "sv";
		case LANG_THAI:
			ourLanguage = "th";
		case LANG_TURKISH:
			ourLanguage = "tr";
		case LANG_URDU:
			ourLanguage = "ur";
		case LANG_INDONESIAN:
			ourLanguage = "id";
		case LANG_UKRAINIAN:
			ourLanguage = "uk";
		case LANG_BELARUSIAN:
			ourLanguage = "be";
		case LANG_SLOVENIAN:
			ourLanguage = "sl";
		case LANG_ESTONIAN:
			ourLanguage = "et";
		case LANG_LATVIAN:
			ourLanguage = "lv";
		case LANG_LITHUANIAN:
			ourLanguage = "lt";
		case LANG_FARSI:
			ourLanguage = "fa";
		case LANG_VIETNAMESE:
			ourLanguage = "vi";
		case LANG_ARMENIAN:
			ourLanguage = "hy";
		case LANG_AZERI:
			ourLanguage = "az";
		case LANG_BASQUE:
			ourLanguage = "eu";
		case LANG_MACEDONIAN:
			ourLanguage = "mk";
		case LANG_AFRIKAANS:
			ourLanguage = "af";
		case LANG_GEORGIAN:
			ourLanguage = "ka";
		case LANG_FAEROESE:
			ourLanguage = "fo";
		case LANG_HINDI:
			ourLanguage = "hi";
		case LANG_MALAY:
			ourLanguage = "ms";
		case LANG_KAZAK:
			ourLanguage = "kk";
		case LANG_KYRGYZ:
			ourLanguage = "ky";
		case LANG_SWAHILI:
			ourLanguage = "sw";
		case LANG_UZBEK:
			ourLanguage = "uz";
		case LANG_TATAR:
			ourLanguage = "tt";
		case LANG_BENGALI:
			ourLanguage = "bn";
		case LANG_PUNJABI:
			ourLanguage = "pa";
		case LANG_GUJARATI:
			ourLanguage = "gu";
		case LANG_ORIYA:
			ourLanguage = "or";
		case LANG_TAMIL:
			ourLanguage = "ta";
		case LANG_TELUGU:
			ourLanguage = "te";
		case LANG_KANNADA:
			ourLanguage = "kn";
		case LANG_MALAYALAM:
			ourLanguage = "ml";
		case LANG_ASSAMESE:
			ourLanguage = "as";
		case LANG_MARATHI:
			ourLanguage = "mr";
		case LANG_SANSKRIT:
			ourLanguage = "sa";
		case LANG_MONGOLIAN:
			ourLanguage = "mn";
		case LANG_GALICIAN:
			ourLanguage = "gl";
		case LANG_KONKANI:
			ourLanguage = "kok";
		//case LANG_MANIPURI:
			//ourLanguage = "";
		//case LANG_SINDHI:
			//ourLanguage = "";
		case LANG_SYRIAC:
			ourLanguage = "syr";
		//case LANG_KASHMIRI:
			//ourLanguage = "";
		case LANG_NEPALI:
			ourLanguage = "ne";
		case LANG_DIVEHI:
			ourLanguage = "dv";
		default:
			ourLanguage = "en";
	}
}
