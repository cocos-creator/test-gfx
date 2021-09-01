#pragma once

#include "../TestBase.h"

namespace cc {

namespace standard {
constexpr uint CAMERA = 0;
constexpr uint COLOR  = 1;
constexpr uint MVP    = 2;
} // namespace standard

struct StandardUniformBuffers {
    StandardUniformBuffers();
    ~StandardUniformBuffers();
    DISABLE_COPY_SEMANTICS(StandardUniformBuffers)
    ENABLE_MOVE_SEMANTICS(StandardUniformBuffers)

    vector<std::unique_ptr<gfx::Buffer>> bufferViews;

    inline float *getBuffer(uint binding) { return &_rootBuffer[_bufferViewOffsets[binding] / sizeof(float)]; }
    inline void   update(gfx::CommandBuffer *cmdBuff = nullptr) {
        if (cmdBuff) {
            cmdBuff->updateBuffer(_rootUBO.get(), _rootBuffer.data(), _rootBuffer.size() * sizeof(float));
        } else {
            _rootUBO->update(_rootBuffer.data(), _rootBuffer.size() * sizeof(float));
        }
    }

    std::unique_ptr<gfx::DescriptorSet> descriptorSet{nullptr};

    static std::unique_ptr<gfx::DescriptorSetLayout> descriptorSetLayout;
    static std::unique_ptr<gfx::PipelineLayout>      pipelineLayout;

private:
    friend void createStandardUniformBuffers(gfx::Device *device, StandardUniformBuffers *out);

    std::unique_ptr<gfx::Buffer> _rootUBO{nullptr};
    vector<float>                _rootBuffer;
    vector<uint>                 _bufferViewOffsets;

    static uint refCount;
};

struct StandardForwardPipeline {
    std::unique_ptr<gfx::Shader>        shader{nullptr};
    std::unique_ptr<gfx::PipelineState> pipelineState{nullptr};

    void recordCommandBuffer(gfx::Device *device, gfx::CommandBuffer *commandBuffer, gfx::Framebuffer *framebuffer,
                             const gfx::Rect &renderArea, const gfx::Color *clearColors, const std::function<void()> &execute) const;
};

struct StandardDeferredPipeline {
    static constexpr bool USE_FRAMEGRAPH{true};

    std::unique_ptr<gfx::Shader>        gbufferShader{nullptr};
    std::unique_ptr<gfx::RenderPass>    gbufferRenderPass{nullptr};
    std::unique_ptr<gfx::PipelineState> gbufferPipelineState{nullptr};

    vector<std::unique_ptr<gfx::Texture>> gbufferTextures;
    std::unique_ptr<gfx::Texture>         gbufferDepthStencilTexture{nullptr};
    std::unique_ptr<gfx::Framebuffer>     gbufferFramebuffer{nullptr};

    // lighting pass
    std::unique_ptr<gfx::Shader>              lightingShader{nullptr};
    std::unique_ptr<gfx::DescriptorSetLayout> lightingDescriptorSetLayout{nullptr};
    std::unique_ptr<gfx::PipelineLayout>      lightingPipelineLayout{nullptr};
    std::unique_ptr<gfx::PipelineState>       lightingPipelineState{nullptr};
    std::unique_ptr<gfx::Buffer>              lightingVertexBuffer{nullptr};
    std::unique_ptr<gfx::InputAssembler>      lightingInputAssembler{nullptr};
    std::unique_ptr<gfx::DescriptorSet>       lightingDescriptorSet{nullptr};

    void recordCommandBuffer(gfx::Device *device, gfx::Swapchain *swapchain, gfx::CommandBuffer *commandBuffer,
                             const gfx::Rect &renderArea, const gfx::Color *clearColors, const std::function<void()> &execute);

private:
    gfx::DescriptorSet *getDescriptorSet(gfx::Device *device, uint32_t hash);
    gfx::PipelineState *getPipelineState(gfx::Device *device, const gfx::PipelineStateInfo &info);

    std::unordered_map<uint32_t, std::unique_ptr<gfx::DescriptorSet>> _descriptorSetPool;
    std::unordered_map<uint32_t, std::unique_ptr<gfx::PipelineState>> _pipelineStatePool;
};

extern void createStandardShader(gfx::Device *device, StandardForwardPipeline *out);
extern void createStandardShader(gfx::Device *device, StandardDeferredPipeline *out);
extern void createStandardUniformBuffers(gfx::Device *device, StandardUniformBuffers *out);
extern void createStandardPipelineResources(gfx::Device *device, StandardForwardPipeline *out, const StandardUniformBuffers &ubos);
extern void createStandardPipelineResources(gfx::Device *device, StandardDeferredPipeline *out, const StandardUniformBuffers &ubos);

} // namespace cc
