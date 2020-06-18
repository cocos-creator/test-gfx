#pragma once

#include "TestBase.h"

namespace cc {

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
    
    gfx::GFXShader* _shader = nullptr;
    gfx::GFXBuffer* _vertexBuffer = nullptr;
    gfx::GFXBuffer* _indexBuffer = nullptr;
    gfx::GFXBuffer* _mvpMatrix = nullptr;
    gfx::GFXBuffer* _color = nullptr;
    gfx::GFXBindingLayout* _bindingLayout = nullptr;
    gfx::GFXInputAssembler* _inputAssembler = nullptr;
    gfx::GFXPipelineState* _pipelineState = nullptr;
    gfx::GFXPipelineLayout* _pipelineLayout = nullptr;
    
    Mat4 _view;
    float _dt = 0.0f;
};

} // namespace cc
