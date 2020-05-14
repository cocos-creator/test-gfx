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
#if (CC_PLATFORM == CC_PLATFORM_MAC_OSX && defined(MAC_USE_METAL))
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
            
#if defined(USE_VULKAN)
            vertexShaderStage.source = R"(
            layout(location = 0) in vec2 a_position;
            layout(location = 0) out vec2 v_texcoord;
            
            void main() {
                v_texcoord = (a_position + 1.0) * 0.5;
            #ifndef GL_ES
                v_texcoord = vec2(v_texcoord.x, 1.0 - v_texcoord.y);
            #endif
                gl_Position = vec4(a_position, 0, 1);
            }
            )";
#elif defined(USE_GLES2)
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
            vertexShaderStage.source = R"(
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
            
#if (CC_PLATFORM == CC_PLATFORM_MAC_OSX && defined(MAC_USE_METAL))
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
            
#if defined(USE_VULKAN)
            fragmentShaderStage.source = R"(
            layout(location = 0) in vec2 v_texcoord;
            layout(binding = 0) uniform Near_Far_Uniform
            {
                float u_near;
                float u_far;
            };
            layout(binding = 1) uniform sampler2D u_texture;
            layout(location = 0) out vec4 o_color;
            void main() {
                float z = texture(u_texture, v_texcoord).x;
                float viewZ = (u_near * u_far) / ((u_far - u_near) * z - u_far);
                float depth = (viewZ + u_near) / (u_near - u_far);
                
                o_color.rgb = vec3(depth);
                o_color.a = 1.0;
            }
            )";
#elif defined(USE_GLES2)
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
            fragmentShaderStage.source = R"(
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
             GFXUniformBlockList uniformBlockList = { { GFXShaderType::FRAGMENT,  0, "Near_Far_Uniform", nearFarUniform} };
             GFXUniformSamplerList samplers = { { GFXShaderType::FRAGMENT,  1, "u_texture", GFXType::SAMPLER2D, 1} };
             
             GFXShaderInfo shaderInfo;
             shaderInfo.name = "BigTriangle";
             shaderInfo.stages = std::move(shaderStageList);
             shaderInfo.blocks = std::move(uniformBlockList);
             shaderInfo.samplers = std::move(samplers);
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
                GFXMemoryUsage::DEVICE,
                2 * sizeof(float),
                sizeof(vertices),
                GFXBufferFlagBit::NONE
            };
            vertexBuffer = device->createBuffer(vertexBufferInfo);
            vertexBuffer->update(vertices, 0, sizeof(vertices));
            
            //create uniform buffer
            GFXBufferInfo uniformBufferInfo = {
                GFXBufferUsage::UNIFORM,
                GFXMemoryUsage::HOST | GFXMemoryUsage::DEVICE,
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
                {GFXShaderType::FRAGMENT, 0, GFXBindingType::UNIFORM_BUFFER, "Near_Far_Uniform", 1},
                {GFXShaderType::FRAGMENT, texBindingLoc, GFXBindingType::SAMPLER, "u_texture", 1}
            };
            GFXBindingLayoutInfo bindingLayoutInfo = { bindingList };
            bindingLayout = device->createBindingLayout(bindingLayoutInfo);
            
            bindingLayout->bindBuffer(0, nearFarUniformBuffer);
            bindingLayout->bindSampler(texBindingLoc, sampler);
            // don't update just yet for the texture is still missing
            
            GFXPipelineLayoutInfo pipelineLayoutInfo;
            pipelineLayoutInfo.layouts = { bindingLayout };
            pipelineLayout = device->createPipelineLayout(pipelineLayoutInfo);
            
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
            CC_SAFE_DESTROY(pipelineLayout);
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
        GFXPipelineLayout* pipelineLayout = nullptr;
        uint texBindingLoc = 0;
    };
    
    struct Bunny : public Object
    {
        Bunny(GFXDevice* _device, GFXFramebuffer* _fbo)
        : device(_device)
        {
            createShader();
            createBuffers();
            createInputAssembler();
            createPipeline(_fbo);
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
#if (CC_PLATFORM == CC_PLATFORM_MAC_OSX && defined(MAC_USE_METAL))
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
            
#if defined(USE_VULKAN)
            vertexShaderStage.source = R"(
            layout(location = 0) in vec3 a_position;
            layout(binding = 0) uniform MVP_Matrix
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
#elif defined(USE_GLES2)
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
            vertexShaderStage.source = R"(
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
#if (CC_PLATFORM == CC_PLATFORM_MAC_OSX && defined(MAC_USE_METAL))
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
            
#if defined(USE_VULKAN)
            fragmentShaderStage.source = R"(
            layout(location = 0) out vec4 o_color;
            void main ()
            {
                o_color = vec4(1, 1, 1, 1);
            }
            )";
#elif defined(USE_GLES2)
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
            fragmentShaderStage.source = R"(
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
            GFXUniformBlockList uniformBlockList = { {GFXShaderType::VERTEX, 0, "MVP_Matrix", mvpMatrix} };
            
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
                GFXMemoryUsage::DEVICE,
                3*sizeof(float),
                sizeof(bunny_positions),
                GFXBufferFlagBit::NONE };
            
            vertexBuffer = device->createBuffer(vertexBufferInfo);
            vertexBuffer->update((void*)&bunny_positions[0][0], 0, sizeof(bunny_positions));
            
            //index buffer
            GFXBufferInfo indexBufferInfo = {
                GFXBufferUsage::INDEX,
                GFXMemoryUsage::DEVICE,
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
                GFXMemoryUsage::HOST | GFXMemoryUsage::DEVICE,
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
        
        void createPipeline(GFXFramebuffer* _fbo)
        {
            GFXBindingList bindingList = {
                {GFXShaderType::VERTEX, 0, GFXBindingType::UNIFORM_BUFFER, "MVP_Matrix", 1},
            };
            GFXBindingLayoutInfo bindingLayoutInfo = { bindingList };

            for (uint i = 0u; i < BUNNY_NUM; i++)
            {
                bindingLayout[i] = device->createBindingLayout(bindingLayoutInfo);
                bindingLayout[i]->bindBuffer(0, mvpUniformBuffer[i]);
                bindingLayout[i]->update();
            }

            GFXPipelineLayoutInfo pipelineLayoutInfo;
            pipelineLayoutInfo.layouts = { bindingLayout[0] };
            pipelineLayout = device->createPipelineLayout(pipelineLayoutInfo);

            GFXPipelineStateInfo pipelineInfo;
            pipelineInfo.primitive = GFXPrimitiveMode::TRIANGLE_LIST;
            pipelineInfo.shader = shader;
            pipelineInfo.inputState = { inputAssembler->getAttributes() };
            pipelineInfo.layout = pipelineLayout;
            pipelineInfo.renderPass = _fbo->getRenderPass();
            pipelineInfo.depthStencilState.depthTest = true;
            pipelineInfo.depthStencilState.depthWrite = true;
            pipelineInfo.depthStencilState.depthFunc = GFXComparisonFunc::LESS;

            pipelineState = device->createPipelineState(pipelineInfo);
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
            }
            CC_SAFE_DESTROY(pipelineLayout);
            CC_SAFE_DESTROY(pipelineState);
        }
        const static uint BUNNY_NUM = 2;
        GFXDevice* device = nullptr;
        GFXShader* shader = nullptr;
        GFXBuffer* vertexBuffer = nullptr;
        GFXBuffer* indexBuffer = nullptr;
        GFXSampler* sampler = nullptr;
        GFXTexture* depthTexture = nullptr;
        GFXTextureView* depthView = nullptr;
        GFXInputAssembler* inputAssembler = nullptr;
        GFXBuffer* mvpUniformBuffer[BUNNY_NUM] = { nullptr, nullptr };
        GFXBindingLayout* bindingLayout[BUNNY_NUM] = { nullptr, nullptr };
        GFXPipelineLayout* pipelineLayout = nullptr;
        GFXPipelineState* pipelineState = nullptr;
    };
    
    BigTriangle* bg;
    Bunny* bunny;
}

