#include "StressTest.h"
#include "base/job-system/JobSystem.h"
#include "base/threading/MessageQueue.h"
#include "renderer/gfx-agent/DeviceAgent.h"

namespace cc {

#define USE_PARALLEL_RECORDING 1
#define TEST_MODE              4

#if TEST_MODE == 1
constexpr uint WORKLOAD                    = 10;
constexpr uint PASS_COUNT                  = 1;
constexpr uint MODELS_PER_LINE[PASS_COUNT] = {WORKLOAD};
#elif TEST_MODE == 2
constexpr uint WORKLOAD                    = 100;
constexpr uint PASS_COUNT                  = 4;
constexpr uint MODELS_PER_LINE[PASS_COUNT] = {WORKLOAD, WORKLOAD, WORKLOAD, WORKLOAD};
#elif TEST_MODE == 3
constexpr uint WORKLOAD                    = 150;
constexpr uint PASS_COUNT                  = 4;
constexpr uint MODELS_PER_LINE[PASS_COUNT] = {WORKLOAD, WORKLOAD, WORKLOAD, WORKLOAD};
#elif TEST_MODE == 4
constexpr uint PASS_COUNT                  = 4;
constexpr uint MODELS_PER_LINE[PASS_COUNT] = {150, 2, 3, 150};
#endif
/*
constexpr uint PASS_COUNT = 9;
constexpr uint MODELS_PER_LINE[PASS_COUNT] = {100, 2, 100, 100, 3, 4, 100, 5, 100};
//constexpr uint MODELS_PER_LINE[PASS_COUNT] = {WORKLOAD, WORKLOAD, WORKLOAD, WORKLOAD, WORKLOAD, WORKLOAD, WORKLOAD, WORKLOAD, WORKLOAD};
// */
constexpr uint  MAIN_THREAD_SLEEP = 10;
constexpr float QUAD_SIZE         = .01F;

const gfx::Color StressTest::CLEAR_COLORS[] = {
    {.2F, .2F, .2F, 1.F},
    {1.F, 0.F, 0.F, 1.F},
    {0.F, 1.F, 0.F, 1.F},
    {.4F, .4F, .4F, 1.F},
    {0.F, 0.F, 1.F, 1.F},
    {1.F, 1.F, 0.F, 1.F},
    {.8F, .8F, .8F, 1.F},
    {1.F, 0.F, 1.F, 1.F},
    {0.F, 1.F, 1.F, 1.F},
};

#define PARALLEL_STRATEGY_SEQUENTIAL                        0 // complete sequential recording & submission
#define PARALLEL_STRATEGY_RP_BASED_PRIMARY                  1 // render pass level concurrency
#define PARALLEL_STRATEGY_RP_BASED_SECONDARY                2 // render pass level concurrency with the use of secondary command buffers, which completely sequentializes the submission process
#define PARALLEL_STRATEGY_DC_BASED                          3 // draw call level concurrency: current endgame milestone
#define PARALLEL_STRATEGY_DC_BASED_FINER_JOBS               4 // draw call level concurrency with finer job generation (not works for single-threaded metal)
#define PARALLEL_STRATEGY_DC_BASED_FINER_JOBS_MULTI_PRIMARY 5 // draw call level concurrency with finer job generation

#define PARALLEL_STRATEGY 5

#define USE_DYNAMIC_UNIFORM_BUFFER 1

uint const TASK_COUNT = std::thread::hardware_concurrency() - 1U;

void hsV2Rgb(const float h, const float s, const float v, float &r, float &g, float &b) {
    int   hi = static_cast<int>(h / 60.0F) % 6;
    float f  = h / 60.0F - static_cast<float>(hi);
    float p  = v * (1.0F - s);
    float q  = v * (1.0F - s * f);
    float t  = v * (1.0F - s * (1.0F - f));

    switch (hi) {
        case 0: r = v, g = t, b = p; break;
        case 1: r = q, g = v, b = p; break;
        case 2: r = p, g = v, b = t; break;
        case 3: r = p, g = q, b = v; break;
        case 4: r = t, g = p, b = v; break;
        case 5: r = v, g = p, b = q; break;
    }
}

void StressTest::onDestroy() {
    CC_SAFE_DESTROY(_vertexBuffer);
    CC_SAFE_DESTROY(_inputAssembler);

#if USE_DYNAMIC_UNIFORM_BUFFER
    CC_SAFE_DESTROY(_uniDescriptorSet);
    CC_SAFE_DESTROY(_uniWorldBufferView);
    CC_SAFE_DESTROY(_uniWorldBuffer);
#else
    for (uint i = 0u; i < _descriptorSets.size(); i++) {
        CC_SAFE_DESTROY(_descriptorSets[i]);
    }
    _descriptorSets.clear();

    for (uint i = 0u; i < _worldBuffers.size(); i++) {
        CC_SAFE_DESTROY(_worldBuffers[i]);
    }
    _worldBuffers.clear();
#endif

    CC_SAFE_DESTROY(_uniformBufferVP);
    CC_SAFE_DESTROY(_shader);
    CC_SAFE_DESTROY(_descriptorSetLayout);
    CC_SAFE_DESTROY(_pipelineLayout);
    CC_SAFE_DESTROY(_pipelineState);

    for (auto &parallelCB : _parallelCBs) {
        CC_SAFE_DESTROY(parallelCB);
    }
    _parallelCBs.clear();

    for (size_t i = 1U; i < commandBuffers.size(); ++i) {
        CC_SAFE_DESTROY(commandBuffers[i]);
    }
    commandBuffers.resize(1);
}

bool StressTest::onInit() {
    createShader();
    createVertexBuffer();
    createInputAssembler();
    createPipeline();

    _threadCount = JobSystem::getInstance()->threadCount(); // main thread counts too
    gfx::CommandBufferInfo info;
    info.queue   = device->getQueue();
    uint cbCount = 0U;

#if PARALLEL_STRATEGY == PARALLEL_STRATEGY_DC_BASED_FINER_JOBS_MULTI_PRIMARY
    for (uint i = 1U; i < PASS_COUNT; ++i) {
        commandBuffers.push_back(device->createCommandBuffer(info));
    }
    cbCount   = _threadCount * PASS_COUNT;
    info.type = gfx::CommandBufferType::SECONDARY;
#elif PARALLEL_STRATEGY == PARALLEL_STRATEGY_DC_BASED_FINER_JOBS
    cbCount                            = _threadCount * PASS_COUNT;
    info.type                          = gfx::CommandBufferType::SECONDARY;
#elif PARALLEL_STRATEGY == PARALLEL_STRATEGY_DC_BASED
    cbCount   = _threadCount;
    info.type = gfx::CommandBufferType::SECONDARY;
#elif PARALLEL_STRATEGY == PARALLEL_STRATEGY_RP_BASED_SECONDARY
    cbCount   = PASS_COUNT;
    info.type = gfx::CommandBufferType::SECONDARY;
#elif PARALLEL_STRATEGY == PARALLEL_STRATEGY_RP_BASED_PRIMARY
    for (uint i = 1u; i < PASS_COUNT; ++i) {
        commandBuffers.push_back(device->createCommandBuffer(info));
    }
#endif

    for (uint i = _parallelCBs.size(); i < cbCount; ++i) {
        _parallelCBs.push_back(device->createCommandBuffer(info));
    }

    return true;
}

void StressTest::createShader() {
    ShaderSources<StandardShaderSource> sources;
    sources.glsl4 = {
        R"(
            precision mediump float;
            layout(location = 0) in vec2 a_position;
            layout(set = 0, binding = 0) uniform ViewProj { mat4 u_viewProj; vec4 u_color; };
            layout(set = 0, binding = 1) uniform World { vec4 u_world; };

            void main() {
                gl_Position = u_viewProj * vec4(a_position + u_world.xy, 0.0, 1.0);
            }
        )",
        R"(
            precision mediump float;
            layout(set = 0, binding = 0) uniform ViewProj { mat4 u_viewProj; vec4 u_color; };
            layout(location = 0) out vec4 o_color;

            void main() {
                o_color = u_color;
            }
        )",
    };

    sources.glsl3 = {
        R"(
            precision mediump float;
            in vec2 a_position;
            layout(std140) uniform ViewProj { mat4 u_viewProj; vec4 u_color; };
            layout(std140) uniform World { vec4 u_world; };

            void main() {
                gl_Position = u_viewProj * vec4(a_position + u_world.xy, 0.0, 1.0);
            }
        )",
        R"(
            precision mediump float;
            layout(std140) uniform ViewProj { mat4 u_viewProj; vec4 u_color; };

            out vec4 o_color;
            void main() {
                o_color = u_color;
            }
        )",
    };

    sources.glsl1 = {
        R"(
            precision mediump float;
            attribute vec2 a_position;
            uniform mat4 u_viewProj;
            uniform vec4 u_world;

            void main() {
                gl_Position = u_viewProj * vec4(a_position + u_world.xy, 0.0, 1.0);
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
        {0, 0, "ViewProj", {{"u_viewProj", gfx::Type::MAT4, 1}, {"u_color", gfx::Type::FLOAT4, 1}}, 1},
        {0, 1, "World", {{"u_world", gfx::Type::FLOAT4, 1}}, 1},
    };
    gfx::AttributeList attributeList = {{"a_position", gfx::Format::RG32F, false, 0, false, 0}};

    gfx::ShaderInfo shaderInfo;
    shaderInfo.name       = "StressTest";
    shaderInfo.stages     = std::move(shaderStageList);
    shaderInfo.attributes = std::move(attributeList);
    shaderInfo.blocks     = std::move(uniformBlockList);
    _shader               = device->createShader(shaderInfo);
}

