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
    CC_DISABLE_COPY_AND_MOVE_SEMANTICS(StandardUniformBuffers)

    vector<std::unique_ptr<gfx::Buffer>> bufferViews;

    float *getBuffer(uint binding, uint instance = 0);
    void   update(gfx::CommandBuffer *cmdBuff = nullptr);
    void   bindDescriptorSet(gfx::CommandBuffer *cmdBuff, uint set, uint instance = 0);

    std::unique_ptr<gfx::DescriptorSet> descriptorSet{nullptr};

    static std::unique_ptr<gfx::DescriptorSetLayout> descriptorSetLayout;
    static std::unique_ptr<gfx::PipelineLayout>      pipelineLayout;

private:
    friend void createStandardUniformBuffers(gfx::Device *device, StandardUniformBuffers *out, uint instances);

    std::unique_ptr<gfx::Buffer> _rootUBO{nullptr};
    vector<float>                _rootBuffer;
    vector<uint>                 _bufferViewOffsets;
    vector<uint>                 _alignedBufferSizes;
    vector<uint>                 _dynamicOffsets;
    uint                         _instances{0U};

    static uint refCount;
};

struct StandardForwardPipeline {
    std::unique_ptr<gfx::Shader>        shader{nullptr};
    std::unique_ptr<gfx::PipelineState> pipelineState{nullptr};

    void recordCommandBuffer(gfx::Device *device, gfx::CommandBuffer *commandBuffer, gfx::Framebuffer *framebuffer,
                             const gfx::Rect &renderArea, const gfx::Color &clearColor, const std::function<void()> &execute) const;
};

struct StandardDeferredPipeline {
    static constexpr bool USE_SUBPASS{true};

    std::unique_ptr<gfx::Shader> gbufferShader{nullptr};
    std::unique_ptr<gfx::Shader> lightingShader{nullptr};

    std::unique_ptr<gfx::DescriptorSetLayout> lightingDescriptorSetLayout{nullptr};
    std::unique_ptr<gfx::PipelineLayout>      lightingPipelineLayout{nullptr};
    std::unique_ptr<gfx::Buffer>              lightingVertexBuffer{nullptr};
    std::unique_ptr<gfx::InputAssembler>      lightingInputAssembler{nullptr};
    std::unique_ptr<gfx::DescriptorSet>       lightingDescriptorSet{nullptr};

    void recordCommandBuffer(gfx::Device *device, gfx::CommandBuffer *commandBuffer, gfx::Framebuffer *framebuffer,
                             const gfx::Rect &renderArea, const gfx::Color &clearColor, const std::function<void()> &execute);

private:
    gfx::PipelineState *getPipelineState(gfx::Device *device, const gfx::PipelineStateInfo &info);
    gfx::DescriptorSet *getDescriptorSet(gfx::Device *device, uint32_t hash);

    std::unordered_map<uint32_t, std::unique_ptr<gfx::PipelineState>> _pipelineStatePool;
    std::unordered_map<uint32_t, std::unique_ptr<gfx::DescriptorSet>> _descriptorSetPool;
};

extern void createStandardShader(gfx::Device *device, StandardForwardPipeline *out);
extern void createStandardShader(gfx::Device *device, StandardDeferredPipeline *out);
extern void createStandardUniformBuffers(gfx::Device *device, StandardUniformBuffers *out, uint instances);
extern void createStandardPipelineResources(gfx::Device *device, StandardForwardPipeline *out, const StandardUniformBuffers &ubos);
extern void createStandardPipelineResources(gfx::Device *device, StandardDeferredPipeline *out, const StandardUniformBuffers &ubos);

} // namespace cc
