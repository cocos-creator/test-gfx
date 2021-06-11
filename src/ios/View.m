//
//  View.m
//  gfx-tests
//
//  Created by minggo on 2019/12/4.
//
#import "View.h"
#import "AppDelegate.h"

@implementation View

#ifdef CC_USE_METAL
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
#ifdef CC_USE_METAL
    if (self = [super initWithFrame:frame]) {
        float scale = [[UIScreen mainScreen] scale];
        self.contentScaleFactor = scale;

        // Config metal layer
        CAMetalLayer *layer = (CAMetalLayer*)self.layer;
        layer.pixelFormat = MTLPixelFormatBGRA8Unorm;
        layer.device = self.device = MTLCreateSystemDefaultDevice();
        CGSize size = CGSizeMake(frame.size.width * scale, frame.size.height * scale);
        layer.drawableSize = size;

        self.device = MTLCreateSystemDefaultDevice();
    }
#else
    self = [super initWithFrame:frame];
#endif

    return self;
}

@end
