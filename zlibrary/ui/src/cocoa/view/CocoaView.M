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

#include <ZLApplication.h>

#import "CocoaView.h"

#include "ZLCocoaPaintContext.h"

@implementation CocoaView

- (id)initWithFrame:(NSRect)frame {
	self = [super initWithFrame:frame];
	if (self) {
		ZLApplication::Instance().initWindow();
	}
	return self;
}

- (void)drawRect:(NSRect)dirtyRect {
	shared_ptr<ZLView> view = ZLApplication::Instance().currentView();
	if (view.isNull()) {
		return;
	}

	ZLCocoaPaintContext &context = (ZLCocoaPaintContext&)view->context();
	NSSize size = [self bounds].size;
	context.begin(size.width, size.height);
	view->paint();
	context.end();
}

#include <iostream>

- (void)mouseDown:(NSEvent*)theEvent {
	shared_ptr<ZLView> view = ZLApplication::Instance().currentView();
	if (view.isNull()) {
		return;
	}
	NSPoint location = [theEvent locationInWindow];
	NSSize size = [self bounds].size;
	view->onStylusPress(location.x, size.height - location.y);
}

- (void)mouseUp:(NSEvent*)theEvent {
	shared_ptr<ZLView> view = ZLApplication::Instance().currentView();
	if (view.isNull()) {
		return;
	}
	NSPoint location = [theEvent locationInWindow];
	NSSize size = [self bounds].size;
	view->onStylusRelease(location.x, size.height - location.y);
}

- (void)mouseMoved:(NSEvent*)theEvent {
	shared_ptr<ZLView> view = ZLApplication::Instance().currentView();
	if (view.isNull()) {
		return;
	}
	NSPoint location = [theEvent locationInWindow];
	NSSize size = [self bounds].size;
	view->onStylusMove(location.x, size.height - location.y);
}

- (void)mouseDragged:(NSEvent*)theEvent {
	shared_ptr<ZLView> view = ZLApplication::Instance().currentView();
	if (view.isNull()) {
		return;
	}
	NSPoint location = [theEvent locationInWindow];
	NSSize size = [self bounds].size;
	view->onStylusMovePressed(location.x, size.height - location.y);
}

- (BOOL)acceptsFirstResponder {
	return YES;
}

- (void)keyDown:(NSEvent*)theEvent {
	NSString *chars = [theEvent characters];
	if ([chars isEqualToString:@"l"]) {
		ZLApplication::Instance().doActionByKey("<L>");
		return;
	}
	if ([chars isEqualToString:@"c"]) {
		ZLApplication::Instance().doActionByKey("<C>");
		return;
	}
	switch ([theEvent keyCode]) {
		case 116:
			ZLApplication::Instance().doActionByKey("<PageUp>");
			return;
		case 121:
			ZLApplication::Instance().doActionByKey("<PageDown>");
			return;
		case 125:
			ZLApplication::Instance().doActionByKey("<DownArrow>");
			return;
		case 126:
			ZLApplication::Instance().doActionByKey("<UpArrow>");
			return;
		default:
			std::cerr << [theEvent keyCode] << "\n";
	}
}

@end
