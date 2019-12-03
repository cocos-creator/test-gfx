#pragma once
#include "Core.h"

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

    virtual bool initialize() { return true; };
    virtual void Destroy();
    virtual ~TestBaseI() = default;

protected:
    GFXDevice* _device               = nullptr;
    GFXFramebuffer* _fbo             = nullptr;
    GFXCommandBuffer* _commandBuffer = nullptr;

};

NS_CC_END
