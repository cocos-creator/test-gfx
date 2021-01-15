#include "ComputeTest.h"

namespace cc {

#define GROUP_SIZE   64
#define VERTEX_COUNT 200
#define RADIUS       .7f

#define BG_GROUP_SIZE_X 32
#define BG_GROUP_SIZE_Y 32
#define BG_WIDTH        256
#define BG_HEIGHT       256

void ComputeTest::destroy() {
    CC_SAFE_DESTROY(_inputAssembler);
    CC_SAFE_DESTROY(_uniformBufferMVP);
    CC_SAFE_DESTROY(_shader);
    CC_SAFE_DESTROY(_descriptorSet);
    CC_SAFE_DESTROY(_descriptorSetLayout);
    CC_SAFE_DESTROY(_pipelineLayout);
    CC_SAFE_DESTROY(_pipelineState);

    CC_SAFE_DESTROY(_compShader);
    CC_SAFE_DESTROY(_compConstantsBuffer);
    CC_SAFE_DESTROY(_compStorageBuffer);
    CC_SAFE_DESTROY(_compDescriptorSet);
    CC_SAFE_DESTROY(_compDescriptorSetLayout);
    CC_SAFE_DESTROY(_compPipelineLayout);
    CC_SAFE_DESTROY(_compPipelineState);

    CC_SAFE_DESTROY(_compBGImage);
    CC_SAFE_DESTROY(_compBGShader);
    CC_SAFE_DESTROY(_compBGDescriptorSet);
    CC_SAFE_DESTROY(_compBGDescriptorSetLayout);
    CC_SAFE_DESTROY(_compBGPipelineLayout);
    CC_SAFE_DESTROY(_compBGPipelineState);
}

bool ComputeTest::initialize() {
    createComputeVBPipeline();
    createComputeBGPipeline();

    createShader();
    createUniformBuffer();
    createInputAssembler();
    createPipeline();

    return true;
}

void ComputeTest::createComputeVBPipeline() {
    _compStorageBuffer = _device->createBuffer({
        gfx::BufferUsage::STORAGE | gfx::BufferUsage::VERTEX,
        gfx::MemoryUsage::DEVICE,
        VERTEX_COUNT * 2 * sizeof(Vec4),
        2 * sizeof(Vec4),
    });

    // default value for storage buffer
    vector<Vec4> buffer{VERTEX_COUNT * 2};
    for (uint i = 0u; i < VERTEX_COUNT; ++i) {
        float alpha = 2.f * math::PI * i / VERTEX_COUNT;
        buffer[i * 2] = Vec4(std::sin(alpha) * RADIUS, std::cos(alpha) * RADIUS, 0.f, 1.f);
        buffer[i * 2 + 1] = Vec4(1.f, 1.f, 1.f, 1.f);
    }
    _compStorageBuffer->update(buffer.data(), buffer.size() * sizeof(Vec4));

    if (!_device->hasFeature(gfx::Feature::COMPUTE_SHADER)) return;

    ShaderSources<ComputeShaderSource> sources;
    sources.glsl4 = StringUtil::Format(
        R"(
        #define RADIUS %.1f

        layout(local_size_x = %d, local_size_y = 1, local_size_z = 1) in;

        layout(set = 0, binding = 0) readonly uniform Constants { float time; float vertexCount; };

        struct AttribData { vec4 p; vec4 c; };
        layout(set = 0, binding = 1) writeonly buffer DestBuffer { AttribData vertex[]; };

        void main() {
            if (gl_GlobalInvocationID.x >= uint(vertexCount)) return;

            int i = int(gl_GlobalInvocationID.x);
            float t = float(i) / vertexCount;

            float alpha = 2.0 * 3.14159265359 * t;
            vertex[i].p = vec4(sin(alpha) * RADIUS, cos(alpha) * RADIUS, 0.0, 1.0);
            vertex[i].c = vec4(fract(t - time), fract(2.0 * t - time), 1.0, 1.0);
        })",
        RADIUS, GROUP_SIZE);
    sources.glsl3 = StringUtil::Format(
        R"(
        #define RADIUS %.1f

        layout(local_size_x = %d, local_size_y = 1, local_size_z = 1) in;

        layout(binding = 0, std140) uniform Constants { float time; float vertexCount; };

        struct AttribData { vec4 p; vec4 c; };
        layout(binding = 1, std140) writeonly buffer DestBuffer { AttribData vertex[]; };

        void main() {
            if (gl_GlobalInvocationID.x >= uint(vertexCount)) return;

            int i = int(gl_GlobalInvocationID.x);
            float t = float(i) / vertexCount;

            float alpha = 2.0 * 3.14159265359 * t;
            vertex[i].p = vec4(sin(alpha) * RADIUS, cos(alpha) * RADIUS, 0.0, 1.0);
            vertex[i].c = vec4(fract(t - time), fract(2.0 * t - time), 1.0, 1.0);
        })",
        RADIUS, GROUP_SIZE);
    // no compute support in GLES2

    gfx::ShaderInfo shaderInfo;
    shaderInfo.name = "Compute VB";
    shaderInfo.stages = {{gfx::ShaderStageFlagBit::COMPUTE, TestBaseI::getAppropriateShaderSource(sources)}};
    shaderInfo.blocks = {{0, 0, "Constants", {{"time", gfx::Type::FLOAT, 1}, {"vertexCount", gfx::Type::FLOAT, 1}}, 1}};
    shaderInfo.buffers = {{0, 1, "DestBuffer", 1}};
    _compShader = _device->createShader(shaderInfo);

    _compConstantsBuffer = _device->createBuffer({
        gfx::BufferUsage::UNIFORM,
        gfx::MemoryUsage::DEVICE | gfx::MemoryUsage::HOST,
        TestBaseI::getUBOSize(sizeof(Vec4)),
    });

    gfx::DescriptorSetLayoutInfo dslInfo;
    dslInfo.bindings.push_back({0, gfx::DescriptorType::UNIFORM_BUFFER, 1, gfx::ShaderStageFlagBit::COMPUTE});
    dslInfo.bindings.push_back({1, gfx::DescriptorType::STORAGE_BUFFER, 1, gfx::ShaderStageFlagBit::COMPUTE});
    _compDescriptorSetLayout = _device->createDescriptorSetLayout(dslInfo);

    _compPipelineLayout = _device->createPipelineLayout({{_compDescriptorSetLayout}});

    _compDescriptorSet = _device->createDescriptorSet({_compDescriptorSetLayout});

    _compDescriptorSet->bindBuffer(0, _compConstantsBuffer);
    _compDescriptorSet->bindBuffer(1, _compStorageBuffer);
    _compDescriptorSet->update();

    gfx::PipelineStateInfo pipelineInfo;
    pipelineInfo.shader = _compShader;
    pipelineInfo.pipelineLayout = _compPipelineLayout;
    pipelineInfo.bindPoint = gfx::PipelineBindPoint::COMPUTE;

    _compPipelineState = _device->createPipelineState(pipelineInfo);
}