void StressTest::createVertexBuffer() {
    auto *swapchain = swapchains[0];

    float y            = 1.F - QUAD_SIZE;
    float vertexData[] = {-1.F, -y,
                          -1.F, -1.F,
                          -y, -y,
                          -y, -1.F};

    gfx::BufferInfo vertexBufferInfo = {
        gfx::BufferUsage::VERTEX,
        gfx::MemoryUsage::DEVICE,
        sizeof(vertexData),
        2 * sizeof(float),
    };

    _vertexBuffer = device->createBuffer(vertexBufferInfo);
    _vertexBuffer->update(vertexData, sizeof(vertexData));

#if USE_DYNAMIC_UNIFORM_BUFFER
    _worldBufferStride                 = TestBaseI::getAlignedUBOStride(sizeof(Vec4));
    gfx::BufferInfo uniformBufferWInfo = {
        gfx::BufferUsage::UNIFORM,
        gfx::MemoryUsage::DEVICE,
        TestBaseI::getUBOSize(_worldBufferStride * MODELS_PER_LINE[0] * MODELS_PER_LINE[0]),
        _worldBufferStride,
    };
    _uniWorldBuffer = device->createBuffer(uniformBufferWInfo);

    uint          stride = _worldBufferStride / sizeof(float);
    vector<float> buffer(stride * MODELS_PER_LINE[0] * MODELS_PER_LINE[0]);
    uint          idx = 0U;
    for (float i = 0U; i < MODELS_PER_LINE[0]; i++) {
        for (float j = 0U; j < MODELS_PER_LINE[0]; j++, idx++) {
            buffer[idx * stride]     = 2.F * j / MODELS_PER_LINE[0];
            buffer[idx * stride + 1] = 2.F * i / MODELS_PER_LINE[0];
        }
    }
    _uniWorldBuffer->update(buffer.data(), buffer.size() * sizeof(float));

    gfx::BufferViewInfo worldBufferViewInfo = {
        _uniWorldBuffer,
        0,
        sizeof(Vec4),
    };
    _uniWorldBufferView = device->createBuffer(worldBufferViewInfo);
#else
    uint            size               = TestBaseI::getUBOSize(sizeof(Vec4));
    gfx::BufferInfo uniformBufferWInfo = {
        gfx::BufferUsage::UNIFORM,
        gfx::MemoryUsage::DEVICE,
        size, size};

    _worldBuffers.resize(MODELS_PER_LINE[0] * MODELS_PER_LINE[0]);
    vector<float> buffer(size / sizeof(float));
    for (uint i = 0u, idx = 0u; i < MODELS_PER_LINE[0]; i++) {
        for (uint j = 0u; j < MODELS_PER_LINE[0]; j++, idx++) {
            _worldBuffers[idx] = device->createBuffer(uniformBufferWInfo);

            buffer[0] = 2.f * j / MODELS_PER_LINE[0];
            buffer[1] = 2.f * i / MODELS_PER_LINE[0];

            _worldBuffers[idx]->update(buffer.data(), size);
        }
    }
#endif

    gfx::BufferInfo uniformBufferVPInfo = {
        gfx::BufferUsage::UNIFORM,
        gfx::MemoryUsage::DEVICE | gfx::MemoryUsage::HOST,
        TestBaseI::getUBOSize(sizeof(Mat4) + sizeof(Vec4)),
    };
    _uniformBufferVP = device->createBuffer(uniformBufferVPInfo);

    TestBaseI::createOrthographic(-1, 1, -1, 1, -1, 1, &_uboVP.matViewProj, swapchain);
}

