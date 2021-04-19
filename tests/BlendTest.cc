#include "BlendTest.h"

namespace cc {

namespace {
enum {
    NO_BLEND = 0x0,
    NORMAL_BLEND,
    ADDITIVE_BLEND,
    SUBSTRACT_BLEND,
    MULTIPLY_BLEND,
    TOTAL_BLEND
};

struct Quad : public cc::Object {
    Quad(gfx::Device *device, gfx::Framebuffer *fbo) : device(device), fbo(fbo) {
        createShader();
        createVertexBuffer();
        createInputAssembler();
        createTexture();
        createPipeline();
    }

    ~Quad() {}

    void destroy() {
        CC_SAFE_DESTROY(shader);
        CC_SAFE_DESTROY(vertexBuffer);
        CC_SAFE_DESTROY(inputAssembler);
        CC_SAFE_DESTROY(indexBuffer);
        CC_SAFE_DESTROY(texture);
        CC_SAFE_DESTROY(sampler);
        CC_SAFE_DESTROY(descriptorSet);
        CC_SAFE_DESTROY(uniformBufferView);
        CC_SAFE_DESTROY(uniformBuffer);
        CC_SAFE_DESTROY(descriptorSetLayout);
        CC_SAFE_DESTROY(pipelineLayout);
        for (int i = 0; i < TOTAL_BLEND; i++) {
            CC_SAFE_DESTROY(pipelineState[i]);
        }
    }

    void createShader() {

        ShaderSources<StandardShaderSource> sources;
        sources.glsl4 = {
            R"(
                precision highp float;
                layout(location = 0) in vec2 a_position;
                layout(location = 1) in vec2 a_uv;
                layout(location = 0) out vec2 uv;
                layout(set = 0, binding = 0) uniform MVP_Matrix {
                    mat4 u_model, u_projection;
                };
                void main() {
                    uv = a_uv;
                    gl_Position = u_projection * u_model * vec4(a_position, 0, 1);
                }
            )",
            R"(
                precision highp float;
                layout(location = 0) in vec2 uv;
                layout(set = 0, binding = 1) uniform sampler2D u_texture;
                layout(location = 0) out vec4 o_color;
                void main() {
                    o_color = texture(u_texture, uv);
                }
            )",
        };

