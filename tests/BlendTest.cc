#include "BlendTest.h"

#if (CC_PLATFORM == CC_PLATFORM_MAC_OSX)
#include "gfx-metal/GFXMTL.h"
#else
#include "gfx-gles2/GFXGLES2.h"
#include "gfx-gles3/GFXGLES3.h"
#endif

#include "cocos2d.h"

NS_CC_BEGIN

namespace {
    enum
    {
        NO_BLEND = 0x0,
        NORMAL_BLEND,
        ADDITIVE_BLEND,
        SUBSTRACT_BLEND,
        MULTIPLY_BLEND,
        TOTAL_BLEND
    };
    
    GFXTexture* createTexture(GFXDevice* device, const GFXTextureInfo& textureInfo, std::string imageFile)
    {
        auto img = new cocos2d::Image();
        img->autorelease();
        bool ret = img->initWithImageFile(imageFile);
        CCASSERT(ret, "load image failed");
        
        GFXBufferInfo imageBufferInfo = {
            GFXBufferUsage::TRANSFER_SRC,
            GFXMemoryUsage::HOST,
            1,
            static_cast<uint>(img->getDataLen()),
            GFXBufferFlagBit::NONE };
        
        auto image = device->CreateGFXBuffer(imageBufferInfo);
        image->Update(img->getData(), 0, imageBufferInfo.size);
        
        auto texture = device->CreateGFXTexture(textureInfo);
        
        GFXBufferTextureCopy textureRegion;
        textureRegion.buff_tex_height = img->getHeight();
        textureRegion.tex_extent.width = img->getWidth();
        textureRegion.tex_extent.height = img->getHeight();
        textureRegion.tex_extent.depth = 1;
        
        GFXBufferTextureCopyList regions;
        regions.push_back(std::move(textureRegion));
        
        device->CopyBuffersToTexture(image, texture, regions);
        CC_SAFE_DESTROY(image);
        return texture;
    }
    
    struct Quad : public Object
    {
        Quad(GFXDevice* _device)
        : device(_device)
        {
            createShader();
            createVertexBuffer();
            createInputAssembler();
            createTexture();
            createPipeline();
        }
        
        ~Quad()
        {
        }
        
        void Destroy()
        {
            CC_SAFE_DESTROY(shader);
            CC_SAFE_DESTROY(vertexBuffer);
            CC_SAFE_DESTROY(inputAssembler);
            CC_SAFE_DESTROY(indexBuffer);
            CC_SAFE_DESTROY(texture);
            CC_SAFE_DESTROY(sampler);
            CC_SAFE_DESTROY(texView);
            for (int i = 0; i < TOTAL_BLEND; i++) {
                CC_SAFE_DESTROY(pipelineState[i]);
                CC_SAFE_DESTROY(bindingLayout[i]);
                CC_SAFE_DESTROY(uniformBuffer[i]);
            }
        }
        
        void createShader()
        {
            GFXShaderStageList shaderStageList;
            GFXShaderStage vertexShaderStage;
            vertexShaderStage.type = GFXShaderType::VERTEX;
            
#ifdef USE_GLES2
            vertexShaderStage.source = R"(
            attribute vec2 a_position;
            attribute vec2 a_uv;
            varying vec2 uv;
            uniform mat4 u_model, u_projection;
            
            void main()
            {
                uv = a_uv;
                gl_Position = u_projection * u_model * vec4(a_position, 0, 1);
            }
            )";
#else
            vertexShaderStage.source = R"(#version 300 es
            in vec2 a_position;
            in vec2 a_uv;
            out vec2 uv;
            layout(std140) uniform MVP_Matrix
            {
                mat4 u_model, u_projection;
            };
            void main()
            {
                uv = a_uv;
                gl_Position = u_projection * u_model * vec4(a_position, 0, 1);
            }
            )";
#endif // USE_GLES2
            
            shaderStageList.emplace_back(std::move(vertexShaderStage));
            
