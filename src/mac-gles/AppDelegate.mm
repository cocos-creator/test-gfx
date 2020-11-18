//
//  AppDelegate.m
//  gfx-tests
//
//  Created by minggo on 2019/12/24.
//  Copyright © 2019 minggo. All rights reserved.
//

#import "AppDelegate.h"
#include "tests/ClearScreenTest.h"
#include "tests/BasicTriangleTest.h"
#include "tests/BasicTextureTest.h"
#include "tests/StencilTest.h"
#include "tests/ParticleTest.h"
#include "tests/DepthTest.h"
#include "tests/TestBase.h"
#include "tests/BunnyTest.h"

using namespace cocos2d;

namespace
{
    int g_nextTextIndex = 0;
    using createFunc = TestBaseI * (*)(const WindowInfo& info);
    std::vector<createFunc> g_tests;
    TestBaseI* g_test    = nullptr;
    WindowInfo g_windowInfo;
}

@interface AppDelegate ()
{
    NSWindow* _window;
    NSView* _view;
    NSTimer* _timer;
}
@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    NSRect rect = NSMakeRect(200, 200, 960, 640);
    _window = [[NSWindow alloc] initWithContentRect:rect
                                          styleMask:NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskTitled | NSWindowStyleMaskClosable
                                           backing:NSBackingStoreBuffered
                                             defer:NO];
    if (!_window) {
        NSLog(@"Failed to allocated the window.");
        return;
    }
    
    _window.title = @"GFX test case";
    [_window makeFirstResponder:self];
    _view = [[NSView alloc] initWithFrame:rect];
    
    _window.contentView = _view;
    [_window makeKeyAndOrderFront:nil];
    
    g_windowInfo.windowHandle = (intptr_t)_view;
    
    g_windowInfo.screen.x = 0;
    g_windowInfo.screen.y = 0;
    g_windowInfo.screen.width = rect.size.width;
    g_windowInfo.screen.height = rect.size.height;
    
    g_windowInfo.physicalHeight = g_windowInfo.screen.height;
    g_windowInfo.physicalWidth = g_windowInfo.screen.width;
    
    [self initTests];
    
    _timer = [NSTimer scheduledTimerWithTimeInterval:(1.0 / 60)
                                              target:self
                                            selector:@selector(renderScene)
                                            userInfo:nil repeats:YES];
}


- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
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
            BunnyTest::create,
        };
        g_test = g_tests[g_nextTextIndex](g_windowInfo);
        if (g_test == nullptr)
            return;
        first = false;
    }
}

- (BOOL) renderScene {
    g_test->tick();
    return true;
}

- (void)keyDown:(NSEvent *)event {
    g_nextTextIndex = (++g_nextTextIndex) % g_tests.size();
    CC_SAFE_DESTROY(g_test);
    g_test = g_tests[g_nextTextIndex](g_windowInfo);
}

- (BOOL)acceptsFirstResponder {
    return YES;
}

@end
