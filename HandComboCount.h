//
//  HandComboCount.h
//  sniper
//
//  Created by Mark Blackwell on 13/05/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>


@interface HandComboCount : NSObject {
	
	UIButton *hand;
	NSString *handCode;
	UILabel *comboCountLabel;
	
	int rawCount;
	int userAdjustedCount;

}

@property (retain, nonatomic) UIButton *hand;
@property (retain, nonatomic) NSString *handCode;
@property (retain, nonatomic) UILabel *comboCountLabel;
@property (nonatomic) int rawCount;
@property (nonatomic) int userAdjustedCount;

- (id)init;

- (id)initWithHandCode:(NSString *)handCode comboCount:(UILabel *)lbl onButton:(UIButton *)hand;


@end
