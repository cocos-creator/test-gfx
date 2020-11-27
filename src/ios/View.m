//
//  View.m
//  gfx-tests
//
//  Created by minggo on 2019/12/4.
//
#import "View.h"
#import "AppDelegate.h"

@implementation View

#ifdef USE_METAL
+ (Class)layerClass
{
    return [CAMetalLayer class];
}
#else
+ (Class)layerClass
{
    return [CAEAGLLayer class];
}
#endif

- (id)initWithFrame:(CGRect)frame {
#ifdef USE_METAL
    if (self = [super initWithFrame:frame device:MTLCreateSystemDefaultDevice()]) {
        self.framebufferOnly = YES;
        self.delegate = self;
    }
#else
    self = [super initWithFrame:frame];
#endif

    return self;
}

#ifdef USE_METAL
- (void)drawInMTKView:(nonnull MTKView *)view {
    AppDelegate *delegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
    [delegate loop:view];
}

- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size {
    //TODO
    AppDelegate *delegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
    [delegate initWindowInfo:view size:size];
}
#endif

@end
