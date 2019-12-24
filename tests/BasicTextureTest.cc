#include "BasicTextureTest.h"
#include "gfx-gles2/GFXGLES2.h"
#include "gfx-gles3/GFXGLES3.h"
#include "cocos2d.h"

NS_CC_BEGIN

void BasicTexture::Destroy()
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
#endif
    shaderStageList.emplace_back(std::move(vertexShaderStage));

    GFXShaderStage fragmentShaderStage;
    fragmentShaderStage.type = GFXShaderType::FRAGMENT;
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
#endif
    shaderStageList.emplace_back(std::move(fragmentShaderStage));

    GFXUniformList mvpMatrix = { { "u_mvpMatrix", GFXType::MAT4, 1} };
    GFXUniformBlockList uniformBlockList = { {0, "MVP_Matrix", mvpMatrix} };
    
    GFXUniformSamplerList sampler = { {1, "u_texture", GFXType::SAMPLER2D, 1} };

    GFXShaderInfo shaderInfo;
    shaderInfo.name = "Basic Texture";
    shaderInfo.stages = std::move(shaderStageList);
    shaderInfo.blocks = std::move(uniformBlockList);
    shaderInfo.samplers = std::move(sampler);
    _shader = _device->CreateGFXShader(shaderInfo);
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

    _vertexBuffer = _device->CreateGFXBuffer(vertexBufferInfo);
    _vertexBuffer->Update(vertexData, 0, sizeof(vertexData));

    GFXBufferInfo uniformBufferInfo = {
           GFXBufferUsage::UNIFORM,
           GFXMemoryUsage::HOST,
           sizeof(Mat4),
           sizeof(Mat4),
           GFXBufferFlagBit::NONE };
     _uniformBuffer = _device->CreateGFXBuffer(uniformBufferInfo);
}

void BasicTexture::createInputAssembler()
{
    GFXAttribute position = {"a_position", GFXFormat::RG32F, false, 0, false};
    GFXInputAssemblerInfo inputAssemblerInfo;
    inputAssemblerInfo.attributes.emplace_back(std::move(position));
    inputAssemblerInfo.vertex_buffers.emplace_back(_vertexBuffer);
    _inputAssembler = _device->CreateGFXInputAssembler(inputAssemblerInfo);
}

void BasicTexture::createPipeline()
{
    GFXBindingList bindingList = {
        {0, GFXBindingType::UNIFORM_BUFFER, "u_mvpMatrix"},
        {1, GFXBindingType::SAMPLER, "u_texture"}
    };
    GFXBindingLayoutInfo bindingLayoutInfo = { bindingList };
    _bindingLayout = _device->CreateGFXBindingLayout(bindingLayoutInfo);
    
    Mat4 mvpMatrix;
    _uniformBuffer->Update(&mvpMatrix, 0, sizeof(mvpMatrix));
    _bindingLayout->BindBuffer(0, _uniformBuffer);
    _bindingLayout->BindSampler(1, _sampler);
    _bindingLayout->BindTextureView(1, _texView);
    _bindingLayout->Update();

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

void BasicTexture::createTexture()
{
    auto img = new cocos2d::Image();
    bool ret = img->initWithImageFile("uv_checker_01.jpg");
    CCASSERT(ret, "BasicTexture load image failed");
    
    Data data;
    data.copy(img->getData(), img->getDataLen());
    
    GFXBufferInfo vertexBufferInfo = {
        GFXBufferUsage::TRANSFER_SRC,
        GFXMemoryUsage::HOST,
        1,
        static_cast<uint>(img->getDataLen()),
        GFXBufferFlagBit::NONE };
    
    _image = _device->CreateGFXBuffer(vertexBufferInfo);
    _image->Update(data.getBytes(), 0, static_cast<uint>(data.getSize()));
    
    GFXTextureInfo textureInfo;
    textureInfo.usage = GFXTextureUsage::SAMPLED;
    textureInfo.format = GFXFormat::RGB8;
    textureInfo.width = img->getWidth();
    textureInfo.height = img->getHeight();
    
    _texture = _device->CreateGFXTexture(textureInfo);
    
    GFXBufferTextureCopy textureRegion;
    textureRegion.buff_tex_height = img->getHeight();
    textureRegion.tex_extent.width = img->getWidth();
    textureRegion.tex_extent.height = img->getHeight();
    textureRegion.tex_extent.depth = 1;
    
    GFXBufferTextureCopyList regions;
    regions.push_back(std::move(textureRegion));
    
    _device->CopyBuffersToTexture(_image, _texture, regions);
    
    //create sampler
    GFXSamplerInfo samplerInfo;
    _sampler = _device->CreateGFXSampler(samplerInfo);
    
    GFXTextureViewInfo texViewInfo;
    texViewInfo.texture = _texture;
    texViewInfo.format = GFXFormat::RGB8;
    _texView = _device->CreateGFXTextureView(texViewInfo);
}

void BasicTexture::tick(float dt) {

    GFXRect render_area = {0, 0, _device->width(), _device->height() };
    GFXColor clear_color = {0, 0, 0, 1.0f};

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
