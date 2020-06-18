#include "DepthTest.h"
#include "BunnyData.h"

namespace cc {

    namespace {
        struct BigTriangle : public gfx::Object {
            BigTriangle(gfx::GFXDevice *_device, gfx::GFXFramebuffer *_fbo) : device(_device), fbo(_fbo) {
                createShader();
                createBuffers();
                createSampler();
                createInputAssembler();
                createPipeline();
            }

            void createShader() {
                gfx::GFXShaderStageList shaderStageList;
                gfx::GFXShaderStage vertexShaderStage;
                vertexShaderStage.type = gfx::GFXShaderType::VERTEX;

#if defined(USE_VULKAN) || defined(USE_METAL)
                vertexShaderStage.source = R"(
            layout(location = 0) in vec2 a_position;
            layout(location = 0) out vec2 v_texcoord;
            
            void main() {
                v_texcoord = (a_position + 1.0) * 0.5;
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

                shaderStageList.emplace_back(std::move(vertexShaderStage));

                gfx::GFXShaderStage fragmentShaderStage;
                fragmentShaderStage.type = gfx::GFXShaderType::FRAGMENT;

#if defined(USE_VULKAN) || defined(USE_METAL)
                fragmentShaderStage.source = R"(
            #ifdef GL_ES
                        precision highp float;
            #endif
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

                shaderStageList.emplace_back(std::move(fragmentShaderStage));

                gfx::GFXAttributeList attributeList = { { "a_position", gfx::GFXFormat::RG32F, false, 0, false, 0 } };
                gfx::GFXUniformList nearFarUniform = { {"u_near", gfx::GFXType::FLOAT, 1},
                                                 {"u_far", gfx::GFXType::FLOAT, 1} };
                gfx::GFXUniformBlockList uniformBlockList = {
                    {gfx::GFXShaderType::FRAGMENT, 0, "Near_Far_Uniform", nearFarUniform} };
                gfx::GFXUniformSamplerList samplers = {
                    {gfx::GFXShaderType::FRAGMENT, 1, "u_texture", gfx::GFXType::SAMPLER2D, 1} };

                gfx::GFXShaderInfo shaderInfo;
                shaderInfo.name = "BigTriangle";
                shaderInfo.stages = std::move(shaderStageList);
                shaderInfo.attributes = std::move(attributeList);
                shaderInfo.blocks = std::move(uniformBlockList);
                shaderInfo.samplers = std::move(samplers);
                shader = device->createShader(shaderInfo);
            }

            void createSampler() {
                // create sampler
                gfx::GFXSamplerInfo samplerInfo;
                samplerInfo.addressU = gfx::GFXAddress::CLAMP;
                samplerInfo.addressV = gfx::GFXAddress::CLAMP;
                sampler = device->createSampler(samplerInfo);
            }

            void createBuffers() {
                // create vertex buffer
                float ySign = device->getProjectionSignY();
                float vertices[] = { -1, 4 * ySign, -1, -1 * ySign, 4, -1 * ySign };
                gfx::GFXBufferInfo vertexBufferInfo = { gfx::GFXBufferUsage::VERTEX,
                                                  gfx::GFXMemoryUsage::DEVICE, 2 * sizeof(float),
                                                  sizeof(vertices), gfx::GFXBufferFlagBit::NONE };
                vertexBuffer = device->createBuffer(vertexBufferInfo);
                vertexBuffer->update(vertices, 0, sizeof(vertices));

                // create uniform buffer
                gfx::GFXBufferInfo uniformBufferInfo = {
                    gfx::GFXBufferUsage::UNIFORM, gfx::GFXMemoryUsage::DEVICE, sizeof(float),
                    2 * sizeof(float), gfx::GFXBufferFlagBit::NONE };
                nearFarUniformBuffer = device->createBuffer(uniformBufferInfo);

                float nearValue = 0.1f;
                float farValue = 100.0f;
                nearFarUniformBuffer->update(&nearValue, 0, sizeof(nearValue));
                nearFarUniformBuffer->update(&farValue, sizeof(nearValue),
                    sizeof(farValue));
            }

            void createInputAssembler() {
                gfx::GFXAttribute position = { "a_position", gfx::GFXFormat::RG32F, false, 0, false };
                gfx::GFXInputAssemblerInfo inputAssemblerInfo;
                inputAssemblerInfo.attributes.emplace_back(std::move(position));
                inputAssemblerInfo.vertexBuffers.emplace_back(vertexBuffer);
                inputAssembler = device->createInputAssembler(inputAssemblerInfo);
            }

            void createPipeline() {
                texBindingLoc = 1;
                gfx::GFXBindingList bindingList = { {gfx::GFXShaderType::FRAGMENT, 0,
                                               gfx::GFXBindingType::UNIFORM_BUFFER,
                                               "Near_Far_Uniform", 1},
                                              {gfx::GFXShaderType::FRAGMENT, texBindingLoc,
                                               gfx::GFXBindingType::SAMPLER, "u_texture", 1} };
                gfx::GFXBindingLayoutInfo bindingLayoutInfo = { bindingList };
                bindingLayout = device->createBindingLayout(bindingLayoutInfo);

                bindingLayout->bindBuffer(0, nearFarUniformBuffer);
                bindingLayout->bindSampler(texBindingLoc, sampler);
                // don't update just yet for the texture is still missing

                gfx::GFXPipelineLayoutInfo pipelineLayoutInfo;
                pipelineLayoutInfo.layouts = { bindingLayout };
                pipelineLayout = device->createPipelineLayout(pipelineLayoutInfo);

                gfx::GFXPipelineStateInfo pipelineInfo;
                pipelineInfo.primitive = gfx::GFXPrimitiveMode::TRIANGLE_LIST;
                pipelineInfo.shader = shader;
                pipelineInfo.inputState = { inputAssembler->getAttributes() };
                pipelineInfo.layout = pipelineLayout;
                pipelineInfo.renderPass = fbo->getRenderPass();

                pipelineInfo.depthStencilState.depthTest = false;
                pipelineInfo.depthStencilState.depthWrite = false;
                pipelineInfo.depthStencilState.stencilTestBack = false;
                pipelineInfo.depthStencilState.stencilTestFront = false;

                pipelineInfo.rasterizerState.cullMode = gfx::GFXCullMode::NONE;

                pipelineState = device->createPipelineState(pipelineInfo);
            }
            ~BigTriangle() {}

            void destroy() {
                CC_SAFE_DESTROY(shader);
                CC_SAFE_DESTROY(vertexBuffer);
                CC_SAFE_DESTROY(inputAssembler);
                CC_SAFE_DESTROY(bindingLayout);
                CC_SAFE_DESTROY(sampler);
                CC_SAFE_DESTROY(texture);
                CC_SAFE_DESTROY(pipelineState);
                CC_SAFE_DESTROY(pipelineLayout);
                CC_SAFE_DESTROY(nearFarUniformBuffer);
            }

            gfx::GFXShader *shader = nullptr;
            gfx::GFXFramebuffer *fbo = nullptr;
            gfx::GFXBuffer *vertexBuffer = nullptr;
            gfx::GFXBuffer *nearFarUniformBuffer = nullptr;
            gfx::GFXDevice *device = nullptr;
            gfx::GFXInputAssembler *inputAssembler = nullptr;
            gfx::GFXBindingLayout *bindingLayout = nullptr;
            gfx::GFXSampler *sampler = nullptr;
            gfx::GFXTexture *texture = nullptr;
            gfx::GFXPipelineState *pipelineState = nullptr;
            gfx::GFXPipelineLayout *pipelineLayout = nullptr;
            uint texBindingLoc = 0;
        };

