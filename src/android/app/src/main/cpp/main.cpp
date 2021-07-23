#include <android_native_app_glue.h>
#include <jni.h>
#include "platform/android/FileUtils-android.h"
#include "tests/TestBase.h"

#include <android/log.h>
#include <cocos/bindings/event/CustomEventTypes.h>
#include <cocos/bindings/event/EventDispatcher.h>
#include <time.h>

#define LOG_TAG   "main"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
using namespace cc;

namespace {
static double now() {
    struct timespec res;
    clock_gettime(CLOCK_MONOTONIC, &res);
    return res.tv_sec + (double)res.tv_nsec / 1e9;
}

/**
     * Shared state for our app.
     */
struct SavedState {
    struct android_app* app;

    int     animating;
    int32_t width;
    int32_t height;
};

WindowInfo g_windowInfo;

void engineHandleCmd(struct android_app* app, int32_t cmd) {
    struct SavedState* state = (struct SavedState*)app->userData;
    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
            if (state->app->window && !g_windowInfo.windowHandle) {
                g_windowInfo.windowHandle  = state->app->window;
                g_windowInfo.screen.width = ANativeWindow_getWidth(app->window);
                g_windowInfo.screen.height = ANativeWindow_getHeight(app->window);
                g_windowInfo.screen.x = g_windowInfo.screen.y = 0;

                TestBaseI::setWindowInfo(g_windowInfo);
                TestBaseI::nextTest();
            } else {
                CustomEvent event;
                event.name         = EVENT_RECREATE_WINDOW;
                event.args->ptrVal = state->app->window;
                cc::EventDispatcher::dispatchCustomEvent(event);
            }
            break;
        case APP_CMD_TERM_WINDOW: {
            CustomEvent event;
            event.name = EVENT_DESTROY_WINDOW;
            EventDispatcher::dispatchCustomEvent(event);
        }
            state->animating = 0;
            break;
        case APP_CMD_GAINED_FOCUS:
            state->animating = 1;
            break;
        case APP_CMD_LOST_FOCUS:
            state->animating = 0;
        default:
            break;
    }
}

/**
     * Process the next input event.
     */
int32_t engineHandleInput(struct android_app* app, AInputEvent* event) {
    int type = AInputEvent_getType(event);
    if (type == AINPUT_EVENT_TYPE_KEY) {
        return 1;
    }
    if (type == AINPUT_EVENT_TYPE_MOTION) {
        int action = AMotionEvent_getAction(event);
        switch (action) {
            case AMOTION_EVENT_ACTION_UP:
            case AMOTION_EVENT_ACTION_POINTER_UP: {

                TestBaseI::nextTest(AMotionEvent_getX(event, 0) < TestBaseI::swapchains[0]->getWidth() / 2.F);
                break;
            }
        }
        return 1;
    }
    return 0;
}
} // namespace

void android_main(struct android_app* state) {
    struct SavedState savedState;
    memset(&savedState, 0, sizeof(savedState));
    state->userData     = &savedState;
    state->onAppCmd     = engineHandleCmd;
    state->onInputEvent = engineHandleInput;
    savedState.app      = state;
    static_cast<FileUtilsAndroid*>(FileUtils::getInstance())->setassetmanager(state->activity->assetManager);
    double lastTime = now();

    while (1) {
        // Read all pending events.
        int                         ident;
        int                         events;
        struct android_poll_source* source;

        // If not animating, we will block forever waiting for events.
        // If animating, we loop until all events are read, then continue
        // to draw the next frame of animation.
        while ((ident = ALooper_pollAll(savedState.animating ? 0 : -1, NULL, &events,
                                        (void**)&source)) >= 0) {
            // Process this event.
            if (source != nullptr)
                source->process(state, source);
        }

        if (state->destroyRequested != 0) {
            TestBaseI::destroyGlobal();
            return;
        }

        double time = now();

        if (savedState.animating) {
            TestBaseI::update();
            lastTime = time;
        }
    }
}
