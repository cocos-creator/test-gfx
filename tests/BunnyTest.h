#pragma once

#include "TestBase.h"
#include "math/Mat4.h"

NS_CC_BEGIN

class BunnyTest: public TestBaseI
{
public:
    DEFINE_CREATE_METHOD(BunnyTest)
    BunnyTest(const WindowInfo& info) : TestBaseI(info) {};
    ~BunnyTest() = default;

public:
     virtual void tick(float dt) override;
     virtual bool initialize() override;
     virtual void destroy() override;

private:
    void createShader();
    void createBuffers();
    void createInputAssembler();
    void createPipelineState();
    
    GFXShader* _shader = nullptr;
    GFXBuffer* _vertexBuffer = nullptr;
    GFXBuffer* _indexBuffer = nullptr;
    GFXBuffer* _mvpMatrix = nullptr;
    GFXBuffer* _color = nullptr;
    GFXBindingLayout* _bindingLayout = nullptr;
    GFXInputAssembler* _inputAssembler = nullptr;
    GFXPipelineState* _pipelineState = nullptr;
    
    Mat4 _view;
    float _dt = 0.0f;
};

NS_CC_END
