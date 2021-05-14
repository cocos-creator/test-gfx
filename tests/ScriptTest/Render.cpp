#include "Transform.h"

#include "../TestBase.h"
#include "bindings/jswrapper/v8/ScriptEngine.h"
#include "tests/ScriptTest/Math.h"

namespace cc {

gfx::Shader *                Model::shader{nullptr};
gfx::Buffer *                Model::vertexBuffer{nullptr};
gfx::Buffer *                Model::uniformBuffer{nullptr};
gfx::Buffer *                Model::uniformBufferView{nullptr};
gfx::DescriptorSet *         Model::descriptorSet{nullptr};
gfx::DescriptorSetLayout *   Model::descriptorSetLayout{nullptr};
gfx::PipelineLayout *        Model::pipelineLayout{nullptr};
gfx::PipelineState *         Model::pipelineState{nullptr};
gfx::InputAssembler *        Model::inputAssembler{nullptr};
vector<gfx::GlobalBarrier *> Model::globalBarriers;

namespace {
vector<float> uniformBufferData;
uint          uboStride     = 0U;
uint          modelCapacity = 8U;
} // namespace

void Root::initialize() {
    gfx::Device *device = gfx::Device::getInstance();

    ShaderSources<StandardShaderSource> sources;
    sources.glsl4 = {
        R"(
            precision highp float;
            layout(location = 0) in vec2 a_position;
            layout(set = 0, binding = 0) uniform Constants {
                vec4 u_color;
                mat4 u_worldView;
                mat4 u_project;
            };

            void main() {
                gl_Position = u_project * u_worldView * vec4(a_position, 0.0, 1.0);
            }
        )",
        R"(
            precision highp float;
            layout(set = 0, binding = 0) uniform Constants {
                vec4 u_color;
                mat4 u_worldView;
                mat4 u_project;
            };
            layout(location = 0) out vec4 o_color;

            void main() {
                o_color = u_color;
            }
        )",
    };

    sources.glsl3 = {
        R"(
            in vec2 a_position;
            layout(std140) uniform Constants {
                vec4 u_color;
                mat4 u_worldView;
                mat4 u_project;
            };

            void main() {
                gl_Position = u_project * u_worldView * vec4(a_position, 0.0, 1.0);
            }
        )",
        R"(
            precision mediump float;
            layout(std140) uniform Constants {
                vec4 u_color;
                mat4 u_worldView;
                mat4 u_project;
            };

            out vec4 o_color;
            void main() {
                o_color = u_color;
            }
        )",
    };

    sources.glsl1 = {
        R"(
            attribute vec2 a_position;
            uniform vec4 u_color;
            uniform mat4 u_worldView;
            uniform mat4 u_project;

            void main() {
                gl_Position = u_project * u_worldView * vec4(a_position, 0.0, 1.0);
            }
        )",
        R"(
            precision mediump float;
            uniform vec4 u_color;
            void main() {
                gl_FragColor = u_color;
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
            "Constants",
            {
                {"u_color", gfx::Type::FLOAT4, 1},
                {"u_worldView", gfx::Type::MAT4, 1},
                {"u_project", gfx::Type::MAT4, 1},
            },
            1,
        },
    };
    gfx::AttributeList attributeList = {{"a_position", gfx::Format::RG32F, false, 0, false, 0}};

    gfx::ShaderInfo shaderInfo;
    shaderInfo.name       = "Basic Triangle";
    shaderInfo.stages     = std::move(shaderStageList);
    shaderInfo.attributes = std::move(attributeList);
    shaderInfo.blocks     = std::move(uniformBlockList);
    Model::shader         = device->createShader(shaderInfo);

    float vertexData[] = {-0.02F, -0.04F,
                          0.02F, -0.04F,
                          0.0F, 0.04F};

    gfx::BufferInfo vertexBufferInfo = {
        gfx::BufferUsage::VERTEX,
        gfx::MemoryUsage::DEVICE,
        sizeof(vertexData),
        2 * sizeof(float),
        gfx::BufferFlagBit::NONE,
    };

    Model::vertexBuffer = device->createBuffer(vertexBufferInfo);
    Model::vertexBuffer->update(vertexData, sizeof(vertexData));

    constexpr uint memberCount = 4 + 16 + 16;
    uboStride                  = TestBaseI::getAlignedUBOStride(sizeof(float) * memberCount);

    uniformBufferData.resize(uboStride * modelCapacity / sizeof(float));

    gfx::BufferInfo uniformBufferInfo = {
        gfx::BufferUsage::UNIFORM,
        gfx::MemoryUsage::DEVICE | gfx::MemoryUsage::HOST,
        uboStride * modelCapacity,
    };
    Model::uniformBuffer = device->createBuffer(uniformBufferInfo);

    gfx::BufferViewInfo uniformBufferViewInfo = {
        Model::uniformBuffer,
        0U,
        sizeof(float) * memberCount,
    };
    Model::uniformBufferView = device->createBuffer(uniformBufferViewInfo);

    gfx::Attribute          position{"a_position", gfx::Format::RG32F, false, 0, false};
    gfx::InputAssemblerInfo inputAssemblerInfo;
    inputAssemblerInfo.attributes.emplace_back(std::move(position));
    inputAssemblerInfo.vertexBuffers.emplace_back(Model::vertexBuffer);
    Model::inputAssembler = device->createInputAssembler(inputAssemblerInfo);

    gfx::DescriptorSetLayoutInfo dslInfo;
    dslInfo.bindings.push_back({
        0,
        gfx::DescriptorType::DYNAMIC_UNIFORM_BUFFER,
        1,
        gfx::ShaderStageFlagBit::VERTEX | gfx::ShaderStageFlagBit::FRAGMENT,
    });
    Model::descriptorSetLayout = device->createDescriptorSetLayout(dslInfo);

    Model::pipelineLayout = device->createPipelineLayout({{Model::descriptorSetLayout}});

    Model::descriptorSet = device->createDescriptorSet({Model::descriptorSetLayout});

    Model::descriptorSet->bindBuffer(0, Model::uniformBufferView);
    Model::descriptorSet->update();

    gfx::PipelineStateInfo pipelineInfo;
    pipelineInfo.primitive      = gfx::PrimitiveMode::TRIANGLE_LIST;
    pipelineInfo.shader         = Model::shader;
    pipelineInfo.inputState     = {Model::inputAssembler->getAttributes()};
    pipelineInfo.renderPass     = TestBaseI::fbo->getRenderPass();
    pipelineInfo.pipelineLayout = Model::pipelineLayout;

    Model::pipelineState = device->createPipelineState(pipelineInfo);

    Model::globalBarriers.push_back(TestBaseI::getGlobalBarrier({
        {
            gfx::AccessType::TRANSFER_WRITE,
        },
        {
            gfx::AccessType::VERTEX_SHADER_READ_UNIFORM_BUFFER,
            gfx::AccessType::FRAGMENT_SHADER_READ_UNIFORM_BUFFER,
            gfx::AccessType::VERTEX_BUFFER,
        },
    }));
}

