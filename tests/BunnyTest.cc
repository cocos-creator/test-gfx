#include "BunnyTest.h"
#include "BunnyData.h"

namespace cc {

    namespace {
        enum class Binding : uint8_t { MVP, COLOR };
    }

    void BunnyTest::destroy() {
        CC_SAFE_DESTROY(_shader);
        CC_SAFE_DESTROY(_vertexBuffer);
        CC_SAFE_DESTROY(_indexBuffer);
        CC_SAFE_DESTROY(_mvpMatrix);
        CC_SAFE_DESTROY(_color);
        CC_SAFE_DESTROY(_inputAssembler);
        CC_SAFE_DESTROY(_bindingLayout);
        CC_SAFE_DESTROY(_pipelineState);
    }

    bool BunnyTest::initialize() {
        createShader();
        createBuffers();
        createInputAssembler();
        createPipelineState();
        return true;
    }

    void BunnyTest::createShader() {
        gfx::ShaderStageList shaderStageList;
        gfx::ShaderStage vertexShaderStage;
        vertexShaderStage.type = gfx::ShaderType::VERTEX;

#if defined(USE_VULKAN) || defined(USE_METAL)
        vertexShaderStage.source = R"(
    layout(location = 0) in vec3 a_position;
    
    layout(binding = 0) uniform MVP_Matrix
    {
        mat4 u_model, u_view, u_projection;
    };
    
    layout(location = 0) out vec3 v_position;

    void main () {
        vec4 pos = u_projection * u_view * u_model * vec4(a_position, 1);
        v_position = a_position.xyz;
        gl_Position = pos;
    }
    )";
#elif defined(USE_GLES2)
        vertexShaderStage.source = R"(
    attribute vec3 a_position;
    uniform mat4 u_model, u_view, u_projection;
    varying vec3 v_position;

    void main () {
        vec4 pos = u_projection * u_view * u_model * vec4(a_position, 1);
        v_position = a_position.xyz;
        gl_Position = pos;
    }
    )";
#else
        vertexShaderStage.source = R"(
    in vec3 a_position;
    
    layout(std140) uniform MVP_Matrix
    {
        mat4 u_model, u_view, u_projection;
    };
    
    out vec3 v_position;

    void main () {
        vec4 pos = u_projection * u_view * u_model * vec4(a_position, 1);
        v_position = a_position.xyz;
        gl_Position = pos;
    }
    )";
#endif // USE_GLES2

        shaderStageList.emplace_back(std::move(vertexShaderStage));

        // fragment shader
        gfx::ShaderStage fragmentShaderStage;
        fragmentShaderStage.type = gfx::ShaderType::FRAGMENT;

#if defined(USE_VULKAN) || defined(USE_METAL)
        fragmentShaderStage.source = R"(
    #ifdef GL_ES
                precision highp float;
    #endif
    layout(binding = 1) uniform Color
    {
        vec4 u_color;
    };
    layout(location = 0) in vec3 v_position;
    layout(location = 0) out vec4 o_color;
    void main () {
        o_color = u_color * vec4(v_position, 1);
    }
    )";
#elif defined(USE_GLES2)
        fragmentShaderStage.source = R"(
    #ifdef GL_ES
    precision highp float;
    #endif
    uniform vec4 u_color;
    varying vec3 v_position;

    void main () {
        gl_FragColor = u_color * vec4(v_position, 1);
    }
    )";
#else
        fragmentShaderStage.source = R"(
#ifdef GL_ES
    precision highp float;
#endif
    layout(std140) uniform Color
    {
        vec4 u_color;
    };
    in vec3 v_position;
    out vec4 o_color;
    void main () {
        o_color = u_color * vec4(v_position, 1);
    }
    )";
