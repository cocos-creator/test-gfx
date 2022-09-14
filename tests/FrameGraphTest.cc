#include "FrameGraphTest.h"
#include "frame-graph/Resource.h"

namespace cc {

void FrameGraphTest::onDestroy() {
    CC_SAFE_DESTROY_AND_DELETE(_vertexBuffer);
    CC_SAFE_DESTROY_AND_DELETE(_inputAssembler);
    CC_SAFE_DESTROY_AND_DELETE(_uniformBuffer);
    CC_SAFE_DESTROY_AND_DELETE(_uniformBufferMVP);
    CC_SAFE_DESTROY_AND_DELETE(_shader);
    CC_SAFE_DESTROY_AND_DELETE(_descriptorSet);
    CC_SAFE_DESTROY_AND_DELETE(_descriptorSetLayout);
    CC_SAFE_DESTROY_AND_DELETE(_pipelineLayout);
    CC_SAFE_DESTROY_AND_DELETE(_pipelineState);
    CC_SAFE_DESTROY_AND_DELETE(_indexBuffer);
}

bool FrameGraphTest::onInit() {
    createShader();
    createVertexBuffer();
    createInputAssembler();
    createPipeline();

    return true;
}

void FrameGraphTest::createShader() {
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

void FrameGraphTest::createVertexBuffer() {
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
}

void FrameGraphTest::createInputAssembler() {
    gfx::Attribute          position = {"a_position", gfx::Format::RG32F, false, 0, false};
    gfx::InputAssemblerInfo inputAssemblerInfo;
    inputAssemblerInfo.attributes.emplace_back(std::move(position));
    inputAssemblerInfo.vertexBuffers.emplace_back(_vertexBuffer);
    inputAssemblerInfo.indexBuffer = _indexBuffer;
    _inputAssembler                = device->createInputAssembler(inputAssemblerInfo);
}

void FrameGraphTest::createPipeline() {
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

void FrameGraphTest::onTick() {
    auto *swapchain        = swapchains[0];
    uint  generalBarrierIdx = _frameCount ? 1 : 0;

    gfx::Color clearColor = {1.0F, 0, 0, 1.0F};

    gfx::Color uniformColor;
    uniformColor.x = std::abs(std::sin(_time));
    uniformColor.y = 1.0F;
    uniformColor.z = 0.0F;
    uniformColor.w = 1.0F;

    Mat4 mvp;
    TestBaseI::createOrthographic(-1, 1, -1, 1, -1, 1, &mvp, swapchain);

    device->acquire(&swapchain, 1);

    _uniformBuffer->update(&uniformColor, sizeof(uniformColor));
    _uniformBufferMVP->update(mvp.m, sizeof(Mat4));

    auto *commandBuffer = commandBuffers[0];
    commandBuffer->begin();

    if (TestBaseI::MANUAL_BARRIER) {
        commandBuffer->pipelineBarrier(_generalBarriers[generalBarrierIdx]);
    }

    ////////////////////////////////////////////////////////

    static framegraph::StringHandle leftName            = framegraph::FrameGraph::stringToHandle("Left");
    static framegraph::StringHandle rightName           = framegraph::FrameGraph::stringToHandle("Right");
    static framegraph::StringHandle colorTexName        = framegraph::FrameGraph::stringToHandle("ColorTex");
    static framegraph::StringHandle depthStencilTexName = framegraph::FrameGraph::stringToHandle("DepthStencilTex");

    struct DrawData {
        framegraph::TextureHandle colorTex;
        framegraph::TextureHandle depthStencilTex;
    };

    auto drawSetupFactory = [&](int second) {
        return [&, second](framegraph::PassNodeBuilder &builder, DrawData &data) {
            framegraph::RenderTargetAttachment::Descriptor colorAttachmentInfo;
            colorAttachmentInfo.usage       = framegraph::RenderTargetAttachment::Usage::COLOR;
            colorAttachmentInfo.loadOp      = gfx::LoadOp::CLEAR;
            colorAttachmentInfo.clearColor  = clearColor;
            colorAttachmentInfo.endAccesses = gfx::AccessFlagBit::COLOR_ATTACHMENT_WRITE;

            data.colorTex = framegraph::TextureHandle(builder.readFromBlackboard(colorTexName));

            if (data.colorTex.isValid()) {
                colorAttachmentInfo.loadOp        = gfx::LoadOp::LOAD;
                colorAttachmentInfo.beginAccesses = gfx::AccessFlagBit::COLOR_ATTACHMENT_WRITE;
            } else {
                framegraph::Texture::Descriptor colorTexInfo;
                colorTexInfo.format = swapchain->getColorTexture()->getFormat();
                colorTexInfo.usage  = gfx::TextureUsageBit::COLOR_ATTACHMENT;
                colorTexInfo.width  = swapchain->getWidth();
                colorTexInfo.height = swapchain->getHeight();
                data.colorTex       = builder.create(colorTexName, colorTexInfo);
            }

            data.colorTex = builder.write(data.colorTex, colorAttachmentInfo);
            // if (!second)
            builder.writeToBlackboard(colorTexName, data.colorTex);

            framegraph::RenderTargetAttachment::Descriptor depthStencilAttachmentInfo;
            depthStencilAttachmentInfo.clearDepth   = 1.0f;
            depthStencilAttachmentInfo.clearStencil = 0;
            depthStencilAttachmentInfo.usage        = framegraph::RenderTargetAttachment::Usage::DEPTH_STENCIL;
            depthStencilAttachmentInfo.loadOp       = gfx::LoadOp::CLEAR;
            depthStencilAttachmentInfo.endAccesses  = gfx::AccessFlagBit::DEPTH_STENCIL_ATTACHMENT_WRITE;

            data.depthStencilTex = framegraph::TextureHandle(builder.readFromBlackboard(depthStencilTexName));

            if (data.depthStencilTex.isValid()) {
                depthStencilAttachmentInfo.loadOp        = gfx::LoadOp::LOAD;
                depthStencilAttachmentInfo.beginAccesses = gfx::AccessFlagBit::DEPTH_STENCIL_ATTACHMENT_WRITE;
            } else {
                framegraph::Texture::Descriptor depthStencilTexInfo;
                depthStencilTexInfo.format = gfx::Format::DEPTH_STENCIL;
                depthStencilTexInfo.usage  = gfx::TextureUsageBit::DEPTH_STENCIL_ATTACHMENT;
                depthStencilTexInfo.width  = swapchain->getWidth();
                depthStencilTexInfo.height = swapchain->getHeight();
                data.depthStencilTex       = builder.create(depthStencilTexName, depthStencilTexInfo);
            }

            data.depthStencilTex = builder.write(data.depthStencilTex, depthStencilAttachmentInfo);
            builder.writeToBlackboard(depthStencilTexName, data.depthStencilTex);

            int           xOff = second * static_cast<int>(swapchain->getWidth()) / 2;
            gfx::Viewport vp   = {xOff, 0, swapchain->getWidth() / 2, swapchain->getHeight(), 0, 1};
            gfx::Rect     rect = {xOff, 0, swapchain->getWidth() / 2, swapchain->getHeight()};
            builder.setViewport(vp, rect);
        };
    };

    auto drawExecFactory = [=](DrawData const & /*data*/, const framegraph::DevicePassResourceTable & /*table*/) {
        commandBuffer->bindInputAssembler(_inputAssembler);
        commandBuffer->bindPipelineState(_pipelineState);
        commandBuffer->bindDescriptorSet(0, _descriptorSet);
        commandBuffer->draw(_inputAssembler);
    };

    fg.reset();

    fg.addPass<DrawData>(100, leftName, drawSetupFactory(0), drawExecFactory);
    fg.addPass<DrawData>(200, rightName, drawSetupFactory(1), drawExecFactory);

    fg.presentFromBlackboard(colorTexName, swapchain->getColorTexture());

    fg.compile();

    //fg.exportGraphViz("fg_vis.dot");

    fg.execute();

    ////////////////////////////////////////////////////////

    commandBuffer->end();
    device->flushCommands(commandBuffers);
    device->getQueue()->submit(commandBuffers);
    device->present();
}

} // namespace cc
