#pragma once

#include "base/CoreStd.h"
#include "math/Mat4.h"
#include "math/Vec4.h"
#include "platform/Image.h"
#include "renderer/frame-graph/FrameGraph.h"
#include "renderer/gfx-base/GFXDef.h"
#include "renderer/gfx-base/GFXDevice.h"

#include <algorithm>
#include <iterator>

namespace tinyobj {
class ObjReader;
}

namespace cc {

using WindowInfo = struct WindowInfo {
    intptr_t  windowHandle = 0;
    gfx::Rect screen;
    float     pixelRatio = 1.F;
};

struct StandardShaderSource {
    String vert;
    String frag;
};

using ComputeShaderSource = String;

template <typename T>
struct ShaderSources {
    T glsl4;
    T glsl3;
    T glsl1;

    ShaderSources<T> operator+(const T &b) const {
        ShaderSources<T> res;
        res.glsl4 = glsl4 + b;
        res.glsl3 = glsl3 + b;
        res.glsl1 = glsl1 + b;
        return res;
    }

    ShaderSources<T> &operator+=(const T &b) {
        glsl4 += b;
        glsl3 += b;
        glsl1 += b;
        return *this;
    }

    ShaderSources<T> operator+(const ShaderSources<T> &b) const {
        ShaderSources<T> res;
        res.glsl4 = glsl4 + b.glsl4;
        res.glsl3 = glsl3 + b.glsl3;
        res.glsl1 = glsl1 + b.glsl1;
        return res;
    }

    ShaderSources<T> &operator+=(const ShaderSources<T> &b) {
        glsl4 += b.glsl4;
        glsl3 += b.glsl3;
        glsl1 += b.glsl1;
        return *this;
    }
};

struct FrameRate {
    std::chrono::steady_clock::time_point prevTime;
    std::chrono::steady_clock::time_point curTime;
    float                                 dt{0};

    uint  frameAcc = 0U;
    float timeAcc  = 0.F;
};

#define DEFINE_CREATE_METHOD(className)                \
    static TestBaseI *create(const WindowInfo &info) { \
        TestBaseI *instance = CC_NEW(className(info)); \
        if (instance->initialize())                    \
            return instance;                           \
        CC_SAFE_DESTROY(instance);                     \
        return nullptr;                                \
    }

class TestBaseI : public cc::Object {
public:
    explicit TestBaseI(const WindowInfo &info);

    using createFunc = TestBaseI *(*)(const WindowInfo &info);

    static void lookupTime(FrameRate *statistics = &logicThread) {
        if (!statistics->frameAcc) {
            statistics->prevTime = statistics->curTime = std::chrono::steady_clock::now();
        } else {
            statistics->prevTime = statistics->curTime;
            statistics->curTime  = std::chrono::steady_clock::now();
            auto time            = std::chrono::duration_cast<std::chrono::nanoseconds>(statistics->curTime - statistics->prevTime);
            statistics->dt       = static_cast<float>(time.count()) / NANOSECONDS_PER_SECOND;

            if (statistics->timeAcc == 0.F) {
                statistics->timeAcc = statistics->dt;
            } else {
                statistics->timeAcc = statistics->timeAcc * 0.95F + statistics->dt * 0.05F;
            }
        }

        ++statistics->frameAcc;
    }
    static void printTime(const FrameRate &statistics = logicThread, const String &prefix = "Logic thread") {
        if (statistics.frameAcc % 6 == 0) {
            CC_LOG_INFO("%s: frame %d, avg: %.2fms (~%ld FPS)",
                        prefix.c_str(),
                        statistics.frameAcc,
                        statistics.timeAcc * 1000.F,
                        lround(1.F / statistics.timeAcc));
        }
    }
    static gfx::Device *getDevice() { return device; }
    static void         resizeGlobal(uint width, uint height) {
        if (test) test->resize(width, height);
    }
    static void setWindowInfo(const WindowInfo &info) { windowInfo = info; }
    static void spacePressed() { test->onSpacePressed(); }

