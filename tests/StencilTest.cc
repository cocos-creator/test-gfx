#include "StencilTest.h"
#include "gfx-gles2/GFXGLES2.h"
#include "gfx-gles3/GFXGLES3.h"
#include "cocos2d.h"

NS_CC_BEGIN

namespace {
    enum class PipelineType : uint8_t
    {
        STENCIL,
        IMAGE,
        CAVAS,
        FRONT_STENCIL,
        BACK_STENCIL,
        FRONT_BACK_STENCIL,
        COUNT
    };
}

void StencilTest::Destroy()
{
    CC_SAFE_DESTROY(_shader);
    CC_SAFE_DESTROY(_labelTexture);
    CC_SAFE_DESTROY(_uvCheckerTexture);
    CC_SAFE_DESTROY(_labelTexView);
    CC_SAFE_DESTROY(_uvCheckerTexView);
    CC_SAFE_DESTROY(_vertexBuffer);
    CC_SAFE_DESTROY(_inputAssembler);
    CC_SAFE_DESTROY(_sampler);
    for(uint i = 0; i < BINDING_COUNT; i++)
    {
        CC_SAFE_DESTROY(_uniformBuffer[i]);
        CC_SAFE_DESTROY(_bindingLayout[i]);
    }
    for(uint i = 0; i < PIPELIE_COUNT; i++)
    {
        CC_SAFE_DESTROY(_pipelineState[i]);
    }
}

bool StencilTest::initialize()
{
    createShader();
    createBuffers();
    createTextures();
    createInputAssembler();
    createPipelineState();
    return true;
}

void StencilTest::createShader()
{
    GFXShaderStageList shaderStageList;
    GFXShaderStage vertexShaderStage;
    vertexShaderStage.type = GFXShaderType::VERTEX;
#ifdef USE_GLES2
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
    vertexShaderStage.source = R"(#version 300 es
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
    uniform sampler2D u_texture;
    in vec2 v_texcoord;
    out vec4 o_color;
    void main () {
        o_color = texture(u_texture, v_texcoord);
    }
    )";
#endif
    shaderStageList.emplace_back(std::move(fragmentShaderStage));
    
    GFXUniformList mvpMatrix = { { "u_mvpMatrix", GFXType::MAT4, 1} };
    GFXUniformBlockList uniformBlockList = { {0, "MVP_Matrix", mvpMatrix} };
    GFXUniformSamplerList sampler = { {1, "u_texture", GFXType::SAMPLER2D, 1} };
    
    GFXShaderInfo shaderInfo;
    shaderInfo.name = "Stencil Test";
    shaderInfo.stages = std::move(shaderStageList);
    shaderInfo.blocks = std::move(uniformBlockList);
    shaderInfo.samplers = std::move(sampler);
    _shader = _device->CreateGFXShader(shaderInfo);
}

void StencilTest::createBuffers()
{
    float vertexData[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f,
        
         1.0f,  1.0f,
        -1.0f, -1.0f,
        -1.0f,  1.0f
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
        sizeof(Mat4),
        sizeof(Mat4),
        GFXBufferFlagBit::NONE };
    
    Mat4 transform[BINDING_COUNT];
    transform[0].scale(0.5f);
    transform[0].rotateZ(math::PI / 4);
    transform[1].scale(0.5f, 0.5f, 0.25f);
    
    for (uint i = 0; i < BINDING_COUNT; i++)
    {
        _uniformBuffer[i] = _device->CreateGFXBuffer(uniformBufferInfo);
        _uniformBuffer[i]->Update(&transform[i], 0, sizeof(transform[i]));
    }
}

