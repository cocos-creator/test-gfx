#pragma once

#include "TestBase.h"
#include "math/Mat4.h"

NS_CC_BEGIN

class BasicTexture: public TestBaseI
{
public:
    DEFINE_CREATE_METHOD(BasicTexture)
    BasicTexture(const WindowInfo& info) : TestBaseI(info) {};
    ~BasicTexture() = default;

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

    GFXShader* _shader = nullptr;
    GFXBuffer* _vertexBuffer = nullptr;
    GFXBuffer* _uniformBuffer = nullptr;
    GFXInputAssembler* _inputAssembler = nullptr;
    GFXBindingLayout* _bindingLayout = nullptr;
    GFXPipelineLayout* _pipelineLayout = nullptr;
    GFXPipelineState* _pipelineState = nullptr;
    GFXTexture* _texture = nullptr;
    GFXBuffer* _image = nullptr;
    GFXSampler* _sampler = nullptr;
    GFXTextureView* _texView = nullptr;
};

NS_CC_END
