#pragma once

#include "TestBase.h"

namespace cc {

class BasicTexture : public TestBaseI {
public:
    DEFINE_CREATE_METHOD(BasicTexture)
    using TestBaseI::TestBaseI;

    void onTick() override;
    bool onInit() override;
    void onDestroy() override;

private:
    void createShader();
    void createVertexBuffer();
    void createPipeline();
    void createInputAssembler();
    void createTexture();

    gfx::Shader *             _shader              = nullptr;
    gfx::Buffer *             _vertexBuffer        = nullptr;
    gfx::Buffer *             _uniformBuffer       = nullptr;
    gfx::InputAssembler *     _inputAssembler      = nullptr;
    gfx::DescriptorSet *      _descriptorSet       = nullptr;
    gfx::DescriptorSetLayout *_descriptorSetLayout = nullptr;
    gfx::PipelineLayout *     _pipelineLayout      = nullptr;
    gfx::PipelineState *      _pipelineState       = nullptr;
    gfx::Sampler *            _sampler             = nullptr;
};

} // namespace cc
