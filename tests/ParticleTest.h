#pragma once

#include "TestBase.h"
#include "math/Mat4.h"

NS_CC_BEGIN

class ParticleTest: public TestBaseI
{
public:
    DEFINE_CREATE_METHOD(ParticleTest)
    ParticleTest(const WindowInfo& info) : TestBaseI(info) {};
    ~ParticleTest() = default;

public:
     virtual void tick(float dt) override;
     virtual bool initialize() override;
     virtual void Destroy() override;

private:
    void createShader();
    void createVertexBuffer();
    void createPipeline();
    void createInputAssembler();
    void createTexture();

    GFXShader* _shader = nullptr;
    GFXBuffer* _vertexBuffer = nullptr;
    GFXBuffer* _indexBuffer = nullptr;
    GFXBuffer* _uniformBuffer = nullptr;
    GFXPipelineState* _pipelineState = nullptr;
    GFXInputAssembler* _inputAssembler = nullptr;
    GFXBindingLayout* _bindingLayout = nullptr;
    GFXTexture* _texture = nullptr;
    GFXSampler* _sampler = nullptr;
    GFXTextureView* _texView = nullptr;
        
#define MAX_QUAD_COUNT 1024
#define VERTEX_STRIDE 9
#define PARTICLE_COUNT 100
    float _vbufferArray[MAX_QUAD_COUNT][4][VERTEX_STRIDE];
    uint16_t _ibufferArray[MAX_QUAD_COUNT][6];
    
    struct ParticleData
    {
        cocos2d::Vec3 position;
        cocos2d::Vec3 velocity;
        float age;
        float life;
    };
    
    ParticleData _particles[PARTICLE_COUNT];
};

NS_CC_END
