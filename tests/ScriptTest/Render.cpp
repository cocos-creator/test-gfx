#include "Chassis.h"

#include "../TestBase.h"

// fix-functioned pipeline just for test purposes

namespace cc {
namespace experimental {

namespace {
struct MultisampledFramebuffer {
    MultisampledFramebuffer(gfx::Device *device, gfx::Swapchain *swapchain) {
        gfx::RenderPassInfo renderPassInfo;

        gfx::ColorAttachment &colorAttachment{renderPassInfo.colorAttachments.emplace_back()};
        colorAttachment.format      = device->getColorFormat();
        colorAttachment.sampleCount = gfx::SampleCount::MULTIPLE;
        colorAttachment.storeOp     = gfx::StoreOp::DISCARD;
        colorAttachment.endAccesses = {gfx::AccessType::COLOR_ATTACHMENT_WRITE};

        gfx::ColorAttachment &colorResolveAttachment{renderPassInfo.colorAttachments.emplace_back()};
        colorResolveAttachment.format      = device->getColorFormat();
        colorResolveAttachment.loadOp      = gfx::LoadOp::DISCARD;
        colorResolveAttachment.endAccesses = {gfx::AccessType::TRANSFER_READ};

        gfx::DepthStencilAttachment &depthStencilAttachment{renderPassInfo.depthStencilAttachment};
        depthStencilAttachment.format         = device->getDepthStencilFormat();
        depthStencilAttachment.sampleCount    = gfx::SampleCount::MULTIPLE;
        depthStencilAttachment.depthStoreOp   = gfx::StoreOp::DISCARD;
        depthStencilAttachment.stencilStoreOp = gfx::StoreOp::DISCARD;

        gfx::SubpassInfo &subpass{renderPassInfo.subpasses.emplace_back()};
        subpass.colors.push_back(0);
        subpass.resolves.push_back(1);
        subpass.depthStencil = 2;

        renderPass = device->createRenderPass(renderPassInfo);

        gfx::TextureInfo colorTexMSAAInfo;
        colorTexMSAAInfo.type    = gfx::TextureType::TEX2D;
        colorTexMSAAInfo.usage   = gfx::TextureUsageBit::COLOR_ATTACHMENT;
        colorTexMSAAInfo.samples = gfx::SampleCount::MULTIPLE;
        colorTexMSAAInfo.format  = swapchain->getColorTexture()->getFormat();
        colorTexMSAAInfo.width   = swapchain->getWidth();
        colorTexMSAAInfo.height  = swapchain->getHeight();
        colorTexMSAA             = device->createTexture(colorTexMSAAInfo);

        gfx::TextureInfo colorTexInfo;
        colorTexInfo.type   = gfx::TextureType::TEX2D;
        colorTexInfo.usage  = gfx::TextureUsageBit::COLOR_ATTACHMENT | gfx::TextureUsageBit::TRANSFER_SRC;
        colorTexInfo.format = swapchain->getColorTexture()->getFormat();
        colorTexInfo.width  = swapchain->getWidth();
        colorTexInfo.height = swapchain->getHeight();
        colorTex            = device->createTexture(colorTexInfo);

        gfx::TextureInfo depthStencilTexInfo;
        depthStencilTexInfo.type    = gfx::TextureType::TEX2D;
        depthStencilTexInfo.usage   = gfx::TextureUsageBit::DEPTH_STENCIL_ATTACHMENT;
        depthStencilTexInfo.samples = gfx::SampleCount::MULTIPLE;
        depthStencilTexInfo.format  = swapchain->getDepthStencilTexture()->getFormat();
        depthStencilTexInfo.width   = swapchain->getWidth();
        depthStencilTexInfo.height  = swapchain->getHeight();
        depthStencilTex             = device->createTexture(depthStencilTexInfo);

        gfx::FramebufferInfo fboInfo;
        fboInfo.renderPass = renderPass;
        fboInfo.colorTextures.push_back(colorTexMSAA);
        fboInfo.colorTextures.push_back(colorTex);
        fboInfo.depthStencilTexture = depthStencilTex;
        framebuffer                 = device->createFramebuffer(fboInfo);
    }

