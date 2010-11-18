//
//  SniperView.h
//  sniper
//
//  Created by Mark Blackwell on 16/05/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

//ref: http://www.dejal.com/developer/open/dsview

#import <UIKit/UIKit.h>

@class SniperView;

@protocol SniperViewDelegate <NSObject>
@optional

- (UIView *)view:(SniperView *)view hitTest:(CGPoint)point
       withEvent:(UIEvent *)event hitView:(UIView *)hitView;

@end

@interface SniperView : UIView {
    id <SniperViewDelegate> viewDelegate;
}

//"as conventional, the delegate is not retained:" from orig author

@property (nonatomic, assign) id <SniperViewDelegate> viewDelegate;

@end
