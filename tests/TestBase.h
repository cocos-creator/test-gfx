#pragma once
#include "base/CoreStd.h"
#include "math/Mat4.h"
#include "math/Vec4.h"
#include "platform/Image.h"
#include "renderer/frame-graph/FrameGraph.h"
#include "renderer/gfx-base/GFXDef.h"
#include "renderer/gfx-base/GFXDevice.h"

namespace cc {

typedef struct WindowInfo {
    intptr_t  windowHandle;
    gfx::Rect screen;
    int       physicalWidth;
    int       physicalHeight;
} WindowInfo;

struct Framebuffer {
    gfx::RenderPass * renderPass      = nullptr;
    gfx::Texture *    colorTex        = nullptr;
    gfx::Texture *    depthStencilTex = nullptr;
    gfx::Framebuffer *framebuffer     = nullptr;

    ~Framebuffer() {
        CC_SAFE_DESTROY(framebuffer);
        CC_SAFE_DESTROY(depthStencilTex);
        CC_SAFE_DESTROY(colorTex);
        CC_SAFE_DESTROY(renderPass);
    }
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
};

struct FrameRate {
    std::chrono::steady_clock::time_point prevTime;
    std::chrono::steady_clock::time_point curTime;
    float                                 dt{0};

    uint  frameAcc = 0u;
    float timeAcc  = 0.f;
};

template <typename T, size_t Size>
char (*countofHelper(T (&_Array)[Size]))[Size];

#define COUNTOF(array) (sizeof(*countofHelper(array)) + 0)

#define DEFINE_CREATE_METHOD(className)                \
    static TestBaseI *create(const WindowInfo &info) { \
        TestBaseI *test = CC_NEW(className(info));     \
        if (test->initialize())                        \
            return test;                               \
        CC_SAFE_DESTROY(test);                         \
        return nullptr;                                \
    }

class TestBaseI : public cc::Object {
public:
    TestBaseI(const WindowInfo &info);
    virtual ~TestBaseI() = default;

    using createFunc = TestBaseI *(*)(const WindowInfo &info);

    static void lookupTime(FrameRate &statistics = hostThread) {
        statistics.curTime  = std::chrono::steady_clock::now();
        statistics.dt       = float(std::chrono::duration_cast<std::chrono::nanoseconds>(statistics.curTime - statistics.prevTime).count()) / NANOSECONDS_PER_SECOND;
        statistics.prevTime = statistics.curTime;
    }
    static gfx::Device *getDevice() { return _device; }
    static void         resizeGlobal(uint width, uint height) {
        if (_test) _test->resize(width, height);
    }
    static void setWindowInfo(const WindowInfo &info) { _windowInfo = info; }

    static void nextTest(bool backward = false);
    static void destroyGlobal();
    static void toggleMultithread();
    static void onTouchEnd();
    static void update();

    static void                 evalString(std::string code);
    static void                 runScript(std::string file);
    static void                 tickScript();
    static unsigned char *      RGB2RGBA(Image *img);
    static gfx::Texture *       createTextureWithFile(gfx::Device *device, gfx::TextureInfo &textureInfo, std::string imageFile);
    static void                 modifyProjectionBasedOnDevice(Mat4 &projection, bool isOffscreen = false);
    static void                 createOrthographic(float left, float right, float bottom, float top, float near, float ZFar, Mat4 *dst, bool isOffscreen = false);
    static void                 createPerspective(float fov, float aspect, float near, float ZFar, Mat4 *dst, bool isOffscreen = false);
    static gfx::Extent          getOrientedSurfaceSize();
    static gfx::Viewport        getViewportBasedOnDevice(const Vec4 &relativeArea);
    static uint                 getUBOSize(uint size);
    static uint                 getMipmapLevelCounts(uint width, uint height);
    static uint                 getAlignedUBOStride(gfx::Device *device, uint stride);
    static gfx::GlobalBarrier * getGlobalBarrier(const gfx::GlobalBarrierInfo &info);
    static gfx::TextureBarrier *getTextureBarrier(const gfx::TextureBarrierInfo &info);

    template <typename T>
    static T &getAppropriateShaderSource(ShaderSources<T> &sources) {
        switch (_device->getGfxAPI()) {
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
    static FrameRate  hostThread;
    static FrameRate  deviceThread;
    static const bool MANUAL_BARRIER;

    static const uint NANOSECONDS_PER_SECOND;
    static const uint NANOSECONDS_60FPS;

    virtual bool onInit() { return true; }
    virtual void onTick() {}
    virtual void onResize(uint width, uint height) {}
    virtual void onDestroy() {}

    CC_INLINE bool initialize() {
        return onInit();
    }

    CC_INLINE void tick() {
        lookupTime();

        _time += hostThread.dt;

        onTick();

        ++_frameCount;
    }

    CC_INLINE void resize(uint width, uint height) {
        onResize(width, height);

        _device->resize(width, height);
    }

    CC_INLINE void destroy() {
        onDestroy();

        for (auto texture : _textures) {
            CC_SAFE_DESTROY(texture);
        }
        _textures.clear();
    }

protected:
    static WindowInfo              _windowInfo;
    static int                     _curTestIndex;
    static std::vector<createFunc> _tests;
    static TestBaseI *             _test;
    static int                     _nextDirection;

    static gfx::Device *                     _device;
    static gfx::Framebuffer *                _fbo;
    static std::vector<gfx::CommandBuffer *> _commandBuffers;

    static gfx::RenderPass *_renderPass;

    static std::unordered_map<uint, gfx::GlobalBarrier *>  _globalBarrierMap;
    static std::unordered_map<uint, gfx::TextureBarrier *> _textureBarrierMap;

    std::vector<gfx::GlobalBarrier *> _globalBarriers;

    std::vector<gfx::Texture *>        _textures;
    std::vector<gfx::TextureBarrier *> _textureBarriers;

    float _time       = 0.f;
    uint  _frameCount = 0u;
};

} // namespace cc