    void resize(uint width, uint height) const {
        framebuffer->destroy();

        colorTexMSAA->resize(width, height);
        colorTex->resize(width, height);
        depthStencilTex->resize(width, height);

        gfx::FramebufferInfo fboInfo;
        fboInfo.renderPass = renderPass;
        fboInfo.colorTextures.push_back(colorTexMSAA);
        fboInfo.colorTextures.push_back(nullptr);
        fboInfo.depthStencilTexture = depthStencilTex;
        framebuffer->initialize(fboInfo);
    }

    void destroy() {
        CC_SAFE_DESTROY(framebuffer);
        CC_SAFE_DESTROY(depthStencilTex);
        CC_SAFE_DESTROY(colorTex);
        CC_SAFE_DESTROY(colorTexMSAA);
        CC_SAFE_DESTROY(renderPass);
    }

    gfx::RenderPass * renderPass{nullptr};
    gfx::Texture *    colorTexMSAA{nullptr};
    gfx::Texture *    colorTex{nullptr};
    gfx::Texture *    depthStencilTex{nullptr};
    gfx::Framebuffer *framebuffer{nullptr};
};

gfx::Framebuffer *fbo;

MultisampledFramebuffer *    msaaFBO;
gfx::Framebuffer *           backBufferFBO;
gfx::RenderPass *            backBufferRenderPass;
vector<gfx::CommandBuffer *> commandBuffers;

gfx::Shader *                shader{nullptr};
gfx::Buffer *                vertexBuffer{nullptr};
gfx::Buffer *                instancedBuffer{nullptr};
gfx::Buffer *                uniformBufferGlobal{nullptr};
gfx::DescriptorSet *         descriptorSet{nullptr};
gfx::DescriptorSetLayout *   descriptorSetLayout{nullptr};
gfx::PipelineLayout *        pipelineLayout{nullptr};
gfx::PipelineState *         pipelineState{nullptr};
gfx::InputAssembler *        inputAssembler{nullptr};
vector<gfx::GlobalBarrier *> globalBarriers;
vector<float>                uniformBufferData;
uint                         uboStride{0U};
uint                         floatCountPerModel{0U};
uint                         modelCapacity{Root::INITIAL_CAPACITY};
vmath::IndexP                index;
constexpr bool               ROTATE_VIEW{true};
constexpr bool               OFFSCREEN_MSAA{true};

gfx::Buffer *        vertexBufferOutline{nullptr};
gfx::PipelineState * pipelineStateOutline{nullptr};
gfx::InputAssembler *inputAssemblerOutline{nullptr};
} // namespace

void Root::initialize() {
    gfx::Device *device = gfx::Device::getInstance();

    gfx::RenderPassInfo renderPassInfo;
    renderPassInfo.colorAttachments.emplace_back().format = device->getColorFormat();
    renderPassInfo.depthStencilAttachment.format          = device->getDepthStencilFormat();
    backBufferRenderPass                                  = device->createRenderPass(renderPassInfo);

    gfx::FramebufferInfo fboInfo;
    fboInfo.colorTextures.resize(1);
    fboInfo.renderPass = backBufferRenderPass;
    backBufferFBO      = device->createFramebuffer(fboInfo);

    msaaFBO = CC_NEW(MultisampledFramebuffer(device, TestBaseI::swapchain));

    fbo = OFFSCREEN_MSAA ? msaaFBO->framebuffer : backBufferFBO;

    commandBuffers.push_back(device->getCommandBuffer());

    ShaderSources<StandardShaderSource> sources;
    sources.glsl4 = {
        R"(
            precision highp float;
            layout(location = 0) in vec3 a_position;
            layout(location = 1) in vec4 a_color;
            layout(location = 2) in vec4 a_worldView0;
            layout(location = 3) in vec4 a_worldView1;
            layout(location = 4) in vec4 a_worldView2;
            layout(location = 5) in vec4 a_worldView3;

            mat4 getWorldViewMatrix () {
                return mat4(
                    a_worldView0,
                    a_worldView1,
                    a_worldView2,
                    a_worldView3
                );
            }

            layout(set = 0, binding = 0) uniform Global {
                mat4 u_project;
            };

            layout(location = 0) out vec4 v_color;

            void main () {
                v_color = a_color;
                gl_Position = u_project * getWorldViewMatrix() * vec4(a_position, 1.0);
            }
        )",
        R"(
            precision highp float;
            layout(location = 0) in vec4 v_color;
            layout(location = 0) out vec4 o_color;

            void main () {
                o_color = v_color;
            }
        )",
    };