        struct Bunny : public gfx::Object {
            Bunny(gfx::GFXDevice *_device, gfx::GFXFramebuffer *_fbo) : device(_device) {
                createShader();
                createBuffers();
                createInputAssembler();
                createPipeline(_fbo);
            }

            ~Bunny() {}

            void createShader() {
                // vertex shader
                gfx::GFXShaderStageList shaderStageList;
                gfx::GFXShaderStage vertexShaderStage;
                vertexShaderStage.type = gfx::GFXShaderType::VERTEX;

#if defined(USE_VULKAN) | defined(USE_METAL)
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

                shaderStageList.emplace_back(std::move(vertexShaderStage));

                // fragment shader
                gfx::GFXShaderStage fragmentShaderStage;
                fragmentShaderStage.type = gfx::GFXShaderType::FRAGMENT;

#if defined(USE_VULKAN) | defined(USE_METAL)
                fragmentShaderStage.source = R"(
            #ifdef GL_ES
                       precision highp float;
                       #endif
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

                shaderStageList.emplace_back(std::move(fragmentShaderStage));

                gfx::GFXAttributeList attributeList = { { "a_position", gfx::GFXFormat::RGB32F, false, 0, false, 0 } };
                gfx::GFXUniformList mvpMatrix = { {"u_model", gfx::GFXType::MAT4, 1},
                                            {"u_view", gfx::GFXType::MAT4, 1},
                                            {"u_projection", gfx::GFXType::MAT4, 1} };
                gfx::GFXUniformBlockList uniformBlockList = {
                    {gfx::GFXShaderType::VERTEX, 0, "MVP_Matrix", mvpMatrix} };

                gfx::GFXShaderInfo shaderInfo;
                shaderInfo.name = "Bunny";
                shaderInfo.attributes = std::move(attributeList);
                shaderInfo.stages = std::move(shaderStageList);
                shaderInfo.blocks = std::move(uniformBlockList);
                shader = device->createShader(shaderInfo);
            }

