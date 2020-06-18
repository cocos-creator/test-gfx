#include "StencilTest.h"

namespace cc {

    namespace {
        enum class PipelineType : uint8_t {
            STENCIL,
            IMAGE,
            CAVAS,
            FRONT_STENCIL,
            BACK_STENCIL,
            FRONT_BACK_STENCIL,
            COUNT
        };
    }

    void StencilTest::destroy() {
        CC_SAFE_DESTROY(_shader);
        CC_SAFE_DESTROY(_labelTexture);
        CC_SAFE_DESTROY(_uvCheckerTexture);
        CC_SAFE_DESTROY(_vertexBuffer);
        CC_SAFE_DESTROY(_inputAssembler);
        CC_SAFE_DESTROY(_sampler);
        for (uint i = 0; i < BINDING_COUNT; i++) {
            CC_SAFE_DESTROY(_uniformBuffer[i]);
            CC_SAFE_DESTROY(_bindingLayout[i]);
            CC_SAFE_DESTROY(_pipelineLayout[i]);
        }
        for (uint i = 0; i < PIPELIE_COUNT; i++) {
            CC_SAFE_DESTROY(_pipelineState[i]);
        }
    }

    bool StencilTest::initialize() {
        createShader();
        createBuffers();
        createTextures();
        createInputAssembler();
        createPipelineState();
        return true;
    }

    void StencilTest::createShader() {
        gfx::GFXShaderStageList shaderStageList;
        gfx::GFXShaderStage vertexShaderStage;
        vertexShaderStage.type = gfx::GFXShaderType::VERTEX;

        //#if (CC_PLATFORM == CC_PLATFORM_MAC_OSX && defined(USE_METAL))
        //    vertexShaderStage.source = R"(
        //    #include <metal_stdlib>
        //    #include <simd/simd.h>
        //
        //    using namespace metal;
        //
        //    struct MVP_Matrix
        //    {
        //        float4x4 u_mvpMatrix;
        //    };
        //
        //    struct main0_out
        //    {
        //        float2 v_texcoord [[user(locn0)]];
        //        float4 gl_Position [[position]];
        //    };
        //
        //    struct main0_in
        //    {
        //        float2 a_position [[attribute(0)]];
        //    };
        //
        //    vertex main0_out main0(main0_in in [[stage_in]], constant MVP_Matrix&
        //    _16 [[buffer(0)]])
        //    {
        //        main0_out out = {};
        //        out.gl_Position = _16.u_mvpMatrix * float4(in.a_position, 0.0, 1.0);
        //        out.v_texcoord = (in.a_position * 0.5) + float2(0.5);
        //        out.v_texcoord = float2(out.v_texcoord.x, 1.0 - out.v_texcoord.y);
        //        return out;
        //    })";
        //#else

#if defined(USE_VULKAN) || defined(USE_METAL)
        vertexShaderStage.source = R"(
    layout(location = 0) in vec2 a_position;
    layout(binding = 0) uniform MVP_Matrix
    {
        mat4 u_mvpMatrix;
    };
    layout(location = 0) out vec2 v_texcoord;
    void main () {
        gl_Position = u_mvpMatrix * vec4(a_position, 0, 1);
        v_texcoord = a_position * 0.5 + 0.5;
        v_texcoord = vec2(v_texcoord.x, 1.0 - v_texcoord.y);
    }
    )";
#elif defined(USE_GLES2)
        vertexShaderStage.source = R"(
    attribute vec2 a_position;
    uniform mat4 u_mvpMatrix;
    varying vec2 v_texcoord;
    void main () {
        gl_Position = u_mvpMatrix * vec4(a_position, 0, 1);
        v_texcoord = a_position * 0.5 + 0.5;
        v_texcoord = vec2(v_texcoord.x, 1.0 - v_texcoord.y);
    }
    )";
#else
        vertexShaderStage.source = R"(
    in vec2 a_position;
    layout(std140) uniform MVP_Matrix
    {
        mat4 u_mvpMatrix;
    };
    out vec2 v_texcoord;
    void main () {
        gl_Position = u_mvpMatrix * vec4(a_position, 0, 1);
        v_texcoord = a_position * 0.5 + 0.5;
        v_texcoord = vec2(v_texcoord.x, 1.0 - v_texcoord.y);
    }
    )";
