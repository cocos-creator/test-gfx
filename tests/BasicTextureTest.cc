#include "BasicTextureTest.h"
#include <stdint.h>

namespace cc {

void BasicTexture::onDestroy() {
    CC_SAFE_DESTROY(_shader);
    for (auto *view : _textureViews) {
        CC_SAFE_DESTROY(view);
    }
    _textureViews.clear();
    CC_SAFE_DESTROY(_vertexBuffer);
    CC_SAFE_DESTROY(_inputAssembler);
    CC_SAFE_DESTROY(_descriptorSet);
    CC_SAFE_DESTROY(_descriptorSetLayout);
    CC_SAFE_DESTROY(_pipelineLayout);
    CC_SAFE_DESTROY(_pipelineState);
    CC_SAFE_DESTROY(_uniformBuffer);
}

bool BasicTexture::onInit() {
    _oldTime = static_cast<uint32_t>(_time);
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
            layout(location = 1) in vec2 a_texCoord;
            layout(location = 0) out vec2 texcoord;
            layout(set = 0, binding = 0) uniform MVP_Matrix
            {
                mat4 u_mvpMatrix;
            };

            void main()
            {
                gl_Position = u_mvpMatrix * vec4(a_position, 0, 1);
                texcoord = a_texCoord;
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
            in vec2 a_texCoord;
            out vec2 texcoord;
            layout(std140) uniform MVP_Matrix
            {
                mat4 u_mvpMatrix;
            };

            void main()
            {
                gl_Position = u_mvpMatrix * vec4(a_position, 0, 1);
                texcoord = a_texCoord;
            }
        )",
        R"(
            precision mediump float;
            in vec2 texcoord;
            uniform sampler2D u_texture[2];
            out vec4 o_color;
            void main () {
                if (texcoord.x > 0.5) {
                    o_color = texture(u_texture[1], texcoord);
                } else {
                    o_color = texture(u_texture[0], texcoord);
                }
            }
        )",
    };

    sources.glsl1 = {
        R"(
            precision highp float;
            attribute vec2 a_position;
            attribute vec2 a_texCoord;
            uniform mat4 u_mvpMatrix;
            varying vec2 texcoord;
            void main ()
            {
                gl_Position = u_mvpMatrix * vec4(a_position, 0, 1);
                texcoord = a_texCoord;
            }
        )",
        R"(
            precision mediump float;
            uniform sampler2D u_texture[2];
            varying vec2 texcoord;
            void main () {
                if (texcoord.x > 0.5) {
                    gl_FragColor = texture2D(u_texture[1], texcoord);
                } else {
                    gl_FragColor = texture2D(u_texture[0], texcoord);
                }
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
    gfx::UniformList               mvpMatrix        = {{"u_mvpMatrix", gfx::Type::MAT4, 1}};
    gfx::UniformBlockList          uniformBlockList = {{0, 0, "MVP_Matrix", mvpMatrix, 1}};
    gfx::UniformSamplerTextureList sampler          = {{0, 1, "u_texture", gfx::Type::SAMPLER2D, 2}};

    gfx::ShaderInfo shaderInfo;
    shaderInfo.name            = "Basic Texture";
    shaderInfo.stages          = std::move(shaderStageList);
    shaderInfo.attributes      = std::move(attributeList);
    shaderInfo.blocks          = std::move(uniformBlockList);
    shaderInfo.samplerTextures = std::move(sampler);
    _shader                    = device->createShader(shaderInfo);
}

void BasicTexture::createVertexBuffer() {
    //float left = -.7f, bottom = -.2f, right = .1f, top = .6f;
    float left         = -1.F;
    float bottom       = -1.F;
    float right        = 1.F;
    float top          = 1.F;
    float vertexData[] = {left, bottom, 0.F, 1.F,
                          right, bottom, 1.F, 1.F,
                          right, top, 1.F, 0.F,

                          right, top, 1.F, 0.F,
                          left, top, 0.F, 0.F,
                          left, bottom, 0.F, 1.F};

    _vertexBuffer = device->createBuffer({
        gfx::BufferUsage::VERTEX,
        gfx::MemoryUsage::DEVICE,
        sizeof(vertexData),
        4 * sizeof(float),
    });
    _vertexBuffer->update(vertexData, sizeof(vertexData));

    _uniformBuffer = device->createBuffer({
        gfx::BufferUsage::UNIFORM,
        gfx::MemoryUsage::DEVICE,
        TestBaseI::getUBOSize(sizeof(Mat4)),
    });
}

void BasicTexture::createInputAssembler() {
    gfx::InputAssemblerInfo inputAssemblerInfo;
    inputAssemblerInfo.attributes.emplace_back(gfx::Attribute{"a_position", gfx::Format::RG32F, false, 0, false});
    inputAssemblerInfo.attributes.emplace_back(gfx::Attribute{"a_texCoord", gfx::Format::RG32F, false, 0, false});
    inputAssemblerInfo.vertexBuffers.emplace_back(_vertexBuffer);
    _inputAssembler = device->createInputAssembler(inputAssemblerInfo);
}

