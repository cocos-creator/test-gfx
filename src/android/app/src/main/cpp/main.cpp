#include <android/log.h>
#include <android_native_app_glue.h>
#include <jni.h>
#include <ctime>

#include "bindings/event/CustomEventTypes.h"
#include "bindings/event/EventDispatcher.h"
#include "platform/android/FileUtils-android.h"
#include "platform/java/jni/JniHelper.h"

#include "gfx-base/GFXDef-common.h"
#include "tests/TestBase.h"

#define LOG_TAG   "main"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

namespace {
/**
 * Shared state for our app.
 */
struct SavedState {
    struct android_app* app;

    int     animating;
    int32_t width;
    int32_t height;
};

cc::WindowInfo gWindowInfo;
JNIEnv*        gEnv{nullptr};

int getOrientation(android_app* app) {
    jobject   inst              = app->activity->clazz;
    jclass    cClazz            = gEnv->GetObjectClass(inst);
    jclass    cWindowManager    = gEnv->FindClass("android/view/WindowManager");
    jclass    cDisplay          = gEnv->FindClass("android/view/Display");
    jmethodID getWindowManager  = gEnv->GetMethodID(cClazz, "getWindowManager", "()Landroid/view/WindowManager;");
    jmethodID getDefaultDisplay = gEnv->GetMethodID(cWindowManager, "getDefaultDisplay", "()Landroid/view/Display;");
    jmethodID getRotation       = gEnv->GetMethodID(cDisplay, "getRotation", "()I");
    jobject   windowManager     = gEnv->CallObjectMethod(inst, getWindowManager);
    jobject   display           = gEnv->CallObjectMethod(windowManager, getDefaultDisplay);

    return gEnv->CallIntMethod(display, getRotation);
}

void engineHandleCmd(struct android_app* app, int32_t cmd) {
    static int prevOrientation = 0;

    auto* state = static_cast<SavedState*>(app->userData);
    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
            if (state->app->window && !gWindowInfo.windowHandle) {
                gWindowInfo.windowHandle  = state->app->window;
                gWindowInfo.screen.width  = ANativeWindow_getWidth(app->window);
                gWindowInfo.screen.height = ANativeWindow_getHeight(app->window);
                gWindowInfo.screen.x = gWindowInfo.screen.y = 0;

                cc::TestBaseI::setWindowInfo(gWindowInfo);
                cc::TestBaseI::nextTest();
            } else {
                cc::CustomEvent event;
                event.name         = EVENT_RECREATE_WINDOW;
                event.args->ptrVal = state->app->window;
                cc::EventDispatcher::dispatchCustomEvent(event);
            }
            break;
        case APP_CMD_TERM_WINDOW: {
            cc::CustomEvent event;
            event.name = EVENT_DESTROY_WINDOW;
            cc::EventDispatcher::dispatchCustomEvent(event);
            state->animating = 0;
            break;
        }
        case APP_CMD_CONFIG_CHANGED: {
            uint32_t width     = ANativeWindow_getWidth(app->window);
            uint32_t height    = ANativeWindow_getHeight(app->window);
            int      transform = getOrientation(app);
            if ((transform ^ prevOrientation) & 1) {
                std::swap(width, height); // orientation changed, swap sides
            }
            prevOrientation = transform;
            cc::TestBaseI::resizeGlobal(state->app->window, width, height, static_cast<cc::gfx::SurfaceTransform>(transform));
            break;
        }
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
int32_t engineHandleInput(struct android_app* /*app*/, AInputEvent* event) {
    int type = AInputEvent_getType(event);
    if (type == AINPUT_EVENT_TYPE_KEY) {
        return 1;
    }
    if (type == AINPUT_EVENT_TYPE_MOTION) {
        int action = AMotionEvent_getAction(event);
        switch (action) {
            case AMOTION_EVENT_ACTION_UP:
            case AMOTION_EVENT_ACTION_POINTER_UP: {
                auto width = static_cast<float>(cc::TestBaseI::swapchains[0]->getWidth());
                cc::TestBaseI::nextTest(AMotionEvent_getX(event, 0) < width / 2.F);
                break;
            }
        }
        return 1;
    }
    return 0;
}
} // namespace

void android_main(android_app* app) { // NOLINT(readability-identifier-naming)
    struct SavedState savedState;
    memset(&savedState, 0, sizeof(savedState));
    app->userData     = &savedState;
    app->onAppCmd     = engineHandleCmd;
    app->onInputEvent = engineHandleInput;
    savedState.app    = app;

    app->activity->vm->AttachCurrentThread(&gEnv, nullptr);
    cc::JniHelper::init(gEnv, app->activity->clazz);

    cc::FileUtilsAndroid::setassetmanager(app->activity->assetManager);

    while (true) {
        // Read all pending events.
        int                         events;
        struct android_poll_source* source;

        // If not animating, we will block forever waiting for events.
        // If animating, we loop until all events are read, then continue
        // to draw the next frame of animation.
        while (ALooper_pollAll(savedState.animating ? 0 : -1, nullptr, &events,
                               reinterpret_cast<void**>(&source)) >= 0) {
            // Process this event.
            if (source != nullptr) {
                source->process(app, source);
            }
        }

        if (app->destroyRequested != 0) {
            cc::TestBaseI::destroyGlobal();
            return;
        }

        if (savedState.animating) {
            cc::TestBaseI::update();
        }
    }
}
