//
//  HandComboCount.m
//  sniper
//
//  Created by Mark Blackwell on 13/05/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "HandComboCount.h"




@implementation HandComboCount

@synthesize hand;
@synthesize handCode;
@synthesize comboCountLabel;
@synthesize rawCount;
@synthesize userAdjustedCount;

- (id)init {
	if (self = [super init]) {
		rawCount = 0;
		userAdjustedCount = 0;
	}
	
    return self;

}


- (id)initWithHandCode:(NSString *)hc comboCount:(UILabel *)lbl onButton:(UIButton *)handBtn {
	if (self = [super init]) {
		self.handCode = hc;
		self.comboCountLabel = lbl;
		self.hand = handBtn;
		
		rawCount = 0;
		userAdjustedCount = 0;
	}
	
    return self;

}




@end
