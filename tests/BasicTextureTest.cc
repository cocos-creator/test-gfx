#include "BasicTextureTest.h"

namespace cc {

void BasicTexture::destroy() {
    CC_SAFE_DESTROY(_shader);
    CC_SAFE_DESTROY(_vertexBuffer);
    CC_SAFE_DESTROY(_inputAssembler);
    CC_SAFE_DESTROY(_descriptorSet);
    CC_SAFE_DESTROY(_descriptorSetLayout);
    CC_SAFE_DESTROY(_pipelineLayout);
    CC_SAFE_DESTROY(_pipelineState);
    CC_SAFE_DESTROY(_uniformBuffer);
    CC_SAFE_DESTROY(_texture);
    CC_SAFE_DESTROY(_texture2);
    CC_SAFE_DESTROY(_image);
    CC_SAFE_DESTROY(_sampler);
}

bool BasicTexture::initialize() {
    createShader();
    createVertexBuffer();
    createInputAssembler();
    createTexture();
    createPipeline();
    return true;
}

void BasicTexture::createShader() {

    ShaderSources<StandardShaderSource> sources;
    sources.glsl4 = {
        R"(
            precision highp float;
            layout(location = 0) in vec2 a_position;
            layout(location = 0) out vec2 texcoord;
            layout(set = 0, binding = 0) uniform MVP_Matrix
            {
                mat4 u_mvpMatrix;
            };

            void main()
            {
                gl_Position = u_mvpMatrix * vec4(a_position, 0, 1);
                texcoord = a_position * 0.5 + 0.5;
                texcoord = vec2(texcoord.x, 1.0 - texcoord.y);
            }
        )",
        R"(
            precision highp float;
            layout(location = 0) in vec2 texcoord;
            layout(binding = 1) uniform sampler2D u_texture[2];
            layout(location = 0) out vec4 o_color;
            void main () {
                const int idx = texcoord.x > 0.5 ? 1 : 0; // 1;
                o_color = texture(u_texture[idx], texcoord);
            }
        )",
    };

    sources.glsl3 = {
        R"(
            precision highp float;
            in vec2 a_position;
            out vec2 texcoord;
            layout(std140) uniform MVP_Matrix
            {
                mat4 u_mvpMatrix;
            };

            void main()
            {
                gl_Position = u_mvpMatrix * vec4(a_position, 0, 1);
                texcoord = a_position * 0.5 + 0.5;
                texcoord = vec2(texcoord.x, 1.0 - texcoord.y);
            }
        )",
        R"(
            precision mediump float;
            in vec2 texcoord;
            uniform sampler2D u_texture[2];
            out vec4 o_color;
            void main () {
                const int idx = 1; // texcoord.x > 0.5 ? 1 : 0;
                o_color = texture(u_texture[idx], texcoord);
            }
        )",
    };

    sources.glsl1 = {
        R"(
            precision highp float;
            attribute vec2 a_position;
            uniform mat4 u_mvpMatrix;
            varying vec2 texcoord;
            void main ()
            {
                gl_Position = u_mvpMatrix * vec4(a_position, 0, 1);
                texcoord = a_position * 0.5 + 0.5;
                texcoord = vec2(texcoord.x, 1.0 - texcoord.y);
            }
        )",
        R"(
            precision mediump float;
            uniform sampler2D u_texture[2];
            varying vec2 texcoord;
            void main () {
                const int idx = 1; // texcoord.x > 0.5 ? 1 : 0;
                gl_FragColor = texture2D(u_texture[idx], texcoord);
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

    gfx::AttributeList             attributeList    = {{"a_position", gfx::Format::RG32F, false, 0, false, 0}};
    gfx::UniformList               mvpMatrix        = {{"u_mvpMatrix", gfx::Type::MAT4, 1}};
    gfx::UniformBlockList          uniformBlockList = {{0, 0, "MVP_Matrix", mvpMatrix, 1}};
    gfx::UniformSamplerTextureList sampler          = {{0, 1, "u_texture", gfx::Type::SAMPLER2D, 2}};

    gfx::ShaderInfo shaderInfo;
    shaderInfo.name            = "Basic Texture";
    shaderInfo.stages          = std::move(shaderStageList);
    shaderInfo.attributes      = std::move(attributeList);
    shaderInfo.blocks          = std::move(uniformBlockList);
    shaderInfo.samplerTextures = std::move(sampler);
    _shader                    = _device->createShader(shaderInfo);
}

void BasicTexture::createVertexBuffer() {
    float vertexData[] = {-1.0f, -1.0f,
                          1.0f, -1.0f,
                          1.0f, 1.0f,

                          1.0f, 1.0f,
                          -1.0f, 1.0f,
                          -1.0f, -1.0f};

    _vertexBuffer = _device->createBuffer({
        gfx::BufferUsage::VERTEX,
        gfx::MemoryUsage::DEVICE,
        sizeof(vertexData),
        2 * sizeof(float),
    });
    _vertexBuffer->update(vertexData, sizeof(vertexData));

    _uniformBuffer = _device->createBuffer({
        gfx::BufferUsage::UNIFORM,
        gfx::MemoryUsage::DEVICE,
        TestBaseI::getUBOSize(sizeof(Mat4)),
        0,
    });
}

