//
//  AppDelegate.m
//  ios-test
//
//  Created by minggo on 2019/12/5.
//  Copyright © 2019 minggo. All rights reserved.
//

#import "AppDelegate.h"
#import "ViewController.h"
#import "View.h"
#include "tests/ClearScreenTest.h"
#include "tests/BasicTriangleTest.h"
#include "tests/BasicTextureTest.h"
#include "tests/DepthTest.h"
#include "tests/StencilTest.h"
#include "tests/ParticleTest.h"

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

@end

@implementation AppDelegate

@synthesize window;

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    // Override point for customization after application launch.
    
    self.window = [[UIWindow alloc] initWithFrame: [[UIScreen mainScreen] bounds]];
    ViewController* viewController = [[ViewController alloc] init];
    
    UIView* view = [[View alloc] initWithFrame: [[UIScreen mainScreen] bounds]];
    viewController.view = view;
    
    [self initWindowInfo: view];
    [self initTests];
    [self run];
    
    [self.window setRootViewController:viewController];
    [self.window makeKeyAndVisible];
    
    return YES;
}

- (void)initWindowInfo:(UIView*)view {
    g_windowInfo.windowHandle = (intptr_t)(view);
    
    CGRect rect = [[UIScreen mainScreen] bounds];
    
    float scale = 1.0f;
    if ( [view respondsToSelector:@selector(setContentScaleFactor:)] )
    {
        scale = [[UIScreen mainScreen] scale];
        view.contentScaleFactor = scale;
    }
    
    g_windowInfo.screen.x = rect.origin.x * scale;
    g_windowInfo.screen.y = rect.origin.y * scale;
    g_windowInfo.screen.width = rect.size.width * scale;
    g_windowInfo.screen.height = rect.size.height * scale;
    
    g_windowInfo.physicalHeight = g_windowInfo.screen.height;
    g_windowInfo.physicalWidth = g_windowInfo.screen.width;
}

- (void)initTests {
    static bool first = true;
    if (first)
    {
        g_tests = {
            ClearScreen::create,
            BasicTriangle::create,
            BasicTexture::create,
            DepthTexture::create,
            StencilTest::create,
            ParticleTest::create,
        };
        g_test = g_tests[g_nextTextIndex](g_windowInfo);
        if (g_test == nullptr)
            return;
        first = false;
    }
}

- (void)run {
    CADisplayLink* displayLink = [NSClassFromString(@"CADisplayLink") displayLinkWithTarget:self selector:@selector(loop:)];
    displayLink.preferredFramesPerSecond = 60;
    [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
}

-(void)loop:(id)sender {
    g_test->tick(1.0f / 60);
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    g_nextTextIndex = (++g_nextTextIndex) % g_tests.size();
    CC_SAFE_DESTROY(g_test);
    g_test = g_tests[g_nextTextIndex](g_windowInfo);
}


@end
