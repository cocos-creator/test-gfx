#pragma once

#include "TestBase.h"
#include "gfx-base/GFXCommandBuffer.h"

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
    inline void   update() { _rootUBO->update(_rootBuffer.data(), _rootBuffer.size() * sizeof(float)); }

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
    std::unique_ptr<gfx::RenderPass>    renderPass{nullptr};
    std::unique_ptr<gfx::PipelineState> pipelineState{nullptr};
    std::unique_ptr<gfx::Framebuffer>   framebuffer{nullptr};

    template <typename Fn>
    void recordCommandBuffer(gfx::Device *device, gfx::CommandBuffer *commandBuffer,
                             const gfx::Rect &renderArea, const gfx::Color *clearColors, Fn execute);
};

struct StandardDeferredPipeline {
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
    std::unique_ptr<gfx::Sampler>             sampler{nullptr};

    template <typename Fn>
    void recordCommandBuffer(gfx::Device *device, gfx::CommandBuffer *commandBuffer,
                             const gfx::Rect &renderArea, const gfx::Color *clearColors, Fn execute);

    vector<gfx::TextureBarrier *> textureBarriers;
};

extern void createStandardShader(gfx::Device *device, StandardForwardPipeline *out);
extern void createStandardShader(gfx::Device *device, StandardDeferredPipeline *out);
extern void createStandardUniformBuffers(gfx::Device *device, StandardUniformBuffers *out);
extern void createStandardPipelineResources(gfx::Device *device, StandardForwardPipeline *out, const StandardUniformBuffers &ubos);
extern void createStandardPipelineResources(gfx::Device *device, StandardDeferredPipeline *out, const StandardUniformBuffers &ubos);

///////////////////////////////////////////////////////////////////////////////

template <typename Fn>
void StandardForwardPipeline::recordCommandBuffer(gfx::Device * /*device*/, gfx::CommandBuffer *commandBuffer,
                                                  const gfx::Rect &renderArea, const gfx::Color *clearColors, Fn execute) {
    commandBuffer->beginRenderPass(renderPass.get(), framebuffer.get(), renderArea, clearColors, 1.0F, 0);
    commandBuffer->bindPipelineState(pipelineState.get());

    execute();

    commandBuffer->endRenderPass();
}

template <typename Fn>
void StandardDeferredPipeline::recordCommandBuffer(gfx::Device *device, gfx::CommandBuffer *commandBuffer,
                                                   const gfx::Rect &renderArea, const gfx::Color *clearColors, Fn execute) {
    static gfx::Texture *backBuffer{nullptr};

    commandBuffer->beginRenderPass(gbufferRenderPass.get(), gbufferFramebuffer.get(), renderArea, clearColors, 1.0F, 0);
    commandBuffer->bindPipelineState(gbufferPipelineState.get());

    execute();

    commandBuffer->nextSubpass();

    commandBuffer->bindInputAssembler(lightingInputAssembler.get());
    commandBuffer->bindPipelineState(lightingPipelineState.get());
    commandBuffer->bindDescriptorSet(0, lightingDescriptorSet.get());
    commandBuffer->draw(lightingInputAssembler.get());

    commandBuffer->endRenderPass();

    commandBuffer->pipelineBarrier(nullptr, &textureBarriers[0], &backBuffer, 1);

    gfx::TextureBlit region;
    region.srcExtent.width  = device->getWidth();
    region.srcExtent.height = device->getHeight();
    region.dstExtent.width  = device->getWidth();
    region.dstExtent.height = device->getHeight();
    commandBuffer->blitTexture(gbufferTextures[3].get(), nullptr, &region, 1, gfx::Filter::POINT);

    commandBuffer->pipelineBarrier(nullptr, &textureBarriers[1], &backBuffer, 1);
}

} // namespace cc
