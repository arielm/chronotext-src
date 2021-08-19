//
//  InfoItem.h
//  TextNoise
//
//  Created by Ariel Malka on 4/14/20.
//  Copyright © 2020 chronotext.com. All rights reserved.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface InfoItem : NSObject

- (instancetype)initWithStackView:(UIStackView*)stackView;
- (void)setTitleText:(NSString*)text;
- (void)setParagraphText:(NSString*)text;

@end

NS_ASSUME_NONNULL_END
