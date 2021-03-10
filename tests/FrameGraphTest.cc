#include "FrameGraphTest.h"

namespace cc {

void FrameGraphTest::onDestroy() {
    CC_SAFE_DESTROY(_vertexBuffer);
    CC_SAFE_DESTROY(_inputAssembler);
    CC_SAFE_DESTROY(_uniformBuffer);
    CC_SAFE_DESTROY(_uniformBufferMVP);
    CC_SAFE_DESTROY(_shader);
    CC_SAFE_DESTROY(_descriptorSet);
    CC_SAFE_DESTROY(_descriptorSetLayout);
    CC_SAFE_DESTROY(_pipelineLayout);
    CC_SAFE_DESTROY(_pipelineState);
    CC_SAFE_DESTROY(_indexBuffer);
}

bool FrameGraphTest::onInit() {
    createShader();
    createVertexBuffer();
    createInputAssembler();
    createPipeline();
    setupPasses();

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
    _shader               = _device->createShader(shaderInfo);
}

void FrameGraphTest::createVertexBuffer() {
    float vertexData[] = {-0.5f, 0.5f,
                          -0.5f, -0.5f,
                          0.5f, -0.5f,
                          0.0f, 0.5f,
                          0.5f, 0.5f};

    gfx::BufferInfo vertexBufferInfo = {
        gfx::BufferUsage::VERTEX,
        gfx::MemoryUsage::DEVICE,
        sizeof(vertexData),
        2 * sizeof(float),
        gfx::BufferFlagBit::NONE,
    };

    _vertexBuffer = _device->createBuffer(vertexBufferInfo);
    _vertexBuffer->update(vertexData, sizeof(vertexData));

    gfx::BufferInfo uniformBufferInfo = {
        gfx::BufferUsage::UNIFORM,
        gfx::MemoryUsage::DEVICE | gfx::MemoryUsage::HOST,
        TestBaseI::getUBOSize(sizeof(gfx::Color)),
    };
    _uniformBuffer = _device->createBuffer(uniformBufferInfo);

    gfx::BufferInfo uniformBufferMVPInfo = {
        gfx::BufferUsage::UNIFORM,
        gfx::MemoryUsage::DEVICE | gfx::MemoryUsage::HOST,
        TestBaseI::getUBOSize(sizeof(Mat4)),
    };
    _uniformBufferMVP = _device->createBuffer(uniformBufferMVPInfo);

    unsigned short  indices[]       = {1, 3, 0, 1, 2, 3, 2, 4, 3};
    gfx::BufferInfo indexBufferInfo = {
        gfx::BufferUsageBit::INDEX,
        gfx::MemoryUsage::DEVICE,
        sizeof(indices),
        sizeof(unsigned short),
    };
    _indexBuffer = _device->createBuffer(indexBufferInfo);
    _indexBuffer->update(indices, sizeof(indices));
}

void FrameGraphTest::createInputAssembler() {
    gfx::Attribute          position = {"a_position", gfx::Format::RG32F, false, 0, false};
    gfx::InputAssemblerInfo inputAssemblerInfo;
    inputAssemblerInfo.attributes.emplace_back(std::move(position));
    inputAssemblerInfo.vertexBuffers.emplace_back(_vertexBuffer);
    inputAssemblerInfo.indexBuffer = _indexBuffer;
    _inputAssembler                = _device->createInputAssembler(inputAssemblerInfo);
}

void FrameGraphTest::createPipeline() {
    gfx::DescriptorSetLayoutInfo dslInfo;
    dslInfo.bindings.push_back({0, gfx::DescriptorType::UNIFORM_BUFFER, 1, gfx::ShaderStageFlagBit::FRAGMENT});
    dslInfo.bindings.push_back({1, gfx::DescriptorType::UNIFORM_BUFFER, 1, gfx::ShaderStageFlagBit::VERTEX});
    _descriptorSetLayout = _device->createDescriptorSetLayout(dslInfo);

    _pipelineLayout = _device->createPipelineLayout({{_descriptorSetLayout}});

    _descriptorSet = _device->createDescriptorSet({_descriptorSetLayout});

    _descriptorSet->bindBuffer(0, _uniformBuffer);
    _descriptorSet->bindBuffer(1, _uniformBufferMVP);
    _descriptorSet->update();

    gfx::PipelineStateInfo pipelineInfo;
    pipelineInfo.primitive      = gfx::PrimitiveMode::TRIANGLE_LIST;
    pipelineInfo.shader         = _shader;
    pipelineInfo.inputState     = {_inputAssembler->getAttributes()};
    pipelineInfo.renderPass     = _fbo->getRenderPass();
    pipelineInfo.pipelineLayout = _pipelineLayout;

    _pipelineState = _device->createPipelineState(pipelineInfo);

    _globalBarriers.push_back(TestBaseI::getGlobalBarrier({
        {
            gfx::AccessType::TRANSFER_WRITE,
        },
        {
            gfx::AccessType::VERTEX_SHADER_READ_UNIFORM_BUFFER,
            gfx::AccessType::FRAGMENT_SHADER_READ_UNIFORM_BUFFER,
            gfx::AccessType::INDIRECT_BUFFER,
            gfx::AccessType::VERTEX_BUFFER,
            gfx::AccessType::INDEX_BUFFER,
        },
    }));

    _globalBarriers.push_back(TestBaseI::getGlobalBarrier({
        {
            gfx::AccessType::TRANSFER_WRITE,
        },
        {
            gfx::AccessType::VERTEX_SHADER_READ_UNIFORM_BUFFER,
            gfx::AccessType::FRAGMENT_SHADER_READ_UNIFORM_BUFFER,
        },
    }));

    _textureBarriers.push_back(TestBaseI::getTextureBarrier({
        {},
        {
            gfx::AccessType::TRANSFER_WRITE,
        },
    }));

    _textureBarriers.push_back(TestBaseI::getTextureBarrier({
        {
            gfx::AccessType::TRANSFER_WRITE,
        },
        {
            gfx::AccessType::PRESENT,
        },
    }));
}

void FrameGraphTest::setupPasses() {
    fg.enableMerge(true);
}

void FrameGraphTest::onTick() {
    uint globalBarrierIdx = _frameCount ? 1 : 0;

    gfx::Color clearColor = {1.0f, 0, 0, 1.0f};

    gfx::Color uniformColor;
    uniformColor.x = std::abs(std::sin(_time));
    uniformColor.y = 1.0f;
    uniformColor.z = 0.0f;
    uniformColor.w = 1.0f;

    Mat4 MVP;
    TestBaseI::createOrthographic(-1, 1, -1, 1, -1, 1, &MVP);

    _device->acquire();

    _uniformBuffer->update(&uniformColor, sizeof(uniformColor));
    _uniformBufferMVP->update(MVP.m, sizeof(Mat4));

    auto commandBuffer = _commandBuffers[0];
    commandBuffer->begin();

    if (TestBaseI::MANUAL_BARRIER)
        commandBuffer->pipelineBarrier(_globalBarriers[globalBarrierIdx]);

    ////////////////////////////////////////////////////////

    static framegraph::StringHandle leftName            = framegraph::FrameGraph::stringToHandle("Left");
    static framegraph::StringHandle rightName           = framegraph::FrameGraph::stringToHandle("Right");
    static framegraph::StringHandle colorTexName        = framegraph::FrameGraph::stringToHandle("ColorTex");
    static framegraph::StringHandle depthStencilTexName = framegraph::FrameGraph::stringToHandle("DepthStencilTex");

    struct DrawData {
        framegraph::TextureHandle colorTex;
        framegraph::TextureHandle depthStencilTex;
    };

    auto drawSetupFactory = [&](uint second) {
        return [&, second](framegraph::PassNodeBuilder &builder, DrawData &data) {
            framegraph::RenderTargetAttachment::Descriptor colorAttachmentInfo;
            colorAttachmentInfo.usage       = framegraph::RenderTargetAttachment::Usage::COLOR;
            colorAttachmentInfo.loadOp      = gfx::LoadOp::CLEAR;
            colorAttachmentInfo.clearColor  = clearColor;
            colorAttachmentInfo.endAccesses = {gfx::AccessType::TRANSFER_READ};

            data.colorTex = framegraph::TextureHandle(builder.readFromBlackboard(colorTexName));

            if (data.colorTex.isValid()) {
                colorAttachmentInfo.loadOp        = gfx::LoadOp::LOAD;
                colorAttachmentInfo.beginAccesses = {gfx::AccessType::TRANSFER_READ};
            } else {
                framegraph::Texture::Descriptor colorTexInfo;
                colorTexInfo.format = _device->getColorFormat();
                colorTexInfo.usage  = gfx::TextureUsageBit::COLOR_ATTACHMENT | gfx::TextureUsageBit::TRANSFER_SRC;
                colorTexInfo.width  = _device->getWidth();
                colorTexInfo.height = _device->getHeight();
                builder.create(data.colorTex, colorTexName, colorTexInfo);
            }

            data.colorTex = builder.write(data.colorTex, colorAttachmentInfo);
            // if (!second)
            builder.writeToBlackboard(colorTexName, data.colorTex);

            framegraph::RenderTargetAttachment::Descriptor depthStencilAttachmentInfo;
            depthStencilAttachmentInfo.clearDepth   = 1.0f;
            depthStencilAttachmentInfo.clearStencil = 0;
            depthStencilAttachmentInfo.usage        = framegraph::RenderTargetAttachment::Usage::DEPTH_STENCIL;
            depthStencilAttachmentInfo.loadOp       = gfx::LoadOp::CLEAR;
            depthStencilAttachmentInfo.endAccesses  = {gfx::AccessType::DEPTH_STENCIL_ATTACHMENT_WRITE};

            data.depthStencilTex = framegraph::TextureHandle(builder.readFromBlackboard(depthStencilTexName));

            if (data.depthStencilTex.isValid()) {
                depthStencilAttachmentInfo.loadOp        = gfx::LoadOp::LOAD;
                depthStencilAttachmentInfo.beginAccesses = {gfx::AccessType::DEPTH_STENCIL_ATTACHMENT_WRITE};
            } else {
                framegraph::Texture::Descriptor depthStencilTexInfo;
                depthStencilTexInfo.format = _device->getDepthStencilFormat();
                depthStencilTexInfo.usage  = gfx::TextureUsageBit::DEPTH_STENCIL_ATTACHMENT;
                depthStencilTexInfo.width  = _device->getWidth();
                depthStencilTexInfo.height = _device->getHeight();
                builder.create(data.depthStencilTex, depthStencilTexName, depthStencilTexInfo);
            }

            data.depthStencilTex = builder.write(data.depthStencilTex, depthStencilAttachmentInfo);
            builder.writeToBlackboard(depthStencilTexName, data.depthStencilTex);

            int           xOff = second * _device->getWidth() / 2;
            gfx::Viewport vp   = {xOff, 0, _device->getWidth() / 2, _device->getHeight(), 0, 1};
            gfx::Rect     rect = {xOff, 0, _device->getWidth() / 2, _device->getHeight()};
            builder.setViewport(vp, rect);
        };
    };

    auto drawExecFactory = [=](DrawData const &data, const framegraph::DevicePassResourceTable &table) {
        commandBuffer->bindInputAssembler(_inputAssembler);
        commandBuffer->bindPipelineState(_pipelineState);
        commandBuffer->bindDescriptorSet(0, _descriptorSet);
        commandBuffer->draw(_inputAssembler);
    };

    static framegraph::StringHandle blitName                   = framegraph::FrameGraph::stringToHandle("Blit");
    static framegraph::StringHandle backBufferName             = framegraph::FrameGraph::stringToHandle("BackBuffer");
    static framegraph::StringHandle depthStencilBackBufferName = framegraph::FrameGraph::stringToHandle("DepthStencilBackBuffer");

    struct BlitData {
        framegraph::TextureHandle colorInput;
        framegraph::TextureHandle colorOutput;
    };

    auto blitSetup = [&](framegraph::PassNodeBuilder &builder, BlitData &data) {
        data.colorInput = builder.read(framegraph::TextureHandle(builder.readFromBlackboard(colorTexName)));
        builder.writeToBlackboard(colorTexName, data.colorInput);

        data.colorOutput = builder.write(framegraph::TextureHandle(builder.readFromBlackboard(backBufferName)));
        builder.writeToBlackboard(backBufferName, data.colorOutput);
    };

    auto blitExec = [=](BlitData &data, const framegraph::DevicePassResourceTable &table) {
        auto input  = table.getRead(data.colorInput);
        auto output = table.getWrite(data.colorOutput);
        commandBuffer->pipelineBarrier(nullptr, &_textureBarriers[0], &output, 1);

        gfx::TextureBlit region;
        region.srcExtent.width  = _device->getWidth();
        region.srcExtent.height = _device->getHeight();
        region.dstExtent.width  = _device->getWidth();
        region.dstExtent.height = _device->getHeight();
        commandBuffer->blitTexture(input, nullptr, &region, 1, gfx::Filter::LINEAR);

        commandBuffer->pipelineBarrier(nullptr, &_textureBarriers[1], &output, 1);
    };

    fg.reset();

    fg.getBlackboard().put(backBufferName, fg.importExternal(backBufferName, _backBuffer));
    fg.getBlackboard().put(depthStencilBackBufferName, fg.importExternal(depthStencilBackBufferName, _depthStencilBackBuffer));

    fg.addPass<DrawData>(100, leftName, drawSetupFactory(0), drawExecFactory);
    fg.addPass<DrawData>(200, rightName, drawSetupFactory(1), drawExecFactory);
    fg.addPass<BlitData>(300, blitName, blitSetup, blitExec);

    fg.presentFromBlackboard(backBufferName);

    fg.compile();

    fg.exportGraphViz("fg_vis.dot");

    fg.execute();

    ////////////////////////////////////////////////////////

    commandBuffer->end();
    _device->flushCommands(_commandBuffers);
    _device->getQueue()->submit(_commandBuffers);
    _device->present();
}

} // namespace cc
