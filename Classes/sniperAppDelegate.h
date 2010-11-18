//
//  sniperAppDelegate.h
//  sniper
//
//  Created by Mark Blackwell on 1/02/10.
//  Copyright __MyCompanyName__ 2010. All rights reserved.
//

@class SniperViewController;

@interface sniperAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    SniperViewController *mainViewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) SniperViewController *mainViewController;

@end

