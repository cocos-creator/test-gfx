#include "DepthTest.h"
#include "tiny_obj_loader.h"

namespace cc {

namespace {

struct DepthResolveFramebuffer {
    explicit DepthResolveFramebuffer(gfx::Device *device) {
        gfx::RenderPassInfo renderPassInfo;

        gfx::ColorAttachment &depthStencilAttachment{renderPassInfo.colorAttachments.emplace_back()};
        depthStencilAttachment.format      = device->getDepthStencilFormat();
        depthStencilAttachment.sampleCount = gfx::SampleCount::X4;
        depthStencilAttachment.storeOp     = gfx::StoreOp::DISCARD;
        depthStencilAttachment.endAccesses = {gfx::AccessType::DEPTH_STENCIL_ATTACHMENT_WRITE};

        gfx::ColorAttachment &depthStencilResolveAttachment{renderPassInfo.colorAttachments.emplace_back()};
        depthStencilResolveAttachment.format      = device->getDepthStencilFormat();
        depthStencilResolveAttachment.loadOp      = gfx::LoadOp::DISCARD;
        depthStencilResolveAttachment.endAccesses = {gfx::AccessType::FRAGMENT_SHADER_READ_TEXTURE};

        gfx::SubpassInfo &subpass{renderPassInfo.subpasses.emplace_back()};
        subpass.depthStencil        = 0U;
        subpass.depthStencilResolve = 1U;
        subpass.depthResolveMode    = gfx::ResolveMode::AVERAGE;
        subpass.stencilResolveMode  = gfx::ResolveMode::AVERAGE;

        renderPass = device->createRenderPass(renderPassInfo);

        gfx::TextureInfo depthStecnilTexMSAAInfo;
        depthStecnilTexMSAAInfo.type    = gfx::TextureType::TEX2D;
        depthStecnilTexMSAAInfo.usage   = gfx::TextureUsageBit::DEPTH_STENCIL_ATTACHMENT;
        depthStecnilTexMSAAInfo.samples = gfx::SampleCount::X4;
        depthStecnilTexMSAAInfo.format  = device->getDepthStencilFormat();
        depthStecnilTexMSAAInfo.width   = device->getWidth();
        depthStecnilTexMSAAInfo.height  = device->getHeight();
        depthStencilTexMSAA             = device->createTexture(depthStecnilTexMSAAInfo);

        gfx::TextureInfo depthStecnilTexInfo;
        depthStecnilTexInfo.type   = gfx::TextureType::TEX2D;
        depthStecnilTexInfo.usage  = gfx::TextureUsageBit::DEPTH_STENCIL_ATTACHMENT | gfx::TextureUsageBit::SAMPLED;
        depthStecnilTexInfo.format = device->getDepthStencilFormat();
        depthStecnilTexInfo.width  = device->getWidth();
        depthStecnilTexInfo.height = device->getHeight();
        depthStencilTex            = device->createTexture(depthStecnilTexInfo);

        gfx::FramebufferInfo fboInfo;
        fboInfo.renderPass = renderPass;
        fboInfo.colorTextures.push_back(depthStencilTexMSAA);
        fboInfo.colorTextures.push_back(depthStencilTex);
        framebuffer = device->createFramebuffer(fboInfo);
    }

    void resize(uint width, uint height) const {
        framebuffer->destroy();

        depthStencilTexMSAA->resize(width, height);
        depthStencilTex->resize(width, height);

        gfx::FramebufferInfo fboInfo;
        fboInfo.renderPass = renderPass;
        fboInfo.colorTextures.push_back(depthStencilTexMSAA);
        fboInfo.colorTextures.push_back(depthStencilTex);
        framebuffer->initialize(fboInfo);
    }

    void destroy() {
        CC_SAFE_DESTROY(framebuffer);
        CC_SAFE_DESTROY(depthStencilTex);
        CC_SAFE_DESTROY(depthStencilTexMSAA);
        CC_SAFE_DESTROY(renderPass);
    }

    gfx::RenderPass * renderPass          = nullptr;
    gfx::Texture *    depthStencilTexMSAA = nullptr;
    gfx::Texture *    depthStencilTex     = nullptr;
    gfx::Framebuffer *framebuffer         = nullptr;
};

struct BigTriangle : public cc::Object {
    BigTriangle(gfx::Device *device, gfx::Framebuffer *fbo) : fbo(fbo), device(device) {
        createShader();
        createBuffers();
        createSampler();
        createInputAssembler();
        createPipeline();
    }