            void createBuffers() {
                // vertex buffer
                gfx::GFXBufferInfo vertexBufferInfo = {
                    gfx::GFXBufferUsage::VERTEX, gfx::GFXMemoryUsage::DEVICE, 3 * sizeof(float),
                    sizeof(bunny_positions), gfx::GFXBufferFlagBit::NONE };

                vertexBuffer = device->createBuffer(vertexBufferInfo);
                vertexBuffer->update((void *)&bunny_positions[0][0], 0,
                    sizeof(bunny_positions));

                // index buffer
                gfx::GFXBufferInfo indexBufferInfo = {
                    gfx::GFXBufferUsage::INDEX, gfx::GFXMemoryUsage::DEVICE, sizeof(unsigned short),
                    sizeof(bunny_cells), gfx::GFXBufferFlagBit::NONE };
                indexBuffer = device->createBuffer(indexBufferInfo);
                indexBuffer->update((void *)&bunny_cells[0], 0, sizeof(bunny_cells));

                // uniform buffer
                // create uniform buffer
                gfx::GFXBufferInfo uniformBufferInfo = {
                    gfx::GFXBufferUsage::UNIFORM, gfx::GFXMemoryUsage::HOST | gfx::GFXMemoryUsage::DEVICE,
                    sizeof(Mat4), 3 * sizeof(Mat4), gfx::GFXBufferFlagBit::NONE };
                for (uint i = 0; i < BUNNY_NUM; i++)
                    mvpUniformBuffer[i] = device->createBuffer(uniformBufferInfo);
            }

            void createInputAssembler() {
                gfx::GFXAttribute position = { "a_position", gfx::GFXFormat::RGB32F, false, 0, false };
                gfx::GFXInputAssemblerInfo inputAssemblerInfo;
                inputAssemblerInfo.attributes.emplace_back(std::move(position));
                inputAssemblerInfo.vertexBuffers.emplace_back(vertexBuffer);
                inputAssemblerInfo.indexBuffer = indexBuffer;
                inputAssembler = device->createInputAssembler(inputAssemblerInfo);
            }

            void createPipeline(gfx::GFXFramebuffer *_fbo) {
                gfx::GFXBindingList bindingList = {
                    {gfx::GFXShaderType::VERTEX, 0, gfx::GFXBindingType::UNIFORM_BUFFER, "MVP_Matrix",
                     1},
                };
                gfx::GFXBindingLayoutInfo bindingLayoutInfo = { bindingList };

                for (uint i = 0u; i < BUNNY_NUM; i++) {
                    bindingLayout[i] = device->createBindingLayout(bindingLayoutInfo);
                    bindingLayout[i]->bindBuffer(0, mvpUniformBuffer[i]);
                    bindingLayout[i]->update();
                }

                gfx::GFXPipelineLayoutInfo pipelineLayoutInfo;
                pipelineLayoutInfo.layouts = { bindingLayout[0] };
                pipelineLayout = device->createPipelineLayout(pipelineLayoutInfo);

                gfx::GFXPipelineStateInfo pipelineInfo;
                pipelineInfo.primitive = gfx::GFXPrimitiveMode::TRIANGLE_LIST;
                pipelineInfo.shader = shader;
                pipelineInfo.inputState = { inputAssembler->getAttributes() };
                pipelineInfo.layout = pipelineLayout;
                pipelineInfo.renderPass = _fbo->getRenderPass();
                pipelineInfo.depthStencilState.depthTest = true;
                pipelineInfo.depthStencilState.depthWrite = true;
                pipelineInfo.depthStencilState.depthFunc = gfx::GFXComparisonFunc::LESS;

                pipelineState = device->createPipelineState(pipelineInfo);
            }