void BasicTexture::createInputAssembler() {
    gfx::Attribute          position = {"a_position", gfx::Format::RG32F, false, 0, false};
    gfx::InputAssemblerInfo inputAssemblerInfo;
    inputAssemblerInfo.attributes.emplace_back(std::move(position));
    inputAssemblerInfo.vertexBuffers.emplace_back(_vertexBuffer);
    _inputAssembler = _device->createInputAssembler(inputAssemblerInfo);
}

void BasicTexture::createPipeline() {
    gfx::DescriptorSetLayoutInfo dslInfo;
    dslInfo.bindings.push_back({0, gfx::DescriptorType::UNIFORM_BUFFER, 1, gfx::ShaderStageFlagBit::VERTEX});
    dslInfo.bindings.push_back({1, gfx::DescriptorType::SAMPLER_TEXTURE, 2, gfx::ShaderStageFlagBit::FRAGMENT});
    _descriptorSetLayout = _device->createDescriptorSetLayout(dslInfo);

    _pipelineLayout = _device->createPipelineLayout({{_descriptorSetLayout}});

    _descriptorSet = _device->createDescriptorSet({_descriptorSetLayout});

    _descriptorSet->bindBuffer(0, _uniformBuffer);
    _descriptorSet->bindSampler(1, _sampler);
    _descriptorSet->bindTexture(1, _texture2);
    _descriptorSet->bindSampler(1, _sampler, 1);
    _descriptorSet->bindTexture(1, _texture, 1);
    _descriptorSet->update();

    gfx::PipelineStateInfo pipelineInfo;
    pipelineInfo.primitive      = gfx::PrimitiveMode::TRIANGLE_LIST;
    pipelineInfo.shader         = _shader;
    pipelineInfo.inputState     = {_inputAssembler->getAttributes()};
    pipelineInfo.renderPass     = _fbo->getRenderPass();
    pipelineInfo.pipelineLayout = _pipelineLayout;

    _pipelineState = _device->createPipelineState(pipelineInfo);
}

void BasicTexture::createTexture() {
    gfx::TextureInfo textureInfo;
    textureInfo.usage  = gfx::TextureUsage::SAMPLED | gfx::TextureUsage::TRANSFER_DST;
    textureInfo.format = gfx::Format::RGBA8;
    _texture           = TestBaseI::createTextureWithFile(_device, textureInfo, "uv_checker_01.jpg");
    _texture2          = TestBaseI::createTextureWithFile(_device, textureInfo, "uv_checker_02.jpg");

    //create sampler
    gfx::SamplerInfo samplerInfo;
    _sampler = _device->createSampler(samplerInfo);
}

void BasicTexture::tick() {
    gfx::AccessType accesses[] = {
        gfx::AccessType::VERTEX_SHADER_READ_UNIFORM_BUFFER,
        gfx::AccessType::VERTEX_BUFFER,
        gfx::AccessType::INDEX_BUFFER,
    };
    gfx::GlobalBarrier  shader_RAW_transfer{&gfx::AccessTypeV::TRANSFER_WRITE, 1, &accesses[0], COUNTOF(accesses)};
    gfx::TextureBarrier beforeRender[] = {
        {&gfx::AccessTypeV::TRANSFER_WRITE, 1, &gfx::AccessTypeV::FRAGMENT_SHADER_READ_TEXTURE, 1, false, _texture},
        {&gfx::AccessTypeV::TRANSFER_WRITE, 1, &gfx::AccessTypeV::FRAGMENT_SHADER_READ_TEXTURE, 1, false, _texture2},
    };
    uint textureBarriers = COUNTOF(beforeRender);
    if (_time) {
        shader_RAW_transfer.nextAccessCount = 1;
        textureBarriers                     = 0;
    }

    lookupTime();

    _time += hostThread.dt;

    gfx::Color clearColor = {0, 0, 0, 1.0f};

    Mat4 mvpMatrix;
    TestBaseI::createOrthographic(-1, 1, -1, 1, -1, 1, &mvpMatrix);

    _device->acquire();

    _uniformBuffer->update(&mvpMatrix, sizeof(mvpMatrix));
    gfx::Rect renderArea = {0, 0, _device->getWidth(), _device->getHeight()};

    auto commandBuffer = _commandBuffers[0];
    commandBuffer->begin();

    if (TestBaseI::MANUAL_BARRIER)
        commandBuffer->pipelineBarrier(&shader_RAW_transfer, beforeRender, textureBarriers);

    commandBuffer->beginRenderPass(_fbo->getRenderPass(), _fbo, renderArea, &clearColor, 1.0f, 0);
    commandBuffer->bindInputAssembler(_inputAssembler);
    commandBuffer->bindPipelineState(_pipelineState);
    commandBuffer->bindDescriptorSet(0, _descriptorSet);
    commandBuffer->draw(_inputAssembler);
    commandBuffer->endRenderPass();
    commandBuffer->end();

    _device->flushCommands(_commandBuffers);
    _device->getQueue()->submit(_commandBuffers);
    _device->present();
}

} // namespace cc
