/*
 * Copyright (C) 2009-2010 Geometer Plus <contact@geometerplus.com>
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

#include <cstdlib>

#include <ZLStringUtil.h>
#include <ZLUnicodeUtil.h>
#include <ZLNetworkUtil.h>
#include <ZLNetworkManager.h>

#include <ZLibrary.h>
#include <ZLDir.h>
#include <ZLFile.h>
#include <ZLTime.h>

#include "LitResUtil.h"

#include "LitResGenresParser.h"

#include "LitResGenre.h"

const std::string LitResUtil::LFROM = "lfrom=51";
const std::string LitResUtil::CURRENCY_SUFFIX = " р.";

static const std::string GENRES_CACHE_PREFIX = "litres_genres_";
static const std::string GENRES_CACHE_SUFFIX = ".xml.gz";


shared_ptr<LitResUtil> LitResUtil::ourInstance;


LitResUtil::LitResUtil() : myGenresValid(false) {
}

LitResUtil &LitResUtil::Instance() {
	if (ourInstance.isNull()) {
		ourInstance = new LitResUtil();
	}
	return *ourInstance;
}

std::string LitResUtil::appendLFrom(const std::string &original) {
	if (original.find("lfrom=") != std::string::npos) {
		return original;
	}
	std::string url = original;
	size_t index = url.rfind('/');
	size_t qindex = url.find('?', index + 1);
	if (qindex == std::string::npos) {
		url.append("?");
	} else {
		url.append("&");
	}
	url.append(LFROM);
	return url;
}

std::string LitResUtil::litresLink(const std::string &path) {
	std::string protocol = "http";
	if (path.find("?sid=") != std::string::npos ||
			path.find("&sid=") != std::string::npos ||
			path.find("?pwd=") != std::string::npos ||
			path.find("&pwd=") != std::string::npos) {
		protocol = "https";
	}
	return appendLFrom(protocol + "://robot.litres.ru/" + path);
}

void LitResUtil::makeDemoUrl(std::string &url, const std::string &bookId) {
	std::string id;
	if (bookId.length() < 8) {
		id.assign(8 - bookId.length(), '0');
	}
	id.append(bookId);
	url = "http://robot.litres.ru/static/trials/";
	url.append(id, 0, 2).append("/");
	url.append(id, 2, 2).append("/");
	url.append(id, 4, 2).append("/");
	url.append(id).append(".fb2.zip");
}


void LitResUtil::validateGenres() {
	if (!myGenresValid) {
		if (loadGenres()) {
			buildGenresTitles(myGenresTree);
			myGenresValid = true;
		}
	}
}

const std::map<std::string, shared_ptr<LitResGenre> > &LitResUtil::genresMap() {
	validateGenres();
	return myGenresMap;
}

const std::vector<shared_ptr<LitResGenre> > &LitResUtil::genresTree() {
	validateGenres();
	return myGenresTree;
}

const std::map<shared_ptr<LitResGenre>, std::string> &LitResUtil::genresTitles() {
	validateGenres();
	return myGenresTitles;
}


void LitResUtil::fillGenreIds(const std::string &tag, std::vector<std::string> &ids) {
	std::vector<std::string> words;
	int index = 0;

	const std::map<shared_ptr<LitResGenre>, std::string> map = genresTitles();

	do {
		int index2 = tag.find(' ', index);
		std::string word = tag.substr(index, index2 - index);
		ZLStringUtil::stripWhiteSpaces(word);
		if (!word.empty()) {
			words.push_back(ZLUnicodeUtil::toLower(word));
		}
		index = index2 + 1;
	} while (index != 0);

	for (std::map<shared_ptr<LitResGenre>, std::string>::const_iterator it = map.begin(); it != map.end(); ++it) {
		const LitResGenre &genre = *it->first;
		std::string title = ZLUnicodeUtil::toLower(it->second);
		bool containsAll = true;
		for (std::vector<std::string>::const_iterator jt = words.begin(); jt != words.end(); ++jt) {
			if (title.find(*jt) == std::string::npos) {
				containsAll = false;
				break;
			}
		}
		if (containsAll) {
			ids.push_back(genre.Id);
		}
	}
}

bool LitResUtil::loadGenres() {
	static const std::string directoryPath = ZLNetworkManager::CacheDirectory();
	static shared_ptr<ZLDir> dir = ZLFile(directoryPath).directory(true);

	const std::string url = litresLink("pages/catalit_genres/");

	myGenresTree.clear();
	myGenresMap.clear();
	myGenresTitles.clear();

	if (dir.isNull()) {
		ZLExecutionData::Vector dataList;
		dataList.push_back(ZLNetworkManager::Instance().createXMLParserData(
			url, new LitResGenresParser(myGenresTree, myGenresMap))
		);
		const std::string error = ZLNetworkManager::Instance().perform(dataList);
		if (!error.empty()) {
			myGenresTree.clear();
			myGenresMap.clear();
			myGenresTitles.clear();
			return false;
		}
		return true;
	}

	std::string cacheName;
	bool cacheValid = false;

	std::vector<std::string> files;
	dir->collectFiles(files, false);
	for (std::vector<std::string>::const_iterator it = files.begin(); it != files.end(); ++it) {
		const std::string &name = *it;
		if (ZLStringUtil::stringStartsWith(name, GENRES_CACHE_PREFIX)) {
			cacheName = name;
		}
	}
	files.clear();

	ZLTime now;

	if (!cacheName.empty()) {
		std::string cacheDate = cacheName.substr(GENRES_CACHE_PREFIX.size(), 8);
		int cacheYear = atoi(cacheDate.substr(0, 4).c_str());
		int cacheMonth = atoi(cacheDate.substr(4, 2).c_str());
		int cacheDay = atoi(cacheDate.substr(6, 2).c_str());
		int daysDiff = (now.year() - cacheYear) * 365 + (now.month() - cacheMonth) * 31 + (now.dayOfMonth() - cacheDay);
		if (daysDiff < 30) {
			cacheValid = true;
		}
		cacheName = dir->itemPath(cacheName);
	}

	if (!cacheValid) {
		std::string yearStr, monthStr, dayStr;
		ZLStringUtil::appendNumber(yearStr, now.year());
		ZLStringUtil::appendNumber(monthStr, now.month());
		ZLStringUtil::appendNumber(dayStr, now.dayOfMonth());
		while (monthStr.size() < 2) {
			monthStr = "0" + monthStr;
		}
		while (dayStr.size() < 2) {
			dayStr = "0" + dayStr;
		}

		const std::string fileName = dir->path() + ZLibrary::FileNameDelimiter + 
			GENRES_CACHE_PREFIX + yearStr + monthStr + dayStr + GENRES_CACHE_SUFFIX;

		const std::string error = ZLNetworkManager::Instance().downloadFile(url, fileName);
		if (!error.empty()) {
			ZLFile(fileName).remove();
		} else {
			ZLFile(cacheName).remove();
			cacheName = fileName;
		}
	}

	if (cacheName.empty()) {
		return false;
	}

	shared_ptr<ZLXMLReader> parser = new LitResGenresParser(myGenresTree, myGenresMap);
	return parser->readDocument(cacheName);
}

void LitResUtil::buildGenresTitles(const std::vector<shared_ptr<LitResGenre> > &genres, const std::string &titlePrefix) {
	for (std::vector<shared_ptr<LitResGenre> >::const_iterator it = genres.begin(); it != genres.end(); ++it) {
		shared_ptr<LitResGenre> genre = *it;
		std::string title = titlePrefix.empty() ? (genre->Title) : (titlePrefix + "/" + genre->Title);
		if (genre->Id.empty()) {
			buildGenresTitles(genre->Children, title);
		} else {
			myGenresTitles[genre] = title;
		}
	}
}