            void destroy() {
                CC_SAFE_DESTROY(shader);
                CC_SAFE_DESTROY(vertexBuffer);
                CC_SAFE_DESTROY(indexBuffer);
                CC_SAFE_DESTROY(sampler);
                CC_SAFE_DESTROY(depthTexture);
                CC_SAFE_DESTROY(inputAssembler);
                for (uint i = 0; i < BUNNY_NUM; i++) {
                    CC_SAFE_DESTROY(mvpUniformBuffer[i]);
                    CC_SAFE_DESTROY(bindingLayout[i]);
                }
                CC_SAFE_DESTROY(pipelineLayout);
                CC_SAFE_DESTROY(pipelineState);
            }
            const static uint BUNNY_NUM = 2;
            gfx::GFXDevice *device = nullptr;
            gfx::GFXShader *shader = nullptr;
            gfx::GFXBuffer *vertexBuffer = nullptr;
            gfx::GFXBuffer *indexBuffer = nullptr;
            gfx::GFXSampler *sampler = nullptr;
            gfx::GFXTexture *depthTexture = nullptr;
            gfx::GFXInputAssembler *inputAssembler = nullptr;
            gfx::GFXBuffer *mvpUniformBuffer[BUNNY_NUM] = { nullptr, nullptr };
            gfx::GFXBindingLayout *bindingLayout[BUNNY_NUM] = { nullptr, nullptr };
            gfx::GFXPipelineLayout *pipelineLayout = nullptr;
            gfx::GFXPipelineState *pipelineState = nullptr;
        };

        BigTriangle *bg;
        Bunny *bunny;
    } // namespace

    void DepthTexture::destroy() {
        CC_SAFE_DESTROY(bg);
        CC_SAFE_DESTROY(bunny);
        CC_SAFE_DELETE(_bunnyFBO);
    }

    bool DepthTexture::initialize() {
        _bunnyFBO = CC_NEW(Framebuffer);

        gfx::GFXRenderPassInfo renderPassInfo;

        gfx::GFXColorAttachment colorAttachment;
        colorAttachment.format = _device->getColorFormat();
        colorAttachment.loadOp = gfx::GFXLoadOp::DISCARD;
        colorAttachment.storeOp = gfx::GFXStoreOp::DISCARD;
        colorAttachment.sampleCount = 1;
        colorAttachment.beginLayout = gfx::GFXTextureLayout::UNDEFINED;
        colorAttachment.endLayout = gfx::GFXTextureLayout::COLOR_ATTACHMENT_OPTIMAL;
        renderPassInfo.colorAttachments.emplace_back(colorAttachment);

        gfx::GFXDepthStencilAttachment &depthStencilAttachment =
            renderPassInfo.depthStencilAttachment;
        depthStencilAttachment.format = _device->getDepthStencilFormat();
        depthStencilAttachment.depthLoadOp = gfx::GFXLoadOp::CLEAR;
        depthStencilAttachment.depthStoreOp = gfx::GFXStoreOp::STORE;
        depthStencilAttachment.stencilLoadOp = gfx::GFXLoadOp::CLEAR;
        depthStencilAttachment.stencilStoreOp = gfx::GFXStoreOp::STORE;
        depthStencilAttachment.sampleCount = 1;
        depthStencilAttachment.beginLayout = gfx::GFXTextureLayout::UNDEFINED;
        depthStencilAttachment.endLayout = gfx::GFXTextureLayout::SHADER_READONLY_OPTIMAL;

        _bunnyFBO->renderPass = _device->createRenderPass(renderPassInfo);

        gfx::GFXTextureInfo colorTexInfo;
        colorTexInfo.type = gfx::GFXTextureType::TEX2D;
        colorTexInfo.usage =
            gfx::GFXTextureUsageBit::COLOR_ATTACHMENT | gfx::GFXTextureUsageBit::SAMPLED;
        colorTexInfo.format = _device->getColorFormat();
        colorTexInfo.width = _device->getWidth();
        colorTexInfo.height = _device->getHeight();
        colorTexInfo.depth = 1;
        colorTexInfo.arrayLayer = 1;
        colorTexInfo.mipLevel = 1;
        _bunnyFBO->colorTex = _device->createTexture(colorTexInfo);

        gfx::GFXTextureInfo depthStecnilTexInfo;
        depthStecnilTexInfo.type = gfx::GFXTextureType::TEX2D;
        depthStecnilTexInfo.usage = gfx::GFXTextureUsageBit::DEPTH_STENCIL_ATTACHMENT |
            gfx::GFXTextureUsageBit::SAMPLED;
        depthStecnilTexInfo.format = _device->getDepthStencilFormat();
        depthStecnilTexInfo.width = _device->getWidth();
        depthStecnilTexInfo.height = _device->getHeight();
        depthStecnilTexInfo.depth = 1;
        depthStecnilTexInfo.arrayLayer = 1;
        depthStecnilTexInfo.mipLevel = 1;
        _bunnyFBO->depthStencilTex = _device->createTexture(depthStecnilTexInfo);

        gfx::GFXFramebufferInfo fboInfo;
        fboInfo.renderPass = _bunnyFBO->renderPass;
        if (_bunnyFBO->colorTex) {
            fboInfo.colorTextures.push_back(_bunnyFBO->colorTex);
        }
        fboInfo.depthStencilTexture = _bunnyFBO->depthStencilTex;
        fboInfo.isOffscreen = true;
        _bunnyFBO->framebuffer = _device->createFramebuffer(fboInfo);

        bunny = CC_NEW(Bunny(_device, _bunnyFBO->framebuffer));
        bg = CC_NEW(BigTriangle(_device, _fbo));
        return true;
    }

