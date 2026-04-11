#import <UIKit/UIKit.h>

extern "C" int chaos_mobile_ios_run_host(void);

@interface AppDelegate : UIResponder <UIApplicationDelegate>
@end

@implementation AppDelegate

- (BOOL)application:(UIApplication*)application didFinishLaunchingWithOptions:(NSDictionary*)launchOptions
{
    (void)application;
    (void)launchOptions;
    int exitCode = chaos_mobile_ios_run_host();
    NSLog(@"ChaosMobileHost runHost exitCode=%d", exitCode);
    return YES;
}

@end
