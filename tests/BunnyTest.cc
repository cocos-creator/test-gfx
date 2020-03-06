#include "BunnyTest.h"

#if (CC_PLATFORM == CC_PLATFORM_MAC_OSX)
#include "gfx-metal/GFXMTL.h"
#else
#include "gfx-gles2/GFXGLES2.h"
#include "gfx-gles3/GFXGLES3.h"
#endif

#include "cocos2d.h"
#include "BunnyData.h"

NS_CC_BEGIN

namespace {
    enum class Binding : uint8_t
    {
        MVP,
        COLOR
    };
}

void BunnyTest::destroy()
{
    CC_SAFE_DESTROY(_shader);
    CC_SAFE_DESTROY(_vertexBuffer);
    CC_SAFE_DESTROY(_indexBuffer);
    CC_SAFE_DESTROY(_mvpMatrix);
    CC_SAFE_DESTROY(_color);
    CC_SAFE_DESTROY(_inputAssembler);
    CC_SAFE_DESTROY(_bindingLayout);
}

bool BunnyTest::initialize()
{
    createShader();
    createBuffers();
    createInputAssembler();
    createPipelineState();
    return true;
}

void BunnyTest::createShader()
{
    GFXShaderStageList shaderStageList;
    GFXShaderStage vertexShaderStage;
    vertexShaderStage.type = GFXShaderType::VERTEX;
    
#if (CC_PLATFORM == CC_PLATFORM_MAC_OSX)
    vertexShaderStage.source = R"(
        #include <metal_stdlib>
        #include <simd/simd.h>

        using namespace metal;

        struct MVP_Matrix
        {
            float4x4 u_model;
            float4x4 u_view;
            float4x4 u_projection;
        };

        struct main0_out
        {
            float3 v_position [[user(locn0)]];
            float4 gl_Position [[position]];
        };

        struct main0_in
        {
            float3 a_position [[attribute(0)]];
        };

        vertex main0_out main0(main0_in in [[stage_in]], constant MVP_Matrix& _13 [[buffer(0)]])
        {
            main0_out out = {};
            float4 pos = ((_13.u_projection * _13.u_view) * _13.u_model) * float4(in.a_position, 1.0);
            out.v_position = in.a_position;
            out.gl_Position = pos;
            return out;
        }
    )";
#else
    
#ifdef USE_GLES2
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
    vertexShaderStage.source = R"(#version 300 es
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
#endif // (CC_PLATFORM == CC_PLATFORM_MAC_OSX)
    
    shaderStageList.emplace_back(std::move(vertexShaderStage));
    
    //fragment shader
    GFXShaderStage fragmentShaderStage;
    fragmentShaderStage.type = GFXShaderType::FRAGMENT;
    
#if (CC_PLATFORM == CC_PLATFORM_MAC_OSX)
    fragmentShaderStage.source = R"(
        #include <metal_stdlib>
        #include <simd/simd.h>

        using namespace metal;

        struct Color
        {
            float4 u_color;
        };

        struct main0_out
        {
            float4 o_color [[color(0)]];
        };

        struct main0_in
        {
            float3 v_position [[user(locn0)]];
        };

        fragment main0_out main0(main0_in in [[stage_in]], constant Color& _12 [[buffer(0)]])
        {
            main0_out out = {};
            out.o_color = _12.u_color * float4(in.v_position, 1.0);
            return out;
        }
    )";
#else
    
#ifdef USE_GLES2
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
    fragmentShaderStage.source = R"(#version 300 es
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
    
#endif // (CC_PLATFORM == CC_PLATFORM_MAC_OSX)
    shaderStageList.emplace_back(std::move(fragmentShaderStage));
    
    GFXUniformList mvpMatrix = { 
        { "u_model", GFXType::MAT4, 1},
        { "u_view", GFXType::MAT4, 1},
        { "u_projection", GFXType::MAT4, 1}
    };
    GFXUniformList color = { { "u_color", GFXType::FLOAT4, 1} };
    GFXUniformBlockList uniformBlockList = {
        {static_cast<uint>(Binding::MVP), "MVP_Matrix", mvpMatrix},
        {static_cast<uint>(Binding::COLOR), "Color", color}
    };
    
    GFXShaderInfo shaderInfo;
    shaderInfo.name = "Bunny Test";
    shaderInfo.stages = std::move(shaderStageList);
    shaderInfo.blocks = std::move(uniformBlockList);
    _shader = _device->createShader(shaderInfo);
}

