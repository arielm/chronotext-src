//
//  Bridge.h
//  TextNoise
//
//  Created by Ariel Malka on 4/6/20.
//  Copyright © 2020 chronotext.com. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "SavedStore.h"
#import "ReceivedStore.h"

NS_ASSUME_NONNULL_BEGIN

class Sketch;

typedef NS_ENUM(NSInteger, BridgeState)
{
    BridgeStateIdle,
    BridgeStateTextInput,
    BridgeStateImporting,
    BridgeStateExporting,
    BridgeStateInfo
};

@interface Bridge : NSObject

@property (nonatomic, strong) SavedStore *savedStore;
@property (nonatomic, strong) ReceivedStore *receivedStore;

+ (Bridge*)sharedInstance;

- (void)openURL:(NSURL*)url;
- (void)setSketch:(Sketch*)sketch;
- (void)showTextInput;
- (void)updateText:(NSString*)text;
- (void)showTextTooLongAlert;
- (void)showPathTooShortAlert;
- (void)uploadSceneWithTitle:(NSString*)title;
- (void)saveSceneWithTitle:(NSString*)title;
- (void)setSceneWithData:(NSData*)data;
- (void)showExportScreen;
- (void)showImportScreen;
- (void)showInfoScreen;
- (void)backToIdleState;

@end

NS_ASSUME_NONNULL_END
