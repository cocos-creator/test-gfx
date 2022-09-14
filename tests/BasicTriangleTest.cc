#include "BasicTriangleTest.h"
#include "gfx-base/GFXDef-common.h"

namespace cc {

void BasicTriangle::onDestroy() {
    CC_SAFE_DESTROY_AND_DELETE(_vertexBuffer);
    CC_SAFE_DESTROY_AND_DELETE(_inputAssembler);
    CC_SAFE_DESTROY_AND_DELETE(_uniformBuffer);
    CC_SAFE_DESTROY_AND_DELETE(_uniformBufferMVP);
    CC_SAFE_DESTROY_AND_DELETE(_shader);
    CC_SAFE_DESTROY_AND_DELETE(_descriptorSet);
    CC_SAFE_DESTROY_AND_DELETE(_descriptorSetLayout);
    CC_SAFE_DESTROY_AND_DELETE(_pipelineLayout);
    CC_SAFE_DESTROY_AND_DELETE(_pipelineState);
    CC_SAFE_DESTROY_AND_DELETE(_invisiblePipelineState);
    CC_SAFE_DESTROY_AND_DELETE(_indexBuffer);
    CC_SAFE_DESTROY_AND_DELETE(_indirectBuffer);
}

bool BasicTriangle::onInit() {
    createShader();
    createVertexBuffer();
    createInputAssembler();
    createPipeline();

    return true;
}

void BasicTriangle::createShader() {
    ShaderSources<StandardShaderSource> sources;
    sources.glsl4 = {
        R"(
            precision highp float;
            layout(location = 0) in vec2 a_position;
            layout(set = 0, binding = 1) uniform MVP { mat4 u_MVP; };

            void main() {
                gl_Position = u_MVP * vec4(a_position, 0.0, 1.0);
            }
        )",
        R"(
            precision highp float;
            layout(set = 0, binding = 0) uniform Color {
                vec4 u_color;
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
            layout(std140) uniform MVP { mat4 u_MVP; };

            void main() {
                gl_Position = u_MVP * vec4(a_position, 0.0, 1.0);
            }
        )",
        R"(
            precision mediump float;
            layout(std140) uniform Color {
                vec4 u_color;
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
            uniform mat4 u_MVP;

            void main() {
                gl_Position = u_MVP * vec4(a_position, 0.0, 1.0);
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
        {0, 0, "Color", {{"u_color", gfx::Type::FLOAT4, 1}}, 1},
        {0, 1, "MVP", {{"u_MVP", gfx::Type::MAT4, 1}}, 1},
    };
    gfx::AttributeList attributeList = {{"a_position", gfx::Format::RG32F, false, 0, false, 0}};

    gfx::ShaderInfo shaderInfo;
    shaderInfo.name       = "Basic Triangle";
    shaderInfo.stages     = std::move(shaderStageList);
    shaderInfo.attributes = std::move(attributeList);
    shaderInfo.blocks     = std::move(uniformBlockList);
    _shader               = device->createShader(shaderInfo);
}

void BasicTriangle::createVertexBuffer() {
    float vertexData[] = {-0.5F, 0.5F,
                          -0.5F, -0.5F,
                          0.5F, -0.5F,
                          0.0F, 0.5F,
                          0.5F, 0.5F};

    gfx::BufferInfo vertexBufferInfo = {
        gfx::BufferUsage::VERTEX,
        gfx::MemoryUsage::DEVICE,
        sizeof(vertexData),
        2 * sizeof(float),
        gfx::BufferFlagBit::NONE,
    };

    _vertexBuffer = device->createBuffer(vertexBufferInfo);
    _vertexBuffer->update(vertexData, sizeof(vertexData));

    gfx::BufferInfo uniformBufferInfo = {
        gfx::BufferUsage::UNIFORM,
        gfx::MemoryUsage::DEVICE | gfx::MemoryUsage::HOST,
        TestBaseI::getUBOSize(sizeof(gfx::Color)),
    };
    _uniformBuffer = device->createBuffer(uniformBufferInfo);

    gfx::BufferInfo uniformBufferMVPInfo = {
        gfx::BufferUsage::UNIFORM,
        gfx::MemoryUsage::DEVICE | gfx::MemoryUsage::HOST,
        TestBaseI::getUBOSize(sizeof(Mat4)),
    };
    _uniformBufferMVP = device->createBuffer(uniformBufferMVPInfo);

    uint16_t        indices[]       = {1, 3, 0, 1, 2, 3, 2, 4, 3};
    gfx::BufferInfo indexBufferInfo = {
        gfx::BufferUsageBit::INDEX,
        gfx::MemoryUsage::DEVICE,
        sizeof(indices),
        sizeof(uint16_t),
    };
    _indexBuffer = device->createBuffer(indexBufferInfo);
    _indexBuffer->update(indices, sizeof(indices));

    gfx::DrawInfo drawInfo;
    drawInfo.firstIndex = 3;
    drawInfo.indexCount = 3;

    gfx::BufferInfo indirectBufferInfo = {
        gfx::BufferUsageBit::INDIRECT,
        gfx::MemoryUsage::DEVICE,
        sizeof(gfx::DrawInfo),
        sizeof(gfx::DrawInfo),
    };
    _indirectBuffer = device->createBuffer(indirectBufferInfo);
    _indirectBuffer->update(&drawInfo, sizeof(gfx::DrawInfo));
}

void BasicTriangle::createInputAssembler() {
    gfx::Attribute          position = {"a_position", gfx::Format::RG32F, false, 0, false};
    gfx::InputAssemblerInfo inputAssemblerInfo;
    inputAssemblerInfo.attributes.emplace_back(std::move(position));
    inputAssemblerInfo.vertexBuffers.emplace_back(_vertexBuffer);
    inputAssemblerInfo.indexBuffer    = _indexBuffer;
    inputAssemblerInfo.indirectBuffer = _indirectBuffer;
    _inputAssembler                   = device->createInputAssembler(inputAssemblerInfo);
}

void BasicTriangle::createPipeline() {
    gfx::DescriptorSetLayoutInfo dslInfo;
    dslInfo.bindings.push_back({0, gfx::DescriptorType::UNIFORM_BUFFER, 1, gfx::ShaderStageFlagBit::FRAGMENT});
    dslInfo.bindings.push_back({1, gfx::DescriptorType::UNIFORM_BUFFER, 1, gfx::ShaderStageFlagBit::VERTEX});
    _descriptorSetLayout = device->createDescriptorSetLayout(dslInfo);

    _pipelineLayout = device->createPipelineLayout({{_descriptorSetLayout}});

    _descriptorSet = device->createDescriptorSet({_descriptorSetLayout});

    _descriptorSet->bindBuffer(0, _uniformBuffer);
    _descriptorSet->bindBuffer(1, _uniformBufferMVP);
    _descriptorSet->update();

    gfx::PipelineStateInfo pipelineInfo;
    pipelineInfo.primitive      = gfx::PrimitiveMode::TRIANGLE_LIST;
    pipelineInfo.shader         = _shader;
    pipelineInfo.inputState     = {_inputAssembler->getAttributes()};
    pipelineInfo.renderPass     = renderPass;
    pipelineInfo.pipelineLayout = _pipelineLayout;

    _pipelineState = device->createPipelineState(pipelineInfo);

    pipelineInfo.blendState.targets[0].blendColorMask = gfx::ColorMask::NONE;

    _invisiblePipelineState = device->createPipelineState(pipelineInfo);

    _generalBarriers.push_back(device->getGeneralBarrier({
        gfx::AccessFlagBit::TRANSFER_WRITE,
        gfx::AccessFlagBit::VERTEX_SHADER_READ_UNIFORM_BUFFER |
            gfx::AccessFlagBit::FRAGMENT_SHADER_READ_UNIFORM_BUFFER |
            gfx::AccessFlagBit::INDIRECT_BUFFER |
            gfx::AccessFlagBit::VERTEX_BUFFER |
            gfx::AccessFlagBit::INDEX_BUFFER,
    }));

    _generalBarriers.push_back(device->getGeneralBarrier({
        gfx::AccessFlagBit::TRANSFER_WRITE,
        gfx::AccessFlagBit::VERTEX_SHADER_READ_UNIFORM_BUFFER |
            gfx::AccessFlagBit::FRAGMENT_SHADER_READ_UNIFORM_BUFFER,
    }));
}

#define TEST_OCCLUSION_QUERY 0

void BasicTriangle::onTick() {
    auto *swapchain = swapchains[0];
    auto *fbo       = fbos[0];

    uint generalBarrierIdx = _frameCount ? 1 : 0;

    gfx::Color clearColor = {1.0F, 0, 0, 1.0F};

    gfx::Color uniformColor;
    uniformColor.x = std::abs(std::sin(_time));
    uniformColor.y = 1.0F;
    uniformColor.z = 0.0F;
    uniformColor.w = 1.0F;

    Mat4 mvp;
    TestBaseI::createOrthographic(-1, 1, -1, 1, -1, 1, &mvp, swapchain);

#if TEST_OCCLUSION_QUERY
    Mat4  translate;
    float speed{1.F};
    Mat4::createTranslation({std::cos(_time * speed) - 1.F, std::sin(_time * speed) - 1.F, 0}, &translate);
    mvp = translate * mvp;
#endif

    device->acquire(&swapchain, 1);

    _uniformBuffer->update(&uniformColor, sizeof(uniformColor));
    _uniformBufferMVP->update(mvp.m, sizeof(Mat4));

    gfx::Rect renderArea = {0, 0, swapchain->getWidth(), swapchain->getHeight()};

    auto *commandBuffer = commandBuffers[0];
    commandBuffer->begin();

    if (TestBaseI::MANUAL_BARRIER) {
        commandBuffer->pipelineBarrier(_generalBarriers[generalBarrierIdx]);
    }

#if TEST_OCCLUSION_QUERY
    static bool visible{false};
    auto *      queryPool = device->getQueryPool();
    device->getQueryPoolResults(queryPool);
    commandBuffer->resetQueryPool(queryPool);

    commandBuffer->beginRenderPass(fbo->getRenderPass(), fbo, renderArea, &clearColor, 1.0F, 0);
    commandBuffer->beginQuery(queryPool, 0);
    commandBuffer->bindInputAssembler(_inputAssembler);
    commandBuffer->bindDescriptorSet(0, _descriptorSet);

    if (queryPool->hasResult(0) && !queryPool->getResult(0)) { // invisible last frame
        commandBuffer->bindPipelineState(_invisiblePipelineState);
        if (visible) CC_LOG_DEBUG("occlusion query result: %d", visible = false);
    } else { // visible
        commandBuffer->bindPipelineState(_pipelineState);
        if (!visible) CC_LOG_DEBUG("occlusion query result: %d", visible = true);
    }

    commandBuffer->draw(_inputAssembler);
    commandBuffer->endQuery(queryPool, 0);
    commandBuffer->endRenderPass();

    commandBuffer->completeQueryPool(queryPool);
#else
    commandBuffer->beginRenderPass(fbo->getRenderPass(), fbo, renderArea, &clearColor, 1.0F, 0);
    commandBuffer->bindPipelineState(_pipelineState);
    commandBuffer->bindInputAssembler(_inputAssembler);
    commandBuffer->bindDescriptorSet(0, _descriptorSet);
    commandBuffer->draw(_inputAssembler);
    commandBuffer->endRenderPass();
#endif

    commandBuffer->end();

    device->flushCommands(commandBuffers);
    device->getQueue()->submit(commandBuffers);
    device->present();
}

} // namespace cc
