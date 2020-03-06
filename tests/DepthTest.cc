#include "DepthTest.h"
#if (CC_PLATFORM == CC_PLATFORM_MAC_OSX)
#include "gfx-metal/GFXMTL.h"
#else
#include "gfx-gles2/GFXGLES2.h"
#include "gfx-gles3/GFXGLES3.h"
#endif
#include "cocos2d.h"
#include "BunnyData.h"

NS_CC_BEGIN

namespace
{
    struct BigTriangle : public Object
    {
        BigTriangle(GFXDevice* _device)
        :device(_device)
        {
            createShader();
            createBuffers();
            createSampler();
            createInputAssembler();
            createPipeline();
        }
        
        void createShader()
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
                float2 v_texcoord [[user(locn0)]];
                float4 gl_Position [[position]];
            };
            
            struct main0_in
            {
                float2 a_position [[attribute(0)]];
            };
            
            vertex main0_out main0(main0_in in [[stage_in]])
            {
                main0_out out = {};
                out.v_texcoord = (in.a_position + float2(1.0)) * 0.5;
                out.gl_Position = float4(in.a_position, 0.0, 1.0);
                return out;
            }
            )";
#else
            
#ifdef USE_GLES2
            vertexShaderStage.source = R"(
            attribute vec2 a_position;
            varying vec2 v_texcoord;
            
            void main() {
                v_texcoord = (a_position + 1.0) * 0.5;
            #ifndef GL_ES
                v_texcoord = vec2(v_texcoord.x, 1.0 - v_texcoord.y);
            #endif
                gl_Position = vec4(a_position, 0, 1);
            }
            )";
#else
            vertexShaderStage.source = R"(#version 300 es
            in vec2 a_position;
            out vec2 v_texcoord;
            
            void main() {
                v_texcoord = (a_position + 1.0) * 0.5;
            #ifndef GL_ES
                v_texcoord = vec2(v_texcoord.x, 1.0 - v_texcoord.y);
            #endif
                gl_Position = vec4(a_position, 0, 1);
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
            
            struct Near_Far_Uniform
            {
                float u_near;
                float u_far;
            };
            
            struct main0_out
            {
                float4 o_color [[color(0)]];
            };
            
            struct main0_in
            {
                float2 v_texcoord [[user(locn0)]];
            };
            
            fragment main0_out main0(main0_in in [[stage_in]], constant Near_Far_Uniform& _26 [[buffer(0)]], texture2d<float> u_texture [[texture(0)]], sampler u_textureSmplr [[sampler(0)]])
            {
                main0_out out = {};
                float z = u_texture.sample(u_textureSmplr, in.v_texcoord).x;
                float viewZ = (_26.u_near * _26.u_far) / (((_26.u_far - _26.u_near) * z) - _26.u_far);
                float depth = (viewZ + _26.u_near) / (_26.u_near - _26.u_far);
                float3 _62 = float3(depth);
                out.o_color = float4(_62.x, _62.y, _62.z, out.o_color.w);
                out.o_color.w = 1.0;
                //out.o_color = float4(1.0, 0, 0, 1.0);
                return out;
            }
            )";
#else
            
#ifdef USE_GLES2
            fragmentShaderStage.source = R"(
            #ifdef GL_ES
            precision highp float;
            #endif
            varying vec2 v_texcoord;
            uniform sampler2D u_texture;
            uniform float u_near;
            uniform float u_far;
            
            void main() {
                float z = texture2D(u_texture, v_texcoord).x;
                float viewZ = (u_near * u_far) / ((u_far - u_near) * z - u_far);
                float depth = (viewZ + u_near) / (u_near - u_far);
                
                gl_FragColor.rgb = vec3(depth);
                gl_FragColor.a = 1.0;
            }
            )";
