#include "BlendTest.h"

namespace cc {

    namespace {
        enum {
            NO_BLEND = 0x0,
            NORMAL_BLEND,
            ADDITIVE_BLEND,
            SUBSTRACT_BLEND,
            MULTIPLY_BLEND,
            TOTAL_BLEND
        };

        gfx::GFXTexture *createTexture(gfx::GFXDevice *device, const gfx::GFXTextureInfo &textureInfo,
            std::string imageFile) {
            auto img = new cc::Image();
            img->autorelease();
            bool valid = img->initWithImageFile(imageFile);
            CCASSERT(valid, "load image failed");
            auto imgData = img->getRenderFormat() == gfx::GFXFormat::RGB8
                ? TestBaseI::RGB2RGBA(img)
                : img->getData();

            auto texture = device->createTexture(textureInfo);

            gfx::GFXBufferTextureCopy textureRegion;
            textureRegion.texExtent.width = img->getWidth();
            textureRegion.texExtent.height = img->getHeight();
            textureRegion.texExtent.depth = 1;

            gfx::GFXBufferTextureCopyList regions;
            regions.push_back(std::move(textureRegion));

            gfx::GFXDataArray imageBuffers = { {imgData} };
            device->copyBuffersToTexture(imageBuffers, texture, regions);
            delete[] imgData;
            return texture;
        }

        struct Quad : public gfx::Object {
            Quad(gfx::GFXDevice *_device, gfx::GFXFramebuffer *_fbo) : device(_device), fbo(_fbo) {
                createShader();
                createVertexBuffer();
                createInputAssembler();
                createTexture();
                createPipeline();
            }

            ~Quad() {}

            void destroy() {
                CC_SAFE_DESTROY(shader);
                CC_SAFE_DESTROY(vertexBuffer);
                CC_SAFE_DESTROY(inputAssembler);
                CC_SAFE_DESTROY(indexBuffer);
                CC_SAFE_DESTROY(texture);
                CC_SAFE_DESTROY(sampler);
                for (int i = 0; i < TOTAL_BLEND; i++) {
                    CC_SAFE_DESTROY(pipelineState[i]);
                    CC_SAFE_DESTROY(pipelineLayout[i]);
                    CC_SAFE_DESTROY(bindingLayout[i]);
                    CC_SAFE_DESTROY(uniformBuffer[i]);
                }
            }

            void createShader() {
                gfx::GFXShaderStageList shaderStageList;
                gfx::GFXShaderStage vertexShaderStage;
                vertexShaderStage.type = gfx::GFXShaderType::VERTEX;

#if defined(USE_VULKAN) || defined(USE_METAL)
                vertexShaderStage.source = R"(
            #ifdef GL_ES
                        precision highp float;
            #endif
            layout(location = 0) in vec2 a_position;
            layout(location = 1) in vec2 a_uv;
            layout(location = 0) out vec2 uv;
            layout(binding = 0) uniform MVP_Matrix
            {
                mat4 u_model, u_projection;
            };
            void main()
            {
                uv = a_uv;
                gl_Position = u_projection * u_model * vec4(a_position, 0, 1);
            }
            )";
#elif defined(USE_GLES2)
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
                vertexShaderStage.source = R"(
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

                gfx::GFXShaderStage fragmentShaderStage;
                fragmentShaderStage.type = gfx::GFXShaderType::FRAGMENT;

#if defined(USE_VULKAN) || defined(USE_METAL)
                fragmentShaderStage.source = R"(
            #ifdef GL_ES
                        precision highp float;
            #endif
            layout(location = 0) in vec2 uv;
            layout(binding = 1) uniform sampler2D u_texture;
            layout(location = 0) out vec4 o_color;
            void main()
            {
                o_color = texture(u_texture, uv);
            }
            )";
#elif defined(USE_GLES2)
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
                fragmentShaderStage.source = R"(
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

                gfx::GFXAttributeList attributeList = {
                    { "a_position", gfx::GFXFormat::RG32F, false, 0, false, 0 },
                    { "a_uv", gfx::GFXFormat::RG32F, false, 0, false, 1 },
                };
                gfx::GFXUniformList mvpMatrix = {
                    {"u_model", gfx::GFXType::MAT4, 1},
                    {"u_projection", gfx::GFXType::MAT4, 1},
                };
                gfx::GFXUniformBlockList uniformBlockList = {
                    {gfx::GFXShaderType::VERTEX, 0, "MVP_Matrix", mvpMatrix} };

