#pragma once

#include "TestBase.h"

namespace cc {

class ComputeTest : public TestBaseI {
public:
    DEFINE_CREATE_METHOD(ComputeTest)
    ComputeTest(const WindowInfo &info) : TestBaseI(info){};
    ~ComputeTest() = default;

public:
    bool onInit() override;
    void onTick() override;
    void onDestroy() override;

private:
    void createShader();
    void createUniformBuffer();
    void createPipeline();
    void createInputAssembler();

    void createComputeVBPipeline();
    void createComputeBGPipeline();

    gfx::Shader *             _shader              = nullptr;
    gfx::Buffer *             _uniformBufferMVP    = nullptr;
    gfx::DescriptorSet *      _descriptorSet       = nullptr;
    gfx::DescriptorSetLayout *_descriptorSetLayout = nullptr;
    gfx::PipelineLayout *     _pipelineLayout      = nullptr;
    gfx::PipelineState *      _pipelineState       = nullptr;
    gfx::InputAssembler *     _inputAssembler      = nullptr;
    gfx::RenderPass *         _renderPassLoad      = nullptr;

    gfx::Buffer *             _compConstantsBuffer     = nullptr;
    gfx::Buffer *             _compStorageBuffer       = nullptr;
    gfx::Shader *             _compShader              = nullptr;
    gfx::DescriptorSetLayout *_compDescriptorSetLayout = nullptr;
    gfx::PipelineLayout *     _compPipelineLayout      = nullptr;
    gfx::PipelineState *      _compPipelineState       = nullptr;
    gfx::DescriptorSet *      _compDescriptorSet       = nullptr;

    gfx::Shader *             _compBGShader              = nullptr;
    gfx::DescriptorSetLayout *_compBGDescriptorSetLayout = nullptr;
    gfx::PipelineLayout *     _compBGPipelineLayout      = nullptr;
    gfx::PipelineState *      _compBGPipelineState       = nullptr;
    gfx::DescriptorSet *      _compBGDescriptorSet       = nullptr;
};

} // namespace cc