#else
            fragmentShaderStage.source = R"(#version 300 es
            #ifdef GL_ES
            precision highp float;
            #endif
            in vec2 v_texcoord;
            uniform sampler2D u_texture;
            layout(std140) uniform Near_Far_Uniform
            {
                float u_near;
                float u_far;
            };
            out vec4 o_color;
            void main() {
                float z = texture(u_texture, v_texcoord).x;
                float viewZ = (u_near * u_far) / ((u_far - u_near) * z - u_far);
                float depth = (viewZ + u_near) / (u_near - u_far);
                
                o_color.rgb = vec3(depth);
                o_color.a = 1.0;
            }
            )";
#endif // USE_GLES2
            
#endif // (CC_PLATFORM == CC_PLATFORM_MAC_OSX)
            shaderStageList.emplace_back(std::move(fragmentShaderStage));
                                         
             GFXUniformList nearFarUniform = {
                 { "u_near", GFXType::FLOAT, 1},
                 { "u_far", GFXType::FLOAT, 1}
             };
             GFXUniformBlockList uniformBlockList = { {0, "Near_Far_Uniform", nearFarUniform} };
             GFXUniformSamplerList sampler = { {1, "u_texture", GFXType::SAMPLER2D, 1} };
             
             GFXShaderInfo shaderInfo;
             shaderInfo.name = "BigTriangle";
             shaderInfo.stages = std::move(shaderStageList);
             shaderInfo.blocks = std::move(uniformBlockList);
             shaderInfo.samplers = std::move(sampler);
             shader = device->createShader(shaderInfo);
        }
        
        void createSampler()
        {
            //create sampler
            GFXSamplerInfo samplerInfo;
            samplerInfo.addressU = GFXAddress::CLAMP;
            samplerInfo.addressV = GFXAddress::CLAMP;
            sampler = device->createSampler(samplerInfo);
        }
                                         
        void createBuffers()
        {
            //create vertex buffer
            float vertices[] = {-1, 4, -1, -1, 4, -1};
            GFXBufferInfo vertexBufferInfo = {
                GFXBufferUsage::VERTEX,
                GFXMemoryUsage::HOST,
                2 * sizeof(float),
                sizeof(vertices),
                GFXBufferFlagBit::NONE
            };
            vertexBuffer = device->createBuffer(vertexBufferInfo);
            vertexBuffer->update(vertices, 0, sizeof(vertices));
            
            //create uniform buffer
            GFXBufferInfo uniformBufferInfo = {
                GFXBufferUsage::UNIFORM,
                GFXMemoryUsage::HOST,
                sizeof(float),
                2 * sizeof(float),
                GFXBufferFlagBit::NONE };
            nearFarUniformBuffer = device->createBuffer(uniformBufferInfo);
            
            float nearValue = 0.1f;
            float farValue = 100.0f;
            nearFarUniformBuffer->update(&nearValue, 0, sizeof(nearValue));
            nearFarUniformBuffer->update(&farValue, sizeof(nearValue), sizeof(farValue));
        }
                                         
         void createInputAssembler()
         {
             GFXAttribute position = {"a_position", GFXFormat::RG32F, false, 0, false};
             GFXInputAssemblerInfo inputAssemblerInfo;
             inputAssemblerInfo.attributes.emplace_back(std::move(position));
             inputAssemblerInfo.vertexBuffers.emplace_back(vertexBuffer);
             inputAssembler = device->createInputAssembler(inputAssemblerInfo);
         }
                
        void createPipeline()
        {
            texBindingLoc = 1;
            GFXBindingList bindingList = {
                {0, GFXBindingType::UNIFORM_BUFFER, "Near_Far_Uniform"},
                {texBindingLoc, GFXBindingType::SAMPLER, "u_texture"}
            };
            GFXBindingLayoutInfo bindingLayoutInfo = { bindingList };
            bindingLayout = device->createBindingLayout(bindingLayoutInfo);
            
            
            bindingLayout->bindBuffer(0, nearFarUniformBuffer);
            bindingLayout->bindSampler(texBindingLoc, sampler);
            bindingLayout->update();
            
            GFXPipelineLayoutInfo pipelineLayoutInfo;
            pipelineLayoutInfo.layouts = { bindingLayout };
            auto pipelineLayout = device->createPipelineLayout(pipelineLayoutInfo);
            
            GFXPipelineStateInfo pipelineInfo;
            pipelineInfo.primitive = GFXPrimitiveMode::TRIANGLE_LIST;
            pipelineInfo.shader = shader;
            pipelineInfo.inputState = { inputAssembler->getAttributes() };
            pipelineInfo.layout = pipelineLayout;
            pipelineInfo.renderPass = device->getMainWindow()->getRenderPass();
            
            pipelineInfo.depthStencilState.depthTest = false;
            pipelineInfo.depthStencilState.depthWrite = false;
            pipelineInfo.depthStencilState.stencilTestBack = false;
            pipelineInfo.depthStencilState.stencilTestFront = false;
            
            pipelineInfo.rasterizerState.cullMode = GFXCullMode::NONE;
            
            pipelineState = device->createPipelineState(pipelineInfo);
            
            CC_SAFE_DESTROY(pipelineLayout);
        }
        ~BigTriangle()
        {
            
        }
        
        void destroy()
        {
            CC_SAFE_DESTROY(shader);
            CC_SAFE_DESTROY(vertexBuffer);
            CC_SAFE_DESTROY(inputAssembler);
            CC_SAFE_DESTROY(bindingLayout);
            CC_SAFE_DESTROY(sampler);
            CC_SAFE_DESTROY(texture);
            CC_SAFE_DESTROY(textureView);
            CC_SAFE_DESTROY(pipelineState);
            CC_SAFE_DESTROY(nearFarUniformBuffer);
        }
        
        GFXShader* shader = nullptr;
        GFXBuffer* vertexBuffer = nullptr;
        GFXBuffer* nearFarUniformBuffer = nullptr;
        GFXDevice* device = nullptr;
        GFXInputAssembler* inputAssembler = nullptr;
        GFXBindingLayout* bindingLayout = nullptr;
        GFXSampler* sampler = nullptr;
        GFXTexture* texture = nullptr;
        GFXTextureView* textureView = nullptr;
        GFXPipelineState* pipelineState = nullptr;
        uint texBindingLoc = 0;
    };
    
    struct Bunny : public Object
    {
        Bunny(GFXDevice* _device, GFXWindow* _window)
        : device(_device)
        {
            createShader();
            createBuffers();
            createInputAssembler();
            createPipeline(_window);
        }
        
        ~Bunny()
        {
        }
        
        void createShader()
        {
            //vertex shader
            GFXShaderStageList shaderStageList;
            GFXShaderStage vertexShaderStage;
            vertexShaderStage.type = GFXShaderType::VERTEX;
#if (CC_PLATFORM == CC_PLATFORM_MAC_OSX)
            vertexShaderStage.source = R"(
            #include <metal_stdlib>
            #include <simd/simd.h>
            
            using namespace metal;
            
            struct MVP_Matrix
            {
                float4x4 u_model;
                float4x4 u_view;
                float4x4 u_projection;
            };
            
            struct main0_out
            {
                float4 gl_Position [[position]];
            };
            
            struct main0_in
            {
                float3 a_position [[attribute(0)]];
            };
            
            vertex main0_out main0(main0_in in [[stage_in]], constant MVP_Matrix& _13 [[buffer(0)]])
            {
                main0_out out = {};
                float4 pos = ((_13.u_projection * _13.u_view) * _13.u_model) * float4(in.a_position, 1.0);
                out.gl_Position = pos;
                return out;
            }
            )";
#else
            
#ifdef USE_GLES2
            vertexShaderStage.source = R"(
            #ifdef GL_ES
            precision highp float;
            #endif
            attribute vec3 a_position;
            uniform mat4 u_model, u_view, u_projection;
            
            void main ()
            {
                vec4 pos = u_projection * u_view * u_model * vec4(a_position, 1);
                
                gl_Position = pos;
            }
            )";
