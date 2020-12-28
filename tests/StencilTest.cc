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
}

void StencilTest::destroy() {
    CC_SAFE_DESTROY(_shader);
    CC_SAFE_DESTROY(_labelTexture);
    CC_SAFE_DESTROY(_uvCheckerTexture);
    CC_SAFE_DESTROY(_vertexBuffer);
    CC_SAFE_DESTROY(_inputAssembler);
    CC_SAFE_DESTROY(_sampler);
    for (uint i = 0; i < BINDING_COUNT; i++) {
        CC_SAFE_DESTROY(_uniformBuffer[i]);
        CC_SAFE_DESTROY(_descriptorSet[i]);
    }
    CC_SAFE_DESTROY(_descriptorSetLayout);
    CC_SAFE_DESTROY(_pipelineLayout);
    for (uint i = 0; i < PIPELIE_COUNT; i++) {
        CC_SAFE_DESTROY(_pipelineState[i]);
    }
}

bool StencilTest::initialize() {
    createShader();
    createBuffers();
    createTextures();
    createInputAssembler();
    createPipelineState();
    return true;
}

void StencilTest::createShader() {

    ShaderSources sources;
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
    gfx::UniformList worldMatrix = {{"u_worldMatrix", gfx::Type::MAT4, 1}, {"u_projMatrix", gfx::Type::MAT4, 1}};
    gfx::UniformBlockList uniformBlockList = {{0, 0, "World_Matrix", worldMatrix, 1}};
    gfx::UniformSamplerList sampler = {{0, 1, "u_texture", gfx::Type::SAMPLER2D, 1}};

    gfx::ShaderInfo shaderInfo;
    shaderInfo.name = "Stencil Test";
    shaderInfo.stages = std::move(shaderStageList);
    shaderInfo.attributes = std::move(attributeList);
    shaderInfo.blocks = std::move(uniformBlockList);
    shaderInfo.samplers = std::move(sampler);
    _shader = _device->createShader(shaderInfo);
}

