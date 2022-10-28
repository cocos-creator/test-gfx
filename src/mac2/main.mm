#include <AppKit/NSApplication.h>
#include "GameApp.h"

@interface ApplicationMac : NSApplication
{
}
@end

@interface ApplicationDelegateMac : NSObject<NSApplicationDelegate>
{
}
@end

@implementation ApplicationMac

@end

@implementation ApplicationDelegateMac

@end


int main(int argc, char* argv[]) {

    @autoreleasepool {
        [ApplicationMac sharedApplication];
        [NSApp setDelegate: [[ApplicationDelegateMac alloc] init]];
        [NSApp finishLaunching];
    }

    cc::GameApp app;
    app.Run();

    return EXIT_SUCCESS;
}
