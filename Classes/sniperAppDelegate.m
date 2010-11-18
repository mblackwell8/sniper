//
//  sniperAppDelegate.m
//  sniper
//
//  Created by Mark Blackwell on 1/02/10.
//  Copyright __MyCompanyName__ 2010. All rights reserved.
//

#import "sniperAppDelegate.h"
#import "SniperViewController.h"

@implementation sniperAppDelegate


@synthesize window;
@synthesize mainViewController;


- (void)applicationDidFinishLaunching:(UIApplication *)application {
    
	SniperViewController *aController = [[SniperViewController alloc] initWithNibName:@"SniperViewController" bundle:nil];
	self.mainViewController = aController;
	[aController release];
	
    self.mainViewController.view.frame = [UIScreen mainScreen].applicationFrame;
	[window addSubview:[mainViewController view]];
    [window makeKeyAndVisible];
}


- (void)dealloc {
    [mainViewController release];
    [window release];
    [super dealloc];
}

@end