void StencilTest::createTextures()
{
    //load stencil image
    auto stencilImage = new cocos2d::Image();
    stencilImage->autorelease();
    bool ret = stencilImage->initWithImageFile("stencil.jpg");
    assert(ret);
    
    GFXBufferTextureCopy labelTextureRegion;
    labelTextureRegion.buff_tex_height = stencilImage->getHeight();
    labelTextureRegion.tex_extent.width = stencilImage->getWidth();
    labelTextureRegion.tex_extent.height = stencilImage->getHeight();
    labelTextureRegion.tex_extent.depth = 1;
    
    GFXBufferTextureCopyList regions;
    regions.push_back(std::move(labelTextureRegion));
    
    GFXBufferInfo imageBufferInfo = {
        GFXBufferUsage::TRANSFER_SRC,
        GFXMemoryUsage::HOST,
        1,
        static_cast<uint>(stencilImage->getDataLen()),
        GFXBufferFlagBit::NONE };
    
    auto stencilImageBuffer = _device->CreateGFXBuffer(imageBufferInfo);
    stencilImageBuffer->Update(stencilImage->getData(), 0, imageBufferInfo.size);
    
    GFXTextureInfo labelTextureInfo;
    labelTextureInfo.usage = GFXTextureUsage::SAMPLED;
    labelTextureInfo.format = GFXFormat::RGB8;
    labelTextureInfo.width = stencilImage->getWidth();
    labelTextureInfo.height = stencilImage->getHeight();
    
    _labelTexture = _device->CreateGFXTexture(labelTextureInfo);
    _device->CopyBuffersToTexture(stencilImageBuffer, _labelTexture, regions);
    
    GFXTextureViewInfo texViewInfo;
    texViewInfo.texture = _labelTexture;
    texViewInfo.format = GFXFormat::RGB8;
    _labelTexView = _device->CreateGFXTextureView(texViewInfo);
    
    //load uv_checker_02.jpg
    auto img = new cocos2d::Image();
    img->autorelease();
    ret = img->initWithImageFile("uv_checker_02.jpg");
    assert(ret);
    
    imageBufferInfo.size = static_cast<uint>(img->getDataLen());
    
    auto image = _device->CreateGFXBuffer(imageBufferInfo);
    image->Update(img->getData(), 0, imageBufferInfo.size);
    
    GFXTextureInfo textureInfo;
    textureInfo.usage = GFXTextureUsage::SAMPLED;
    textureInfo.format = GFXFormat::RGB8;
    textureInfo.width = img->getWidth();
    textureInfo.height = img->getHeight();
    
    _uvCheckerTexture = _device->CreateGFXTexture(textureInfo);
    
    GFXBufferTextureCopy textureRegion;
    textureRegion.buff_tex_height = img->getHeight();
    textureRegion.tex_extent.width = img->getWidth();
    textureRegion.tex_extent.height = img->getHeight();
    textureRegion.tex_extent.depth = 1;
    
    GFXBufferTextureCopyList uvTextureRegions;
    uvTextureRegions.push_back(std::move(textureRegion));
    
    _device->CopyBuffersToTexture(image, _uvCheckerTexture, uvTextureRegions);
    
    GFXTextureViewInfo uvTexViewInfo;
    uvTexViewInfo.texture = _uvCheckerTexture;
    uvTexViewInfo.format = GFXFormat::RGB8;
    _uvCheckerTexView = _device->CreateGFXTextureView(uvTexViewInfo);
    
    //create sampler
    GFXSamplerInfo samplerInfo;
    _sampler = _device->CreateGFXSampler(samplerInfo);
    
    CC_SAFE_DESTROY(stencilImageBuffer);
    CC_SAFE_DESTROY(image);
}

void StencilTest::createInputAssembler()
{
    GFXAttribute position = {"a_position", GFXFormat::RG32F, false, 0, false};
    GFXInputAssemblerInfo inputAssemblerInfo;
    inputAssemblerInfo.attributes.emplace_back(std::move(position));
    inputAssemblerInfo.vertex_buffers.emplace_back(_vertexBuffer);
    _inputAssembler = _device->CreateGFXInputAssembler(inputAssemblerInfo);
}