            GFXShaderStage fragmentShaderStage;
            fragmentShaderStage.type = GFXShaderType::FRAGMENT;
            
#ifdef USE_GLES2
            fragmentShaderStage.source = R"(
#ifdef GL_ES
            precision highp float;
#endif
            varying vec2 uv;
            uniform sampler2D u_texture;
            
            void main()
            {
                gl_FragColor = texture2D(u_texture, uv);
            }
            )";
#else
            fragmentShaderStage.source = R"(#version 300 es
#ifdef GL_ES
            precision highp float;
#endif
            in vec2 uv;
            uniform sampler2D u_texture;
            out vec4 o_color;
            void main()
            {
                o_color = texture(u_texture, uv);
            }
            )";
#endif // USE_GLES2
            
            shaderStageList.emplace_back(std::move(fragmentShaderStage));
            
            GFXUniformList mvpMatrix = {
                { "u_model", GFXType::MAT4, 1},
                { "u_projection", GFXType::MAT4, 1},
            };
            GFXUniformBlockList uniformBlockList = { {0, "MVP_Matrix", mvpMatrix} };
            
            GFXUniformSamplerList sampler = { {1, "u_texture", GFXType::SAMPLER2D, 1} };
            
            GFXShaderInfo shaderInfo;
            shaderInfo.name = "Blend Test: Quad";
            shaderInfo.stages = std::move(shaderStageList);
            shaderInfo.blocks = std::move(uniformBlockList);
            shaderInfo.samplers = std::move(sampler);
            shader = device->CreateGFXShader(shaderInfo);
        }
        
        void createVertexBuffer()
        {
            float vertexData[] = {
                -0.5f, -0.5f,   0,      0,
                -0.5f,  0.5f,   0,      1.f,
                 0.5f,  0.5f,   1.f,    1.f,
                 0.5f, -0.5f,   1.f,    0
            };
            
            unsigned short indices[6] = {0, 3, 1, 1, 3, 2};
            
            //vertex buffer
            GFXBufferInfo vertexBufferInfo = {
                GFXBufferUsage::VERTEX,
                GFXMemoryUsage::NONE,
                4 * sizeof(float),
                sizeof(vertexData),
                GFXBufferFlagBit::NONE };
            
            vertexBuffer = device->CreateGFXBuffer(vertexBufferInfo);
            vertexBuffer->Update(vertexData, 0, sizeof(vertexData));
            
            //index buffer
            GFXBufferInfo indexBufferInfo = {
                GFXBufferUsage::INDEX,
                GFXMemoryUsage::NONE,
                sizeof(unsigned short),
                sizeof(indices),
                GFXBufferFlagBit::NONE
            };
            indexBuffer = device->CreateGFXBuffer(indexBufferInfo);
            indexBuffer->Update(indices, 0, sizeof(indices));
            
            //uniform buffer
            GFXBufferInfo uniformBufferInfo = {
                GFXBufferUsage::UNIFORM,
                GFXMemoryUsage::HOST,
                sizeof(Mat4),
                2 * sizeof(Mat4),
                GFXBufferFlagBit::NONE };
            
            Mat4 projection;
            Mat4::createOrthographicOffCenter(0, device->width(), device->height(), 0, 0, 1000.f, &projection);
            for (int i = 0; i < TOTAL_BLEND; i++) {
                uniformBuffer[i] = device->CreateGFXBuffer(uniformBufferInfo);
                uniformBuffer[i]->Update(projection.m, sizeof(Mat4), sizeof(projection));
            }
        }
        
        void createInputAssembler()
        {
            GFXAttribute position = {"a_position", GFXFormat::RG32F, false, 0, false};
            GFXAttribute texcoord = {"a_uv", GFXFormat::RG32F, false, 0, false};
            GFXInputAssemblerInfo inputAssemblerInfo;
            inputAssemblerInfo.attributes.emplace_back(std::move(position));
            inputAssemblerInfo.attributes.emplace_back(std::move(texcoord));
            inputAssemblerInfo.vertex_buffers.emplace_back(vertexBuffer);
            inputAssemblerInfo.index_buffer = indexBuffer;
            inputAssembler = device->CreateGFXInputAssembler(inputAssemblerInfo);
        }
        
        void createTexture()
        {
            GFXTextureInfo textureInfo;
            textureInfo.usage = GFXTextureUsage::SAMPLED;
            textureInfo.format = GFXFormat::RGBA8;
            textureInfo.width = 128;
            textureInfo.height = 128;
            texture = ::cocos2d::createTexture(device, textureInfo, "sprite0.png");
            
            //create sampler
            GFXSamplerInfo samplerInfo;
            samplerInfo.address_u = GFXAddress::CLAMP;
            samplerInfo.address_v = GFXAddress::CLAMP;
            sampler = device->CreateGFXSampler(samplerInfo);
            
            GFXTextureViewInfo texViewInfo;
            texViewInfo.texture = texture;
            texViewInfo.format = GFXFormat::RGBA8;
            texView = device->CreateGFXTextureView(texViewInfo);
        }
        
        void createPipeline()
        {
            GFXBindingList bindingList = {
                {0, GFXBindingType::UNIFORM_BUFFER, "MVP_Matrix"},
                {1, GFXBindingType::SAMPLER, "Texture2D"}
            };
            GFXBindingLayoutInfo bindingLayoutInfo = { bindingList };
            
            GFXPipelineLayout* pipelineLayout[TOTAL_BLEND] = { nullptr };
            for (int i = 0; i < TOTAL_BLEND; i++) {
                bindingLayout[i] = device->CreateGFXBindingLayout(bindingLayoutInfo);
                
                bindingLayout[i]->BindBuffer(0, uniformBuffer[i]);
                bindingLayout[i]->BindSampler(1, sampler);
                bindingLayout[i]->BindTextureView(1, texView);
                bindingLayout[i]->Update();
                
                GFXPipelineLayoutInfo pipelineLayoutInfo;
                pipelineLayoutInfo.layouts = { bindingLayout[i] };
                pipelineLayout[i] = device->CreateGFXPipelieLayout(pipelineLayoutInfo);
            }
            
            GFXPipelineStateInfo pipelineInfo[TOTAL_BLEND];
            pipelineInfo[NO_BLEND].primitive = GFXPrimitiveMode::TRIANGLE_LIST;
            pipelineInfo[NO_BLEND].shader = shader;
            pipelineInfo[NO_BLEND].is = { inputAssembler->attributes() };
            pipelineInfo[NO_BLEND].layout = pipelineLayout[NO_BLEND];
            pipelineInfo[NO_BLEND].render_pass = device->window()->render_pass();
            pipelineInfo[NO_BLEND].rs.cull_mode = GFXCullMode::NONE;
            pipelineInfo[NO_BLEND].bs.targets[0].is_blend = true;
            pipelineInfo[NO_BLEND].bs.targets[0].blend_eq = GFXBlendOp::ADD;
            pipelineInfo[NO_BLEND].bs.targets[0].blend_alpha_eq = GFXBlendOp::ADD;
            pipelineInfo[NO_BLEND].bs.targets[0].blend_src = GFXBlendFactor::ONE;
            pipelineInfo[NO_BLEND].bs.targets[0].blend_dst = GFXBlendFactor::ZERO;
            pipelineInfo[NO_BLEND].bs.targets[0].blend_src_alpha = GFXBlendFactor::ONE;
            pipelineInfo[NO_BLEND].bs.targets[0].blend_dst_alpha = GFXBlendFactor::ZERO;
            pipelineState[NO_BLEND] = device->CreateGFXPipelineState(pipelineInfo[NO_BLEND]);
            
            pipelineInfo[NORMAL_BLEND].primitive = GFXPrimitiveMode::TRIANGLE_LIST;
            pipelineInfo[NORMAL_BLEND].shader = shader;
            pipelineInfo[NORMAL_BLEND].is = { inputAssembler->attributes() };
            pipelineInfo[NORMAL_BLEND].layout = pipelineLayout[NORMAL_BLEND];
            pipelineInfo[NORMAL_BLEND].render_pass = device->window()->render_pass();
            pipelineInfo[NORMAL_BLEND].rs.cull_mode = GFXCullMode::NONE;
            pipelineInfo[NORMAL_BLEND].bs.targets[0].is_blend = true;
            pipelineInfo[NORMAL_BLEND].bs.targets[0].blend_eq = GFXBlendOp::ADD;
            pipelineInfo[NORMAL_BLEND].bs.targets[0].blend_alpha_eq = GFXBlendOp::ADD;
            pipelineInfo[NORMAL_BLEND].bs.targets[0].blend_src = GFXBlendFactor::SRC_ALPHA;
            pipelineInfo[NORMAL_BLEND].bs.targets[0].blend_dst = GFXBlendFactor::ONE_MINUS_SRC_ALPHA;
            pipelineInfo[NORMAL_BLEND].bs.targets[0].blend_src_alpha = GFXBlendFactor::ONE;
            pipelineInfo[NORMAL_BLEND].bs.targets[0].blend_dst_alpha = GFXBlendFactor::ONE;
            pipelineState[NORMAL_BLEND] = device->CreateGFXPipelineState(pipelineInfo[NORMAL_BLEND]);
            
            pipelineInfo[ADDITIVE_BLEND].primitive = GFXPrimitiveMode::TRIANGLE_LIST;
            pipelineInfo[ADDITIVE_BLEND].shader = shader;
            pipelineInfo[ADDITIVE_BLEND].is = { inputAssembler->attributes() };
            pipelineInfo[ADDITIVE_BLEND].layout = pipelineLayout[ADDITIVE_BLEND];
            pipelineInfo[ADDITIVE_BLEND].render_pass = device->window()->render_pass();
            pipelineInfo[ADDITIVE_BLEND].rs.cull_mode = GFXCullMode::NONE;
            pipelineInfo[ADDITIVE_BLEND].bs.targets[0].is_blend = true;
            pipelineInfo[ADDITIVE_BLEND].bs.targets[0].blend_eq = GFXBlendOp::ADD;
            pipelineInfo[ADDITIVE_BLEND].bs.targets[0].blend_alpha_eq = GFXBlendOp::ADD;
            pipelineInfo[ADDITIVE_BLEND].bs.targets[0].blend_src = GFXBlendFactor::SRC_ALPHA;
            pipelineInfo[ADDITIVE_BLEND].bs.targets[0].blend_dst = GFXBlendFactor::ONE;
            pipelineInfo[ADDITIVE_BLEND].bs.targets[0].blend_src_alpha = GFXBlendFactor::ONE;
            pipelineInfo[ADDITIVE_BLEND].bs.targets[0].blend_dst_alpha = GFXBlendFactor::ONE;
            pipelineState[ADDITIVE_BLEND] = device->CreateGFXPipelineState(pipelineInfo[ADDITIVE_BLEND]);
            
            pipelineInfo[SUBSTRACT_BLEND].primitive = GFXPrimitiveMode::TRIANGLE_LIST;
            pipelineInfo[SUBSTRACT_BLEND].shader = shader;
            pipelineInfo[SUBSTRACT_BLEND].is = { inputAssembler->attributes() };
            pipelineInfo[SUBSTRACT_BLEND].layout = pipelineLayout[SUBSTRACT_BLEND];
            pipelineInfo[SUBSTRACT_BLEND].render_pass = device->window()->render_pass();
            pipelineInfo[SUBSTRACT_BLEND].rs.cull_mode = GFXCullMode::NONE;
            pipelineInfo[SUBSTRACT_BLEND].bs.targets[0].is_blend = true;
            pipelineInfo[SUBSTRACT_BLEND].bs.targets[0].blend_eq = GFXBlendOp::ADD;
            pipelineInfo[SUBSTRACT_BLEND].bs.targets[0].blend_alpha_eq = GFXBlendOp::ADD;
            pipelineInfo[SUBSTRACT_BLEND].bs.targets[0].blend_src = GFXBlendFactor::ZERO;
            pipelineInfo[SUBSTRACT_BLEND].bs.targets[0].blend_dst = GFXBlendFactor::ONE_MINUS_SRC_COLOR;
            pipelineInfo[SUBSTRACT_BLEND].bs.targets[0].blend_src_alpha = GFXBlendFactor::ONE;
            pipelineInfo[SUBSTRACT_BLEND].bs.targets[0].blend_dst_alpha = GFXBlendFactor::ONE;
            pipelineState[SUBSTRACT_BLEND] = device->CreateGFXPipelineState(pipelineInfo[SUBSTRACT_BLEND]);
            
            pipelineInfo[MULTIPLY_BLEND].primitive = GFXPrimitiveMode::TRIANGLE_LIST;
            pipelineInfo[MULTIPLY_BLEND].shader = shader;
            pipelineInfo[MULTIPLY_BLEND].is = { inputAssembler->attributes() };
            pipelineInfo[MULTIPLY_BLEND].layout = pipelineLayout[MULTIPLY_BLEND];
            pipelineInfo[MULTIPLY_BLEND].render_pass = device->window()->render_pass();
            pipelineInfo[MULTIPLY_BLEND].rs.cull_mode = GFXCullMode::NONE;
            pipelineInfo[MULTIPLY_BLEND].bs.targets[0].is_blend = true;
            pipelineInfo[MULTIPLY_BLEND].bs.targets[0].blend_eq = GFXBlendOp::ADD;
            pipelineInfo[MULTIPLY_BLEND].bs.targets[0].blend_alpha_eq = GFXBlendOp::ADD;
            pipelineInfo[MULTIPLY_BLEND].bs.targets[0].blend_src = GFXBlendFactor::ZERO;
            pipelineInfo[MULTIPLY_BLEND].bs.targets[0].blend_dst = GFXBlendFactor::SRC_COLOR;
            pipelineInfo[MULTIPLY_BLEND].bs.targets[0].blend_src_alpha = GFXBlendFactor::ONE;
            pipelineInfo[MULTIPLY_BLEND].bs.targets[0].blend_dst_alpha = GFXBlendFactor::ONE;
            pipelineState[MULTIPLY_BLEND] = device->CreateGFXPipelineState(pipelineInfo[MULTIPLY_BLEND]);
            
            for (int i= 0 ; i < TOTAL_BLEND; i++) {
                CC_SAFE_DESTROY(pipelineLayout[i]);
            }
        }
        
        GFXDevice* device = nullptr;
        GFXShader* shader = nullptr;
        GFXBuffer* vertexBuffer = nullptr;
        GFXBuffer* indexBuffer = nullptr;
        GFXInputAssembler* inputAssembler = nullptr;
        GFXSampler* sampler = nullptr;
        GFXTexture* texture = nullptr;
        GFXTextureView* texView = nullptr;
        GFXBuffer* uniformBuffer[TOTAL_BLEND] = { nullptr };
        GFXBindingLayout* bindingLayout[TOTAL_BLEND] = { nullptr };
        GFXPipelineState* pipelineState[TOTAL_BLEND] = { nullptr };
        
        Mat4 model;
    };
    
    struct BigTriangle : public Object
    {
        BigTriangle(GFXDevice* _device)
        : device(_device)
        {
            createShader();
            createVertexBuffer();
            createInputAssembler();
            createTexture();
            createPipeline();
        }
        
        void Destroy()
        {
            CC_SAFE_DESTROY(shader);
            CC_SAFE_DESTROY(vertexBuffer);
            CC_SAFE_DESTROY(inputAssembler);
            CC_SAFE_DESTROY(pipelineState);
            CC_SAFE_DESTROY(bindingLayout);
            CC_SAFE_DESTROY(uniformBuffer);
            CC_SAFE_DESTROY(texture);
            CC_SAFE_DESTROY(sampler);
            CC_SAFE_DESTROY(texView);
        }
        
        void createShader()
        {
            GFXShaderStageList shaderStageList;
            GFXShaderStage vertexShaderStage;
            vertexShaderStage.type = GFXShaderType::VERTEX;
            
#ifdef USE_GLES2
            vertexShaderStage.source = R"(
            attribute vec2 a_position;
            varying vec2 uv;
            void main()
            {
                uv = (a_position + 1.0) * 0.5;
                gl_Position = vec4(a_position, 0, 1);
            }
            )";
#else
            vertexShaderStage.source = R"(#version 300 es
            in vec2 a_position;
            out vec2 uv;
            void main()
            {
                uv = (a_position + 1.0) * 0.5;
                gl_Position = vec4(a_position, 0, 1);
            }
            )";
