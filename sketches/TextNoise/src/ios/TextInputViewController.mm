//
//  TextInputViewController.mm
//  TextNoise
//
//  Created by Ariel Malka on 4/22/20.
//  Copyright © 2020 chronotext.com. All rights reserved.
//

#import "TextInputViewController.h"
#import "Bridge.h"

@interface TextInputViewController () <UITextViewDelegate>

@property (nonatomic, strong) UITextView *textView;

@end

@implementation TextInputViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    self.navigationController.navigationBar.tintColor = UIColor.redColor;
    self.navigationItem.leftBarButtonItem = [[UIBarButtonItem alloc] initWithTitle:@"Cancel" style:UIBarButtonItemStylePlain target:self action:@selector(cancel)];
    
    self.view.backgroundColor = UIColor.whiteColor;
    
    NSNotificationCenter *center = [NSNotificationCenter defaultCenter];
    [center addObserver:self selector:@selector(keyboardDidShow:) name:UIKeyboardDidShowNotification object:nil];
    
    self.textView = [[UITextView alloc] initWithFrame:self.view.frame];
    self.textView.alpha = 0; // XXX
    self.textView.delegate = self;
    self.textView.textColor = UIColor.blackColor;
    self.textView.backgroundColor = UIColor.clearColor;
    self.textView.font = [UIFont fontWithName:@"Georgia" size:24];
    self.textView.autocapitalizationType = UITextAutocapitalizationTypeNone;
    self.textView.returnKeyType = UIReturnKeyDone;
    self.textView.textAlignment = NSTextAlignmentCenter;
    
    [self.view addSubview:self.textView];
}

- (void)cancel
{
    [self dismissViewControllerAnimated:NO completion:^{
        [[Bridge sharedInstance] backToIdleState];
        [[Bridge sharedInstance] updateText:@""];
    }];
}

- (void)viewWillAppear:(BOOL)animated
{
    [self.textView becomeFirstResponder];
}

- (void)keyboardDidShow:(NSNotification*)notification
{
    NSDictionary *info = notification.userInfo;
    NSValue *value = info[UIKeyboardFrameEndUserInfoKey];
    CGRect keyboardFrame = [value CGRectValue];

    CGFloat textViewHeight = (self.view.frame.size.height - keyboardFrame.size.height) * 0.5f;
    CGFloat textViewTop = textViewHeight;
    
    self.textView.frame = CGRectMake(0, textViewTop, self.view.frame.size.width, textViewHeight);
    self.textView.alpha = 1; // XXX
}

- (BOOL)textView:(UITextView*)textView shouldChangeTextInRange:(NSRange)range replacementText:(NSString*)text
{
    if ([text isEqualToString:@"\n"])
    {
        [self dismissViewControllerAnimated:NO completion:^{
            [[Bridge sharedInstance] backToIdleState];
            [[Bridge sharedInstance] updateText:self.textView.text];
        }];

        return NO;
    }

    return YES;
}

- (void)dealloc
{
    NSNotificationCenter *center = [NSNotificationCenter defaultCenter];
    [center removeObserver:self name:UIKeyboardDidShowNotification object:nil];
}

@end
