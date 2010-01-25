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

#include <ZLUnicodeUtil.h>
//#include <ZLImage.h>

#include "ZLCocoaPaintContext.h"
#include "../image/ZLCocoaImageManager.h"

ZLCocoaPaintContext::ZLCocoaPaintContext() {
}

ZLCocoaPaintContext::~ZLCocoaPaintContext() {
}

void ZLCocoaPaintContext::fillFamiliesList(std::vector<std::string> &families) const {
	// TODO: implement
}

const std::string ZLCocoaPaintContext::realFontFamilyName(std::string &fontFamily) const {
	// TODO: implement
	return fontFamily.empty() ? "Arial" : fontFamily;
}

void ZLCocoaPaintContext::setFont(const std::string &family, int size, bool bold, bool italic) {
	//CGContextSetTextMatrix(myCGContext, CGAffineTransformIdentity);

	const std::string family1 = "Arial";
	//CFStringRef myFontName = CFStringCreateWithCString(kCFAllocatorDefault, family1.c_str(), kCFStringEncodingUTF8);
	//CGFontRef myFont = CGFontCreateWithFontName(myFontName);
	//CGContextSetFont(myCGContext, myFont);
	//CGContextSetFontSize(myCGContext, size);
	
	//CGContextSelectFont(myCGContext, family1.c_str(), size, kCGEncodingMacRoman);

	myFont = [UIFont fontWithName:[NSString stringWithUTF8String:family1.c_str()] size:size];
	//NSMutableDictionary *fontAttributes = (NSMutableDictionary*)myFontAttributes;
	//[fontAttributes setObject:font forKey:UIFontAttributeName];
	//[fontAttributes setObject:[NSNumber numberWithFloat:italic ? 0.2 : 0.0] forKey:UIObliquenessAttributeName];

}

void ZLCocoaPaintContext::setColor(ZLColor color, LineStyle style) {
	CGContextRef cgContext = (CGContextRef)myCGContext;
	CGContextSetRGBStrokeColor(cgContext,
							   color.Red / 255.0,
							   color.Green / 255.0,
							   color.Blue / 255.0,
							   1.0);
	CGContextSetRGBFillColor(cgContext,
							 color.Red / 255.0,
							 color.Green / 255.0,
							 color.Blue / 255.0,
							 1.0);
}

void ZLCocoaPaintContext::setFillColor(ZLColor color, FillStyle style) {
	CGContextRef cgContext = (CGContextRef)myCGContext;
	CGContextSetRGBFillColor(cgContext,
							 color.Red / 255.0,
							 color.Green / 255.0,
							 color.Blue / 255.0,
							 1.0);
}

int ZLCocoaPaintContext::stringWidth(const char *str, int len, bool rtl) const {
	static std::string buffer;
	buffer.assign(str, len);
	
	NSString* nsstring = [NSString stringWithUTF8String:buffer.c_str()];
	CGSize size = [nsstring sizeWithFont:(UIFont*)myFont];
	return size.width;
}

int ZLCocoaPaintContext::spaceWidth() const {
	NSString* nsstring = [NSString stringWithUTF8String:" "];
	CGSize size = [nsstring sizeWithFont:(UIFont*)myFont];
	return size.width;
}

int ZLCocoaPaintContext::stringHeight() const {
	NSString* nsstring = [NSString stringWithUTF8String:"X"];
	CGSize size = [nsstring sizeWithFont:(UIFont*)myFont];
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
	CGPoint point = { x, y - stringHeight() };
	[nsstring drawAtPoint:point withFont:(UIFont*)myFont];
}

void ZLCocoaPaintContext::drawImage(int x, int y, const ZLImageData &image) {
	UIImage *uiImage = (UIImage*)((ZLCocoaImageData&)image).uiImage();
	if (uiImage == 0) {
		return;
	}
	int height = image.height();
	[uiImage drawInRect:CGRectMake(x, y - height, image.width(), height)];
}

void ZLCocoaPaintContext::drawImage(int x, int y, const ZLImageData &image, int width, int height, ScalingType type) {
	UIImage *uiImage = (UIImage*)((ZLCocoaImageData&)image).uiImage();
	if (uiImage == 0) {
		return;
	}
	const int realWidth = imageWidth(image, width, height, type);
	const int realHeight = imageHeight(image, width, height, type);
	[uiImage drawInRect:CGRectMake(x, y - realHeight, realWidth, realHeight)];
}

void ZLCocoaPaintContext::drawLine(int x0, int y0, int x1, int y1) {
	CGContextRef cgContext = (CGContextRef)myCGContext;
	CGContextSetShouldAntialias(cgContext, false);
	CGContextBeginPath(cgContext);
	CGContextMoveToPoint(cgContext, x0, y0);
	CGContextAddLineToPoint(cgContext, x1, y1);
	CGContextStrokePath(cgContext);
	CGContextSetShouldAntialias(cgContext, true);
}

void ZLCocoaPaintContext::fillRectangle(int x0, int y0, int x1, int y1) {
	CGRect rect;
	rect.origin.x = x0;
	rect.origin.y = y0 - 1;
	rect.size.width = x1 - x0;
	rect.size.height = y1 - y0 + 1;
	CGContextRef cgContext = (CGContextRef)myCGContext;
	CGContextFillRect(cgContext, rect);
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
	
	CGContextRef cgContext = UIGraphicsGetCurrentContext();
	myCGContext = cgContext;
	CGContextSetLineWidth(cgContext, 1);
	CGContextSetLineCap(cgContext, kCGLineCapSquare);
	CGContextSetShouldAntialias(cgContext, true);
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
