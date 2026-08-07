// The test app uses the NDK NativeActivity glue, while the Cocos 4 engine
// sources are also compiled with its GameActivity input declarations.  The
// test application's input loop is implemented in main.cpp, so these local
// no-op compatibility functions keep the engine archive linkable without
// bringing the GameActivity glue (which conflicts with NativeActivity glue).

#include "game-activity/native_app_glue/android_native_app_glue.h"

extern "C" {

struct android_input_buffer* android_app_swap_input_buffers(struct android_app* /*android_app*/) {
    return nullptr;
}

void android_app_clear_motion_events(struct android_input_buffer* /*inputBuffer*/) {
}

void android_app_clear_key_events(struct android_input_buffer* /*inputBuffer*/) {
}

void android_app_set_motion_event_filter(struct android_app* /*app*/,
                                         android_motion_event_filter /*filter*/) {
}

} // extern "C"
