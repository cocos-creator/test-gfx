#pragma once

#include "tests/TestBase.h"

#import <AppKit/AppKit.h>
#import <AppKit/NSWindow.h>
#import <QuartzCore/CAMetalLayer.h>
#import <QuartzCore/QuartzCore.h>
#import <MetalKit/MetalKit.h>

@interface TestView : NSView {}
@property  (nonatomic, assign )CAMetalLayer *metalLayer;
@end

@interface TestViewController : NSViewController {}
- (void)windowWillClose:(NSNotification *)notification;
- (void)windowDidResize:(NSNotification *)notification;
@end

namespace cc {

class GameApp : public cc::CCObject {
public:
    static GameApp *getInstance() { return _instance; }
    GameApp();

    void Run();

    void setExit(bool en);
    void setPause(bool en);

private:
    void initialize();
    bool destroy() override;

    void createWindow(const std::string &name, int width, int height, float x, float y);
    bool processNextEvent();

    static GameApp *_instance;

    int _screenWidth  = 1024;
    int _screenHeight = 768;
    float _pixelRatio = 1.F;

    NSWindow* _nativeWindow = nullptr;
    NSString* _windowTitle = nullptr;
    TestView* _metalView = nullptr;
    TestViewController *_controller = nullptr;

    bool _running = true;
    bool _paused = false;
};

} // namespace cc