    void createShader() {
        ShaderSources<StandardShaderSource> sources;
        sources.glsl4 = {
            R"(
                precision highp float;
                layout(location = 0) in vec2 a_position;
                layout(location = 1) in vec2 a_texCoord;

                layout(location = 0) out vec2 v_texCoord;

                void main() {
                    v_texCoord = a_texCoord;
                    gl_Position = vec4(a_position, 0, 1);
                }
            )",
            R"(
                precision highp float;
                layout(location = 0) in vec2 v_texCoord;
                layout(set = 0, binding = 0) uniform Near_Far_Uniform
                {
                    float u_near;
                    float u_far;
                };
                layout(set = 0, binding = 1) uniform sampler2D u_texture;
                layout(location = 0) out vec4 o_color;
                void main() {
                    float z = texture(u_texture, v_texCoord).x;
                    float viewZ = (u_near * u_far) / ((u_far - u_near) * z - u_far);
                    float depth = (viewZ + u_near) / (u_near - u_far);

                    o_color.rgb = vec3(depth);
                    o_color.a = 1.0;
                }
            )",
        };

        sources.glsl3 = {
            R"(
                in vec2 a_position;
                in vec2 a_texCoord;

                out vec2 v_texCoord;

                void main() {
                    v_texCoord = a_texCoord;
                    gl_Position = vec4(a_position, 0, 1);
                }
            )",
            R"(
                precision mediump float;
                in vec2 v_texCoord;
                uniform sampler2D u_texture;
                layout(std140) uniform Near_Far_Uniform {
                    float u_near;
                    float u_far;
                };
                out vec4 o_color;
                void main() {
                    float z = texture(u_texture, v_texCoord).x;
                    float viewZ = (u_near * u_far) / ((u_far - u_near) * z - u_far);
                    float depth = (viewZ + u_near) / (u_near - u_far);

                    o_color.rgb = vec3(depth);
                    o_color.a = 1.0;
                }
            )",
        };

        sources.glsl1 = {
            R"(
                attribute vec2 a_position;
                attribute vec2 a_texCoord;

                varying vec2 v_texCoord;

                void main() {
                    v_texCoord = a_texCoord;
                    gl_Position = vec4(a_position, 0, 1);
                }
            )",
            R"(
                precision mediump float;
                varying vec2 v_texCoord;
                uniform sampler2D u_texture;
                uniform float u_near;
                uniform float u_far;

                void main() {
                    float z = texture2D(u_texture, v_texCoord).x;
                    float viewZ = (u_near * u_far) / ((u_far - u_near) * z - u_far);
                    float depth = (viewZ + u_near) / (u_near - u_far);

                    gl_FragColor.rgb = vec3(depth);
                    gl_FragColor.a = 1.0;
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

        gfx::AttributeList attributeList = {
            {"a_position", gfx::Format::RG32F, false, 0, false, 0},
            {"a_texCoord", gfx::Format::RG32F, false, 0, false, 1},
        };
        gfx::UniformList               nearFarUniform   = {{"u_near", gfx::Type::FLOAT, 1}, {"u_far", gfx::Type::FLOAT, 1}};
        gfx::UniformBlockList          uniformBlockList = {{0, 0, "Near_Far_Uniform", nearFarUniform, 1}};
        gfx::UniformSamplerTextureList samplers         = {{0, 1, "u_texture", gfx::Type::SAMPLER2D, 1}};

        gfx::ShaderInfo shaderInfo;
        shaderInfo.name            = "BigTriangle";
        shaderInfo.stages          = std::move(shaderStageList);
        shaderInfo.attributes      = std::move(attributeList);
        shaderInfo.blocks          = std::move(uniformBlockList);
        shaderInfo.samplerTextures = std::move(samplers);
        shader                     = device->createShader(shaderInfo);
    }

    void createSampler() {
        // create sampler
        gfx::SamplerInfo samplerInfo;
        sampler = device->createSampler(samplerInfo);
    }

    void createBuffers() {
        // create vertex buffer
        float ySign = device->getCapabilities().screenSpaceSignY;
        // UV space origin is at top-left
        float vertices[] = {-1, 4 * ySign, 0.0, -1.5,
                            -1, -1 * ySign, 0.0, 1.0,
                            4, -1 * ySign, 2.5, 1.0};

        vertexBuffer = device->createBuffer({
            gfx::BufferUsage::VERTEX,
            gfx::MemoryUsage::DEVICE,
            sizeof(vertices),
            4 * sizeof(float),
        });
        vertexBuffer->update(vertices, sizeof(vertices));

        // create uniform buffer
        nearFarUniformBuffer = device->createBuffer({
            gfx::BufferUsage::UNIFORM,
            gfx::MemoryUsage::DEVICE,
            TestBaseI::getUBOSize(2 * sizeof(float)),
        });

        float uboData[] = {1.F, 100.0F};
        nearFarUniformBuffer->update(uboData, sizeof(uboData));
    }

    void createInputAssembler() {
        gfx::InputAssemblerInfo inputAssemblerInfo;
        inputAssemblerInfo.attributes.push_back({"a_position", gfx::Format::RG32F, false, 0, false});
        inputAssemblerInfo.attributes.push_back({"a_texCoord", gfx::Format::RG32F, false, 0, false});
        inputAssemblerInfo.vertexBuffers.emplace_back(vertexBuffer);
        inputAssembler = device->createInputAssembler(inputAssemblerInfo);
    }

    void createPipeline() {
        gfx::DescriptorSetLayoutInfo dslInfo;
        dslInfo.bindings.push_back({0, gfx::DescriptorType::UNIFORM_BUFFER, 1, gfx::ShaderStageFlagBit::FRAGMENT});
        dslInfo.bindings.push_back({1, gfx::DescriptorType::SAMPLER_TEXTURE, 1, gfx::ShaderStageFlagBit::FRAGMENT});
        descriptorSetLayout = device->createDescriptorSetLayout(dslInfo);

        pipelineLayout = device->createPipelineLayout({{descriptorSetLayout}});

        descriptorSet = device->createDescriptorSet({descriptorSetLayout});

        descriptorSet->bindBuffer(0, nearFarUniformBuffer);
        descriptorSet->bindSampler(1, sampler);
        // don't update just yet for the texture is still missing

        gfx::PipelineStateInfo pipelineInfo;
        pipelineInfo.primitive                    = gfx::PrimitiveMode::TRIANGLE_LIST;
        pipelineInfo.shader                       = shader;
        pipelineInfo.inputState.attributes        = inputAssembler->getAttributes();
        pipelineInfo.renderPass                   = fbo->getRenderPass();
        pipelineInfo.depthStencilState.depthTest  = false;
        pipelineInfo.depthStencilState.depthWrite = false;
        pipelineInfo.rasterizerState.cullMode     = gfx::CullMode::NONE;
        pipelineInfo.pipelineLayout               = pipelineLayout;

        pipelineState = device->createPipelineState(pipelineInfo);
    }

    void destroy() {
        CC_SAFE_DESTROY(shader);
        CC_SAFE_DESTROY(vertexBuffer);
        CC_SAFE_DESTROY(inputAssembler);
        CC_SAFE_DESTROY(descriptorSet);
        CC_SAFE_DESTROY(descriptorSetLayout);
        CC_SAFE_DESTROY(pipelineLayout);
        CC_SAFE_DESTROY(sampler);
        CC_SAFE_DESTROY(texture);
        CC_SAFE_DESTROY(pipelineState);
        CC_SAFE_DESTROY(nearFarUniformBuffer);
    }

    gfx::Shader *             shader               = nullptr;
    gfx::Framebuffer *        fbo                  = nullptr;
    gfx::Buffer *             vertexBuffer         = nullptr;
    gfx::Buffer *             nearFarUniformBuffer = nullptr;
    gfx::Device *             device               = nullptr;
    gfx::InputAssembler *     inputAssembler       = nullptr;
    gfx::DescriptorSet *      descriptorSet        = nullptr;
    gfx::DescriptorSetLayout *descriptorSetLayout  = nullptr;
    gfx::PipelineLayout *     pipelineLayout       = nullptr;
    gfx::Sampler *            sampler              = nullptr;
    gfx::Texture *            texture              = nullptr;
    gfx::PipelineState *      pipelineState        = nullptr;
};