                gfx::GFXUniformSamplerList samplers = {
                    {gfx::GFXShaderType::FRAGMENT, 1, "u_texture", gfx::GFXType::SAMPLER2D, 1} };

                gfx::GFXShaderInfo shaderInfo;
                shaderInfo.name = "Blend Test: Quad";
                shaderInfo.stages = std::move(shaderStageList);
                shaderInfo.attributes = std::move(attributeList);
                shaderInfo.blocks = std::move(uniformBlockList);
                shaderInfo.samplers = std::move(samplers);
                shader = device->createShader(shaderInfo);
            }

            void createVertexBuffer() {
                float vertexData[] = { -0.5f, -0.5f, 0,   0,   -0.5f, 0.5f,  0,   1.f,
                                      0.5f,  0.5f,  1.f, 1.f, 0.5f,  -0.5f, 1.f, 0 };

                unsigned short indices[6] = { 0, 3, 1, 1, 3, 2 };

                // vertex buffer
                gfx::GFXBufferInfo vertexBufferInfo = {
                    gfx::GFXBufferUsage::VERTEX, gfx::GFXMemoryUsage::DEVICE, 4 * sizeof(float),
                    sizeof(vertexData), gfx::GFXBufferFlagBit::NONE };

                vertexBuffer = device->createBuffer(vertexBufferInfo);
                vertexBuffer->update(vertexData, 0, sizeof(vertexData));

                // index buffer
                gfx::GFXBufferInfo indexBufferInfo = {
                    gfx::GFXBufferUsage::INDEX, gfx::GFXMemoryUsage::DEVICE, sizeof(unsigned short),
                    sizeof(indices), gfx::GFXBufferFlagBit::NONE };
                indexBuffer = device->createBuffer(indexBufferInfo);
                indexBuffer->update(indices, 0, sizeof(indices));

                // uniform buffer
                gfx::GFXBufferInfo uniformBufferInfo = {
                    gfx::GFXBufferUsage::UNIFORM, gfx::GFXMemoryUsage::DEVICE, sizeof(Mat4),
                    2 * sizeof(Mat4), gfx::GFXBufferFlagBit::NONE };

                Mat4 projection;
                Mat4::createOrthographicOffCenter(0.0f, (float)device->getWidth(),
                    (float)device->getHeight(), 0.0f, 0.0f,
                    1000.f, &projection);
                TestBaseI::modifyProjectionBasedOnDevice(projection);
                for (int i = 0; i < TOTAL_BLEND; i++) {
                    uniformBuffer[i] = device->createBuffer(uniformBufferInfo);
                    uniformBuffer[i]->update(projection.m, sizeof(Mat4), sizeof(projection));
                }
            }

            void createInputAssembler() {
                gfx::GFXAttribute position = { "a_position", gfx::GFXFormat::RG32F, false, 0, false };
                gfx::GFXAttribute texcoord = { "a_uv", gfx::GFXFormat::RG32F, false, 0, false };
                gfx::GFXInputAssemblerInfo inputAssemblerInfo;
                inputAssemblerInfo.attributes.emplace_back(std::move(position));
                inputAssemblerInfo.attributes.emplace_back(std::move(texcoord));
                inputAssemblerInfo.vertexBuffers.emplace_back(vertexBuffer);
                inputAssemblerInfo.indexBuffer = indexBuffer;
                inputAssembler = device->createInputAssembler(inputAssemblerInfo);
            }

            void createTexture() {
                gfx::GFXTextureInfo textureInfo;
                textureInfo.usage = gfx::GFXTextureUsage::SAMPLED;
                textureInfo.format = gfx::GFXFormat::RGBA8;
                textureInfo.width = 128;
                textureInfo.height = 128;
                texture = ::cc::createTexture(device, textureInfo, "sprite0.png");

                // create sampler
                gfx::GFXSamplerInfo samplerInfo;
                samplerInfo.addressU = gfx::GFXAddress::CLAMP;
                samplerInfo.addressV = gfx::GFXAddress::CLAMP;
                sampler = device->createSampler(samplerInfo);
            }

