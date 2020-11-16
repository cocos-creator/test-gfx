#include "Multithread.h"

namespace cc {

#define MODELS_PER_LINE 300

void Multithread::destroy() {
    CC_SAFE_DESTROY(_vertexBuffer);
    CC_SAFE_DESTROY(_inputAssembler);
    CC_SAFE_DESTROY(_descriptorSet);
    CC_SAFE_DESTROY(_uniformBufferWorldView);
    CC_SAFE_DESTROY(_uniformBufferWorld);
    CC_SAFE_DESTROY(_uniformBufferVP);
    CC_SAFE_DESTROY(_shader);
    CC_SAFE_DESTROY(_descriptorSetLayout);
    CC_SAFE_DESTROY(_pipelineLayout);
    CC_SAFE_DESTROY(_pipelineState);
}

bool Multithread::initialize() {
    createShader();
    createVertexBuffer();
    createInputAssembler();
    createPipeline();

    return true;
}

void Multithread::createShader() {

    ShaderSources sources;
    sources.glsl4 = {
        R"(
            layout(location = 0) in vec2 a_position;
            layout(set = 0, binding = 0) uniform ViewProj { mat4 u_viewProj; };
            layout(set = 0, binding = 1) uniform World { vec4 u_world; };

            void main() {
                gl_Position = u_viewProj * vec4(a_position + u_world.xy, 0.0, 1.0);
            }
        )",
        R"(
            layout(location = 0) out vec4 o_color;

            void main() {
                o_color = vec4(1.0);
            }
        )",
    };

    sources.glsl3 = {
        R"(
            in vec2 a_position;
            layout(std140) uniform ViewProj { mat4 u_viewProj; };
            layout(std140) uniform World { vec4 u_world; };

            void main() {
                gl_Position = u_viewProj * vec4(a_position + u_world.xy, 0.0, 1.0);
            }
        )",
        R"(
            precision mediump float;

            out vec4 o_color;
            void main() {
                o_color = vec4(1.0);
            }
        )",
    };

    sources.glsl1 = {
        R"(
            attribute vec2 a_position;
            uniform mat4 u_viewProj;
            uniform vec4 u_world;

            void main() {
                gl_Position = u_viewProj * vec4(a_position + u_world.xy, 0.0, 1.0);
            }
        )",
        R"(
            precision mediump float;
            void main() {
                gl_FragColor = vec4(1.0);
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

    gfx::UniformBlockList uniformBlockList = {
        {0, 0, "ViewProj", {{"u_viewProj", gfx::Type::MAT4, 1}}, 1},
        {0, 1, "World", {{"u_world", gfx::Type::FLOAT4, 1}}, 1},
    };
    gfx::AttributeList attributeList = {{"a_position", gfx::Format::RG32F, false, 0, false, 0}};

    gfx::ShaderInfo shaderInfo;
    shaderInfo.name = "Multithread";
    shaderInfo.stages = std::move(shaderStageList);
    shaderInfo.attributes = std::move(attributeList);
    shaderInfo.blocks = std::move(uniformBlockList);
    _shader = _device->createShader(shaderInfo);
}

void Multithread::createVertexBuffer() {
    float vertexData[] = {-1.f, -.995f,
                          -1.f, -1.f,
                          -.995f, -.995f,
                          -.995f, -1.f};

    gfx::BufferInfo vertexBufferInfo = {
        gfx::BufferUsage::VERTEX,
        gfx::MemoryUsage::DEVICE,
        sizeof(vertexData),
        2 * sizeof(float),
    };

    _vertexBuffer = _device->createBuffer(vertexBufferInfo);
    _vertexBuffer->update(vertexData, 0, sizeof(vertexData));

    _worldBufferStride = TestBaseI::getAlignedUBOStride(_device, sizeof(Vec4));
    gfx::BufferInfo uniformBufferWInfo = {
        gfx::BufferUsage::UNIFORM,
        gfx::MemoryUsage::DEVICE | gfx::MemoryUsage::HOST,
        TestBaseI::getUBOSize(_worldBufferStride * MODELS_PER_LINE * MODELS_PER_LINE),
        _worldBufferStride,
    };
    _uniformBufferWorld = _device->createBuffer(uniformBufferWInfo);

    uint stride = _worldBufferStride / sizeof(float);
    vector<float> buffer(stride * MODELS_PER_LINE * MODELS_PER_LINE);
    for (uint i = 0u, idx = 0u; i < MODELS_PER_LINE; i++) {
        for (uint j = 0u; j < MODELS_PER_LINE; j++, idx++) {
            buffer[idx * stride] = 2.f * j / MODELS_PER_LINE;
            buffer[idx * stride + 1] = 2.f * i / MODELS_PER_LINE;
        }
    }
    _uniformBufferWorld->update(buffer.data(), 0, buffer.size() * sizeof(float));

    gfx::BufferViewInfo worldBufferViewInfo = {
        _uniformBufferWorld,
        0,
        sizeof(Vec4),
    };
    _uniformBufferWorldView = _device->createBuffer(worldBufferViewInfo);

    gfx::BufferInfo uniformBufferVPInfo = {
        gfx::BufferUsage::UNIFORM,
        gfx::MemoryUsage::DEVICE | gfx::MemoryUsage::HOST,
        TestBaseI::getUBOSize(sizeof(Mat4)),
    };
    _uniformBufferVP = _device->createBuffer(uniformBufferVPInfo);

    Mat4 VP;
    TestBaseI::createOrthographic(-1, 1, -1, 1, -1, 1, &VP);
    _uniformBufferVP->update(VP.m, 0, sizeof(Mat4));
}