void StressTest::createInputAssembler() {
    gfx::Attribute          position = {"a_position", gfx::Format::RG32F, false, 0, false};
    gfx::InputAssemblerInfo inputAssemblerInfo;
    inputAssemblerInfo.attributes.emplace_back(std::move(position));
    inputAssemblerInfo.vertexBuffers.emplace_back(_vertexBuffer);
    _inputAssembler = device->createInputAssembler(inputAssemblerInfo);
}

void StressTest::createPipeline() {
    gfx::DescriptorSetLayoutInfo dslInfo;
    dslInfo.bindings.push_back({0, gfx::DescriptorType::UNIFORM_BUFFER, 1,
                                gfx::ShaderStageFlagBit::VERTEX | gfx::ShaderStageFlagBit::FRAGMENT});
    dslInfo.bindings.push_back({1, USE_DYNAMIC_UNIFORM_BUFFER ? gfx::DescriptorType::DYNAMIC_UNIFORM_BUFFER : gfx::DescriptorType::UNIFORM_BUFFER,
                                1, gfx::ShaderStageFlagBit::VERTEX});
    _descriptorSetLayout = device->createDescriptorSetLayout(dslInfo);

    _pipelineLayout = device->createPipelineLayout({{_descriptorSetLayout}});

#if USE_DYNAMIC_UNIFORM_BUFFER
    _uniDescriptorSet = device->createDescriptorSet({_descriptorSetLayout});
    _uniDescriptorSet->bindBuffer(0, _uniformBufferVP);
    _uniDescriptorSet->bindBuffer(1, _uniWorldBufferView);
    _uniDescriptorSet->update();
#else
    _descriptorSets.resize(_worldBuffers.size());
    for (uint i = 0u; i < _worldBuffers.size(); ++i) {
        _descriptorSets[i] = device->createDescriptorSet({_descriptorSetLayout});
        _descriptorSets[i]->bindBuffer(0, _uniformBufferVP);
        _descriptorSets[i]->bindBuffer(1, _worldBuffers[i]);
        _descriptorSets[i]->update();
    }
#endif

    gfx::PipelineStateInfo pipelineInfo;
    pipelineInfo.primitive                = gfx::PrimitiveMode::TRIANGLE_STRIP;
    pipelineInfo.shader                   = _shader;
    pipelineInfo.rasterizerState.cullMode = gfx::CullMode::NONE;
    pipelineInfo.inputState               = {_inputAssembler->getAttributes()};
    pipelineInfo.renderPass               = renderPass;
    pipelineInfo.pipelineLayout           = _pipelineLayout;

    _pipelineState = device->createPipelineState(pipelineInfo);
}

