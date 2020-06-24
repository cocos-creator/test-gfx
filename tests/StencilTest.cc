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
        gfx::ShaderStageList shaderStageList;
        gfx::ShaderStage vertexShaderStage;
        vertexShaderStage.type = gfx::ShaderType::VERTEX;

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

        shaderStageList.emplace_back(std::move(vertexShaderStage));

        gfx::ShaderStage fragmentShaderStage;
        fragmentShaderStage.type = gfx::ShaderType::FRAGMENT;

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

        shaderStageList.emplace_back(std::move(fragmentShaderStage));

        gfx::AttributeList attributeList = { { "a_position", gfx::Format::RG32F, false, 0, false, 0 } };
        gfx::UniformList mvpMatrix = { {"u_mvpMatrix", gfx::Type::MAT4, 1} };
        gfx::UniformBlockList uniformBlockList = { {gfx::ShaderType::VERTEX, 0, "MVP_Matrix", mvpMatrix} };
        gfx::UniformSamplerList sampler = { {gfx::ShaderType::FRAGMENT, 1, "u_texture", gfx::Type::SAMPLER2D, 1} };

        gfx::ShaderInfo shaderInfo;
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

        gfx::BufferInfo vertexBufferInfo = { gfx::BufferUsage::VERTEX,
                                          gfx::MemoryUsage::DEVICE, 2 * sizeof(float),
                                          sizeof(vertexData), gfx::BufferFlagBit::NONE };

        _vertexBuffer = _device->createBuffer(vertexBufferInfo);
        _vertexBuffer->update(vertexData, 0, sizeof(vertexData));

        gfx::BufferInfo uniformBufferInfo = {
            gfx::BufferUsage::UNIFORM, gfx::MemoryUsage::HOST | gfx::MemoryUsage::DEVICE,
            sizeof(Mat4), sizeof(Mat4), gfx::BufferFlagBit::NONE,
        };

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

        gfx::BufferTextureCopy labelTextureRegion;
        labelTextureRegion.buffTexHeight = stencilImage->getHeight();
        labelTextureRegion.texExtent.width = stencilImage->getWidth();
        labelTextureRegion.texExtent.height = stencilImage->getHeight();
        labelTextureRegion.texExtent.depth = 1;

        gfx::BufferTextureCopyList regions;
        regions.push_back(std::move(labelTextureRegion));

        gfx::TextureInfo labelTextureInfo;
        labelTextureInfo.usage = gfx::TextureUsage::SAMPLED | gfx::TextureUsage::TRANSFER_DST;
        labelTextureInfo.format = gfx::Format::RGBA8;
        labelTextureInfo.width = stencilImage->getWidth();
        labelTextureInfo.height = stencilImage->getHeight();
        _labelTexture = _device->createTexture(labelTextureInfo);

        gfx::DataArray stencilBuffer = { {stencilImageData} };
        _device->copyBuffersToTexture(stencilBuffer, _labelTexture, regions);

        // load uv_checker_02.jpg
        auto img = new cc::Image();
        img->autorelease();
        ret = img->initWithImageFile("uv_checker_02.jpg");
        assert(ret);
        auto imgData = TestBaseI::RGB2RGBA(img);

        gfx::TextureInfo textureInfo;
        textureInfo.usage = gfx::TextureUsage::SAMPLED | gfx::TextureUsage::TRANSFER_DST;
        textureInfo.format = gfx::Format::RGBA8;
        textureInfo.width = img->getWidth();
        textureInfo.height = img->getHeight();
        _uvCheckerTexture = _device->createTexture(textureInfo);

        gfx::BufferTextureCopy textureRegion;
        textureRegion.buffTexHeight = img->getHeight();
        textureRegion.texExtent.width = img->getWidth();
        textureRegion.texExtent.height = img->getHeight();
        textureRegion.texExtent.depth = 1;

        gfx::BufferTextureCopyList uvTextureRegions;
        uvTextureRegions.push_back(std::move(textureRegion));

        gfx::DataArray uvImageBuffer = { {imgData} };
        _device->copyBuffersToTexture(uvImageBuffer, _uvCheckerTexture, uvTextureRegions);

        // create sampler
        gfx::SamplerInfo samplerInfo;
        _sampler = _device->createSampler(samplerInfo);

        delete[] imgData;
        delete[] stencilImageData;
    }

    void StencilTest::createInputAssembler() {
        gfx::Attribute position = { "a_position", gfx::Format::RG32F, false, 0, false };
        gfx::InputAssemblerInfo inputAssemblerInfo;
        inputAssemblerInfo.attributes.emplace_back(std::move(position));
        inputAssemblerInfo.vertexBuffers.emplace_back(_vertexBuffer);
        _inputAssembler = _device->createInputAssembler(inputAssemblerInfo);
    }

    void StencilTest::createPipelineState() {
        gfx::BindingLayoutInfo bindingLayoutInfo = { _shader };

        gfx::Texture *texView[BINDING_COUNT] = { _labelTexture, _uvCheckerTexture };
        for (uint i = 0; i < BINDING_COUNT; i++) {
            _bindingLayout[i] = _device->createBindingLayout(bindingLayoutInfo);
            _bindingLayout[i]->bindBuffer(0, _uniformBuffer[i]);
            _bindingLayout[i]->bindSampler(1, _sampler);
            _bindingLayout[i]->bindTexture(1, texView[i]);
            _bindingLayout[i]->update();
        }

        // lable
        gfx::PipelineStateInfo pipelineInfo[PIPELIE_COUNT];
        pipelineInfo[(uint8_t)PipelineType::STENCIL].primitive = gfx::PrimitiveMode::TRIANGLE_LIST;
        pipelineInfo[(uint8_t)PipelineType::STENCIL].shader = _shader;
        pipelineInfo[(uint8_t)PipelineType::STENCIL].inputState = { _inputAssembler->getAttributes() };
        pipelineInfo[(uint8_t)PipelineType::STENCIL].renderPass = _fbo->getRenderPass();
        pipelineInfo[(uint8_t)PipelineType::STENCIL].depthStencilState.depthTest = false;
        pipelineInfo[(uint8_t)PipelineType::STENCIL].depthStencilState.stencilTestFront = false;
        pipelineInfo[(uint8_t)PipelineType::STENCIL].depthStencilState.stencilTestBack = false;
        pipelineInfo[(uint8_t)PipelineType::STENCIL].rasterizerState.cullMode = gfx::CullMode::NONE;
        pipelineInfo[(uint8_t)PipelineType::STENCIL].dynamicStates.push_back(gfx::DynamicState::VIEWPORT);
        _pipelineState[(uint8_t)PipelineType::STENCIL] = _device->createPipelineState(pipelineInfo[(uint8_t)PipelineType::STENCIL]);

        // uv_image
        pipelineInfo[(uint8_t)PipelineType::IMAGE].primitive = gfx::PrimitiveMode::TRIANGLE_LIST;
        pipelineInfo[(uint8_t)PipelineType::IMAGE].shader = _shader;
        pipelineInfo[(uint8_t)PipelineType::IMAGE].inputState = { _inputAssembler->getAttributes() };
        pipelineInfo[(uint8_t)PipelineType::IMAGE].renderPass = _fbo->getRenderPass();
        pipelineInfo[(uint8_t)PipelineType::IMAGE].depthStencilState.depthTest = false;
        pipelineInfo[(uint8_t)PipelineType::IMAGE].depthStencilState.stencilTestFront = false;
        pipelineInfo[(uint8_t)PipelineType::IMAGE].depthStencilState.stencilTestBack = false;
        pipelineInfo[(uint8_t)PipelineType::IMAGE].rasterizerState.cullMode = gfx::CullMode::NONE;
        pipelineInfo[(uint8_t)PipelineType::IMAGE].dynamicStates.push_back(gfx::DynamicState::VIEWPORT);
        _pipelineState[(uint8_t)PipelineType::IMAGE] = _device->createPipelineState(pipelineInfo[(uint8_t)PipelineType::IMAGE]);

        // do back and front stencil test
        pipelineInfo[(uint8_t)PipelineType::CAVAS].primitive = gfx::PrimitiveMode::TRIANGLE_LIST;
        pipelineInfo[(uint8_t)PipelineType::CAVAS].shader = _shader;
        pipelineInfo[(uint8_t)PipelineType::CAVAS].inputState = { _inputAssembler->getAttributes() };
        pipelineInfo[(uint8_t)PipelineType::CAVAS].renderPass = _fbo->getRenderPass();
        pipelineInfo[(uint8_t)PipelineType::CAVAS].depthStencilState.depthTest = true;
        pipelineInfo[(uint8_t)PipelineType::CAVAS].depthStencilState.depthWrite = false;
        pipelineInfo[(uint8_t)PipelineType::CAVAS].depthStencilState.stencilTestFront =
            pipelineInfo[(uint8_t)PipelineType::CAVAS].depthStencilState.stencilTestBack = true;
        pipelineInfo[(uint8_t)PipelineType::CAVAS].depthStencilState.stencilFuncFront =
            pipelineInfo[(uint8_t)PipelineType::CAVAS].depthStencilState.stencilFuncBack = gfx::ComparisonFunc::NEVER;
        pipelineInfo[(uint8_t)PipelineType::CAVAS].depthStencilState.stencilReadMaskFront =
            pipelineInfo[(uint8_t)PipelineType::CAVAS].depthStencilState.stencilReadMaskBack = 0xFF;
        pipelineInfo[(uint8_t)PipelineType::CAVAS].depthStencilState.stencilWriteMaskFront =
            pipelineInfo[(uint8_t)PipelineType::CAVAS].depthStencilState.stencilWriteMaskBack = 0xFF;
        pipelineInfo[(uint8_t)PipelineType::CAVAS].depthStencilState.stencilFailOpFront =
            pipelineInfo[(uint8_t)PipelineType::CAVAS].depthStencilState.stencilFailOpBack = gfx::StencilOp::REPLACE;
        pipelineInfo[(uint8_t)PipelineType::CAVAS].depthStencilState.stencilZFailOpFront =
            pipelineInfo[(uint8_t)PipelineType::CAVAS].depthStencilState.stencilZFailOpBack = gfx::StencilOp::KEEP;
        pipelineInfo[(uint8_t)PipelineType::CAVAS].depthStencilState.stencilPassOpFront =
            pipelineInfo[(uint8_t)PipelineType::CAVAS].depthStencilState.stencilPassOpBack = gfx::StencilOp::KEEP;
        pipelineInfo[(uint8_t)PipelineType::CAVAS].depthStencilState.stencilRefFront =
            pipelineInfo[(uint8_t)PipelineType::CAVAS].depthStencilState.stencilRefBack = 0x1;
        pipelineInfo[(uint8_t)PipelineType::CAVAS].rasterizerState.cullMode = gfx::CullMode::NONE;
        pipelineInfo[(uint8_t)PipelineType::CAVAS].dynamicStates.push_back(gfx::DynamicState::VIEWPORT);
        _pipelineState[(uint8_t)PipelineType::CAVAS] = _device->createPipelineState(pipelineInfo[(uint8_t)PipelineType::CAVAS]);

        pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].primitive = gfx::PrimitiveMode::TRIANGLE_LIST;
        pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].shader = _shader;
        pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].inputState = { _inputAssembler->getAttributes() };
        pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].renderPass = _fbo->getRenderPass();
        pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].depthStencilState.depthTest = true;
        pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].depthStencilState.depthWrite = false;
        pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].depthStencilState.stencilTestFront =
            pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].depthStencilState.stencilTestBack = true;
        pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].depthStencilState.stencilFuncFront =
            pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].depthStencilState.stencilFuncBack = gfx::ComparisonFunc::EQUAL;
        pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].depthStencilState.stencilReadMaskFront =
            pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].depthStencilState.stencilReadMaskBack = 0xFF;
        pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].depthStencilState.stencilWriteMaskFront =
            pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].depthStencilState.stencilWriteMaskBack = 0xFF;
        pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].depthStencilState.stencilFailOpFront =
            pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].depthStencilState.stencilFailOpBack = gfx::StencilOp::KEEP;
        pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].depthStencilState.stencilZFailOpFront =
            pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].depthStencilState.stencilZFailOpBack = gfx::StencilOp::KEEP;
        pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].depthStencilState.stencilPassOpFront =
            pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].depthStencilState.stencilPassOpBack = gfx::StencilOp::KEEP;
        pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].depthStencilState.stencilRefFront =
            pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].depthStencilState.stencilRefBack = 0x1;
        pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].rasterizerState.cullMode = gfx::CullMode::NONE;
        pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].dynamicStates.push_back(gfx::DynamicState::VIEWPORT);
        _pipelineState[(uint8_t)PipelineType::FRONT_BACK_STENCIL] = _device->createPipelineState(pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL]);

        // do back stencil test
        pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].primitive = gfx::PrimitiveMode::TRIANGLE_LIST;
        pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].shader = _shader;
        pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].inputState = { _inputAssembler->getAttributes() };
        pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].renderPass = _fbo->getRenderPass();
        pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].depthStencilState.depthTest = true;
        pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].depthStencilState.depthWrite = false;
        pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].depthStencilState.stencilTestFront =
            pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].depthStencilState.stencilTestBack = true;
        pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].depthStencilState.stencilFuncFront = gfx::ComparisonFunc::ALWAYS;
        pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].depthStencilState.stencilFuncBack = gfx::ComparisonFunc::EQUAL;
        pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].depthStencilState.stencilReadMaskFront =
            pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].depthStencilState.stencilReadMaskBack = 0xFF;
        pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].depthStencilState.stencilWriteMaskFront =
            pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].depthStencilState.stencilWriteMaskBack = 0xFF;
        pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].depthStencilState.stencilFailOpFront =
            pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].depthStencilState.stencilFailOpBack = gfx::StencilOp::KEEP;
        pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].depthStencilState.stencilZFailOpFront =
            pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].depthStencilState.stencilZFailOpBack = gfx::StencilOp::KEEP;
        pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].depthStencilState.stencilPassOpFront =
            pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].depthStencilState.stencilPassOpBack = gfx::StencilOp::KEEP;
        pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].depthStencilState.stencilRefFront =
            pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].depthStencilState.stencilRefBack = 0x1;
        pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].rasterizerState.cullMode = gfx::CullMode::NONE;
        pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].dynamicStates.push_back(gfx::DynamicState::VIEWPORT);
        _pipelineState[(uint8_t)PipelineType::BACK_STENCIL] = _device->createPipelineState(pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL]);

        // do front stencil test
        pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].primitive = gfx::PrimitiveMode::TRIANGLE_LIST;
        pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].shader = _shader;
        pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].inputState = { _inputAssembler->getAttributes() };
        pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].renderPass = _fbo->getRenderPass();
        pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].depthStencilState.depthTest = true;
        pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].depthStencilState.depthWrite = false;
        pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].depthStencilState.stencilTestFront =
            pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].depthStencilState.stencilTestBack = true;
        pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].depthStencilState.stencilFuncFront = gfx::ComparisonFunc::EQUAL;
        pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].depthStencilState.stencilFuncBack = gfx::ComparisonFunc::ALWAYS;
        pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].depthStencilState.stencilReadMaskFront =
            pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].depthStencilState.stencilReadMaskBack = 0xFF;
        pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].depthStencilState.stencilWriteMaskFront =
            pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].depthStencilState.stencilWriteMaskBack = 0xFF;
        pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].depthStencilState.stencilFailOpFront =
            pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].depthStencilState.stencilFailOpBack = gfx::StencilOp::KEEP;
        pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].depthStencilState.stencilZFailOpFront =
            pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].depthStencilState.stencilZFailOpBack = gfx::StencilOp::KEEP;
        pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].depthStencilState.stencilPassOpFront =
            pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].depthStencilState.stencilPassOpBack = gfx::StencilOp::KEEP;
        pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].depthStencilState.stencilRefFront =
            pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].depthStencilState.stencilRefBack = 0x1;
        pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].rasterizerState.cullMode = gfx::CullMode::NONE;
        pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].dynamicStates.push_back(gfx::DynamicState::VIEWPORT);
        _pipelineState[(uint8_t)PipelineType::FRONT_STENCIL] = _device->createPipelineState(pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL]);
    }

    void StencilTest::tick(float dt) {
        _dt += dt;
        gfx::Rect render_area = { 0, 0, _device->getWidth(), _device->getHeight() };
        gfx::Color clear_color = { 1.0f, 0, 0, 1.0f };

        _device->acquire();

        auto commandBuffer = _commandBuffers[0];
        commandBuffer->begin();
        commandBuffer->beginRenderPass(_fbo, render_area, gfx::ClearFlagBit::ALL,
            std::move(std::vector<gfx::Color>({ clear_color })), 1.0f, 0);

        commandBuffer->bindInputAssembler(_inputAssembler);

        // draw label
        gfx::Viewport viewport;
        viewport.left = render_area.width / 6;
        viewport.top = uint(TestBaseI::getViewportTopBasedOnDevice(0.5f, 0.5f) *            render_area.height);
        viewport.width = render_area.width / 3;
        viewport.height = render_area.height / 2;
        commandBuffer->setViewport(viewport);
        commandBuffer->bindPipelineState(_pipelineState[(uint8_t)PipelineType::STENCIL]);
        commandBuffer->bindBindingLayout(_bindingLayout[0]);
        commandBuffer->draw(_inputAssembler);

        // draw uv_image
        viewport.left = render_area.width / 2;
        commandBuffer->setViewport(viewport);
        commandBuffer->bindPipelineState(_pipelineState[(uint8_t)PipelineType::IMAGE]);
        commandBuffer->bindBindingLayout(_bindingLayout[1]);
        commandBuffer->draw(_inputAssembler);

        // do back and front stencil test
        viewport.left = 0;
        viewport.top = uint(TestBaseI::getViewportTopBasedOnDevice(0.0f, 0.5f) *            render_area.height);
        commandBuffer->setViewport(viewport);
        commandBuffer->bindPipelineState(_pipelineState[(uint8_t)PipelineType::CAVAS]);
        commandBuffer->bindBindingLayout(_bindingLayout[0]);
        commandBuffer->draw(_inputAssembler);

        commandBuffer->bindPipelineState(_pipelineState[(uint8_t)PipelineType::FRONT_BACK_STENCIL]);
        commandBuffer->bindBindingLayout(_bindingLayout[1]);
        commandBuffer->draw(_inputAssembler);

        // do back stencil test
        viewport.left = render_area.width / 3;
        commandBuffer->setViewport(viewport);
        commandBuffer->bindPipelineState(_pipelineState[(uint8_t)PipelineType::CAVAS]);
        commandBuffer->bindBindingLayout(_bindingLayout[0]);
        commandBuffer->draw(_inputAssembler);

        commandBuffer->bindPipelineState(_pipelineState[(uint8_t)PipelineType::BACK_STENCIL]);
        commandBuffer->bindBindingLayout(_bindingLayout[1]);
        commandBuffer->draw(_inputAssembler);

        // do front stencil test
        viewport.left = render_area.width * 2 / 3;
        commandBuffer->setViewport(viewport);
        commandBuffer->bindPipelineState(_pipelineState[(uint8_t)PipelineType::CAVAS]);
        commandBuffer->bindBindingLayout(_bindingLayout[0]);
        commandBuffer->draw(_inputAssembler);

        commandBuffer->bindPipelineState(_pipelineState[(uint8_t)PipelineType::FRONT_STENCIL]);
        commandBuffer->bindBindingLayout(_bindingLayout[1]);
        commandBuffer->draw(_inputAssembler);

        commandBuffer->endRenderPass();
        commandBuffer->end();

        _device->getQueue()->submit(_commandBuffers);
        _device->present();
    }

} // namespace cc
