//
//  Controller.mm
//  FBReader
//
//  Created by Alexey Vyskubov on 25.01.09.
//  Copyright 2009 Teleca. All rights reserved.
//
#import "Controller.h"

static Controller* myController;

@implementation Controller

- (void)showDialogWithTitle:(NSString *)title
				   Message:(NSString *)message
					 Style:(NSAlertStyle)style {
	
	NSAlert *alert = [[NSAlert alloc] init];
	
	[alert addButtonWithTitle:@"OK"];
	[alert setMessageText:title];
	[alert setInformativeText:message];
	[alert setAlertStyle:style];
	
	[alert runModal];
	[alert release];
}

- (int)showQuestionWithTitle:(NSString *)title
					 Message:(NSString *)message
					 Button0:(NSString *)button0
					 Button1:(NSString *)button1
					 Button2:(NSString *)button2 {
	
	NSAlert *alert = [[NSAlert alloc] init];
	
	if (button0) {
		[alert addButtonWithTitle:button0];
	} else {
		[alert addButtonWithTitle:@"OK"];
	}
	
	if (button1) {
		[alert addButtonWithTitle:button1];
	}
	
	if (button2) {
		[alert addButtonWithTitle:button2];
	}
	
	[alert setMessageText:title];
	[alert setInformativeText:message];
	[alert setAlertStyle:NSInformationalAlertStyle];
	
	int response = [alert runModal] - NSAlertFirstButtonReturn;
	
	[alert release];
	
	return response;
	
}

+ (Controller *)ourController {
	return myController;
}


@end