#include "BasicTriangleTest.h"

namespace cc {

    void BasicTriangle::destroy() {
        CC_SAFE_DESTROY(_vertexBuffer);
        CC_SAFE_DESTROY(_inputAssembler);
        CC_SAFE_DESTROY(_uniformBuffer);
        CC_SAFE_DESTROY(_shader);
        CC_SAFE_DESTROY(_bindingLayout);
        CC_SAFE_DESTROY(_pipelineLayout);
        CC_SAFE_DESTROY(_pipelineState);
        CC_SAFE_DESTROY(_indexBuffer);
        CC_SAFE_DESTROY(_indirectBuffer);
    }

    bool BasicTriangle::initialize() {
        createShader();
        createVertexBuffer();
        createInputAssembler();
        createPipeline();

        return true;
    }

    void BasicTriangle::createShader() {
        gfx::GFXShaderStageList shaderStageList;
        gfx::GFXShaderStage vertexShaderStage;
        vertexShaderStage.type = gfx::GFXShaderType::VERTEX;

        //#if (CC_PLATFORM == CC_PLATFORM_MAC_OSX && defined(USE_METAL))
        //    vertexShaderStage.source = R"(
        //        #include <metal_stdlib>
        //        #include <simd/simd.h>
        //
        //        using namespace metal;
        //
        //        struct main0_out
        //        {
        //            float4 gl_Position [[position]];
        //        };
        //
        //        struct main0_in
        //        {
        //            float2 a_position [[attribute(0)]];
        //        };
        //
        //        vertex main0_out main0(main0_in in [[stage_in]])
        //        {
        //            main0_out out = {};
        //            out.gl_Position = float4(in.a_position, 0.0, 1.0);
        //            return out;
        //        }
        //    )";
        //#else

#if defined(USE_VULKAN) || defined(USE_METAL)
        vertexShaderStage.source = R"(
        layout(location = 0) in vec2 a_position;
        void main()
        {
            gl_Position = vec4(a_position, 0.0, 1.0);
        }
    )";
#elif defined(USE_GLES2)
        vertexShaderStage.source = R"(
        attribute vec2 a_position;
        void main()
        {
            gl_Position = vec4(a_position, 0.0, 1.0);
        }
    )";
#else
        vertexShaderStage.source = R"(
    in vec2 a_position;
    void main()
    {
    gl_Position = vec4(a_position, 0.0, 1.0);
    }
    )";
#endif // USE_GLES2

        //#endif // (CC_PLATFORM == CC_PLATFORM_MAC_OSX)
        shaderStageList.emplace_back(std::move(vertexShaderStage));

        gfx::GFXShaderStage fragmentShaderStage;
        fragmentShaderStage.type = gfx::GFXShaderType::FRAGMENT;

        //#if (CC_PLATFORM == CC_PLATFORM_MAC_OSX && defined(USE_METAL))
        //    fragmentShaderStage.source = R"(
        //    #ifdef GL_ES
        //    precision highp float;
        //    #endif
        //    layout(std140, binding = 0) uniform Color
        //    {
        //        vec4 u_color;
        //    };
        //    layout(location = 0) out vec4 o_color;
        //
        //    void main()
        //    {
        //        o_color = u_color;
        //    }
        //    )";
        //    fragmentShaderStage.source = R"(
        //        #include <metal_stdlib>
        //        #include <simd/simd.h>
        //
        //        using namespace metal;
        //
        //        struct Color
        //        {
        //            float4 u_color;
        //        };
        //
        //        struct main0_out
        //        {
        //            float4 o_color [[color(0)]];
        //        };
        //
        //        fragment main0_out main0(constant Color& _12 [[buffer(0)]])
        //        {
        //            main0_out out = {};
        //            out.o_color = _12.u_color;
        //            return out;
        //        }
        //    )";
        //#else

#if defined(USE_VULKAN) || defined(USE_METAL)
        fragmentShaderStage.source = R"(
    #ifdef GL_ES
                precision highp float;
    #endif
        layout(binding = 0) uniform Color
        {
            vec4 u_color;
        };
        layout(location = 0) out vec4 o_color;
    
        void main()
        {
            o_color = u_color;
        }
    )";
#elif defined(USE_GLES2)
        fragmentShaderStage.source = R"(
        precision highp float;
        uniform vec4 u_color;
        void main()
        {
            gl_FragColor = vec4(1, 1, 0, 1); // u_color;
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
    out vec4 o_color;
    
    void main()
    {
        o_color = vec4(1, 1, 0, 1); // u_color;
    }
    )";
