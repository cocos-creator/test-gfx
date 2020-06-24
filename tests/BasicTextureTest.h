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

    gfx::Shader* _shader = nullptr;
    gfx::Buffer* _vertexBuffer = nullptr;
    gfx::Buffer* _uniformBuffer = nullptr;
    gfx::InputAssembler* _inputAssembler = nullptr;
    gfx::BindingLayout* _bindingLayout = nullptr;
    gfx::PipelineState* _pipelineState = nullptr;
    gfx::Texture* _texture = nullptr;
    gfx::Buffer* _image = nullptr;
    gfx::Sampler* _sampler = nullptr;
};

} // namespace cc
