#pragma once

#include "TestBase.h"

namespace cc {

class StressTest: public TestBaseI
{
 public:
    DEFINE_CREATE_METHOD(StressTest)
    StressTest(const WindowInfo& info) : TestBaseI(info) {};
    ~StressTest() = default;

 public:
     virtual void tick() override;
     virtual bool initialize() override;
     virtual void destroy() override;

 private:
    void createShader();
    void createVertexBuffer();
    void createPipeline();
    void createInputAssembler();

    void recordRenderPass(uint index);

    gfx::Shader *_shader = nullptr;
    gfx::Buffer *_vertexBuffer = nullptr;
    gfx::Buffer *_uniformBufferVP = nullptr;

    gfx::Buffer *_uniWorldBuffer = nullptr, *_uniWorldBufferView = nullptr;
    gfx::DescriptorSet* _uniDescriptorSet = nullptr;

    vector<gfx::Buffer *> _worldBuffers;
    vector<gfx::DescriptorSet *> _descriptorSets;

    gfx::DescriptorSetLayout* _descriptorSetLayout = nullptr;
    gfx::PipelineLayout* _pipelineLayout = nullptr;
    gfx::PipelineState* _pipelineState = nullptr;
    gfx::InputAssembler* _inputAssembler = nullptr;

    uint _worldBufferStride = 0u;
    JobGraph _jobGraph;
};

} // namespace cc