#if PARALLEL_STRATEGY == PARALLEL_STRATEGY_SEQUENTIAL
void StressTest::recordRenderPass(uint passIndex) {
    auto *swapchain = swapchains[0];
    auto *fbo       = fbos[0];

    gfx::Rect           renderArea    = {0, 0, swapchain->getWidth(), swapchain->getHeight()};
    gfx::CommandBuffer *commandBuffer = commandBuffers[0];

    commandBuffer->begin();
    for (uint i = 0u; i < PASS_COUNT; ++i) {
        commandBuffer->beginRenderPass(renderPass, fbo, renderArea,
                                       &clearColors[i], 1.0f, 0);
        commandBuffer->bindInputAssembler(_inputAssembler);
        commandBuffer->bindPipelineState(_pipelineState);

        uint dynamicOffset = 0u;
        for (uint t = 0; t < MODELS_PER_LINE[i] * MODELS_PER_LINE[i]; ++t) {
    #if USE_DYNAMIC_UNIFORM_BUFFER
            dynamicOffset = _worldBufferStride * t;
            commandBuffer->bindDescriptorSet(0, _uniDescriptorSet, 1, &dynamicOffset);
    #else
            commandBuffer->bindDescriptorSet(0, _descriptorSets[t]);
    #endif
            commandBuffer->draw(_inputAssembler);
        }

        commandBuffer->endRenderPass();
    }
    commandBuffer->end();
}
#elif PARALLEL_STRATEGY == PARALLEL_STRATEGY_DC_BASED_FINER_JOBS || PARALLEL_STRATEGY == PARALLEL_STRATEGY_DC_BASED_FINER_JOBS_MULTI_PRIMARY
void StressTest::recordRenderPass(uint jobIdx) {
    auto *swapchain = swapchains[0];
    auto *fbo       = fbos[0];

    gfx::Rect     scissor = {0, 0, swapchain->getWidth(), swapchain->getHeight()};
    gfx::Rect vp      = {0, 0, swapchain->getWidth(), swapchain->getHeight()};

    uint passIdx        = jobIdx / _threadCount;
    uint threadIdx      = jobIdx % _threadCount;
    uint totalCount     = MODELS_PER_LINE[passIdx] * MODELS_PER_LINE[passIdx];
    uint perThreadCount = totalCount / _threadCount;
    uint dcCount        = perThreadCount;
    if (threadIdx == _threadCount - 1) dcCount += totalCount % _threadCount;

    //CC_LOG_INFO("======= %d %d %d", passIdx, threadIdx, jobIdx);
    //CC_LOG_INFO("---- idx %x cb %x id %x dc %d", threadIdx, commandBuffer,
    //            std::hash<std::thread::id>()(std::this_thread::get_id()), dcCount);

    uint                dynamicOffset = 0U;
    gfx::CommandBuffer *commandBuffer = _parallelCBs[jobIdx];
    commandBuffer->begin(renderPass, 0, fbo);
    if (dcCount) {
        commandBuffer->bindInputAssembler(_inputAssembler);
        commandBuffer->bindPipelineState(_pipelineState);
        commandBuffer->setViewport(vp);

        for (uint j = 0, t = perThreadCount * threadIdx; j < dcCount; ++j, ++t) {
    #if USE_DYNAMIC_UNIFORM_BUFFER
            dynamicOffset = _worldBufferStride * t;
            commandBuffer->bindDescriptorSet(0, _uniDescriptorSet, 1, &dynamicOffset);
    #else
            commandBuffer->bindDescriptorSet(0, _descriptorSets[t]);
    #endif
            commandBuffer->draw(_inputAssembler);
        }
    }
    commandBuffer->end();
}
#elif PARALLEL_STRATEGY == PARALLEL_STRATEGY_DC_BASED
void StressTest::recordRenderPass(uint threadIdx) {
    auto *swapchain = swapchains[0];
    auto *fbo       = fbos[0];

    gfx::Rect           scissor       = {0, 0, swapchain->getWidth(), swapchain->getHeight()};
    gfx::Rect       vp            = {0, 0, swapchain->getWidth(), swapchain->getHeight()};
    gfx::CommandBuffer *commandBuffer = _parallelCBs[threadIdx];

    for (uint i = 0u; i < PASS_COUNT; ++i) {
        uint dynamicOffset  = 0u;
        uint totalCount     = MODELS_PER_LINE[i] * MODELS_PER_LINE[i];
        uint perThreadCount = totalCount / _threadCount;
        uint dcCount        = perThreadCount;
        if (threadIdx == _threadCount - 1) dcCount += totalCount % _threadCount;

        //CC_LOG_INFO("---- idx %x cb %x id %x dc %d", threadIdx, commandBuffer,
        //            std::hash<std::thread::id>()(std::this_thread::get_id()), dcCount);

        commandBuffer->begin(renderPass, 0, fbo);
        if (dcCount) {
            commandBuffer->bindInputAssembler(_inputAssembler);
            commandBuffer->bindPipelineState(_pipelineState);
            commandBuffer->setScissor(scissor);
            commandBuffer->setViewport(vp);

            for (uint j = 0, t = perThreadCount * threadIdx; j < dcCount; ++j, ++t) {
    #if USE_DYNAMIC_UNIFORM_BUFFER
                dynamicOffset = _worldBufferStride * t;
                commandBuffer->bindDescriptorSet(0, _uniDescriptorSet, 1, &dynamicOffset);
    #else
                commandBuffer->bindDescriptorSet(0, _descriptorSets[t]);
    #endif
                commandBuffer->draw(_inputAssembler);
            }
        }
        commandBuffer->end();
    }
}
#elif PARALLEL_STRATEGY == PARALLEL_STRATEGY_RP_BASED_SECONDARY
void StressTest::recordRenderPass(uint passIndex) {
    auto *swapchain = swapchains[0];
    auto *fbo       = fbos[0];

    gfx::Rect     scissor   = {0, 0, swapchain->getWidth(), swapchain->getHeight()};
    gfx::Rect vp        = {0, 0, swapchain->getWidth(), swapchain->getHeight()};

    gfx::CommandBuffer *commandBuffer = _parallelCBs[passIndex];

    commandBuffer->begin(renderPass, 0, fbo);
    commandBuffer->bindInputAssembler(_inputAssembler);
    commandBuffer->bindPipelineState(_pipelineState);
    commandBuffer->setScissor(scissor);
    commandBuffer->setViewport(vp);

    uint dynamicOffset = 0u;
    for (uint t = 0; t < MODELS_PER_LINE[passIndex] * MODELS_PER_LINE[passIndex]; ++t) {
    #if USE_DYNAMIC_UNIFORM_BUFFER
        dynamicOffset = _worldBufferStride * t;
        commandBuffer->bindDescriptorSet(0, _uniDescriptorSet, 1, &dynamicOffset);
    #else
        commandBuffer->bindDescriptorSet(0, _descriptorSets[t]);
    #endif
        commandBuffer->draw(_inputAssembler);
    }

    commandBuffer->end();
}
#elif PARALLEL_STRATEGY == PARALLEL_STRATEGY_RP_BASED_PRIMARY
void StressTest::recordRenderPass(uint passIndex) {
    auto *swapchain = swapchains[0];
    auto *fbo       = fbos[0];

    gfx::Rect renderArea = {0, 0, swapchain->getWidth(), swapchain->getHeight()};
    gfx::CommandBuffer *commandBuffer = commandBuffers[passIndex];

    commandBuffer->begin();
    commandBuffer->beginRenderPass(renderPass, fbo, renderArea,
                                   &clearColors[passIndex], 1.0f, 0);
    commandBuffer->bindInputAssembler(_inputAssembler);
    commandBuffer->bindPipelineState(_pipelineState);

    uint dynamicOffset = 0u;
    for (uint t = 0; t < MODELS_PER_LINE[passIndex] * MODELS_PER_LINE[passIndex]; ++t) {
    #if USE_DYNAMIC_UNIFORM_BUFFER
        dynamicOffset = _worldBufferStride * t;
        commandBuffer->bindDescriptorSet(0, _uniDescriptorSet, 1, &dynamicOffset);
    #else
        commandBuffer->bindDescriptorSet(0, _descriptorSets[t]);
    #endif
        commandBuffer->draw(_inputAssembler);
    }

    commandBuffer->endRenderPass();
    commandBuffer->end();
}
#endif