    void DepthTexture::tick(float dt) {
        _dt += dt;

        _eye.set(30.f * std::cos(_dt), 20.f, 30.f * std::sin(_dt));
        _center.set(0, 2.5f, 0);
        _up.set(0, 1.f, 0);
        Mat4::createLookAt(_eye, _center, _up, &_view);
        Mat4::createPerspective(45.f,
            1.0f * _device->getWidth() / _device->getHeight(),
            0.1f, 100.f, &_projection);
        TestBaseI::modifyProjectionBasedOnDevice(_projection);

        gfx::GFXRect render_area = { 0, 0, _device->getWidth(), _device->getHeight() };
        gfx::GFXColor clear_color = { 1.0, 0, 0, 1.0f };

        _device->acquire();

        auto commandBuffer = _commandBuffers[0];
        commandBuffer->begin();

        // render bunny
        commandBuffer->beginRenderPass(
            _bunnyFBO->framebuffer, render_area, gfx::GFXClearFlagBit::DEPTH,
            std::move(std::vector<gfx::GFXColor>({ clear_color })), 1.0f, 0);
        commandBuffer->bindPipelineState(bunny->pipelineState);
        commandBuffer->bindInputAssembler(bunny->inputAssembler);
        for (uint i = 0; i < Bunny::BUNNY_NUM; i++) {
            _model = Mat4::IDENTITY;
            if (i % 2 == 0)
                _model.translate(5, 0, 0);
            else
                _model.translate(-5, 0, 0);
            bunny->mvpUniformBuffer[i]->update(_model.m, 0, sizeof(_model));
            bunny->mvpUniformBuffer[i]->update(_view.m, sizeof(_model),
                sizeof(_view));
            bunny->mvpUniformBuffer[i]->update(
                _projection.m, sizeof(_model) + sizeof(_view), sizeof(_projection));
            bunny->bindingLayout[i]->update();

            commandBuffer->bindBindingLayout(bunny->bindingLayout[i]);
            commandBuffer->draw(bunny->inputAssembler);
        }
        commandBuffer->endRenderPass();

        // render bg
        bg->bindingLayout->bindTexture(bg->texBindingLoc,
            _bunnyFBO->depthStencilTex);
        bg->bindingLayout->update();
        commandBuffer->beginRenderPass(
            _fbo, render_area, gfx::GFXClearFlagBit::ALL,
            std::move(std::vector<gfx::GFXColor>({ clear_color })), 1.0f, 0);
        commandBuffer->bindInputAssembler(bg->inputAssembler);
        commandBuffer->bindPipelineState(bg->pipelineState);
        commandBuffer->bindBindingLayout(bg->bindingLayout);
        commandBuffer->draw(bg->inputAssembler);
        commandBuffer->endRenderPass();

        commandBuffer->end();

        _device->getQueue()->submit(_commandBuffers);
        _device->present();
    }

} // namespace cc
