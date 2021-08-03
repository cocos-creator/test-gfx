#pragma once

#include "TestBase.h"

namespace cc {

class ParticleTest : public TestBaseI {
public:
    DEFINE_CREATE_METHOD(ParticleTest)
    using TestBaseI::TestBaseI;

    bool onInit() override;
    void onTick() override;
    void onDestroy() override;

private:
    void createShader();
    void createVertexBuffer();
    void createPipeline();
    void createInputAssembler();
    void createTexture();

    gfx::Shader *             _shader              = nullptr;
    gfx::Buffer *             _vertexBuffer        = nullptr;
    gfx::Buffer *             _indexBuffer         = nullptr;
    gfx::Buffer *             _uniformBuffer       = nullptr;
    gfx::PipelineState *      _pipelineState       = nullptr;
    gfx::InputAssembler *     _inputAssembler      = nullptr;
    gfx::DescriptorSet *      _descriptorSet       = nullptr;
    gfx::DescriptorSetLayout *_descriptorSetLayout = nullptr;
    gfx::PipelineLayout *     _pipelineLayout      = nullptr;

#define MAX_QUAD_COUNT 1024
#define VERTEX_STRIDE  9
#define PARTICLE_COUNT 100
    float    _vbufferArray[MAX_QUAD_COUNT][4][VERTEX_STRIDE];
    uint16_t _ibufferArray[MAX_QUAD_COUNT][6];

    struct ParticleData {
        Vec3  position;
        Vec3  velocity;
        float age;
        float life;
    };

    Mat4         _matrices[3];
    ParticleData _particles[PARTICLE_COUNT];
};

} // namespace cc
