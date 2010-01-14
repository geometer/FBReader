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

#include <iostream>
//#include <algorithm>

//#include <ZLUnicodeUtil.h>
//#include <ZLImage.h>

#include "ZLCocoaPaintContext.h"
//#include "../image/ZLCocoaImageManager.h"

NSMutableDictionary* myFontAttributes = nil;

ZLCocoaPaintContext::ZLCocoaPaintContext() {
	myFontAttributes = [NSMutableDictionary new];
}

ZLCocoaPaintContext::~ZLCocoaPaintContext() {
	[myFontAttributes release];
}

void ZLCocoaPaintContext::fillFamiliesList(std::vector<std::string> &families) const {
	// TODO: implement
}

const std::string ZLCocoaPaintContext::realFontFamilyName(std::string &fontFamily) const {
	// TODO: implement
	return fontFamily.empty() ? "Arial" : fontFamily;
}

void ZLCocoaPaintContext::setFont(const std::string &family, int size, bool bold, bool italic) {
	const std::string family1 = "Georgia";
	NSFont* font = [NSFont fontWithName:[NSString stringWithUTF8String:family1.c_str()] size:size];
	[myFontAttributes setObject:font forKey:NSFontAttributeName];
	[myFontAttributes setObject:[NSNumber numberWithFloat:italic ? 0.4 : 0.0] forKey:NSObliquenessAttributeName];
}

void ZLCocoaPaintContext::setColor(ZLColor color, LineStyle style) {
	NSColor *fgColor = [NSColor colorWithCalibratedRed:color.Red / 255.0
												 green:color.Green / 255.0
												  blue:color.Blue /255.0
												 alpha:1.0];
	[myFontAttributes setObject:fgColor forKey:NSForegroundColorAttributeName];
}

void ZLCocoaPaintContext::setFillColor(ZLColor color, FillStyle style) {
	// TODO: implement
}

int ZLCocoaPaintContext::stringWidth(const char *str, int len, bool rtl) const {
	static std::string buffer;
	buffer.assign(str, len);
	
	NSString* nsstring = [NSString stringWithUTF8String:buffer.c_str()];
	NSSize size = [nsstring sizeWithAttributes:myFontAttributes];
	return size.width;
}

int ZLCocoaPaintContext::spaceWidth() const {
	NSString* nsstring = [NSString stringWithUTF8String:" "];
	NSSize size = [nsstring sizeWithAttributes:myFontAttributes];
	return size.width;
}

int ZLCocoaPaintContext::stringHeight() const {
	NSString* nsstring = [NSString stringWithUTF8String:"X"];
	NSSize size = [nsstring sizeWithAttributes:myFontAttributes];
	return size.height;
}

int ZLCocoaPaintContext::descent() const {
	// TODO: implement
	return 0;
}

void ZLCocoaPaintContext::drawString(int x, int y, const char *str, int len, bool rtl) {
	static std::string buffer;
	buffer.assign(str, len);
	
	NSString* nsstring = [NSString stringWithUTF8String:buffer.c_str()];
	NSPoint point = NSMakePoint(x, height() - y);
	[nsstring drawAtPoint:point withAttributes:myFontAttributes];
}

void ZLCocoaPaintContext::drawImage(int x, int y, const ZLImageData &image) {
	// TODO: implement
}

void ZLCocoaPaintContext::drawImage(int x, int y, const ZLImageData &image, int width, int height, ScalingType type) {
	// TODO: implement
}

void ZLCocoaPaintContext::drawLine(int x0, int y0, int x1, int y1) {
	// TODO: implement
}

void ZLCocoaPaintContext::fillRectangle(int x0, int y0, int x1, int y1) {
	// TODO: implement
}

void ZLCocoaPaintContext::drawFilledCircle(int x, int y, int r) {
	// TODO: implement
}

void ZLCocoaPaintContext::clear(ZLColor color) {
	NSColor *bgColor = [NSColor colorWithCalibratedRed:color.Red / 255.0
												 green:color.Green / 255.0
												  blue:color.Blue / 255.0
												 alpha:1.0];
	[bgColor setFill];
	[NSBezierPath fillRect: NSMakeRect(0, 0, width(), height())];
}

void ZLCocoaPaintContext::setSize(int width, int height) {
	myWidth = width;
	myHeight = height;
}

int ZLCocoaPaintContext::width() const {
	return myWidth;
}

int ZLCocoaPaintContext::height() const {
	return myHeight;
}

// vim:ts=2:sw=2:noet
