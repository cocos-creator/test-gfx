#pragma once
#include "Core.h"

#define USE_VULKAN

NS_CC_BEGIN

typedef struct WindowInfo
{
    intptr_t windowHandle;
    GFXRect screen;
    int physicalWidth;
    int physicalHeight;
}WindowInfo;

struct Framebuffer
{
    GFXRenderPass* renderPass = nullptr;
    GFXTexture* colorTex = nullptr;
    GFXTextureView* colorTexView = nullptr;
    GFXTexture* depthStencilTex = nullptr;
    GFXTextureView* depthStencilTexView = nullptr;
    GFXFramebuffer* framebuffer = nullptr;

    ~Framebuffer()
    {
        CC_SAFE_DESTROY(framebuffer);
        CC_SAFE_DESTROY(depthStencilTexView);
        CC_SAFE_DESTROY(depthStencilTex);
        CC_SAFE_DESTROY(colorTexView);
        CC_SAFE_DESTROY(colorTex);
        CC_SAFE_DESTROY(renderPass);
    }
};

#define DEFINE_CREATE_METHOD(className) \
static TestBaseI* create(const WindowInfo& info) { \
    TestBaseI* test =  CC_NEW(className(info)); \
    if (test->initialize()) return test; else { CC_SAFE_DESTROY(test); return nullptr; } \
}

class TestBaseI : public Object
{
public:
    TestBaseI(const WindowInfo& info);
    virtual void tick(float dt) = 0;

    virtual bool initialize() { return true; }
    virtual void destroy() {}
    virtual ~TestBaseI() = default;

    static GFXDevice* getDevice() { return _device; }
    static void destroyGlobal();

protected:
    static GFXDevice* _device;
    static GFXFramebuffer* _fbo;
    static std::vector<GFXCommandBuffer*> _commandBuffers;
};

NS_CC_END