            void createPipeline() {
                gfx::GFXBindingList bindingList = {
                    {gfx::GFXShaderType::VERTEX, 0, gfx::GFXBindingType::UNIFORM_BUFFER, "MVP_Matrix",
                     1},
                    {gfx::GFXShaderType::FRAGMENT, 1, gfx::GFXBindingType::SAMPLER, "u_texture", 1} };
                gfx::GFXBindingLayoutInfo bindingLayoutInfo = { bindingList };

                for (int i = 0; i < TOTAL_BLEND; i++) {
                    bindingLayout[i] = device->createBindingLayout(bindingLayoutInfo);

                    bindingLayout[i]->bindBuffer(0, uniformBuffer[i]);
                    bindingLayout[i]->bindSampler(1, sampler);
                    bindingLayout[i]->bindTexture(1, texture);
                    bindingLayout[i]->update();

                    gfx::GFXPipelineLayoutInfo pipelineLayoutInfo;
                    pipelineLayoutInfo.layouts = { bindingLayout[i] };
                    pipelineLayout[i] = device->createPipelineLayout(pipelineLayoutInfo);
                }

                gfx::GFXPipelineStateInfo pipelineInfo[TOTAL_BLEND];
                pipelineInfo[NO_BLEND].primitive = gfx::GFXPrimitiveMode::TRIANGLE_LIST;
                pipelineInfo[NO_BLEND].shader = shader;
                pipelineInfo[NO_BLEND].inputState = { inputAssembler->getAttributes() };
                pipelineInfo[NO_BLEND].layout = pipelineLayout[NO_BLEND];
                pipelineInfo[NO_BLEND].renderPass = fbo->getRenderPass();
                pipelineInfo[NO_BLEND].rasterizerState.cullMode = gfx::GFXCullMode::NONE;
                pipelineInfo[NO_BLEND].depthStencilState.depthWrite = false;
                pipelineInfo[NO_BLEND].blendState.targets[0].blend = true;
                pipelineInfo[NO_BLEND].blendState.targets[0].blendEq = gfx::GFXBlendOp::ADD;
                pipelineInfo[NO_BLEND].blendState.targets[0].blendAlphaEq = gfx::GFXBlendOp::ADD;
                pipelineInfo[NO_BLEND].blendState.targets[0].blendSrc = gfx::GFXBlendFactor::ONE;
                pipelineInfo[NO_BLEND].blendState.targets[0].blendDst =
                    gfx::GFXBlendFactor::ZERO;
                pipelineInfo[NO_BLEND].blendState.targets[0].blendSrcAlpha =
                    gfx::GFXBlendFactor::ONE;
                pipelineInfo[NO_BLEND].blendState.targets[0].blendDstAlpha =
                    gfx::GFXBlendFactor::ONE_MINUS_SRC_ALPHA;
                pipelineState[NO_BLEND] =
                    device->createPipelineState(pipelineInfo[NO_BLEND]);

                pipelineInfo[NORMAL_BLEND].primitive = gfx::GFXPrimitiveMode::TRIANGLE_LIST;
                pipelineInfo[NORMAL_BLEND].shader = shader;
                pipelineInfo[NORMAL_BLEND].inputState = { inputAssembler->getAttributes() };
                pipelineInfo[NORMAL_BLEND].layout = pipelineLayout[NORMAL_BLEND];
                pipelineInfo[NORMAL_BLEND].renderPass = fbo->getRenderPass();
                pipelineInfo[NORMAL_BLEND].rasterizerState.cullMode = gfx::GFXCullMode::NONE;
                pipelineInfo[NORMAL_BLEND].depthStencilState.depthWrite = false;
                pipelineInfo[NORMAL_BLEND].blendState.targets[0].blend = true;
                pipelineInfo[NORMAL_BLEND].blendState.targets[0].blendEq = gfx::GFXBlendOp::ADD;
                pipelineInfo[NORMAL_BLEND].blendState.targets[0].blendAlphaEq =
                    gfx::GFXBlendOp::ADD;
                pipelineInfo[NORMAL_BLEND].blendState.targets[0].blendSrc =
                    gfx::GFXBlendFactor::SRC_ALPHA;
                pipelineInfo[NORMAL_BLEND].blendState.targets[0].blendDst =
                    gfx::GFXBlendFactor::ONE_MINUS_SRC_ALPHA;
                pipelineInfo[NORMAL_BLEND].blendState.targets[0].blendSrcAlpha =
                    gfx::GFXBlendFactor::ONE;
                pipelineInfo[NORMAL_BLEND].blendState.targets[0].blendDstAlpha =
                    gfx::GFXBlendFactor::ONE_MINUS_SRC_ALPHA;
                pipelineState[NORMAL_BLEND] =
                    device->createPipelineState(pipelineInfo[NORMAL_BLEND]);

                pipelineInfo[ADDITIVE_BLEND].primitive = gfx::GFXPrimitiveMode::TRIANGLE_LIST;
                pipelineInfo[ADDITIVE_BLEND].shader = shader;
                pipelineInfo[ADDITIVE_BLEND].inputState = { inputAssembler->getAttributes() };
                pipelineInfo[ADDITIVE_BLEND].layout = pipelineLayout[ADDITIVE_BLEND];
                pipelineInfo[ADDITIVE_BLEND].renderPass = fbo->getRenderPass();
                pipelineInfo[ADDITIVE_BLEND].rasterizerState.cullMode = gfx::GFXCullMode::NONE;
                pipelineInfo[ADDITIVE_BLEND].depthStencilState.depthWrite = false;
                pipelineInfo[ADDITIVE_BLEND].blendState.targets[0].blend = true;
                pipelineInfo[ADDITIVE_BLEND].blendState.targets[0].blendEq =
                    gfx::GFXBlendOp::ADD;
                pipelineInfo[ADDITIVE_BLEND].blendState.targets[0].blendAlphaEq =
                    gfx::GFXBlendOp::ADD;
                pipelineInfo[ADDITIVE_BLEND].blendState.targets[0].blendSrc =
                    gfx::GFXBlendFactor::SRC_ALPHA;
                pipelineInfo[ADDITIVE_BLEND].blendState.targets[0].blendDst =
                    gfx::GFXBlendFactor::ONE;
                pipelineInfo[ADDITIVE_BLEND].blendState.targets[0].blendSrcAlpha =
                    gfx::GFXBlendFactor::ONE;
                pipelineInfo[ADDITIVE_BLEND].blendState.targets[0].blendDstAlpha =
                    gfx::GFXBlendFactor::ONE_MINUS_SRC_ALPHA;
                pipelineState[ADDITIVE_BLEND] =
                    device->createPipelineState(pipelineInfo[ADDITIVE_BLEND]);

                pipelineInfo[SUBSTRACT_BLEND].primitive = gfx::GFXPrimitiveMode::TRIANGLE_LIST;
                pipelineInfo[SUBSTRACT_BLEND].shader = shader;
                pipelineInfo[SUBSTRACT_BLEND].inputState = {
                    inputAssembler->getAttributes() };
                pipelineInfo[SUBSTRACT_BLEND].layout = pipelineLayout[SUBSTRACT_BLEND];
                pipelineInfo[SUBSTRACT_BLEND].renderPass = fbo->getRenderPass();
                pipelineInfo[SUBSTRACT_BLEND].rasterizerState.cullMode = gfx::GFXCullMode::NONE;
                pipelineInfo[SUBSTRACT_BLEND].depthStencilState.depthWrite = false;
                pipelineInfo[SUBSTRACT_BLEND].blendState.targets[0].blend = true;
                pipelineInfo[SUBSTRACT_BLEND].blendState.targets[0].blendEq =
                    gfx::GFXBlendOp::ADD;
                pipelineInfo[SUBSTRACT_BLEND].blendState.targets[0].blendAlphaEq =
                    gfx::GFXBlendOp::ADD;
                pipelineInfo[SUBSTRACT_BLEND].blendState.targets[0].blendSrc =
                    gfx::GFXBlendFactor::ZERO;
                pipelineInfo[SUBSTRACT_BLEND].blendState.targets[0].blendDst =
                    gfx::GFXBlendFactor::ONE_MINUS_SRC_COLOR;
                pipelineInfo[SUBSTRACT_BLEND].blendState.targets[0].blendSrcAlpha =
                    gfx::GFXBlendFactor::ONE;
                pipelineInfo[SUBSTRACT_BLEND].blendState.targets[0].blendDstAlpha =
                    gfx::GFXBlendFactor::ONE_MINUS_SRC_ALPHA;
                pipelineState[SUBSTRACT_BLEND] =
                    device->createPipelineState(pipelineInfo[SUBSTRACT_BLEND]);

                pipelineInfo[MULTIPLY_BLEND].primitive = gfx::GFXPrimitiveMode::TRIANGLE_LIST;
                pipelineInfo[MULTIPLY_BLEND].shader = shader;
                pipelineInfo[MULTIPLY_BLEND].inputState = { inputAssembler->getAttributes() };
                pipelineInfo[MULTIPLY_BLEND].layout = pipelineLayout[MULTIPLY_BLEND];
                pipelineInfo[MULTIPLY_BLEND].renderPass = fbo->getRenderPass();
                pipelineInfo[MULTIPLY_BLEND].rasterizerState.cullMode = gfx::GFXCullMode::NONE;
                pipelineInfo[MULTIPLY_BLEND].depthStencilState.depthWrite = false;
                pipelineInfo[MULTIPLY_BLEND].blendState.targets[0].blend = true;
                pipelineInfo[MULTIPLY_BLEND].blendState.targets[0].blendEq =
                    gfx::GFXBlendOp::ADD;
                pipelineInfo[MULTIPLY_BLEND].blendState.targets[0].blendAlphaEq =
                    gfx::GFXBlendOp::ADD;
                pipelineInfo[MULTIPLY_BLEND].blendState.targets[0].blendSrc =
                    gfx::GFXBlendFactor::ZERO;
                pipelineInfo[MULTIPLY_BLEND].blendState.targets[0].blendDst =
                    gfx::GFXBlendFactor::SRC_COLOR;
                pipelineInfo[MULTIPLY_BLEND].blendState.targets[0].blendSrcAlpha =
                    gfx::GFXBlendFactor::ONE;
                pipelineInfo[MULTIPLY_BLEND].blendState.targets[0].blendDstAlpha =
                    gfx::GFXBlendFactor::ONE_MINUS_SRC_ALPHA;
                pipelineState[MULTIPLY_BLEND] =
                    device->createPipelineState(pipelineInfo[MULTIPLY_BLEND]);
            }