#endif // USE_GLES2
        //#endif // (CC_PLATFORM == CC_PLATFORM_MAC_OSX)

        shaderStageList.emplace_back(std::move(vertexShaderStage));

        gfx::GFXShaderStage fragmentShaderStage;
        fragmentShaderStage.type = gfx::GFXShaderType::FRAGMENT;

        //#if (CC_PLATFORM == CC_PLATFORM_MAC_OSX && defined(USE_METAL))
        //    fragmentShaderStage.source = R"(
        //    #include <metal_stdlib>
        //    #include <simd/simd.h>
        //
        //    using namespace metal;
        //
        //    struct main0_out
        //    {
        //        float4 o_color [[color(0)]];
        //    };
        //
        //    struct main0_in
        //    {
        //        float2 v_texcoord [[user(locn0)]];
        //    };
        //
        //    fragment main0_out main0(main0_in in [[stage_in]], texture2d<float>
        //    u_texture [[texture(0)]], sampler u_textureSmplr [[sampler(0)]])
        //    {
        //        main0_out out = {};
        //        out.o_color = u_texture.sample(u_textureSmplr, in.v_texcoord);
        //        return out;
        //    }
        //    )";
        //#else

#if defined(USE_VULKAN) || defined(USE_METAL)
        fragmentShaderStage.source = R"(
    #ifdef GL_ES
                precision highp float;
    #endif
    layout(binding = 1) uniform sampler2D u_texture;
    layout(location = 0) in vec2 v_texcoord;
    layout(location = 0) out vec4 o_color;
    void main () {
        o_color = texture(u_texture, v_texcoord);
    }
    )";
#elif defined(USE_GLES2)
        fragmentShaderStage.source = R"(
    #ifdef GL_ES
    precision highp float;
    #endif
    uniform sampler2D u_texture;
    varying vec2 v_texcoord;
    void main () {
        gl_FragColor = texture2D(u_texture, v_texcoord);
    }
    )";
#else
        fragmentShaderStage.source = R"(
#ifdef GL_ES
    precision highp float;
#endif
    uniform sampler2D u_texture;
    in vec2 v_texcoord;
    out vec4 o_color;
    void main () {
        o_color = texture(u_texture, v_texcoord);
    }
    )";