void Root::destroy() {
    CC_SAFE_DESTROY(Model::vertexBuffer);
    CC_SAFE_DESTROY(Model::inputAssembler);
    CC_SAFE_DESTROY(Model::uniformBuffer);
    CC_SAFE_DESTROY(Model::uniformBufferView);
    CC_SAFE_DESTROY(Model::shader);
    CC_SAFE_DESTROY(Model::descriptorSet);
    CC_SAFE_DESTROY(Model::descriptorSetLayout);
    CC_SAFE_DESTROY(Model::pipelineLayout);
    CC_SAFE_DESTROY(Model::pipelineState);
    Model::globalBarriers.clear();
}

void Root::render() {
    gfx::Device *device     = gfx::Device::getInstance();
    gfx::Color   clearColor = {0, 0, 0, 1.F};

    Mat4 projection;
    TestBaseI::createOrthographic(-1, 1, -1, 1, -1, 1, &projection);

    if (_models.size() > modelCapacity) {
        modelCapacity = utils::nextPOT(_models.size());
        Model::uniformBuffer->resize(modelCapacity * uboStride);
        uniformBufferData.resize(modelCapacity * uboStride / sizeof(float));
    }

    for (uint i = 0U; i < _models.size(); ++i) {
        auto *model = _models[i];
        vmath::store(&uniformBufferData[i * uboStride / sizeof(float)], model->getColor());
        vmath::store(&uniformBufferData[i * uboStride / sizeof(float) + 4], model->getTransform()->getWorldMatrix());
        memcpy(&uniformBufferData[i * uboStride / sizeof(float) + 20], &projection.m, sizeof(float) * 16);
    }

    device->acquire();

    if (!_models.empty()) {
        Model::uniformBuffer->update(uniformBufferData.data(), _models.size() * uboStride);
    }

    gfx::Rect renderArea = {0, 0, device->getWidth(), device->getHeight()};

    auto *commandBuffer = TestBaseI::commandBuffers[0];
    commandBuffer->begin();

    if (TestBaseI::MANUAL_BARRIER) {
        commandBuffer->pipelineBarrier(Model::globalBarriers[0]);
    }

    commandBuffer->beginRenderPass(TestBaseI::fbo->getRenderPass(), TestBaseI::fbo, renderArea, &clearColor, 1.F, 0);

    if (!_models.empty()) {
        commandBuffer->bindPipelineState(Model::pipelineState);
        commandBuffer->bindInputAssembler(Model::inputAssembler);
    }

    uint dynamicOffset = 0U;
    for (uint i = 0U; i < _models.size(); ++i) {
        dynamicOffset = i * uboStride;
        commandBuffer->bindDescriptorSet(0, Model::descriptorSet, 1, &dynamicOffset);
        commandBuffer->draw(Model::inputAssembler);
    }

    commandBuffer->endRenderPass();
    commandBuffer->end();

    device->flushCommands(TestBaseI::commandBuffers);
    device->getQueue()->submit(TestBaseI::commandBuffers);
    device->present();
}

} // namespace cc
