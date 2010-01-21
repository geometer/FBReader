//
//  CocoaWindow.M
//  cocoa
//
//  Created by Nikolay Pultsin on 22.01.10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "CocoaWindow.h"


@implementation CocoaWindow

- (id)initWithContentRect:(NSRect)contentRect styleMask:(NSUInteger)aStyle backing:(NSBackingStoreType)bufferingType defer:(BOOL)flag {
	self = [super initWithContentRect:contentRect styleMask:aStyle backing:bufferingType defer:flag];
	if (self) {
		NSString *windowFilename = @"MainWindow";
		[self setFrameUsingName:windowFilename];
		[self setFrameAutosaveName:windowFilename];
	}
	return self;
}

@end
