#pragma once

#include "TestBase.h"

namespace cc {

class Multithread: public TestBaseI
{
 public:
    DEFINE_CREATE_METHOD(Multithread)
    Multithread(const WindowInfo& info) : TestBaseI(info) {};
    ~Multithread() = default;

 public:
     virtual void tick(float dt) override;
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

     uint _frameAccumulator = 0u;
     float _timeAccumulator = 0.f;
};

} // namespace cc
