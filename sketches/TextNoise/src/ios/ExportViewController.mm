//
//  ExportViewController.mm
//  TextNoise
//
//  Created by Ariel Malka on 4/8/20.
//  Copyright © 2020 chronotext.com. All rights reserved.
//

#import "ExportViewController.h"
#import "Bridge.h"

#define MAX_TITLE_LENGTH 32

@interface ExportViewController ()
{
    NSTimeInterval elapsed;
}

@property (weak, nonatomic) IBOutlet UITextField *titleLabel;
@property (weak, nonatomic) IBOutlet UIButton *shareButton;
@property (weak, nonatomic) IBOutlet UIActivityIndicatorView *activityIndicator;

@property (strong, nonatomic) NSMutableSet<NSString*> *producedTitles;
@property (strong, nonatomic) NSString *objectId;

@end

@implementation ExportViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    self.producedTitles = [[NSMutableSet alloc] init];
    elapsed = [[NSDate date] timeIntervalSinceReferenceDate];
    
    self.navigationController.navigationBar.tintColor = UIColor.redColor;
    self.navigationItem.leftBarButtonItem = [[UIBarButtonItem alloc] initWithTitle:@"Close" style:UIBarButtonItemStylePlain target:self action:@selector(close)];
    
    self.title = @"Export";
}

- (void)close
{
    [self dismissViewControllerAnimated:YES completion:^{
        [[Bridge sharedInstance] backToIdleState];
    }];
}

- (IBAction)shareButtonPressed:(id)sender
{
    BOOL shouldSaveScene = !self.objectId || ![self.producedTitles containsObject:[self produceTitle]];
    if (shouldSaveScene)
    {
        [self.producedTitles addObject:[self produceTitle]];
        
        self.shareButton.hidden = YES;
        [self.activityIndicator startAnimating];
        
        [[Bridge sharedInstance] uploadSceneWithTitle:[self produceTitle]];
    }
    else
    {
        [self shareScene];
    }
}

- (IBAction)saveButtonPressed:(id)sender
{
    [[Bridge sharedInstance] saveSceneWithTitle:[self produceTitle]];
    [self close];
}

- (NSString*)produceTitle
{
    NSString *title = self.titleLabel.text;
    if (title && title.length > 0)
    {
        NSString *trimmed = [title stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]];
        if (trimmed.length > 0)
        {
            if (trimmed.length >= MAX_TITLE_LENGTH)
            {
                return [trimmed substringToIndex:MAX_TITLE_LENGTH];
            }
            
            return trimmed;
        }
    }

    return [NSString stringWithFormat:@"untitled %.0f", elapsed];
}

- (void)sceneSavedWithObjectId:(NSString*)objectId
{
    self.objectId = objectId;
    
    self.shareButton.hidden = NO;
    [self.activityIndicator stopAnimating];
    
    [self shareScene];
}

- (void)sceneFailedToSaveWithError:(NSError*)error
{
    self.shareButton.hidden = NO;
    [self.activityIndicator stopAnimating];
    
    UIAlertController *alertController = [UIAlertController alertControllerWithTitle:@"Scene can't be shared" message:error.localizedDescription preferredStyle:UIAlertControllerStyleAlert];
    UIAlertAction *actionOk = [UIAlertAction actionWithTitle:@"Ok" style:UIAlertActionStyleDefault handler:nil];
    [alertController addAction:actionOk];
    [self presentViewController:alertController animated:YES completion:nil];
}

- (void)shareScene
{
    NSURL *url = [[NSURL alloc] initWithString:[NSString stringWithFormat:@"textnoise://%@", self.objectId]];
    NSString *description = @"Open this link on iPhone with the TextNoise application installed. "; // XXX: Adding a space is necessary when user is copying (to clipboard)
    NSArray *activityItems = @[description, url];

    UIActivityViewController* activityViewController = [[UIActivityViewController alloc] initWithActivityItems:activityItems applicationActivities:nil];
    activityViewController.excludedActivityTypes = @[UIActivityTypePostToFacebook, UIActivityTypePostToTwitter];

    [self presentViewController:activityViewController animated:YES completion:nil];
}

@end
