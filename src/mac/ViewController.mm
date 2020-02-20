//
//  ViewController.m
//  gfx-tests
//
//  Created by minggo on 2019/12/24.
//  Copyright © 2019 minggo. All rights reserved.
//

#import "ViewController.h"
#include "tests/ClearScreenTest.h"
#include "tests/BasicTriangleTest.h"
#include "tests/BasicTextureTest.h"
#include "tests/StencilTest.h"
#include "tests/ParticleTest.h"
#include "tests/DepthTest.h"
#include "tests/TestBase.h"
#include "tests/BunnyTest.h"

#import <AppKit/NSTouch.h>
#import <AppKit/NSEvent.h>

using namespace cocos2d;

namespace
{
    int g_nextTextIndex = 0;
    using createFunc = TestBaseI * (*)(const WindowInfo& info);
    std::vector<createFunc> g_tests;
    TestBaseI* g_test    = nullptr;
    WindowInfo g_windowInfo;
}

@implementation ViewController
{
    MTKView *_view;
}

- (void)viewDidLoad {
    [super viewDidLoad];

    [super viewDidLoad];
    
    // Set the view to use the default device
    _view = (MTKView *)self.view;
    _view.depthStencilPixelFormat = MTLPixelFormatDepth24Unorm_Stencil8;
    _view.device = MTLCreateSystemDefaultDevice();
    
    NSAssert(_view.device, @"Metal is not supported on this device");
    
    //[_renderer mtkView:_view drawableSizeWillChange:_view.drawableSize];
    
    _view.delegate = self;
    
    g_windowInfo.windowHandle = (intptr_t)_view;
    
    CGSize size = _view.drawableSize;
    g_windowInfo.screen.x = 0;
    g_windowInfo.screen.y = 0;
    g_windowInfo.screen.width = size.width;
    g_windowInfo.screen.height = size.height;
    
    g_windowInfo.physicalHeight = g_windowInfo.screen.height;
    g_windowInfo.physicalWidth = g_windowInfo.screen.width;
    
    [self initTests];
}


- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];

    // Update the view, if already loaded.
}


- (void)drawInMTKView:(nonnull MTKView *)view {
    g_test->tick(1.0f / 60);
}

- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size {
    
}

- (void)viewDidAppear
{
    // Make the view controller the window's first responder so that it can handle the Key events
    [_view.window makeFirstResponder:self];
}

- (void)keyDown:(NSEvent *)event {
    g_nextTextIndex = (++g_nextTextIndex) % g_tests.size();
    CC_SAFE_DESTROY(g_test);
    g_test = g_tests[g_nextTextIndex](g_windowInfo);
}

- (BOOL)acceptsFirstResponder
{
    return YES;
}

- (void)initTests {
    static bool first = true;
    if (first)
    {
        g_tests = {
            ClearScreen::create,
            BasicTriangle::create,
            BasicTexture::create,
            StencilTest::create,
            ParticleTest::create,
            DepthTexture::create,
        };
        g_test = g_tests[g_nextTextIndex](g_windowInfo);
        if (g_test == nullptr)
            return;
        first = false;
    }
}

@end
