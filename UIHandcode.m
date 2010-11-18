//
//  UIHandcode.m
//  sniper
//
//  Created by Mark Blackwell on 5/05/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "UIHandcode.h"


@implementation UIHandcode

- (id)initWithFrame:(CGRect)frame {
    if (self = [super initWithFrame:frame]) {
        // Initialization code
		
		//make the hand button same size as view
		hand = [UIButton buttonWithType:UIButtonTypeCustom];
		hand.frame = frame;
		[hand setTitle:@"" forState:UIControlStateNormal];
		hand.titleLabel.adjustsFontSizeToFitWidth = YES; 		
		[hand.titleLabel setFont:[UIFont boldSystemFontOfSize:24.0]];
		
		[self addSubview:hand];
		
		//label is at (3,50) and is 88x21
		CGRect f = CGRectMake(3.0, 50.0, 88.0, 21.0);
		percentLabel = [[UILabel alloc] initWithFrame:f];
		percentLabel.text = @"";
		percentLabel.font = [UIFont systemFontOfSize:12.0];
		
		
		[self addSubview:percentLabel];
    }
    return self;
}


- (void)drawRect:(CGRect)rect {
    // Drawing code
	[hand drawRect:rect];
	[percentLabel drawRect:rect];
}


- (void)dealloc {
	[hand release];
	[percentLabel release];
	
    [super dealloc];
}


@end
