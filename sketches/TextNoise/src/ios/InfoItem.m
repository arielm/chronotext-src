//
//  InfoItem.m
//  TextNoise
//
//  Created by Ariel Malka on 4/14/20.
//  Copyright © 2020 chronotext.com. All rights reserved.
//

#import "InfoItem.h"

@interface InfoItem ()

@property (nonatomic, strong) UILabel *title;
@property (nonatomic, strong) UILabel *paragraph;

@end

@implementation InfoItem

- (instancetype)initWithStackView:(UIStackView*)stackView
{
    if (self = [super init]) {
        self.title = [[UILabel alloc] init];
        self.title.translatesAutoresizingMaskIntoConstraints = NO;
        self.title.font = [UIFont boldSystemFontOfSize:14];
        self.title.textColor = UIColor.blackColor;
        [stackView addArrangedSubview:self.title];
        
        self.paragraph = [[UILabel alloc] init];
        self.paragraph.translatesAutoresizingMaskIntoConstraints = NO;
        self.paragraph.lineBreakMode = NSLineBreakByWordWrapping;
        self.paragraph.numberOfLines = 0;
        self.paragraph.font = [UIFont systemFontOfSize:14];
        self.paragraph.textColor = UIColor.blackColor;
        [stackView addArrangedSubview:self.paragraph];
        
        [stackView setCustomSpacing:20 afterView:self.paragraph];
    }
    
    return self;
}

- (void)setTitleText:(NSString*)text
{
    self.title.text = text;
}

- (void)setParagraphText:(NSString*)text
{
    NSMutableParagraphStyle *style = [[NSMutableParagraphStyle alloc] init];
    style.minimumLineHeight = 20;
    NSDictionary *attributtes = @{NSParagraphStyleAttributeName : style};
    self.paragraph.attributedText = [[NSAttributedString alloc] initWithString:text attributes:attributtes];
}

@end