        sources.glsl3 = {
            R"(
                in vec2 a_position;
                in vec2 a_uv;
                out vec2 uv;
                layout(std140) uniform MVP_Matrix {
                    mat4 u_model, u_projection;
                };
                void main() {
                    uv = a_uv;
                    gl_Position = u_projection * u_model * vec4(a_position, 0, 1);
                    }
            )",
            R"(
                precision mediump float;
                in vec2 uv;
                uniform sampler2D u_texture;
                out vec4 o_color;
                void main() {
                    o_color = texture(u_texture, uv);
                }
            )",
        };

        sources.glsl1 = {
            R"(
                attribute vec2 a_position;
                attribute vec2 a_uv;
                varying vec2 uv;
                uniform mat4 u_model, u_projection;

                void main() {
                    uv = a_uv;
                    gl_Position = u_projection * u_model * vec4(a_position, 0, 1);
                }
            )",
            R"(
                precision mediump float;
                varying vec2 uv;
                uniform sampler2D u_texture;
                void main() {
                    gl_FragColor = texture2D(u_texture, uv);
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
            {"a_uv", gfx::Format::RG32F, false, 0, false, 1},
        };
        gfx::UniformList mvpMatrix = {
            {"u_model", gfx::Type::MAT4, 1},
            {"u_projection", gfx::Type::MAT4, 1},
        };
        gfx::UniformBlockList          uniformBlockList = {{0, 0, "MVP_Matrix", mvpMatrix, 1}};
        gfx::UniformSamplerTextureList samplers         = {{0, 1, "u_texture", gfx::Type::SAMPLER2D, 1}};

        gfx::ShaderInfo shaderInfo;
        shaderInfo.name            = "Blend Test: Quad";
        shaderInfo.stages          = std::move(shaderStageList);
        shaderInfo.attributes      = std::move(attributeList);
        shaderInfo.blocks          = std::move(uniformBlockList);
        shaderInfo.samplerTextures = std::move(samplers);
        shader                     = device->createShader(shaderInfo);
    }

    void createVertexBuffer() {
        float vertexData[] = {-0.5f, -0.5f, 0.f, 0.f,
                              -0.5f, 0.5f, 0.f, 1.f,
                              0.5f, 0.5f, 1.f, 1.f,
                              0.5f, -0.5f, 1.f, 0.f};

        unsigned short indices[6] = {0, 3, 1, 1, 3, 2};

        // vertex buffer
        vertexBuffer = device->createBuffer({
            gfx::BufferUsage::VERTEX,
            gfx::MemoryUsage::DEVICE,
            sizeof(vertexData),
            4 * sizeof(float),
        });
        vertexBuffer->update(vertexData, sizeof(vertexData));

        // index buffer
        indexBuffer = device->createBuffer({
            gfx::BufferUsage::INDEX,
            gfx::MemoryUsage::DEVICE,
            sizeof(indices),
            sizeof(unsigned short),
        });
        indexBuffer->update(indices, sizeof(indices));

        // dynamic uniform buffer
        uboStride = TestBaseI::getAlignedUBOStride(sizeof(Mat4) * 2);
        models.resize(uboStride * TOTAL_BLEND / sizeof(float), 0);

        uniformBuffer     = device->createBuffer({
            gfx::BufferUsage::UNIFORM,
            gfx::MemoryUsage::DEVICE | gfx::MemoryUsage::HOST,
            uboStride * TOTAL_BLEND,
            uboStride,
        });
        uniformBufferView = device->createBuffer({
            uniformBuffer,
            0,
            uboStride,
        });
        for (uint i = 0; i < TOTAL_BLEND; i++) {
            dynamicOffsets[i] = i * uboStride;
        }
    }

    void createInputAssembler() {
        gfx::Attribute          position = {"a_position", gfx::Format::RG32F, false, 0, false, 0};
        gfx::Attribute          texcoord = {"a_uv", gfx::Format::RG32F, false, 0, false, 1};
        gfx::InputAssemblerInfo inputAssemblerInfo;
        inputAssemblerInfo.attributes.emplace_back(std::move(position));
        inputAssemblerInfo.attributes.emplace_back(std::move(texcoord));
        inputAssemblerInfo.vertexBuffers.emplace_back(vertexBuffer);
        inputAssemblerInfo.indexBuffer = indexBuffer;
        inputAssembler                 = device->createInputAssembler(inputAssemblerInfo);
    }

    void createTexture() {
        gfx::TextureInfo textureInfo;
        textureInfo.usage  = gfx::TextureUsage::SAMPLED | gfx::TextureUsage::TRANSFER_DST;
        textureInfo.format = gfx::Format::RGBA8;
        texture            = TestBaseI::createTextureWithFile(textureInfo, "sprite0.png");

        // create sampler
        gfx::SamplerInfo samplerInfo;
        samplerInfo.addressU = gfx::Address::CLAMP;
        samplerInfo.addressV = gfx::Address::CLAMP;
        samplerInfo.addressW = gfx::Address::CLAMP;
        sampler              = device->createSampler(samplerInfo);
    }

    void createPipeline() {
        gfx::DescriptorSetLayoutInfo dslInfo;
        dslInfo.bindings.push_back({0, gfx::DescriptorType::DYNAMIC_UNIFORM_BUFFER, 1, gfx::ShaderStageFlagBit::VERTEX});
        dslInfo.bindings.push_back({1, gfx::DescriptorType::SAMPLER_TEXTURE, 1, gfx::ShaderStageFlagBit::FRAGMENT});
        descriptorSetLayout = device->createDescriptorSetLayout(dslInfo);

        pipelineLayout = device->createPipelineLayout({{descriptorSetLayout}});

        descriptorSet = device->createDescriptorSet({descriptorSetLayout});

        descriptorSet->bindBuffer(0, uniformBufferView);
        descriptorSet->bindSampler(1, sampler);
        descriptorSet->bindTexture(1, texture);
        descriptorSet->update();

        gfx::PipelineStateInfo pipelineInfo[TOTAL_BLEND];
        pipelineInfo[NO_BLEND].primitive                           = gfx::PrimitiveMode::TRIANGLE_LIST;
        pipelineInfo[NO_BLEND].shader                              = shader;
        pipelineInfo[NO_BLEND].inputState                          = {inputAssembler->getAttributes()};
        pipelineInfo[NO_BLEND].renderPass                          = fbo->getRenderPass();
        pipelineInfo[NO_BLEND].rasterizerState.cullMode            = gfx::CullMode::NONE;
        pipelineInfo[NO_BLEND].depthStencilState.depthWrite        = false;
        pipelineInfo[NO_BLEND].blendState.targets[0].blend         = true;
        pipelineInfo[NO_BLEND].blendState.targets[0].blendEq       = gfx::BlendOp::ADD;
        pipelineInfo[NO_BLEND].blendState.targets[0].blendAlphaEq  = gfx::BlendOp::ADD;
        pipelineInfo[NO_BLEND].blendState.targets[0].blendSrc      = gfx::BlendFactor::ONE;
        pipelineInfo[NO_BLEND].blendState.targets[0].blendDst      = gfx::BlendFactor::ZERO;
        pipelineInfo[NO_BLEND].blendState.targets[0].blendSrcAlpha = gfx::BlendFactor::ONE;
        pipelineInfo[NO_BLEND].blendState.targets[0].blendDstAlpha = gfx::BlendFactor::ONE_MINUS_SRC_ALPHA;
        pipelineInfo[NO_BLEND].pipelineLayout                      = pipelineLayout;
        pipelineState[NO_BLEND]                                    = device->createPipelineState(pipelineInfo[NO_BLEND]);

        pipelineInfo[NORMAL_BLEND].primitive                    = gfx::PrimitiveMode::TRIANGLE_LIST;
        pipelineInfo[NORMAL_BLEND].shader                       = shader;
        pipelineInfo[NORMAL_BLEND].inputState                   = {inputAssembler->getAttributes()};
        pipelineInfo[NORMAL_BLEND].renderPass                   = fbo->getRenderPass();
        pipelineInfo[NORMAL_BLEND].rasterizerState.cullMode     = gfx::CullMode::NONE;
        pipelineInfo[NORMAL_BLEND].depthStencilState.depthWrite = false;

        pipelineInfo[NORMAL_BLEND].blendState.targets[0].blend         = true;
        pipelineInfo[NORMAL_BLEND].blendState.targets[0].blendEq       = gfx::BlendOp::ADD;
        pipelineInfo[NORMAL_BLEND].blendState.targets[0].blendAlphaEq  = gfx::BlendOp::ADD;
        pipelineInfo[NORMAL_BLEND].blendState.targets[0].blendSrc      = gfx::BlendFactor::SRC_ALPHA;
        pipelineInfo[NORMAL_BLEND].blendState.targets[0].blendDst      = gfx::BlendFactor::ONE_MINUS_SRC_ALPHA;
        pipelineInfo[NORMAL_BLEND].blendState.targets[0].blendSrcAlpha = gfx::BlendFactor::ONE;
        pipelineInfo[NORMAL_BLEND].blendState.targets[0].blendDstAlpha = gfx::BlendFactor::ONE_MINUS_SRC_ALPHA;
        pipelineInfo[NORMAL_BLEND].pipelineLayout                      = pipelineLayout;
        pipelineState[NORMAL_BLEND]                                    = device->createPipelineState(pipelineInfo[NORMAL_BLEND]);

        pipelineInfo[ADDITIVE_BLEND].blendState.targets[0].blend         = true;
        pipelineInfo[ADDITIVE_BLEND].blendState.targets[0].blendEq       = gfx::BlendOp::ADD;
        pipelineInfo[ADDITIVE_BLEND].blendState.targets[0].blendAlphaEq  = gfx::BlendOp::ADD;
        pipelineInfo[ADDITIVE_BLEND].blendState.targets[0].blendSrc      = gfx::BlendFactor::SRC_ALPHA;
        pipelineInfo[ADDITIVE_BLEND].blendState.targets[0].blendDst      = gfx::BlendFactor::ONE;
        pipelineInfo[ADDITIVE_BLEND].blendState.targets[0].blendSrcAlpha = gfx::BlendFactor::ONE;
        pipelineInfo[ADDITIVE_BLEND].blendState.targets[0].blendDstAlpha = gfx::BlendFactor::ONE_MINUS_SRC_ALPHA;
        pipelineInfo[ADDITIVE_BLEND].primitive                           = gfx::PrimitiveMode::TRIANGLE_LIST;
        pipelineInfo[ADDITIVE_BLEND].shader                              = shader;
        pipelineInfo[ADDITIVE_BLEND].inputState                          = {inputAssembler->getAttributes()};
        pipelineInfo[ADDITIVE_BLEND].renderPass                          = fbo->getRenderPass();
        pipelineInfo[ADDITIVE_BLEND].rasterizerState.cullMode            = gfx::CullMode::NONE;
        pipelineInfo[ADDITIVE_BLEND].depthStencilState.depthWrite        = false;
        pipelineInfo[ADDITIVE_BLEND].pipelineLayout                      = pipelineLayout;
        pipelineState[ADDITIVE_BLEND]                                    = device->createPipelineState(pipelineInfo[ADDITIVE_BLEND]);

        pipelineInfo[SUBSTRACT_BLEND].blendState.targets[0].blend         = true;
        pipelineInfo[SUBSTRACT_BLEND].blendState.targets[0].blendEq       = gfx::BlendOp::ADD;
        pipelineInfo[SUBSTRACT_BLEND].blendState.targets[0].blendAlphaEq  = gfx::BlendOp::ADD;
        pipelineInfo[SUBSTRACT_BLEND].blendState.targets[0].blendSrc      = gfx::BlendFactor::ZERO;
        pipelineInfo[SUBSTRACT_BLEND].blendState.targets[0].blendDst      = gfx::BlendFactor::ONE_MINUS_SRC_COLOR;
        pipelineInfo[SUBSTRACT_BLEND].blendState.targets[0].blendSrcAlpha = gfx::BlendFactor::ONE;
        pipelineInfo[SUBSTRACT_BLEND].blendState.targets[0].blendDstAlpha = gfx::BlendFactor::ONE_MINUS_SRC_ALPHA;
        pipelineInfo[SUBSTRACT_BLEND].primitive                           = gfx::PrimitiveMode::TRIANGLE_LIST;
        pipelineInfo[SUBSTRACT_BLEND].shader                              = shader;
        pipelineInfo[SUBSTRACT_BLEND].inputState                          = {inputAssembler->getAttributes()};
        pipelineInfo[SUBSTRACT_BLEND].renderPass                          = fbo->getRenderPass();
        pipelineInfo[SUBSTRACT_BLEND].rasterizerState.cullMode            = gfx::CullMode::NONE;
        pipelineInfo[SUBSTRACT_BLEND].depthStencilState.depthWrite        = false;
        pipelineInfo[SUBSTRACT_BLEND].pipelineLayout                      = pipelineLayout;
        pipelineState[SUBSTRACT_BLEND]                                    = device->createPipelineState(pipelineInfo[SUBSTRACT_BLEND]);

        pipelineInfo[MULTIPLY_BLEND].blendState.targets[0].blend         = true;
        pipelineInfo[MULTIPLY_BLEND].blendState.targets[0].blendEq       = gfx::BlendOp::ADD;
        pipelineInfo[MULTIPLY_BLEND].blendState.targets[0].blendAlphaEq  = gfx::BlendOp::ADD;
        pipelineInfo[MULTIPLY_BLEND].blendState.targets[0].blendSrc      = gfx::BlendFactor::ZERO;
        pipelineInfo[MULTIPLY_BLEND].blendState.targets[0].blendDst      = gfx::BlendFactor::SRC_COLOR;
        pipelineInfo[MULTIPLY_BLEND].blendState.targets[0].blendSrcAlpha = gfx::BlendFactor::ONE;
        pipelineInfo[MULTIPLY_BLEND].blendState.targets[0].blendDstAlpha = gfx::BlendFactor::ONE_MINUS_SRC_ALPHA;
        pipelineInfo[MULTIPLY_BLEND].primitive                           = gfx::PrimitiveMode::TRIANGLE_LIST;
        pipelineInfo[MULTIPLY_BLEND].shader                              = shader;
        pipelineInfo[MULTIPLY_BLEND].inputState                          = {inputAssembler->getAttributes()};
        pipelineInfo[MULTIPLY_BLEND].renderPass                          = fbo->getRenderPass();
        pipelineInfo[MULTIPLY_BLEND].rasterizerState.cullMode            = gfx::CullMode::NONE;
        pipelineInfo[MULTIPLY_BLEND].depthStencilState.depthWrite        = false;
        pipelineInfo[MULTIPLY_BLEND].pipelineLayout                      = pipelineLayout;
        pipelineState[MULTIPLY_BLEND]                                    = device->createPipelineState(pipelineInfo[MULTIPLY_BLEND]);
    }

    gfx::Device *             device                     = nullptr;
    gfx::Framebuffer *        fbo                        = nullptr;
    gfx::Shader *             shader                     = nullptr;
    gfx::Buffer *             vertexBuffer               = nullptr;
    gfx::Buffer *             indexBuffer                = nullptr;
    gfx::InputAssembler *     inputAssembler             = nullptr;
    gfx::Sampler *            sampler                    = nullptr;
    gfx::Texture *            texture                    = nullptr;
    gfx::DescriptorSetLayout *descriptorSetLayout        = nullptr;
    gfx::PipelineLayout *     pipelineLayout             = nullptr;
    gfx::Buffer *             uniformBuffer              = nullptr;
    gfx::Buffer *             uniformBufferView          = nullptr;
    gfx::DescriptorSet *      descriptorSet              = nullptr;
    gfx::PipelineState *      pipelineState[TOTAL_BLEND] = {nullptr};
    uint                      dynamicOffsets[TOTAL_BLEND];

    vector<float> models;
    uint          uboStride;
};

