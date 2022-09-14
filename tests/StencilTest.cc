#include "StencilTest.h"

namespace cc {

namespace {
enum class PipelineType : uint8_t {
    STENCIL,
    IMAGE,
    CANVAS,
    FRONT_STENCIL,
    BACK_STENCIL,
    FRONT_BACK_STENCIL,
    COUNT
};
} // namespace

void StencilTest::onDestroy() {
    CC_SAFE_DESTROY_AND_DELETE(_shader);
    CC_SAFE_DESTROY_AND_DELETE(_vertexBuffer);
    CC_SAFE_DESTROY_AND_DELETE(_inputAssembler);
    for (uint i = 0; i < BINDING_COUNT; i++) {
        CC_SAFE_DESTROY_AND_DELETE(_uniformBuffer[i]);
        CC_SAFE_DESTROY_AND_DELETE(_descriptorSet[i]);
    }
    CC_SAFE_DESTROY_AND_DELETE(_descriptorSetLayout);
    CC_SAFE_DESTROY_AND_DELETE(_pipelineLayout);
    for (auto &i : _pipelineState) {
        CC_SAFE_DESTROY_AND_DELETE(i);
    }
}

bool StencilTest::onInit() {
    createShader();
    createBuffers();
    createTextures();
    createInputAssembler();
    createPipelineState();
    return true;
}

void StencilTest::createShader() {
    ShaderSources<StandardShaderSource> sources;
    sources.glsl4 = {
        R"(
            precision highp float;
            layout(location = 0) in vec2 a_position;
            layout(location = 1) in vec2 a_texCoord;
            layout(set = 0, binding = 0) uniform World_Matrix {
                mat4 u_worldMatrix;
                mat4 u_projMatrix;
            };
            layout(location = 0) out vec2 v_texCoord;
            void main () {
                v_texCoord = a_texCoord;
                gl_Position = u_projMatrix * u_worldMatrix * vec4(a_position, 0, 1);
            }
        )",
        R"(
            precision highp float;
            layout(set = 0, binding = 1) uniform sampler2D u_texture;
            layout(location = 0) in vec2 v_texCoord;
            layout(location = 0) out vec4 o_color;
            void main () {
                o_color = texture(u_texture, v_texCoord);
            }
        )",
    };

    sources.glsl3 = {
        R"(
            in vec2 a_position;
            in vec2 a_texCoord;
            layout(std140) uniform World_Matrix {
                mat4 u_worldMatrix;
                mat4 u_projMatrix;
            };
            out vec2 v_texCoord;
            void main () {
                v_texCoord = a_texCoord;
                gl_Position = u_projMatrix * u_worldMatrix * vec4(a_position, 0, 1);
            }
        )",
        R"(
            precision mediump float;
            uniform sampler2D u_texture;
            in vec2 v_texCoord;
            out vec4 o_color;
            void main () {
                o_color = texture(u_texture, v_texCoord);
            }
        )",
    };

    sources.glsl1 = {
        R"(
            attribute vec2 a_position;
            attribute vec2 a_texCoord;
            varying vec2 v_texCoord;

            uniform mat4 u_worldMatrix;
            uniform mat4 u_projMatrix;

            void main () {
                v_texCoord = a_texCoord;
                gl_Position = u_projMatrix * u_worldMatrix * vec4(a_position, 0, 1);
            }
        )",
        R"(
            precision mediump float;
            uniform sampler2D u_texture;
            varying vec2 v_texCoord;
            void main () {
                gl_FragColor = texture2D(u_texture, v_texCoord);
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
    gfx::UniformList               worldMatrix      = {{"u_worldMatrix", gfx::Type::MAT4, 1}, {"u_projMatrix", gfx::Type::MAT4, 1}};
    gfx::UniformBlockList          uniformBlockList = {{0, 0, "World_Matrix", worldMatrix, 1}};
    gfx::UniformSamplerTextureList sampler          = {{0, 1, "u_texture", gfx::Type::SAMPLER2D, 1}};

    gfx::ShaderInfo shaderInfo;
    shaderInfo.name            = "Stencil Test";
    shaderInfo.stages          = std::move(shaderStageList);
    shaderInfo.attributes      = std::move(attributeList);
    shaderInfo.blocks          = std::move(uniformBlockList);
    shaderInfo.samplerTextures = std::move(sampler);
    _shader                    = device->createShader(shaderInfo);
}

