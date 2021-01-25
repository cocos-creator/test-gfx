#pragma once

#include "TestBase.h"

namespace cc {

typedef struct Vertex {
    float pos[2];
    float col[3];
} Vertex;

class BasicTriangle : public TestBaseI {
public:
    DEFINE_CREATE_METHOD(BasicTriangle)
    BasicTriangle(const WindowInfo &info) : TestBaseI(info){};
    ~BasicTriangle() = default;

public:
    bool onInit() override;
    void onTick() override;
    void onDestroy() override;

private:
    void createShader();
    void createVertexBuffer();
    void createPipeline();
    void createInputAssembler();

    gfx::Shader *             _shader              = nullptr;
    gfx::Buffer *             _vertexBuffer        = nullptr;
    gfx::Buffer *             _uniformBuffer       = nullptr;
    gfx::Buffer *             _uniformBufferMVP    = nullptr;
    gfx::DescriptorSet *      _descriptorSet       = nullptr;
    gfx::DescriptorSetLayout *_descriptorSetLayout = nullptr;
    gfx::PipelineLayout *     _pipelineLayout      = nullptr;
    gfx::PipelineState *      _pipelineState       = nullptr;
    gfx::InputAssembler *     _inputAssembler      = nullptr;
    gfx::Buffer *             _indirectBuffer      = nullptr;
    gfx::Buffer *             _indexBuffer         = nullptr;
};

} // namespace cc
