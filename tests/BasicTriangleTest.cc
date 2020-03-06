#include "BasicTriangleTest.h"

#if (CC_PLATFORM == CC_PLATFORM_MAC_OSX)
#include "gfx-metal/GFXMTL.h"
#else
#include "gfx-gles2/GFXGLES2.h"
#include "gfx-gles3/GFXGLES3.h"
#endif

NS_CC_BEGIN

void BasicTriangle::destroy()
{
    CC_SAFE_DESTROY(_shader);
    CC_SAFE_DESTROY(_vertexBuffer);
    CC_SAFE_DESTROY(_inputAssembler);
    CC_SAFE_DESTROY(_pipelineState);
    CC_SAFE_DESTROY(_bindingLayout);
    CC_SAFE_DESTROY(_uniformBuffer);
}

bool BasicTriangle::initialize()
{
    createShader();
    createVertexBuffer();
    createInputAssembler();
    createPipeline();

    return true;
}

void BasicTriangle::createShader()
{
    GFXShaderStageList shaderStageList;
    GFXShaderStage vertexShaderStage;
    vertexShaderStage.type = GFXShaderType::VERTEX;
    
#if (CC_PLATFORM == CC_PLATFORM_MAC_OSX)
    vertexShaderStage.source = R"(
        #include <metal_stdlib>
        #include <simd/simd.h>
        
        using namespace metal;
        
        struct main0_out
        {
            float4 gl_Position [[position]];
        };
        
        struct main0_in
        {
            float2 a_position [[attribute(0)]];
        };
        
        vertex main0_out main0(main0_in in [[stage_in]])
        {
            main0_out out = {};
            out.gl_Position = float4(in.a_position, 0.0, 1.0);
            return out;
        }
    )";
#else
    
#ifdef USE_GLES2
    vertexShaderStage.source = R"(
        attribute vec2 a_position;
        void main()
        {
            gl_Position = vec4(a_position, 0.0, 1.0);
        }
    )";
#else
    vertexShaderStage.source = R"(#version 300 es
    in vec2 a_position;
    void main()
    {
    gl_Position = vec4(a_position, 0.0, 1.0);
    }
    )";
#endif // USE_GLES2
    
#endif // (CC_PLATFORM == CC_PLATFORM_MAC_OSX)
    shaderStageList.emplace_back(std::move(vertexShaderStage));

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
        
        fragment main0_out main0(constant Color& _12 [[buffer(0)]])
        {
            main0_out out = {};
            out.o_color = _12.u_color;
            return out;
        }
    )";
#else
    
#ifdef USE_GLES2
    fragmentShaderStage.source = R"(
        precision highp float;
        uniform vec4 u_color;
        void main()
        {
            gl_FragColor = u_color;
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
    out vec4 o_color;
    
    void main()
    {
        o_color = u_color;
    }
    )";
#endif // #ifdef USE_GLES2
    
#endif // #if (CC_PLATFORM == CC_PLATFORM_MAC_OSX)
    shaderStageList.emplace_back(std::move(fragmentShaderStage));

    GFXUniformList uniformList = { { "u_color", GFXType::FLOAT4, 1 } };
    GFXUniformBlockList uniformBlockList = { {0, "Color", uniformList} };

    GFXShaderInfo shaderInfo;
    shaderInfo.name = "Basic Triangle";
    shaderInfo.stages = std::move(shaderStageList);
    shaderInfo.blocks = std::move(uniformBlockList);
    _shader = _device->createShader(shaderInfo);
}

void BasicTriangle::createVertexBuffer()
{
    float vertexData[] = {
        -1.0f, -1.0f,
        1.0f, -1.0f,
        0.0f, 1.0f
    };

    GFXBufferInfo vertexBufferInfo = {
          GFXBufferUsage::VERTEX,
          GFXMemoryUsage::HOST,
          2 * sizeof(float),
          sizeof(vertexData),
          GFXBufferFlagBit::NONE };

    _vertexBuffer = _device->createBuffer(vertexBufferInfo);
    _vertexBuffer->update(vertexData, 0, sizeof(vertexData));

    GFXBufferInfo uniformBufferInfo = {
           GFXBufferUsage::UNIFORM,
           GFXMemoryUsage::HOST,
           4 * sizeof(float),
           sizeof(GFXColor),
           GFXBufferFlagBit::NONE };
     _uniformBuffer = _device->createBuffer(uniformBufferInfo);
}

void BasicTriangle::createInputAssembler()
{
    GFXAttribute position = {"a_position", GFXFormat::RG32F, false, 0, false};
    GFXInputAssemblerInfo inputAssemblerInfo;
    inputAssemblerInfo.attributes.emplace_back(std::move(position));
    inputAssemblerInfo.vertexBuffers.emplace_back(_vertexBuffer);
    _inputAssembler = _device->createInputAssembler(inputAssemblerInfo);
}

void BasicTriangle::createPipeline()
{
    GFXBindingList bindingList = { {0, GFXBindingType::UNIFORM_BUFFER, "u_color"} };
    GFXBindingLayoutInfo bindingLayoutInfo = { bindingList };
    _bindingLayout = _device->createBindingLayout(bindingLayoutInfo);

    GFXPipelineLayoutInfo pipelineLayoutInfo;
    pipelineLayoutInfo.layouts = { _bindingLayout };
    auto pipelineLayout = _device->createPipelineLayout(pipelineLayoutInfo);

    GFXPipelineStateInfo pipelineInfo;
    pipelineInfo.primitive = GFXPrimitiveMode::TRIANGLE_LIST;
    pipelineInfo.shader = _shader;
    pipelineInfo.inputState = { _inputAssembler->getAttributes() };
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = _device->getMainWindow()->getRenderPass();

    _pipelineState = _device->createPipelineState(pipelineInfo);

    CC_SAFE_DESTROY(pipelineLayout);
}

void BasicTriangle::tick(float dt) {

    GFXRect render_area = {0, 0, _device->getWidth(), _device->getHeight() };
    _time += dt;
    GFXColor clear_color = {1.0f, 0, 0, 1.0f};
    
    GFXColor uniformColor;
    uniformColor.r = std::abs(std::sin(_time));
    uniformColor.g = 1.0f;
    uniformColor.b = 0.0f;
    uniformColor.a = 1.0f;

    _uniformBuffer->update(&uniformColor, 0, sizeof(uniformColor));
    _bindingLayout->bindBuffer(0, _uniformBuffer);
    _bindingLayout->update();

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