struct BigTriangle : public cc::Object {
    BigTriangle(gfx::Device *device, gfx::Framebuffer *fbo) : device(device), fbo(fbo) {
        createShader();
        createVertexBuffer();
        createInputAssembler();
        createTexture();
        createPipeline();
    }

    void destroy() {
        CC_SAFE_DESTROY(shader);
        CC_SAFE_DESTROY(vertexBuffer);
        CC_SAFE_DESTROY(inputAssembler);
        CC_SAFE_DESTROY(descriptorSet);
        CC_SAFE_DESTROY(descriptorSetLayout);
        CC_SAFE_DESTROY(pipelineLayout);
        CC_SAFE_DESTROY(pipelineState);
        CC_SAFE_DESTROY(timeBuffer);
        CC_SAFE_DESTROY(texture);
        CC_SAFE_DESTROY(sampler);
    }

    void createShader() {

        ShaderSources<StandardShaderSource> sources;
        sources.glsl4 = {
            R"(
                precision highp float;
                layout(location = 0) in vec2 a_position;
                layout(location = 1) in vec2 a_texCoord;
                layout(location = 0) out vec2 uv;
                void main() {
                    uv = a_texCoord;
                    gl_Position = vec4(a_position, 0.9, 1);
                }
            )",
            R"(
                precision highp float;
                layout(location = 0) in vec2 uv;
                layout(set = 0, binding = 0) uniform Time {
                    float u_time;
                };
                layout(set = 0, binding = 1) uniform sampler2D u_texture;
                layout(location = 0) out vec4 o_color;
                void main() {
                    vec2 offset = vec2(u_time * -0.01);
                    o_color = texture(u_texture, 20.0 * (uv + offset));
                }
            )",
        };

        sources.glsl3 = {
            R"(
                in vec2 a_position;
                in vec2 a_texCoord;
                out vec2 uv;
                void main() {
                    uv = a_texCoord;
                    gl_Position = vec4(a_position, 0.9, 1);
                }
            )",
            R"(
                precision mediump float;
                in vec2 uv;
                uniform sampler2D u_texture;
                layout(std140) uniform Time {
                    float u_time;
                };
                out vec4 o_color;
                void main() {
                    vec2 offset = vec2(u_time * -0.01);
                    o_color = texture(u_texture, 20.0 * (uv + offset));
                }
            )",
        };

        sources.glsl1 = {
            R"(
                attribute vec2 a_position;
                attribute vec2 a_texCoord;
                varying vec2 uv;
                void main() {
                    uv = a_texCoord;
                    gl_Position = vec4(a_position, 0.9, 1);
                }
            )",
            R"(
                precision mediump float;
                varying vec2 uv;
                uniform sampler2D u_texture;
                uniform float u_time;

                void main() {
                    vec2 offset = vec2(u_time * -0.01);
                    gl_FragColor = texture2D(u_texture, 20.0 * (uv + offset));
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
        gfx::UniformList               time             = {{"u_time", gfx::Type::FLOAT, 1}};
        gfx::UniformBlockList          uniformBlockList = {{0, 0, "Time", time, 1}};
        gfx::UniformSamplerTextureList samplers         = {{0, 1, "u_texture", gfx::Type::SAMPLER2D, 1}};

        gfx::ShaderInfo shaderInfo;
        shaderInfo.name            = "Blend Test: BigTriangle";
        shaderInfo.stages          = std::move(shaderStageList);
        shaderInfo.attributes      = std::move(attributeList);
        shaderInfo.blocks          = std::move(uniformBlockList);
        shaderInfo.samplerTextures = std::move(samplers);
        shader                     = device->createShader(shaderInfo);
    }

    void createVertexBuffer() {
        float ySign        = device->getCapabilities().screenSpaceSignY;
        float vertexData[] = {-1.0f, 4.0f * ySign, 0.0, -1.5,
                              -1.0f, -1.0f * ySign, 0.0, 1.0,
                              4.0f, -1.0f * ySign, 2.5, 1.0};

        // vertex buffer
        vertexBuffer = device->createBuffer({
            gfx::BufferUsage::VERTEX | gfx::BufferUsage::TRANSFER_DST,
            gfx::MemoryUsage::HOST,
            sizeof(vertexData),
            4 * sizeof(float),
        });
        vertexBuffer->update(vertexData, sizeof(vertexData));

        // uniform buffer
        timeBuffer = device->createBuffer({
            gfx::BufferUsage::UNIFORM,
            gfx::MemoryUsage::HOST | gfx::MemoryUsage::DEVICE,
            TestBaseI::getUBOSize(sizeof(float)),
        });
    }

    void createInputAssembler() {
        gfx::InputAssemblerInfo inputAssemblerInfo;
        inputAssemblerInfo.attributes.push_back({"a_position", gfx::Format::RG32F, false, 0, false});
        inputAssemblerInfo.attributes.push_back({"a_texCoord", gfx::Format::RG32F, false, 0, false});
        inputAssemblerInfo.vertexBuffers.emplace_back(vertexBuffer);
        inputAssembler = device->createInputAssembler(inputAssemblerInfo);
    }

    void createTexture() {
        gfx::TextureInfo textureInfo;
        textureInfo.usage  = gfx::TextureUsage::SAMPLED | gfx::TextureUsage::TRANSFER_DST;
        textureInfo.format = gfx::Format::RGBA8;
        textureInfo.flags  = gfx::TextureFlagBit::GEN_MIPMAP;
        texture            = TestBaseI::createTextureWithFile(textureInfo, "background.png");

        // create sampler
        gfx::SamplerInfo samplerInfo;
        samplerInfo.mipFilter = gfx::Filter::LINEAR;
        samplerInfo.addressU  = gfx::Address::WRAP;
        samplerInfo.addressV  = gfx::Address::WRAP;
        sampler               = device->createSampler(samplerInfo);
    }

    void createPipeline() {
        gfx::DescriptorSetLayoutInfo dslInfo;
        dslInfo.bindings.push_back({0, gfx::DescriptorType::UNIFORM_BUFFER, 1, gfx::ShaderStageFlagBit::FRAGMENT});
        dslInfo.bindings.push_back({1, gfx::DescriptorType::SAMPLER_TEXTURE, 1, gfx::ShaderStageFlagBit::FRAGMENT});
        descriptorSetLayout = device->createDescriptorSetLayout(dslInfo);

        pipelineLayout = device->createPipelineLayout({{descriptorSetLayout}});

        descriptorSet = device->createDescriptorSet({descriptorSetLayout});

        descriptorSet->bindBuffer(0, timeBuffer);
        descriptorSet->bindSampler(1, sampler);
        descriptorSet->bindTexture(1, texture);
        descriptorSet->update();

        gfx::PipelineStateInfo pipelineInfo;
        pipelineInfo.primitive      = gfx::PrimitiveMode::TRIANGLE_LIST;
        pipelineInfo.shader         = shader;
        pipelineInfo.inputState     = {inputAssembler->getAttributes()};
        pipelineInfo.renderPass     = fbo->getRenderPass();
        pipelineInfo.pipelineLayout = pipelineLayout;

        pipelineState = device->createPipelineState(pipelineInfo);
    }

    gfx::Device *             device              = nullptr;
    gfx::Framebuffer *        fbo                 = nullptr;
    gfx::Shader *             shader              = nullptr;
    gfx::Buffer *             vertexBuffer        = nullptr;
    gfx::Buffer *             timeBuffer          = nullptr;
    gfx::InputAssembler *     inputAssembler      = nullptr;
    gfx::Sampler *            sampler             = nullptr;
    gfx::Texture *            texture             = nullptr;
    gfx::DescriptorSet *      descriptorSet       = nullptr;
    gfx::DescriptorSetLayout *descriptorSetLayout = nullptr;
    gfx::PipelineLayout *     pipelineLayout      = nullptr;
    gfx::PipelineState *      pipelineState       = nullptr;
};

