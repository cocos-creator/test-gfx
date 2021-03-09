#pragma once

#include "TestBase.h"

namespace cc {

class FrameGraphTest : public TestBaseI {
public:
    DEFINE_CREATE_METHOD(FrameGraphTest)
    FrameGraphTest(const WindowInfo &info) : TestBaseI(info){};
    ~FrameGraphTest() = default;

public:
    bool onInit() override;
    void onTick() override;
    void onDestroy() override;

private:
    void createShader();
    void createVertexBuffer();
    void createPipeline();
    void createInputAssembler();

    void setupPasses();

    gfx::Shader *             _shader              = nullptr;
    gfx::Buffer *             _vertexBuffer        = nullptr;
    gfx::Buffer *             _uniformBuffer       = nullptr;
    gfx::Buffer *             _uniformBufferMVP    = nullptr;
    gfx::DescriptorSet *      _descriptorSet       = nullptr;
    gfx::DescriptorSetLayout *_descriptorSetLayout = nullptr;
    gfx::PipelineLayout *     _pipelineLayout      = nullptr;
    gfx::PipelineState *      _pipelineState       = nullptr;
    gfx::InputAssembler *     _inputAssembler      = nullptr;
    gfx::Buffer *             _indexBuffer         = nullptr;

    framegraph::FrameGraph  fg;

    framegraph::Texture _backBuffer;
    framegraph::Texture _depthStencilBackBuffer;
};

} // namespace cc
