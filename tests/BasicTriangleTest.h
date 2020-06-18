#pragma once

#include "TestBase.h"

namespace cc {

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

     gfx::GFXShader* _shader = nullptr;
     gfx::GFXBuffer* _vertexBuffer = nullptr;
     gfx::GFXBuffer* _uniformBuffer = nullptr;
     gfx::GFXBindingLayout* _bindingLayout = nullptr;
     gfx::GFXPipelineLayout* _pipelineLayout = nullptr;
     gfx::GFXPipelineState* _pipelineState = nullptr;
     gfx::GFXInputAssembler* _inputAssembler = nullptr;
    gfx::GFXBuffer *_indirectBuffer = nullptr;
    gfx::GFXBuffer *_indexBuffer = nullptr;

     float _time = 0.0f;
};

} // namespace cc
