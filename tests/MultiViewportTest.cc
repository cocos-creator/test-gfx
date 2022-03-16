#include <vector>

#include "MultiViewportTest.h"
#include "gfx-base/GFXDef-common.h"
#include "tiny_obj_loader.h"

namespace cc {

void MultiViewportTest::onDestroy() {
    CC_SAFE_DESTROY(_vertexBuffer);
    CC_SAFE_DESTROY(_inputAssembler);
    CC_SAFE_DESTROY(_uniformBuffer);
    CC_SAFE_DESTROY(_uniformBuffer);
    CC_SAFE_DESTROY(_shader);
    CC_SAFE_DESTROY(_descriptorSet);
    CC_SAFE_DESTROY(_descriptorSetLayout);
    CC_SAFE_DESTROY(_pipelineLayout);
    CC_SAFE_DESTROY(_pipelineState);
    CC_SAFE_DESTROY(_invisiblePipelineState);
    CC_SAFE_DESTROY(_indexBuffer);
    CC_SAFE_DESTROY(_indirectBuffer);
}

bool MultiViewportTest::onInit() {
    createShader();
    createVertexBuffer();
    createInputAssembler();
    createPipeline();

    return true;
}

void MultiViewportTest::createShader() {
    ShaderSources<StandardShaderSource> sources;
    sources.glsl4 = {
        R"(
            precision highp float;
            layout(location = 0) in vec3 a_position;

            void main() {
                gl_Position = vec4(a_position, 1.0);
            }
        )",
        R"(
            precision highp float;

            layout(location = 0) in vec4 vColor;

            layout(location = 0) out vec4 o_color;

            void main() {
                o_color = vColor;
            }
        )",
    };

    sources.glsl3 = {
        R"(
            in vec3 a_position;
            layout(std140) uniform MVP { mat4 u_MVP; };

            void main() {
                gl_Position = u_MVP * vec4(a_position, 1.0);
            }
        )",
        R"(
            precision mediump float;

            out vec4 o_color;
            void main() {
                o_color = vec4(1., 1., 1., 1.);
            }
        )",
    };

    sources.glsl1 = {
        R"(
            attribute vec3 a_position;
            uniform mat4 u_MVP;

            void main() {
                gl_Position = u_MVP * vec4(a_position, 1.0);
            }
        )",
        R"(
            precision mediump float;
            void main() {
                gl_FragColor = vec4(1., 1., 1., 1.);
            }
        )",
    };

    ShaderSources<GeometryShaderSource> geomSources;
    geomSources.glsl4 = {
        R"(
            #extension GL_ARB_viewport_array : enable
            layout (triangles, invocations = 2) in;
            layout (triangle_strip, max_vertices = 3) out;

            layout (binding = 0) uniform UBO {
                mat4 modelview[2];
                mat4 projection[2];
            } ubo;

            layout(location = 0) out vec4 vColor;

            void main() {
                for (int i = 0; i < gl_in.length(); i++) {
                    vec4 pos = gl_in[i].gl_Position;
                    vec4 worldPos = ubo.modelview[gl_InvocationID] * pos;

                    gl_Position = ubo.projection[gl_InvocationID] * worldPos;

                    vColor = vec4(float(gl_InvocationID), 1.0 - float(gl_InvocationID), 0.0, 1.0);

                    gl_ViewportIndex = gl_InvocationID;

                gl_PrimitiveID = gl_PrimitiveIDIn;
                    EmitVertex();
                }
                EndPrimitive();
            }
        )"};

    StandardShaderSource &source     = TestBaseI::getAppropriateShaderSource(sources);
    GeometryShaderSource &geomSource = TestBaseI::getAppropriateShaderSource(geomSources);

    gfx::ShaderStageList shaderStageList;

    gfx::ShaderStage vertexShaderStage;
    vertexShaderStage.stage  = gfx::ShaderStageFlagBit::VERTEX;
    vertexShaderStage.source = source.vert;
    shaderStageList.emplace_back(std::move(vertexShaderStage));

    gfx::ShaderStage geometryShaderStage;
    geometryShaderStage.stage  = gfx::ShaderStageFlagBit::GEOMETRY;
    geometryShaderStage.source = geomSource;
    shaderStageList.emplace_back(std::move(geometryShaderStage));

    gfx::ShaderStage fragmentShaderStage;
    fragmentShaderStage.stage  = gfx::ShaderStageFlagBit::FRAGMENT;
    fragmentShaderStage.source = source.frag;
    shaderStageList.emplace_back(std::move(fragmentShaderStage));

    gfx::UniformBlockList uniformBlockList = {
        {0, 0, "ubo", {{"modelview", gfx::Type::MAT4, 2}, {"projection", gfx::Type::MAT4, 2}}, 1},
    };
    gfx::AttributeList attributeList = {{"a_position", gfx::Format::RGB32F, false, 0, false, 0}};

    gfx::ShaderInfo shaderInfo;
    shaderInfo.name       = "Basic Triangle";
    shaderInfo.stages     = std::move(shaderStageList);
    shaderInfo.attributes = std::move(attributeList);
    shaderInfo.blocks     = std::move(uniformBlockList);
    _shader               = device->createShader(shaderInfo);
}

