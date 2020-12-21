//
//  View.h
//  gfx-tests
//
//  Created by minggo on 2019/12/4.
//

#pragma once

#import <UIKit/UIView.h>
#ifdef USE_METAL
#import <MetalKit/MetalKit.h>
#endif

@interface View : UIView

@property(nonatomic, assign) NSTimer *timer;
@property(nonatomic, assign) id<MTLDevice> device;

@end

