#include "Chassis.h"

#include "../TestBase.h"
#include "bindings/jswrapper/v8/ScriptEngine.h"
#include "tests/ScriptTest/Math.h"

namespace cc {

namespace {
// fix-functioned resources just for test purposes
gfx::Shader *                shader{nullptr};
gfx::Buffer *                vertexBuffer{nullptr};
gfx::Buffer *                uniformBufferGlobal{nullptr};
gfx::Buffer *                uniformBuffer{nullptr};
gfx::Buffer *                uniformBufferView{nullptr};
gfx::DescriptorSet *         descriptorSet{nullptr};
gfx::DescriptorSetLayout *   descriptorSetLayout{nullptr};
gfx::PipelineLayout *        pipelineLayout{nullptr};
gfx::PipelineState *         pipelineState{nullptr};
gfx::InputAssembler *        inputAssembler{nullptr};
vector<gfx::GlobalBarrier *> globalBarriers;
vector<float>                uniformBufferData;
uint                         uboStride     = 0U;
uint                         modelCapacity = Root::INITIAL_CAPACITY;
vmath::IndexP                index;

gfx::Buffer *        vertexBufferOutline{nullptr};
gfx::PipelineState * pipelineStateOutline{nullptr};
gfx::InputAssembler *inputAssemblerOutline{nullptr};
} // namespace

void Root::initialize() {
    gfx::Device *device = gfx::Device::getInstance();

    ShaderSources<StandardShaderSource> sources;
    sources.glsl4 = {
        R"(
            precision highp float;
            layout(location = 0) in vec3 a_position;
            layout(set = 0, binding = 0) uniform Constants {
                vec4 u_color;
                mat4 u_worldView;
            };
            layout(set = 0, binding = 1) uniform Global {
                mat4 u_project;
            };

            void main() {
                gl_Position = u_project * u_worldView * vec4(a_position, 1.0);
            }
        )",
        R"(
            precision highp float;
            layout(set = 0, binding = 0) uniform Constants {
                vec4 u_color;
                mat4 u_worldView;
            };
            layout(location = 0) out vec4 o_color;

            void main() {
                o_color = u_color;
            }
        )",
    };

    sources.glsl3 = {
        R"(
            in vec3 a_position;
            layout(std140) uniform Constants {
                vec4 u_color;
                mat4 u_worldView;
            };
            layout(std140) uniform Global {
                mat4 u_project;
            };

            void main() {
                gl_Position = u_project * u_worldView * vec4(a_position, 1.0);
            }
        )",
        R"(
            precision mediump float;
            layout(std140) uniform Constants {
                vec4 u_color;
                mat4 u_worldView;
            };

            out vec4 o_color;
            void main() {
                o_color = u_color;
            }
        )",
    };

    sources.glsl1 = {
        R"(
            attribute vec3 a_position;
            uniform vec4 u_color;
            uniform mat4 u_worldView;
            uniform mat4 u_project;

            void main() {
                gl_Position = u_project * u_worldView * vec4(a_position, 1.0);
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
            },
            1,
        },
        {
            0,
            1,
            "Global",
            {
                {"u_project", gfx::Type::MAT4, 1},
            },
            1,
        },
    };
    gfx::AttributeList attributeList = {{"a_position", gfx::Format::RGB32F, false, 0, false, 0}};

    gfx::ShaderInfo shaderInfo;
    shaderInfo.name       = "Basic Triangle";
    shaderInfo.stages     = std::move(shaderStageList);
    shaderInfo.attributes = std::move(attributeList);
    shaderInfo.blocks     = std::move(uniformBlockList);
    shader                = device->createShader(shaderInfo);

    float vertexData[] = {-0.02F, -0.04F, 0.F,
                          0.02F, -0.04F, 0.F,
                          0.0F, 0.04F, 0.F};

    gfx::BufferInfo vertexBufferInfo = {
        gfx::BufferUsage::VERTEX,
        gfx::MemoryUsage::DEVICE,
        sizeof(vertexData),
        3 * sizeof(float),
    };

    vertexBuffer = device->createBuffer(vertexBufferInfo);
    vertexBuffer->update(vertexData, sizeof(vertexData));

    constexpr uint memberCount = 4 + 16;

    uboStride = TestBaseI::getAlignedUBOStride(sizeof(float) * memberCount);

    uniformBufferData.resize(uboStride * (modelCapacity + 1) / sizeof(float));

    gfx::BufferInfo uniformBufferInfo = {
        gfx::BufferUsage::UNIFORM,
        gfx::MemoryUsage::DEVICE | gfx::MemoryUsage::HOST,
        uboStride * (modelCapacity + 1),
    };
    uniformBuffer = device->createBuffer(uniformBufferInfo);

    gfx::BufferInfo uniformBufferGlobalInfo = {
        gfx::BufferUsage::UNIFORM,
        gfx::MemoryUsage::DEVICE | gfx::MemoryUsage::HOST,
        sizeof(float) * 16,
    };
    uniformBufferGlobal = device->createBuffer(uniformBufferGlobalInfo);

    Mat4 view;
    Mat4 projection;
    Mat4::createLookAt({1.F, 1.F, 3.F}, {0.F, 0.F, .5F}, {0.F, 1.F, 0.F}, &view);
    TestBaseI::createOrthographic(-1.5F, 1.5F, -1.5F, 1.5F, 1.F, 10.F, &projection);
    projection = projection * view;
    uniformBufferGlobal->update(projection.m, sizeof(projection));

    gfx::BufferViewInfo uniformBufferViewInfo = {
        uniformBuffer,
        0U,
        sizeof(float) * memberCount,
    };
    uniformBufferView = device->createBuffer(uniformBufferViewInfo);

    gfx::Attribute          position{"a_position", gfx::Format::RGB32F, false, 0, false};
    gfx::InputAssemblerInfo inputAssemblerInfo;
    inputAssemblerInfo.attributes.emplace_back(std::move(position));
    inputAssemblerInfo.vertexBuffers.emplace_back(vertexBuffer);
    inputAssembler = device->createInputAssembler(inputAssemblerInfo);

    gfx::DescriptorSetLayoutInfo dslInfo;
    dslInfo.bindings.push_back({
        0,
        gfx::DescriptorType::DYNAMIC_UNIFORM_BUFFER,
        1,
        gfx::ShaderStageFlagBit::VERTEX | gfx::ShaderStageFlagBit::FRAGMENT,
    });
    dslInfo.bindings.push_back({
        1,
        gfx::DescriptorType::UNIFORM_BUFFER,
        1,
        gfx::ShaderStageFlagBit::VERTEX,
    });
    descriptorSetLayout = device->createDescriptorSetLayout(dslInfo);

    pipelineLayout = device->createPipelineLayout({{descriptorSetLayout}});

    descriptorSet = device->createDescriptorSet({descriptorSetLayout});

    descriptorSet->bindBuffer(0, uniformBufferView);
    descriptorSet->bindBuffer(1, uniformBufferGlobal);
    descriptorSet->update();

    gfx::PipelineStateInfo pipelineInfo;
    pipelineInfo.primitive                           = gfx::PrimitiveMode::TRIANGLE_LIST;
    pipelineInfo.shader                              = shader;
    pipelineInfo.inputState                          = {inputAssembler->getAttributes()};
    pipelineInfo.renderPass                          = TestBaseI::fbo->getRenderPass();
    pipelineInfo.rasterizerState.cullMode            = gfx::CullMode::NONE;
    pipelineInfo.depthStencilState.depthTest         = false;
    pipelineInfo.depthStencilState.depthWrite        = false;
    pipelineInfo.blendState.targets[0].blend         = true;
    pipelineInfo.blendState.targets[0].blendSrc      = gfx::BlendFactor::SRC_ALPHA;
    pipelineInfo.blendState.targets[0].blendDst      = gfx::BlendFactor::ONE_MINUS_SRC_ALPHA;
    pipelineInfo.blendState.targets[0].blendSrcAlpha = gfx::BlendFactor::ONE;
    pipelineInfo.blendState.targets[0].blendDstAlpha = gfx::BlendFactor::ONE_MINUS_SRC_ALPHA;
    pipelineInfo.pipelineLayout                      = pipelineLayout;

    pipelineState = device->createPipelineState(pipelineInfo);

    globalBarriers.push_back(TestBaseI::getGlobalBarrier({
        {
            gfx::AccessType::TRANSFER_WRITE,
        },
        {
            gfx::AccessType::VERTEX_SHADER_READ_UNIFORM_BUFFER,
            gfx::AccessType::FRAGMENT_SHADER_READ_UNIFORM_BUFFER,
            gfx::AccessType::VERTEX_BUFFER,
        },
    }));

    auto max = static_cast<int>((vmath::IndexP::Size - 1) * uboStride / sizeof(float));
    index    = vmath::linspace<vmath::IndexP>(0, max);

    // bounding box outline

    float vertexDataOutline[] = {-1.F, -1.F, 0.F,
                                 -1.F, 1.F, 0.F,
                                 -1.F, 1.F, 0.F,
                                 1.F, 1.F, 0.F,
                                 1.F, 1.F, 0.F,
                                 1.F, -1.F, 0.F,
                                 1.F, -1.F, 0.F,
                                 -1.F, -1.F, 0.F,

                                 -1.F, -1.F, 1.F,
                                 -1.F, 1.F, 1.F,
                                 -1.F, 1.F, 1.F,
                                 1.F, 1.F, 1.F,
                                 1.F, 1.F, 1.F,
                                 1.F, -1.F, 1.F,
                                 1.F, -1.F, 1.F,
                                 -1.F, -1.F, 1.F,

                                 -1.F, -1.F, 1.F,
                                 -1.F, -1.F, 0.F,
                                 -1.F, 1.F, 1.F,
                                 -1.F, 1.F, 0.F,
                                 1.F, 1.F, 1.F,
                                 1.F, 1.F, 0.F,
                                 1.F, -1.F, 1.F,
                                 1.F, -1.F, 0.F};

    gfx::BufferInfo vertexBufferOutlineInfo = {
        gfx::BufferUsage::VERTEX,
        gfx::MemoryUsage::DEVICE,
        sizeof(vertexDataOutline),
        3 * sizeof(float),
    };

    vertexBufferOutline = device->createBuffer(vertexBufferOutlineInfo);
    vertexBufferOutline->update(vertexDataOutline, sizeof(vertexDataOutline));

    gfx::Color outlineColor{1.F, 1.F, 1.F, 1.F};
    memcpy(uniformBufferData.data(), &outlineColor, sizeof(outlineColor));
    memcpy(&uniformBufferData[4], Mat4::IDENTITY.m, sizeof(Mat4));

    gfx::InputAssemblerInfo inputAssemblerInfoOutline;
    inputAssemblerInfoOutline.attributes = inputAssemblerInfo.attributes;
    inputAssemblerInfoOutline.vertexBuffers.emplace_back(vertexBufferOutline);
    inputAssemblerOutline = device->createInputAssembler(inputAssemblerInfoOutline);

    gfx::PipelineStateInfo pipelineInfoOutline;
    pipelineInfoOutline.primitive      = gfx::PrimitiveMode::LINE_LIST;
    pipelineInfoOutline.shader         = shader;
    pipelineInfoOutline.inputState     = {inputAssemblerOutline->getAttributes()};
    pipelineInfoOutline.renderPass     = TestBaseI::fbo->getRenderPass();
    pipelineInfoOutline.pipelineLayout = pipelineLayout;

    pipelineStateOutline = device->createPipelineState(pipelineInfoOutline);
}

