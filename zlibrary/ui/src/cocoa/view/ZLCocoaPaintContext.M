/*
 * Copyright (C) 2010 Geometer Plus <contact@geometerplus.com>
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

//#include <algorithm>

#import <Cocoa/Cocoa.h>

#include <ZLUnicodeUtil.h>
//#include <ZLImage.h>

#include "ZLCocoaPaintContext.h"
#include "../image/ZLCocoaImageManager.h"

ZLCocoaPaintContext::ZLCocoaPaintContext() {
	myFont = 0;
	myFontAttributes = [[NSMutableDictionary alloc] init];
}

ZLCocoaPaintContext::~ZLCocoaPaintContext() {
	NSMutableDictionary *fontAttributes = (NSMutableDictionary*)myFontAttributes;
	[fontAttributes release];
}

void ZLCocoaPaintContext::fillFamiliesList(std::vector<std::string> &families) const {
	// TODO: implement
}

const std::string ZLCocoaPaintContext::realFontFamilyName(std::string &fontFamily) const {
	// TODO: implement
	return fontFamily.empty() ? "Times" : fontFamily;
}

void ZLCocoaPaintContext::setFont(const std::string &family, int size, bool bold, bool italic) {
	NSFont *font = [NSFont fontWithName:[NSString stringWithUTF8String:family.c_str()] size:size];
	if (font == nil) {
		font = [NSFont fontWithName:@"Times" size:size];
	}
	if (font == nil) {
		font = [NSFont fontWithName:@"Times" size:20];
	}
	NSMutableDictionary *fontAttributes = (NSMutableDictionary*)myFontAttributes;
	if (bold || italic) {
		int trait =
			(bold ? NSBoldFontMask : NSUnboldFontMask) |
			(italic ? NSItalicFontMask : NSUnitalicFontMask);
		font = [[NSFontManager sharedFontManager] convertFont:font
												  toHaveTrait:trait];
	}
	myFont = font;
	[fontAttributes setObject:font forKey:NSFontAttributeName];
}

void ZLCocoaPaintContext::setColor(ZLColor color, LineStyle style) {
	CGContextSetRGBStrokeColor(myCGContext,
							   color.Red / 255.0,
							   color.Green / 255.0,
							   color.Blue / 255.0,
							   1.0);

	NSMutableDictionary *fontAttributes = (NSMutableDictionary*)myFontAttributes;
	NSColor *fgColor = [NSColor colorWithCalibratedRed:color.Red / 255.0
												 green:color.Green / 255.0 
												  blue:color.Blue / 255.0
												 alpha:1.0];
	[fontAttributes setObject:fgColor forKey:NSForegroundColorAttributeName];
}

void ZLCocoaPaintContext::setFillColor(ZLColor color, FillStyle style) {
	CGContextSetRGBFillColor(myCGContext,
							 color.Red / 255.0,
							 color.Green / 255.0,
							 color.Blue / 255.0,
							 1.0);
}

int ZLCocoaPaintContext::stringWidth(const char *str, int len, bool rtl) const {
	static std::string buffer;
	buffer.assign(str, len);
	
	NSString* nsstring = [NSString stringWithUTF8String:buffer.c_str()];
	NSMutableDictionary *fontAttributes = (NSMutableDictionary*)myFontAttributes;
	NSSize size = [nsstring sizeWithAttributes:fontAttributes];
	return size.width;
}

int ZLCocoaPaintContext::spaceWidth() const {
	NSString* nsstring = [NSString stringWithUTF8String:" "];
	NSMutableDictionary *fontAttributes = (NSMutableDictionary*)myFontAttributes;
	NSSize size = [nsstring sizeWithAttributes:fontAttributes];
	return size.width;
}

int ZLCocoaPaintContext::stringHeight() const {
	return [(NSFont*)myFont pointSize];
}

int ZLCocoaPaintContext::descent() const {
	return - [(NSFont*)myFont descender];
}

void ZLCocoaPaintContext::drawString(int x, int y, const char *str, int len, bool rtl) {
	static std::string buffer;
	buffer.assign(str, len);
	
	NSString* nsstring = [NSString stringWithUTF8String:buffer.c_str()];
	NSPoint point = NSMakePoint(x, height() - y - descent());
	NSMutableDictionary *fontAttributes = (NSMutableDictionary*)myFontAttributes;
	[nsstring drawAtPoint:point withAttributes:fontAttributes];	
}

void ZLCocoaPaintContext::drawImage(int x, int y, const ZLImageData &image) {
	NSImage *nsImage = (NSImage*)((ZLCocoaImageData&)image).nsImage();
	if (nsImage == 0) {
		return;
	}
	[nsImage drawAtPoint:NSMakePoint(x, myHeight - y)
							fromRect:NSZeroRect
					   operation:NSCompositeSourceOver
							fraction:1.0];
}

void ZLCocoaPaintContext::drawImage(int x, int y, const ZLImageData &image, int width, int height, ScalingType type) {
	NSImage *nsImage = (NSImage*)((ZLCocoaImageData&)image).nsImage();
	if (nsImage == 0) {
		return;
	}
	const int realWidth = imageWidth(image, width, height, type);
	const int realHeight = imageHeight(image, width, height, type);
	[nsImage drawInRect:NSMakeRect(x, myHeight - y, realWidth, realHeight)
						 fromRect:NSZeroRect
					  operation:NSCompositeSourceOver
						 fraction:1.0];
}

void ZLCocoaPaintContext::drawLine(int x0, int y0, int x1, int y1) {
	CGContextSetShouldAntialias(myCGContext, false);
	CGContextBeginPath(myCGContext);
	CGContextMoveToPoint(myCGContext, x0, myHeight - y0);
	CGContextAddLineToPoint(myCGContext, x1, myHeight - y1);
	CGContextStrokePath(myCGContext);
	CGContextSetShouldAntialias(myCGContext, true);
}

void ZLCocoaPaintContext::fillRectangle(int x0, int y0, int x1, int y1) {
	CGRect rect;
	rect.origin.x = x0;
	rect.origin.y = myHeight - y0 + 1;
	rect.size.width = x1 - x0 + 1;
	rect.size.height = y0 - y1 - 1;
	CGContextFillRect(myCGContext, rect);
}

void ZLCocoaPaintContext::drawFilledCircle(int x, int y, int r) {
	// TODO: implement
}

void ZLCocoaPaintContext::clear(ZLColor color) {
	setFillColor(color, ZLPaintContext::SOLID_FILL);
	fillRectangle(0, 0, myWidth, myHeight);
}

void ZLCocoaPaintContext::begin(int width, int height) {
	myWidth = width;
	myHeight = height;
	
	myCGContext = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
	CGContextSetLineWidth(myCGContext, 1);
	CGContextSetLineCap(myCGContext, kCGLineCapSquare);
	CGContextSetShouldAntialias(myCGContext, true);
}

void ZLCocoaPaintContext::end() {
}

int ZLCocoaPaintContext::width() const {
	return myWidth;
}

int ZLCocoaPaintContext::height() const {
	return myHeight;
}

// vim:ts=2:sw=2:noet
