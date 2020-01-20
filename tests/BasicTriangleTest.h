#pragma once

#include "TestBase.h"

#if (CC_PLATFORM == CC_PLATFORM_MAC_OSX)
#include "MTLStd.h"
#else
#include "GLES2Std.h"
#endif

NS_CC_BEGIN

typedef struct Vertex
{
    float pos[2];
    float col[3];
} Vertex;

class BasicTriangle: public TestBaseI
{
 public:
    DEFINE_CREATE_METHOD(BasicTriangle)
    BasicTriangle(const WindowInfo& info) : TestBaseI(info) {};
    ~BasicTriangle() = default;

 public:
     virtual void tick(float dt) override;
     virtual bool initialize() override;
     virtual void destroy() override;

 private:
     void createShader();
     void createVertexBuffer();
     void createPipeline();
     void createInputAssembler();

     GFXShader* _shader = nullptr;
     GFXBuffer* _vertexBuffer = nullptr;
     GFXBuffer* _uniformBuffer = nullptr;
     GFXPipelineState* _pipelineState = nullptr;
     GFXInputAssembler* _inputAssembler = nullptr;
     GFXBindingLayout* _bindingLayout = nullptr;

     float _time = 0.0f;
};

NS_CC_END
