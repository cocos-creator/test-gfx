#import <FileUtils.h>
#include "GameApp.h"

@implementation TestView
@synthesize metalLayer = _metalLayer;

- (id)initWithFrame: (CGRect)frame
              scale: (CGFloat)scale
             device: (id<MTLDevice>)device
{
    if ((self = [super initWithFrame: frame]))
    {
        self.wantsLayer = YES;
        self.layer = self.metalLayer = [CAMetalLayer layer];
        self.autoresizingMask = (NSViewWidthSizable | NSViewHeightSizable);

        self.metalLayer.device = device;
        self.metalLayer.framebufferOnly = TRUE;
        self.metalLayer.opaque = YES;
        self.metalLayer.drawsAsynchronously = TRUE;
        self.metalLayer.presentsWithTransaction = FALSE;
        self.metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
        CGFloat components[] = { 0.0, 0.0, 0.0, 1 };
        self.metalLayer.backgroundColor = CGColorCreate(CGColorSpaceCreateDeviceRGB(), components);
    }

    return self;
}

- (void)setFrameSize: (CGSize) size
{
    [super setFrameSize: size];
    [self.metalLayer setDrawableSize: size];
}
@end

@implementation TestViewController

- (void)windowWillClose:(NSNotification *)notification {
    cc::GameApp::getInstance()->setExit(true);
}

- (void)windowDidResize:(NSNotification *)notification {
    TestView* metalView = reinterpret_cast<TestView*>([notification.object contentView]);
    CGSize drawableSize = metalView.metalLayer.drawableSize;
    cc::TestBaseI::resizeGlobal(metalView.metalLayer, drawableSize.width, drawableSize.height, cc::gfx::SurfaceTransform::IDENTITY);
}
@end

namespace cc {
GameApp *GameApp::_instance = nullptr;

GameApp::GameApp() {
    _instance = this;
    std::vector<std::string> path = {"Resources"};
    createFileUtils();
    FileUtils::getInstance()->setSearchPaths(path);
}

void GameApp::setExit(bool en) {
    _running = !en;
}

void GameApp::setPause(bool en) {
    _paused = en;
}

void GameApp::Run() {
    initialize();

    while (_running) {
        bool eventProcessed = false;
        do {
            eventProcessed = processNextEvent();
        } while (eventProcessed);

        if (!_paused) TestBaseI::update();
    }
    
    cc::TestBaseI::destroyGlobal();
    destroy();

    [_controller release];
}

void GameApp::initialize() {
    // Give the application a name.
    createWindow("Cocos GFX Test", _screenWidth, _screenHeight, 0.1, 0.1);

    WindowInfo info;
    info.windowHandle  = _metalView.metalLayer;
    info.screen.width  = _screenWidth;
    info.screen.height = _screenHeight;
    info.pixelRatio = _pixelRatio;

    TestBaseI::setWindowInfo(info);
    TestBaseI::nextTest();
}

bool GameApp::destroy() {
    return true;
}

void GameApp::createWindow(const std::string &name, int width, int height, float x, float y) {
    _windowTitle = [NSString stringWithCString:name.c_str() encoding:NSUTF8StringEncoding];
    CGRect screenBounds = CGRectMake(x, y, width, height);

    const NSWindowStyleMask defaultMask = NSWindowStyleMaskResizable | NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable;
    _nativeWindow = [[NSWindow alloc] initWithContentRect: screenBounds styleMask: defaultMask backing: NSBackingStoreBuffered defer:false];
    _nativeWindow.title = _windowTitle;
    _nativeWindow.tabbingMode = NSWindowTabbingModeDisallowed;

    [_nativeWindow setCollectionBehavior:NSWindowCollectionBehaviorFullScreenPrimary];
    [_nativeWindow makeKeyAndOrderFront:nil];

    id<MTLDevice> device = MTLCreateSystemDefaultDevice();

    NSScreen *nativeScreen = [NSScreen mainScreen];
    _pixelRatio = [nativeScreen backingScaleFactor];
    _metalView = [[TestView alloc] initWithFrame: screenBounds
                                           scale: _pixelRatio
                                          device: device];

    _controller = [[TestViewController alloc] init];
    [_controller setView: _metalView];
    [_controller retain];

    _nativeWindow.contentViewController = _controller;
    [_nativeWindow makeFirstResponder: _metalView];

    [[NSNotificationCenter defaultCenter] addObserver: _controller
                                             selector: @selector(windowWillClose:)
                                                 name: NSWindowWillCloseNotification
                                               object: _nativeWindow];

    //Used for window resizing notification
    [[NSNotificationCenter defaultCenter] addObserver: _controller
                                             selector: @selector(windowDidResize:)
                                                 name: NSWindowDidResizeNotification
                                               object: _nativeWindow];
}

bool GameApp::processNextEvent() {
    @autoreleasepool {
        NSEvent* event = [NSApp nextEventMatchingMask: NSEventMaskAny
                                            untilDate: [NSDate distantPast]
                                               inMode: NSDefaultRunLoopMode
                                              dequeue: YES];
        if (event != nil) {
            switch (event.type) {
                case NSEventTypeLeftMouseDown:
                case NSEventTypeRightMouseDown:
                    break;
                case NSEventTypeKeyUp:
                    if (event.keyCode == NSEventTypeKeyUp) {
                        cc::TestBaseI::spacePressed();
                    }
                    break;
                case NSEventTypeLeftMouseUp:
                case NSEventTypeRightMouseUp:
                    cc::TestBaseI::onTouchEnd();
                    break;
            }

            [NSApp sendEvent: event];
            return true;
        } else {
            return false;
        }
    }
}

}
