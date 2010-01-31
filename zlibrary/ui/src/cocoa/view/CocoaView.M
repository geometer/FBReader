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
#include "../util/ZLCocoaKeyUtil.h"

@implementation CocoaView

- (id)init {
	self = [super init];
	if (self) {
		[self registerForDraggedTypes:[NSArray arrayWithObjects:NSFilenamesPboardType, nil]];
	}
	return self;
}

- (NSDragOperation)draggingEntered:(id <NSDraggingInfo>)sender {
	NSPasteboard *pboard = [sender draggingPasteboard];
	NSDragOperation sourceDragMask = [sender draggingSourceOperationMask];

	if ([[pboard types] containsObject:NSFilenamesPboardType] &&
			(sourceDragMask & NSDragOperationGeneric)) {
		std::vector<std::string> filePaths;
		for (NSString *path in [pboard propertyListForType:NSFilenamesPboardType]) {
			filePaths.push_back([path UTF8String]);
		}
		if (ZLApplication::Instance().canDragFiles(filePaths)) {
			return NSDragOperationGeneric;
		}
	}
	return NSDragOperationNone;
}

- (BOOL)performDragOperation:(id <NSDraggingInfo>)sender {
	NSDragOperation sourceDragMask = [sender draggingSourceOperationMask];
	NSPasteboard *pboard = [sender draggingPasteboard];

	if ([[pboard types] containsObject:NSFilenamesPboardType] &&
			(sourceDragMask & NSDragOperationGeneric)) {
		std::vector<std::string> filePaths;
		for (NSString *path in [pboard propertyListForType:NSFilenamesPboardType]) {
			filePaths.push_back([path UTF8String]);
		}
		ZLApplication::Instance().dragFiles(filePaths);
		[self display];
	}
	return YES;
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
	ZLApplication::Instance().doActionByKey(ZLCocoaKeyUtil::keyName(theEvent));
}

@end
