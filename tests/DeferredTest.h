#pragma once

#include "TestBase.h"

namespace cc {

class DeferredTest : public TestBaseI {
public:
    DEFINE_CREATE_METHOD(DeferredTest)
    using TestBaseI::TestBaseI;

    bool onInit() override;
    void onTick() override;
    void onDestroy() override;

protected:
    void onSpacePressed() override;

    void createShader();
    void createDeferredResources();
    void createBuffers();
    void createInputAssembler();
    void createPipelineState();

    gfx::Shader *             _shaderForward        = nullptr;
    gfx::Shader *             _shaderGBuffer        = nullptr;
    gfx::Buffer *             _vertexPositionBuffer = nullptr;
    gfx::Buffer *             _vertexNormalBuffer   = nullptr;
    gfx::Buffer *             _indexBuffer          = nullptr;
    gfx::DescriptorSet *      _descriptorSet        = nullptr;
    gfx::DescriptorSetLayout *_descriptorSetLayout  = nullptr;
    gfx::PipelineLayout *     _pipelineLayout       = nullptr;
    gfx::InputAssembler *     _inputAssembler       = nullptr;
    gfx::PipelineState *      _pipelineState        = nullptr;

    gfx::Buffer *         _rootUBO = nullptr;
    vector<gfx::Buffer *> _bufferViews;
    vector<uint>          _bufferViewOffsets;
    vector<float>         _rootBuffer;

    Mat4 _worldMatrix;
    Mat4 _projectionMatrix;

    gfx::Sampler *_sampler = nullptr;

    vector<gfx::Texture *> _deferredGBuffers;
    gfx::Texture *         _deferredGBufferDepthTexture  = nullptr;
    gfx::RenderPass *      _deferredGBufferRenderPass    = nullptr;
    gfx::Framebuffer *     _deferredGBufferFramebuffer   = nullptr;
    gfx::PipelineState *   _deferredGBufferPipelineState = nullptr;

    gfx::Shader *             _deferredShader              = nullptr;
    gfx::Buffer *             _deferredVB                  = nullptr;
    gfx::InputAssembler *     _deferredInputAssembler      = nullptr;
    gfx::DescriptorSetLayout *_deferredDescriptorSetLayout = nullptr;
    gfx::PipelineLayout *     _deferredPipelineLayout      = nullptr;
    gfx::RenderPass *         _deferredRenderPass          = nullptr;
    gfx::Framebuffer *        _deferredFramebuffer         = nullptr;
    gfx::PipelineState *      _deferredPipelineState       = nullptr;
    gfx::DescriptorSet *      _deferredDescriptorSet       = nullptr;

    gfx::ColorList _clearColors;

    bool _useDeferred = true;
};

} // namespace cc
