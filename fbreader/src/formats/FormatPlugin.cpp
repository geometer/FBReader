/*
 * Copyright (C) 2004-2010 Geometer Plus <contact@geometerplus.com>
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

#include <ZLInputStream.h>
#include <ZLLanguageDetector.h>
#include <ZLImage.h>

#include "FormatPlugin.h"

#include "../library/Book.h"

#include <ctime>
#include <iostream>

struct LineTimer
{
	LineTimer(int line) : lastLine(line), start(clock()) {
	}
	~LineTimer() {
	}
	
	void reset(int line)
	{
		std::cerr << "Section " << lastLine << ":" << line << " finished at "
		          << (double(clock() - start) * 1000. / CLOCKS_PER_SEC) << " ms"
		          << std::endl;
		std::cerr.flush();
		lastLine = line;
		start = clock();
	}

	int lastLine;
	clock_t start;
};

void FormatPlugin::detectEncodingAndLanguage(Book &book, ZLInputStream &stream) {
	LineTimer timer(__LINE__);
	std::string language = book.language();
	std::string encoding = book.encoding();
	if (!encoding.empty() && !language.empty()) {
		return;
	}

	PluginCollection &collection = PluginCollection::Instance();
	timer.reset(__LINE__);
	if (language.empty()) {
		language = collection.DefaultLanguageOption.value();
	}
	if (encoding.empty()) {
		encoding = collection.DefaultEncodingOption.value();
	}
	timer.reset(__LINE__);
	if (collection.LanguageAutoDetectOption.value() && stream.open()) {
		timer.reset(__LINE__);
		static const int BUFSIZE = 65536;
		char *buffer = new char[BUFSIZE];
		const size_t size = stream.read(buffer, BUFSIZE);
		stream.close();
		timer.reset(__LINE__);
		ZLLanguageDetector detector;
		timer.reset(__LINE__);
		shared_ptr<ZLLanguageDetector::LanguageInfo> info = detector.findInfo(buffer, size);
		timer.reset(__LINE__);
		delete[] buffer;
		timer.reset(__LINE__);
		if (!info.isNull()) {
			if (!info->Language.empty()) {
				language = info->Language;
			}
			encoding = info->Encoding;
			if ((encoding == "ASCII") || (encoding == "ISO-8859-1") || (encoding == "US-ASCII")) {
				encoding = "windows-1252";
			}
		}
	}
	timer.reset(__LINE__);
	book.setEncoding(encoding);
	book.setLanguage(language);
	timer.reset(__LINE__);
}

void FormatPlugin::detectLanguage(Book &book, ZLInputStream &stream) {
	std::string language = book.language();
	if (!language.empty()) {
		return;
	}

	PluginCollection &collection = PluginCollection::Instance();
	if (language.empty()) {
		language = collection.DefaultLanguageOption.value();
	}
	if (collection.LanguageAutoDetectOption.value() && stream.open()) {
		static const int BUFSIZE = 65536;
		char *buffer = new char[BUFSIZE];
		const size_t size = stream.read(buffer, BUFSIZE);
		stream.close();
		shared_ptr<ZLLanguageDetector::LanguageInfo> info =
			ZLLanguageDetector().findInfo(buffer, size);
		delete[] buffer;
		if (!info.isNull()) {
			if (!info->Language.empty()) {
				language = info->Language;
			}
		}
	}
	book.setLanguage(language);
}

const std::string &FormatPlugin::tryOpen(const ZLFile&) const {
	static const std::string EMPTY = "";
	return EMPTY;
}

shared_ptr<ZLImage> FormatPlugin::coverImage(const ZLFile &file) const {
	return 0;
}
