#include "FullscreenQuad.h"
#include "tests/TestBase.h"

namespace cc {

FullscreenQuad::FullscreenQuad(gfx::Device *device, gfx::RenderPass *renderPass, gfx::Texture *texture) {
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
            layout(set = 0, binding = 0) uniform sampler2D u_texture;

            layout(location = 0) out vec4 o_color;
            void main() {
                o_color = texture(u_texture, v_texCoord);
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

            out vec4 o_color;
            void main() {
                o_color = texture(u_texture, v_texCoord);
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

            void main() {
                gl_FragColor = texture2D(u_texture, v_texCoord);
            }
        )",
    };

    StandardShaderSource &source = TestBaseI::getAppropriateShaderSource(sources);

    gfx::ShaderStageList shaderStageList;
    gfx::ShaderStage &   vertexShaderStage = shaderStageList.emplace_back();
    vertexShaderStage.stage                = gfx::ShaderStageFlagBit::VERTEX;
    vertexShaderStage.source               = source.vert;
    gfx::ShaderStage &fragmentShaderStage  = shaderStageList.emplace_back();
    fragmentShaderStage.stage              = gfx::ShaderStageFlagBit::FRAGMENT;
    fragmentShaderStage.source             = source.frag;

    gfx::AttributeList attributeList = {
        {"a_position", gfx::Format::RG32F, false, 0, false, 0},
        {"a_texCoord", gfx::Format::RG32F, false, 0, false, 1},
    };
    gfx::UniformSamplerTextureList samplerTextures = {{0, 0, "u_texture", gfx::Type::SAMPLER2D, 1}};

    gfx::ShaderInfo shaderInfo;
    shaderInfo.name            = "FullscreenQuad";
    shaderInfo.stages          = shaderStageList;
    shaderInfo.attributes      = attributeList;
    shaderInfo.samplerTextures = samplerTextures;
    _shader                    = device->createShader(shaderInfo);

    // create vertex buffer
    // UV space origin is at top-left
    float vertices[] = {-1, -4, 0.0, -1.5,
                        -1, 1, 0.0, 1.0,
                        4, 1, 2.5, 1.0};

    _vertexBuffer = device->createBuffer({
        gfx::BufferUsage::VERTEX,
        gfx::MemoryUsage::DEVICE,
        sizeof(vertices),
        4 * sizeof(float),
    });
    _vertexBuffer->update(vertices, sizeof(vertices));

    gfx::InputAssemblerInfo inputAssemblerInfo;
    inputAssemblerInfo.attributes = attributeList;
    inputAssemblerInfo.vertexBuffers.emplace_back(_vertexBuffer);
    _inputAssembler = device->createInputAssembler(inputAssemblerInfo);

    gfx::DescriptorSetLayoutInfo dslInfo;
    dslInfo.bindings.push_back({0, gfx::DescriptorType::SAMPLER_TEXTURE, 1, gfx::ShaderStageFlagBit::FRAGMENT});
    _descriptorSetLayout = device->createDescriptorSetLayout(dslInfo);

    _pipelineLayout = device->createPipelineLayout({{_descriptorSetLayout}});

    gfx::PipelineStateInfo pipelineInfo;
    pipelineInfo.primitive                    = gfx::PrimitiveMode::TRIANGLE_LIST;
    pipelineInfo.shader                       = _shader;
    pipelineInfo.inputState.attributes        = _inputAssembler->getAttributes();
    pipelineInfo.renderPass                   = renderPass;
    pipelineInfo.depthStencilState.depthTest  = false;
    pipelineInfo.depthStencilState.depthWrite = false;
    pipelineInfo.rasterizerState.cullMode     = gfx::CullMode::NONE;
    pipelineInfo.pipelineLayout               = _pipelineLayout;

    _pipelineState = device->createPipelineState(pipelineInfo);

    gfx::SamplerInfo samplerInfo;
    auto *           sampler = device->getSampler(samplerInfo);

    _descriptorSet = device->createDescriptorSet({_descriptorSetLayout});

    _descriptorSet->bindSampler(0, sampler);
    _descriptorSet->bindTexture(0, texture);
    _descriptorSet->update();
}

FullscreenQuad::~FullscreenQuad() {
    CC_SAFE_DESTROY_AND_DELETE(_shader)
    CC_SAFE_DESTROY_AND_DELETE(_vertexBuffer)
    CC_SAFE_DESTROY_AND_DELETE(_inputAssembler)
    CC_SAFE_DESTROY_AND_DELETE(_descriptorSetLayout)
    CC_SAFE_DESTROY_AND_DELETE(_pipelineLayout)
    CC_SAFE_DESTROY_AND_DELETE(_pipelineState)
    CC_SAFE_DESTROY_AND_DELETE(_descriptorSet)
}

void FullscreenQuad::draw(gfx::CommandBuffer *commandBuffer) {
    commandBuffer->bindPipelineState(_pipelineState);
    commandBuffer->bindDescriptorSet(0, _descriptorSet);
    commandBuffer->bindInputAssembler(_inputAssembler);
    commandBuffer->draw(_inputAssembler);
}

} // namespace cc
