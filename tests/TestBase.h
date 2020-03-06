#pragma once
#include "Core.h"

#define USE_GLES2

NS_CC_BEGIN

typedef struct WindowInfo
{
    intptr_t windowHandle;
    GFXRect screen;
    int physicalWidth;
    int physicalHeight;
}WindowInfo;

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
    virtual void destroy();
    virtual ~TestBaseI() = default;

    static GFXDevice* getDevice() { return _device; }

protected:
    static GFXDevice* _device;
    static GFXFramebuffer* _fbo;
    static std::vector<GFXCommandBuffer*> _commandBuffers;
};

NS_CC_END
