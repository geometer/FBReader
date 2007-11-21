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

#ifndef __ZLLANGUAGEMATCHER_H__
#define __ZLLANGUAGEMATCHER_H__

#include <set>

#include "ZLLanguageDetector.h"

class ZLLanguageMatcher {

public:
	ZLLanguageMatcher(const std::string &fileName, shared_ptr<ZLLanguageDetector::LanguageInfo> info);

	void reset();
	void processWord(const std::string &word);

	int criterion() const;
	shared_ptr<ZLLanguageDetector::LanguageInfo> info() const;

private:
	unsigned int myProCounter;
	unsigned int myContraCounter;
	std::set<std::string> myDictionary;
	shared_ptr<ZLLanguageDetector::LanguageInfo> myInfo;
};

#endif /* __ZLLANGUAGEMATCHER_H__ */
