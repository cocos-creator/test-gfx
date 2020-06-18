#include "BasicTextureTest.h"

namespace cc {

    void BasicTexture::destroy() {
        CC_SAFE_DESTROY(_shader);
        CC_SAFE_DESTROY(_vertexBuffer);
        CC_SAFE_DESTROY(_inputAssembler);
        CC_SAFE_DESTROY(_bindingLayout);
        CC_SAFE_DESTROY(_pipelineLayout);
        CC_SAFE_DESTROY(_pipelineState);
        CC_SAFE_DESTROY(_uniformBuffer);
        CC_SAFE_DESTROY(_texture);
        CC_SAFE_DESTROY(_image);
        CC_SAFE_DESTROY(_sampler);
    }

    bool BasicTexture::initialize() {
        createShader();
        createVertexBuffer();
        createInputAssembler();
        createTexture();
        createPipeline();
        return true;
    }

    void BasicTexture::createShader() {
        gfx::GFXShaderStageList shaderStageList;
        gfx::GFXShaderStage vertexShaderStage;
        vertexShaderStage.type = gfx::GFXShaderType::VERTEX;

#if defined(USE_VULKAN) || defined(USE_METAL)
        vertexShaderStage.source = R"(
    layout(location = 0) in vec2 a_position;
    layout(location = 0) out vec2 texcoord;
    layout(binding = 0) uniform MVP_Matrix
    {
        mat4 u_mvpMatrix;
    };
   
    void main()
    {
        gl_Position = u_mvpMatrix * vec4(a_position, 0, 1);
        texcoord = a_position * 0.5 + 0.5;
        texcoord = vec2(texcoord.x, 1.0 - texcoord.y);
    }
    )";
#elif defined(USE_GLES2)
        vertexShaderStage.source = R"(
    attribute vec2 a_position;
    uniform mat4 u_mvpMatrix;
    varying vec2 v_texcoord;
    void main ()
    {
        gl_Position = u_mvpMatrix * vec4(a_position, 0, 1);
        v_texcoord = a_position * 0.5 + 0.5;
        v_texcoord = vec2(v_texcoord.x, 1.0 - v_texcoord.y);
    }
    )";
#else
        vertexShaderStage.source = R"(
    in vec2 a_position;
    out vec2 texcoord;
    layout(std140) uniform MVP_Matrix
    {
        mat4 u_mvpMatrix;
    };
   
    void main()
    {
        gl_Position = u_mvpMatrix * vec4(a_position, 0, 1);
        texcoord = a_position * 0.5 + 0.5;
        texcoord = vec2(texcoord.x, 1.0 - texcoord.y);
    }
    )";
#endif // USE_GLES2

        shaderStageList.emplace_back(std::move(vertexShaderStage));

        gfx::GFXShaderStage fragmentShaderStage;
        fragmentShaderStage.type = gfx::GFXShaderType::FRAGMENT;

#if defined(USE_VULKAN) || defined(USE_METAL)
        fragmentShaderStage.source = R"(
    layout(location = 0) in vec2 texcoord;
    layout(binding = 1) uniform sampler2D u_texture;
    layout(location = 0) out vec4 o_color;
    void main () {
        o_color = texture(u_texture, texcoord);
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
    in vec2 texcoord;
    uniform sampler2D u_texture;
    out vec4 o_color;
    void main () {
        o_color = texture(u_texture, texcoord);
    }
    )";
