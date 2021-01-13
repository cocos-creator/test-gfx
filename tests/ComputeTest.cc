#include "ComputeTest.h"

namespace cc {

#define GROUP_SIZE   64
#define VERTEX_COUNT 200

void ComputeTest::destroy() {
    CC_SAFE_DESTROY(_inputAssembler);
    CC_SAFE_DESTROY(_uniformBufferMVP);
    CC_SAFE_DESTROY(_shader);
    CC_SAFE_DESTROY(_descriptorSet);
    CC_SAFE_DESTROY(_descriptorSetLayout);
    CC_SAFE_DESTROY(_pipelineLayout);
    CC_SAFE_DESTROY(_pipelineState);

    CC_SAFE_DESTROY(_compShader);
    CC_SAFE_DESTROY(_compRadiusBuffer);
    CC_SAFE_DESTROY(_compStorageBuffer);
    CC_SAFE_DESTROY(_compDescriptorSet);
    CC_SAFE_DESTROY(_compDescriptorSetLayout);
    CC_SAFE_DESTROY(_compPipelineLayout);
    CC_SAFE_DESTROY(_compPipelineState);
}

bool ComputeTest::initialize() {
    createComputePipeline();

    createShader();
    createUniformBuffer();
    createInputAssembler();
    createPipeline();

    return true;
}

void ComputeTest::createComputePipeline() {
    ShaderSources<ComputeShaderSource> sources;
    sources.glsl4 = StringUtil::Format(R"(
        #define GROUP_SIZE %du
        #define VERTEX_COUNT %du

        layout (local_size_x = GROUP_SIZE, local_size_y = 1, local_size_z = 1) in;

        struct AttribData { vec4 p; vec4 c; };
        layout(set = 0, binding = 0) buffer DestBuffer { vec4 vertex[]; };

        layout(set = 0, binding = 1) uniform Constants { float radius; };

        void main() {
            if (gl_GlobalInvocationID.x >= VERTEX_COUNT) return;

            int i = int(gl_GlobalInvocationID.x);
            float t = float(i) / float(VERTEX_COUNT);

            float alpha = 2.0 * 3.14159265359 * t;
            vertex[i * 2] = vec4(sin(alpha) * radius, cos(alpha) * radius, 0.0, 1.0);
            vertex[i * 2 + 1] = vec4(t, 0.5, 0.5, 1.0);
        }
    )", GROUP_SIZE, VERTEX_COUNT);
    sources.glsl3 = StringUtil::Format(R"(
        #define GROUP_SIZE %d
        #define VERTEX_COUNT %du

        layout (local_size_x = GROUP_SIZE, local_size_y = 1, local_size_z = 1) in;

        struct AttribData { vec4 p; vec4 c; };
        layout(std140, binding = 0) buffer DestBuffer { vec4 vertex[]; };

        layout(std140, binding = 1) uniform Constants { float radius; };

        void main() {
            if (gl_GlobalInvocationID.x >= VERTEX_COUNT) return;

            int i = int(gl_GlobalInvocationID.x);
            float t = float(i) / float(VERTEX_COUNT);

            float alpha = 2.0 * 3.14159265359 * t;
            vertex[i * 2] = vec4(sin(alpha) * radius, cos(alpha) * radius, 0.0, 1.0);
            vertex[i * 2 + 1] = vec4(t, 0.5, 0.5, 1.0);
        }
    )", GROUP_SIZE, VERTEX_COUNT);
    // no compute support in GLES2

    gfx::ShaderInfo shaderInfo;
    shaderInfo.name = "Compute Pass";
    shaderInfo.stages = {{gfx::ShaderStageFlagBit::COMPUTE, TestBaseI::getAppropriateShaderSource(sources)}};
    shaderInfo.buffers = {{0, 0, "DestBuffer", {{"vertex", gfx::Type::FLOAT4, 0}}, 1}};
    shaderInfo.blocks = {{0, 1, "Constants", {{"radius", gfx::Type::FLOAT, 1}}, 1}};
    _compShader = _device->createShader(shaderInfo);

    gfx::BufferInfo radiusBufferInfo = {
        gfx::BufferUsage::UNIFORM,
        gfx::MemoryUsage::DEVICE,
        TestBaseI::getUBOSize(sizeof(Vec4)),
    };
    _compRadiusBuffer = _device->createBuffer(radiusBufferInfo);

    Vec4 radius{.7f, 0.f, 0.f, 0.f};
    _compRadiusBuffer->update(&radius, sizeof(radius));

    gfx::BufferInfo storageBufferInfo = {
        gfx::BufferUsage::VERTEX | gfx::BufferUsage::STORAGE,
        gfx::MemoryUsage::DEVICE,
        VERTEX_COUNT * 2 * sizeof(Vec4),
        2 * sizeof(Vec4),
    };

    _compStorageBuffer = _device->createBuffer(storageBufferInfo);

    gfx::DescriptorSetLayoutInfo dslInfo;
    dslInfo.bindings.push_back({0, gfx::DescriptorType::STORAGE_BUFFER, 1, gfx::ShaderStageFlagBit::COMPUTE});
    dslInfo.bindings.push_back({1, gfx::DescriptorType::UNIFORM_BUFFER, 1, gfx::ShaderStageFlagBit::COMPUTE});
    _compDescriptorSetLayout = _device->createDescriptorSetLayout(dslInfo);

    _compPipelineLayout = _device->createPipelineLayout({{_compDescriptorSetLayout}});

    _compDescriptorSet = _device->createDescriptorSet({_compDescriptorSetLayout});

    _compDescriptorSet->bindBuffer(0, _compStorageBuffer);
    _compDescriptorSet->bindBuffer(1, _compRadiusBuffer);
    _compDescriptorSet->update();

    gfx::PipelineStateInfo pipelineInfo;
    pipelineInfo.shader = _compShader;
    pipelineInfo.pipelineLayout = _compPipelineLayout;
    pipelineInfo.bindPoint = gfx::PipelineBindPoint::COMPUTE;

    _compPipelineState = _device->createPipelineState(pipelineInfo);
}

void ComputeTest::createShader() {

    ShaderSources<StandardShaderSource> sources;
    sources.glsl4 = {
        R"(
            precision highp float;
            layout(location = 0) in vec4 a_position;
            layout(location = 1) in vec4 a_color;
            layout(set = 0, binding = 0) uniform MVP { mat4 u_MVP; };

            layout(location = 0) out vec4 v_color;

            void main() {
                v_color = a_color;
                gl_Position = u_MVP * a_position;
            }
        )",
        R"(
            precision highp float;
            layout(location = 0) in vec4 v_color;
            layout(location = 0) out vec4 o_color;

            void main() {
                o_color = v_color;
            }
        )",
    };

    sources.glsl3 = {
        R"(
            in vec4 a_position;
            in vec4 a_color;
            layout(std140) uniform MVP { mat4 u_MVP; };

            out vec4 v_color;

            void main() {
                v_color = a_color;
                gl_Position = u_MVP * a_position;
            }
        )",
        R"(
            precision mediump float;
            in vec4 v_color;
            out vec4 o_color;

            void main() {
                o_color = v_color;
            }
        )",
    };

    sources.glsl1 = {
        R"(
            attribute vec4 a_position;
            attribute vec4 a_color;
            uniform mat4 u_MVP;

            varying vec4 v_color;

            void main() {
                v_color = a_color;
                gl_Position = u_MVP * a_position;
            }
        )",
        R"(
            precision mediump float;
            varying vec4 v_color;
            void main() {
                gl_FragColor = v_color;
            }
        )",
    };

    StandardShaderSource &source = TestBaseI::getAppropriateShaderSource(sources);

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
        {0, 0, "MVP", {{"u_MVP", gfx::Type::MAT4, 1}}, 1},
    };
    gfx::AttributeList attributeList = {
        {"a_position", gfx::Format::RGBA32F, false, 0, false, 0},
        {"a_color", gfx::Format::RGBA32F, false, 0, false, 1},
    };

    gfx::ShaderInfo shaderInfo;
    shaderInfo.name = "Graphics Pass";
    shaderInfo.stages = std::move(shaderStageList);
    shaderInfo.attributes = std::move(attributeList);
    shaderInfo.blocks = std::move(uniformBlockList);
    _shader = _device->createShader(shaderInfo);
}

