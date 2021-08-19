//
//  InfoViewController.mm
//  TextNoise
//
//  Created by Ariel Malka on 4/13/20.
//  Copyright © 2020 chronotext.com. All rights reserved.
//

#import "InfoViewController.h"
#import "Bridge.h"
#import "InfoItem.h"

@interface InfoViewController ()

@property (nonatomic, strong) UIScrollView *scrollView;

@end

@implementation InfoViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    self.navigationController.navigationBar.translucent = NO;
    
    self.navigationController.navigationBar.tintColor = UIColor.redColor;
    self.navigationItem.leftBarButtonItem = [[UIBarButtonItem alloc] initWithTitle:@"Close" style:UIBarButtonItemStylePlain target:self action:@selector(close)];
    
    self.title = @"About";
    
    //
    
    self.scrollView = [[UIScrollView alloc] init];
    self.scrollView.translatesAutoresizingMaskIntoConstraints = NO;
    self.scrollView.backgroundColor = UIColor.whiteColor;
    [self.view addSubview:self.scrollView];
    
    UILayoutGuide *frameLayoutGuide = self.scrollView.frameLayoutGuide;
    UILayoutGuide *contentLayoutGuide = self.scrollView.contentLayoutGuide;
    
    [frameLayoutGuide.leadingAnchor constraintEqualToAnchor:self.view.leadingAnchor].active = YES;
    [frameLayoutGuide.trailingAnchor constraintEqualToAnchor:self.view.trailingAnchor].active = YES;
    [frameLayoutGuide.topAnchor constraintEqualToAnchor:self.view.topAnchor].active = YES;
    [frameLayoutGuide.bottomAnchor constraintEqualToAnchor:self.view.bottomAnchor].active = YES;

    [contentLayoutGuide.widthAnchor constraintEqualToAnchor:frameLayoutGuide.widthAnchor].active = YES;
    
    UIStackView *contentView = [[UIStackView alloc] init];
    contentView.translatesAutoresizingMaskIntoConstraints = NO;
    contentView.axis = UILayoutConstraintAxisVertical;
    [self.scrollView addSubview:contentView];
    
    [contentLayoutGuide.leadingAnchor constraintEqualToAnchor:contentView.leadingAnchor constant:-20].active = YES;
    [contentLayoutGuide.topAnchor constraintEqualToAnchor:contentView.topAnchor constant:-20].active = YES;
    [contentLayoutGuide.trailingAnchor constraintEqualToAnchor:contentView.trailingAnchor constant:+20].active = YES;
    [contentLayoutGuide.bottomAnchor constraintEqualToAnchor:contentView.bottomAnchor constant:+20].active = YES;
    
    //
    
    InfoItem *item1 = [[InfoItem alloc] initWithStackView:contentView];
    item1.titleText = @"How it works";
    item1.paragraphText = @"Each letter of the Latin alphabet, as well as each of the 10 ten digits, can produce a sound at a particular frequency. On a straight part of the curve, the letter “A” would produce a barely audible 55hz sound. Depending upon the curvature, the frequency could reach up to 55hz + 3,520hz. Similarly, the letter “B” would produce sound between 110hz and 110hz + 3,520hz depending upon the curvature. The same progression applies to the rest of the letters. Then come the digits, the highest frequency of which is produced by the digit 9: between 1,980hz and 1,980hz + 3,520hz.\nOnly the first 36 characters of the entered text will produce sound. In addition, the velocity of text affects volume; and finally, pressing on a word will raise the volume as well.";

    InfoItem *item2 = [[InfoItem alloc] initWithStackView:contentView];
    item2.titleText = @"Chronotext";
    item2.paragraphText = @"Chronotext is a growing collection of software experiments exploring the relation between text, space and time. TextNoise is the latest chronotext experiment. The collection began in 2003 and features many experiments such as Babel Tower (reading text on a rotating cylindrical helix), the Text Time Curvature (recording an act of writing and replaying it later on) or He liked thick word soup (reading Ulysses with your fingers...)\nThe man behind chronotext is Ariel Malka, a Tel Aviv-based designer and programmer.";
}

- (void)close
{
    [self dismissViewControllerAnimated:YES completion:^{
        [[Bridge sharedInstance] backToIdleState];
    }];
}

@end
