//
//  Bridge.mm
//  TextNoise
//
//  Created by Ariel Malka on 4/6/20.
//  Copyright © 2020 chronotext.com. All rights reserved.
//

#import "Bridge.h"
#import "TextInputViewController.h"
#import "ExportViewController.h"
#import "ImportViewController.h"
#import "InfoViewController.h"
#import "SavedStore.h"

#import <AVFoundation/AVFoundation.h>

#import <Parse/Parse.h>

#import "chr/ios/CrossBridge.h"

#include "Sketch.h"

@interface Bridge ()
{
    Sketch *sketch;
    BridgeState state;
}

@property (nonatomic, strong) FMDatabase *db;

@property (nonatomic, strong) UIActivityIndicatorView *indicatorView;
@property (nonatomic, weak) ExportViewController *exportViewController;

@end

@implementation Bridge

+ (Bridge*)sharedInstance {
    static Bridge *instance = nil;
    static dispatch_once_t onceToken;
    
    dispatch_once(&onceToken, ^{
        instance = [self new];
    });
    
    return instance;
}

- (instancetype)init
{
    if (self = [super init])
    {
        [self initDB];
        
        state = BridgeStateIdle;
        [UIApplication sharedApplication].idleTimerDisabled = YES;
        
        NSNotificationCenter *center = [NSNotificationCenter defaultCenter];
        [center addObserver:self selector:@selector(handleAudioInterruption:) name:AVAudioSessionInterruptionNotification object:[AVAudioSession sharedInstance]];
    }
    
    return self;
}

- (void)handleAudioInterruption:(NSNotification*)notification
{
    NSDictionary *info = notification.userInfo;
    NSNumber *value = info[AVAudioSessionInterruptionTypeKey];
    
    if ([value unsignedIntValue] == AVAudioSessionInterruptionTypeBegan)
    {
        sketch->stopSound();
    }
    else
    {
        sketch->startSound();
    }
}

/*
 * When application is started via an URL, this occurs after Sketch::setup() but before Sketch::resize()
 */
- (void)openURL:(NSURL*)url
{
    if (state == BridgeStateIdle)
    {
        if (url.host && url.host.length == 10)
        {
            if (sketch)
            {
                [self downloadSceneWithObjectId:url.host];
                [self startIndicatorView];
                sketch->cancelScene();
            }
        }
    }
}

- (void)startIndicatorView
{
    UIViewController *controller = chr::system::bridge.viewController;
    
    self.indicatorView = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleGray];
    self.indicatorView.translatesAutoresizingMaskIntoConstraints = NO;
    
    [controller.view addSubview:self.indicatorView];
    [self.indicatorView.centerXAnchor constraintEqualToAnchor:controller.view.centerXAnchor].active = YES;
    [self.indicatorView.centerYAnchor constraintEqualToAnchor:controller.view.centerYAnchor].active = YES;
    
    [self.indicatorView startAnimating];
}

- (void)stopIndicatorView
{
    [self.indicatorView stopAnimating];
    [self.indicatorView removeFromSuperview];
}

- (void)setSketch:(Sketch*)sketch
{
    self->sketch = sketch;
}

- (void)showTextInput
{
    UIViewController *controller = chr::system::bridge.viewController;
    
    TextInputViewController *viewController = [[TextInputViewController alloc] init];
    
    UINavigationController *navigationController = [[UINavigationController alloc] initWithRootViewController:viewController];
    navigationController.modalTransitionStyle = UIModalTransitionStyleCoverVertical;
    navigationController.modalPresentationStyle = UIModalPresentationFullScreen;
    [controller presentViewController:navigationController animated:YES completion:nil];
    
    state = BridgeStateTextInput;
    [UIApplication sharedApplication].idleTimerDisabled = NO;
}

- (void)updateText:(NSString*)text
{
    sketch->updateText([text UTF8String]);
}

- (void)showTextTooLongAlert
{
    UIViewController *controller = chr::system::bridge.viewController;
    
    UIAlertController *alertController = [UIAlertController alertControllerWithTitle:nil message:@"Text is too long for path!" preferredStyle:UIAlertControllerStyleAlert];
    UIAlertAction *actionOk = [UIAlertAction actionWithTitle:@"Ok" style:UIAlertActionStyleDefault handler:nil];
    [alertController addAction:actionOk];
    [controller presentViewController:alertController animated:YES completion:nil];
}

- (void)showPathTooShortAlert
{
    UIViewController *controller = chr::system::bridge.viewController;
    
    UIAlertController *alertController = [UIAlertController alertControllerWithTitle:nil message:@"Path is too short for text!" preferredStyle:UIAlertControllerStyleAlert];
    UIAlertAction *actionOk = [UIAlertAction actionWithTitle:@"Ok" style:UIAlertActionStyleDefault handler:nil];
    [alertController addAction:actionOk];
    [controller presentViewController:alertController animated:YES completion:nil];
}

