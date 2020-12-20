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
- (CALayer *)makeBackingLayer
{
    return [CAMetalLayer layer];
}
#else
+ (Class)layerClass
{
    return [CAEAGLLayer class];
}
#endif

- (id)initWithFrame:(CGRect)frame {
#ifdef USE_METAL
    if (self = [super initWithFrame:frame]) {
        // Create CAMetalLayer
        self.wantsLayer = YES;
        // Config metal layer
        CAMetalLayer *layer = (CAMetalLayer*)self.layer;
        layer.pixelFormat = MTLPixelFormatBGRA8Unorm;
        layer.device = self.device = MTLCreateSystemDefaultDevice();
        CGSize size = CGSizeMake(frameRect.size);
        layer.drawableSize = size;
        
        self.device = MTLCreateSystemDefaultDevice();
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