#else
            vertexShaderStage.source = R"(#version 300 es
            in vec3 a_position;
            layout(std140) uniform MVP_Matrix
            {
                mat4 u_model;
                mat4 u_view;
                mat4 u_projection;
            };
            void main ()
            {
                vec4 pos = u_projection * u_view * u_model * vec4(a_position, 1);
                
                gl_Position = pos;
            }
            )";
#endif // USE_GLES2
            
#endif // (CC_PLATFORM == CC_PLATFORM_MAC_OSX)
            shaderStageList.emplace_back(std::move(vertexShaderStage));
            
            //fragment shader
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
            
            fragment main0_out main0()
            {
                main0_out out = {};
                out.o_color = float4(1.0);
                return out;
            }
            )";
#else
            
#ifdef USE_GLES2
            fragmentShaderStage.source = R"(
            #ifdef GL_ES
            precision highp float;
            #endif
            void main ()
            {
                gl_FragColor = vec4(1, 1, 1, 1);
            }
            )";
#else
            fragmentShaderStage.source = R"(#version 300 es
            #ifdef GL_ES
            precision highp float;
            #endif
            out vec4 o_color;
            void main ()
            {
                o_color = vec4(1, 1, 1, 1);
            }
            )";
#endif // USE_GLES2
            
#endif // #if (CC_PLATFORM == CC_PLATFORM_MAC_OSX)
            shaderStageList.emplace_back(std::move(fragmentShaderStage));
            
            GFXUniformList mvpMatrix = {
                { "u_model", GFXType::MAT4, 1},
                { "u_view", GFXType::MAT4, 1},
                { "u_projection", GFXType::MAT4, 1} };
            GFXUniformBlockList uniformBlockList = { {0, "MVP_Matrix", mvpMatrix} };
            
            GFXShaderInfo shaderInfo;
            shaderInfo.name = "Bunny";
            shaderInfo.stages = std::move(shaderStageList);
            shaderInfo.blocks = std::move(uniformBlockList);
            shader = device->createShader(shaderInfo);
        }
        
        void createBuffers()
        {
            //vertex buffer
            GFXBufferInfo vertexBufferInfo = {
                GFXBufferUsage::VERTEX,
                GFXMemoryUsage::HOST,
                3*sizeof(float),
                sizeof(bunny_positions),
                GFXBufferFlagBit::NONE };
            
            vertexBuffer = device->createBuffer(vertexBufferInfo);
            vertexBuffer->update((void*)&bunny_positions[0][0], 0, sizeof(bunny_positions));
            
            //index buffer
            GFXBufferInfo indexBufferInfo = {
                GFXBufferUsage::INDEX,
                GFXMemoryUsage::NONE,
                sizeof(unsigned short),
                sizeof(bunny_cells),
                GFXBufferFlagBit::NONE
            };
            indexBuffer = device->createBuffer(indexBufferInfo);
            indexBuffer->update((void*)&bunny_cells[0], 0, sizeof(bunny_cells));
            
            //uniform buffer
            //create uniform buffer
            GFXBufferInfo uniformBufferInfo = {
                GFXBufferUsage::UNIFORM,
                GFXMemoryUsage::HOST,
                sizeof(Mat4),
                3*sizeof(Mat4),
                GFXBufferFlagBit::NONE
            };
            for(uint i = 0; i < BUNNY_NUM; i++)
                mvpUniformBuffer[i] = device->createBuffer(uniformBufferInfo);
        }
        
        void createInputAssembler()
        {
            GFXAttribute position = {"a_position", GFXFormat::RGB32F, false, 0, false};
            GFXInputAssemblerInfo inputAssemblerInfo;
            inputAssemblerInfo.attributes.emplace_back(std::move(position));
            inputAssemblerInfo.vertexBuffers.emplace_back(vertexBuffer);
            inputAssemblerInfo.indexBuffer = indexBuffer;
            inputAssembler = device->createInputAssembler(inputAssemblerInfo);
        }
        
        void createPipeline(GFXWindow* _window)
        {
            GFXBindingList bindingList = {
                {0, GFXBindingType::UNIFORM_BUFFER, "MVP_Matrix"},
            };
            GFXBindingLayoutInfo bindingLayoutInfo = { bindingList };
            auto bunnyIndex = 0;
            bindingLayout[bunnyIndex] = device->createBindingLayout(bindingLayoutInfo);
            bindingLayout[bunnyIndex]->bindBuffer(0, mvpUniformBuffer[bunnyIndex]);
            bindingLayout[bunnyIndex]->update();
            
            GFXPipelineLayoutInfo pipelineLayoutInfo;
            pipelineLayoutInfo.layouts = { bindingLayout[bunnyIndex] };
            auto pipelineLayout = device->createPipelineLayout(pipelineLayoutInfo);
            
            GFXPipelineStateInfo pipelineInfo;
            pipelineInfo.primitive = GFXPrimitiveMode::TRIANGLE_LIST;
            pipelineInfo.shader = shader;
            pipelineInfo.inputState = { inputAssembler->getAttributes() };
            pipelineInfo.layout = pipelineLayout;
            pipelineInfo.renderPass = _window->getRenderPass();
            pipelineInfo.depthStencilState.depthTest = true;
            pipelineInfo.depthStencilState.depthWrite = true;
            pipelineInfo.depthStencilState.depthFunc = GFXComparisonFunc::LESS;

            pipelineState[bunnyIndex] = device->createPipelineState(pipelineInfo);
            CC_SAFE_DESTROY(pipelineLayout);
            
            bunnyIndex++;
            pipelineLayoutInfo.layouts.clear();
            pipelineLayoutInfo.layouts = { bindingLayout[bunnyIndex] };
            pipelineLayout = device->createPipelineLayout(pipelineLayoutInfo);
            bindingLayout[bunnyIndex] = device->createBindingLayout(bindingLayoutInfo);
            bindingLayout[bunnyIndex]->bindBuffer(0, mvpUniformBuffer[bunnyIndex]);
            bindingLayout[bunnyIndex]->update();
            pipelineInfo.layout = pipelineLayout;
            pipelineState[bunnyIndex] = device->createPipelineState(pipelineInfo);
            CC_SAFE_DESTROY(pipelineLayout);
        }

        void destroy()
        {
            CC_SAFE_DESTROY(shader);
            CC_SAFE_DESTROY(vertexBuffer);
            CC_SAFE_DESTROY(indexBuffer);
            CC_SAFE_DESTROY(sampler);
            CC_SAFE_DESTROY(depthTexture);
            CC_SAFE_DESTROY(depthView);
            CC_SAFE_DESTROY(inputAssembler);
            for(uint i = 0; i < BUNNY_NUM; i++)
            {
                CC_SAFE_DESTROY(mvpUniformBuffer[i]);
                CC_SAFE_DESTROY(bindingLayout[i]);
                CC_SAFE_DESTROY(pipelineState[i]);
            }
        }
        const static uint BUNNY_NUM = 2;
        GFXDevice* device = nullptr;
        GFXShader* shader = nullptr;
        GFXBuffer* vertexBuffer = nullptr;
        GFXBuffer* indexBuffer = nullptr;
        GFXSampler* sampler = nullptr;
        GFXTexture* depthTexture = nullptr;
        GFXTextureView* depthView = nullptr;
        GFXBuffer* mvpUniformBuffer[BUNNY_NUM] = { nullptr, nullptr };
        GFXInputAssembler* inputAssembler = nullptr;
        GFXBindingLayout* bindingLayout[BUNNY_NUM] = { nullptr, nullptr };
        GFXPipelineState* pipelineState[BUNNY_NUM] = { nullptr, nullptr };
    };
    
    BigTriangle* bg;
    Bunny* bunny;
}

