#include "DepthTest.h"
#include "BunnyData.h"

namespace cc {

namespace {
struct BigTriangle : public cc::Object {
    BigTriangle(gfx::Device *_device, gfx::Framebuffer *_fbo) : fbo(_fbo), device(_device) {
        createShader();
        createBuffers();
        createSampler();
        createInputAssembler();
        createPipeline();
    }

    void createShader() {

        ShaderSources sources;
        sources.glsl4 = {
            R"(
                layout(location = 0) in vec2 a_position;
                layout(location = 1) in vec2 a_texCoord;

                layout(location = 0) out vec2 v_texCoord;

                void main() {
                    v_texCoord = a_texCoord;
                    gl_Position = vec4(a_position, 0, 1);
                }
            )",
            R"(
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

        ShaderSource &source = TestBaseI::getAppropriateShaderSource(sources);

        gfx::ShaderStageList shaderStageList;
        gfx::ShaderStage vertexShaderStage;
        vertexShaderStage.stage = gfx::ShaderStageFlagBit::VERTEX;
        vertexShaderStage.source = source.vert;
        shaderStageList.emplace_back(std::move(vertexShaderStage));

        gfx::ShaderStage fragmentShaderStage;
        fragmentShaderStage.stage = gfx::ShaderStageFlagBit::FRAGMENT;
        fragmentShaderStage.source = source.frag;
        shaderStageList.emplace_back(std::move(fragmentShaderStage));

        gfx::AttributeList attributeList = {
            {"a_position", gfx::Format::RG32F, false, 0, false, 0},
            {"a_texCoord", gfx::Format::RG32F, false, 0, false, 1},
        };
        gfx::UniformList nearFarUniform = {{"u_near", gfx::Type::FLOAT, 1}, {"u_far", gfx::Type::FLOAT, 1}};
        gfx::UniformBlockList uniformBlockList = {{0, 0, "Near_Far_Uniform", nearFarUniform, 1}};
        gfx::UniformSamplerList samplers = {{0, 1, "u_texture", gfx::Type::SAMPLER2D, 1}};

        gfx::ShaderInfo shaderInfo;
        shaderInfo.name = "BigTriangle";
        shaderInfo.stages = std::move(shaderStageList);
        shaderInfo.attributes = std::move(attributeList);
        shaderInfo.blocks = std::move(uniformBlockList);
        shaderInfo.samplers = std::move(samplers);
        shader = device->createShader(shaderInfo);
    }

    void createSampler() {
        // create sampler
        gfx::SamplerInfo samplerInfo;
        sampler = device->createSampler(samplerInfo);
    }

    void createBuffers() {
        // create vertex buffer
        float ySign = device->getScreenSpaceSignY();
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
        vertexBuffer->update(vertices, 0, sizeof(vertices));

        // create uniform buffer
        nearFarUniformBuffer = device->createBuffer({
            gfx::BufferUsage::UNIFORM,
            gfx::MemoryUsage::DEVICE,
            TestBaseI::getUBOSize(2 * sizeof(float)),
        });

        float uboData[] = {0.1f, 100.0f};
        nearFarUniformBuffer->update(uboData, 0, sizeof(uboData));
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
        dslInfo.bindings.push_back({1, gfx::DescriptorType::SAMPLER, 1, gfx::ShaderStageFlagBit::FRAGMENT});
        descriptorSetLayout = device->createDescriptorSetLayout(dslInfo);

        pipelineLayout = device->createPipelineLayout({{descriptorSetLayout}});

        descriptorSet = device->createDescriptorSet({descriptorSetLayout});

        descriptorSet->bindBuffer(0, nearFarUniformBuffer);
        descriptorSet->bindSampler(1, sampler);
        // don't update just yet for the texture is still missing

        gfx::PipelineStateInfo pipelineInfo;
        pipelineInfo.primitive = gfx::PrimitiveMode::TRIANGLE_LIST;
        pipelineInfo.shader = shader;
        pipelineInfo.blendState.targets.push_back(new gfx::BlendTarget());
        pipelineInfo.inputState.attributes = inputAssembler->getAttributes();
        pipelineInfo.renderPass = fbo->getRenderPass();
        pipelineInfo.depthStencilState.depthTest = false;
        pipelineInfo.depthStencilState.depthWrite = false;
        pipelineInfo.rasterizerState.cullMode = gfx::CullMode::NONE;
        pipelineInfo.pipelineLayout = pipelineLayout;

        pipelineState = device->createPipelineState(pipelineInfo);
    }
    ~BigTriangle() {}

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