void createModelTransform(Mat4 &model, const Vec3 &t, const Vec3 &s) {
    Mat4::createTranslation(t, &model);
    model.scale(s);
}

BigTriangle *         bigTriangle = nullptr;
Quad *                quad        = nullptr;
gfx::SurfaceTransform orientation = gfx::SurfaceTransform::IDENTITY;
gfx::Color            clearColor  = {0, 0, 0, 1};
gfx::Rect             renderArea;
} // namespace

void BlendTest::onDestroy() {
    _textures.clear();
    CC_SAFE_DESTROY(bigTriangle);
    CC_SAFE_DESTROY(quad);
    renderArea.width = renderArea.height = 1u;
    orientation                          = gfx::SurfaceTransform::IDENTITY;
}

bool BlendTest::onInit() {
    bigTriangle = CC_NEW(BigTriangle(device, fbo));
    quad        = CC_NEW(Quad(device, fbo));

    _globalBarriers.push_back(TestBaseI::getGlobalBarrier({
        {
            gfx::AccessType::TRANSFER_WRITE,
        },
        {
            gfx::AccessType::FRAGMENT_SHADER_READ_UNIFORM_BUFFER,
            gfx::AccessType::VERTEX_SHADER_READ_UNIFORM_BUFFER,
            gfx::AccessType::VERTEX_BUFFER,
            gfx::AccessType::INDEX_BUFFER,
        },
    }));

    _globalBarriers.push_back(TestBaseI::getGlobalBarrier({
        {
            gfx::AccessType::TRANSFER_WRITE,
        },
        {
            gfx::AccessType::FRAGMENT_SHADER_READ_UNIFORM_BUFFER,
            gfx::AccessType::VERTEX_SHADER_READ_UNIFORM_BUFFER,
        },
    }));

    _textureBarriers.push_back(TestBaseI::getTextureBarrier({
        {
            gfx::AccessType::TRANSFER_WRITE,
        },
        {
            gfx::AccessType::FRAGMENT_SHADER_READ_TEXTURE,
        },
        false,
    }));

    _textureBarriers.push_back(_textureBarriers.back());

    _textures.assign({bigTriangle->texture, quad->texture});

    return true;
}

