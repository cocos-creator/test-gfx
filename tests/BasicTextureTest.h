#pragma once

#include "TestBase.h"

namespace cc {

class BasicTexture: public TestBaseI
{
public:
    DEFINE_CREATE_METHOD(BasicTexture)
    BasicTexture(const WindowInfo& info) : TestBaseI(info) {};
    ~BasicTexture() = default;

public:
     virtual void tick(float dt) override;
     virtual bool initialize() override;
     virtual void destroy() override;

private:
    void createShader();
    void createVertexBuffer();
    void createPipeline();
    void createInputAssembler();
    void createTexture();

    gfx::GFXShader* _shader = nullptr;
    gfx::GFXBuffer* _vertexBuffer = nullptr;
    gfx::GFXBuffer* _uniformBuffer = nullptr;
    gfx::GFXInputAssembler* _inputAssembler = nullptr;
    gfx::GFXBindingLayout* _bindingLayout = nullptr;
    gfx::GFXPipelineLayout* _pipelineLayout = nullptr;
    gfx::GFXPipelineState* _pipelineState = nullptr;
    gfx::GFXTexture* _texture = nullptr;
    gfx::GFXBuffer* _image = nullptr;
    gfx::GFXSampler* _sampler = nullptr;
};

} // namespace cc
