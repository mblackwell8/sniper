//
//  FlipsideViewController.h
//  sniper
//
//  Created by Mark Blackwell on 1/02/10.
//  Copyright __MyCompanyName__ 2010. All rights reserved.
//

@protocol FlipsideViewControllerDelegate;


@interface FlipsideViewController : UIViewController <UIWebViewDelegate> {
	id <FlipsideViewControllerDelegate> delegate;
	
	IBOutlet UIWebView *contentWebView;
	NSString *resourceFileName;
}

@property (retain, nonatomic) NSString *resourceFileName;

@property (nonatomic, assign) id <FlipsideViewControllerDelegate> delegate;

- (IBAction)done;


@end


@protocol FlipsideViewControllerDelegate
- (void)flipsideViewControllerDidFinish:(FlipsideViewController *)controller;
@end

