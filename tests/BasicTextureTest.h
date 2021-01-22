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
     virtual void tick() override;
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
    gfx::DescriptorSet* _descriptorSet = nullptr;
    gfx::DescriptorSetLayout* _descriptorSetLayout = nullptr;
    gfx::PipelineLayout* _pipelineLayout = nullptr;
    gfx::PipelineState* _pipelineState = nullptr;
    gfx::Texture* _texture = nullptr;
    gfx::Texture* _texture2 = nullptr;
    gfx::Buffer* _image = nullptr;
    gfx::Sampler* _sampler = nullptr;

    float _time = 0.f;
};

} // namespace cc
