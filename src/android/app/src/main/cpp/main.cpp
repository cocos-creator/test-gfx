#include <jni.h>
#include <android_native_app_glue.h>
#include "platform/android/FileUtils-android.h"
#include "tests/TestBase.h"

#include <android/log.h>
#include <time.h>

#define LOG_TAG "main"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
using namespace cc;

namespace
{
    static double now() {
        struct timespec res;
        clock_gettime(CLOCK_MONOTONIC, &res);
        return res.tv_sec + (double) res.tv_nsec / 1e9;
    }

    /**
     * Shared state for our app.
     */
    struct SavedState {
        struct android_app* app;

        int animating;
        int32_t width;
        int32_t height;
    };

    WindowInfo g_windowInfo;

    void destroyTests()
    {
        TestBaseI::destroyGlobal();
    }

    void engineHandleCmd(struct android_app* app, int32_t cmd)
    {
        struct SavedState* state = (struct SavedState*)app->userData;
        switch (cmd)
        {
            case APP_CMD_INIT_WINDOW:
                if (state->app->window)
                {
                    g_windowInfo.windowHandle = (intptr_t)state->app->window;
                    g_windowInfo.physicalWidth = g_windowInfo.screen.width = ANativeWindow_getWidth(app->window);
                    g_windowInfo.physicalHeight = g_windowInfo.screen.height = ANativeWindow_getHeight(app->window);
                    g_windowInfo.screen.x = g_windowInfo.screen.y = 0;

                    TestBaseI::setWindowInfo(g_windowInfo);
                    TestBaseI::nextTest();
                }

                break;
            case APP_CMD_TERM_WINDOW:
                destroyTests();
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
        if(type == AINPUT_EVENT_TYPE_KEY){
            return 1;
        } else if(type == AINPUT_EVENT_TYPE_MOTION){
            int action = AMotionEvent_getAction(event);
            switch (action)
            {
                case AMOTION_EVENT_ACTION_DOWN:
                case AMOTION_EVENT_ACTION_POINTER_DOWN:
                    return 1;
                    break;
                case AMOTION_EVENT_ACTION_UP:
                case AMOTION_EVENT_ACTION_POINTER_UP:
                    TestBaseI::onTouchEnd();
                    return 1;
                    break;
            }
            return 1;
        }
        return 0;
    }
}

void android_main(struct android_app* state) {
    struct SavedState savedState;
    memset(&savedState, 0, sizeof(savedState));
    state->userData = &savedState;
    state->onAppCmd = engineHandleCmd;
    state->onInputEvent = engineHandleInput;
    savedState.app = state;
    static_cast<FileUtilsAndroid*>(FileUtils::getInstance())->setassetmanager(state->activity->assetManager);
    double lastTime = now();

    while (1)
    {
        // Read all pending events.
        int ident;
        int events;
        struct android_poll_source* source;

        // If not animating, we will block forever waiting for events.
        // If animating, we loop until all events are read, then continue
        // to draw the next frame of animation.
        while ((ident = ALooper_pollAll(savedState.animating ? 0 : -1, NULL, &events,
                                      (void**)&source)) >= 0)
        {

            // Process this event.
            if (source != nullptr)
                source->process(state, source);
        }

        if (state->destroyRequested != 0)
        {
            return;
        }

        double time = now();

        if (savedState.animating)
        {
            TestBaseI::onTick();
            lastTime = time;
        }
    }
}
