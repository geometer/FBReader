//
//  Utils.mm
//  FBReader
//
//  Created by Alexey Vyskubov on 25.01.09.
//
#include <string>
#import "Utils.h"


@implementation Utils

+ (NSString *)stringWithCxxString:(std::string const&)cxxString {
	return [[[NSString alloc] initWithBytes:cxxString.data()
									 length:cxxString.size()
								   encoding:NSUTF8StringEncoding] autorelease];
}

+ (NSString *)cocoaButtonName:(const ZLResourceKey&)key {
	if (key.Name.empty()) {
		return nil;
	}
	return [Utils stringWithCxxString:(ZLDialogManager::buttonName(key))];
}

@end