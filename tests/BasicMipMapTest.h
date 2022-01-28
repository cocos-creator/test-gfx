#pragma once

#include "TestBase.h"

namespace cc {

class BasicMipMap : public TestBaseI {
public:
    DEFINE_CREATE_METHOD(BasicMipMap)
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

    vector<gfx::Texture*> _textureViews = {};

    gfx::Shader*              _shader              = nullptr;
    gfx::Buffer*              _vertexBuffer        = nullptr;
    gfx::Buffer*              _uniformBuffer       = nullptr;
    gfx::InputAssembler*      _inputAssembler      = nullptr;
    gfx::DescriptorSet*       _descriptorSet       = nullptr;
    gfx::DescriptorSetLayout* _descriptorSetLayout = nullptr;
    gfx::PipelineLayout*      _pipelineLayout      = nullptr;
    gfx::PipelineState*       _pipelineState       = nullptr;
};

} // namespace cc
