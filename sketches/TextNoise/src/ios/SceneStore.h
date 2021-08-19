//
//  SceneStore.h
//  TextNoise
//
//  Created by Ariel Malka on 4/10/20.
//  Copyright © 2020 chronotext.com. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "FMDatabase.h"

NS_ASSUME_NONNULL_BEGIN

@protocol SceneStore <NSObject>

- (instancetype)initWithDB:(FMDatabase*)db;

- (NSMutableArray*)rowIds;
- (void)removeRowWithId:(NSNumber*)rowId;
- (NSString*)titleForRowWithId:(NSNumber*)rowId;
- (NSData*)dataForRowWithId:(NSNumber*)rowId;
- (NSNumber*)addSceneWithTitle:(NSString*)title data:(NSData*)data;

@end

NS_ASSUME_NONNULL_END
