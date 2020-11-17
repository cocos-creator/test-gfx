//
//  ViewController.m
//  gfx-tests
//
//  Created by minggo on 2019/12/24.
//  Copyright © 2019 minggo. All rights reserved.
//

#import "ViewController.h"
#import "View.h"

@implementation ViewController
{
    View* _view;
}

- (void)viewDidLoad {

    [super viewDidLoad];

    NSRect rect = NSMakeRect(200, 200, 960, 640);
    _view = [[View alloc] initWithFrame:rect];
    self.view = _view;
}


- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];

    // Update the view, if already loaded.
}

- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size {
    
}

- (void)viewDidAppear
{
    // Make the view controller the window's first responder so that it can handle the Key events
    [_view.window makeFirstResponder:self];
}

- (BOOL)acceptsFirstResponder
{
    return YES;
}

@end