    gfx::Shader *shader = nullptr;
    gfx::Framebuffer *fbo = nullptr;
    gfx::Buffer *vertexBuffer = nullptr;
    gfx::Buffer *nearFarUniformBuffer = nullptr;
    gfx::Device *device = nullptr;
    gfx::InputAssembler *inputAssembler = nullptr;
    gfx::DescriptorSet *descriptorSet = nullptr;
    gfx::DescriptorSetLayout *descriptorSetLayout = nullptr;
    gfx::PipelineLayout *pipelineLayout = nullptr;
    gfx::Sampler *sampler = nullptr;
    gfx::Texture *texture = nullptr;
    gfx::PipelineState *pipelineState = nullptr;
};

struct Bunny : public cc::Object {
    Bunny(gfx::Device *_device, gfx::Framebuffer *_fbo) : device(_device) {
        createShader();
        createBuffers();
        createInputAssembler();
        createPipeline(_fbo);
    }

    ~Bunny() {}

    void createShader() {

        ShaderSources sources;
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

        ShaderSource &source = TestBaseI::getAppropriateShaderSource(sources);

        // vertex shader
        gfx::ShaderStageList shaderStageList;
        gfx::ShaderStage vertexShaderStage;
        vertexShaderStage.stage = gfx::ShaderStageFlagBit::VERTEX;
        vertexShaderStage.source = source.vert;
        shaderStageList.emplace_back(std::move(vertexShaderStage));

        // fragment shader
        gfx::ShaderStage fragmentShaderStage;
        fragmentShaderStage.stage = gfx::ShaderStageFlagBit::FRAGMENT;
        fragmentShaderStage.source = source.frag;
        shaderStageList.emplace_back(std::move(fragmentShaderStage));

        gfx::AttributeList attributeList = {{"a_position", gfx::Format::RGB32F, false, 0, false, 0}};
        gfx::UniformList mvpMatrix = {
            {"u_model", gfx::Type::MAT4, 1},
            {"u_view", gfx::Type::MAT4, 1},
            {"u_projection", gfx::Type::MAT4, 1},
        };
        gfx::UniformBlockList uniformBlockList = {{0, 0, "MVP_Matrix", mvpMatrix, 1}};

        gfx::ShaderInfo shaderInfo;
        shaderInfo.name = "Bunny";
        shaderInfo.attributes = std::move(attributeList);
        shaderInfo.stages = std::move(shaderStageList);
        shaderInfo.blocks = std::move(uniformBlockList);
        shader = device->createShader(shaderInfo);
    }

    void createBuffers() {
        // vertex buffer
        vertexBuffer = device->createBuffer({
            gfx::BufferUsage::VERTEX,
            gfx::MemoryUsage::DEVICE,
            sizeof(bunny_positions),
            3 * sizeof(float),
        });
        vertexBuffer->update((void *)&bunny_positions[0][0], 0, sizeof(bunny_positions));

        // index buffer
        indexBuffer = device->createBuffer({
            gfx::BufferUsage::INDEX,
            gfx::MemoryUsage::DEVICE,
            sizeof(bunny_cells),
            sizeof(unsigned short),
        });
        indexBuffer->update((void *)&bunny_cells[0], 0, sizeof(bunny_cells));

        // uniform buffer
        // create uniform buffer
        gfx::BufferInfo uniformBufferInfo = {
            gfx::BufferUsage::UNIFORM,
            gfx::MemoryUsage::HOST | gfx::MemoryUsage::DEVICE,
            TestBaseI::getUBOSize(3 * sizeof(Mat4)),
        };
        for (uint i = 0; i < BUNNY_NUM; i++)
            mvpUniformBuffer[i] = device->createBuffer(uniformBufferInfo);
    }

