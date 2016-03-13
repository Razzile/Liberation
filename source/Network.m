#import "Network.h"
@implementation Network
-(id)init{
	self=[super init];
	self->bundleID=[NSBundle mainBundle].bundleIdentifier;
	self->Version=[[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleShortVersionString"];
	return self;
}
-(void)fetchWithURL:(NSString*)URLString{
NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:URLString]];
request.HTTPMethod = @"POST";
[request setValue:@"application/json" forHTTPHeaderField:@"Content-Type"];
NSMutableDictionary *INFO = [NSMutableDictionary dictionaryWithObjectsAndKeys:self->bundleID,@"bundleID",self->Version,@"Version",nil];

//Shall We Check ARMV7/V7S/64 ?
#ifdef TARGET_CPU_ARM 
	[INFO setObject:@"ARM" forKey:@"Arch"];
#else
 	[INFO setObject:@"ARM64" forKey:@"Arch"];
#endif

NSData* jsonData = [NSJSONSerialization dataWithJSONObject:INFO options:0 error:nil];

request.HTTPBody = jsonData;
NSData* result=[NSURLConnection sendSynchronousRequest:request returningResponse:nil error:nil];
[request release];
[INFO release];
[jsonData release];

NSDictionary* JSON= [NSJSONSerialization JSONObjectWithData:result options:kNilOptions error:nil];
self->Address=[JSON objectForKey:@"Address"];
self->dataToWrite=[JSON objectForKey:@"Data"];
}
@end