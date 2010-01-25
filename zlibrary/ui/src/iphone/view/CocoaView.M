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

- (void)drawRect:(CGRect)dirtyRect {
	shared_ptr<ZLView> view = ZLApplication::Instance().currentView();
	if (view.isNull()) {
		return;
	}

	ZLCocoaPaintContext &context = (ZLCocoaPaintContext&)view->context();
	CGSize size = [self bounds].size;
	context.begin(size.width, size.height);
	view->paint();
	context.end();
}

- (void)touchesBegan:(NSSet*)touches withEvent:(UIEvent*)theEvent {
	NSSet *allTouches = [theEvent allTouches];
	if ([allTouches count] != 1) {
		return;
	}
	shared_ptr<ZLView> view = ZLApplication::Instance().currentView();
	if (view.isNull()) {
		return;
	}
	UITouch *touch = [[allTouches allObjects] objectAtIndex:0];
	CGPoint location = [touch locationInView:self];
	CGSize size = [self bounds].size;
	view->onStylusPress(location.x, size.height - location.y);
}

- (void)touchesCancelled:(NSSet*)touches withEvent:(UIEvent*)theEvent {
	NSSet *allTouches = [theEvent allTouches];
	if ([allTouches count] != 1) {
		return;
	}
	shared_ptr<ZLView> view = ZLApplication::Instance().currentView();
	if (view.isNull()) {
		return;
	}
	UITouch *touch = [[allTouches allObjects] objectAtIndex:0];
	CGPoint location = [touch locationInView:self];
	CGSize size = [self bounds].size;
	view->onStylusRelease(location.x, size.height - location.y);
}

- (void)touchesEnded:(NSSet*)touches withEvent:(UIEvent*)theEvent {
	NSSet *allTouches = [theEvent allTouches];
	if ([allTouches count] != 1) {
		return;
	}
	shared_ptr<ZLView> view = ZLApplication::Instance().currentView();
	if (view.isNull()) {
		return;
	}
	UITouch *touch = [[allTouches allObjects] objectAtIndex:0];
	CGPoint location = [touch locationInView:self];
	CGSize size = [self bounds].size;
	view->onStylusRelease(location.x, size.height - location.y);
}

- (void)touchesMoved:(NSSet*)touches withEvent:(UIEvent*)theEvent {
	NSSet *allTouches = [theEvent allTouches];
	if ([allTouches count] != 1) {
		return;
	}
	shared_ptr<ZLView> view = ZLApplication::Instance().currentView();
	if (view.isNull()) {
		return;
	}
	UITouch *touch = [[allTouches allObjects] objectAtIndex:0];
	CGPoint location = [touch locationInView:self];
	CGSize size = [self bounds].size;
	view->onStylusMovePressed(location.x, size.height - location.y);
}

- (BOOL)canBecomeFirstResponder {
	return YES;
}

@end
