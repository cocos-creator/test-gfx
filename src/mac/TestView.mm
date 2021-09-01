/****************************************************************************
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 http://www.cocos2d-x.org

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/
#import "TestView.h"
#import "tests/TestBase.h"

#if CC_PLATFORM == CC_PLATFORM_MAC_OSX
    #import <AppKit/AppKit.h>
#endif
#if CC_PLATFORM == CC_PLATFORM_MAC_IOS
    #import <UIKit/UIKit.h>
#endif
#import <AppKit/NSTouch.h>
#import <AppKit/NSEvent.h>
#import <QuartzCore/QuartzCore.h>
#import <AppKit/NSWindow.h>

namespace
{
    cc::WindowInfo g_windowInfo;
}

@implementation TestView {

}

- (CALayer *)makeBackingLayer
{
    return [CAMetalLayer layer];
}

- (instancetype)initWithFrame:(NSRect)frameRect {
    if (self = [super initWithFrame:frameRect]) {
        [self.window makeFirstResponder:self];

        int pixelRatio = 1;
#if CC_PLATFORM == CC_PLATFORM_MAC_OSX
        pixelRatio = [[NSScreen mainScreen] backingScaleFactor];
#else
        pixelRatio = [[UIScreen mainScreen] scale];
#endif //CC_PLATFORM == CC_PLATFORM_MAC_OSX
        CGSize size = CGSizeMake(frameRect.size.width * pixelRatio, frameRect.size.height * pixelRatio);

        // Create CAMetalLayer
        self.wantsLayer = YES;
        // Config metal layer
        CAMetalLayer *layer = (CAMetalLayer*)self.layer;
        layer.pixelFormat = MTLPixelFormatBGRA8Unorm;
        layer.device = self.device = MTLCreateSystemDefaultDevice();
        layer.drawableSize = size;
        self.depthStencilPixelFormat = MTLPixelFormatDepth24Unorm_Stencil8;
        self.mtlCommandQueue = [self.device newCommandQueue];

        g_windowInfo.windowHandle = self;
        g_windowInfo.screen.x = frameRect.origin.x;
        g_windowInfo.screen.y = frameRect.origin.y;
        g_windowInfo.screen.width = size.width;
        g_windowInfo.screen.height = size.height;
        g_windowInfo.pixelRatio = pixelRatio;

        // Start main loop
        self.timer = [NSTimer scheduledTimerWithTimeInterval:1.0f / 60
                                                 target:self
                                               selector:@selector(tick)
                                               userInfo:nil
                                                repeats:YES];

        cc::TestBaseI::setWindowInfo(g_windowInfo);
        
    }
    return self;
}

- (void)setFrameSize:(NSSize)newSize {
    CAMetalLayer *layer = (CAMetalLayer *)self.layer;

    CGSize nativeSize = [self convertSizeToBacking:newSize];
    [super setFrameSize:newSize];
    layer.drawableSize = nativeSize;
    [self viewDidChangeBackingProperties];

    // Add tracking area to receive mouse move events.
    NSRect          rect         = {0, 0, nativeSize.width, nativeSize.height};
    NSTrackingArea *trackingArea = [[NSTrackingArea alloc] initWithRect:rect
                                                                options:(NSTrackingMouseEnteredAndExited | NSTrackingMouseMoved | NSTrackingActiveInKeyWindow)
                                                                  owner:self
                                                               userInfo:nil];
    [self addTrackingArea:trackingArea];
    
    void* windowHandle = nullptr;
#if CC_USE_METAL
    windowHandle = self;
#else
    windowHandle = layer;
#endif
    cc::TestBaseI::resizeGlobal(windowHandle, nativeSize.width, nativeSize.height);
}

- (void)viewDidChangeBackingProperties {
    [super viewDidChangeBackingProperties];
    CAMetalLayer *layer = (CAMetalLayer *)self.layer;
    layer.contentsScale = self.window.backingScaleFactor;
}

- (void)keyUp:(NSEvent *)event {

}

- (void)tick {

}

- (void)mouseUp:(NSEvent *)event {

}

- (void)rightMouseUp:(NSEvent *)event {

}

- (BOOL)acceptsFirstResponder
{
    return YES;
}

- (void)viewDidAppear
{
    // Make the view controller the window's first responder so that it can handle the Key events
    [self.window makeFirstResponder:self];
}

- (void)onTerminate
{
}

@end