void StencilTest::createPipelineState()
{
    GFXBindingList bindingList = {
        {0, GFXBindingType::UNIFORM_BUFFER, "u_mvpMatrix"},
        {1, GFXBindingType::SAMPLER, "u_texture"}
    };
    GFXBindingLayoutInfo bindingLayoutInfo = { bindingList };
   
    GFXTextureView* texView[BINDING_COUNT] = { _labelTexView, _uvCheckerTexView };
    GFXPipelineLayout* pipelineLayout[BINDING_COUNT] = { nullptr };
    for(uint i = 0; i < BINDING_COUNT; i++)
    {
        _bindingLayout[i] = _device->CreateGFXBindingLayout(bindingLayoutInfo);
        _bindingLayout[i]->BindBuffer(0, _uniformBuffer[i]);
        _bindingLayout[i]->BindSampler(1, _sampler);
        _bindingLayout[i]->BindTextureView(1, texView[i]);
        _bindingLayout[i]->Update();
        
        GFXPipelineLayoutInfo pipelineLayoutInfo;
        pipelineLayoutInfo.layouts = { _bindingLayout[i] };
        pipelineLayout[i] = _device->CreateGFXPipelieLayout(pipelineLayoutInfo);
    }
    
    //lable
    GFXPipelineStateInfo pipelineInfo[PIPELIE_COUNT];
    pipelineInfo[(uint8_t)PipelineType::STENCIL].primitive = GFXPrimitiveMode::TRIANGLE_LIST;
    pipelineInfo[(uint8_t)PipelineType::STENCIL].shader = _shader;
    pipelineInfo[(uint8_t)PipelineType::STENCIL].is = { _inputAssembler->attributes() };
    pipelineInfo[(uint8_t)PipelineType::STENCIL].layout = pipelineLayout[0];
    pipelineInfo[(uint8_t)PipelineType::STENCIL].render_pass = _device->window()->render_pass();
    pipelineInfo[(uint8_t)PipelineType::STENCIL].dss.depth_test = false;
    pipelineInfo[(uint8_t)PipelineType::STENCIL].dss.stencil_test_front = false;
    pipelineInfo[(uint8_t)PipelineType::STENCIL].dss.stencil_test_back = false;
    pipelineInfo[(uint8_t)PipelineType::STENCIL].rs.cull_mode = GFXCullMode::NONE;
    _pipelineState[(uint8_t)PipelineType::STENCIL] = _device->CreateGFXPipelineState(pipelineInfo[(uint8_t)PipelineType::STENCIL]);

    //uv_image
    pipelineInfo[(uint8_t)PipelineType::IMAGE].primitive = GFXPrimitiveMode::TRIANGLE_LIST;
    pipelineInfo[(uint8_t)PipelineType::IMAGE].shader = _shader;
    pipelineInfo[(uint8_t)PipelineType::IMAGE].is = { _inputAssembler->attributes() };
    pipelineInfo[(uint8_t)PipelineType::IMAGE].layout = pipelineLayout[1];
    pipelineInfo[(uint8_t)PipelineType::IMAGE].render_pass = _device->window()->render_pass();
    pipelineInfo[(uint8_t)PipelineType::IMAGE].dss.depth_test = false;
    pipelineInfo[(uint8_t)PipelineType::IMAGE].dss.stencil_test_front = false;
    pipelineInfo[(uint8_t)PipelineType::IMAGE].dss.stencil_test_back = false;
    pipelineInfo[(uint8_t)PipelineType::IMAGE].rs.cull_mode = GFXCullMode::NONE;
    _pipelineState[(uint8_t)PipelineType::IMAGE] = _device->CreateGFXPipelineState(pipelineInfo[(uint8_t)PipelineType::IMAGE]);
    
    //do back and front stencil test
    pipelineInfo[(uint8_t)PipelineType::CAVAS].primitive = GFXPrimitiveMode::TRIANGLE_LIST;
    pipelineInfo[(uint8_t)PipelineType::CAVAS].shader = _shader;
    pipelineInfo[(uint8_t)PipelineType::CAVAS].is = { _inputAssembler->attributes() };
    pipelineInfo[(uint8_t)PipelineType::CAVAS].layout = pipelineLayout[0];
    pipelineInfo[(uint8_t)PipelineType::CAVAS].render_pass = _device->window()->render_pass();
    pipelineInfo[(uint8_t)PipelineType::CAVAS].dss.depth_test = true;
    pipelineInfo[(uint8_t)PipelineType::CAVAS].dss.depth_write = false;
    pipelineInfo[(uint8_t)PipelineType::CAVAS].dss.stencil_test_front         = pipelineInfo[(uint8_t)PipelineType::CAVAS].dss.stencil_test_back        = true;
    pipelineInfo[(uint8_t)PipelineType::CAVAS].dss.stencil_func_front         = pipelineInfo[(uint8_t)PipelineType::CAVAS].dss.stencil_func_back        = GFXComparisonFunc::NEVER;
    pipelineInfo[(uint8_t)PipelineType::CAVAS].dss.stencil_read_mask_front    = pipelineInfo[(uint8_t)PipelineType::CAVAS].dss.stencil_read_mask_back   = 0xFF;
    pipelineInfo[(uint8_t)PipelineType::CAVAS].dss.stencil_write_mask_front   = pipelineInfo[(uint8_t)PipelineType::CAVAS].dss.stencil_write_mask_back  = 0xFF;
    pipelineInfo[(uint8_t)PipelineType::CAVAS].dss.stencil_fail_op_front      = pipelineInfo[(uint8_t)PipelineType::CAVAS].dss.stencil_fail_op_back     = GFXStencilOp::REPLACE;
    pipelineInfo[(uint8_t)PipelineType::CAVAS].dss.stencil_z_fail_op_front    = pipelineInfo[(uint8_t)PipelineType::CAVAS].dss.stencil_z_fail_op_back   = GFXStencilOp::KEEP;
    pipelineInfo[(uint8_t)PipelineType::CAVAS].dss.stencil_pass_op_front      = pipelineInfo[(uint8_t)PipelineType::CAVAS].dss.stencil_pass_op_back     = GFXStencilOp::KEEP;
    pipelineInfo[(uint8_t)PipelineType::CAVAS].dss.stencil_ref_front          = pipelineInfo[(uint8_t)PipelineType::CAVAS].dss.stencil_ref_back         = 0x1;
    pipelineInfo[(uint8_t)PipelineType::CAVAS].rs.cull_mode = GFXCullMode::NONE;
    _pipelineState[(uint8_t)PipelineType::CAVAS] = _device->CreateGFXPipelineState(pipelineInfo[(uint8_t)PipelineType::CAVAS]);
    
    pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].primitive = GFXPrimitiveMode::TRIANGLE_LIST;
    pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].shader = _shader;
    pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].is = { _inputAssembler->attributes() };
    pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].layout = pipelineLayout[1];
    pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].render_pass = _device->window()->render_pass();
    pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].dss.depth_test = true;
    pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].dss.depth_write = false;
    pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].dss.stencil_test_front         = pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].dss.stencil_test_back        = true;
    pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].dss.stencil_func_front         = pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].dss.stencil_func_back        = GFXComparisonFunc::EQUAL;
    pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].dss.stencil_read_mask_front    = pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].dss.stencil_read_mask_back   = 0xFF;
    pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].dss.stencil_write_mask_front   = pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].dss.stencil_write_mask_back  = 0xFF;
    pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].dss.stencil_fail_op_front      = pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].dss.stencil_fail_op_back     = GFXStencilOp::KEEP;
    pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].dss.stencil_z_fail_op_front    = pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].dss.stencil_z_fail_op_back   = GFXStencilOp::KEEP;
    pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].dss.stencil_pass_op_front      = pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].dss.stencil_pass_op_back     = GFXStencilOp::KEEP;
    pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].dss.stencil_ref_front          = pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].dss.stencil_ref_back         = 0x1;
    pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL].rs.cull_mode = GFXCullMode::NONE;
    _pipelineState[(uint8_t)PipelineType::FRONT_BACK_STENCIL] = _device->CreateGFXPipelineState(pipelineInfo[(uint8_t)PipelineType::FRONT_BACK_STENCIL]);

    //do back stencil test
    pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].primitive = GFXPrimitiveMode::TRIANGLE_LIST;
    pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].shader = _shader;
    pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].is = { _inputAssembler->attributes() };
    pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].layout = pipelineLayout[1];
    pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].render_pass = _device->window()->render_pass();
    pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].dss.depth_test = true;
    pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].dss.depth_write = false;
    pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].dss.stencil_test_front         = pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].dss.stencil_test_back        = true;
    pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].dss.stencil_func_front         = GFXComparisonFunc::ALWAYS;
    pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].dss.stencil_func_back          = GFXComparisonFunc::EQUAL;
    pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].dss.stencil_read_mask_front    = pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].dss.stencil_read_mask_back   = 0xFF;
    pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].dss.stencil_write_mask_front   = pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].dss.stencil_write_mask_back  = 0xFF;
    pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].dss.stencil_fail_op_front      = pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].dss.stencil_fail_op_back     = GFXStencilOp::KEEP;
    pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].dss.stencil_z_fail_op_front    = pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].dss.stencil_z_fail_op_back   = GFXStencilOp::KEEP;
    pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].dss.stencil_pass_op_front      = pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].dss.stencil_pass_op_back     = GFXStencilOp::KEEP;
    pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].dss.stencil_ref_front          = pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].dss.stencil_ref_back         = 0x1;
    pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL].rs.cull_mode = GFXCullMode::NONE;
    _pipelineState[(uint8_t)PipelineType::BACK_STENCIL] = _device->CreateGFXPipelineState(pipelineInfo[(uint8_t)PipelineType::BACK_STENCIL]);
    
    //do front stencil test
    pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].primitive = GFXPrimitiveMode::TRIANGLE_LIST;
    pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].shader = _shader;
    pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].is = { _inputAssembler->attributes() };
    pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].layout = pipelineLayout[0];
    pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].render_pass = _device->window()->render_pass();
    pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].dss.depth_test = true;
    pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].dss.depth_write = false;
    pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].dss.stencil_test_front         = pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].dss.stencil_test_back        = true;
    pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].dss.stencil_func_front         = GFXComparisonFunc::EQUAL;
    pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].dss.stencil_func_back          = GFXComparisonFunc::ALWAYS;
    pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].dss.stencil_read_mask_front    = pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].dss.stencil_read_mask_back   = 0xFF;
    pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].dss.stencil_write_mask_front   = pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].dss.stencil_write_mask_back  = 0xFF;
    pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].dss.stencil_fail_op_front      = pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].dss.stencil_fail_op_back     = GFXStencilOp::KEEP;
    pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].dss.stencil_z_fail_op_front    = pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].dss.stencil_z_fail_op_back   = GFXStencilOp::KEEP;
    pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].dss.stencil_pass_op_front      = pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].dss.stencil_pass_op_back     = GFXStencilOp::KEEP;
    pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].dss.stencil_ref_front          = pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].dss.stencil_ref_back         = 0x1;
    pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL].rs.cull_mode = GFXCullMode::NONE;
    _pipelineState[(uint8_t)PipelineType::FRONT_STENCIL] = _device->CreateGFXPipelineState(pipelineInfo[(uint8_t)PipelineType::FRONT_STENCIL]);
    
    for(uint i = 0; i < BINDING_COUNT; i++)
        CC_SAFE_DESTROY(pipelineLayout[i]);
}