    sources.glsl3 = {
        R"(
            in vec3 a_position;
            in vec4 a_color;
            in vec4 a_worldView0;
            in vec4 a_worldView1;
            in vec4 a_worldView2;
            in vec4 a_worldView3;

            mat4 getWorldViewMatrix () {
                return mat4(
                    a_worldView0,
                    a_worldView1,
                    a_worldView2,
                    a_worldView3
                );
            }

            layout(std140) uniform Global {
                mat4 u_project;
            };

            out vec4 v_color;

            void main () {
                v_color = a_color;
                gl_Position = u_project * getWorldViewMatrix() * vec4(a_position, 1.0);
            }
        )",
        R"(
            precision mediump float;
            in vec4 v_color;
            out vec4 o_color;

            void main () {
                o_color = v_color;
            }
        )",
    };

    sources.glsl1 = {
        R"(
            attribute vec3 a_position;
            attribute vec4 a_color;
            attribute vec4 a_worldView0;
            attribute vec4 a_worldView1;
            attribute vec4 a_worldView2;
            attribute vec4 a_worldView3;

            mat4 getWorldViewMatrix () {
                return mat4(
                    a_worldView0,
                    a_worldView1,
                    a_worldView2,
                    a_worldView3
                );
            }

            uniform mat4 u_project;

            varying vec4 v_color;

            void main () {
                v_color = a_color;
                gl_Position = u_project * getWorldViewMatrix() * vec4(a_position, 1.0);
            }
        )",
        R"(
            precision mediump float;
            varying vec4 v_color;

            void main() {
                gl_FragColor = v_color;
            }
        )",
    };

    StandardShaderSource &source = TestBaseI::getAppropriateShaderSource(sources);

    gfx::ShaderStageList shaderStageList;
    gfx::ShaderStage     vertexShaderStage;
    vertexShaderStage.stage  = gfx::ShaderStageFlagBit::VERTEX;
    vertexShaderStage.source = source.vert;
    shaderStageList.emplace_back(std::move(vertexShaderStage));

    gfx::ShaderStage fragmentShaderStage;
    fragmentShaderStage.stage  = gfx::ShaderStageFlagBit::FRAGMENT;
    fragmentShaderStage.source = source.frag;
    shaderStageList.emplace_back(std::move(fragmentShaderStage));

    gfx::UniformBlockList uniformBlockList = {
        {
            0,
            0,
            "Global",
            {
                {"u_project", gfx::Type::MAT4, 1},
            },
            1,
        },
    };
    gfx::AttributeList attributeList = {
        {"a_position", gfx::Format::RGB32F, false, 0, false, 0},
        {"a_color", gfx::Format::RGBA32F, false, 1, true, 1},
        {"a_worldView0", gfx::Format::RGBA32F, false, 1, true, 2},
        {"a_worldView1", gfx::Format::RGBA32F, false, 1, true, 3},
        {"a_worldView2", gfx::Format::RGBA32F, false, 1, true, 4},
        {"a_worldView3", gfx::Format::RGBA32F, false, 1, true, 5},
    };

    gfx::ShaderInfo shaderInfo;
    shaderInfo.name       = "Basic Triangle";
    shaderInfo.stages     = std::move(shaderStageList);
    shaderInfo.attributes = std::move(attributeList);
    shaderInfo.blocks     = std::move(uniformBlockList);
    shader                = device->createShader(shaderInfo);

    float vertexData[] = {-0.02F, -0.04F, 0.F,
                          0.02F, -0.04F, 0.F,
                          0.0F, 0.04F, 0.F};

    gfx::BufferInfo vertexBufferInfo = {
        gfx::BufferUsage::VERTEX,
        gfx::MemoryUsage::DEVICE,
        sizeof(vertexData),
        3 * sizeof(float),
    };

    vertexBuffer = device->createBuffer(vertexBufferInfo);
    vertexBuffer->update(vertexData, sizeof(vertexData));

    floatCountPerModel = 4 + 16;
    uboStride          = sizeof(float) * floatCountPerModel;

    uniformBufferData.resize(floatCountPerModel * (modelCapacity + 1));

    gfx::BufferInfo instancedBufferInfo = {
        gfx::BufferUsage::VERTEX,
        gfx::MemoryUsage::DEVICE,
        uboStride * (modelCapacity + 1),
        uboStride,
    };
    instancedBuffer = device->createBuffer(instancedBufferInfo);

    gfx::BufferInfo uniformBufferGlobalInfo = {
        gfx::BufferUsage::UNIFORM,
        gfx::MemoryUsage::DEVICE | gfx::MemoryUsage::HOST,
        sizeof(float) * 16,
    };
    uniformBufferGlobal = device->createBuffer(uniformBufferGlobalInfo);

    Mat4 view;
    Mat4 projection;
    Mat4::createLookAt({1.F, 1.F, 3.F}, {0.F, 0.F, .5F}, {0.F, 1.F, 0.F}, &view);
    TestBaseI::createOrthographic(-1.5F, 1.5F, -1.5F, 1.5F, 1.F, 10.F, &projection);
    projection = projection * view;
    uniformBufferGlobal->update(projection.m, sizeof(projection));

    gfx::InputAssemblerInfo inputAssemblerInfo;
    inputAssemblerInfo.attributes = shaderInfo.attributes;
    inputAssemblerInfo.vertexBuffers.emplace_back(vertexBuffer);
    inputAssemblerInfo.vertexBuffers.emplace_back(instancedBuffer);
    inputAssembler = device->createInputAssembler(inputAssemblerInfo);
    inputAssembler->setFirstInstance(1);

    gfx::DescriptorSetLayoutInfo dslInfo;
    dslInfo.bindings.push_back({
        0,
        gfx::DescriptorType::UNIFORM_BUFFER,
        1,
        gfx::ShaderStageFlagBit::VERTEX,
    });
    descriptorSetLayout = device->createDescriptorSetLayout(dslInfo);

    pipelineLayout = device->createPipelineLayout({{descriptorSetLayout}});

    descriptorSet = device->createDescriptorSet({descriptorSetLayout});

    descriptorSet->bindBuffer(0, uniformBufferGlobal);
    descriptorSet->update();

    gfx::PipelineStateInfo pipelineInfo;
    pipelineInfo.primitive                           = gfx::PrimitiveMode::TRIANGLE_LIST;
    pipelineInfo.shader                              = shader;
    pipelineInfo.inputState                          = {inputAssembler->getAttributes()};
    pipelineInfo.renderPass                          = fbo->getRenderPass();
    pipelineInfo.rasterizerState.cullMode            = gfx::CullMode::NONE;
    pipelineInfo.depthStencilState.depthWrite        = false;
    pipelineInfo.blendState.targets[0].blend         = true;
    pipelineInfo.blendState.targets[0].blendSrc      = gfx::BlendFactor::SRC_ALPHA;
    pipelineInfo.blendState.targets[0].blendDst      = gfx::BlendFactor::ONE_MINUS_SRC_ALPHA;
    pipelineInfo.blendState.targets[0].blendSrcAlpha = gfx::BlendFactor::ONE;
    pipelineInfo.blendState.targets[0].blendDstAlpha = gfx::BlendFactor::ONE_MINUS_SRC_ALPHA;
    pipelineInfo.pipelineLayout                      = pipelineLayout;

    pipelineState = device->createPipelineState(pipelineInfo);

    globalBarriers.push_back(TestBaseI::getGlobalBarrier({
        {
            gfx::AccessType::TRANSFER_WRITE,
        },
        {
            gfx::AccessType::VERTEX_SHADER_READ_UNIFORM_BUFFER,
            gfx::AccessType::FRAGMENT_SHADER_READ_UNIFORM_BUFFER,
            gfx::AccessType::VERTEX_BUFFER,
        },
    }));

    auto max = static_cast<int>((vmath::IndexP::Size - 1) * floatCountPerModel);
    index    = vmath::linspace<vmath::IndexP>(0, max);

    // bounding box outline

    float vertexDataOutline[] = {-1.F, -1.F, 0.F,
                                 -1.F, 1.F, 0.F,
                                 -1.F, 1.F, 0.F,
                                 1.F, 1.F, 0.F,
                                 1.F, 1.F, 0.F,
                                 1.F, -1.F, 0.F,
                                 1.F, -1.F, 0.F,
                                 -1.F, -1.F, 0.F,

                                 -1.F, -1.F, 1.F,
                                 -1.F, 1.F, 1.F,
                                 -1.F, 1.F, 1.F,
                                 1.F, 1.F, 1.F,
                                 1.F, 1.F, 1.F,
                                 1.F, -1.F, 1.F,
                                 1.F, -1.F, 1.F,
                                 -1.F, -1.F, 1.F,

                                 -1.F, -1.F, 1.F,
                                 -1.F, -1.F, 0.F,
                                 -1.F, 1.F, 1.F,
                                 -1.F, 1.F, 0.F,
                                 1.F, 1.F, 1.F,
                                 1.F, 1.F, 0.F,
                                 1.F, -1.F, 1.F,
                                 1.F, -1.F, 0.F};

    gfx::BufferInfo vertexBufferOutlineInfo = {
        gfx::BufferUsage::VERTEX,
        gfx::MemoryUsage::DEVICE,
        sizeof(vertexDataOutline),
        3 * sizeof(float),
    };

    vertexBufferOutline = device->createBuffer(vertexBufferOutlineInfo);
    vertexBufferOutline->update(vertexDataOutline, sizeof(vertexDataOutline));

    gfx::Color outlineColor{1.F, 1.F, 1.F, 1.F};
    memcpy(uniformBufferData.data(), &outlineColor, sizeof(outlineColor));
    memcpy(&uniformBufferData[4], Mat4::IDENTITY.m, sizeof(Mat4));

    gfx::InputAssemblerInfo inputAssemblerInfoOutline;
    inputAssemblerInfoOutline.attributes = inputAssemblerInfo.attributes;
    inputAssemblerInfoOutline.vertexBuffers.emplace_back(vertexBufferOutline);
    inputAssemblerInfoOutline.vertexBuffers.emplace_back(instancedBuffer);
    inputAssemblerOutline = device->createInputAssembler(inputAssemblerInfoOutline);

    gfx::PipelineStateInfo pipelineInfoOutline;
    pipelineInfoOutline.primitive      = gfx::PrimitiveMode::LINE_LIST;
    pipelineInfoOutline.shader         = shader;
    pipelineInfoOutline.inputState     = {inputAssemblerOutline->getAttributes()};
    pipelineInfoOutline.renderPass     = fbo->getRenderPass();
    pipelineInfoOutline.pipelineLayout = pipelineLayout;

    pipelineStateOutline = device->createPipelineState(pipelineInfoOutline);
}