#endif // USE_GLES2
            
            shaderStageList.emplace_back(std::move(vertexShaderStage));
            
            GFXShaderStage fragmentShaderStage;
            fragmentShaderStage.type = GFXShaderType::FRAGMENT;
            
#ifdef USE_GLES2
            fragmentShaderStage.source = R"(
#ifdef GL_ES
            precision highp float;
#endif
            varying vec2 uv;
            uniform sampler2D u_texture;
            uniform float u_time;
            
            void main()
            {
                vec2 offset = vec2(u_time * -0.01);
                gl_FragColor = texture2D(u_texture, 20.0 * (uv + offset));
            }
            )";
#else
            fragmentShaderStage.source = R"(#version 300 es
#ifdef GL_ES
            precision highp float;
#endif
            in vec2 uv;
            uniform sampler2D u_texture;
            layout(std140) uniform Time
            {
                float u_time;
            };
            out vec4 o_color;
            void main()
            {
                vec2 offset = vec2(u_time * -0.01);
                o_color = texture(u_texture, 20.0 * (uv + offset));
            }
            )";
#endif // USE_GLES2
            
            shaderStageList.emplace_back(std::move(fragmentShaderStage));
            
            GFXUniformList mvpMatrix = { { "u_time", GFXType::FLOAT, 1} };
            GFXUniformBlockList uniformBlockList = { {0, "Time", mvpMatrix} };
            
            GFXUniformSamplerList sampler = { {1, "u_texture", GFXType::SAMPLER2D, 1} };
            
            GFXShaderInfo shaderInfo;
            shaderInfo.name = "Blend Test: BigTriangle";
            shaderInfo.stages = std::move(shaderStageList);
            shaderInfo.blocks = std::move(uniformBlockList);
            shaderInfo.samplers = std::move(sampler);
            shader = device->CreateGFXShader(shaderInfo);
        }
        
        void createVertexBuffer()
        {
            float vertexData[] = {
                -1.0f,  4.0f,
                -1.0f, -1.0f,
                 4.0f, -1.0f
            };
            
            //vertex buffer
            GFXBufferInfo vertexBufferInfo = {
                GFXBufferUsage::VERTEX,
                GFXMemoryUsage::HOST,
                2 * sizeof(float),
                sizeof(vertexData),
                GFXBufferFlagBit::NONE };
            
            vertexBuffer = device->CreateGFXBuffer(vertexBufferInfo);
            vertexBuffer->Update(vertexData, 0, sizeof(vertexData));
            
            //uniform buffer
            GFXBufferInfo uniformBufferInfo = {
                GFXBufferUsage::UNIFORM,
                GFXMemoryUsage::HOST,
                sizeof(float),
                sizeof(float),
                GFXBufferFlagBit::NONE };
            uniformBuffer = device->CreateGFXBuffer(uniformBufferInfo);
        }
        
        void createInputAssembler()
        {
            GFXAttribute position = {"a_position", GFXFormat::RG32F, false, 0, false};
            GFXInputAssemblerInfo inputAssemblerInfo;
            inputAssemblerInfo.attributes.emplace_back(std::move(position));
            inputAssemblerInfo.vertex_buffers.emplace_back(vertexBuffer);
            inputAssembler = device->CreateGFXInputAssembler(inputAssemblerInfo);
        }
        
        void createTexture()
        {
            GFXTextureInfo textureInfo;
            textureInfo.usage = GFXTextureUsage::SAMPLED;
            textureInfo.format = GFXFormat::RGBA8;
            textureInfo.width = 128;
            textureInfo.height = 128;
            textureInfo.flags = GFXTextureFlagBit::GEN_MIPMAP;
            texture = ::cocos2d::createTexture(device, textureInfo, "background.png");
            
            //create sampler
            GFXSamplerInfo samplerInfo;
            samplerInfo.name = "Background Generate mipmap";
            samplerInfo.mip_filter = GFXFilter::LINEAR;
            samplerInfo.address_u = GFXAddress::WRAP;
            samplerInfo.address_v = GFXAddress::WRAP;
            sampler = device->CreateGFXSampler(samplerInfo);
            
            GFXTextureViewInfo texViewInfo;
            texViewInfo.texture = texture;
            texViewInfo.format = GFXFormat::RGBA8;
            texView = device->CreateGFXTextureView(texViewInfo);
        }
        
        void createPipeline()
        {
            GFXBindingList bindingList = {
                {0, GFXBindingType::UNIFORM_BUFFER, "Time"},
                {1, GFXBindingType::SAMPLER, "Texture2D"}
            };
            GFXBindingLayoutInfo bindingLayoutInfo = { bindingList };
            bindingLayout = device->CreateGFXBindingLayout(bindingLayoutInfo);
            
            bindingLayout->BindBuffer(0, uniformBuffer);
            bindingLayout->BindSampler(1, sampler);
            bindingLayout->BindTextureView(1, texView);
            bindingLayout->Update();
            
            GFXPipelineLayoutInfo pipelineLayoutInfo;
            pipelineLayoutInfo.layouts = { bindingLayout };
            auto pipelineLayout = device->CreateGFXPipelieLayout(pipelineLayoutInfo);
            
            GFXPipelineStateInfo pipelineInfo;
            pipelineInfo.primitive = GFXPrimitiveMode::TRIANGLE_LIST;
            pipelineInfo.shader = shader;
            pipelineInfo.is = { inputAssembler->attributes() };
            pipelineInfo.layout = pipelineLayout;
            pipelineInfo.render_pass = device->window()->render_pass();
            
            pipelineState = device->CreateGFXPipelineState(pipelineInfo);
            
            CC_SAFE_DESTROY(pipelineLayout);
        }
        
        GFXDevice* device = nullptr;
        GFXShader* shader = nullptr;
        GFXBuffer* vertexBuffer = nullptr;
        GFXBuffer* uniformBuffer = nullptr;
        GFXInputAssembler* inputAssembler = nullptr;
        GFXSampler* sampler = nullptr;
        GFXBindingLayout* bindingLayout = nullptr;
        GFXTexture* texture = nullptr;
        GFXTextureView* texView = nullptr;
        GFXPipelineState* pipelineState = nullptr;
    };
    
    Mat4 createModel(const Vec3& t, const Vec3& s)
    {
        Mat4 model;
        model.translate(t);
        model.scale(s);
        
        return model;
    }
    
    BigTriangle* bigTriangle = nullptr;
    Quad* quad = nullptr;
}

