#pragma once

#include "TestBase.h"
#include "math/Mat4.h"

NS_CC_BEGIN

class StencilTest: public TestBaseI
{
public:
    DEFINE_CREATE_METHOD(StencilTest)
    StencilTest(const WindowInfo& info) : TestBaseI(info) {};
    ~StencilTest() = default;

public:
     virtual void tick(float dt) override;
     virtual bool initialize() override;
     virtual void Destroy() override;

private:
    void createShader();
    void createBuffers();
    void createTextures();
    void createInputAssembler();
    void createPipelineState();
    
    const static uint BINDING_COUNT = 2;
    const static uint PIPELIE_COUNT = 6;
    GFXShader* _shader = nullptr;
    GFXTexture* _labelTexture = nullptr;
    GFXTexture* _uvCheckerTexture = nullptr;
    GFXTextureView* _labelTexView = nullptr;
    GFXTextureView* _uvCheckerTexView = nullptr;
    GFXBuffer* _vertexBuffer = nullptr;
    GFXInputAssembler* _inputAssembler = nullptr;
    GFXBuffer* _uniformBuffer[BINDING_COUNT] = { nullptr };
    GFXBindingLayout* _bindingLayout[BINDING_COUNT] = { nullptr };
    GFXPipelineState* _pipelineState[PIPELIE_COUNT] = { nullptr };
    GFXSampler* _sampler = nullptr;
    
    float _dt = 0.0f;
};

NS_CC_END