void StencilTest::createBuffers() {
    float vertexData[] = {-1.0F, -1.0F, 0.0F, 1.0F,
                          1.0F, -1.0F, 1.0F, 1.0F,
                          1.0F, 1.0F, 1.0F, 0.0F,
                          1.0F, 1.0F, 1.0F, 0.0F,
                          -1.0F, -1.0F, 0.0F, 1.0F,
                          -1.0F, 1.0F, 0.0F, 0.0F};

    _vertexBuffer = device->createBuffer({
        gfx::BufferUsage::VERTEX,
        gfx::MemoryUsage::DEVICE,
        sizeof(vertexData),
        4 * sizeof(float),
    });
    _vertexBuffer->update(vertexData, sizeof(vertexData));

    gfx::BufferInfo uniformBufferInfo = {
        gfx::BufferUsage::UNIFORM,
        gfx::MemoryUsage::HOST | gfx::MemoryUsage::DEVICE,
        TestBaseI::getUBOSize(sizeof(Mat4) * 2),
    };

    _uboData[0].world.scale(0.5F);
    _uboData[0].world.rotateZ(cc::math::PI / 4);
    _uboData[1].world.scale(0.5F, 0.5F, 0.25F);

    for (auto &i : _uniformBuffer) {
        i = device->createBuffer(uniformBufferInfo);
    }
}

void StencilTest::createTextures() {
    gfx::TextureInfo textureInfo;
    textureInfo.usage = gfx::TextureUsage::SAMPLED | gfx::TextureUsage::TRANSFER_DST;

    _textures.resize(2);
    _textures[0] = TestBaseI::createTextureWithFile(textureInfo, "stencil.jpg");
    _textures[1] = TestBaseI::createTextureWithFile(textureInfo, "uv_checker_02.jpg");
}

void StencilTest::createInputAssembler() {
    gfx::InputAssemblerInfo inputAssemblerInfo;
    inputAssemblerInfo.attributes.push_back({"a_position", gfx::Format::RG32F, false, 0, false});
    inputAssemblerInfo.attributes.push_back({"a_texCoord", gfx::Format::RG32F, false, 0, false});
    inputAssemblerInfo.vertexBuffers.emplace_back(_vertexBuffer);
    _inputAssembler = device->createInputAssembler(inputAssemblerInfo);
}

