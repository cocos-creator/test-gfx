#include "BasicTriangleTest.h"
#include "gfx-gles2/GFXGLES2.h"
#include "gfx-gles3/GFXGLES3.h"

NS_CC_BEGIN

void BasicTriangle::Destroy()
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
    vertexShaderStage.source = R"(
        attribute vec2 a_position;
        void main()
        {
            gl_Position = vec4(a_position, 0.0, 1.0);
        }
    )";
    shaderStageList.emplace_back(std::move(vertexShaderStage));

    GFXShaderStage fragmentShaderStage;
    fragmentShaderStage.type = GFXShaderType::FRAGMENT;
    fragmentShaderStage.source = R"(
        #ifdef GL_ES
        precision highp float;
        #endif
        uniform vec4 u_color;
        void main()
        {
            gl_FragColor = u_color;
        }
    )";
    shaderStageList.emplace_back(std::move(fragmentShaderStage));

    GFXUniformList uniformList = { { "u_color", GFXType::FLOAT4, 1 } };
    GFXUniformBlockList uniformBlockList = { {0, "UniformColor", uniformList} };

    GFXShaderInfo shaderInfo;
    shaderInfo.name = "Basic Triangle";
    shaderInfo.stages = std::move(shaderStageList);
    shaderInfo.blocks = std::move(uniformBlockList);
    _shader = _device->CreateGFXShader(shaderInfo);
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

    _vertexBuffer = _device->CreateGFXBuffer(vertexBufferInfo);
    _vertexBuffer->Update(vertexData, 0, sizeof(vertexData));

    GFXBufferInfo uniformBufferInfo = {
           GFXBufferUsage::UNIFORM,
           GFXMemoryUsage::HOST,
           4 * sizeof(float),
           sizeof(GFXColor),
           GFXBufferFlagBit::BAKUP_BUFFER };
     _uniformBuffer = _device->CreateGFXBuffer(uniformBufferInfo);
}

void BasicTriangle::createInputAssembler()
{
    GFXAttribute position = {"a_position", GFXFormat::RG32F, false, 0, false};
    GFXInputAssemblerInfo inputAssemblerInfo;
    inputAssemblerInfo.attributes.emplace_back(std::move(position));
    inputAssemblerInfo.vertex_buffers.emplace_back(_vertexBuffer);
    _inputAssembler = _device->CreateGFXInputAssembler(inputAssemblerInfo);
}

void BasicTriangle::createPipeline()
{
    GFXBindingList bindingList = { {0, GFXBindingType::UNIFORM_BUFFER, "u_color"} };
    GFXBindingLayoutInfo bindingLayoutInfo = { bindingList };
    _bindingLayout = _device->CreateGFXBindingLayout(bindingLayoutInfo);

    GFXPipelineLayoutInfo pipelineLayoutInfo;
    pipelineLayoutInfo.layouts = { _bindingLayout };
    auto pipelineLayout = _device->CreateGFXPipelieLayout(pipelineLayoutInfo);

    GFXPipelineStateInfo pipelineInfo;
    pipelineInfo.primitive = GFXPrimitiveMode::TRIANGLE_LIST;
    pipelineInfo.shader = _shader;
    pipelineInfo.is = { _inputAssembler->attributes() };
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.render_pass = _device->window()->render_pass();

    _pipelineState = _device->CreateGFXPipelineState(pipelineInfo);

    CC_SAFE_DESTROY(pipelineLayout);
}

void BasicTriangle::tick(float dt) {

    GFXRect render_area = {0, 0, _device->width(), _device->height() };
    _time += dt;
    GFXColor clear_color = {0.1f, 0.1f, 0.1f, 1.0f};
    
    GFXColor uniformColor;
    uniformColor.r = std::abs(std::sin(_time));
    uniformColor.g = 1.0f;
    uniformColor.b = 0.0f;
    uniformColor.a = 1.0f;

    _uniformBuffer->Update(&uniformColor, 0, sizeof(uniformColor));
    _bindingLayout->BindBuffer(0, _uniformBuffer);
    _bindingLayout->Update();

    _commandBuffer->Begin();
    _commandBuffer->BeginRenderPass(_fbo, render_area, GFXClearFlagBit::ALL, &clear_color, 1, 1.0f, 0);
    _commandBuffer->BindInputAssembler(_inputAssembler);
    _commandBuffer->BindBindingLayout(_bindingLayout);
    _commandBuffer->BindPipelineState(_pipelineState);
    _commandBuffer->Draw(_inputAssembler);
    _commandBuffer->EndRenderPass();
    _commandBuffer->End();

    _device->queue()->submit(&_commandBuffer, 1);
    _device->Present();
}

NS_CC_END