void Root::destroy() {
    CC_SAFE_DESTROY(inputAssembler)
    CC_SAFE_DESTROY(vertexBuffer)
    CC_SAFE_DESTROY(instancedBuffer)
    CC_SAFE_DESTROY(uniformBufferGlobal)
    CC_SAFE_DESTROY(shader)
    CC_SAFE_DESTROY(descriptorSet)
    CC_SAFE_DESTROY(descriptorSetLayout)
    CC_SAFE_DESTROY(pipelineLayout)
    CC_SAFE_DESTROY(pipelineState)

    CC_SAFE_DESTROY(vertexBufferOutline)
    CC_SAFE_DESTROY(pipelineStateOutline)
    CC_SAFE_DESTROY(inputAssemblerOutline)

    fbo = nullptr;
    CC_SAFE_DESTROY(msaaFBO)
    CC_SAFE_DESTROY(backBufferFBO)
    CC_SAFE_DESTROY(backBufferRenderPass)
    commandBuffers.clear();

    globalBarriers.clear();
}

void Root::render() {
    static gfx::Texture *backBuffer{nullptr};

    TestBaseI::lookupTime(&TestBaseI::renderThread);
    TestBaseI::printTime(TestBaseI::renderThread, "Render thread");

    gfx::Device *   device     = gfx::Device::getInstance();
    gfx::Swapchain *swapchain  = TestBaseI::swapchain;
    gfx::Color      clearColor = {.1F, .1F, .1F, 1.F};

    size_t modelCount = ModelView::views.size();

    if (modelCount >= modelCapacity) {
        modelCapacity = utils::nextPOT(modelCount + 1);
        instancedBuffer->resize(modelCapacity * uboStride);
        uniformBufferData.resize(modelCapacity * floatCountPerModel);
    }

    for (size_t i = 0; i < modelCount; ++i) {
        auto &&model = vmath::slice(ModelView::buffer, i);
        if (model.transform < 0) break;
        TransformView::views[model.transform]->updateWorldTransform();

        float *pDst = &uniformBufferData[(i + 1) * floatCountPerModel];
        vmath::store(pDst, model.color);
        vmath::store(pDst + 4, vmath::slice(TransformView::buffer.mat, model.transform));
    }

    device->acquire();

    if (ROTATE_VIEW) {
        Mat4         view;
        Mat4         projection;
        static float time = M_PI * 0.5F;
        time += TestBaseI::renderThread.dt * 0.1F;
        Mat4::createLookAt({std::cos(time) * 4.F + 1.F, 1.F, std::sin(time) * 3.F},
                           {0.F, 0.F, .5F}, {0.F, 1.F, 0.F}, &view);
        TestBaseI::createOrthographic(-1.5F, 1.5F, -1.5F, 1.5F, 1.F, 10.F, &projection);
        projection = projection * view;
        uniformBufferGlobal->update(projection.m, sizeof(projection));
    }

    instancedBuffer->update(uniformBufferData.data(), (modelCount + 1) * uboStride);

    gfx::Rect renderArea = {0, 0, swapchain->getWidth(), swapchain->getHeight()};

    auto *commandBuffer = commandBuffers[0];
    commandBuffer->begin();

    if (TestBaseI::MANUAL_BARRIER) {
        commandBuffer->pipelineBarrier(globalBarriers[0]);
    }

    commandBuffer->beginRenderPass(fbo->getRenderPass(), fbo, renderArea, &clearColor, 1.F, 0);

    commandBuffer->bindPipelineState(pipelineStateOutline);
    commandBuffer->bindInputAssembler(inputAssemblerOutline);
    commandBuffer->bindDescriptorSet(0, descriptorSet);
    commandBuffer->draw(inputAssemblerOutline);

    if (modelCount) {
        for (uint i = 0; i < 1; ++i) {
            inputAssembler->setInstanceCount(modelCount);
            commandBuffer->bindPipelineState(pipelineState);
            commandBuffer->bindInputAssembler(inputAssembler);
            commandBuffer->draw(inputAssembler);
        }
    }

    commandBuffer->endRenderPass();

    if (OFFSCREEN_MSAA) {
        gfx::TextureBlit region;
        region.srcExtent.width  = swapchain->getWidth();
        region.srcExtent.height = swapchain->getHeight();
        region.dstExtent.width  = swapchain->getWidth();
        region.dstExtent.height = swapchain->getHeight();
        commandBuffer->blitTexture(msaaFBO->colorTex, nullptr, &region, 1, gfx::Filter::POINT);
    }

    commandBuffer->end();

    device->flushCommands(commandBuffers);
    device->getQueue()->submit(commandBuffers);
    device->present();
}

} // namespace experimental
} // namespace cc