struct Bunny : public cc::Object {
    Bunny(gfx::Device *device, gfx::Framebuffer *fbo) : device(device) {
        createShader();
        createBuffers();
        createInputAssembler();
        createPipeline(fbo);
    }

    void createShader() {
        ShaderSources<StandardShaderSource> sources;
        sources.glsl4 = {
            R"(
                layout(location = 0) in vec3 a_position;
                layout(set = 0, binding = 0) uniform MVP_Matrix {
                    mat4 u_model;
                    mat4 u_view;
                    mat4 u_projection;
                };
                void main () {
                    vec4 pos = u_projection * u_view * u_model * vec4(a_position, 1);

                    gl_Position = pos;
                }
            )",
            R"(
                void main () {}
            )",
        };

        sources.glsl3 = {
            R"(
                in vec3 a_position;
                layout(std140) uniform MVP_Matrix {
                    mat4 u_model;
                    mat4 u_view;
                    mat4 u_projection;
                };
                void main () {
                    vec4 pos = u_projection * u_view * u_model * vec4(a_position, 1);

                    gl_Position = pos;
                }
            )",
            R"(
                precision mediump float;
                void main () {}
            )",
        };

        sources.glsl1 = {
            R"(
                attribute vec3 a_position;
                uniform mat4 u_model, u_view, u_projection;

                void main () {
                    vec4 pos = u_projection * u_view * u_model * vec4(a_position, 1);

                    gl_Position = pos;
                }
            )",
            R"(
                precision mediump float;
                void main () {}
            )",
        };

        StandardShaderSource &source = TestBaseI::getAppropriateShaderSource(sources);

        // vertex shader
        gfx::ShaderStageList shaderStageList;
        gfx::ShaderStage     vertexShaderStage;
        vertexShaderStage.stage  = gfx::ShaderStageFlagBit::VERTEX;
        vertexShaderStage.source = source.vert;
        shaderStageList.emplace_back(std::move(vertexShaderStage));

        // fragment shader
        gfx::ShaderStage fragmentShaderStage;
        fragmentShaderStage.stage  = gfx::ShaderStageFlagBit::FRAGMENT;
        fragmentShaderStage.source = source.frag;
        shaderStageList.emplace_back(std::move(fragmentShaderStage));

        gfx::AttributeList attributeList = {{"a_position", gfx::Format::RGB32F, false, 0, false, 0}};
        gfx::UniformList   mvpMatrix     = {
            {"u_model", gfx::Type::MAT4, 1},
            {"u_view", gfx::Type::MAT4, 1},
            {"u_projection", gfx::Type::MAT4, 1},
        };
        gfx::UniformBlockList uniformBlockList = {{0, 0, "MVP_Matrix", mvpMatrix, 1}};

        gfx::ShaderInfo shaderInfo;
        shaderInfo.name       = "Bunny";
        shaderInfo.attributes = std::move(attributeList);
        shaderInfo.stages     = std::move(shaderStageList);
        shaderInfo.blocks     = std::move(uniformBlockList);
        shader                = device->createShader(shaderInfo);
    }

    void createBuffers() {
        auto obj = TestBaseI::loadOBJ("bunny.obj");

        // vertex buffer
        const auto &positions = obj.GetAttrib().vertices;
        vertexBuffer          = device->createBuffer({
            gfx::BufferUsage::VERTEX,
            gfx::MemoryUsage::DEVICE,
            static_cast<uint>(positions.size() * sizeof(float)),
            3 * sizeof(float),
        });
        vertexBuffer->update(positions.data(), positions.size() * sizeof(float));

        // index buffer
        const auto &     indicesInfo = obj.GetShapes()[0].mesh.indices;
        vector<uint16_t> indices;
        indices.reserve(indicesInfo.size());
        std::transform(indicesInfo.begin(), indicesInfo.end(), std::back_inserter(indices),
                       [](auto &&info) { return static_cast<uint16_t>(info.vertex_index); });

        indexBuffer = device->createBuffer({
            gfx::BufferUsage::INDEX,
            gfx::MemoryUsage::DEVICE,
            static_cast<uint>(indices.size() * sizeof(uint16_t)),
            sizeof(uint16_t),
        });
        indexBuffer->update(indices.data(), indices.size() * sizeof(uint16_t));

        // uniform buffer
        // create uniform buffer
        gfx::BufferInfo uniformBufferInfo = {
            gfx::BufferUsage::UNIFORM,
            gfx::MemoryUsage::HOST | gfx::MemoryUsage::DEVICE,
            TestBaseI::getUBOSize(3 * sizeof(Mat4)),
        };
        for (auto &i : mvpUniformBuffer) {
            i = device->createBuffer(uniformBufferInfo);
        }
    }

    void createInputAssembler() {
        gfx::Attribute          position = {"a_position", gfx::Format::RGB32F, false, 0, false};
        gfx::InputAssemblerInfo inputAssemblerInfo;
        inputAssemblerInfo.attributes.emplace_back(std::move(position));
        inputAssemblerInfo.vertexBuffers.emplace_back(vertexBuffer);
        inputAssemblerInfo.indexBuffer = indexBuffer;
        inputAssembler                 = device->createInputAssembler(inputAssemblerInfo);
    }

    void createPipeline(gfx::Framebuffer *fbo) {
        gfx::DescriptorSetLayoutInfo dslInfo;
        dslInfo.bindings.push_back({0, gfx::DescriptorType::UNIFORM_BUFFER, 1, gfx::ShaderStageFlagBit::VERTEX});
        descriptorSetLayout = device->createDescriptorSetLayout(dslInfo);

        pipelineLayout = device->createPipelineLayout({{descriptorSetLayout}});

        for (uint i = 0U; i < BUNNY_NUM; i++) {
            descriptorSet[i] = device->createDescriptorSet({descriptorSetLayout});

            descriptorSet[i]->bindBuffer(0, mvpUniformBuffer[i]);
            descriptorSet[i]->update();
        }

        gfx::PipelineStateInfo pipelineInfo;
        pipelineInfo.primitive  = gfx::PrimitiveMode::TRIANGLE_LIST;
        pipelineInfo.shader     = shader;
        pipelineInfo.inputState = {inputAssembler->getAttributes()};
        pipelineInfo.renderPass = fbo->getRenderPass();
        pipelineInfo.blendState.targets.clear();
        pipelineInfo.pipelineLayout = pipelineLayout;

        pipelineState = device->createPipelineState(pipelineInfo);
    }

    void destroy() {
        CC_SAFE_DESTROY(shader);
        CC_SAFE_DESTROY(vertexBuffer);
        CC_SAFE_DESTROY(indexBuffer);
        CC_SAFE_DESTROY(sampler);
        CC_SAFE_DESTROY(depthTexture);
        CC_SAFE_DESTROY(inputAssembler);
        for (uint i = 0; i < BUNNY_NUM; i++) {
            CC_SAFE_DESTROY(mvpUniformBuffer[i]);
            CC_SAFE_DESTROY(descriptorSet[i]);
        }
        CC_SAFE_DESTROY(descriptorSetLayout);
        CC_SAFE_DESTROY(pipelineLayout);
        CC_SAFE_DESTROY(pipelineState);
    }
    const static uint         BUNNY_NUM                   = 2;
    gfx::Device *             device                      = nullptr;
    gfx::Shader *             shader                      = nullptr;
    gfx::Buffer *             vertexBuffer                = nullptr;
    gfx::Buffer *             indexBuffer                 = nullptr;
    gfx::Sampler *            sampler                     = nullptr;
    gfx::Texture *            depthTexture                = nullptr;
    gfx::InputAssembler *     inputAssembler              = nullptr;
    gfx::DescriptorSetLayout *descriptorSetLayout         = nullptr;
    gfx::PipelineLayout *     pipelineLayout              = nullptr;
    gfx::Buffer *             mvpUniformBuffer[BUNNY_NUM] = {nullptr, nullptr};
    gfx::DescriptorSet *      descriptorSet[BUNNY_NUM]    = {nullptr, nullptr};
    gfx::PipelineState *      pipelineState               = nullptr;
};

