#pragma once

#include "TestBase.h"

namespace cc {

class SubpassTest : public TestBaseI {
public:
    DEFINE_CREATE_METHOD(SubpassTest)
    explicit SubpassTest(const WindowInfo &info) : TestBaseI(info){};
    ~SubpassTest() override = default;

    bool onInit() override;
    void onTick() override;

protected:
    void onSpacePressed() override;

    void createShader();
    void createDeferredResources();
    void createBuffers();
    void createInputAssembler();
    void createPipelineState();

    std::unique_ptr<gfx::Shader>              _shaderForward        = nullptr;
    std::unique_ptr<gfx::Shader>              _shaderGBuffer        = nullptr;
    std::unique_ptr<gfx::Buffer>              _vertexPositionBuffer = nullptr;
    std::unique_ptr<gfx::Buffer>              _vertexNormalBuffer   = nullptr;
    std::unique_ptr<gfx::Buffer>              _indexBuffer          = nullptr;
    std::unique_ptr<gfx::DescriptorSet>       _descriptorSet        = nullptr;
    std::unique_ptr<gfx::DescriptorSetLayout> _descriptorSetLayout  = nullptr;
    std::unique_ptr<gfx::PipelineLayout>      _pipelineLayout       = nullptr;
    std::unique_ptr<gfx::InputAssembler>      _inputAssembler       = nullptr;
    std::unique_ptr<gfx::PipelineState>       _pipelineState        = nullptr;

    std::unique_ptr<gfx::Buffer>         _rootUBO = nullptr;
    vector<std::unique_ptr<gfx::Buffer>> _bufferViews;
    vector<uint>                         _bufferViewOffsets;
    vector<float>                        _rootBuffer;

    Mat4 _worldMatrix;
    Mat4 _projectionMatrix;

    std::unique_ptr<gfx::Sampler> _sampler = nullptr;

    vector<std::unique_ptr<gfx::Texture>> _deferredGBuffers;
    std::unique_ptr<gfx::Texture>         _deferredGBufferDepthTexture  = nullptr;
    std::unique_ptr<gfx::RenderPass>      _deferredGBufferRenderPass    = nullptr;
    std::unique_ptr<gfx::Framebuffer>     _deferredGBufferFramebuffer   = nullptr;
    std::unique_ptr<gfx::PipelineState>   _deferredGBufferPipelineState = nullptr;

    std::unique_ptr<gfx::Shader>              _deferredShader              = nullptr;
    std::unique_ptr<gfx::Buffer>              _deferredVB                  = nullptr;
    std::unique_ptr<gfx::InputAssembler>      _deferredInputAssembler      = nullptr;
    std::unique_ptr<gfx::DescriptorSetLayout> _deferredDescriptorSetLayout = nullptr;
    std::unique_ptr<gfx::PipelineLayout>      _deferredPipelineLayout      = nullptr;
    std::unique_ptr<gfx::PipelineState>       _deferredPipelineState       = nullptr;
    std::unique_ptr<gfx::DescriptorSet>       _deferredDescriptorSet       = nullptr;

    gfx::ColorList _clearColors;

    bool _useDeferred = true;
};

} // namespace cc
