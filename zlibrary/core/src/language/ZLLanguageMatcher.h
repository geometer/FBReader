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

#ifndef __ZLLANGUAGEMATCHER_H__
#define __ZLLANGUAGEMATCHER_H__

#include <set>

#include "ZLLanguageDetector.h"

class ZLLanguageMatcher {

public:
	static const std::string UTF8_ENCODING_NAME;

public:
	ZLLanguageMatcher(shared_ptr<ZLLanguageDetector::LanguageInfo> info);
	virtual ~ZLLanguageMatcher();

	virtual void reset() = 0;

	virtual int criterion() const = 0;
	shared_ptr<ZLLanguageDetector::LanguageInfo> info() const;

private:
	shared_ptr<ZLLanguageDetector::LanguageInfo> myInfo;
};

class ZLWordBasedMatcher : public ZLLanguageMatcher {

public:
	ZLWordBasedMatcher(shared_ptr<ZLLanguageDetector::LanguageInfo> info);

	virtual void processWord(const std::string &word, int length) = 0;
};

class ZLLanguagePatternBasedMatcher : public ZLWordBasedMatcher {

public:
	ZLLanguagePatternBasedMatcher(const std::string &fileName, shared_ptr<ZLLanguageDetector::LanguageInfo> info);

private:
	void reset();
	void processWord(const std::string &word, int length);

	int criterion() const;

private:
	unsigned int myProCounter;
	unsigned int myContraCounter;
	std::set<std::string> myDictionary;
};

class ZLChineseUtf8Matcher : public ZLWordBasedMatcher {

public:
	ZLChineseUtf8Matcher();

private:
	void processWord(const std::string &word, int length);
	void reset();
	int criterion() const;

private:
	unsigned int myChineseCharacterCounter;
	unsigned int myNonChineseCharacterCounter;
};

class ZLChineseMatcher : public ZLLanguageMatcher {

public:
	ZLChineseMatcher(const std::string &encoding);

public:
	void reset();
	int criterion() const;
	virtual void processBuffer(const unsigned char *start, const unsigned char *end) = 0;

protected:
	unsigned int myChineseCharacterCounter;
	unsigned int myNonChineseCharacterCounter;
};

class ZLChineseBig5Matcher : public ZLChineseMatcher {

public:
	ZLChineseBig5Matcher();

private:
	void processBuffer(const unsigned char *start, const unsigned char *end);
};

class ZLChineseGBKMatcher : public ZLChineseMatcher {

public:
	ZLChineseGBKMatcher();

private:
	void processBuffer(const unsigned char *start, const unsigned char *end);
};

#endif /* __ZLLANGUAGEMATCHER_H__ */
