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
@interface View : MTKView<MTKViewDelegate>
#else
@interface View : UIView
#endif

@end