BigTriangle *            bg{nullptr};
Bunny *                  bunny{nullptr};
DepthResolveFramebuffer *bunnyFBO{nullptr};
} // namespace

void DepthTexture::onDestroy() {
    CC_SAFE_DESTROY(bg);
    CC_SAFE_DESTROY(bunny);
    CC_SAFE_DESTROY(bunnyFBO);
}

void DepthTexture::onResize(uint width, uint height) {
    bunnyFBO->resize(width, height);
}

bool DepthTexture::onInit() {
    bunnyFBO = CC_NEW(DepthResolveFramebuffer(device));
    bunny    = CC_NEW(Bunny(device, bunnyFBO->framebuffer));
    bg       = CC_NEW(BigTriangle(device, fbo));

    bg->descriptorSet->bindTexture(1, bunnyFBO->depthStencilTex);
    bg->descriptorSet->update();

    _globalBarriers.push_back(TestBaseI::getGlobalBarrier({
        {
            gfx::AccessType::TRANSFER_WRITE,
        },
        {
            gfx::AccessType::VERTEX_SHADER_READ_UNIFORM_BUFFER,
            gfx::AccessType::FRAGMENT_SHADER_READ_UNIFORM_BUFFER,
            gfx::AccessType::VERTEX_BUFFER,
            gfx::AccessType::INDEX_BUFFER,
        },
    }));

    _globalBarriers.push_back(TestBaseI::getGlobalBarrier({
        {
            gfx::AccessType::TRANSFER_WRITE,
        },
        {
            gfx::AccessType::VERTEX_SHADER_READ_UNIFORM_BUFFER,
        },
    }));

    return true;
}