void DepthTexture::destroy()
{
    CC_SAFE_DESTROY(bg);
    CC_SAFE_DESTROY(bunny);
    CC_SAFE_DELETE(_bunnyFBO);
}

bool DepthTexture::initialize()
{
    _bunnyFBO = CC_NEW(Framebuffer);

    GFXRenderPassInfo renderPassInfo;

    GFXColorAttachment colorAttachment;
    colorAttachment.format = _device->getColorFormat();
    colorAttachment.loadOp = GFXLoadOp::DISCARD;
    colorAttachment.storeOp = GFXStoreOp::DISCARD;
    colorAttachment.sampleCount = 1;
    colorAttachment.beginLayout = GFXTextureLayout::COLOR_ATTACHMENT_OPTIMAL;
    colorAttachment.endLayout = GFXTextureLayout::COLOR_ATTACHMENT_OPTIMAL;
    renderPassInfo.colorAttachments.emplace_back(colorAttachment);

    GFXDepthStencilAttachment& depthStencilAttachment = renderPassInfo.depthStencilAttachment;
    depthStencilAttachment.format = _device->getDepthStencilFormat();
    depthStencilAttachment.depthLoadOp = GFXLoadOp::CLEAR;
    depthStencilAttachment.depthStoreOp = GFXStoreOp::STORE;
    depthStencilAttachment.stencilLoadOp = GFXLoadOp::CLEAR;
    depthStencilAttachment.stencilStoreOp = GFXStoreOp::STORE;
    depthStencilAttachment.sampleCount = 1;
    depthStencilAttachment.beginLayout = GFXTextureLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    depthStencilAttachment.endLayout = GFXTextureLayout::SHADER_READONLY_OPTIMAL;

    _bunnyFBO->renderPass = _device->createRenderPass(renderPassInfo);

    GFXTextureInfo colorTexInfo;
    colorTexInfo.type = GFXTextureType::TEX2D;
    colorTexInfo.usage = GFXTextureUsageBit::COLOR_ATTACHMENT | GFXTextureUsageBit::SAMPLED;
    colorTexInfo.format = _device->getColorFormat();
    colorTexInfo.width = _device->getWidth();
    colorTexInfo.height = _device->getHeight();
    colorTexInfo.depth = 1;
    colorTexInfo.arrayLayer = 1;
    colorTexInfo.mipLevel = 1;
    _bunnyFBO->colorTex = _device->createTexture(colorTexInfo);

    GFXTextureViewInfo colorTexViewInfo;
    colorTexViewInfo.texture = _bunnyFBO->colorTex;
    colorTexViewInfo.type = GFXTextureViewType::TV2D;
    colorTexViewInfo.format = _device->getColorFormat();
    colorTexViewInfo.baseLevel = 0;
    colorTexViewInfo.levelCount = 1;
    colorTexViewInfo.baseLayer = 0;
    colorTexViewInfo.layerCount = 1;
    _bunnyFBO->colorTexView = _device->createTextureView(colorTexViewInfo);

    GFXTextureInfo depthStecnilTexInfo;
    depthStecnilTexInfo.type = GFXTextureType::TEX2D;
    depthStecnilTexInfo.usage = GFXTextureUsageBit::DEPTH_STENCIL_ATTACHMENT | GFXTextureUsageBit::SAMPLED;
    depthStecnilTexInfo.format = _device->getDepthStencilFormat();
    depthStecnilTexInfo.width = _device->getWidth();
    depthStecnilTexInfo.height = _device->getHeight();
    depthStecnilTexInfo.depth = 1;
    depthStecnilTexInfo.arrayLayer = 1;
    depthStecnilTexInfo.mipLevel = 1;
    _bunnyFBO->depthStencilTex = _device->createTexture(depthStecnilTexInfo);

    GFXTextureViewInfo depthStecnilTexViewInfo;
    depthStecnilTexViewInfo.texture = _bunnyFBO->depthStencilTex;
    depthStecnilTexViewInfo.type = GFXTextureViewType::TV2D;
    depthStecnilTexViewInfo.format = _device->getDepthStencilFormat();
    depthStecnilTexViewInfo.baseLevel = 0;
    depthStecnilTexViewInfo.levelCount = 1;
    depthStecnilTexViewInfo.baseLayer = 0;
    depthStecnilTexViewInfo.layerCount = 1;
    _bunnyFBO->depthStencilTexView = _device->createTextureView(depthStecnilTexViewInfo);

    GFXFramebufferInfo fboInfo;
    fboInfo.renderPass = _bunnyFBO->renderPass;
    if (_bunnyFBO->colorTexView)
    {
        fboInfo.colorViews.push_back(_bunnyFBO->colorTexView);
    }
    fboInfo.depthStencilView = _bunnyFBO->depthStencilTexView;
    fboInfo.isOffscreen = true;
    _bunnyFBO->framebuffer = _device->createFramebuffer(fboInfo);

    bunny = CC_NEW(Bunny(_device, _bunnyFBO->framebuffer));
    bg = CC_NEW(BigTriangle(_device));
    return true;
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

    _device->begin();

    for (auto commandBuffer : _commandBuffers)
    {
        commandBuffer->begin();
        
        //render bunny
        commandBuffer->beginRenderPass(_bunnyFBO->framebuffer, render_area, GFXClearFlagBit::DEPTH, std::move(std::vector<GFXColor>({clear_color})), 1.0f, 0);
        commandBuffer->bindPipelineState(bunny->pipelineState);
        commandBuffer->bindInputAssembler(bunny->inputAssembler);
        for (uint i = 0; i < Bunny::BUNNY_NUM; i++)
        {
            _model = Mat4::IDENTITY;
            /*if(i % 2 == 0)
                _model.translate(5, 0, 0);
            else
                _model.translate(-5, 0, 0);*/
            bunny->mvpUniformBuffer[i]->update(_model.m, 0, sizeof(_model));
            bunny->mvpUniformBuffer[i]->update(_view.m, sizeof(_model), sizeof(_view));
            bunny->mvpUniformBuffer[i]->update(_projection.m, sizeof(_model) + sizeof(_view), sizeof(_projection));
            bunny->bindingLayout[i]->update();
            
            commandBuffer->bindBindingLayout(bunny->bindingLayout[i]);
            commandBuffer->draw(bunny->inputAssembler);
        }
        commandBuffer->endRenderPass();
        
        //render bg
        bg->bindingLayout->bindTextureView(bg->texBindingLoc, _bunnyFBO->depthStencilTexView);
        bg->bindingLayout->update();
        commandBuffer->beginRenderPass(_fbo, render_area, GFXClearFlagBit::ALL, std::move(std::vector<GFXColor>({clear_color})), 1.0f, 0);
        commandBuffer->bindInputAssembler(bg->inputAssembler);
        commandBuffer->bindPipelineState(bg->pipelineState);
        commandBuffer->bindBindingLayout(bg->bindingLayout);
        commandBuffer->draw(bg->inputAssembler);
        commandBuffer->endRenderPass();
        
        commandBuffer->end();
    }
    _device->getQueue()->submit(_commandBuffers);
    _device->present();
}

NS_CC_END
