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

#include <map>

#include <ZLibrary.h>
#include <ZLResource.h>
#include <ZLFile.h>

#include "ZLLanguageList.h"

std::vector<std::string> ZLLanguageList::ourLanguageCodes;

shared_ptr<ZLDir> ZLLanguageList::patternsDirectory() {
	const std::string dirName =
		ZLibrary::ZLibraryDirectory() + ZLibrary::FileNameDelimiter + "languagePatterns.zip";
	return ZLFile(dirName).directory(false);
}

std::string ZLLanguageList::languageName(const std::string &code) {
	return ZLResource::resource("language")[ZLResourceKey(code)].value();
}

const std::vector<std::string> &ZLLanguageList::languageCodes() {
	if (ourLanguageCodes.empty()) {
		std::map<std::string,std::string> languages;
		languages[languageName("zh")] = "zh";

		shared_ptr<ZLDir> dir = patternsDirectory();
		if (!dir.isNull()) {
			std::vector<std::string> fileNames;
			dir->collectFiles(fileNames, false);
			for (std::vector<std::string>::const_iterator it = fileNames.begin(); it != fileNames.end(); ++it) {
				const int index = it->find('_');
				if (index != -1) {
					const std::string code = it->substr(0, index);
					languages[languageName(code)] = code;
				}
			}
		}

		for (std::map<std::string,std::string>::const_iterator it = languages.begin(); it != languages.end(); ++it) {
			ourLanguageCodes.push_back(it->second);
		}
		ourLanguageCodes.push_back("other");
	}
	return ourLanguageCodes;
}
