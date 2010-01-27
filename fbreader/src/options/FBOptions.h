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

#ifndef __FBOPTIONS_H__
#define __FBOPTIONS_H__

#include <string>
#include <map>

#include <shared_ptr.h>

#include <ZLOptions.h>
#include <ZLTextStyle.h>

class FBOptions {

public:
	static FBOptions& Instance();

private:
	static FBOptions *ourInstance;

public:
	ZLIntegerRangeOption LeftMarginOption;
	ZLIntegerRangeOption RightMarginOption;
	ZLIntegerRangeOption TopMarginOption;
	ZLIntegerRangeOption BottomMarginOption;
	ZLColorOption BackgroundColorOption;
	ZLColorOption RegularTextColorOption;

	ZLColorOption &colorOption(const std::string &style);

private:
	FBOptions();
	FBOptions(const FBOptions&);
	const FBOptions &operator = (const FBOptions&);

private:
	std::map<std::string,shared_ptr<ZLColorOption> > myColorOptions;
};

inline FBOptions& FBOptions::Instance() {
	if (ourInstance == 0) {
		ourInstance = new FBOptions();
	}
	return *ourInstance;
}

#endif /* __FBOPTIONS_H__ */
