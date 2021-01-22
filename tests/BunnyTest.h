#pragma once

#include "TestBase.h"

namespace cc {

class BunnyTest: public TestBaseI
{
public:
    DEFINE_CREATE_METHOD(BunnyTest)
    BunnyTest(const WindowInfo& info) : TestBaseI(info) {};
    ~BunnyTest() = default;

public:
     virtual void tick() override;
     virtual bool initialize() override;
     virtual void destroy() override;

private:
    void createShader();
    void createBuffers();
    void createInputAssembler();
    void createPipelineState();
    
    gfx::Shader* _shader = nullptr;
    gfx::Buffer* _vertexBuffer = nullptr;
    gfx::Buffer* _indexBuffer = nullptr;
    gfx::Buffer* _rootUBO = nullptr;
    vector<float> _rootBuffer;
    gfx::Buffer* _mvpMatrix = nullptr;
    gfx::Buffer* _color = nullptr;
    gfx::DescriptorSet* _descriptorSet = nullptr;
    gfx::DescriptorSetLayout* _descriptorSetLayout = nullptr;
    gfx::PipelineLayout* _pipelineLayout = nullptr;
    gfx::InputAssembler* _inputAssembler = nullptr;
    gfx::PipelineState* _pipelineState = nullptr;
    
    Mat4 _view;
    float _time = 0.0f;
};

} // namespace cc
