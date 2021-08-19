//
//  StoreViewController.mm
//  TextNoise
//
//  Created by Ariel Malka on 4/9/20.
//  Copyright © 2020 chronotext.com. All rights reserved.
//

#import "StoreViewController.h"
#import "Bridge.h"

@interface StoreViewController ()

@property (nonatomic, strong) NSMutableArray<NSNumber*> *rowIds;
@property (nonatomic, strong) NSMutableArray<NSString*> *titles;

@end

@implementation StoreViewController

- (void)viewDidLoad
{
    [super viewDidLoad];

    [self.tableView registerClass:[UITableViewCell class] forCellReuseIdentifier:@"IDENTIFIER"];
    
    self.navigationController.navigationBar.translucent = NO;
    
    self.navigationController.navigationBar.tintColor = UIColor.redColor;
    self.navigationItem.rightBarButtonItem = self.editButtonItem;
    
    [self refreshItems];
}

- (void)close
{
    [self dismissViewControllerAnimated:YES completion:^{
        [[Bridge sharedInstance] backToIdleState];
    }];
}

- (void)refreshItems
{
    self.titles = [NSMutableArray array];
    self.rowIds = [self.store rowIds];

    for (NSNumber *rowId in self.rowIds)
    {
        [self.titles addObject:[self.store titleForRowWithId:rowId]];
    }
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return self.titles.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:@"IDENTIFIER" forIndexPath:indexPath];
    cell.textLabel.text = self.titles[indexPath.row];
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    NSNumber *rowId = self.rowIds[indexPath.row];
    [[Bridge sharedInstance] setSceneWithData:[self.store dataForRowWithId:rowId]];
    
    [self close];
}

- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath
{
    return YES;
}

- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath
{
    if (editingStyle == UITableViewCellEditingStyleDelete)
    {
        NSNumber *rowId = self.rowIds[indexPath.row];
        [self.store removeRowWithId:rowId];
        [self refreshItems];
        
        [tableView deleteRowsAtIndexPaths:@[indexPath] withRowAnimation:UITableViewRowAnimationFade];
    }
}

@end
