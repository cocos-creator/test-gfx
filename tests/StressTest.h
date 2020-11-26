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

    //     ThreadPool _tp;

    gfx::Shader *_shader = nullptr;
    gfx::Buffer *_vertexBuffer = nullptr;
    gfx::Buffer *_uniformBufferWorld, *_uniformBufferWorldView;
    gfx::Buffer *_uniformBufferVP = nullptr;
    gfx::DescriptorSet* _descriptorSet = nullptr;
    gfx::DescriptorSetLayout* _descriptorSetLayout = nullptr;
    gfx::PipelineLayout* _pipelineLayout = nullptr;
    gfx::PipelineState* _pipelineState = nullptr;
    gfx::InputAssembler* _inputAssembler = nullptr;

    uint _worldBufferStride = 0u;
};

} // namespace cc