    static void nextTest(bool backward = false);
    static void destroyGlobal();
    static void toggleMultithread();
    static void onTouchEnd();
    static void update();

    static void                 evalString(const String &code);
    static void                 runScript(const String &file);
    static void                 tickScript();
    static void                 scriptEngineGC();
    static unsigned char *      rgb2rgba(Image *img);
    static gfx::Texture *       createTextureWithFile(const gfx::TextureInfo &partialInfo, const String &imageFile);
    static void                 modifyProjectionBasedOnDevice(Mat4 *projection);
    static void                 createOrthographic(float left, float right, float bottom, float top, float near, float zFar, Mat4 *dst);
    static void                 createPerspective(float fov, float aspect, float near, float zFar, Mat4 *dst);
    static gfx::Extent          getOrientedSurfaceSize();
    static gfx::Viewport        getViewportBasedOnDevice(const Vec4 &relativeArea);
    static uint                 getUBOSize(uint size);
    static uint                 getMipmapLevelCounts(uint width, uint height);
    static uint                 getAlignedUBOStride(uint stride);
    static tinyobj::ObjReader   loadOBJ(const String &path);
    static gfx::GlobalBarrier * getGlobalBarrier(const gfx::GlobalBarrierInfo &info);
    static gfx::TextureBarrier *getTextureBarrier(const gfx::TextureBarrierInfo &info);
    static void                 createUberBuffer(const vector<uint> &sizes, gfx::Buffer **pBuffer,
                                                 vector<gfx::Buffer *> *pBufferViews, vector<uint> *pBufferViewOffsets);

    template <typename T>
    static T &getAppropriateShaderSource(ShaderSources<T> &sources) {
        switch (device->getGfxAPI()) {
            case gfx::API::GLES2:
                return sources.glsl1;
            case gfx::API::GLES3:
                return sources.glsl3;
            case gfx::API::METAL:
            case gfx::API::VULKAN:
                return sources.glsl4;
            default: break;
        }
        return sources.glsl4;
    }

    // FPS calculation
    static FrameRate  logicThread;
    static FrameRate  renderThread;
    static FrameRate  deviceThread;
    static const bool MANUAL_BARRIER;

    static const float NANOSECONDS_PER_SECOND;

    virtual bool onInit() { return true; }
    virtual void onTick() {}
    virtual void onResize(uint width, uint height) {}
    virtual void onDestroy() {}

    CC_INLINE bool initialize() {
        return onInit();
    }

    CC_INLINE void tick() {
        lookupTime();

        _time += logicThread.dt;

        onTick();

        ++_frameCount;
    }

    CC_INLINE void resize(uint width, uint height) {
        onResize(width, height);

        device->resize(width, height);
    }

    CC_INLINE void destroy() {
        onDestroy();

        for (auto *texture : _textures) {
            CC_SAFE_DESTROY(texture)
        }
        _textures.clear();
    }

protected:
    static WindowInfo         windowInfo;
    static int                curTestIndex;
    static vector<createFunc> tests;
    static TestBaseI *        test;
    static int                nextDirection;

    static gfx::Device *                device;
    static gfx::Framebuffer *           fbo;
    static vector<gfx::CommandBuffer *> commandBuffers;

    static gfx::RenderPass *renderPass;

    static unordered_map<uint, gfx::GlobalBarrier *>  globalBarrierMap;
    static unordered_map<uint, gfx::TextureBarrier *> textureBarrierMap;

    static framegraph::FrameGraph fg;
    static framegraph::Texture    fgBackBuffer;
    static framegraph::Texture    fgDepthStencilBackBuffer;

    virtual void onSpacePressed() {}

    vector<gfx::GlobalBarrier *> _globalBarriers;

    vector<gfx::Texture *>        _textures;
    vector<gfx::TextureBarrier *> _textureBarriers;

    float _time       = 0.F;
    uint  _frameCount = 0U;

    friend class Root;
};

} // namespace cc
