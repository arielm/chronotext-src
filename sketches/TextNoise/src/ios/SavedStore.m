//
//  SavedStore.m
//  TextNoise
//
//  Created by Ariel Malka on 4/9/20.
//  Copyright © 2020 chronotext.com. All rights reserved.
//

#import "SavedStore.h"

#import "FMDatabaseAdditions.h"

@interface SavedStore ()

@property (nonatomic, strong) FMDatabase *db;

@end

@implementation SavedStore

- (instancetype)initWithDB:(FMDatabase*)db
{
    if (self = [super init])
    {
        self.db = db;
        
        if (![db tableExists:@"saved"])
        {
            [db executeUpdate:@"CREATE TABLE saved (title TEXT, data BLOB)"];
        }
    }
    
    return self;
}

- (NSMutableArray*)rowIds
{
    NSMutableArray *array = [NSMutableArray array];
    FMResultSet *rs = [self.db executeQuery:@"SELECT rowId FROM saved ORDER BY rowId DESC"];
    
    while ([rs next])
    {
        [array addObject:[NSNumber numberWithInt:[rs intForColumnIndex:0]]];
    }
    
    return array;
}

- (void)removeRowWithId:(NSNumber*)rowId
{
    [self.db executeUpdate:@"DELETE FROM saved WHERE rowid=?", rowId];
}

- (NSString*)titleForRowWithId:(NSNumber*)rowId
{
    return [self.db stringForQuery:@"SELECT title FROM saved WHERE rowid=?", rowId];
}

- (NSData*)dataForRowWithId:(NSNumber*)rowId
{
    return [self.db dataForQuery:@"SELECT data FROM saved WHERE rowid=?", rowId];
}

- (NSNumber*)addSceneWithTitle:(NSString*)title data:(NSData*)data
{
    [self.db executeUpdate:@"INSERT INTO saved (title, data) VALUES (?, ?)", title, data];
    return [NSNumber numberWithLongLong:[self.db lastInsertRowId]];
}

@end
