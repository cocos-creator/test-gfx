//
//  View.m
//  gfx-tests
//
//  Created by minggo on 2019/12/4.
//
#import "View.h"
#import "AppDelegate.h"

@implementation View

+ (Class)layerClass
{
    return [CAMetalLayer class];
}

- (id)initWithFrame:(CGRect)frame {
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

    return self;
}

@end