            gfx::GFXDevice *device = nullptr;
            gfx::GFXFramebuffer *fbo = nullptr;
            gfx::GFXShader *shader = nullptr;
            gfx::GFXBuffer *vertexBuffer = nullptr;
            gfx::GFXBuffer *indexBuffer = nullptr;
            gfx::GFXInputAssembler *inputAssembler = nullptr;
            gfx::GFXSampler *sampler = nullptr;
            gfx::GFXTexture *texture = nullptr;
            gfx::GFXBuffer *uniformBuffer[TOTAL_BLEND] = { nullptr };
            gfx::GFXBindingLayout *bindingLayout[TOTAL_BLEND] = { nullptr };
            gfx::GFXPipelineLayout *pipelineLayout[TOTAL_BLEND] = { nullptr };
            gfx::GFXPipelineState *pipelineState[TOTAL_BLEND] = { nullptr };

            Mat4 model;
        };

        struct BigTriangle : public gfx::Object {
            BigTriangle(gfx::GFXDevice *_device, gfx::GFXFramebuffer* _fbo) : device(_device), fbo(_fbo) {
                createShader();
                createVertexBuffer();
                createInputAssembler();
                createTexture();
                createPipeline();
            }

            void destroy() {
                CC_SAFE_DESTROY(shader);
                CC_SAFE_DESTROY(vertexBuffer);
                CC_SAFE_DESTROY(inputAssembler);
                CC_SAFE_DESTROY(pipelineState);
                CC_SAFE_DESTROY(pipelineLayout);
                CC_SAFE_DESTROY(bindingLayout);
                CC_SAFE_DESTROY(timeBuffer);
                CC_SAFE_DESTROY(texture);
                CC_SAFE_DESTROY(sampler);
            }

            void createShader() {
                gfx::GFXShaderStageList shaderStageList;
                gfx::GFXShaderStage vertexShaderStage;
                vertexShaderStage.type = gfx::GFXShaderType::VERTEX;

#if defined(USE_VULKAN) | defined(USE_METAL)
                vertexShaderStage.source = R"(
            layout(location = 0) in vec2 a_position;
            layout(location = 0) out vec2 uv;
            void main()
            {
                uv = (a_position + 1.0) * 0.5;
                gl_Position = vec4(a_position, 0.1, 1);
            }
            )";
#elif defined(USE_GLES2)
                vertexShaderStage.source = R"(
            attribute vec2 a_position;
            varying vec2 uv;
            void main()
            {
                uv = (a_position + 1.0) * 0.5;
                gl_Position = vec4(a_position, 0.1, 1);
            }
            )";
