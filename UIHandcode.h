//
//  UIHandcode.h
//  sniper
//
//  Created by Mark Blackwell on 5/05/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//


#import "userinp.h"


@interface UIHandcode {
	
	UIButton *hand;
	UILabel *percentLabel;
	
}

@property (readonly, retain) UIButton *hand;
@property (readonly, retain) UILabel *percentLabel;

- (void)displayComboCounts;

- (void)removeComboCounts;


@end
