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

#include <cctype>
#include <iostream>

#import <Cocoa/Cocoa.h>

#include <ZLUnicodeUtil.h>
#include <ZLStringUtil.h>

#include "ZLCocoaKeyUtil.h"

std::string ZLCocoaKeyUtil::keyName(void *nsEvent) {
	NSEvent *event = (NSEvent*)nsEvent;
	const std::string chars = [[event characters] UTF8String];
	ZLUnicodeUtil::Ucs4Char unicode = -1;
	if (ZLUnicodeUtil::utf8Length(chars) == 1) {
		ZLUnicodeUtil::firstChar(unicode, chars);
	}

	std::cerr << (char)unicode << ":" << [event keyCode] << ":" << [event modifierFlags] << "\n";
	std::cerr << ZLKeyUtil::keyName(unicode, [event keyCode], [event modifierFlags]) << "\n";
	return ZLKeyUtil::keyName(unicode, [event keyCode], [event modifierFlags]);
}