void MultiViewportTest::createVertexBuffer() {
    auto        obj       = loadOBJ("bunny.obj");
    const auto &positions = obj.GetAttrib().vertices;

    _vertexBuffer = device->createBuffer({
        gfx::BufferUsage::VERTEX,
        gfx::MemoryUsage::DEVICE,
        static_cast<uint>(positions.size() * sizeof(float)),
        3 * sizeof(float),
    });
    _vertexBuffer->update(positions.data(), positions.size() * sizeof(float));

    const auto &          indicesInfo = obj.GetShapes()[0].mesh.indices;
    std::vector<uint16_t> indices;
    indices.reserve(indicesInfo.size());
    std::transform(indicesInfo.begin(), indicesInfo.end(), std::back_inserter(indices),
                   [](auto &&info) { return static_cast<uint16_t>(info.vertex_index); });

    _indexBuffer = device->createBuffer({
        gfx::BufferUsage::INDEX,
        gfx::MemoryUsage::DEVICE,
        static_cast<uint>(indices.size() * sizeof(uint16_t)),
        sizeof(uint16_t),
    });
    _indexBuffer->update(indices.data(), indices.size() * sizeof(uint16_t));

    gfx::BufferInfo uniformBufferInfo = {
        gfx::BufferUsage::UNIFORM,
        gfx::MemoryUsage::DEVICE | gfx::MemoryUsage::HOST,
        TestBaseI::getUBOSize(sizeof(UBOSetting)),
    };
    _uniformBuffer = device->createBuffer(uniformBufferInfo);
}

void MultiViewportTest::createInputAssembler() {
    gfx::Attribute          position = {"a_position", gfx::Format::RGB32F, false, 0, false};
    gfx::InputAssemblerInfo inputAssemblerInfo;
    inputAssemblerInfo.attributes.emplace_back(std::move(position));
    inputAssemblerInfo.vertexBuffers.emplace_back(_vertexBuffer);
    inputAssemblerInfo.indexBuffer = _indexBuffer;
    _inputAssembler                = device->createInputAssembler(inputAssemblerInfo);
}

void MultiViewportTest::createPipeline() {
    gfx::DescriptorSetLayoutInfo dslInfo;
    dslInfo.bindings.push_back({0, gfx::DescriptorType::UNIFORM_BUFFER, 1, gfx::ShaderStageFlagBit::GEOMETRY});
    _descriptorSetLayout = device->createDescriptorSetLayout(dslInfo);

    _pipelineLayout = device->createPipelineLayout({{_descriptorSetLayout}});

    _descriptorSet = device->createDescriptorSet({_descriptorSetLayout});

    _descriptorSet->bindBuffer(0, _uniformBuffer);
    _descriptorSet->update();

    gfx::PipelineStateInfo pipelineInfo;
    pipelineInfo.primitive                = gfx::PrimitiveMode::TRIANGLE_LIST;
    pipelineInfo.shader                   = _shader;
    pipelineInfo.inputState               = {_inputAssembler->getAttributes()};
    pipelineInfo.renderPass               = renderPass;
    pipelineInfo.rasterizerState.cullMode = gfx::CullMode::NONE;
    pipelineInfo.pipelineLayout           = _pipelineLayout;

    _pipelineState = device->createPipelineState(pipelineInfo);

    _generalBarriers.push_back(device->getGeneralBarrier({
        gfx::AccessFlagBit::TRANSFER_WRITE,
        gfx::AccessFlagBit::VERTEX_SHADER_READ_UNIFORM_BUFFER |
            gfx::AccessFlagBit::FRAGMENT_SHADER_READ_UNIFORM_BUFFER |
            gfx::AccessFlagBit::VERTEX_BUFFER |
            gfx::AccessFlagBit::INDEX_BUFFER,
    }));

    _generalBarriers.push_back(device->getGeneralBarrier({
        gfx::AccessFlagBit::TRANSFER_WRITE,
        gfx::AccessFlagBit::VERTEX_SHADER_READ_UNIFORM_BUFFER |
            gfx::AccessFlagBit::FRAGMENT_SHADER_READ_UNIFORM_BUFFER,
    }));
}

