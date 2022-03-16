#pragma once

#include "TestBase.h"

namespace cc {

class MultiViewportTest : public TestBaseI {
public:
    DEFINE_CREATE_METHOD(MultiViewportTest)
    using TestBaseI::TestBaseI;

    bool onInit() override;
    void onTick() override;
    void onDestroy() override;
    void updateUniform();

    struct UBOSetting {
        Mat4 modelview[2];
        Mat4 projection[2];
    };

private:
    void createShader();
    void createVertexBuffer();
    void createPipeline();
    void createInputAssembler();

    gfx::Shader *             _shader                 = nullptr;
    gfx::Buffer *             _vertexBuffer           = nullptr;
    gfx::Buffer *             _uniformBuffer          = nullptr;
    gfx::DescriptorSet *      _descriptorSet          = nullptr;
    gfx::DescriptorSetLayout *_descriptorSetLayout    = nullptr;
    gfx::PipelineLayout *     _pipelineLayout         = nullptr;
    gfx::PipelineState *      _pipelineState          = nullptr;
    gfx::PipelineState *      _invisiblePipelineState = nullptr;
    gfx::InputAssembler *     _inputAssembler         = nullptr;
    gfx::Buffer *             _indirectBuffer         = nullptr;
    gfx::Buffer *             _indexBuffer            = nullptr;

    UBOSetting _setting;
};

} // namespace cc