void Multithread::createInputAssembler() {
    gfx::Attribute position = {"a_position", gfx::Format::RG32F, false, 0, false};
    gfx::InputAssemblerInfo inputAssemblerInfo;
    inputAssemblerInfo.attributes.emplace_back(std::move(position));
    inputAssemblerInfo.vertexBuffers.emplace_back(_vertexBuffer);
    _inputAssembler = _device->createInputAssembler(inputAssemblerInfo);
}

void Multithread::createPipeline() {
    gfx::DescriptorSetLayoutInfo dslInfo;
    dslInfo.bindings.push_back({0, gfx::DescriptorType::UNIFORM_BUFFER, 1, gfx::ShaderStageFlagBit::VERTEX});
    dslInfo.bindings.push_back({1, gfx::DescriptorType::DYNAMIC_UNIFORM_BUFFER, 1, gfx::ShaderStageFlagBit::VERTEX});
    _descriptorSetLayout = _device->createDescriptorSetLayout(dslInfo);

    _pipelineLayout = _device->createPipelineLayout({{_descriptorSetLayout}});

    _descriptorSet = _device->createDescriptorSet({_descriptorSetLayout});
    _descriptorSet->bindBuffer(0, _uniformBufferVP);
    _descriptorSet->bindBuffer(1, _uniformBufferWorldView);
    _descriptorSet->update();

    gfx::PipelineStateInfo pipelineInfo;
    pipelineInfo.primitive = gfx::PrimitiveMode::TRIANGLE_STRIP;
    pipelineInfo.shader = _shader;
    pipelineInfo.blendState.targets.push_back(new gfx::BlendTarget());
    pipelineInfo.inputState = {_inputAssembler->getAttributes()};
    pipelineInfo.renderPass = _fbo->getRenderPass();
    pipelineInfo.pipelineLayout = _pipelineLayout;

    _pipelineState = _device->createPipelineState(pipelineInfo);
}

void Multithread::tick(float dt) {

    _timeAccumulator = _timeAccumulator * 0.95f + dt * 0.05f;
    _frameAccumulator++;
    if (_frameAccumulator % 6 == 0) {
        CC_LOG_INFO("CPU avg: %.2f (~%d FPS)", _timeAccumulator * 1000.f, uint(1.f / _timeAccumulator));
    }

    gfx::Color clearColor = {.2f, .2f, .2f, 1.f};

    _device->acquire();

    /* un-toggle this to support dynamic screen rotation *
    Mat4 VP;
    TestBaseI::createOrthographic(-1, 1, -1, 1, -1, 1, &VP);
    _uniformBufferVP->update(VP.m, 0, sizeof(Mat4));
    /* */

    gfx::Rect renderArea = {0, 0, _device->getWidth(), _device->getHeight()};

    auto commandBuffer = _commandBuffers[0];
    commandBuffer->begin();
    commandBuffer->beginRenderPass(_fbo->getRenderPass(), _fbo, renderArea, &clearColor, 1.0f, 0);
    commandBuffer->bindInputAssembler(_inputAssembler);
    commandBuffer->bindPipelineState(_pipelineState);

    uint dynamicOffset = 0u;
    for (uint i = 0; i < MODELS_PER_LINE * MODELS_PER_LINE; i++) {
        dynamicOffset = i * _worldBufferStride;
        commandBuffer->bindDescriptorSet(0, _descriptorSet, 1, &dynamicOffset);
        commandBuffer->draw(_inputAssembler);
    }
    commandBuffer->endRenderPass();
    commandBuffer->end();

    _device->getQueue()->submit(_commandBuffers);
    _device->present();
}

} // namespace cc
