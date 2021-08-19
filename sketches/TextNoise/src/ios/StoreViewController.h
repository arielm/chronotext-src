//
//  StoreViewController.h
//  TextNoise
//
//  Created by Ariel Malka on 4/9/20.
//  Copyright © 2020 chronotext.com. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "SceneStore.h"

NS_ASSUME_NONNULL_BEGIN

@interface StoreViewController : UITableViewController

@property (nonatomic, strong) id<SceneStore> store;

@end

NS_ASSUME_NONNULL_END
