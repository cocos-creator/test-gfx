#include "BasicTextureTest.h"

#if (CC_PLATFORM == CC_PLATFORM_MAC_OSX)
#include "gfx-metal/GFXMTL.h"
#else
#include "gfx-gles2/GFXGLES2.h"
#include "gfx-gles3/GFXGLES3.h"
#endif

#include "cocos2d.h"

NS_CC_BEGIN

void BasicTexture::destroy()
{
    CC_SAFE_DESTROY(_shader);
    CC_SAFE_DESTROY(_vertexBuffer);
    CC_SAFE_DESTROY(_inputAssembler);
    CC_SAFE_DESTROY(_pipelineState);
    CC_SAFE_DESTROY(_bindingLayout);
    CC_SAFE_DESTROY(_uniformBuffer);
    CC_SAFE_DESTROY(_texture);
    CC_SAFE_DESTROY(_image);
    CC_SAFE_DESTROY(_sampler);
    CC_SAFE_DESTROY(_texView);
}

bool BasicTexture::initialize()
{
    createShader();
    createVertexBuffer();
    createInputAssembler();
    createTexture();
    createPipeline();
    return true;
}

void BasicTexture::createShader()
{
    GFXShaderStageList shaderStageList;
    GFXShaderStage vertexShaderStage;
    vertexShaderStage.type = GFXShaderType::VERTEX;
#if (CC_PLATFORM == CC_PLATFORM_MAC_OSX)
    vertexShaderStage.source = R"(#include <metal_stdlib>
    #include <simd/simd.h>
    
    using namespace metal;
    
    struct MVP_Matrix
    {
        float4x4 u_mvpMatrix;
    };
    
    struct main0_out
    {
        float2 texcoord [[user(locn0)]];
        float4 gl_Position [[position]];
    };
    
    struct main0_in
    {
        float2 a_position [[attribute(0)]];
    };
    
    vertex main0_out main0(main0_in in [[stage_in]], constant MVP_Matrix& _16 [[buffer(0)]])
    {
        main0_out out = {};
        out.gl_Position = _16.u_mvpMatrix * float4(in.a_position, 0.0, 1.0);
        out.texcoord = (in.a_position * 0.5) + float2(0.5);
        out.texcoord = float2(out.texcoord.x, 1.0 - out.texcoord.y);
        return out;
    })";
    
#else
    
#ifdef USE_GLES2
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
    vertexShaderStage.source = R"(#version 300 es
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
    
#endif // (CC_PLATFORM == CC_PLATFORM_MAC_OSX)
    shaderStageList.emplace_back(std::move(vertexShaderStage));

    GFXShaderStage fragmentShaderStage;
    fragmentShaderStage.type = GFXShaderType::FRAGMENT;
    
#if (CC_PLATFORM == CC_PLATFORM_MAC_OSX)
    fragmentShaderStage.source = R"(
    #include <metal_stdlib>
    #include <simd/simd.h>
    
    using namespace metal;
    
    struct main0_out
    {
        float4 o_color [[color(0)]];
    };
    
    struct main0_in
    {
        float2 texcoord [[user(locn0)]];
    };
    
    fragment main0_out main0(main0_in in [[stage_in]], texture2d<float> u_texture [[texture(0)]], sampler u_textureSmplr [[sampler(0)]])
    {
        main0_out out = {};
        out.o_color = u_texture.sample(u_textureSmplr, in.texcoord);
        return out;
    }
    )";
#else
    
#ifdef USE_GLES2
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
    fragmentShaderStage.source = R"(#version 300 es
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
    
#endif // (CC_PLATFORM == CC_PLATFORM_MAC_OSX)
    shaderStageList.emplace_back(std::move(fragmentShaderStage));

    GFXUniformList mvpMatrix = { { "u_mvpMatrix", GFXType::MAT4, 1} };
    GFXUniformBlockList uniformBlockList = { {0, "MVP_Matrix", mvpMatrix} };
    
    GFXUniformSamplerList sampler = { {1, "u_texture", GFXType::SAMPLER2D, 1} };

    GFXShaderInfo shaderInfo;
    shaderInfo.name = "Basic Texture";
    shaderInfo.stages = std::move(shaderStageList);
    shaderInfo.blocks = std::move(uniformBlockList);
    shaderInfo.samplers = std::move(sampler);
    _shader = _device->createShader(shaderInfo);
}

void BasicTexture::createVertexBuffer()
{
    float vertexData[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f,
        
         1.0f,  1.0f,
        -1.0f,  1.0f,
        -1.0f, -1.0f
    };
    
//    unsigned short indices[6] = {0, 1, 2, 1, 2, 3};

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
           sizeof(Mat4),
           sizeof(Mat4),
           GFXBufferFlagBit::NONE };
     _uniformBuffer = _device->createBuffer(uniformBufferInfo);
}

void BasicTexture::createInputAssembler()
{
    GFXAttribute position = {"a_position", GFXFormat::RG32F, false, 0, false};
    GFXInputAssemblerInfo inputAssemblerInfo;
    inputAssemblerInfo.attributes.emplace_back(std::move(position));
    inputAssemblerInfo.vertexBuffers.emplace_back(_vertexBuffer);
    _inputAssembler = _device->createInputAssembler(inputAssemblerInfo);
}

void BasicTexture::createPipeline()
{
    GFXBindingList bindingList = {
        {0, GFXBindingType::UNIFORM_BUFFER, "u_mvpMatrix"},
        {1, GFXBindingType::SAMPLER, "u_texture"}
    };
    GFXBindingLayoutInfo bindingLayoutInfo = { bindingList };
    _bindingLayout = _device->createBindingLayout(bindingLayoutInfo);
    
    Mat4 mvpMatrix;
    _uniformBuffer->update(&mvpMatrix, 0, sizeof(mvpMatrix));
    _bindingLayout->bindBuffer(0, _uniformBuffer);
    _bindingLayout->bindSampler(1, _sampler);
    _bindingLayout->bindTextureView(1, _texView);
    _bindingLayout->update();

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

void BasicTexture::createTexture()
{
    auto img = new cocos2d::Image();
    bool ret = img->initWithImageFile("uv_checker_01.jpg");
    CCASSERT(ret, "BasicTexture load image failed");
    
    Data data;
    data.copy(img->getData(), img->getDataLen());
    
    GFXTextureInfo textureInfo;
    textureInfo.usage = GFXTextureUsage::SAMPLED;
    textureInfo.format = GFXFormat::RGB8;
    textureInfo.width = img->getWidth();
    textureInfo.height = img->getHeight();
    _texture = _device->createTexture(textureInfo);
    
    GFXBufferTextureCopy textureRegion;
    textureRegion.buffTexHeight = img->getHeight();
    textureRegion.texExtent.width = img->getWidth();
    textureRegion.texExtent.height = img->getHeight();
    textureRegion.texExtent.depth = 1;
    
    GFXBufferTextureCopyList regions;
    regions.push_back(std::move(textureRegion));
    
    GFXDataArray imageBuffer = { { img->getData() } };
    _device->copyBuffersToTexture(imageBuffer, _texture, regions);
    
    //create sampler
    GFXSamplerInfo samplerInfo;
    _sampler = _device->createSampler(samplerInfo);
    
    GFXTextureViewInfo texViewInfo;
    texViewInfo.texture = _texture;
    texViewInfo.format = GFXFormat::RGB8;
    _texView = _device->createTextureView(texViewInfo);
}

void BasicTexture::tick(float dt) {

    GFXRect render_area = {0, 0, _device->getWidth(), _device->getHeight() };
    GFXColor clear_color = {0, 0, 0, 1.0f};

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
