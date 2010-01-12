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

#include <windows.h>

#include <ZLApplication.h>
#include <ZLibrary.h>
#include <ZLUnicodeUtil.h>

#include "../../../../core/src/win32/filesystem/ZLWin32FSManager.h"
#include "../time/ZLWin32Time.h"
#include "../dialogs/ZLWin32DialogManager.h"
#include "../image/ZLWin32ImageManager.h"
#include "../view/ZLWin32PaintContext.h"
#include "../message/ZLWin32Message.h"
#include "../../../../core/src/win32/config/ZLWin32Config.h"
#include "../../../../core/src/win32/encoding/ZLWin32EncodingConverter.h"
#include "../../../../core/src/unix/curl/ZLCurlNetworkManager.h"

const std::string ZLibrary::FileNameDelimiter("\\");
const std::string ZLibrary::PathDelimiter(";");
const std::string ZLibrary::EndOfLine("\r\n");

bool ZLibrary::init(int &argc, char **&argv) {
	WCHAR **wArgv = CommandLineToArgvW(GetCommandLineW(), &argc);
	static std::string arguments;
	std::vector<int> offsets;
	offsets.push_back(0);
	for (int i = 0; i < argc; ++i) {
		ZLUnicodeUtil::Ucs2String wArg;
		for (int j = 0; wArgv[i][j] != 0; ++j) {
			wArg.push_back(wArgv[i][j]);
		}
		std::string arg;
		ZLUnicodeUtil::ucs2ToUtf8(arg, wArg);
		arguments += arg + '\0';
		offsets.push_back(arguments.length());
	}
	argv = new char*[argc];
	for (int i = 0; i < argc; ++i) {
		argv[i] = (char*)arguments.data() + offsets[i];
	}

	parseArguments(argc, argv);

	ZLWin32ConfigManager::createInstance();
	ZLWin32FSManager::createInstance();
	ZLWin32TimeManager::createInstance();
	ZLWin32DialogManager::createInstance();
	ZLWin32CommunicationManager::createInstance();
	ZLWin32ImageManager::createInstance();
	ZLEncodingCollection::Instance().registerProvider(new ZLWin32EncodingConverterProvider());
	ZLCurlNetworkManager::createInstance();

	return true;
}

ZLPaintContext *ZLibrary::createContext() {
	return new ZLWin32PaintContext();
}