void StencilTest::createBuffers() {
    float vertexData[] = {-1.0f, -1.0f, 0.0f, 1.0f,
                          1.0f, -1.0f, 1.0f, 1.0f,
                          1.0f, 1.0f, 1.0f, 0.0f,
                          1.0f, 1.0f, 1.0f, 0.0f,
                          -1.0f, -1.0f, 0.0f, 1.0f,
                          -1.0f, 1.0f, 0.0f, 0.0f};

    _vertexBuffer = _device->createBuffer({
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

    _uboData[0].world.scale(0.5f);
    _uboData[0].world.rotateZ(cc::math::PI / 4);
    _uboData[1].world.scale(0.5f, 0.5f, 0.25f);

    for (uint i = 0; i < BINDING_COUNT; i++) {
        _uniformBuffer[i] = _device->createBuffer(uniformBufferInfo);
    }
}

void StencilTest::createTextures() {
    // load stencil image
    auto stencilImage = new cc::Image();
    stencilImage->autorelease();
    bool ret = stencilImage->initWithImageFile("stencil.jpg");
    assert(ret);
    auto stencilImageData = TestBaseI::RGB2RGBA(stencilImage);

    gfx::BufferTextureCopy labelTextureRegion;
    labelTextureRegion.buffTexHeight = stencilImage->getHeight();
    labelTextureRegion.texExtent.width = stencilImage->getWidth();
    labelTextureRegion.texExtent.height = stencilImage->getHeight();
    labelTextureRegion.texExtent.depth = 1;

    gfx::BufferTextureCopyList regions;
    regions.push_back(std::move(labelTextureRegion));

    gfx::TextureInfo labelTextureInfo;
    labelTextureInfo.usage = gfx::TextureUsage::SAMPLED | gfx::TextureUsage::TRANSFER_DST;
    labelTextureInfo.format = gfx::Format::RGBA8;
    labelTextureInfo.width = stencilImage->getWidth();
    labelTextureInfo.height = stencilImage->getHeight();
    _labelTexture = _device->createTexture(labelTextureInfo);

    gfx::BufferDataList stencilBuffer = {stencilImageData};
    _device->copyBuffersToTexture(stencilBuffer, _labelTexture, regions);

    // load uv_checker_02.jpg
    auto img = new cc::Image();
    img->autorelease();
    ret = img->initWithImageFile("uv_checker_02.jpg");
    assert(ret);
    auto imgData = TestBaseI::RGB2RGBA(img);

    gfx::TextureInfo textureInfo;
    textureInfo.usage = gfx::TextureUsage::SAMPLED | gfx::TextureUsage::TRANSFER_DST;
    textureInfo.format = gfx::Format::RGBA8;
    textureInfo.width = img->getWidth();
    textureInfo.height = img->getHeight();
    _uvCheckerTexture = _device->createTexture(textureInfo);

    gfx::BufferTextureCopy textureRegion;
    textureRegion.buffTexHeight = img->getHeight();
    textureRegion.texExtent.width = img->getWidth();
    textureRegion.texExtent.height = img->getHeight();
    textureRegion.texExtent.depth = 1;

    gfx::BufferTextureCopyList uvTextureRegions;
    uvTextureRegions.push_back(std::move(textureRegion));

    gfx::BufferDataList uvImageBuffer = {imgData};
    _device->copyBuffersToTexture(uvImageBuffer, _uvCheckerTexture, uvTextureRegions);

    // create sampler
    gfx::SamplerInfo samplerInfo;
    _sampler = _device->createSampler(samplerInfo);

    delete[] imgData;
    delete[] stencilImageData;
}

void StencilTest::createInputAssembler() {
    gfx::InputAssemblerInfo inputAssemblerInfo;
    inputAssemblerInfo.attributes.push_back({"a_position", gfx::Format::RG32F, false, 0, false});
    inputAssemblerInfo.attributes.push_back({"a_texCoord", gfx::Format::RG32F, false, 0, false});
    inputAssemblerInfo.vertexBuffers.emplace_back(_vertexBuffer);
    _inputAssembler = _device->createInputAssembler(inputAssemblerInfo);
}

void StencilTest::createPipelineState() {
    gfx::DescriptorSetLayoutInfo dslInfo;
    dslInfo.bindings.push_back({0, gfx::DescriptorType::UNIFORM_BUFFER, 1, gfx::ShaderStageFlagBit::VERTEX});
    dslInfo.bindings.push_back({1, gfx::DescriptorType::SAMPLER, 1, gfx::ShaderStageFlagBit::FRAGMENT});
    _descriptorSetLayout = _device->createDescriptorSetLayout(dslInfo);

    _pipelineLayout = _device->createPipelineLayout({{_descriptorSetLayout}});

    gfx::Texture *texView[BINDING_COUNT] = {_labelTexture, _uvCheckerTexture};
    for (uint i = 0; i < BINDING_COUNT; i++) {
        _descriptorSet[i] = _device->createDescriptorSet({_descriptorSetLayout});
        _descriptorSet[i]->bindBuffer(0, _uniformBuffer[i]);
        _descriptorSet[i]->bindSampler(1, _sampler);
        _descriptorSet[i]->bindTexture(1, texView[i]);
        _descriptorSet[i]->update();
    }

    gfx::PipelineStateInfo pipelineInfo[PIPELIE_COUNT];
    pipelineInfo[(uint8_t)PipelineType::STENCIL].primitive = gfx::PrimitiveMode::TRIANGLE_LIST;
    pipelineInfo[(uint8_t)PipelineType::STENCIL].shader = _shader;
    pipelineInfo[(uint8_t)PipelineType::STENCIL].inputState = {_inputAssembler->getAttributes()};
    pipelineInfo[(uint8_t)PipelineType::STENCIL].renderPass = _fbo->getRenderPass();
    pipelineInfo[(uint8_t)PipelineType::STENCIL].depthStencilState.depthTest = false;
    pipelineInfo[(uint8_t)PipelineType::STENCIL].depthStencilState.stencilTestFront = false;
    pipelineInfo[(uint8_t)PipelineType::STENCIL].depthStencilState.stencilTestBack = false;
    pipelineInfo[(uint8_t)PipelineType::STENCIL].rasterizerState.cullMode = gfx::CullMode::NONE;
    pipelineInfo[(uint8_t)PipelineType::STENCIL].dynamicStates = gfx::DynamicStateFlagBit::VIEWPORT;
    pipelineInfo[(uint8_t)PipelineType::STENCIL].pipelineLayout = _pipelineLayout;
    _pipelineState[(uint8_t)PipelineType::STENCIL] = _device->createPipelineState(pipelineInfo[(uint8_t)PipelineType::STENCIL]);

    pipelineInfo[(uint8_t)PipelineType::IMAGE].primitive = gfx::PrimitiveMode::TRIANGLE_LIST;
    pipelineInfo[(uint8_t)PipelineType::IMAGE].shader = _shader;
    pipelineInfo[(uint8_t)PipelineType::IMAGE].inputState = {_inputAssembler->getAttributes()};
    pipelineInfo[(uint8_t)PipelineType::IMAGE].renderPass = _fbo->getRenderPass();
    pipelineInfo[(uint8_t)PipelineType::IMAGE].depthStencilState.depthTest = false;
    pipelineInfo[(uint8_t)PipelineType::IMAGE].depthStencilState.stencilTestFront = false;
    pipelineInfo[(uint8_t)PipelineType::IMAGE].depthStencilState.stencilTestBack = false;
    pipelineInfo[(uint8_t)PipelineType::IMAGE].rasterizerState.cullMode = gfx::CullMode::NONE;
    pipelineInfo[(uint8_t)PipelineType::IMAGE].dynamicStates = gfx::DynamicStateFlagBit::VIEWPORT;
    pipelineInfo[(uint8_t)PipelineType::IMAGE].pipelineLayout = _pipelineLayout;
    _pipelineState[(uint8_t)PipelineType::IMAGE] = _device->createPipelineState(pipelineInfo[(uint8_t)PipelineType::IMAGE]);

    pipelineInfo[(uint8_t)PipelineType::CANVAS].primitive = gfx::PrimitiveMode::TRIANGLE_LIST;
    pipelineInfo[(uint8_t)PipelineType::CANVAS].shader = _shader;
    pipelineInfo[(uint8_t)PipelineType::CANVAS].inputState = {_inputAssembler->getAttributes()};
    pipelineInfo[(uint8_t)PipelineType::CANVAS].renderPass = _fbo->getRenderPass();
    gfx::DepthStencilState &dss2 = pipelineInfo[(uint8_t)PipelineType::CANVAS].depthStencilState;
    dss2.depthTest = true;
    dss2.depthWrite = false;
    dss2.stencilTestFront = dss2.stencilTestBack = true;
    dss2.stencilFuncFront = dss2.stencilFuncBack = gfx::ComparisonFunc::NEVER;
    dss2.stencilReadMaskFront = dss2.stencilReadMaskBack = 0xFF;
    dss2.stencilWriteMaskFront = dss2.stencilWriteMaskBack = 0xFF;
    dss2.stencilFailOpFront = dss2.stencilFailOpBack = gfx::StencilOp::REPLACE;
    dss2.stencilZFailOpFront = dss2.stencilZFailOpBack = gfx::StencilOp::KEEP;
    dss2.stencilPassOpFront = dss2.stencilPassOpBack = gfx::StencilOp::KEEP;
    dss2.stencilRefFront = dss2.stencilRefBack = 0x1;
    pipelineInfo[(uint8_t)PipelineType::CANVAS].rasterizerState.cullMode = gfx::CullMode::NONE;
    pipelineInfo[(uint8_t)PipelineType::CANVAS].dynamicStates = gfx::DynamicStateFlagBit::VIEWPORT;
    pipelineInfo[(uint8_t)PipelineType::CANVAS].pipelineLayout = _pipelineLayout;
    _pipelineState[(uint8_t)PipelineType::CANVAS] = _device->createPipelineState(pipelineInfo[(uint8_t)PipelineType::CANVAS]);

    pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].primitive = gfx::PrimitiveMode::TRIANGLE_LIST;
    pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].shader = _shader;
    pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].inputState = {_inputAssembler->getAttributes()};
    pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].renderPass = _fbo->getRenderPass();
    gfx::DepthStencilState &dss3 = pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].depthStencilState;
    dss3.depthTest = true;
    dss3.depthWrite = false;
    dss3.stencilTestFront = dss3.stencilTestBack = true;
    dss3.stencilFuncFront = gfx::ComparisonFunc::EQUAL;
    dss3.stencilFuncBack = gfx::ComparisonFunc::ALWAYS;
    dss3.stencilReadMaskFront = dss3.stencilReadMaskBack = 0xFF;
    dss3.stencilWriteMaskFront = dss3.stencilWriteMaskBack = 0xFF;
    dss3.stencilFailOpFront = dss3.stencilFailOpBack = gfx::StencilOp::KEEP;
    dss3.stencilZFailOpFront = dss3.stencilZFailOpBack = gfx::StencilOp::KEEP;
    dss3.stencilPassOpFront = dss3.stencilPassOpBack = gfx::StencilOp::KEEP;
    dss3.stencilRefFront = dss3.stencilRefBack = 0x1;
    pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].rasterizerState.cullMode = gfx::CullMode::NONE;
    pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].dynamicStates = gfx::DynamicStateFlagBit::VIEWPORT;
    pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].pipelineLayout = _pipelineLayout;
    _pipelineState[(uint8_t)PipelineType::FRONT_STENCIL] = _device->createPipelineState(pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL]);

    pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].primitive = gfx::PrimitiveMode::TRIANGLE_LIST;
    pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].shader = _shader;
    pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].inputState = {_inputAssembler->getAttributes()};
    pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].renderPass = _fbo->getRenderPass();
    gfx::DepthStencilState &dss4 = pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].depthStencilState;
    dss4.depthTest = true;
    dss4.depthWrite = false;
    dss4.stencilTestFront = dss4.stencilTestBack = true;
    dss4.stencilFuncFront = gfx::ComparisonFunc::ALWAYS;
    dss4.stencilFuncBack = gfx::ComparisonFunc::EQUAL;
    dss4.stencilReadMaskFront = dss4.stencilReadMaskBack = 0xFF;
    dss4.stencilWriteMaskFront = dss4.stencilWriteMaskBack = 0xFF;
    dss4.stencilFailOpFront = dss4.stencilFailOpBack = gfx::StencilOp::KEEP;
    dss4.stencilZFailOpFront = dss4.stencilZFailOpBack = gfx::StencilOp::KEEP;
    dss4.stencilPassOpFront = dss4.stencilPassOpBack = gfx::StencilOp::KEEP;
    dss4.stencilRefFront = dss4.stencilRefBack = 0x1;
    pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].rasterizerState.cullMode = gfx::CullMode::NONE;
    pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].dynamicStates = gfx::DynamicStateFlagBit::VIEWPORT;
    pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].pipelineLayout = _pipelineLayout;
    _pipelineState[(uint8_t)PipelineType::BACK_STENCIL] = _device->createPipelineState(pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL]);

    pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].primitive = gfx::PrimitiveMode::TRIANGLE_LIST;
    pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].shader = _shader;
    pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].inputState = {_inputAssembler->getAttributes()};
    pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].renderPass = _fbo->getRenderPass();
    gfx::DepthStencilState &dss5 = pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].depthStencilState;
    dss5.depthTest = true;
    dss5.depthWrite = false;
    dss5.stencilTestFront = dss5.stencilTestBack = true;
    dss5.stencilFuncFront = dss5.stencilFuncBack = gfx::ComparisonFunc::EQUAL;
    dss5.stencilReadMaskFront = dss5.stencilReadMaskBack = 0xFF;
    dss5.stencilWriteMaskFront = dss5.stencilWriteMaskBack = 0xFF;
    dss5.stencilFailOpFront = dss5.stencilFailOpBack = gfx::StencilOp::KEEP;
    dss5.stencilZFailOpFront = dss5.stencilZFailOpBack = gfx::StencilOp::KEEP;
    dss5.stencilPassOpFront = dss5.stencilPassOpBack = gfx::StencilOp::KEEP;
    dss5.stencilRefFront = dss5.stencilRefBack = 0x1;
    pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].rasterizerState.cullMode = gfx::CullMode::NONE;
    pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].dynamicStates = gfx::DynamicStateFlagBit::VIEWPORT;
    pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].pipelineLayout = _pipelineLayout;
    _pipelineState[(uint8_t)PipelineType::FRONT_BACK_STENCIL] = _device->createPipelineState(pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL]);
}

