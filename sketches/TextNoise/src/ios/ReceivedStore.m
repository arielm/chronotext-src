//
//  ReceivedStore.m
//  TextNoise
//
//  Created by Ariel Malka on 4/10/20.
//  Copyright © 2020 chronotext.com. All rights reserved.
//

#import "ReceivedStore.h"

#import "FMDatabaseAdditions.h"

@interface ReceivedStore ()

@property (nonatomic, strong) FMDatabase *db;

@end

@implementation ReceivedStore

- (instancetype)initWithDB:(FMDatabase*)db
{
    if (self = [super init])
    {
        self.db = db;
        
        if (![db tableExists:@"received"])
        {
            [db executeUpdate:@"CREATE TABLE received (title TEXT, data BLOB)"];
        }
    }
    
    return self;
}

- (NSMutableArray*)rowIds
{
    NSMutableArray *array = [NSMutableArray array];
    FMResultSet *rs = [self.db executeQuery:@"SELECT rowId FROM received ORDER BY rowId DESC"];
    
    while ([rs next])
    {
        [array addObject:[NSNumber numberWithInt:[rs intForColumnIndex:0]]];
    }
    
    return array;
}

- (void)removeRowWithId:(NSNumber*)rowId
{
    [self.db executeUpdate:@"DELETE FROM received WHERE rowid=?", rowId];
}

- (NSString*)titleForRowWithId:(NSNumber*)rowId
{
    return [self.db stringForQuery:@"SELECT title FROM received WHERE rowid=?", rowId];
}

- (NSData*)dataForRowWithId:(NSNumber*)rowId
{
    return [self.db dataForQuery:@"SELECT data FROM received WHERE rowid=?", rowId];
}

- (NSNumber*)addSceneWithTitle:(NSString*)title data:(NSData*)data
{
    FMResultSet *rs = [self.db executeQuery:@"SELECT title, data FROM received"];
    
    while ([rs next])
    {
        if ([[rs stringForColumnIndex:0] isEqualToString:title] && [[rs dataForColumnIndex:1] isEqualToData:data])
        {
            return [NSNumber numberWithBool:NO];
        }
    }
    
    [self.db executeUpdate:@"INSERT INTO received (title, data) VALUES (?, ?)", title, data];
    return [NSNumber numberWithLongLong:[self.db lastInsertRowId]];
}

@end
