//
//  ImportViewController.mm
//  TextNoise
//
//  Created by Ariel Malka on 4/10/20.
//  Copyright © 2020 chronotext.com. All rights reserved.
//

#import "ImportViewController.h"
#import "Bridge.h"
#import "StoreViewController.h"
#import "PreloadedViewController.h"

@interface ImportViewController ()

@end

@implementation ImportViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    [self.tableView registerClass:[UITableViewCell class] forCellReuseIdentifier:@"IDENTIFIER"];
    
    self.navigationController.navigationBar.translucent = NO;
    
    self.navigationController.navigationBar.tintColor = UIColor.redColor;
    self.navigationItem.leftBarButtonItem = [[UIBarButtonItem alloc] initWithTitle:@"Close" style:UIBarButtonItemStylePlain target:self action:@selector(close)];
    
    self.title = @"Import";
}

- (void)close
{
    [self dismissViewControllerAnimated:YES completion:^{
        [[Bridge sharedInstance] backToIdleState];
    }];
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return 3;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:@"IDENTIFIER" forIndexPath:indexPath];
    cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
    
    switch (indexPath.row)
    {
        case 0:
            cell.textLabel.text = @"Preloaded";
            break;
            
        case 1:
            cell.textLabel.text = @"Received";
            break;

        case 2:
            cell.textLabel.text = @"Saved";
            break;
    }
    
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    switch (indexPath.row)
    {
        case 0:
        {
            PreloadedViewController *viewController = [[PreloadedViewController alloc] init];
            [self.navigationController pushViewController:viewController animated:YES];
            break;
        }
            
        case 1:
        {
            StoreViewController *viewController = [[StoreViewController alloc] init];
            viewController.title = @"Received";
            viewController.store = [Bridge sharedInstance].receivedStore;
            [self.navigationController pushViewController:viewController animated:YES];
            break;
        }
            
        case 2:
        {
            StoreViewController *viewController = [[StoreViewController alloc] init];
            viewController.title = @"Saved";
            viewController.store = [Bridge sharedInstance].savedStore;
            [self.navigationController pushViewController:viewController animated:YES];
            break;
        }
    }
}

@end