void StencilTest::tick() {
    lookupTime();
    _dt += hostThread.dt;
    gfx::Color clearColor = {1.0f, 0, 0, 1.0f};

    Mat4 proj;
    TestBaseI::createOrthographic(-1, 1, -1, 1, -1, 1, &proj);

    _device->acquire();

    for (uint i = 0; i < BINDING_COUNT; i++) {
        TestBaseI::createOrthographic(-1, 1, -1, 1, -1, 1, &_uboData[i].viewProj);
        _uniformBuffer[i]->update(&_uboData[i], sizeof(MatrixUBO));
    }

    gfx::Rect renderArea = {0, 0, _device->getWidth(), _device->getHeight()};

    auto commandBuffer = _commandBuffers[0];
    commandBuffer->begin();
    commandBuffer->beginRenderPass(_fbo->getRenderPass(), _fbo, renderArea, &clearColor, 1.0f, 0);

    commandBuffer->bindInputAssembler(_inputAssembler);

    // draw label
    Vec4 relativeViewport{1.f / 6.f, 0.5f, 1.f / 3.f, 0.5f};
    commandBuffer->setViewport(TestBaseI::getViewportBasedOnDevice(relativeViewport));
    commandBuffer->bindPipelineState(_pipelineState[(uint8_t)PipelineType::STENCIL]);
    commandBuffer->bindDescriptorSet(0, _descriptorSet[0]);
    commandBuffer->draw(_inputAssembler);

    // draw uv_image
    relativeViewport.x = 0.5f;
    commandBuffer->setViewport(TestBaseI::getViewportBasedOnDevice(relativeViewport));
    commandBuffer->bindPipelineState(_pipelineState[(uint8_t)PipelineType::IMAGE]);
    commandBuffer->bindDescriptorSet(0, _descriptorSet[1]);
    commandBuffer->draw(_inputAssembler);

    // do back and front stencil test
    relativeViewport.x = 0.f;
    relativeViewport.y = 0.f;
    commandBuffer->setViewport(TestBaseI::getViewportBasedOnDevice(relativeViewport));
    commandBuffer->bindPipelineState(_pipelineState[(uint8_t)PipelineType::CANVAS]);
    commandBuffer->bindDescriptorSet(0, _descriptorSet[0]);
    commandBuffer->draw(_inputAssembler);

    commandBuffer->bindPipelineState(_pipelineState[(uint8_t)PipelineType::FRONT_BACK_STENCIL]);
    commandBuffer->bindDescriptorSet(0, _descriptorSet[1]);
    commandBuffer->draw(_inputAssembler);

    // do back stencil test
    relativeViewport.x = 1.f / 3.f;
    commandBuffer->setViewport(TestBaseI::getViewportBasedOnDevice(relativeViewport));
    commandBuffer->bindPipelineState(_pipelineState[(uint8_t)PipelineType::CANVAS]);
    commandBuffer->bindDescriptorSet(0, _descriptorSet[0]);
    commandBuffer->draw(_inputAssembler);

    commandBuffer->bindPipelineState(_pipelineState[(uint8_t)PipelineType::BACK_STENCIL]);
    commandBuffer->bindDescriptorSet(0, _descriptorSet[1]);
    commandBuffer->draw(_inputAssembler);

    // do front stencil test
    relativeViewport.x = 2.f / 3.f;
    commandBuffer->setViewport(TestBaseI::getViewportBasedOnDevice(relativeViewport));
    commandBuffer->bindPipelineState(_pipelineState[(uint8_t)PipelineType::CANVAS]);
    commandBuffer->bindDescriptorSet(0, _descriptorSet[0]);
    commandBuffer->draw(_inputAssembler);

    commandBuffer->bindPipelineState(_pipelineState[(uint8_t)PipelineType::FRONT_STENCIL]);
    commandBuffer->bindDescriptorSet(0, _descriptorSet[1]);
    commandBuffer->draw(_inputAssembler);

    commandBuffer->endRenderPass();
    commandBuffer->end();

    _device->getQueue()->submit(_commandBuffers);
    _device->present();
}

} // namespace cc
