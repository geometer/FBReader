//
//  Utils.h
//  FBReader
//
//  Created by Alexey Vyskubov on 25.01.09.
//

#include <string>
#include <ZLDialogManager.h>
#import <Cocoa/Cocoa.h>


@interface Utils : NSObject {

}

+ (NSString *)stringWithCxxString:(std::string const&)cxxString;
+ (NSString *)cocoaButtonName:(const ZLResourceKey&)key;

@end
