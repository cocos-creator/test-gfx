#pragma once

#include "TestBase.h"
#include "GLES2Std.h"
#include "math/Mat4.h"

NS_CC_BEGIN

class DepthTexture: public TestBaseI
{
public:
    DEFINE_CREATE_METHOD(DepthTexture)
    DepthTexture(const WindowInfo& info) : TestBaseI(info) {};
    ~DepthTexture() = default;

public:
     virtual void tick(float dt) override;
     virtual bool initialize() override;
     virtual void destroy() override;

private:
    void createFBO();

    GFXWindow* _bunnyWindow = nullptr;
    
    cocos2d::Mat4 _view;
    cocos2d::Mat4 _model;
    cocos2d::Mat4 _projection;
    cocos2d::Vec3 _eye;
    cocos2d::Vec3 _center;
    cocos2d::Vec3 _up;
    
    float _dt = 0.0f;
};

NS_CC_END