#else
                vertexShaderStage.source = R"(
            in vec2 a_position;
            out vec2 uv;
            void main()
            {
                uv = (a_position + 1.0) * 0.5;
                gl_Position = vec4(a_position, 0.1, 1);
            }
            )";
#endif // USE_GLES2

                shaderStageList.emplace_back(std::move(vertexShaderStage));

                gfx::GFXShaderStage fragmentShaderStage;
                fragmentShaderStage.type = gfx::GFXShaderType::FRAGMENT;

#if defined(USE_VULKAN) | defined(USE_METAL)
                fragmentShaderStage.source = R"(
            #ifdef GL_ES
                        precision highp float;
            #endif
            layout(location = 0) in vec2 uv;
            layout(binding = 0) uniform Time
            {
                float u_time;
            };
            layout(binding = 1) uniform sampler2D u_texture;
            layout(location = 0) out vec4 o_color;
            void main()
            {
                vec2 offset = vec2(u_time * -0.01);
                o_color = texture(u_texture, 20.0 * (uv + offset));
            }
            )";
#elif defined(USE_GLES2)
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
                fragmentShaderStage.source = R"(
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

                gfx::GFXAttributeList attributeList = { { "a_position", gfx::GFXFormat::RG32F, false, 0, false, 0 } };
                gfx::GFXUniformList time = { {"u_time", gfx::GFXType::FLOAT, 1} };
                gfx::GFXUniformBlockList uniformBlockList = {
                    {gfx::GFXShaderType::FRAGMENT, 0, "Time", time} };

                gfx::GFXUniformSamplerList samplers = {
                    {gfx::GFXShaderType::FRAGMENT, 1, "u_texture", gfx::GFXType::SAMPLER2D, 1} };

                gfx::GFXShaderInfo shaderInfo;
                shaderInfo.name = "Blend Test: BigTriangle";
                shaderInfo.stages = std::move(shaderStageList);
                shaderInfo.attributes = std::move(attributeList);
                shaderInfo.blocks = std::move(uniformBlockList);
                shaderInfo.samplers = std::move(samplers);
                shader = device->createShader(shaderInfo);
            }

            void createVertexBuffer() {
                float ySign = device->getProjectionSignY();
                float vertexData[] = { -1.0f,         4.0f * ySign, -1.0f,
                                      -1.0f * ySign, 4.0f,         -1.0f * ySign };

                // vertex buffer
                gfx::GFXBufferInfo vertexBufferInfo = {
                    gfx::GFXBufferUsage::VERTEX, gfx::GFXMemoryUsage::HOST, 2 * sizeof(float),
                    sizeof(vertexData), gfx::GFXBufferFlagBit::NONE };

                vertexBuffer = device->createBuffer(vertexBufferInfo);
                vertexBuffer->update(vertexData, 0, sizeof(vertexData));

                // uniform buffer
                gfx::GFXBufferInfo uniformBufferInfo = {
                    gfx::GFXBufferUsage::UNIFORM, gfx::GFXMemoryUsage::HOST | gfx::GFXMemoryUsage::DEVICE,
                    sizeof(float), sizeof(float), gfx::GFXBufferFlagBit::NONE };
                timeBuffer = device->createBuffer(uniformBufferInfo);
            }

            void createInputAssembler() {
                gfx::GFXAttribute position = { "a_position", gfx::GFXFormat::RG32F, false, 0, false };
                gfx::GFXInputAssemblerInfo inputAssemblerInfo;
                inputAssemblerInfo.attributes.emplace_back(std::move(position));
                inputAssemblerInfo.vertexBuffers.emplace_back(vertexBuffer);
                inputAssembler = device->createInputAssembler(inputAssemblerInfo);
            }

            void createTexture() {
                gfx::GFXTextureInfo textureInfo;
                textureInfo.usage = gfx::GFXTextureUsage::SAMPLED;
                textureInfo.format = gfx::GFXFormat::RGBA8;
                textureInfo.width = 128;
                textureInfo.height = 128;
                textureInfo.flags = gfx::GFXTextureFlagBit::GEN_MIPMAP;
                textureInfo.mipLevel =
                    TestBaseI::getMipmapLevelCounts(textureInfo.width, textureInfo.height);
                texture = ::cc::createTexture(device, textureInfo, "background.png");

                // create sampler
                gfx::GFXSamplerInfo samplerInfo;
                samplerInfo.name = "Background Generate mipmap";
                samplerInfo.mipFilter = gfx::GFXFilter::LINEAR;
                samplerInfo.addressU = gfx::GFXAddress::WRAP;
                samplerInfo.addressV = gfx::GFXAddress::WRAP;
                sampler = device->createSampler(samplerInfo);
            }

            void createPipeline() {
                gfx::GFXBindingList bindingList = {
                    {gfx::GFXShaderType::FRAGMENT, 0, gfx::GFXBindingType::UNIFORM_BUFFER, "Time", 1},
                    {gfx::GFXShaderType::FRAGMENT, 1, gfx::GFXBindingType::SAMPLER, "u_texture", 1},
                };
                gfx::GFXBindingLayoutInfo bindingLayoutInfo = { bindingList };
                bindingLayout = device->createBindingLayout(bindingLayoutInfo);

                bindingLayout->bindBuffer(0, timeBuffer);
                bindingLayout->bindSampler(1, sampler);
                bindingLayout->bindTexture(1, texture);
                bindingLayout->update();

                gfx::GFXPipelineLayoutInfo pipelineLayoutInfo;
                pipelineLayoutInfo.layouts = { bindingLayout };
                pipelineLayout = device->createPipelineLayout(pipelineLayoutInfo);

                gfx::GFXPipelineStateInfo pipelineInfo;
                pipelineInfo.primitive = gfx::GFXPrimitiveMode::TRIANGLE_LIST;
                pipelineInfo.shader = shader;
                pipelineInfo.inputState = { inputAssembler->getAttributes() };
                pipelineInfo.layout = pipelineLayout;
                pipelineInfo.renderPass = fbo->getRenderPass();

                pipelineState = device->createPipelineState(pipelineInfo);
            }

            gfx::GFXDevice *device = nullptr;
            gfx::GFXFramebuffer* fbo = nullptr;
            gfx::GFXShader *shader = nullptr;
            gfx::GFXBuffer *vertexBuffer = nullptr;
            gfx::GFXBuffer *timeBuffer = nullptr;
            gfx::GFXInputAssembler *inputAssembler = nullptr;
            gfx::GFXSampler *sampler = nullptr;
            gfx::GFXBindingLayout *bindingLayout = nullptr;
            gfx::GFXTexture *texture = nullptr;
            gfx::GFXPipelineState *pipelineState = nullptr;
            gfx::GFXPipelineLayout *pipelineLayout = nullptr;
        };

        Mat4 createModelTransform(const Vec3 &t, const Vec3 &s) {
            Mat4 model;
            model.translate(t);
            model.scale(s);

            return model;
        }

        BigTriangle *bigTriangle = nullptr;
        Quad *quad = nullptr;
    } // namespace

    void BlendTest::destroy() {
        CC_SAFE_DESTROY(bigTriangle);
        CC_SAFE_DESTROY(quad);
    }

    bool BlendTest::initialize() {
        bigTriangle = CC_NEW(BigTriangle(_device, _fbo));
        quad = CC_NEW(Quad(_device, _fbo));
        return true;
    }

    void BlendTest::tick(float dt) {

        _dt += dt;

        gfx::GFXRect render_area = { 0, 0, _device->getWidth(), _device->getHeight() };
        gfx::GFXColor clear_color = { 0.0f, 0, 0, 1.0f };

        _device->acquire();

        auto commandBuffer = _commandBuffers[0];
        commandBuffer->begin();
        commandBuffer->beginRenderPass(
            _fbo, render_area, gfx::GFXClearFlagBit::ALL,
            std::move(std::vector<gfx::GFXColor>({ clear_color })), 1.0f, 0);

        // draw background
        bigTriangle->timeBuffer->update(&_dt, 0, sizeof(_dt));
        commandBuffer->bindInputAssembler(bigTriangle->inputAssembler);
        commandBuffer->bindPipelineState(bigTriangle->pipelineState);
        commandBuffer->bindBindingLayout(bigTriangle->bindingLayout);
        commandBuffer->draw(bigTriangle->inputAssembler);

        commandBuffer->bindInputAssembler(quad->inputAssembler);

        // draw sprite without blending
        float size = std::min(render_area.width, render_area.height) * 0.15f;
        float halfSize = size * 0.5f;
        float offsetX = 5.f + halfSize;
        float offsetY = 50.f + halfSize;
        quad->model = std::move(
            createModelTransform(Vec3(offsetX, offsetY, 0), Vec3(size, size, 1)));
        quad->uniformBuffer[NO_BLEND]->update(quad->model.m, 0,
            sizeof(quad->model));
        commandBuffer->bindPipelineState(quad->pipelineState[NO_BLEND]);
        commandBuffer->bindBindingLayout(quad->bindingLayout[NO_BLEND]);
        commandBuffer->draw(quad->inputAssembler);

        // normal
        offsetY += 5.f + size;
        quad->model = std::move(createModelTransform(
            cc::Vec3(offsetX, offsetY, 0), cc::Vec3(size, size, 1)));
        quad->uniformBuffer[NORMAL_BLEND]->update(quad->model.m, 0,
            sizeof(quad->model));
        commandBuffer->bindPipelineState(quad->pipelineState[NORMAL_BLEND]);
        commandBuffer->bindBindingLayout(quad->bindingLayout[NORMAL_BLEND]);
        commandBuffer->draw(quad->inputAssembler);

        // additive
        offsetY += 5.f + size;
        quad->model = std::move(createModelTransform(
            cc::Vec3(offsetX, offsetY, 0), cc::Vec3(size, size, 1)));
        quad->uniformBuffer[ADDITIVE_BLEND]->update(quad->model.m, 0,
            sizeof(quad->model));
        commandBuffer->bindPipelineState(quad->pipelineState[ADDITIVE_BLEND]);
        commandBuffer->bindBindingLayout(quad->bindingLayout[ADDITIVE_BLEND]);
        commandBuffer->draw(quad->inputAssembler);

        // substract
        offsetY += 5.f + size;
        quad->model = std::move(createModelTransform(
            cc::Vec3(offsetX, offsetY, 0), cc::Vec3(size, size, 1)));
        quad->uniformBuffer[SUBSTRACT_BLEND]->update(quad->model.m, 0,
            sizeof(quad->model));
        commandBuffer->bindPipelineState(quad->pipelineState[SUBSTRACT_BLEND]);
        commandBuffer->bindBindingLayout(quad->bindingLayout[SUBSTRACT_BLEND]);
        commandBuffer->draw(quad->inputAssembler);

        // multiply
        offsetY += 5.f + size;
        quad->model = std::move(createModelTransform(
            cc::Vec3(offsetX, offsetY, 0), cc::Vec3(size, size, 1)));
        quad->uniformBuffer[MULTIPLY_BLEND]->update(quad->model.m, 0,
            sizeof(quad->model));
        commandBuffer->bindPipelineState(quad->pipelineState[MULTIPLY_BLEND]);
        commandBuffer->bindBindingLayout(quad->bindingLayout[MULTIPLY_BLEND]);
        commandBuffer->draw(quad->inputAssembler);

        commandBuffer->endRenderPass();
        commandBuffer->end();
        
        _device->getQueue()->submit(_commandBuffers);
        _device->present();
    }

} // namespace cc