void DepthTexture::destroy()
{
    CC_SAFE_DESTROY(_bunnyWindow);
    CC_SAFE_DESTROY(bunny);
    CC_SAFE_DESTROY(bg);
}

bool DepthTexture::initialize()
{
    createFBO();
    bunny = CC_NEW(Bunny(_device, _bunnyWindow));
    bg = CC_NEW(BigTriangle(_device));
    return true;
}

void DepthTexture::createFBO()
{
    GFXWindowInfo windowInfo;
    windowInfo.width = _device->getWidth();
    windowInfo.height = _device->getHeight();
    windowInfo.colorFmt = GFXFormat::RGBA8;
    windowInfo.depthStencilFmt = GFXFormat::D24S8;
    windowInfo.isOffscreen = true;
    _bunnyWindow = _device->createWindow(windowInfo);
}

void DepthTexture::tick(float dt)
{
    _dt += dt;
     
    _eye.set(30.f * std::cos(_dt), 20.f, 30.f * std::sin(_dt));
    _center.set(0, 2.5f, 0);
    _up.set(0, 1.f, 0);
    Mat4::createLookAt(_eye, _center, _up, &_view);
    Mat4::createPerspective(45.f, 1.0f * _device->getWidth()/ _device->getHeight(), 0.1f, 100.f, &_projection);
    
    GFXRect render_area = {0, 0, _device->getWidth(), _device->getHeight() };
    GFXColor clear_color = {1.0, 0, 0, 1.0f};

    for(auto commandBuffer : _commandBuffers)
    {
        commandBuffer->begin();
        
        //render bunny
        commandBuffer->beginRenderPass(_bunnyWindow->getFramebuffer(), render_area, GFXClearFlagBit::DEPTH, std::move(std::vector<GFXColor>({clear_color})), 1.0f, 0);
        for(uint i = 0; i < Bunny::BUNNY_NUM; i++)
        {
            _model = Mat4::IDENTITY;
            if(i % 2 == 0)
                _model.translate(5, 0, 0);
            else
                _model.translate(-5, 0, 0);
            bunny->mvpUniformBuffer[i]->update(_model.m, 0, sizeof(_model));
            bunny->mvpUniformBuffer[i]->update(_view.m, sizeof(_model), sizeof(_view));
            bunny->mvpUniformBuffer[i]->update(_projection.m, sizeof(_model) + sizeof(_view), sizeof(_projection));
            bunny->bindingLayout[i]->update();
            
            commandBuffer->bindInputAssembler(bunny->inputAssembler);
            commandBuffer->bindBindingLayout(bunny->bindingLayout[i]);
            commandBuffer->bindPipelineState(bunny->pipelineState[i]);
            commandBuffer->draw(bunny->inputAssembler);
        }
        commandBuffer->endRenderPass();
        
        //render bg
        bg->bindingLayout->bindTextureView(bg->texBindingLoc, _bunnyWindow->getDepthStencilTexView());
        bg->bindingLayout->update();
        commandBuffer->beginRenderPass(_fbo, render_area, GFXClearFlagBit::ALL, std::move(std::vector<GFXColor>({clear_color})), 1.0f, 0);
        commandBuffer->bindInputAssembler(bg->inputAssembler);
        commandBuffer->bindBindingLayout(bg->bindingLayout);
        commandBuffer->bindPipelineState(bg->pipelineState);
        commandBuffer->draw(bg->inputAssembler);
        commandBuffer->endRenderPass();
        
        commandBuffer->end();
    }
    _device->getQueue()->submit(_commandBuffers);
    _device->present();
}

NS_CC_END
