//
//  main.m
//  gfx-tests
//
//  Created by minggo on 2019/12/24.
//  Copyright © 2019 minggo. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "AppDelegate.h"

int main(int argc, const char * argv[]) {
    id delegate = [AppDelegate alloc];
    NSApplication.sharedApplication.delegate = delegate;
    return NSApplicationMain(argc, argv);
}
