#pragma once
#include "Core.h"
#include "cocos2d.h"

namespace cc {

    typedef struct WindowInfo {
        intptr_t windowHandle;
        gfx::Rect screen;
        int physicalWidth;
        int physicalHeight;
    } WindowInfo;

    struct Framebuffer {
        gfx::RenderPass *renderPass = nullptr;
        gfx::Texture *colorTex = nullptr;
        gfx::Texture *depthStencilTex = nullptr;
        gfx::Framebuffer *framebuffer = nullptr;

        ~Framebuffer() {
            CC_SAFE_DESTROY(framebuffer);
            CC_SAFE_DESTROY(depthStencilTex);
            CC_SAFE_DESTROY(colorTex);
            CC_SAFE_DESTROY(renderPass);
        }
    };

    struct ShaderSource {
        String vert;
        String frag;
    };
    struct ShaderSources {
        ShaderSource glsl4;
        ShaderSource glsl3;
        ShaderSource glsl1;
    };

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

        virtual bool initialize() { return true; }
        virtual void destroy() {}
        virtual void tick(float dt) = 0;
        virtual void resize(uint width, uint height) { _device->resize(width, height); }

        static gfx::Device *getDevice() { return _device; }
        static void destroyGlobal();

        static unsigned char *RGB2RGBA(Image *img);
        static void modifyProjectionBasedOnDevice(Mat4 &projection, bool isOffscreen = false);
        static void createOrthographic(float left, float right, float bottom, float top, float near, float ZFar, Mat4 *dst, bool isOffscreen = false);
        static void createPerspective(float fov, float aspect, float near, float ZFar, Mat4 *dst, bool isOffscreen = false);
        static gfx::Extent getOrientedSurfaceSize();
        static gfx::Viewport getViewportBasedOnDevice(const Vec4 &relativeArea);
        static uint getUBOSize(uint size);
        static uint getMipmapLevelCounts(uint width, uint height);
        static ShaderSource &getAppropriateShaderSource(ShaderSources &sources);
        static uint getAlignedUBOStride(gfx::Device *device, uint stride);

    protected:
        static gfx::Device *_device;
        static gfx::Framebuffer* _fbo;
        static std::vector<gfx::CommandBuffer *> _commandBuffers;

        static gfx::RenderPass *_renderPass;
    };

} // namespace cc