void DepthTexture::onTick() {
    uint globalBarrierIdx = _frameCount ? 1 : 0;

    static constexpr float CAMERA_DISTANCE = 8.F;
    _eye.set(CAMERA_DISTANCE * std::cos(_time), CAMERA_DISTANCE * 0.5F, CAMERA_DISTANCE * std::sin(_time));
    _center.set(0, 0.5F, 0);
    _up.set(0, 1.F, 0);
    Mat4::createLookAt(_eye, _center, _up, &_bunnyMatrices[1]);
    gfx::Extent orientedSize = TestBaseI::getOrientedSurfaceSize();
    TestBaseI::createPerspective(45.F,
                                 static_cast<float>(orientedSize.width) / static_cast<float>(orientedSize.height),
                                 1.F, 10.F, &_bunnyMatrices[2]);

    gfx::Color clearColor[2] = {{1.0, 0, 0, 1.0F}};

    device->acquire();

    for (uint i = 0; i < Bunny::BUNNY_NUM; i++) {
        _bunnyMatrices[0].m[12] = i % 2 ? -1.5F : 1.5F;
        bunny->mvpUniformBuffer[i]->update(_bunnyMatrices, sizeof(_bunnyMatrices));
    }
    gfx::Rect renderArea = {0, 0, device->getWidth(), device->getHeight()};

    auto *commandBuffer = commandBuffers[0];
    commandBuffer->begin();

    if (TestBaseI::MANUAL_BARRIER) {
        commandBuffer->pipelineBarrier(_globalBarriers[globalBarrierIdx]);
    }

    // render bunny
    commandBuffer->beginRenderPass(bunnyFBO->renderPass, bunnyFBO->framebuffer, renderArea, clearColor, 1.0F, 0);
    commandBuffer->bindPipelineState(bunny->pipelineState);
    commandBuffer->bindInputAssembler(bunny->inputAssembler);
    for (auto &i : bunny->descriptorSet) {
        commandBuffer->bindDescriptorSet(0, i);
        commandBuffer->draw(bunny->inputAssembler);
    }
    commandBuffer->endRenderPass();

    // render bg
    commandBuffer->beginRenderPass(fbo->getRenderPass(), fbo, renderArea, clearColor, 1.0F, 0);
    commandBuffer->bindInputAssembler(bg->inputAssembler);
    commandBuffer->bindPipelineState(bg->pipelineState);
    commandBuffer->bindDescriptorSet(0, bg->descriptorSet);
    commandBuffer->draw(bg->inputAssembler);
    commandBuffer->endRenderPass();

    commandBuffer->end();

    device->flushCommands(commandBuffers);
    device->getQueue()->submit(commandBuffers);
    device->present();
}

} // namespace cc