void StencilTest::tick(float dt)
{
    _dt += dt;
    GFXRect render_area = { 0, 0, _device->width(), _device->height() };
    GFXColor clear_color = {1.0f, 0, 0, 1.0f};
    
    _commandBuffer->Begin();
    _commandBuffer->BeginRenderPass(_fbo, render_area, GFXClearFlagBit::ALL, &clear_color, 1, 1.0f, 0);
    
    //draw label
    GFXViewport viewport;
    viewport.left = render_area.width / 6;
    viewport.top = render_area.height / 2;
    viewport.width = render_area.width / 3;
    viewport.height = render_area.height / 2;
    _commandBuffer->SetViewport(viewport);
    _commandBuffer->BindInputAssembler(_inputAssembler);
    _commandBuffer->BindBindingLayout(_bindingLayout[0]);
    _commandBuffer->BindPipelineState(_pipelineState[(uint8_t)PipelineType::STENCIL]);
    _commandBuffer->Draw(_inputAssembler);

    //draw uv_image
    viewport.left = _device->width() / 2;
    viewport.top = _device->height() / 2;
    _commandBuffer->SetViewport(viewport);
    _commandBuffer->BindInputAssembler(_inputAssembler);
    _commandBuffer->BindBindingLayout(_bindingLayout[1]);
    _commandBuffer->BindPipelineState(_pipelineState[(uint8_t)PipelineType::IMAGE]);
    _commandBuffer->Draw(_inputAssembler);

    //do back and front stencil test
    viewport.left = 0;
    viewport.top = 0;
    viewport.width = render_area.width / 3;
    viewport.height = render_area.height / 2;
    _commandBuffer->SetViewport(viewport);
    _commandBuffer->BindInputAssembler(_inputAssembler);
    _commandBuffer->BindBindingLayout(_bindingLayout[0]);
    _commandBuffer->BindPipelineState(_pipelineState[(uint8_t)PipelineType::CAVAS]);
    _commandBuffer->Draw(_inputAssembler);

    _commandBuffer->BindInputAssembler(_inputAssembler);
    _commandBuffer->BindBindingLayout(_bindingLayout[1]);
    _commandBuffer->BindPipelineState(_pipelineState[(uint8_t)PipelineType::FRONT_BACK_STENCIL]);
    _commandBuffer->Draw(_inputAssembler);

    //do back stencil test
    viewport.left = render_area.width / 3;
    viewport.top = 0;
    _commandBuffer->SetViewport(viewport);
    _commandBuffer->BindInputAssembler(_inputAssembler);
    _commandBuffer->BindBindingLayout(_bindingLayout[0]);
    _commandBuffer->BindPipelineState(_pipelineState[(uint8_t)PipelineType::CAVAS]);
    _commandBuffer->Draw(_inputAssembler);

    _commandBuffer->BindInputAssembler(_inputAssembler);
    _commandBuffer->BindBindingLayout(_bindingLayout[1]);
    _commandBuffer->BindPipelineState(_pipelineState[(uint8_t)PipelineType::BACK_STENCIL]);
    _commandBuffer->Draw(_inputAssembler);

    //do front stencil test
    viewport.left = render_area.width * 2 / 3;
    viewport.top = 0;
    _commandBuffer->SetViewport(viewport);
    _commandBuffer->BindInputAssembler(_inputAssembler);
    _commandBuffer->BindBindingLayout(_bindingLayout[0]);
    _commandBuffer->BindPipelineState(_pipelineState[(uint8_t)PipelineType::CAVAS]);
    _commandBuffer->Draw(_inputAssembler);

    _commandBuffer->BindInputAssembler(_inputAssembler);
    _commandBuffer->BindBindingLayout(_bindingLayout[1]);
    _commandBuffer->BindPipelineState(_pipelineState[(uint8_t)PipelineType::FRONT_STENCIL]);
    _commandBuffer->Draw(_inputAssembler);

    _commandBuffer->EndRenderPass();
    _commandBuffer->End();
    
    _device->queue()->submit(&_commandBuffer, 1);
    _device->Present();
}

NS_CC_END
