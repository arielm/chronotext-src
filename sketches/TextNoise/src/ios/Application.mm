
#import "Application.h"
#import "Bridge.h"

#import <Parse/Parse.h>

@implementation Application

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(nullable NSDictionary<UIApplicationLaunchOptionsKey, id> *)launchOptions
{
    /*
     * https://stackoverflow.com/a/37506632/50335
     */
    [[NSRunLoop currentRunLoop] runUntilDate:[NSDate dateWithTimeIntervalSinceNow:1.0]];
    
    // ---
    
    [Parse initializeWithConfiguration:[ParseClientConfiguration configurationWithBlock:^(id<ParseMutableClientConfiguration> configuration) {
        configuration.applicationId = @"";
        configuration.clientKey = @"";
        configuration.server = @"https://parseapi.back4app.com/";
    }]];
    
    PFInstallation *currentInstallation = [PFInstallation currentInstallation];
    [currentInstallation saveInBackgroundWithBlock:^(BOOL succeeded, NSError *error) {
        if (error) {
            NSLog(@"%@", error.debugDescription);
        }
    }];
    
    // ---
    
    GLViewController *viewController = [[GLViewController alloc] init];
    
    viewController.properties =
    @{
        GLViewControllerRenderingAPIKey: @(kEAGLRenderingAPIOpenGLES2),
        GLViewControllerInterfaceOrientationMaskKey: @(UIInterfaceOrientationMaskPortrait),
        GLViewControllerDepthFormatKey: @(GLKViewDrawableDepthFormatNone),
    };

    bridge = [[CrossBridge alloc] init];
    [bridge bind:viewController];

    window = [[UIWindow alloc] initWithFrame:UIScreen.mainScreen.bounds];
    window.backgroundColor = [UIColor blackColor];

    [window setRootViewController:viewController];
    [window makeKeyAndVisible];

    return YES;
}

- (BOOL)application:(UIApplication *)app openURL:(NSURL *)url options:(NSDictionary<UIApplicationOpenURLOptionsKey, id> *)options
{
    [[Bridge sharedInstance] openURL:url];
    return YES;
}

@end