void StencilTest::createPipelineState() {
    gfx::DescriptorSetLayoutInfo dslInfo;
    dslInfo.bindings.push_back({0, gfx::DescriptorType::UNIFORM_BUFFER, 1, gfx::ShaderStageFlagBit::VERTEX});
    dslInfo.bindings.push_back({1, gfx::DescriptorType::SAMPLER_TEXTURE, 1, gfx::ShaderStageFlagBit::FRAGMENT});
    _descriptorSetLayout = device->createDescriptorSetLayout(dslInfo);

    _pipelineLayout = device->createPipelineLayout({{_descriptorSetLayout}});

    gfx::SamplerInfo samplerInfo;
    auto *           sampler = device->getSampler(samplerInfo);

    for (uint i = 0; i < BINDING_COUNT; i++) {
        _descriptorSet[i] = device->createDescriptorSet({_descriptorSetLayout});
        _descriptorSet[i]->bindBuffer(0, _uniformBuffer[i]);
        _descriptorSet[i]->bindSampler(1, sampler);
        _descriptorSet[i]->bindTexture(1, _textures[i]);
        _descriptorSet[i]->update();
    }

    gfx::PipelineStateInfo pipelineInfo[PIPELIE_COUNT];
    pipelineInfo[static_cast<uint8_t>(PipelineType::STENCIL)].primitive                          = gfx::PrimitiveMode::TRIANGLE_LIST;
    pipelineInfo[static_cast<uint8_t>(PipelineType::STENCIL)].shader                             = _shader;
    pipelineInfo[static_cast<uint8_t>(PipelineType::STENCIL)].inputState                         = {_inputAssembler->getAttributes()};
    pipelineInfo[static_cast<uint8_t>(PipelineType::STENCIL)].renderPass                         = renderPass;
    pipelineInfo[static_cast<uint8_t>(PipelineType::STENCIL)].depthStencilState.depthTest        = false;
    pipelineInfo[static_cast<uint8_t>(PipelineType::STENCIL)].depthStencilState.stencilTestFront = false;
    pipelineInfo[static_cast<uint8_t>(PipelineType::STENCIL)].depthStencilState.stencilTestBack  = false;
    pipelineInfo[static_cast<uint8_t>(PipelineType::STENCIL)].rasterizerState.cullMode           = gfx::CullMode::NONE;
    pipelineInfo[static_cast<uint8_t>(PipelineType::STENCIL)].pipelineLayout                     = _pipelineLayout;
    _pipelineState[static_cast<uint8_t>(PipelineType::STENCIL)]                                  = device->createPipelineState(pipelineInfo[static_cast<uint8_t>(PipelineType::STENCIL)]);

    pipelineInfo[static_cast<uint8_t>(PipelineType::IMAGE)].primitive                          = gfx::PrimitiveMode::TRIANGLE_LIST;
    pipelineInfo[static_cast<uint8_t>(PipelineType::IMAGE)].shader                             = _shader;
    pipelineInfo[static_cast<uint8_t>(PipelineType::IMAGE)].inputState                         = {_inputAssembler->getAttributes()};
    pipelineInfo[static_cast<uint8_t>(PipelineType::IMAGE)].renderPass                         = renderPass;
    pipelineInfo[static_cast<uint8_t>(PipelineType::IMAGE)].depthStencilState.depthTest        = false;
    pipelineInfo[static_cast<uint8_t>(PipelineType::IMAGE)].depthStencilState.stencilTestFront = false;
    pipelineInfo[static_cast<uint8_t>(PipelineType::IMAGE)].depthStencilState.stencilTestBack  = false;
    pipelineInfo[static_cast<uint8_t>(PipelineType::IMAGE)].rasterizerState.cullMode           = gfx::CullMode::NONE;
    pipelineInfo[static_cast<uint8_t>(PipelineType::IMAGE)].pipelineLayout                     = _pipelineLayout;
    _pipelineState[static_cast<uint8_t>(PipelineType::IMAGE)]                                  = device->createPipelineState(pipelineInfo[static_cast<uint8_t>(PipelineType::IMAGE)]);

    pipelineInfo[static_cast<uint8_t>(PipelineType::CANVAS)].primitive  = gfx::PrimitiveMode::TRIANGLE_LIST;
    pipelineInfo[static_cast<uint8_t>(PipelineType::CANVAS)].shader     = _shader;
    pipelineInfo[static_cast<uint8_t>(PipelineType::CANVAS)].inputState = {_inputAssembler->getAttributes()};
    pipelineInfo[static_cast<uint8_t>(PipelineType::CANVAS)].renderPass = renderPass;
    gfx::DepthStencilState &dss2                                        = pipelineInfo[static_cast<uint8_t>(PipelineType::CANVAS)].depthStencilState;
    dss2.depthTest                                                      = true;
    dss2.depthWrite                                                     = false;
    dss2.stencilTestFront = dss2.stencilTestBack = true;
    dss2.stencilFuncFront = dss2.stencilFuncBack = gfx::ComparisonFunc::NEVER;
    dss2.stencilReadMaskFront = dss2.stencilReadMaskBack = 0xFF;
    dss2.stencilWriteMaskFront = dss2.stencilWriteMaskBack = 0xFF;
    dss2.stencilFailOpFront = dss2.stencilFailOpBack = gfx::StencilOp::REPLACE;
    dss2.stencilZFailOpFront = dss2.stencilZFailOpBack = gfx::StencilOp::KEEP;
    dss2.stencilPassOpFront = dss2.stencilPassOpBack = gfx::StencilOp::KEEP;
    dss2.stencilRefFront = dss2.stencilRefBack                                        = 0x1;
    pipelineInfo[static_cast<uint8_t>(PipelineType::CANVAS)].rasterizerState.cullMode = gfx::CullMode::NONE;
    pipelineInfo[static_cast<uint8_t>(PipelineType::CANVAS)].pipelineLayout           = _pipelineLayout;
    _pipelineState[static_cast<uint8_t>(PipelineType::CANVAS)]                        = device->createPipelineState(pipelineInfo[static_cast<uint8_t>(PipelineType::CANVAS)]);

    pipelineInfo[static_cast<uint8_t>(PipelineType::FRONT_STENCIL)].primitive  = gfx::PrimitiveMode::TRIANGLE_LIST;
    pipelineInfo[static_cast<uint8_t>(PipelineType::FRONT_STENCIL)].shader     = _shader;
    pipelineInfo[static_cast<uint8_t>(PipelineType::FRONT_STENCIL)].inputState = {_inputAssembler->getAttributes()};
    pipelineInfo[static_cast<uint8_t>(PipelineType::FRONT_STENCIL)].renderPass = renderPass;
    gfx::DepthStencilState &dss3                                               = pipelineInfo[static_cast<uint8_t>(PipelineType::FRONT_STENCIL)].depthStencilState;
    dss3.depthTest                                                             = true;
    dss3.depthWrite                                                            = false;
    dss3.stencilTestFront = dss3.stencilTestBack = true;
    dss3.stencilFuncFront                        = gfx::ComparisonFunc::EQUAL;
    dss3.stencilFuncBack                         = gfx::ComparisonFunc::ALWAYS;
    dss3.stencilReadMaskFront = dss3.stencilReadMaskBack = 0xFF;
    dss3.stencilWriteMaskFront = dss3.stencilWriteMaskBack = 0xFF;
    dss3.stencilFailOpFront = dss3.stencilFailOpBack = gfx::StencilOp::KEEP;
    dss3.stencilZFailOpFront = dss3.stencilZFailOpBack = gfx::StencilOp::KEEP;
    dss3.stencilPassOpFront = dss3.stencilPassOpBack = gfx::StencilOp::KEEP;
    dss3.stencilRefFront = dss3.stencilRefBack                                               = 0x1;
    pipelineInfo[static_cast<uint8_t>(PipelineType::FRONT_STENCIL)].rasterizerState.cullMode = gfx::CullMode::NONE;
    pipelineInfo[static_cast<uint8_t>(PipelineType::FRONT_STENCIL)].pipelineLayout           = _pipelineLayout;
    _pipelineState[static_cast<uint8_t>(PipelineType::FRONT_STENCIL)]                        = device->createPipelineState(pipelineInfo[static_cast<uint8_t>(PipelineType::FRONT_STENCIL)]);

    pipelineInfo[static_cast<uint8_t>(PipelineType::BACK_STENCIL)].primitive  = gfx::PrimitiveMode::TRIANGLE_LIST;
    pipelineInfo[static_cast<uint8_t>(PipelineType::BACK_STENCIL)].shader     = _shader;
    pipelineInfo[static_cast<uint8_t>(PipelineType::BACK_STENCIL)].inputState = {_inputAssembler->getAttributes()};
    pipelineInfo[static_cast<uint8_t>(PipelineType::BACK_STENCIL)].renderPass = renderPass;
    gfx::DepthStencilState &dss4                                              = pipelineInfo[static_cast<uint8_t>(PipelineType::FRONT_STENCIL)].depthStencilState;
    dss4.depthTest                                                            = true;
    dss4.depthWrite                                                           = false;
    dss4.stencilTestFront = dss4.stencilTestBack = true;
    dss4.stencilFuncFront                        = gfx::ComparisonFunc::ALWAYS;
    dss4.stencilFuncBack                         = gfx::ComparisonFunc::EQUAL;
    dss4.stencilReadMaskFront = dss4.stencilReadMaskBack = 0xFF;
    dss4.stencilWriteMaskFront = dss4.stencilWriteMaskBack = 0xFF;
    dss4.stencilFailOpFront = dss4.stencilFailOpBack = gfx::StencilOp::KEEP;
    dss4.stencilZFailOpFront = dss4.stencilZFailOpBack = gfx::StencilOp::KEEP;
    dss4.stencilPassOpFront = dss4.stencilPassOpBack = gfx::StencilOp::KEEP;
    dss4.stencilRefFront = dss4.stencilRefBack                                              = 0x1;
    pipelineInfo[static_cast<uint8_t>(PipelineType::BACK_STENCIL)].rasterizerState.cullMode = gfx::CullMode::NONE;
    pipelineInfo[static_cast<uint8_t>(PipelineType::BACK_STENCIL)].pipelineLayout           = _pipelineLayout;
    _pipelineState[static_cast<uint8_t>(PipelineType::BACK_STENCIL)]                        = device->createPipelineState(pipelineInfo[static_cast<uint8_t>(PipelineType::BACK_STENCIL)]);

    pipelineInfo[static_cast<uint8_t>(PipelineType::FRONT_BACK_STENCIL)].primitive  = gfx::PrimitiveMode::TRIANGLE_LIST;
    pipelineInfo[static_cast<uint8_t>(PipelineType::FRONT_BACK_STENCIL)].shader     = _shader;
    pipelineInfo[static_cast<uint8_t>(PipelineType::FRONT_BACK_STENCIL)].inputState = {_inputAssembler->getAttributes()};
    pipelineInfo[static_cast<uint8_t>(PipelineType::FRONT_BACK_STENCIL)].renderPass = renderPass;
    gfx::DepthStencilState &dss5                                                    = pipelineInfo[static_cast<uint8_t>(PipelineType::FRONT_BACK_STENCIL)].depthStencilState;
    dss5.depthTest                                                                  = true;
    dss5.depthWrite                                                                 = false;
    dss5.stencilTestFront = dss5.stencilTestBack = true;
    dss5.stencilFuncFront = dss5.stencilFuncBack = gfx::ComparisonFunc::EQUAL;
    dss5.stencilReadMaskFront = dss5.stencilReadMaskBack = 0xFF;
    dss5.stencilWriteMaskFront = dss5.stencilWriteMaskBack = 0xFF;
    dss5.stencilFailOpFront = dss5.stencilFailOpBack = gfx::StencilOp::KEEP;
    dss5.stencilZFailOpFront = dss5.stencilZFailOpBack = gfx::StencilOp::KEEP;
    dss5.stencilPassOpFront = dss5.stencilPassOpBack = gfx::StencilOp::KEEP;
    dss5.stencilRefFront = dss5.stencilRefBack                                                    = 0x1;
    pipelineInfo[static_cast<uint8_t>(PipelineType::FRONT_BACK_STENCIL)].rasterizerState.cullMode = gfx::CullMode::NONE;
    pipelineInfo[static_cast<uint8_t>(PipelineType::FRONT_BACK_STENCIL)].pipelineLayout           = _pipelineLayout;
    _pipelineState[static_cast<uint8_t>(PipelineType::FRONT_BACK_STENCIL)]                        = device->createPipelineState(pipelineInfo[static_cast<uint8_t>(PipelineType::FRONT_BACK_STENCIL)]);

    _generalBarriers.push_back(device->getGeneralBarrier({
        gfx::AccessFlagBit::TRANSFER_WRITE,
        gfx::AccessFlagBit::VERTEX_SHADER_READ_UNIFORM_BUFFER |
            gfx::AccessFlagBit::VERTEX_BUFFER,
    }));

    _generalBarriers.push_back(device->getGeneralBarrier({
        gfx::AccessFlagBit::TRANSFER_WRITE,
        gfx::AccessFlagBit::VERTEX_SHADER_READ_UNIFORM_BUFFER,
    }));

    _textureBarriers.push_back(device->getTextureBarrier({
        gfx::AccessFlagBit::TRANSFER_WRITE,
        gfx::AccessFlagBit::FRAGMENT_SHADER_READ_TEXTURE
    }));

    _textureBarriers.push_back(_textureBarriers.back());
}