void BunnyTest::createBuffers()
{   
    //vertex buffer
    GFXBufferInfo vertexBufferInfo = {
        GFXBufferUsage::VERTEX,
        GFXMemoryUsage::HOST,
        3 * sizeof(float),
        sizeof(bunny_positions),
        GFXBufferFlagBit::NONE };
    
    _vertexBuffer = _device->createBuffer(vertexBufferInfo);
    _vertexBuffer->update((void*)&bunny_positions[0][0], 0, sizeof(bunny_positions));
    
    //index buffer
    GFXBufferInfo indexBufferInfo = {
        GFXBufferUsage::INDEX,
        GFXMemoryUsage::HOST,
        sizeof(uint16_t),
        sizeof(bunny_cells),
        GFXBufferFlagBit::NONE };
    _indexBuffer = _device->createBuffer(indexBufferInfo);
    _indexBuffer->update((void*)&bunny_cells[0], 0, sizeof(bunny_cells));

    //mvp matrix uniform
    GFXBufferInfo mvpMatrixInfo = {
        GFXBufferUsage::UNIFORM,
        GFXMemoryUsage::HOST,
        sizeof(Mat4),
        3 * sizeof(Mat4),
        GFXBufferFlagBit::NONE };

    Mat4 model, projection;
    Mat4::createPerspective(60.0f, 1.0f * _device->getWidth()/_device->getHeight(), 0.01f, 1000.0f, &projection);
    _mvpMatrix = _device->createBuffer(mvpMatrixInfo);
    _mvpMatrix->update(model.m, 0, sizeof(model));
    _mvpMatrix->update(projection.m, 2 * sizeof(Mat4), sizeof(projection));

    //color uniform
    GFXBufferInfo colorInfo = {
        GFXBufferUsage::UNIFORM,
        GFXMemoryUsage::HOST,
        sizeof(float),
        4 * sizeof(float),
        GFXBufferFlagBit::NONE
    };
    float color[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
    _color = _device->createBuffer(colorInfo);
    _color->update(color, 0, sizeof(color));
}

void BunnyTest::createInputAssembler()
{
    GFXAttribute position = {"a_position", GFXFormat::RGB32F, false, 0, false};
    GFXInputAssemblerInfo inputAssemblerInfo;
    inputAssemblerInfo.attributes.emplace_back(std::move(position));
    inputAssemblerInfo.vertexBuffers.emplace_back(_vertexBuffer);
    inputAssemblerInfo.indexBuffer = _indexBuffer;
    _inputAssembler = _device->createInputAssembler(inputAssemblerInfo);
}

void BunnyTest::createPipelineState()
{
    GFXBindingList bindingList = {
        {static_cast<uint>(Binding::MVP), GFXBindingType::UNIFORM_BUFFER, "MVP_Matrix"},
        {static_cast<uint>(Binding::COLOR), GFXBindingType::UNIFORM_BUFFER, "Color"}
    };
    GFXBindingLayoutInfo bindingLayoutInfo = { bindingList };
    _bindingLayout = _device->createBindingLayout(bindingLayoutInfo);
    _bindingLayout->bindBuffer(static_cast<uint>(Binding::MVP), _mvpMatrix);
    _bindingLayout->bindBuffer(static_cast<uint>(Binding::COLOR), _color);
    _bindingLayout->update();
    
    GFXPipelineLayoutInfo pipelineLayoutInfo;
    pipelineLayoutInfo.layouts = { _bindingLayout };
    auto pipelineLayout = _device->createPipelineLayout(pipelineLayoutInfo);

    GFXPipelineStateInfo pipelineStateInfo;
    pipelineStateInfo.primitive = GFXPrimitiveMode::TRIANGLE_LIST;
    pipelineStateInfo.shader = _shader;
    pipelineStateInfo.inputState = { _inputAssembler->getAttributes() };
    pipelineStateInfo.layout = pipelineLayout;
    pipelineStateInfo.renderPass = _device->getMainWindow()->getRenderPass();
    pipelineStateInfo.depthStencilState.depthTest = true;
    pipelineStateInfo.depthStencilState.depthWrite = true;
    pipelineStateInfo.depthStencilState.depthFunc = GFXComparisonFunc::LESS;
    _pipelineState = _device->createPipelineState(pipelineStateInfo);
    CC_SAFE_DESTROY(pipelineLayout);
}

void BunnyTest::tick(float dt)
{
    _dt += dt;
    Mat4::createLookAt(Vec3(30.0f * std::cos(_dt), 20.0f, 30.0f * std::sin(_dt)), Vec3(0.0f, 2.5f, 0.0f), Vec3(0.0f, 1.0f, 0.f), &_view);
    _mvpMatrix->update(_view.m, sizeof(Mat4), sizeof(_view));

    GFXRect render_area = { 0, 0, _device->getWidth(), _device->getHeight() };
    GFXColor clear_color = {0.0f, 0, 0, 1.0f};
    
    for(auto commandBuffer : _commandBuffers)
    {
        commandBuffer->begin();
        commandBuffer->beginRenderPass(_fbo, render_area, GFXClearFlagBit::ALL, std::move(std::vector<GFXColor>({clear_color})), 1.0f, 0);
        
        commandBuffer->bindInputAssembler(_inputAssembler);
        commandBuffer->bindBindingLayout(_bindingLayout);
        commandBuffer->bindPipelineState(_pipelineState);
        commandBuffer->draw(_inputAssembler);

        commandBuffer->endRenderPass();
        commandBuffer->end();
    }
    
    _device->getQueue()->submit(_commandBuffers);
    _device->present();
}

NS_CC_END
