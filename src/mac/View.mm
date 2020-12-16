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
#import "View.h"
#include "tests/TestBase.h"

#if CC_PLATFORM == CC_PLATFORM_MAC_OSX
    #import <AppKit/AppKit.h>
#endif
#if CC_PLATFORM == CC_PLATFORM_MAC_IOS
    #import <UIKit/UIKit.h>
#endif
#import <AppKit/NSTouch.h>
#import <AppKit/NSEvent.h>

namespace
{
    cc::WindowInfo g_windowInfo;
}

@implementation View {

}

- (instancetype)initWithFrame:(NSRect)frameRect {
    if (self = [super initWithFrame:frameRect]) {
        [self.window makeFirstResponder:self];
        self.device = MTLCreateSystemDefaultDevice();
        self.depthStencilPixelFormat = MTLPixelFormatDepth24Unorm_Stencil8;
        self.mtlCommandQueue = [self.device newCommandQueue];
        self.delegate = self;
        int pixelRatio = 1;
#if CC_PLATFORM == CC_PLATFORM_MAC_OSX
        pixelRatio = [[NSScreen mainScreen] backingScaleFactor];
#else
        pixelRatio = [[UIScreen mainScreen] scale];
#endif //CC_PLATFORM == CC_PLATFORM_MAC_OSX

        g_windowInfo.windowHandle = (intptr_t)self;

        g_windowInfo.screen.x = frameRect.origin.x;
        g_windowInfo.screen.y = frameRect.origin.y;
        g_windowInfo.screen.width = frameRect.size.width * pixelRatio;
        g_windowInfo.screen.height = frameRect.size.height * pixelRatio;

        g_windowInfo.physicalHeight = g_windowInfo.screen.height;
        g_windowInfo.physicalWidth = g_windowInfo.screen.width;

        cc::TestBaseI::nextTest(g_windowInfo);
    }
    return self;
}

- (void)drawInMTKView:(MTKView *)view {
    cc::TestBaseI::onTick();
}

- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size {
}

- (void)keyUp:(NSEvent *)event {
    if (event.keyCode == 49) { // space
        cc::TestBaseI::toggleMultithread();
    }
}

- (void)mouseUp:(NSEvent *)event {
    cc::TestBaseI::onTouchEnd(g_windowInfo);
}

- (void)rightMouseUp:(NSEvent *)event {
    cc::TestBaseI::onTouchEnd(g_windowInfo);
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
    cc::TestBaseI::destroyGlobal();
}

@end
