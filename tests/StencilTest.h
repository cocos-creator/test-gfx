#pragma once

#include "TestBase.h"

namespace cc {

class StencilTest: public TestBaseI
{
public:
    DEFINE_CREATE_METHOD(StencilTest)
    StencilTest(const WindowInfo& info) : TestBaseI(info) {};
    ~StencilTest() = default;

public:
     virtual void tick(float dt) override;
     virtual bool initialize() override;
     virtual void destroy() override;

private:
    void createShader();
    void createBuffers();
    void createTextures();
    void createInputAssembler();
    void createPipelineState();
    
    const static uint BINDING_COUNT = 2;
    const static uint PIPELIE_COUNT = 6;
    gfx::GFXShader* _shader = nullptr;
    gfx::GFXTexture* _labelTexture = nullptr;
    gfx::GFXTexture* _uvCheckerTexture = nullptr;
    gfx::GFXBuffer* _vertexBuffer = nullptr;
    gfx::GFXInputAssembler* _inputAssembler = nullptr;
    gfx::GFXBuffer* _uniformBuffer[BINDING_COUNT] = { nullptr };
    gfx::GFXBindingLayout* _bindingLayout[BINDING_COUNT] = { nullptr };
    gfx::GFXPipelineLayout* _pipelineLayout[BINDING_COUNT] = { nullptr };
    gfx::GFXPipelineState* _pipelineState[PIPELIE_COUNT] = { nullptr };
    gfx::GFXSampler* _sampler = nullptr;
    
    float _dt = 0.0f;
};

} // namespace cc
