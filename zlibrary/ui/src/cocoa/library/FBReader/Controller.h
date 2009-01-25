//
//  Controller.h
//  FBReader
//
//  Created by Alexey Vyskubov on 25.01.09.
//  Copyright 2009 Teleca. All rights reserved.
//
#import <Cocoa/Cocoa.h>


@interface Controller : NSObject {
	IBOutlet NSView *myView;
}

- (void)showDialogWithTitle:(NSString *)title
				   Message:(NSString *)message
					 Style:(NSAlertStyle)style;
- (int)showQuestionWithTitle:(NSString *)title
					 Message:(NSString *)message
					 Button0:(NSString *)button0
					 Button1:(NSString *)button1
					 Button2:(NSString *)button2;

+ (Controller *)ourController;

@end
