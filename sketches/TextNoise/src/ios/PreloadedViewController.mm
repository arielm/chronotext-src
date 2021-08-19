//
//  PreloadedViewController.mm
//  TextNoise
//
//  Created by Ariel Malka on 4/10/20.
//  Copyright © 2020 chronotext.com. All rights reserved.
//

#import "PreloadedViewController.h"
#import "Bridge.h"

@interface PreloadedViewController ()

@property (nonatomic, strong) NSArray<NSString*> *titles;
@property (nonatomic, strong) NSArray<NSString*> *paths;

@end

@implementation PreloadedViewController

- (void)viewDidLoad
{
    [super viewDidLoad];

    [self.tableView registerClass:[UITableViewCell class] forCellReuseIdentifier:@"IDENTIFIER"];
    
    self.navigationController.navigationBar.translucent = NO;
    self.navigationController.navigationBar.tintColor = UIColor.redColor;
    
    self.title = @"Preloaded";
    
    self.titles = @[
        @"il piccolo diavolo",
        @"down by law",
        @"police",
        @"lo range (use earphones)",
        @"hi range",
        @"full range",
        @"440hz monster",
        @"terminator",
        @"on the road",
        @"lollipop",
        @"roller coaster",
        @"corkscrew in French",
        @"left right",
        @"French song",
        @"banana"];
    
    self.paths = @[
        @"il_piccolo_diavolo",
        @"down_by_law",
        @"police",
        @"lo_range",
        @"hi_range",
        @"full_range",
        @"440hz_monster",
        @"terminator",
        @"on_the_road",
        @"lollipop",
        @"roller_coaster",
        @"corkscrew_in_french",
        @"left_right",
        @"french_song",
        @"banana"];
}

- (void)close
{
    [self dismissViewControllerAnimated:YES completion:^{
        [[Bridge sharedInstance] backToIdleState];
    }];
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
    NSString* path = [[NSBundle mainBundle] pathForResource:self.paths[indexPath.row] ofType:@"dat"];
    NSData *data = [NSData dataWithContentsOfFile:path];
    [[Bridge sharedInstance] setSceneWithData:data];
    
    [self close];
}

@end