#endif // #ifdef USE_GLES2

        //#endif // #if (CC_PLATFORM == CC_PLATFORM_MAC_OSX)
        shaderStageList.emplace_back(std::move(fragmentShaderStage));

        gfx::GFXUniformList uniformList = { { "u_color", gfx::GFXType::FLOAT4, 1 } };
        gfx::GFXUniformBlockList uniformBlockList = { { gfx::GFXShaderType::FRAGMENT, 0, "Color", uniformList } };
        gfx::GFXAttributeList attributeList = { { "a_position", gfx::GFXFormat::RG32F, false, 0, false, 0 } };

        gfx::GFXShaderInfo shaderInfo;
        shaderInfo.name = "Basic Triangle";
        shaderInfo.stages = std::move(shaderStageList);
        shaderInfo.attributes = std::move(attributeList);
        shaderInfo.blocks = std::move(uniformBlockList);
        _shader = _device->createShader(shaderInfo);
    }

    void BasicTriangle::createVertexBuffer() {
        float ySign = _device->getProjectionSignY();

        float vertexData[] = {
            -0.5f, -0.5f * ySign,
            0.5f, -0.5f * ySign,
            0.0f, 0.5f * ySign,
            0.5f, 0.5f * ySign,
        };

        gfx::GFXBufferInfo vertexBufferInfo = {
              gfx::GFXBufferUsage::VERTEX,
              gfx::GFXMemoryUsage::DEVICE,
              2 * sizeof(float),
              sizeof(vertexData),
              gfx::GFXBufferFlagBit::NONE,
        };

        _vertexBuffer = _device->createBuffer(vertexBufferInfo);
        _vertexBuffer->update(vertexData, 0, sizeof(vertexData));

        gfx::GFXBufferInfo uniformBufferInfo = {
               gfx::GFXBufferUsage::UNIFORM,
               gfx::GFXMemoryUsage::DEVICE | gfx::GFXMemoryUsage::HOST,
               4 * sizeof(float),
               sizeof(gfx::GFXColor),
               gfx::GFXBufferFlagBit::NONE,
        };
        _uniformBuffer = _device->createBuffer(uniformBufferInfo);

        gfx::GFXDrawInfo drawInfo;
        drawInfo.firstIndex = 0;
        drawInfo.indexCount = 6;
        drawInfo.instanceCount = 0;

        gfx::GFXBufferInfo indirectBufferInfo = {
            gfx::GFXBufferUsageBit::INDIRECT,
            gfx::GFXMemoryUsage::DEVICE | gfx::GFXMemoryUsage::HOST,
            sizeof(gfx::GFXDrawInfo),
            sizeof(gfx::GFXDrawInfo),
            gfx::GFXBufferFlagBit::NONE
        };
        _indirectBuffer = _device->createBuffer(indirectBufferInfo);
        _indirectBuffer->update(&drawInfo, 0, sizeof(gfx::GFXDrawInfo));

        unsigned short indices[] = { 0,1,2 };
        gfx::GFXBufferInfo indexBufferInfo = {
            gfx::GFXBufferUsageBit::INDEX,
            gfx::GFXMemoryUsage::DEVICE,
            sizeof(unsigned short),
            sizeof(indices),
            gfx::GFXBufferFlagBit::NONE
        };
        _indexBuffer = _device->createBuffer(indexBufferInfo);
        _indexBuffer->update(indices, 0, sizeof(indices));

    }

    void BasicTriangle::createInputAssembler() {
        gfx::GFXAttribute position = { "a_position", gfx::GFXFormat::RG32F, false, 0, false };
        gfx::GFXInputAssemblerInfo inputAssemblerInfo;
        inputAssemblerInfo.attributes.emplace_back(std::move(position));
        inputAssemblerInfo.vertexBuffers.emplace_back(_vertexBuffer);
        inputAssemblerInfo.indexBuffer = _indexBuffer;
        inputAssemblerInfo.indirectBuffer = _indirectBuffer;
        _inputAssembler = _device->createInputAssembler(inputAssemblerInfo);
    }

    void BasicTriangle::createPipeline() {
        gfx::GFXBindingList bindingList = { { gfx::GFXShaderType::FRAGMENT, 0, gfx::GFXBindingType::UNIFORM_BUFFER, "u_color", 1 } };
        gfx::GFXBindingLayoutInfo bindingLayoutInfo = { bindingList };
        _bindingLayout = _device->createBindingLayout(bindingLayoutInfo);

        gfx::GFXPipelineLayoutInfo pipelineLayoutInfo;
        pipelineLayoutInfo.layouts = { _bindingLayout };
        _pipelineLayout = _device->createPipelineLayout(pipelineLayoutInfo);

        gfx::GFXPipelineStateInfo pipelineInfo;
        pipelineInfo.primitive = gfx::GFXPrimitiveMode::TRIANGLE_LIST;
        pipelineInfo.shader = _shader;
        pipelineInfo.inputState = { _inputAssembler->getAttributes() };
        pipelineInfo.layout = _pipelineLayout;
        pipelineInfo.renderPass = _fbo->getRenderPass();

        _pipelineState = _device->createPipelineState(pipelineInfo);
    }

    void BasicTriangle::tick(float dt) {

        gfx::GFXRect render_area = { 0, 0, _device->getWidth(), _device->getHeight() };
        _time += dt;
        gfx::GFXColor clear_color = { 1.0f, 0, 0, 1.0f };

        gfx::GFXColor uniformColor;
        uniformColor.r = std::abs(std::sin(_time));
        uniformColor.g = 1.0f;
        uniformColor.b = 0.0f;
        uniformColor.a = 1.0f;

        _uniformBuffer->update(&uniformColor, 0, sizeof(uniformColor));
        _bindingLayout->bindBuffer(0, _uniformBuffer);
        _bindingLayout->update();

        _device->acquire();

        auto commandBuffer = _commandBuffers[0];
        commandBuffer->begin();
        commandBuffer->beginRenderPass(_fbo, render_area, gfx::GFXClearFlagBit::ALL, std::move(std::vector<gfx::GFXColor>({ clear_color })), 1.0f, 0);
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
