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

#ifdef USE_METAL
@interface View : NSView
#else
@interface View : UIView
#endif

@property(nonatomic, assign) NSTimer *timer;
@property(nonatomic, assign) id<MTLDevice> device;

@end