#endif // USE_GLES2

        shaderStageList.emplace_back(std::move(fragmentShaderStage));

        gfx::AttributeList attributeList = { { "a_position", gfx::Format::RGB32F, false, 0, false, 0 } };
        gfx::UniformList mvpMatrix = { {"u_model", gfx::Type::MAT4, 1},
                                    {"u_view", gfx::Type::MAT4, 1},
                                    {"u_projection", gfx::Type::MAT4, 1} };
        gfx::UniformList color = { {"u_color", gfx::Type::FLOAT4, 1} };
        gfx::UniformBlockList uniformBlockList = {
            {gfx::ShaderType::VERTEX, static_cast<uint>(Binding::MVP), "MVP_Matrix",
             mvpMatrix},
            {gfx::ShaderType::FRAGMENT, static_cast<uint>(Binding::COLOR), "Color",
             color} };

        gfx::ShaderInfo shaderInfo;
        shaderInfo.name = "Bunny Test";
        shaderInfo.stages = std::move(shaderStageList);
        shaderInfo.attributes = std::move(attributeList);
        shaderInfo.blocks = std::move(uniformBlockList);
        _shader = _device->createShader(shaderInfo);
    }

    void BunnyTest::createBuffers() {
        // vertex buffer
        gfx::BufferInfo vertexBufferInfo = {
            gfx::BufferUsage::VERTEX, gfx::MemoryUsage::DEVICE, 3 * sizeof(float),
            sizeof(bunny_positions), gfx::BufferFlagBit::NONE };

        _vertexBuffer = _device->createBuffer(vertexBufferInfo);
        _vertexBuffer->update((void *)&bunny_positions[0][0], 0, sizeof(bunny_positions));

        // index buffer
        gfx::BufferInfo indexBufferInfo = { gfx::BufferUsage::INDEX,
                                         gfx::MemoryUsage::DEVICE, sizeof(uint16_t),
                                         sizeof(bunny_cells), gfx::BufferFlagBit::NONE };
        _indexBuffer = _device->createBuffer(indexBufferInfo);
        _indexBuffer->update((void *)&bunny_cells[0], 0, sizeof(bunny_cells));

        // mvp matrix uniform
        gfx::BufferInfo mvpMatrixInfo = { gfx::BufferUsage::UNIFORM,
                                       gfx::MemoryUsage::DEVICE, sizeof(Mat4),
                                       3 * sizeof(Mat4), gfx::BufferFlagBit::NONE };

        Mat4 model, projection;
        Mat4::createPerspective(60.0f,
            1.0f * _device->getWidth() / _device->getHeight(),
            0.01f, 1000.0f, &projection);
        TestBaseI::modifyProjectionBasedOnDevice(projection);
        _mvpMatrix = _device->createBuffer(mvpMatrixInfo);
        _mvpMatrix->update(model.m, 0, sizeof(model));
        _mvpMatrix->update(projection.m, 2 * sizeof(Mat4), sizeof(projection));

        // color uniform
        gfx::BufferInfo colorInfo = { gfx::BufferUsage::UNIFORM, gfx::MemoryUsage::DEVICE,
                                   sizeof(float), 4 * sizeof(float),
                                   gfx::BufferFlagBit::NONE };
        float color[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
        _color = _device->createBuffer(colorInfo);
        _color->update(color, 0, sizeof(color));
    }

    void BunnyTest::createInputAssembler() {
        gfx::Attribute position = { "a_position", gfx::Format::RGB32F, false, 0, false };
        gfx::InputAssemblerInfo inputAssemblerInfo;
        inputAssemblerInfo.attributes.emplace_back(std::move(position));
        inputAssemblerInfo.vertexBuffers.emplace_back(_vertexBuffer);
        inputAssemblerInfo.indexBuffer = _indexBuffer;
        _inputAssembler = _device->createInputAssembler(inputAssemblerInfo);
    }

    void BunnyTest::createPipelineState() {
        gfx::BindingLayoutInfo bindingLayoutInfo = { _shader };
        _bindingLayout = _device->createBindingLayout(bindingLayoutInfo);
        _bindingLayout->bindBuffer(static_cast<uint>(Binding::MVP), _mvpMatrix);
        _bindingLayout->bindBuffer(static_cast<uint>(Binding::COLOR), _color);
        _bindingLayout->update();

        gfx::PipelineStateInfo pipelineStateInfo;
        pipelineStateInfo.primitive = gfx::PrimitiveMode::TRIANGLE_LIST;
        pipelineStateInfo.shader = _shader;
        pipelineStateInfo.inputState = { _inputAssembler->getAttributes() };
        pipelineStateInfo.renderPass = _fbo->getRenderPass();
        pipelineStateInfo.depthStencilState.depthTest = true;
        pipelineStateInfo.depthStencilState.depthWrite = true;
        pipelineStateInfo.depthStencilState.depthFunc = gfx::ComparisonFunc::LESS;
        _pipelineState = _device->createPipelineState(pipelineStateInfo);
    }

    void BunnyTest::tick(float dt) {
        _dt += dt;
        Mat4::createLookAt(Vec3(30.0f * std::cos(_dt), 20.0f, 30.0f * std::sin(_dt)),
            Vec3(0.0f, 2.5f, 0.0f), Vec3(0.0f, 1.0f, 0.f), &_view);
        _mvpMatrix->update(_view.m, sizeof(Mat4), sizeof(_view));

        gfx::Rect render_area = { 0, 0, _device->getWidth(), _device->getHeight() };
        gfx::Color clear_color = { 0.0f, 0, 0, 1.0f };

        _device->acquire();

        auto commandBuffer = _commandBuffers[0];
        commandBuffer->begin();
        commandBuffer->beginRenderPass(_fbo, render_area, gfx::ClearFlagBit::ALL,
            std::move(std::vector<gfx::Color>({ clear_color })), 1.0f, 0);

        commandBuffer->bindInputAssembler(_inputAssembler);
        commandBuffer->bindPipelineState(_pipelineState);
        commandBuffer->bindBindingLayout(_bindingLayout);
        commandBuffer->draw(_inputAssembler);

        commandBuffer->endRenderPass();
        commandBuffer->end();

        _device->getQueue()->submit(_commandBuffers);
        _device->present();
    }

} // namespace cc