#endif // USE_GLES2

        //#endif // (CC_PLATFORM == CC_PLATFORM_MAC_OSX)
        shaderStageList.emplace_back(std::move(fragmentShaderStage));

        gfx::GFXAttributeList attributeList = { { "a_position", gfx::GFXFormat::RG32F, false, 0, false, 0 } };
        gfx::GFXUniformList mvpMatrix = { {"u_mvpMatrix", gfx::GFXType::MAT4, 1} };
        gfx::GFXUniformBlockList uniformBlockList = {
            {gfx::GFXShaderType::VERTEX, 0, "MVP_Matrix", mvpMatrix} };
        gfx::GFXUniformSamplerList sampler = {
            {gfx::GFXShaderType::FRAGMENT, 1, "u_texture", gfx::GFXType::SAMPLER2D, 1} };

        gfx::GFXShaderInfo shaderInfo;
        shaderInfo.name = "Stencil Test";
        shaderInfo.stages = std::move(shaderStageList);
        shaderInfo.attributes = std::move(attributeList);
        shaderInfo.blocks = std::move(uniformBlockList);
        shaderInfo.samplers = std::move(sampler);
        _shader = _device->createShader(shaderInfo);
    }

    void StencilTest::createBuffers() {
        float vertexData[] = { -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,

                              1.0f,  1.0f,  -1.0f, -1.0f, -1.0f, 1.0f };

        gfx::GFXBufferInfo vertexBufferInfo = { gfx::GFXBufferUsage::VERTEX,
                                          gfx::GFXMemoryUsage::DEVICE, 2 * sizeof(float),
                                          sizeof(vertexData), gfx::GFXBufferFlagBit::NONE };

        _vertexBuffer = _device->createBuffer(vertexBufferInfo);
        _vertexBuffer->update(vertexData, 0, sizeof(vertexData));

        gfx::GFXBufferInfo uniformBufferInfo = {
            gfx::GFXBufferUsage::UNIFORM, gfx::GFXMemoryUsage::HOST | gfx::GFXMemoryUsage::DEVICE,
            sizeof(Mat4), sizeof(Mat4), gfx::GFXBufferFlagBit::NONE };

        Mat4 transform[BINDING_COUNT];
        transform[0].scale(0.5f);
        transform[0].rotateZ(gfx::math::PI / 4);
        transform[1].scale(0.5f, 0.5f, 0.25f);

        for (uint i = 0; i < BINDING_COUNT; i++) {
            _uniformBuffer[i] = _device->createBuffer(uniformBufferInfo);
            //        TestBaseI::modifyProjectionBasedOnDevice(transform[i]);
            _uniformBuffer[i]->update(&transform[i], 0, sizeof(transform[i]));
        }
    }

    void StencilTest::createTextures() {
        // load stencil image
        auto stencilImage = new cc::Image();
        stencilImage->autorelease();
        bool ret = stencilImage->initWithImageFile("stencil.jpg");
        assert(ret);
        auto stencilImageData = TestBaseI::RGB2RGBA(stencilImage);

        gfx::GFXBufferTextureCopy labelTextureRegion;
        labelTextureRegion.buffTexHeight = stencilImage->getHeight();
        labelTextureRegion.texExtent.width = stencilImage->getWidth();
        labelTextureRegion.texExtent.height = stencilImage->getHeight();
        labelTextureRegion.texExtent.depth = 1;

        gfx::GFXBufferTextureCopyList regions;
        regions.push_back(std::move(labelTextureRegion));

        gfx::GFXTextureInfo labelTextureInfo;
        labelTextureInfo.usage = gfx::GFXTextureUsage::SAMPLED;
        labelTextureInfo.format = gfx::GFXFormat::RGBA8;
        labelTextureInfo.width = stencilImage->getWidth();
        labelTextureInfo.height = stencilImage->getHeight();
        _labelTexture = _device->createTexture(labelTextureInfo);

        gfx::GFXDataArray stencilBuffer = { {stencilImageData} };
        _device->copyBuffersToTexture(stencilBuffer, _labelTexture, regions);

        // load uv_checker_02.jpg
        auto img = new cc::Image();
        img->autorelease();
        ret = img->initWithImageFile("uv_checker_02.jpg");
        assert(ret);
        auto imgData = TestBaseI::RGB2RGBA(img);

        gfx::GFXTextureInfo textureInfo;
        textureInfo.usage = gfx::GFXTextureUsage::SAMPLED;
        textureInfo.format = gfx::GFXFormat::RGBA8;
        textureInfo.width = img->getWidth();
        textureInfo.height = img->getHeight();
        _uvCheckerTexture = _device->createTexture(textureInfo);

        gfx::GFXBufferTextureCopy textureRegion;
        textureRegion.buffTexHeight = img->getHeight();
        textureRegion.texExtent.width = img->getWidth();
        textureRegion.texExtent.height = img->getHeight();
        textureRegion.texExtent.depth = 1;

        gfx::GFXBufferTextureCopyList uvTextureRegions;
        uvTextureRegions.push_back(std::move(textureRegion));

        gfx::GFXDataArray uvImageBuffer = { {imgData} };
        _device->copyBuffersToTexture(uvImageBuffer, _uvCheckerTexture,
            uvTextureRegions);

        // create sampler
        gfx::GFXSamplerInfo samplerInfo;
        _sampler = _device->createSampler(samplerInfo);

        delete[] imgData;
        delete[] stencilImageData;
    }

    void StencilTest::createInputAssembler() {
        gfx::GFXAttribute position = { "a_position", gfx::GFXFormat::RG32F, false, 0, false };
        gfx::GFXInputAssemblerInfo inputAssemblerInfo;
        inputAssemblerInfo.attributes.emplace_back(std::move(position));
        inputAssemblerInfo.vertexBuffers.emplace_back(_vertexBuffer);
        _inputAssembler = _device->createInputAssembler(inputAssemblerInfo);
    }

    void StencilTest::createPipelineState() {
        gfx::GFXBindingList bindingList = {
            {gfx::GFXShaderType::VERTEX, 0, gfx::GFXBindingType::UNIFORM_BUFFER, "u_mvpMatrix",
             1},
            {gfx::GFXShaderType::FRAGMENT, 1, gfx::GFXBindingType::SAMPLER, "u_texture", 1} };
        gfx::GFXBindingLayoutInfo bindingLayoutInfo = { bindingList };

        gfx::GFXTexture *texView[BINDING_COUNT] = { _labelTexture, _uvCheckerTexture };
        for (uint i = 0; i < BINDING_COUNT; i++) {
            _bindingLayout[i] = _device->createBindingLayout(bindingLayoutInfo);
            _bindingLayout[i]->bindBuffer(0, _uniformBuffer[i]);
            _bindingLayout[i]->bindSampler(1, _sampler);
            _bindingLayout[i]->bindTexture(1, texView[i]);
            _bindingLayout[i]->update();

            gfx::GFXPipelineLayoutInfo pipelineLayoutInfo;
            pipelineLayoutInfo.layouts = { _bindingLayout[i] };
            _pipelineLayout[i] = _device->createPipelineLayout(pipelineLayoutInfo);
        }

        // lable
        gfx::GFXPipelineStateInfo pipelineInfo[PIPELIE_COUNT];
        pipelineInfo[(uint8_t)PipelineType::STENCIL].primitive =
            gfx::GFXPrimitiveMode::TRIANGLE_LIST;
        pipelineInfo[(uint8_t)PipelineType::STENCIL].shader = _shader;
        pipelineInfo[(uint8_t)PipelineType::STENCIL].inputState = {
            _inputAssembler->getAttributes() };
        pipelineInfo[(uint8_t)PipelineType::STENCIL].layout = _pipelineLayout[0];
        pipelineInfo[(uint8_t)PipelineType::STENCIL].renderPass =
            _fbo->getRenderPass();
        pipelineInfo[(uint8_t)PipelineType::STENCIL].depthStencilState.depthTest =
            false;
        pipelineInfo[(uint8_t)PipelineType::STENCIL]
            .depthStencilState.stencilTestFront = false;
        pipelineInfo[(uint8_t)PipelineType::STENCIL]
            .depthStencilState.stencilTestBack = false;
        pipelineInfo[(uint8_t)PipelineType::STENCIL].rasterizerState.cullMode =
            gfx::GFXCullMode::NONE;
        pipelineInfo[(uint8_t)PipelineType::STENCIL].dynamicStates.push_back(
            gfx::GFXDynamicState::VIEWPORT);
        _pipelineState[(uint8_t)PipelineType::STENCIL] = _device->createPipelineState(
            pipelineInfo[(uint8_t)PipelineType::STENCIL]);

        // uv_image
        pipelineInfo[(uint8_t)PipelineType::IMAGE].primitive =
            gfx::GFXPrimitiveMode::TRIANGLE_LIST;
        pipelineInfo[(uint8_t)PipelineType::IMAGE].shader = _shader;
        pipelineInfo[(uint8_t)PipelineType::IMAGE].inputState = {
            _inputAssembler->getAttributes() };
        pipelineInfo[(uint8_t)PipelineType::IMAGE].layout = _pipelineLayout[1];
        pipelineInfo[(uint8_t)PipelineType::IMAGE].renderPass = _fbo->getRenderPass();
        pipelineInfo[(uint8_t)PipelineType::IMAGE].depthStencilState.depthTest =
            false;
        pipelineInfo[(uint8_t)PipelineType::IMAGE]
            .depthStencilState.stencilTestFront = false;
        pipelineInfo[(uint8_t)PipelineType::IMAGE].depthStencilState.stencilTestBack =
            false;
        pipelineInfo[(uint8_t)PipelineType::IMAGE].rasterizerState.cullMode =
            gfx::GFXCullMode::NONE;
        pipelineInfo[(uint8_t)PipelineType::IMAGE].dynamicStates.push_back(
            gfx::GFXDynamicState::VIEWPORT);
        _pipelineState[(uint8_t)PipelineType::IMAGE] =
            _device->createPipelineState(pipelineInfo[(uint8_t)PipelineType::IMAGE]);

        // do back and front stencil test
        pipelineInfo[(uint8_t)PipelineType::CAVAS].primitive =
            gfx::GFXPrimitiveMode::TRIANGLE_LIST;
        pipelineInfo[(uint8_t)PipelineType::CAVAS].shader = _shader;
        pipelineInfo[(uint8_t)PipelineType::CAVAS].inputState = {
            _inputAssembler->getAttributes() };
        pipelineInfo[(uint8_t)PipelineType::CAVAS].layout = _pipelineLayout[0];
        pipelineInfo[(uint8_t)PipelineType::CAVAS].renderPass = _fbo->getRenderPass();
        pipelineInfo[(uint8_t)PipelineType::CAVAS].depthStencilState.depthTest = true;
        pipelineInfo[(uint8_t)PipelineType::CAVAS].depthStencilState.depthWrite =
            false;
        pipelineInfo[(uint8_t)PipelineType::CAVAS]
            .depthStencilState.stencilTestFront =
            pipelineInfo[(uint8_t)PipelineType::CAVAS]
            .depthStencilState.stencilTestBack = true;
        pipelineInfo[(uint8_t)PipelineType::CAVAS]
            .depthStencilState.stencilFuncFront =
            pipelineInfo[(uint8_t)PipelineType::CAVAS]
            .depthStencilState.stencilFuncBack = gfx::GFXComparisonFunc::NEVER;
        pipelineInfo[(uint8_t)PipelineType::CAVAS]
            .depthStencilState.stencilReadMaskFront =
            pipelineInfo[(uint8_t)PipelineType::CAVAS]
            .depthStencilState.stencilReadMaskBack = 0xFF;
        pipelineInfo[(uint8_t)PipelineType::CAVAS]
            .depthStencilState.stencilWriteMaskFront =
            pipelineInfo[(uint8_t)PipelineType::CAVAS]
            .depthStencilState.stencilWriteMaskBack = 0xFF;
        pipelineInfo[(uint8_t)PipelineType::CAVAS]
            .depthStencilState.stencilFailOpFront =
            pipelineInfo[(uint8_t)PipelineType::CAVAS]
            .depthStencilState.stencilFailOpBack = gfx::GFXStencilOp::REPLACE;
        pipelineInfo[(uint8_t)PipelineType::CAVAS]
            .depthStencilState.stencilZFailOpFront =
            pipelineInfo[(uint8_t)PipelineType::CAVAS]
            .depthStencilState.stencilZFailOpBack = gfx::GFXStencilOp::KEEP;
        pipelineInfo[(uint8_t)PipelineType::CAVAS]
            .depthStencilState.stencilPassOpFront =
            pipelineInfo[(uint8_t)PipelineType::CAVAS]
            .depthStencilState.stencilPassOpBack = gfx::GFXStencilOp::KEEP;
        pipelineInfo[(uint8_t)PipelineType::CAVAS].depthStencilState.stencilRefFront =
            pipelineInfo[(uint8_t)PipelineType::CAVAS]
            .depthStencilState.stencilRefBack = 0x1;
        pipelineInfo[(uint8_t)PipelineType::CAVAS].rasterizerState.cullMode =
            gfx::GFXCullMode::NONE;
        pipelineInfo[(uint8_t)PipelineType::CAVAS].dynamicStates.push_back(
            gfx::GFXDynamicState::VIEWPORT);
        _pipelineState[(uint8_t)PipelineType::CAVAS] =
            _device->createPipelineState(pipelineInfo[(uint8_t)PipelineType::CAVAS]);

        pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].primitive =
            gfx::GFXPrimitiveMode::TRIANGLE_LIST;
        pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].shader = _shader;
        pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].inputState = {
            _inputAssembler->getAttributes() };
        pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].layout =
            _pipelineLayout[1];
        pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].renderPass =
            _fbo->getRenderPass();
        pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL]
            .depthStencilState.depthTest = true;
        pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL]
            .depthStencilState.depthWrite = false;
        pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL]
            .depthStencilState.stencilTestFront =
            pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL]
            .depthStencilState.stencilTestBack = true;
        pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL]
            .depthStencilState.stencilFuncFront =
            pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL]
            .depthStencilState.stencilFuncBack = gfx::GFXComparisonFunc::EQUAL;
        pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL]
            .depthStencilState.stencilReadMaskFront =
            pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL]
            .depthStencilState.stencilReadMaskBack = 0xFF;
        pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL]
            .depthStencilState.stencilWriteMaskFront =
            pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL]
            .depthStencilState.stencilWriteMaskBack = 0xFF;
        pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL]
            .depthStencilState.stencilFailOpFront =
            pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL]
            .depthStencilState.stencilFailOpBack = gfx::GFXStencilOp::KEEP;
        pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL]
            .depthStencilState.stencilZFailOpFront =
            pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL]
            .depthStencilState.stencilZFailOpBack = gfx::GFXStencilOp::KEEP;
        pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL]
            .depthStencilState.stencilPassOpFront =
            pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL]
            .depthStencilState.stencilPassOpBack = gfx::GFXStencilOp::KEEP;
        pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL]
            .depthStencilState.stencilRefFront =
            pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL]
            .depthStencilState.stencilRefBack = 0x1;
        pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL]
            .rasterizerState.cullMode = gfx::GFXCullMode::NONE;
        pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL]
            .dynamicStates.push_back(gfx::GFXDynamicState::VIEWPORT);
        _pipelineState[(uint8_t)PipelineType::FRONT_BACK_STENCIL] =
            _device->createPipelineState(
                pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL]);

        // do back stencil test
        pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].primitive =
            gfx::GFXPrimitiveMode::TRIANGLE_LIST;
        pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].shader = _shader;
        pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].inputState = {
            _inputAssembler->getAttributes() };
        pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].layout = _pipelineLayout[1];
        pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].renderPass =
            _fbo->getRenderPass();
        pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL]
            .depthStencilState.depthTest = true;
        pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL]
            .depthStencilState.depthWrite = false;
        pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL]
            .depthStencilState.stencilTestFront =
            pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL]
            .depthStencilState.stencilTestBack = true;
        pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL]
            .depthStencilState.stencilFuncFront = gfx::GFXComparisonFunc::ALWAYS;
        pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL]
            .depthStencilState.stencilFuncBack = gfx::GFXComparisonFunc::EQUAL;
        pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL]
            .depthStencilState.stencilReadMaskFront =
            pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL]
            .depthStencilState.stencilReadMaskBack = 0xFF;
        pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL]
            .depthStencilState.stencilWriteMaskFront =
            pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL]
            .depthStencilState.stencilWriteMaskBack = 0xFF;
        pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL]
            .depthStencilState.stencilFailOpFront =
            pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL]
            .depthStencilState.stencilFailOpBack = gfx::GFXStencilOp::KEEP;
        pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL]
            .depthStencilState.stencilZFailOpFront =
            pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL]
            .depthStencilState.stencilZFailOpBack = gfx::GFXStencilOp::KEEP;
        pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL]
            .depthStencilState.stencilPassOpFront =
            pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL]
            .depthStencilState.stencilPassOpBack = gfx::GFXStencilOp::KEEP;
        pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL]
            .depthStencilState.stencilRefFront =
            pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL]
            .depthStencilState.stencilRefBack = 0x1;
        pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].rasterizerState.cullMode =
            gfx::GFXCullMode::NONE;
        pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].dynamicStates.push_back(
            gfx::GFXDynamicState::VIEWPORT);
        _pipelineState[(uint8_t)PipelineType::BACK_STENCIL] =
            _device->createPipelineState(
                pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL]);

        // do front stencil test
        pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].primitive =
            gfx::GFXPrimitiveMode::TRIANGLE_LIST;
        pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].shader = _shader;
        pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].inputState = {
            _inputAssembler->getAttributes() };
        pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].layout =
            _pipelineLayout[0];
        pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].renderPass =
            _fbo->getRenderPass();
        pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL]
            .depthStencilState.depthTest = true;
        pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL]
            .depthStencilState.depthWrite = false;
        pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL]
            .depthStencilState.stencilTestFront =
            pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL]
            .depthStencilState.stencilTestBack = true;
        pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL]
            .depthStencilState.stencilFuncFront = gfx::GFXComparisonFunc::EQUAL;
        pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL]
            .depthStencilState.stencilFuncBack = gfx::GFXComparisonFunc::ALWAYS;
        pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL]
            .depthStencilState.stencilReadMaskFront =
            pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL]
            .depthStencilState.stencilReadMaskBack = 0xFF;
        pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL]
            .depthStencilState.stencilWriteMaskFront =
            pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL]
            .depthStencilState.stencilWriteMaskBack = 0xFF;
        pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL]
            .depthStencilState.stencilFailOpFront =
            pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL]
            .depthStencilState.stencilFailOpBack = gfx::GFXStencilOp::KEEP;
        pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL]
            .depthStencilState.stencilZFailOpFront =
            pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL]
            .depthStencilState.stencilZFailOpBack = gfx::GFXStencilOp::KEEP;
        pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL]
            .depthStencilState.stencilPassOpFront =
            pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL]
            .depthStencilState.stencilPassOpBack = gfx::GFXStencilOp::KEEP;
        pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL]
            .depthStencilState.stencilRefFront =
            pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL]
            .depthStencilState.stencilRefBack = 0x1;
        pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].rasterizerState.cullMode =
            gfx::GFXCullMode::NONE;
        pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].dynamicStates.push_back(
            gfx::GFXDynamicState::VIEWPORT);
        _pipelineState[(uint8_t)PipelineType::FRONT_STENCIL] =
            _device->createPipelineState(
                pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL]);
    }

    void StencilTest::tick(float dt) {
        _dt += dt;
        gfx::GFXRect render_area = { 0, 0, _device->getWidth(), _device->getHeight() };
        gfx::GFXColor clear_color = { 1.0f, 0, 0, 1.0f };

        _device->acquire();

        auto commandBuffer = _commandBuffers[0];
        commandBuffer->begin();
        commandBuffer->beginRenderPass(
            _fbo, render_area, gfx::GFXClearFlagBit::ALL,
            std::move(std::vector<gfx::GFXColor>({ clear_color })), 1.0f, 0);

        commandBuffer->bindInputAssembler(_inputAssembler);

        // draw label
        gfx::GFXViewport viewport;
        viewport.left = render_area.width / 6;
        viewport.top = uint(TestBaseI::getViewportTopBasedOnDevice(0.5f, 0.5f) *
            render_area.height);
        viewport.width = render_area.width / 3;
        viewport.height = render_area.height / 2;
        commandBuffer->setViewport(viewport);
        commandBuffer->bindPipelineState(
            _pipelineState[(uint8_t)PipelineType::STENCIL]);
        commandBuffer->bindBindingLayout(_bindingLayout[0]);
        commandBuffer->draw(_inputAssembler);

        // draw uv_image
        viewport.left = render_area.width / 2;
        commandBuffer->setViewport(viewport);
        commandBuffer->bindPipelineState(
            _pipelineState[(uint8_t)PipelineType::IMAGE]);
        commandBuffer->bindBindingLayout(_bindingLayout[1]);
        commandBuffer->draw(_inputAssembler);

        // do back and front stencil test
        viewport.left = 0;
        viewport.top = uint(TestBaseI::getViewportTopBasedOnDevice(0.0f, 0.5f) *
            render_area.height);
        commandBuffer->setViewport(viewport);
        commandBuffer->bindPipelineState(
            _pipelineState[(uint8_t)PipelineType::CAVAS]);
        commandBuffer->bindBindingLayout(_bindingLayout[0]);
        commandBuffer->draw(_inputAssembler);

        commandBuffer->bindPipelineState(
            _pipelineState[(uint8_t)PipelineType::FRONT_BACK_STENCIL]);
        commandBuffer->bindBindingLayout(_bindingLayout[1]);
        commandBuffer->draw(_inputAssembler);

        // do back stencil test
        viewport.left = render_area.width / 3;
        commandBuffer->setViewport(viewport);
        commandBuffer->bindPipelineState(
            _pipelineState[(uint8_t)PipelineType::CAVAS]);
        commandBuffer->bindBindingLayout(_bindingLayout[0]);
        commandBuffer->draw(_inputAssembler);

        commandBuffer->bindPipelineState(
            _pipelineState[(uint8_t)PipelineType::BACK_STENCIL]);
        commandBuffer->bindBindingLayout(_bindingLayout[1]);
        commandBuffer->draw(_inputAssembler);

        // do front stencil test
        viewport.left = render_area.width * 2 / 3;
        commandBuffer->setViewport(viewport);
        commandBuffer->bindPipelineState(
            _pipelineState[(uint8_t)PipelineType::CAVAS]);
        commandBuffer->bindBindingLayout(_bindingLayout[0]);
        commandBuffer->draw(_inputAssembler);

        commandBuffer->bindPipelineState(
            _pipelineState[(uint8_t)PipelineType::FRONT_STENCIL]);
        commandBuffer->bindBindingLayout(_bindingLayout[1]);
        commandBuffer->draw(_inputAssembler);

        commandBuffer->endRenderPass();
        commandBuffer->end();

        _device->getQueue()->submit(_commandBuffers);
        _device->present();
    }

} // namespace cc