void Root::destroy() {
    CC_SAFE_DESTROY(vertexBuffer);
    CC_SAFE_DESTROY(inputAssembler);
    CC_SAFE_DESTROY(uniformBufferGlobal);
    CC_SAFE_DESTROY(uniformBuffer);
    CC_SAFE_DESTROY(uniformBufferView);
    CC_SAFE_DESTROY(shader);
    CC_SAFE_DESTROY(descriptorSet);
    CC_SAFE_DESTROY(descriptorSetLayout);
    CC_SAFE_DESTROY(pipelineLayout);
    CC_SAFE_DESTROY(pipelineState);

    CC_SAFE_DESTROY(vertexBufferOutline);
    CC_SAFE_DESTROY(pipelineStateOutline);
    CC_SAFE_DESTROY(inputAssemblerOutline);

    globalBarriers.clear();
}

void Root::render() {
    gfx::Device *device     = gfx::Device::getInstance();
    gfx::Color   clearColor = {.1F, .1F, .1F, 1.F};

    size_t modelCount = ModelView::viewCount;

    if (modelCount >= modelCapacity) {
        modelCapacity = utils::nextPOT(modelCount + 1);
        uniformBuffer->resize(modelCapacity * uboStride);
        uniformBufferData.resize(modelCapacity * uboStride / sizeof(float));
    }

    uint lengthPerPacket = uboStride / sizeof(float) * vmath::FloatP::Size;
    for (size_t i = 0; i < vmath::packets(ModelView::buffer); ++i) {
        float *pDst  = &uniformBufferData[i * lengthPerPacket + uboStride / sizeof(float)];
        auto &&model = vmath::packet(ModelView::buffer, i);
        vmath::scatter(pDst + 0, model.color.x(), index, model.enabled);
        vmath::scatter(pDst + 1, model.color.y(), index, model.enabled);
        vmath::scatter(pDst + 2, model.color.z(), index, model.enabled);
        vmath::scatter(pDst + 3, model.color.w(), index, model.enabled);
        for (size_t j = 0; j < model.transform.size(); ++j) {
            TransformView::views[model.transform[j]]->updateWorldTransform();
            vmath::store(pDst + 4, static_cast<TransformF::Mat4>(vmath::slice(TransformView::buffer.mat, model.transform[j])));
        }
    }

    device->acquire();

    if (modelCount) {
        uniformBuffer->update(uniformBufferData.data(), modelCount * uboStride);
    }

    gfx::Rect renderArea = {0, 0, device->getWidth(), device->getHeight()};

    auto *commandBuffer = TestBaseI::commandBuffers[0];
    commandBuffer->begin();

    if (TestBaseI::MANUAL_BARRIER) {
        commandBuffer->pipelineBarrier(globalBarriers[0]);
    }

    commandBuffer->beginRenderPass(TestBaseI::fbo->getRenderPass(), TestBaseI::fbo, renderArea, &clearColor, 1.F, 0);

    uint dynamicOffset = 0U;
    commandBuffer->bindPipelineState(pipelineStateOutline);
    commandBuffer->bindInputAssembler(inputAssemblerOutline);
    commandBuffer->bindDescriptorSet(0, descriptorSet, 1, &dynamicOffset);
    commandBuffer->draw(inputAssemblerOutline);

    if (modelCount) {
        commandBuffer->bindPipelineState(pipelineState);
        commandBuffer->bindInputAssembler(inputAssembler);
    }

    for (uint i = 0U; i < modelCount; ++i) {
        if (!vmath::slice(ModelView::buffer, i).enabled) continue;
        dynamicOffset = (i + 1) * uboStride;
        commandBuffer->bindDescriptorSet(0, descriptorSet, 1, &dynamicOffset);
        commandBuffer->draw(inputAssembler);
    }

    commandBuffer->endRenderPass();
    commandBuffer->end();

    device->flushCommands(TestBaseI::commandBuffers);
    device->getQueue()->submit(TestBaseI::commandBuffers);
    device->present();
}

} // namespace cc
