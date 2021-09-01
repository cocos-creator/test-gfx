//
//  AppDelegate.m
//  gfx-tests
//
//  Created by minggo on 2019/12/24.
//  Copyright © 2019 minggo. All rights reserved.
//

#import "AppDelegate.h"
#import "ViewController.h"
#import "TestViewController.h"

#define MULTI_WINDOW 0

@interface AppDelegate ()

@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    //windowController
    
    NSStoryboard *storyBoard = [NSStoryboard storyboardWithName:@"Main" bundle:nil]; // get a reference to the storyboard
    
#if MULTI_WINDOW
    self.testController = [storyBoard instantiateControllerWithIdentifier:@"TestWindowController"]; // instantiate your window controller
    [self.testController showWindow:self]; // show the window
    [self.testController retainCount];
#endif
}


- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
//    ViewController *controller = (ViewController *)NSApplication.sharedApplication.mainWindow.windowController.contentViewController;
//    [controller onTerminate];
}

-(BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender{
    return YES;
}


@end