void StencilTest::onTick() {
    auto *swapchain = swapchains[0];
    auto *fbo       = fbos[0];

    uint generalBarrierIdx = _frameCount ? 1 : 0;

    gfx::Color clearColor = {1.0F, 0, 0, 1.0F};

    Mat4 proj;
    TestBaseI::createOrthographic(-1, 1, -1, 1, -1, 1, &proj, swapchain);

    device->acquire(&swapchain, 1);

    for (uint i = 0; i < BINDING_COUNT; i++) {
        TestBaseI::createOrthographic(-1, 1, -1, 1, -1, 1, &_uboData[i].viewProj, swapchain);
        _uniformBuffer[i]->update(&_uboData[i], sizeof(MatrixUBO));
    }

    gfx::Rect renderArea = {0, 0, swapchain->getWidth(), swapchain->getHeight()};

    auto *commandBuffer = commandBuffers[0];
    commandBuffer->begin();

    if (TestBaseI::MANUAL_BARRIER) {
        commandBuffer->pipelineBarrier(_generalBarriers[generalBarrierIdx], {}, {}, _textureBarriers, _textures);
    }

    commandBuffer->beginRenderPass(fbo->getRenderPass(), fbo, renderArea, &clearColor, 1.0F, 0);

    commandBuffer->bindInputAssembler(_inputAssembler);

    // draw label
    Vec4 relativeViewport{1.F / 6.F, 0.5F, 1.F / 3.F, 0.5F};
    commandBuffer->setViewport(TestBaseI::getViewportBasedOnDevice(relativeViewport, swapchain));
    commandBuffer->bindPipelineState(_pipelineState[static_cast<uint8_t>(PipelineType::STENCIL)]);
    commandBuffer->bindDescriptorSet(0, _descriptorSet[0]);
    commandBuffer->draw(_inputAssembler);

    // draw uv_image
    relativeViewport.x = 0.5F;
    commandBuffer->setViewport(TestBaseI::getViewportBasedOnDevice(relativeViewport, swapchain));
    commandBuffer->bindPipelineState(_pipelineState[static_cast<uint8_t>(PipelineType::IMAGE)]);
    commandBuffer->bindDescriptorSet(0, _descriptorSet[1]);
    commandBuffer->draw(_inputAssembler);

    // do back and front stencil test
    relativeViewport.x = 0.F;
    relativeViewport.y = 0.F;
    commandBuffer->setViewport(TestBaseI::getViewportBasedOnDevice(relativeViewport, swapchain));
    commandBuffer->bindPipelineState(_pipelineState[static_cast<uint8_t>(PipelineType::CANVAS)]);
    commandBuffer->bindDescriptorSet(0, _descriptorSet[0]);
    commandBuffer->draw(_inputAssembler);

    commandBuffer->bindPipelineState(_pipelineState[static_cast<uint8_t>(PipelineType::FRONT_BACK_STENCIL)]);
    commandBuffer->bindDescriptorSet(0, _descriptorSet[1]);
    commandBuffer->draw(_inputAssembler);

    // do back stencil test
    relativeViewport.x = 1.F / 3.F;
    commandBuffer->setViewport(TestBaseI::getViewportBasedOnDevice(relativeViewport, swapchain));
    commandBuffer->bindPipelineState(_pipelineState[static_cast<uint8_t>(PipelineType::CANVAS)]);
    commandBuffer->bindDescriptorSet(0, _descriptorSet[0]);
    commandBuffer->draw(_inputAssembler);

    commandBuffer->bindPipelineState(_pipelineState[static_cast<uint8_t>(PipelineType::BACK_STENCIL)]);
    commandBuffer->bindDescriptorSet(0, _descriptorSet[1]);
    commandBuffer->draw(_inputAssembler);

    // do front stencil test
    relativeViewport.x = 2.F / 3.F;
    commandBuffer->setViewport(TestBaseI::getViewportBasedOnDevice(relativeViewport, swapchain));
    commandBuffer->bindPipelineState(_pipelineState[static_cast<uint8_t>(PipelineType::CANVAS)]);
    commandBuffer->bindDescriptorSet(0, _descriptorSet[0]);
    commandBuffer->draw(_inputAssembler);

    commandBuffer->bindPipelineState(_pipelineState[static_cast<uint8_t>(PipelineType::FRONT_STENCIL)]);
    commandBuffer->bindDescriptorSet(0, _descriptorSet[1]);
    commandBuffer->draw(_inputAssembler);

    commandBuffer->endRenderPass();
    commandBuffer->end();

    device->flushCommands(commandBuffers);
    device->getQueue()->submit(commandBuffers);
    device->present();
}

} // namespace cc
