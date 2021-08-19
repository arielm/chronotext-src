//
//  ExportViewController.h
//  TextNoise
//
//  Created by Ariel Malka on 4/8/20.
//  Copyright © 2020 chronotext.com. All rights reserved.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface ExportViewController : UIViewController

- (void)sceneSavedWithObjectId:(NSString*)objectId;
- (void)sceneFailedToSaveWithError:(NSError*)error;

@end

NS_ASSUME_NONNULL_END
