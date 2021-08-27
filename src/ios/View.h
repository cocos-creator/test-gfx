//
//  View.h
//  gfx-tests
//
//  Created by minggo on 2019/12/4.
//

#pragma once

#import <UIKit/UIView.h>
#import <MetalKit/MetalKit.h>

@interface View : UIView

@property(nonatomic, assign) NSTimer *timer;
@property(nonatomic, assign) id<MTLDevice> device;

@end