#endif // USE_GLES2

        shaderStageList.emplace_back(std::move(fragmentShaderStage));

        gfx::GFXAttributeList attributeList = { { "a_position", gfx::GFXFormat::RG32F, false, 0, false, 0 } };
        gfx::GFXUniformList mvpMatrix = { { "u_mvpMatrix", gfx::GFXType::MAT4, 1} };
        gfx::GFXUniformBlockList uniformBlockList = { { gfx::GFXShaderType::VERTEX, 0, "MVP_Matrix", mvpMatrix} };
        gfx::GFXUniformSamplerList sampler = { { gfx::GFXShaderType::FRAGMENT, 1, "u_texture", gfx::GFXType::SAMPLER2D, 1} };

        gfx::GFXShaderInfo shaderInfo;
        shaderInfo.name = "Basic Texture";
        shaderInfo.stages = std::move(shaderStageList);
        shaderInfo.attributes = std::move(attributeList);
        shaderInfo.blocks = std::move(uniformBlockList);
        shaderInfo.samplers = std::move(sampler);
        _shader = _device->createShader(shaderInfo);
    }

    void BasicTexture::createVertexBuffer() {
        float vertexData[] = {
            -1.0f, -1.0f,
             1.0f, -1.0f,
             1.0f,  1.0f,

             1.0f,  1.0f,
            -1.0f,  1.0f,
            -1.0f, -1.0f
        };

        //    unsigned short indices[6] = {0, 1, 2, 1, 2, 3};

        gfx::GFXBufferInfo vertexBufferInfo = {
              gfx::GFXBufferUsage::VERTEX,
              gfx::GFXMemoryUsage::DEVICE,
              2 * sizeof(float),
              sizeof(vertexData),
              gfx::GFXBufferFlagBit::NONE };

        _vertexBuffer = _device->createBuffer(vertexBufferInfo);
        _vertexBuffer->update(vertexData, 0, sizeof(vertexData));

        gfx::GFXBufferInfo uniformBufferInfo = {
               gfx::GFXBufferUsage::UNIFORM,
               gfx::GFXMemoryUsage::DEVICE,
               sizeof(Mat4),
               sizeof(Mat4),
               gfx::GFXBufferFlagBit::NONE };
        _uniformBuffer = _device->createBuffer(uniformBufferInfo);
    }

    void BasicTexture::createInputAssembler() {
        gfx::GFXAttribute position = { "a_position", gfx::GFXFormat::RG32F, false, 0, false };
        gfx::GFXInputAssemblerInfo inputAssemblerInfo;
        inputAssemblerInfo.attributes.emplace_back(std::move(position));
        inputAssemblerInfo.vertexBuffers.emplace_back(_vertexBuffer);
        _inputAssembler = _device->createInputAssembler(inputAssemblerInfo);
    }

    void BasicTexture::createPipeline() {
        gfx::GFXBindingList bindingList = {
            {gfx::GFXShaderType::VERTEX, 0, gfx::GFXBindingType::UNIFORM_BUFFER, "u_mvpMatrix", 1},
            {gfx::GFXShaderType::FRAGMENT, 1, gfx::GFXBindingType::SAMPLER, "u_texture", 1}
        };
        gfx::GFXBindingLayoutInfo bindingLayoutInfo = { bindingList };
        _bindingLayout = _device->createBindingLayout(bindingLayoutInfo);

        Mat4 mvpMatrix;
        TestBaseI::modifyProjectionBasedOnDevice(mvpMatrix);
        _uniformBuffer->update(&mvpMatrix, 0, sizeof(mvpMatrix));
        _bindingLayout->bindBuffer(0, _uniformBuffer);
        _bindingLayout->bindSampler(1, _sampler);
        _bindingLayout->bindTexture(1, _texture);
        _bindingLayout->update();

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

    void BasicTexture::createTexture() {
        auto img = new cc::Image();
        img->autorelease();
        bool valid = img->initWithImageFile("uv_checker_01.jpg");
        CCASSERT(valid, "BasicTexture load image failed");

        auto data = TestBaseI::RGB2RGBA(img);

        gfx::GFXTextureInfo textureInfo;
        textureInfo.usage = gfx::GFXTextureUsage::SAMPLED;
        textureInfo.format = gfx::GFXFormat::RGBA8;
        textureInfo.width = img->getWidth();
        textureInfo.height = img->getHeight();
        _texture = _device->createTexture(textureInfo);

        gfx::GFXBufferTextureCopy textureRegion;
        textureRegion.buffTexHeight = img->getHeight();
        textureRegion.texExtent.width = img->getWidth();
        textureRegion.texExtent.height = img->getHeight();
        textureRegion.texExtent.depth = 1;

        gfx::GFXBufferTextureCopyList regions;
        regions.push_back(std::move(textureRegion));

        gfx::GFXDataArray imageBuffer = { { data } };
        _device->copyBuffersToTexture(imageBuffer, _texture, regions);

        //create sampler
        gfx::GFXSamplerInfo samplerInfo;
        _sampler = _device->createSampler(samplerInfo);

        delete[] data;
    }

    void BasicTexture::tick(float dt) {

        gfx::GFXRect render_area = { 0, 0, _device->getWidth(), _device->getHeight() };
        gfx::GFXColor clear_color = { 0, 0, 0, 1.0f };

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
