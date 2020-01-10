#include <jni.h>
#include <android_native_app_glue.h>
#include "platform/android/CCFileUtils-android.h"
#include "tests/ClearScreenTest.h"
#include "tests/BasicTriangleTest.h"
#include "tests/BasicTextureTest.h"
#include "tests/DepthTest.h"
#include "tests/StencilTest.h"

#include <android/log.h>
//#include "AppDelegate.h"

#define LOG_TAG "main"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
using namespace cocos2d;

namespace
{
    /**
     * Shared state for our app.
     */
    struct SavedState {
        struct android_app* app;

        int animating;
        int32_t width;
        int32_t height;
    };

    using createFunc = TestBaseI * (*)(const WindowInfo& info);
    std::vector<createFunc> g_tests;
    TestBaseI* g_test    = nullptr;
    WindowInfo g_windowInfo;
    int g_nextTestIndex = 0;
    void initTests()
    {
        static bool first = true;
        if (first)
        {
            g_tests = {
                    ClearScreen::create,
                    BasicTriangle::create,
                    BasicTexture::create,
                    DepthTexture::create,
                    StencilTest::create,
            };
            g_test = g_tests[g_nextTestIndex](g_windowInfo);
            if (g_test == nullptr)
                return;

            first = false;
        }
    }

    void destroyTests()
    {
        g_test = nullptr;
        //TODO: delete created test case.
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

                    initTests();
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
        struct engine* engine = (struct engine*)app->userData;
        int type = AInputEvent_getType(event);
        int src = AInputEvent_getSource(event);
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
                    g_nextTestIndex = (++g_nextTestIndex) % g_tests.size();
                    CC_SAFE_DESTROY(g_test);
                    g_test = g_tests[g_nextTestIndex](g_windowInfo);
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

        if (savedState.animating)
        {
            //TODO: calculate dt
            g_test->tick(1.0f / 60);
        }
    }
}