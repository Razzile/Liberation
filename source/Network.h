#import <Foundation/Foundation.h>
#import <TargetConditionals.h>
#import <mach-o/dyld.h>
@interface Network<UIWebViewDelegate>:NSObject{
	NSString* bundleID;
	NSString* Version;	
	NSNumber* Address;
	NSData* dataToWrite;
}

-(id)init;
-(void)fetchWithURL:(NSString*)URLString;
@end