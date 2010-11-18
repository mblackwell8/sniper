//
//  SniperView.m
//  sniper
//
//  Created by Mark Blackwell on 16/05/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "SniperView.h"


@implementation SniperView


@synthesize viewDelegate;

/*
 hitTest:withEvent:
 
 Overrides this method to add support for the -view:hitTest:withEvent:hitView view delegate behavior.
 
 Written by DJS 2009-09.
 */

- (UIView *)hitTest:(CGPoint)point withEvent:(UIEvent *)event {
    UIView *hitView = [super hitTest:point withEvent:event];
	
    if ([self.viewDelegate respondsToSelector:
		 @selector(view:hitTest:withEvent:hitView:)])
        return [self.viewDelegate view:self hitTest:point
							 withEvent:event hitView:hitView];
    else
        return hitView;
}

- (void)dealloc {
    [super dealloc];
}


@end
