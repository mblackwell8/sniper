//
//  FlipsideViewController.m
//  sniper
//
//  Created by Mark Blackwell on 1/02/10.
//  Copyright __MyCompanyName__ 2010. All rights reserved.
//

#import "FlipsideViewController.h"


@implementation FlipsideViewController

@synthesize delegate;
@synthesize resourceFileName;


- (void)viewDidLoad {
    [super viewDidLoad];
    self.view.backgroundColor = [UIColor viewFlipsideBackgroundColor];   
	
	NSString *urlAddress = [[NSBundle mainBundle] pathForResource:resourceFileName ofType:@"html"];
	
	//NSString *appPath = [[NSBundle mainBundle] resourcePath];
	//appPath = [NSString stringWithFormat:@"%@/%@", appPath, resourceFileName];
	
	NSURL *url = [NSURL fileURLWithPath:urlAddress];
	NSURLRequest *request = [NSURLRequest requestWithURL:url];
	//[[contentWebView mainFrame] loadRequest:request];
	[contentWebView loadRequest:request];
	
	contentWebView.delegate = self;
	
}


- (IBAction)done {
	[self.delegate flipsideViewControllerDidFinish:self];	
}



- (BOOL)webView:(UIWebView *)webView shouldStartLoadWithRequest:(NSURLRequest *)request navigationType:(UIWebViewNavigationType)navigationType {
	
	//allow internal links (and any links to a file) to pass through unaltered
	if (navigationType == UIWebViewNavigationTypeLinkClicked && !request.URL.isFileURL) {
		[[UIApplication sharedApplication] openURL:request.URL];
		return NO;
	}
	
	return YES;
}



/*
 // Override to allow orientations other than the default portrait orientation.
 - (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
 // Return YES for supported orientations
 return (interfaceOrientation == UIInterfaceOrientationPortrait);
 }
 */

- (void)didReceiveMemoryWarning {
	// Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
	
	// Release any cached data, images, etc that aren't in use.
}

- (void)viewDidUnload {
	// Release any retained subviews of the main view.
	// e.g. self.myOutlet = nil;
}


- (void)dealloc {
    [super dealloc];
}


@end