- (void)downloadSceneWithObjectId:(NSString*)objectId
{
    PFQuery *query = [PFQuery queryWithClassName:@"Scene"];
    [query getObjectInBackgroundWithId:objectId block:^(PFObject *parseObject, NSError *error) {
        if (error) {
            [self stopIndicatorView];
            self->sketch->restoreScene();
        } else {
            PFFileObject *file = parseObject[@"data"];
            [file getDataInBackgroundWithBlock:^(NSData *data, NSError *error) {
                if (error) {
                    [self stopIndicatorView];
                    self->sketch->restoreScene();
                } else {
                    [self stopIndicatorView];
                    [self.receivedStore addSceneWithTitle:parseObject[@"title"] data:data];
                    self->sketch->setSceneData(data);
                }
            }];
        }
    }];
}

- (void)uploadSceneWithTitle:(NSString*)title
{
    [self uploadSceneWithTitle:title data:sketch->getSceneData()];
}

- (void)uploadSceneWithTitle:(NSString*)title data:(NSData*)data
{
    PFFileObject *file = [PFFileObject fileObjectWithName:@"file.dat" data:data];
    
    [file saveInBackgroundWithBlock:^(BOOL succeeded, NSError *error) {
      if (succeeded) {
          PFObject *parseObject = [PFObject objectWithClassName:@"Scene"];
          parseObject[@"title"] = title;
          parseObject[@"data"] = file;
          
          [parseObject saveInBackgroundWithBlock:^(BOOL succeeded, NSError *error) {
            if (succeeded) {
                [self.exportViewController sceneSavedWithObjectId:parseObject.objectId];
            } else {
                [self.exportViewController sceneFailedToSaveWithError:error];
            }
          }];
      } else {
          [self.exportViewController sceneFailedToSaveWithError:error];
      }
    }];
}

- (void)saveSceneWithTitle:(NSString*)title
{
    [self.savedStore addSceneWithTitle:title data:sketch->getSceneData()];
}

- (void)setSceneWithData:(NSData*)data
{
    self->sketch->setSceneData(data);
}

- (void)showExportScreen
{
    UIViewController *controller = chr::system::bridge.viewController;
    
    UIStoryboard *storyBoard = [UIStoryboard storyboardWithName:@"Main" bundle:nil];
    self.exportViewController = (ExportViewController*)[storyBoard instantiateInitialViewController];

    UINavigationController *navigationController = [[UINavigationController alloc] initWithRootViewController:self.exportViewController];
    navigationController.modalTransitionStyle = UIModalTransitionStyleCoverVertical;
    navigationController.modalPresentationStyle = UIModalPresentationFullScreen;
    [controller presentViewController:navigationController animated:YES completion:nil];
    
    state = BridgeStateExporting;
    [UIApplication sharedApplication].idleTimerDisabled = NO;
}

- (void)showImportScreen
{
    UIViewController *controller = chr::system::bridge.viewController;
    
    ImportViewController *viewController = [[ImportViewController alloc] init];
    
    UINavigationController *navigationController = [[UINavigationController alloc] initWithRootViewController:viewController];
    navigationController.modalTransitionStyle = UIModalTransitionStyleCoverVertical;
    navigationController.modalPresentationStyle = UIModalPresentationFullScreen;
    [controller presentViewController:navigationController animated:YES completion:nil];
    
    state = BridgeStateImporting;
    [UIApplication sharedApplication].idleTimerDisabled = NO;
}

- (void)showInfoScreen
{
    UIViewController *controller = chr::system::bridge.viewController;
    
    InfoViewController *viewController = [[InfoViewController alloc] init];
    
    UINavigationController *navigationController = [[UINavigationController alloc] initWithRootViewController:viewController];
    navigationController.modalTransitionStyle = UIModalTransitionStyleCoverVertical;
    navigationController.modalPresentationStyle = UIModalPresentationFullScreen;
    [controller presentViewController:navigationController animated:YES completion:nil];
    
    state = BridgeStateInfo;
    [UIApplication sharedApplication].idleTimerDisabled = NO;
}

- (void)backToIdleState
{
    state = BridgeStateIdle;
    [UIApplication sharedApplication].idleTimerDisabled = YES;
}

- (NSString*)pathForFileInDocuments:(NSString*)name
{
    NSString *documentsPath = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
    return [documentsPath stringByAppendingPathComponent:name];
}

- (void)initDB
{
    self.db = [FMDatabase databaseWithPath:[self pathForFileInDocuments:@"store.sqlite"]];
    [self.db open];
    [self.db setShouldCacheStatements:YES];
    
    self.savedStore = [[SavedStore alloc] initWithDB:self.db];
    self.receivedStore = [[ReceivedStore alloc] initWithDB:self.db];
}

@end