void BasicTexture::createTexture() {
    gfx::TextureInfo textureInfo;
    textureInfo.usage  = gfx::TextureUsage::SAMPLED | gfx::TextureUsage::TRANSFER_DST | gfx::TextureUsage::TRANSFER_SRC;
    textureInfo.format = gfx::Format::RGBA8;
    textureInfo.flags  = gfx::TextureFlagBit::GEN_MIPMAP;

    _textures.resize(2);
    _textures[0] = TestBaseI::createTextureWithFile(textureInfo, "uv_checker_01.jpg");
    _textures[1] = TestBaseI::createTextureWithFile(textureInfo, "uv_checker_02.jpg");

    gfx::TextureViewInfo viewInfo = {};
    viewInfo.type                 = gfx::TextureType::TEX2D;
    viewInfo.format               = gfx::Format::RGBA8;
    viewInfo.baseLevel            = 3;
    viewInfo.levelCount           = 1;

    _textureViews.resize(2);
    viewInfo.texture = _textures[0];
    _textureViews[0] = TestBaseI::device->createTexture(viewInfo);
    viewInfo.texture = _textures[1];
    _textureViews[1] = TestBaseI::device->createTexture(viewInfo);

    ccstd::vector<uint8_t> buffer(_textures[0]->getWidth() * _textures[0]->getHeight() * gfx::GFX_FORMAT_INFOS[toNumber(_textures[0]->getFormat())].size);
    uint8_t *       data = buffer.data();

    gfx::BufferTextureCopy region;
    region.texExtent.width  = _textures[0]->getWidth();
    region.texExtent.height = _textures[0]->getHeight();
    device->copyTextureToBuffers(_textures[0], &data, &region, 1);

    device->copyBuffersToTexture(&data, _textures[0], &region, 1);
}

void BasicTexture::createPipeline() {
    gfx::DescriptorSetLayoutInfo dslInfo;
    dslInfo.bindings.push_back({0, gfx::DescriptorType::UNIFORM_BUFFER, 1, gfx::ShaderStageFlagBit::VERTEX});
    dslInfo.bindings.push_back({1, gfx::DescriptorType::SAMPLER_TEXTURE, 2, gfx::ShaderStageFlagBit::FRAGMENT});
    _descriptorSetLayout = device->createDescriptorSetLayout(dslInfo);

    _pipelineLayout = device->createPipelineLayout({{_descriptorSetLayout}});

    _descriptorSet = device->createDescriptorSet({_descriptorSetLayout});

    gfx::SamplerInfo samplerInfo;
    auto *           sampler = device->getSampler(samplerInfo);

    _descriptorSet->bindBuffer(0, _uniformBuffer);
    _descriptorSet->bindSampler(1, sampler);
    _descriptorSet->bindTexture(1, _textures[1]);
    _descriptorSet->bindSampler(1, sampler, 1);
    _descriptorSet->bindTexture(1, _textures[0], 1);
    _descriptorSet->update();

    gfx::PipelineStateInfo pipelineInfo;
    pipelineInfo.primitive      = gfx::PrimitiveMode::TRIANGLE_LIST;
    pipelineInfo.shader         = _shader;
    pipelineInfo.inputState     = {_inputAssembler->getAttributes()};
    pipelineInfo.renderPass     = renderPass;
    pipelineInfo.pipelineLayout = _pipelineLayout;

    _pipelineState = device->createPipelineState(pipelineInfo);

    _generalBarriers.push_back(device->getGeneralBarrier({
        gfx::AccessFlagBit::TRANSFER_WRITE,
        gfx::AccessFlagBit::VERTEX_SHADER_READ_UNIFORM_BUFFER | gfx::AccessFlagBit::VERTEX_BUFFER | gfx::AccessFlagBit::INDEX_BUFFER,
    }));

    _generalBarriers.push_back(device->getGeneralBarrier({
        gfx::AccessFlagBit::TRANSFER_WRITE,
        gfx::AccessFlagBit::VERTEX_SHADER_READ_UNIFORM_BUFFER,
    }));

    _textureBarriers.push_back(device->getTextureBarrier({
        gfx::AccessFlagBit::TRANSFER_WRITE,
        gfx::AccessFlagBit::FRAGMENT_SHADER_READ_TEXTURE,
    }));

    _textureBarriers.push_back(_textureBarriers.back());
}

void BasicTexture::onTick() {
    auto *swapchain = swapchains[0];
    auto *fbo       = fbos[0];

    if (static_cast<uint32_t>(_time) > _oldTime) {
        _oldTime = static_cast<uint32_t>(_time);
        if (_oldTime % 2) {
            _descriptorSet->bindTexture(1, _textureViews[1]);
            _descriptorSet->bindTexture(1, _textureViews[0], 1);

        } else {
            _descriptorSet->bindTexture(1, _textures[1]);
            _descriptorSet->bindTexture(1, _textures[0], 1);
        }
        _descriptorSet->update();
    }

    uint generalBarrierIdx = _frameCount ? 1 : 0;

    gfx::Color clearColor = {0, 0, 0, 1.0F};

    Mat4 mvpMatrix;
    TestBaseI::createOrthographic(-1, 1, -1, 1, -1, 1, &mvpMatrix, swapchain);

    device->acquire(&swapchain, 1);

    _uniformBuffer->update(&mvpMatrix, sizeof(mvpMatrix));
    gfx::Rect renderArea = {0, 0, swapchain->getWidth(), swapchain->getHeight()};

    auto *commandBuffer = commandBuffers[0];
    commandBuffer->begin();

    if (TestBaseI::MANUAL_BARRIER) {
        commandBuffer->pipelineBarrier(_generalBarriers[generalBarrierIdx], {}, {}, _textureBarriers, _textures);
    }

    commandBuffer->beginRenderPass(fbo->getRenderPass(), fbo, renderArea, &clearColor, 1.0F, 0);
    commandBuffer->bindInputAssembler(_inputAssembler);
    commandBuffer->bindPipelineState(_pipelineState);
    commandBuffer->bindDescriptorSet(0, _descriptorSet);
    commandBuffer->draw(_inputAssembler);
    commandBuffer->endRenderPass();
    commandBuffer->end();

    device->flushCommands(commandBuffers);
    device->getQueue()->submit(commandBuffers);
    device->present();
}

} // namespace cc
