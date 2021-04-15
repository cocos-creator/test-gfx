//
//  AppDelegate.m
//  gfx-tests
//
//  Created by minggo on 2019/12/24.
//  Copyright © 2019 minggo. All rights reserved.
//

#import "AppDelegate.h"
#include "tests/TestBase.h"

using namespace cc;

namespace
{
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

    int pixelRatio = 1;
#if CC_PLATFORM == CC_PLATFORM_MAC_OSX
        pixelRatio = [[NSScreen mainScreen] backingScaleFactor];
#else
        pixelRatio = [[UIScreen mainScreen] scale];
#endif //CC_PLATFORM == CC_PLATFORM_MAC_OSX

    g_windowInfo.screen.x = 0;
    g_windowInfo.screen.y = 0;
    g_windowInfo.screen.width = rect.size.width * pixelRatio;
    g_windowInfo.screen.height = rect.size.height * pixelRatio;

    g_windowInfo.physicalHeight = g_windowInfo.screen.height;
    g_windowInfo.physicalWidth = g_windowInfo.screen.width;

    cc::TestBaseI::setWindowInfo(g_windowInfo);
    cc::TestBaseI::nextTest();

    _timer = [NSTimer scheduledTimerWithTimeInterval:(1.0 / 60)
                      target:self
                      selector:@selector(renderScene)
                      userInfo:nil repeats:YES];
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
    cc:TestBaseI::destroyGlobal();
}

-(BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender{
    return YES;
}

- (BOOL) renderScene {
    cc::TestBaseI::update();
    return true;
}

- (void)keyUp:(NSEvent *)event {
    if (event.keyCode == 49) { // space
        cc::TestBaseI::spacePressed();
    }
}

- (void)mouseUp:(NSEvent *)event {
    cc::TestBaseI::onTouchEnd();
}

- (void)rightMouseUp:(NSEvent *)event {
    cc::TestBaseI::onTouchEnd();
}

- (BOOL)acceptsFirstResponder {
    return YES;
}

@end