void BlendTest::Destroy()
{
    CC_SAFE_DESTROY(bigTriangle);
    CC_SAFE_DESTROY(quad);
}

bool BlendTest::initialize()
{
    bigTriangle = CC_NEW(BigTriangle(_device));
    quad = CC_NEW(Quad(_device));
    return true;
}

void BlendTest::tick(float dt) {

    _dt += dt;

    GFXRect render_area = {0, 0, _device->width(), _device->height() };
    GFXColor clear_color = {0.0f, 0, 0, 1.0f};

    _commandBuffer->Begin();
    _commandBuffer->BeginRenderPass(_fbo, render_area, GFXClearFlagBit::ALL, &clear_color, 1, 1.0f, 0);
    
    //draw background
    bigTriangle->uniformBuffer->Update(&_dt, 0, sizeof(_dt));
    _commandBuffer->BindInputAssembler(bigTriangle->inputAssembler);
    _commandBuffer->BindBindingLayout(bigTriangle->bindingLayout);
    _commandBuffer->BindPipelineState(bigTriangle->pipelineState);
    _commandBuffer->Draw(bigTriangle->inputAssembler);

    //draw sprite without blending
    float size = std::min(render_area.width, render_area.height) * 0.15f;
    float halfSize = size * 0.5f;
    float offsetX = 5.f + halfSize;
    float offsetY = 5.f + halfSize;
    quad->model = std::move(createModel(Vec3(offsetX, offsetY, 0), Vec3(size, size, 0)));
    quad->uniformBuffer[NO_BLEND]->Update(quad->model.m, 0, sizeof(quad->model));
    _commandBuffer->BindInputAssembler(quad->inputAssembler);
    _commandBuffer->BindBindingLayout(quad->bindingLayout[NO_BLEND]);
    _commandBuffer->BindPipelineState(quad->pipelineState[NO_BLEND]);
    _commandBuffer->Draw(quad->inputAssembler);
    
    //normal
    offsetY = offsetY + 5.f + size;
    quad->model = std::move(createModel(cocos2d::Vec3(offsetX, offsetY, 0), cocos2d::Vec3(size, size, 0)));
    quad->uniformBuffer[NORMAL_BLEND]->Update(quad->model.m, 0, sizeof(quad->model));
    _commandBuffer->BindInputAssembler(quad->inputAssembler);
    _commandBuffer->BindBindingLayout(quad->bindingLayout[NORMAL_BLEND]);
    _commandBuffer->BindPipelineState(quad->pipelineState[NORMAL_BLEND]);
    _commandBuffer->Draw(quad->inputAssembler);
    
    //additive
    offsetY = offsetY + 5.f + size;
    quad->model = std::move(createModel(cocos2d::Vec3(offsetX, offsetY, 0), cocos2d::Vec3(size, size, 0)));
    quad->uniformBuffer[ADDITIVE_BLEND]->Update(quad->model.m, 0, sizeof(quad->model));
    _commandBuffer->BindInputAssembler(quad->inputAssembler);
    _commandBuffer->BindBindingLayout(quad->bindingLayout[ADDITIVE_BLEND]);
    _commandBuffer->BindPipelineState(quad->pipelineState[ADDITIVE_BLEND]);
    _commandBuffer->Draw(quad->inputAssembler);
    
    //substract
    offsetY = offsetY + 5.f + size;
    quad->model = std::move(createModel(cocos2d::Vec3(offsetX, offsetY, 0), cocos2d::Vec3(size, size, 0)));
    quad->uniformBuffer[SUBSTRACT_BLEND]->Update(quad->model.m, 0, sizeof(quad->model));
    _commandBuffer->BindInputAssembler(quad->inputAssembler);
    _commandBuffer->BindBindingLayout(quad->bindingLayout[SUBSTRACT_BLEND]);
    _commandBuffer->BindPipelineState(quad->pipelineState[SUBSTRACT_BLEND]);
    _commandBuffer->Draw(quad->inputAssembler);
    
    //multiply
    offsetY = offsetY + 5.f + size;
    quad->model = std::move(createModel(cocos2d::Vec3(offsetX, offsetY, 0), cocos2d::Vec3(size, size, 0)));
    quad->uniformBuffer[MULTIPLY_BLEND]->Update(quad->model.m, 0, sizeof(quad->model));
    _commandBuffer->BindInputAssembler(quad->inputAssembler);
    _commandBuffer->BindBindingLayout(quad->bindingLayout[MULTIPLY_BLEND]);
    _commandBuffer->BindPipelineState(quad->pipelineState[MULTIPLY_BLEND]);
    _commandBuffer->Draw(quad->inputAssembler);
    
    _commandBuffer->EndRenderPass();
    _commandBuffer->End();

    _device->queue()->submit(&_commandBuffer, 1);
    _device->Present();
}

NS_CC_END