void ComputeTest::createComputeBGPipeline() {
    _compBGImage = _device->createTexture({
        gfx::TextureType::TEX2D,
        gfx::TextureUsage::SAMPLED | gfx::TextureUsage::STORAGE | gfx::TextureUsage::TRANSFER_DST,
        gfx::Format::RGBA8,
        BG_WIDTH,
        BG_HEIGHT,
        gfx::TextureFlagBit::IMMUTABLE,
    });

    if (!_device->hasFeature(gfx::Feature::COMPUTE_SHADER)) return;

    ShaderSources<ComputeShaderSource> sources;
    sources.glsl4 = StringUtil::Format(
        R"(
        layout(local_size_x = %d, local_size_y = %d, local_size_z = 1) in;

        layout(set = 0, binding = 0) uniform Constants { float time; float vertexCount; vec2 texSize; };

        layout(set = 0, binding = 1, rgba8) writeonly uniform image2D background;

        void main() {
            if (any(greaterThanEqual(gl_GlobalInvocationID.xy, texSize))) return;

            vec2 uv = vec2(gl_GlobalInvocationID.xy) / texSize;
            vec3 col = 0.5 + 0.5 * cos(time + uv.xyx + vec3(0, 2, 4));
            imageStore(background, ivec2(gl_GlobalInvocationID.xy), vec4(col, 1.0));
        })",
        BG_GROUP_SIZE_X, BG_GROUP_SIZE_Y);
    sources.glsl3 = StringUtil::Format(
        R"(
        layout(local_size_x = %d, local_size_y = %d, local_size_z = 1) in;

        layout(binding = 0, std140) uniform Constants { float time; float vertexCount; vec2 texSize; };

        layout(binding = 1, rgba8) writeonly uniform lowp image2D background;

        void main() {
            if (any(greaterThanEqual(vec2(gl_GlobalInvocationID.xy), texSize))) return;

            vec2 uv = vec2(gl_GlobalInvocationID.xy) / texSize;
            vec3 col = 0.5 + 0.5 * cos(time + uv.xyx + vec3(0, 2, 4));
            imageStore(background, ivec2(gl_GlobalInvocationID.xy), vec4(col, 1.0));
        })",
        BG_GROUP_SIZE_X, BG_GROUP_SIZE_Y);
    // no compute support in GLES2

    gfx::ShaderInfo shaderInfo;
    shaderInfo.name = "Compute BG";
    shaderInfo.stages = {{gfx::ShaderStageFlagBit::COMPUTE, TestBaseI::getAppropriateShaderSource(sources)}};
    shaderInfo.blocks = {{0, 0, "Constants", {{"time", gfx::Type::FLOAT, 1}, {"vertexCount", gfx::Type::FLOAT, 1}, {"texSize", gfx::Type::FLOAT2, 1}}, 1}};
    shaderInfo.samplers = {{0, 1, "background", gfx::Type::SAMPLER2D, 1, gfx::UniformSamplerType::STORAGE_IMAGE}};
    _compBGShader = _device->createShader(shaderInfo);

    gfx::DescriptorSetLayoutInfo dslInfo;
    dslInfo.bindings.push_back({0, gfx::DescriptorType::UNIFORM_BUFFER, 1, gfx::ShaderStageFlagBit::COMPUTE});
    dslInfo.bindings.push_back({1, gfx::DescriptorType::STORAGE_IMAGE, 1, gfx::ShaderStageFlagBit::COMPUTE});
    _compBGDescriptorSetLayout = _device->createDescriptorSetLayout(dslInfo);

    _compBGPipelineLayout = _device->createPipelineLayout({{_compBGDescriptorSetLayout}});

    _compBGDescriptorSet = _device->createDescriptorSet({_compBGDescriptorSetLayout});

    _compBGDescriptorSet->bindBuffer(0, _compConstantsBuffer);
    _compBGDescriptorSet->bindTexture(1, _compBGImage);
    _compBGDescriptorSet->update();

    gfx::PipelineStateInfo pipelineInfo;
    pipelineInfo.shader = _compBGShader;
    pipelineInfo.pipelineLayout = _compBGPipelineLayout;
    pipelineInfo.bindPoint = gfx::PipelineBindPoint::COMPUTE;

    _compBGPipelineState = _device->createPipelineState(pipelineInfo);
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
    Vec4 constants{_time, VERTEX_COUNT, BG_WIDTH, BG_HEIGHT};
    gfx::DispatchInfo dispatchInfo{(VERTEX_COUNT - 1) / GROUP_SIZE + 1, 1, 1};
    gfx::DispatchInfo bgDispatchInfo{(BG_WIDTH - 1) / BG_GROUP_SIZE_X + 1, (BG_HEIGHT - 1) / BG_GROUP_SIZE_Y + 1, 1};
    gfx::GlobalBarrier barrier{{gfx::AccessType::COMPUTE_SHADER_WRITE}, {gfx::AccessType::VERTEX_BUFFER, gfx::AccessType::FRAGMENT_SHADER_READ_TEXTURE}};

    _device->acquire();

    if (_compConstantsBuffer) _compConstantsBuffer->update(&constants, sizeof(constants));
    _uniformBufferMVP->update(MVP.m, sizeof(MVP.m));

    gfx::Rect renderArea = {0, 0, _device->getWidth(), _device->getHeight()};

    auto commandBuffer = _commandBuffers[0];
    commandBuffer->begin();

    if (_device->hasFeature(gfx::Feature::COMPUTE_SHADER)) {
        commandBuffer->bindPipelineState(_compPipelineState);
        commandBuffer->bindDescriptorSet(0, _compDescriptorSet);
        commandBuffer->dispatch(dispatchInfo);

        commandBuffer->bindPipelineState(_compBGPipelineState);
        commandBuffer->bindDescriptorSet(0, _compBGDescriptorSet);
        commandBuffer->dispatch(bgDispatchInfo);

        commandBuffer->pipelineBarrier(barrier);
    }

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