void StressTest::onSpacePressed() {
    toggleMultithread();
}

void StressTest::onTick() {
    auto *swapchain = swapchains[0];
    auto *fbo       = fbos[0];

    // simulate heavy logic operation
    std::this_thread::sleep_for(std::chrono::milliseconds(MAIN_THREAD_SLEEP));

    printTime();

    device->acquire(&swapchain, 1);

    _uboVP.color.w = 1.F;
    hsV2Rgb(float((logicThread.frameAcc * 20U) % 360U), .5F, 1.F, _uboVP.color.x, _uboVP.color.y, _uboVP.color.z);
    _uniformBufferVP->update(&_uboVP, sizeof(_uboVP));

    /* un-toggle this to support dynamic screen rotation *
    Mat4 VP;
    TestBaseI::createOrthographic(-1, 1, -1, 1, -1, 1, &VP);
    _uniformBufferVP->update(VP.m, 0, sizeof(Mat4));
    // */

    gfx::Rect renderArea = {0, 0, swapchain->getWidth(), swapchain->getHeight()};
#if PARALLEL_STRATEGY == PARALLEL_STRATEGY_SEQUENTIAL
    recordRenderPass(0);
    device->flushCommands(commandBuffers);
    device->getQueue()->submit(commandBuffers);
#elif PARALLEL_STRATEGY == PARALLEL_STRATEGY_DC_BASED_FINER_JOBS
    gfx::CommandBuffer *commandBuffer = commandBuffers[0];
    commandBuffer->begin();

    #if USE_PARALLEL_RECORDING
    {
        JobGraph g(JobSystem::getInstance());
        uint     jobs[PASS_COUNT];
        for (uint t = 0u; t < PASS_COUNT; ++t) {
            jobs[t] = g.createForEachIndexJob(t * _threadCount, (t + 1) * _threadCount, 1u, [this](uint jobIdx) {
                recordRenderPass(jobIdx);
            });
            if (t) g.makeEdge(jobs[t - 1], jobs[t]);
        }
        g.run();
        g.waitForAll();
    }
    #else
    for (uint t = 0u; t < _threadCount * PASS_COUNT; ++t) {
        recordRenderPass(t);
    }
    #endif
    device->flushCommands(_parallelCBs);

    for (uint t = 0u; t < PASS_COUNT; ++t) {
        commandBuffer->beginRenderPass(renderPass, fbo, renderArea,
                                       &clearColors[t], 1.0f, 0,
                                       _threadCount, &_parallelCBs[t * _threadCount]);
        commandBuffer->execute(&_parallelCBs[t * _threadCount], _threadCount);
        commandBuffer->endRenderPass();
    }
    commandBuffer->end();

    device->flushCommands(commandBuffers);
    device->getQueue()->submit(commandBuffers);
#elif PARALLEL_STRATEGY == PARALLEL_STRATEGY_DC_BASED_FINER_JOBS_MULTI_PRIMARY
    for (uint t = 0U; t < PASS_COUNT; ++t) {
        gfx::CommandBuffer *commandBuffer = commandBuffers[t];
        commandBuffer->begin();
        commandBuffer->beginRenderPass(renderPass, fbo, renderArea,
                                       &CLEAR_COLORS[t], 1.0F, 0,
                                       &_parallelCBs[t * _threadCount], _threadCount);
        device->flushCommands(&commandBuffer, 1);
    }

    #if USE_PARALLEL_RECORDING
    {
        /* */
        JobGraph g(JobSystem::getInstance());
        g.createForEachIndexJob(0, PASS_COUNT * _threadCount, 1U, [this](uint jobIdx) {
            recordRenderPass(jobIdx);
        });
        g.run();
        g.waitForAll();
        /* *
        JobGraph g(JobSystem::getInstance());
        for (uint t = 0u; t < PASS_COUNT; ++t) {
            g.createForEachIndexJob(t * _threadCount, (t + 1) * _threadCount, 1u, [this](uint jobIdx) {
                recordRenderPass(jobIdx);
            });
        }
        g.run();
        g.waitForAll();
        // */
    }
    #else
    for (uint t = 0u; t < _threadCount * PASS_COUNT; ++t) {
        recordRenderPass(t);
    }
    #endif
    device->flushCommands(_parallelCBs);

    for (uint t = 0U; t < PASS_COUNT; ++t) {
        gfx::CommandBuffer *commandBuffer = commandBuffers[t];
        commandBuffer->execute(&_parallelCBs[t * _threadCount], _threadCount);
        commandBuffer->endRenderPass();
        commandBuffer->end();
        device->flushCommands(&commandBuffer, 1);
    }
    device->getQueue()->submit(commandBuffers);
#else
    gfx::CommandBuffer *commandBuffer = commandBuffers[0];
    commandBuffer->begin();

    uint jobCount = PARALLEL_STRATEGY == PARALLEL_STRATEGY_DC_BASED ? _threadCount : PASS_COUNT;
    #if USE_PARALLEL_RECORDING
    {
        JobGraph g(JobSystem::getInstance());
        g.createForEachIndexJob(0u, jobCount - 1, 1u, [this](uint index) {
            recordRenderPass(index);
        });
        g.run();
        recordRenderPass(jobCount - 1);
        g.waitForAll();
    }
    #else
    for (uint t = 0u; t < jobCount; ++t) {
        recordRenderPass(t);
    }
    #endif
    device->flushCommands(_parallelCBs);

    #if PARALLEL_STRATEGY == PARALLEL_STRATEGY_DC_BASED
    for (uint t = 0u; t < PASS_COUNT; ++t) {
        commandBuffer->beginRenderPass(renderPass, fbo, renderArea,
                                       &clearColors[t], 1.0f, 0,
                                       _parallelCBs.data(), _threadCount);
        commandBuffer->execute(_parallelCBs.data(), _threadCount);
        commandBuffer->endRenderPass();
    }
    commandBuffer->end();
    #elif PARALLEL_STRATEGY == PARALLEL_STRATEGY_RP_BASED_SECONDARY
    for (uint t = 0u; t < PASS_COUNT; ++t) {
        commandBuffer->beginRenderPass(renderPass, fbo, renderArea,
                                       &clearColors[t], 1.0f, 0,
                                       &_parallelCBs[t], 1);
        commandBuffer->execute(&_parallelCBs[t], 1);
        commandBuffer->endRenderPass();
    }
    commandBuffer->end();
    #endif

    device->flushCommands(commandBuffers);
    device->getQueue()->submit(commandBuffers);
#endif

    device->present();

    gfx::DeviceAgent *agent = gfx::DeviceAgent::getInstance();

    if (agent) {
        MessageQueue *queue = agent->getMessageQueue();
        ENQUEUE_MESSAGE_0(
            queue,
            DeviceStatistics,
            {
                lookupTime(&deviceThread);
                printTime(deviceThread, "Device thread");
            });
    }
}

} // namespace cc