void BlendTest::onTick() {
    uint globalBarrierIdx = _frameCount ? 1 : 0;
    uint textureBarriers  = _frameCount ? 0 : _textureBarriers.size();

    device->acquire();

    gfx::Extent orientedSize  = TestBaseI::getOrientedSurfaceSize();
    bool        matricesDirty = renderArea.width != orientedSize.width || renderArea.height != orientedSize.height || device->getSurfaceTransform() != orientation;

    if (matricesDirty) {
        Mat4 model;
        Mat4 projection;
        TestBaseI::createOrthographic(0.f, (float)orientedSize.width, (float)orientedSize.height, 0.f, -1.0f, 1.f, &projection);

        float size     = std::min(orientedSize.width, orientedSize.height) * 0.15f;
        float halfSize = size * 0.5f;
        float offsetX  = 5.f + halfSize;
        float offsetY  = 50.f + halfSize;

        for (uint i = 0; i < TOTAL_BLEND; i++) {
            createModelTransform(model, Vec3(offsetX, offsetY, 0), Vec3(size, size, 1));
            memcpy(quad->models.data() + (quad->uboStride * i) / sizeof(float), model.m, sizeof(model));
            memcpy(quad->models.data() + (quad->uboStride * i + sizeof(Mat4)) / sizeof(float), projection.m, sizeof(projection));
            offsetY += 5.f + size;
        }
        // render area is not oriented
        renderArea.width  = device->getWidth();
        renderArea.height = device->getHeight();
        orientation       = device->getSurfaceTransform();
    }

    auto commandBuffer = commandBuffers[0];
    commandBuffer->begin();

    commandBuffer->updateBuffer(bigTriangle->timeBuffer, &_time, sizeof(_time));

    if (matricesDirty)
        commandBuffer->updateBuffer(quad->uniformBuffer, quad->models.data(), quad->models.size() * sizeof(float));

    if (TestBaseI::MANUAL_BARRIER)
        commandBuffer->pipelineBarrier(_globalBarriers[globalBarrierIdx], _textureBarriers.data(), _textures.data(), textureBarriers);

    commandBuffer->beginRenderPass(fbo->getRenderPass(), fbo, renderArea, &clearColor, 1.0f, 0);
    // draw background
    commandBuffer->bindInputAssembler(bigTriangle->inputAssembler);
    commandBuffer->bindPipelineState(bigTriangle->pipelineState);
    commandBuffer->bindDescriptorSet(0, bigTriangle->descriptorSet);
    commandBuffer->draw(bigTriangle->inputAssembler);

    commandBuffer->bindInputAssembler(quad->inputAssembler);

    // draw sprite without blending
    commandBuffer->bindPipelineState(quad->pipelineState[NO_BLEND]);
    commandBuffer->bindDescriptorSet(0, quad->descriptorSet, 1, &quad->dynamicOffsets[NO_BLEND]);
    commandBuffer->draw(quad->inputAssembler);

    // normal
    commandBuffer->bindPipelineState(quad->pipelineState[NORMAL_BLEND]);
    commandBuffer->bindDescriptorSet(0, quad->descriptorSet, 1, &quad->dynamicOffsets[NORMAL_BLEND]);
    commandBuffer->draw(quad->inputAssembler);

    // additive
    commandBuffer->bindPipelineState(quad->pipelineState[ADDITIVE_BLEND]);
    commandBuffer->bindDescriptorSet(0, quad->descriptorSet, 1, &quad->dynamicOffsets[ADDITIVE_BLEND]);
    commandBuffer->draw(quad->inputAssembler);

    // substract
    commandBuffer->bindPipelineState(quad->pipelineState[SUBSTRACT_BLEND]);
    commandBuffer->bindDescriptorSet(0, quad->descriptorSet, 1, &quad->dynamicOffsets[SUBSTRACT_BLEND]);
    commandBuffer->draw(quad->inputAssembler);

    // multiply
    commandBuffer->bindPipelineState(quad->pipelineState[MULTIPLY_BLEND]);
    commandBuffer->bindDescriptorSet(0, quad->descriptorSet, 1, &quad->dynamicOffsets[MULTIPLY_BLEND]);
    commandBuffer->draw(quad->inputAssembler);

    commandBuffer->endRenderPass();
    commandBuffer->end();

    device->flushCommands(commandBuffers);
    device->getQueue()->submit(commandBuffers);
    device->present();
}

} // namespace cc