void ComputeTest::createUniformBuffer() {
    gfx::BufferInfo uniformBufferMVPInfo = {
        gfx::BufferUsage::UNIFORM,
        gfx::MemoryUsage::DEVICE | gfx::MemoryUsage::HOST,
        TestBaseI::getUBOSize(sizeof(Mat4)),
    };
    _uniformBufferMVP = _device->createBuffer(uniformBufferMVPInfo);

    Mat4 MVP;
    TestBaseI::createOrthographic(-1, 1, -1, 1, -1, 1, &MVP);
    _uniformBufferMVP->update(MVP.m, sizeof(Mat4));
}

void ComputeTest::createInputAssembler() {
    gfx::InputAssemblerInfo inputAssemblerInfo;
    inputAssemblerInfo.attributes.emplace_back(gfx::Attribute{"a_position", gfx::Format::RGBA32F, false, 0, false});
    inputAssemblerInfo.attributes.emplace_back(gfx::Attribute{"a_color", gfx::Format::RGBA32F, false, 0, false});
    inputAssemblerInfo.vertexBuffers.emplace_back(_compStorageBuffer);
    _inputAssembler = _device->createInputAssembler(inputAssemblerInfo);
}

void ComputeTest::createPipeline() {
    gfx::DescriptorSetLayoutInfo dslInfo;
    dslInfo.bindings.push_back({0, gfx::DescriptorType::UNIFORM_BUFFER, 1, gfx::ShaderStageFlagBit::VERTEX});
    _descriptorSetLayout = _device->createDescriptorSetLayout(dslInfo);

    _pipelineLayout = _device->createPipelineLayout({{_descriptorSetLayout}});

    _descriptorSet = _device->createDescriptorSet({_descriptorSetLayout});

    _descriptorSet->bindBuffer(0, _uniformBufferMVP);
    _descriptorSet->update();

    gfx::PipelineStateInfo pipelineInfo;
    pipelineInfo.primitive = gfx::PrimitiveMode::LINE_LIST;
    pipelineInfo.shader = _shader;
    pipelineInfo.inputState = {_inputAssembler->getAttributes()};
    pipelineInfo.renderPass = _fbo->getRenderPass();
    pipelineInfo.pipelineLayout = _pipelineLayout;

    _pipelineState = _device->createPipelineState(pipelineInfo);
}

void ComputeTest::tick() {
    lookupTime();

    _time += hostThread.dt;
    gfx::Color clearColor = {.2f, .2f, .2f, 1.0f};

    Mat4 MVP;
    TestBaseI::createOrthographic(-1, 1, -1, 1, -1, 1, &MVP);

    _device->acquire();

    _uniformBufferMVP->update(MVP.m, sizeof(Mat4));

    gfx::Rect renderArea = {0, 0, _device->getWidth(), _device->getHeight()};
    gfx::DispatchInfo dispatchInfo{(VERTEX_COUNT - 1) / GROUP_SIZE + 1, 1, 1};
    gfx::GlobalBarrier barrier{{gfx::AccessType::COMPUTE_SHADER_WRITE}, {gfx::AccessType::VERTEX_BUFFER}};

    auto commandBuffer = _commandBuffers[0];
    commandBuffer->begin();

    commandBuffer->bindPipelineState(_compPipelineState);
    commandBuffer->bindDescriptorSet(0, _compDescriptorSet);
    commandBuffer->dispatch(dispatchInfo);

    commandBuffer->pipelineBarrier(barrier);

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
