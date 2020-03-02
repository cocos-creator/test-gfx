#pragma once

#include "TestBase.h"
#include "math/Mat4.h"

NS_CC_BEGIN

class BlendTest: public TestBaseI
{
public:
    DEFINE_CREATE_METHOD(BlendTest)
    BlendTest(const WindowInfo& info) : TestBaseI(info) {};
    ~BlendTest() = default;

public:
     virtual void tick(float dt) override;
     virtual bool initialize() override;
     virtual void destroy() override;

private:
   
    float _dt = 0.0f;
};

NS_CC_END