const float     _eyeSeperation  = 0.5F;
const float     _fov            = 90.0F;
const float     _zNear          = 1.0F;
const float     _zFar           = 256.0F;
constexpr float CAMERA_DISTANCE = 4.0F;
const Vec3      _foreHead       = Vec3(0, CAMERA_DISTANCE * 0.5, CAMERA_DISTANCE);

void MultiViewportTest::updateUniform() {
    auto *swapchain = swapchains[0];

    auto width  = swapchain->getWidth();
    auto height = swapchain->getHeight();

    float aspectRatio = static_cast<float>(width) / static_cast<float>(height);

    // left eye
    Mat4::createLookAt(_foreHead - Vec3(_eyeSeperation / 2.0, 0, 0), Vec3(0, 0, 0), Vec3(0, 1, 0), _setting.modelview);
    TestBaseI::createPerspective(_fov, aspectRatio, _zNear, _zFar, _setting.projection, swapchain);
    // right eye
    Mat4::createLookAt(_foreHead + Vec3(_eyeSeperation / 2.0, 0, 0), Vec3(0, 0, 0), Vec3(0, 1, 0), &(_setting.modelview[1]));
    TestBaseI::createPerspective(_fov, aspectRatio, _zNear, _zFar, &(_setting.projection[1]), swapchain);

    static Mat4 model;
    model.rotateY(0.01);

    _setting.modelview[0].multiply(model);
    _setting.modelview[1].multiply(model);

    _uniformBuffer->update(&_setting, sizeof(_setting));
}

#define TEST_OCCLUSION_QUERY 0

void MultiViewportTest::onTick() {
    auto *swapchain = swapchains[0];
    auto *fbo       = fbos[0];

    uint generalBarrierIdx = _frameCount ? 1 : 0;

    gfx::Color clearColor = {0, 0, 0, 1.0F};

    device->acquire(&swapchain, 1);

    updateUniform();

    gfx::Rect renderArea = {0, 0, swapchain->getWidth(), swapchain->getHeight()};

    auto *commandBuffer = commandBuffers[0];
    commandBuffer->begin();

    if (TestBaseI::MANUAL_BARRIER) {
        commandBuffer->pipelineBarrier(_generalBarriers[generalBarrierIdx]);
    }

    static gfx::Rect viewports[2] = {
        {0, 0, swapchain->getWidth() / 2.0, swapchain->getHeight()},
        {swapchain->getWidth() / 2.0, 0, swapchain->getWidth() / 2.0, swapchain->getHeight()},
    };

    commandBuffer->beginRenderPass(fbo->getRenderPass(), fbo, renderArea, &clearColor, 1.0F, 0);
    commandBuffer->bindPipelineState(_pipelineState);
    commandBuffer->setViewports(viewports, 2);
    commandBuffer->bindInputAssembler(_inputAssembler);
    commandBuffer->bindDescriptorSet(0, _descriptorSet);
    commandBuffer->draw(_inputAssembler);
    commandBuffer->endRenderPass();

    commandBuffer->end();

    device->flushCommands(commandBuffers);
    device->getQueue()->submit(commandBuffers);
    device->present();
}

} // namespace cc
