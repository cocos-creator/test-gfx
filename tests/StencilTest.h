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
    gfx::Shader* _shader = nullptr;
    gfx::Texture* _labelTexture = nullptr;
    gfx::Texture* _uvCheckerTexture = nullptr;
    gfx::Buffer* _vertexBuffer = nullptr;
    gfx::InputAssembler* _inputAssembler = nullptr;
    gfx::Buffer* _uniformBuffer[BINDING_COUNT] = { nullptr };
    gfx::BindingLayout* _bindingLayout[BINDING_COUNT] = { nullptr };
    gfx::PipelineState* _pipelineState[PIPELIE_COUNT] = { nullptr };
    gfx::Sampler* _sampler = nullptr;
    
    float _dt = 0.0f;
};

} // namespace cc
