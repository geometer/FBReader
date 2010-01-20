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

#include <ZLApplication.h>

#import "CocoaView.h"

#include "ZLCocoaPaintContext.h"

@implementation CocoaView

- (void)drawRect:(CGRect)dirtyRect {
	shared_ptr<ZLView> view = ZLApplication::Instance().currentView();
	if (!view.isNull()) {
		ZLCocoaPaintContext &context = (ZLCocoaPaintContext&)view->context();
		CGSize size = [self bounds].size;
		context.begin(size.width, size.height);
		view->paint();
		context.end();
	}
}

@end