    void createInputAssembler() {
        gfx::Attribute position = {"a_position", gfx::Format::RGB32F, false, 0, false};
        gfx::InputAssemblerInfo inputAssemblerInfo;
        inputAssemblerInfo.attributes.emplace_back(std::move(position));
        inputAssemblerInfo.vertexBuffers.emplace_back(vertexBuffer);
        inputAssemblerInfo.indexBuffer = indexBuffer;
        inputAssembler = device->createInputAssembler(inputAssemblerInfo);
    }

    void createPipeline(gfx::Framebuffer *_fbo) {
        gfx::DescriptorSetLayoutInfo dslInfo;
        dslInfo.bindings.push_back({0, gfx::DescriptorType::UNIFORM_BUFFER, 1, gfx::ShaderStageFlagBit::VERTEX});
        descriptorSetLayout = device->createDescriptorSetLayout(dslInfo);

        pipelineLayout = device->createPipelineLayout({{descriptorSetLayout}});

        for (uint i = 0u; i < BUNNY_NUM; i++) {
            descriptorSet[i] = device->createDescriptorSet({descriptorSetLayout});

            descriptorSet[i]->bindBuffer(0, mvpUniformBuffer[i]);
            descriptorSet[i]->update();
        }

        gfx::PipelineStateInfo pipelineInfo;
        pipelineInfo.primitive = gfx::PrimitiveMode::TRIANGLE_LIST;
        pipelineInfo.shader = shader;
        pipelineInfo.inputState = {inputAssembler->getAttributes()};
        pipelineInfo.blendState.targets.push_back(new gfx::BlendTarget());
        pipelineInfo.renderPass = _fbo->getRenderPass();
        pipelineInfo.depthStencilState.depthTest = true;
        pipelineInfo.depthStencilState.depthWrite = true;
        pipelineInfo.depthStencilState.depthFunc = gfx::ComparisonFunc::LESS;
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
    const static uint BUNNY_NUM = 2;
    gfx::Device *device = nullptr;
    gfx::Shader *shader = nullptr;
    gfx::Buffer *vertexBuffer = nullptr;
    gfx::Buffer *indexBuffer = nullptr;
    gfx::Sampler *sampler = nullptr;
    gfx::Texture *depthTexture = nullptr;
    gfx::InputAssembler *inputAssembler = nullptr;
    gfx::DescriptorSetLayout *descriptorSetLayout = nullptr;
    gfx::PipelineLayout *pipelineLayout = nullptr;
    gfx::Buffer *mvpUniformBuffer[BUNNY_NUM] = {nullptr, nullptr};
    gfx::DescriptorSet *descriptorSet[BUNNY_NUM] = {nullptr, nullptr};
    gfx::PipelineState *pipelineState = nullptr;
};

BigTriangle *bg;
Bunny *bunny;
} // namespace

void DepthTexture::destroy() {
    CC_SAFE_DESTROY(bg);
    CC_SAFE_DESTROY(bunny);
    CC_SAFE_DELETE(_bunnyFBO);
}

bool DepthTexture::initialize() {
    _bunnyFBO = CC_NEW(Framebuffer);

    gfx::RenderPassInfo renderPassInfo;

    gfx::DepthStencilAttachment &depthStencilAttachment = renderPassInfo.depthStencilAttachment;
    depthStencilAttachment.format = _device->getDepthStencilFormat();
    depthStencilAttachment.depthLoadOp = gfx::LoadOp::CLEAR;
    depthStencilAttachment.depthStoreOp = gfx::StoreOp::STORE;
    depthStencilAttachment.stencilLoadOp = gfx::LoadOp::CLEAR;
    depthStencilAttachment.stencilStoreOp = gfx::StoreOp::STORE;
    depthStencilAttachment.sampleCount = 1;
    depthStencilAttachment.beginLayout = gfx::TextureLayout::UNDEFINED;
    depthStencilAttachment.endLayout = gfx::TextureLayout::SHADER_READONLY_OPTIMAL;

    _bunnyFBO->renderPass = _device->createRenderPass(renderPassInfo);

    gfx::TextureInfo depthStecnilTexInfo;
    depthStecnilTexInfo.type = gfx::TextureType::TEX2D;
    depthStecnilTexInfo.usage = gfx::TextureUsageBit::DEPTH_STENCIL_ATTACHMENT | gfx::TextureUsageBit::SAMPLED;
    depthStecnilTexInfo.format = _device->getDepthStencilFormat();
    depthStecnilTexInfo.width = _device->getWidth();
    depthStecnilTexInfo.height = _device->getHeight();
    _bunnyFBO->depthStencilTex = _device->createTexture(depthStecnilTexInfo);

    gfx::FramebufferInfo fboInfo;
    fboInfo.renderPass = _bunnyFBO->renderPass;
    fboInfo.depthStencilTexture = _bunnyFBO->depthStencilTex;
    _bunnyFBO->framebuffer = _device->createFramebuffer(fboInfo);

    bunny = CC_NEW(Bunny(_device, _bunnyFBO->framebuffer));
    bg = CC_NEW(BigTriangle(_device, _fbo));

    bg->descriptorSet->bindTexture(1, _bunnyFBO->depthStencilTex);
    bg->descriptorSet->update();

    return true;
}

void DepthTexture::resize(uint width, uint height) {
    TestBaseI::resize(width, height);

    _bunnyFBO->depthStencilTex->resize(width, height);

    gfx::FramebufferInfo fboInfo;
    fboInfo.renderPass = _bunnyFBO->renderPass;
    fboInfo.depthStencilTexture = _bunnyFBO->depthStencilTex;

    _bunnyFBO->framebuffer->destroy();
    _bunnyFBO->framebuffer->initialize(fboInfo);
}

void DepthTexture::tick(float dt) {
    _dt += dt;

    _eye.set(30.f * std::cos(_dt), 20.f, 30.f * std::sin(_dt));
    _center.set(0, 2.5f, 0);
    _up.set(0, 1.f, 0);
    Mat4::createLookAt(_eye, _center, _up, &_view);
    gfx::Extent orientedSize = TestBaseI::getOrientedSurfaceSize();
    TestBaseI::createPerspective(45.f, 1.0f * orientedSize.width / orientedSize.height, 0.1f, 100.f, &_projection, true);

    gfx::Color clearColor = {1.0, 0, 0, 1.0f};

    _device->acquire();

    for (uint i = 0; i < Bunny::BUNNY_NUM; i++) {
        _model = Mat4::IDENTITY;
        if (i % 2 == 0)
            _model.translate(5, 0, 0);
        else
            _model.translate(-5, 0, 0);
        bunny->mvpUniformBuffer[i]->update(_model.m, 0, sizeof(_model));
        bunny->mvpUniformBuffer[i]->update(_view.m, sizeof(_model), sizeof(_view));
        bunny->mvpUniformBuffer[i]->update(_projection.m, sizeof(_model) + sizeof(_view), sizeof(_projection));
    }
    gfx::Rect renderArea = {0, 0, _device->getWidth(), _device->getHeight()};

    auto commandBuffer = _commandBuffers[0];
    commandBuffer->begin();

    // render bunny
    commandBuffer->beginRenderPass(_bunnyFBO->renderPass, _bunnyFBO->framebuffer, renderArea, nullptr, 1.0f, 0);
    commandBuffer->bindPipelineState(bunny->pipelineState);
    commandBuffer->bindInputAssembler(bunny->inputAssembler);
    for (uint i = 0; i < Bunny::BUNNY_NUM; i++) {
        commandBuffer->bindDescriptorSet(0, bunny->descriptorSet[i]);
        commandBuffer->draw(bunny->inputAssembler);
    }
    commandBuffer->endRenderPass();

    // render bg
    commandBuffer->beginRenderPass(_fbo->getRenderPass(), _fbo, renderArea, &clearColor, 1.0f, 0);
    commandBuffer->bindInputAssembler(bg->inputAssembler);
    commandBuffer->bindPipelineState(bg->pipelineState);
    commandBuffer->bindDescriptorSet(0, bg->descriptorSet);
    commandBuffer->draw(bg->inputAssembler);
    commandBuffer->endRenderPass();

    commandBuffer->end();

    _device->getQueue()->submit(_commandBuffers);
    _device->present();
}

} // namespace cc
