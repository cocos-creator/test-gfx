#pragma once

#include "TestBase.h"

namespace cc {

class ParticleTest: public TestBaseI
{
public:
    DEFINE_CREATE_METHOD(ParticleTest)
    ParticleTest(const WindowInfo& info) : TestBaseI(info) {};
    ~ParticleTest() = default;

public:
     virtual void tick(float dt) override;
     virtual bool initialize() override;
     virtual void destroy() override;

private:
    void createShader();
    void createVertexBuffer();
    void createPipeline();
    void createInputAssembler();
    void createTexture();

    gfx::GFXShader* _shader = nullptr;
    gfx::GFXBuffer* _vertexBuffer = nullptr;
    gfx::GFXBuffer* _indexBuffer = nullptr;
    gfx::GFXBuffer* _uniformBuffer = nullptr;
    gfx::GFXPipelineState* _pipelineState = nullptr;
    gfx::GFXPipelineLayout* _pipelineLayout = nullptr;
    gfx::GFXInputAssembler* _inputAssembler = nullptr;
    gfx::GFXBindingLayout* _bindingLayout = nullptr;
    gfx::GFXTexture* _texture = nullptr;
    gfx::GFXSampler* _sampler = nullptr;
        
#define MAX_QUAD_COUNT 1024
#define VERTEX_STRIDE 9
#define PARTICLE_COUNT 100
    float _vbufferArray[MAX_QUAD_COUNT][4][VERTEX_STRIDE];
    uint16_t _ibufferArray[MAX_QUAD_COUNT][6];
    
    struct ParticleData
    {
        Vec3 position;
        Vec3 velocity;
        float age;
        float life;
    };
    
    ParticleData _particles[PARTICLE_COUNT];
};

} // namespace cc