void ZLibrary::run(ZLApplication *application) {
	ZLDialogManager::Instance().createApplicationWindow(application);
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
	LCID lcid = GetUserDefaultLCID();
	switch (PRIMARYLANGID(id)) {
		case LANG_ARABIC:
			ourLanguage = "ar";
			break;
		case LANG_BULGARIAN:
			ourLanguage = "bg";
			break;
		case LANG_CATALAN:
			ourLanguage = "ca";
			break;
		case LANG_CHINESE:
			ourLanguage = "zh";
			break;
		case LANG_CZECH:
			ourLanguage = "cs";
			break;
		case LANG_DANISH:
			ourLanguage = "da";
			break;
		case LANG_GERMAN:
			ourLanguage = "de";
			break;
		case LANG_GREEK:
			ourLanguage = "el";
			break;
		case LANG_ENGLISH:
			ourLanguage = "en";
			break;
		case LANG_SPANISH:
			ourLanguage = "es";
			switch (lcid) {
				case 0x040A:
				case 0x0C0A:
					ourCountry = "ES";
					break;
				default:
					ourCountry = "LA";
					break;
			}
			break;
		case LANG_FINNISH:
			ourLanguage = "fi";
			break;
		case LANG_FRENCH:
			ourLanguage = "fr";
			break;
		case LANG_HEBREW:
			ourLanguage = "he";
			break;
		case LANG_HUNGARIAN:
			ourLanguage = "hu";
			break;
		case LANG_ICELANDIC:
			ourLanguage = "is";
			break;
		case LANG_ITALIAN:
			ourLanguage = "it";
			break;
		case LANG_JAPANESE:
			ourLanguage = "ja";
			break;
		case LANG_KOREAN:
			ourLanguage = "ko";
			break;
		case LANG_DUTCH:
			ourLanguage = "nl";
			break;
		case LANG_NORWEGIAN:
			ourLanguage = "no";
			break;
		case LANG_POLISH:
			ourLanguage = "pl";
			break;
		case LANG_PORTUGUESE:
			ourLanguage = "pt";
			switch (SUBLANGID(id)) {
				case SUBLANG_PORTUGUESE_BRAZILIAN:
					ourCountry = "BR";
					break;
				default:
					ourCountry = "PT";
					break;
			}
			break;
		case LANG_ROMANIAN:
			ourLanguage = "ro";
			break;
		case LANG_RUSSIAN:
			ourLanguage = "ru";
			break;
		case LANG_SERBIAN:
			switch (SUBLANGID(id)) {
				case 0x04:
				case 0x10:
					ourLanguage = "hr";
					break;
				case 0x14:
				case 0x20:
				case 0x78:
					ourLanguage = "bs";
					break;
				default:
					ourLanguage = "sr";
					break;
			}
			break;
		case LANG_SLOVAK:
			ourLanguage = "sk";
			break;
		case LANG_ALBANIAN:
			ourLanguage = "sq";
			break;
		case LANG_SWEDISH:
			ourLanguage = "sv";
			break;
		case LANG_THAI:
			ourLanguage = "th";
			break;
		case LANG_TURKISH:
			ourLanguage = "tr";
			break;
		case LANG_URDU:
			ourLanguage = "ur";
			break;
		case LANG_INDONESIAN:
			ourLanguage = "id";
			break;
		case LANG_UKRAINIAN:
			ourLanguage = "uk";
			break;
		case LANG_BELARUSIAN:
			ourLanguage = "be";
			break;
		case LANG_SLOVENIAN:
			ourLanguage = "sl";
			break;
		case LANG_ESTONIAN:
			ourLanguage = "et";
			break;
		case LANG_LATVIAN:
			ourLanguage = "lv";
			break;
		case LANG_LITHUANIAN:
			ourLanguage = "lt";
			break;
		case LANG_FARSI:
			ourLanguage = "fa";
			break;
		case LANG_VIETNAMESE:
			ourLanguage = "vi";
			break;
		case LANG_ARMENIAN:
			ourLanguage = "hy";
			break;
		case LANG_AZERI:
			ourLanguage = "az";
			break;
		case LANG_BASQUE:
			ourLanguage = "eu";
			break;
		case LANG_MACEDONIAN:
			ourLanguage = "mk";
			break;
		case LANG_AFRIKAANS:
			ourLanguage = "af";
			break;
		case LANG_GEORGIAN:
			ourLanguage = "ka";
			break;
		case LANG_FAEROESE:
			ourLanguage = "fo";
			break;
		case LANG_HINDI:
			ourLanguage = "hi";
			break;
		case LANG_MALAY:
			ourLanguage = "ms";
			break;
		case LANG_KAZAK:
			ourLanguage = "kk";
			break;
		case LANG_KYRGYZ:
			ourLanguage = "ky";
			break;
		case LANG_SWAHILI:
			ourLanguage = "sw";
			break;
		case LANG_UZBEK:
			ourLanguage = "uz";
			break;
		case LANG_TATAR:
			ourLanguage = "tt";
			break;
		case LANG_BENGALI:
			ourLanguage = "bn";
			break;
		case LANG_PUNJABI:
			ourLanguage = "pa";
			break;
		case LANG_GUJARATI:
			ourLanguage = "gu";
			break;
		case LANG_ORIYA:
			ourLanguage = "or";
			break;
		case LANG_TAMIL:
			ourLanguage = "ta";
			break;
		case LANG_TELUGU:
			ourLanguage = "te";
			break;
		case LANG_KANNADA:
			ourLanguage = "kn";
			break;
		case LANG_MALAYALAM:
			ourLanguage = "ml";
			break;
		case LANG_ASSAMESE:
			ourLanguage = "as";
			break;
		case LANG_MARATHI:
			ourLanguage = "mr";
			break;
		case LANG_SANSKRIT:
			ourLanguage = "sa";
			break;
		case LANG_MONGOLIAN:
			ourLanguage = "mn";
			break;
		case LANG_GALICIAN:
			ourLanguage = "gl";
			break;
		case LANG_KONKANI:
			ourLanguage = "kok";
			break;
		//case LANG_MANIPURI:
			//ourLanguage = "";
		//case LANG_SINDHI:
			//ourLanguage = "";
		case LANG_SYRIAC:
			ourLanguage = "syr";
			break;
		//case LANG_KASHMIRI:
			//ourLanguage = "";
		case LANG_NEPALI:
			ourLanguage = "ne";
			break;
		case LANG_DIVEHI:
			ourLanguage = "dv";
			break;
		default:
			ourLanguage = "en";
			break;
	}
}
