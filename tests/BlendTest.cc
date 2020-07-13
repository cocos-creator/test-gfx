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

        gfx::Texture *createTexture(gfx::Device *device, const gfx::TextureInfo &textureInfo,
            std::string imageFile) {
            auto img = new cc::Image();
            img->autorelease();
            bool valid = img->initWithImageFile(imageFile);
            CCASSERT(valid, "load image failed");
            auto imgData = img->getRenderFormat() == gfx::Format::RGB8
                ? TestBaseI::RGB2RGBA(img)
                : img->getData();

            auto texture = device->createTexture(textureInfo);

            gfx::BufferTextureCopy textureRegion;
            textureRegion.texExtent.width = img->getWidth();
            textureRegion.texExtent.height = img->getHeight();
            textureRegion.texExtent.depth = 1;

            gfx::BufferTextureCopyList regions;
            regions.push_back(std::move(textureRegion));

            gfx::BufferDataList imageBuffers = {imgData};
            device->copyBuffersToTexture(imageBuffers, texture, regions);
            delete[] imgData;
            return texture;
        }

        struct Quad : public gfx::Object {
            Quad(gfx::Device *_device, gfx::Framebuffer *_fbo) : device(_device), fbo(_fbo) {
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
                    CC_SAFE_DESTROY(bindingLayout[i]);
                    CC_SAFE_DESTROY(uniformBuffer[i]);
                }
            }

            void createShader() {
                gfx::ShaderStageList shaderStageList;
                gfx::ShaderStage vertexShaderStage;
                vertexShaderStage.type = gfx::ShaderType::VERTEX;

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

                gfx::ShaderStage fragmentShaderStage;
                fragmentShaderStage.type = gfx::ShaderType::FRAGMENT;

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

                gfx::AttributeList attributeList = {
                    { "a_position", gfx::Format::RG32F, false, 0, false, 0 },
                    { "a_uv", gfx::Format::RG32F, false, 0, false, 1 },
                };
                gfx::UniformList mvpMatrix = {
                    {"u_model", gfx::Type::MAT4, 1},
                    {"u_projection", gfx::Type::MAT4, 1},
                };
                gfx::UniformBlockList uniformBlockList = {
                    {gfx::ShaderType::VERTEX, 0, "MVP_Matrix", mvpMatrix} };

                gfx::UniformSamplerList samplers = {
                    {gfx::ShaderType::FRAGMENT, 1, "u_texture", gfx::Type::SAMPLER2D, 1} };

                gfx::ShaderInfo shaderInfo;
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
                gfx::BufferInfo vertexBufferInfo = {
                    gfx::BufferUsage::VERTEX, gfx::MemoryUsage::DEVICE, 4 * sizeof(float),
                    sizeof(vertexData), gfx::BufferFlagBit::NONE };

                vertexBuffer = device->createBuffer(vertexBufferInfo);
                vertexBuffer->update(vertexData, 0, sizeof(vertexData));

                // index buffer
                gfx::BufferInfo indexBufferInfo = {
                    gfx::BufferUsage::INDEX, gfx::MemoryUsage::DEVICE, sizeof(unsigned short),
                    sizeof(indices), gfx::BufferFlagBit::NONE };
                indexBuffer = device->createBuffer(indexBufferInfo);
                indexBuffer->update(indices, 0, sizeof(indices));

                // uniform buffer
                gfx::BufferInfo uniformBufferInfo = {
                    gfx::BufferUsage::UNIFORM, gfx::MemoryUsage::DEVICE, sizeof(Mat4),
                    2 * sizeof(Mat4), gfx::BufferFlagBit::NONE };

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
                gfx::Attribute position = { "a_position", gfx::Format::RG32F, false, 0, false };
                gfx::Attribute texcoord = { "a_uv", gfx::Format::RG32F, false, 0, false };
                gfx::InputAssemblerInfo inputAssemblerInfo;
                inputAssemblerInfo.attributes.emplace_back(std::move(position));
                inputAssemblerInfo.attributes.emplace_back(std::move(texcoord));
                inputAssemblerInfo.vertexBuffers.emplace_back(vertexBuffer);
                inputAssemblerInfo.indexBuffer = indexBuffer;
                inputAssembler = device->createInputAssembler(inputAssemblerInfo);
            }

            void createTexture() {
                gfx::TextureInfo textureInfo;
                textureInfo.usage = gfx::TextureUsage::SAMPLED | gfx::TextureUsage::TRANSFER_DST;
                textureInfo.format = gfx::Format::RGBA8;
                textureInfo.width = 128;
                textureInfo.height = 128;
                texture = cc::createTexture(device, textureInfo, "sprite0.png");

                // create sampler
                gfx::SamplerInfo samplerInfo;
                samplerInfo.addressU = gfx::Address::CLAMP;
                samplerInfo.addressV = gfx::Address::CLAMP;
                sampler = device->createSampler(samplerInfo);
            }

            void createPipeline() {
                gfx::BindingLayoutInfo bindingLayoutInfo = { shader };

                for (int i = 0; i < TOTAL_BLEND; i++) {
                    bindingLayout[i] = device->createBindingLayout(bindingLayoutInfo);

                    bindingLayout[i]->bindBuffer(0, uniformBuffer[i]);
                    bindingLayout[i]->bindSampler(1, sampler);
                    bindingLayout[i]->bindTexture(1, texture);
                    bindingLayout[i]->update();
                }

                gfx::PipelineStateInfo pipelineInfo[TOTAL_BLEND];
                pipelineInfo[NO_BLEND].primitive = gfx::PrimitiveMode::TRIANGLE_LIST;
                pipelineInfo[NO_BLEND].shader = shader;
                pipelineInfo[NO_BLEND].inputState = { inputAssembler->getAttributes() };
                pipelineInfo[NO_BLEND].renderPass = fbo->getRenderPass();
                pipelineInfo[NO_BLEND].rasterizerState.cullMode = gfx::CullMode::NONE;
                pipelineInfo[NO_BLEND].depthStencilState.depthWrite = false;
                pipelineInfo[NO_BLEND].blendState.targets[0].blend = true;
                pipelineInfo[NO_BLEND].blendState.targets[0].blendEq = gfx::BlendOp::ADD;
                pipelineInfo[NO_BLEND].blendState.targets[0].blendAlphaEq = gfx::BlendOp::ADD;
                pipelineInfo[NO_BLEND].blendState.targets[0].blendSrc = gfx::BlendFactor::ONE;
                pipelineInfo[NO_BLEND].blendState.targets[0].blendDst = gfx::BlendFactor::ZERO;
                pipelineInfo[NO_BLEND].blendState.targets[0].blendSrcAlpha = gfx::BlendFactor::ONE;
                pipelineInfo[NO_BLEND].blendState.targets[0].blendDstAlpha = gfx::BlendFactor::ONE_MINUS_SRC_ALPHA;
                pipelineState[NO_BLEND] = device->createPipelineState(pipelineInfo[NO_BLEND]);

                pipelineInfo[NORMAL_BLEND].primitive = gfx::PrimitiveMode::TRIANGLE_LIST;
                pipelineInfo[NORMAL_BLEND].shader = shader;
                pipelineInfo[NORMAL_BLEND].inputState = { inputAssembler->getAttributes() };
                pipelineInfo[NORMAL_BLEND].renderPass = fbo->getRenderPass();
                pipelineInfo[NORMAL_BLEND].rasterizerState.cullMode = gfx::CullMode::NONE;
                pipelineInfo[NORMAL_BLEND].depthStencilState.depthWrite = false;
                pipelineInfo[NORMAL_BLEND].blendState.targets[0].blend = true;
                pipelineInfo[NORMAL_BLEND].blendState.targets[0].blendEq = gfx::BlendOp::ADD;
                pipelineInfo[NORMAL_BLEND].blendState.targets[0].blendAlphaEq = gfx::BlendOp::ADD;
                pipelineInfo[NORMAL_BLEND].blendState.targets[0].blendSrc = gfx::BlendFactor::SRC_ALPHA;
                pipelineInfo[NORMAL_BLEND].blendState.targets[0].blendDst = gfx::BlendFactor::ONE_MINUS_SRC_ALPHA;
                pipelineInfo[NORMAL_BLEND].blendState.targets[0].blendSrcAlpha = gfx::BlendFactor::ONE;
                pipelineInfo[NORMAL_BLEND].blendState.targets[0].blendDstAlpha = gfx::BlendFactor::ONE_MINUS_SRC_ALPHA;
                pipelineState[NORMAL_BLEND] = device->createPipelineState(pipelineInfo[NORMAL_BLEND]);

                pipelineInfo[ADDITIVE_BLEND].primitive = gfx::PrimitiveMode::TRIANGLE_LIST;
                pipelineInfo[ADDITIVE_BLEND].shader = shader;
                pipelineInfo[ADDITIVE_BLEND].inputState = { inputAssembler->getAttributes() };
                pipelineInfo[ADDITIVE_BLEND].renderPass = fbo->getRenderPass();
                pipelineInfo[ADDITIVE_BLEND].rasterizerState.cullMode = gfx::CullMode::NONE;
                pipelineInfo[ADDITIVE_BLEND].depthStencilState.depthWrite = false;
                pipelineInfo[ADDITIVE_BLEND].blendState.targets[0].blend = true;
                pipelineInfo[ADDITIVE_BLEND].blendState.targets[0].blendEq = gfx::BlendOp::ADD;
                pipelineInfo[ADDITIVE_BLEND].blendState.targets[0].blendAlphaEq = gfx::BlendOp::ADD;
                pipelineInfo[ADDITIVE_BLEND].blendState.targets[0].blendSrc = gfx::BlendFactor::SRC_ALPHA;
                pipelineInfo[ADDITIVE_BLEND].blendState.targets[0].blendDst = gfx::BlendFactor::ONE;
                pipelineInfo[ADDITIVE_BLEND].blendState.targets[0].blendSrcAlpha = gfx::BlendFactor::ONE;
                pipelineInfo[ADDITIVE_BLEND].blendState.targets[0].blendDstAlpha = gfx::BlendFactor::ONE_MINUS_SRC_ALPHA;
                pipelineState[ADDITIVE_BLEND] = device->createPipelineState(pipelineInfo[ADDITIVE_BLEND]);

                pipelineInfo[SUBSTRACT_BLEND].primitive = gfx::PrimitiveMode::TRIANGLE_LIST;
                pipelineInfo[SUBSTRACT_BLEND].shader = shader;
                pipelineInfo[SUBSTRACT_BLEND].inputState = { inputAssembler->getAttributes() };
                pipelineInfo[SUBSTRACT_BLEND].renderPass = fbo->getRenderPass();
                pipelineInfo[SUBSTRACT_BLEND].rasterizerState.cullMode = gfx::CullMode::NONE;
                pipelineInfo[SUBSTRACT_BLEND].depthStencilState.depthWrite = false;
                pipelineInfo[SUBSTRACT_BLEND].blendState.targets[0].blend = true;
                pipelineInfo[SUBSTRACT_BLEND].blendState.targets[0].blendEq = gfx::BlendOp::ADD;
                pipelineInfo[SUBSTRACT_BLEND].blendState.targets[0].blendAlphaEq = gfx::BlendOp::ADD;
                pipelineInfo[SUBSTRACT_BLEND].blendState.targets[0].blendSrc = gfx::BlendFactor::ZERO;
                pipelineInfo[SUBSTRACT_BLEND].blendState.targets[0].blendDst = gfx::BlendFactor::ONE_MINUS_SRC_COLOR;
                pipelineInfo[SUBSTRACT_BLEND].blendState.targets[0].blendSrcAlpha = gfx::BlendFactor::ONE;
                pipelineInfo[SUBSTRACT_BLEND].blendState.targets[0].blendDstAlpha = gfx::BlendFactor::ONE_MINUS_SRC_ALPHA;
                pipelineState[SUBSTRACT_BLEND] = device->createPipelineState(pipelineInfo[SUBSTRACT_BLEND]);

                pipelineInfo[MULTIPLY_BLEND].primitive = gfx::PrimitiveMode::TRIANGLE_LIST;
                pipelineInfo[MULTIPLY_BLEND].shader = shader;
                pipelineInfo[MULTIPLY_BLEND].inputState = { inputAssembler->getAttributes() };
                pipelineInfo[MULTIPLY_BLEND].renderPass = fbo->getRenderPass();
                pipelineInfo[MULTIPLY_BLEND].rasterizerState.cullMode = gfx::CullMode::NONE;
                pipelineInfo[MULTIPLY_BLEND].depthStencilState.depthWrite = false;
                pipelineInfo[MULTIPLY_BLEND].blendState.targets[0].blend = true;
                pipelineInfo[MULTIPLY_BLEND].blendState.targets[0].blendEq = gfx::BlendOp::ADD;
                pipelineInfo[MULTIPLY_BLEND].blendState.targets[0].blendAlphaEq = gfx::BlendOp::ADD;
                pipelineInfo[MULTIPLY_BLEND].blendState.targets[0].blendSrc = gfx::BlendFactor::ZERO;
                pipelineInfo[MULTIPLY_BLEND].blendState.targets[0].blendDst = gfx::BlendFactor::SRC_COLOR;
                pipelineInfo[MULTIPLY_BLEND].blendState.targets[0].blendSrcAlpha = gfx::BlendFactor::ONE;
                pipelineInfo[MULTIPLY_BLEND].blendState.targets[0].blendDstAlpha = gfx::BlendFactor::ONE_MINUS_SRC_ALPHA;
                pipelineState[MULTIPLY_BLEND] = device->createPipelineState(pipelineInfo[MULTIPLY_BLEND]);
            }

            gfx::Device *device = nullptr;
            gfx::Framebuffer *fbo = nullptr;
            gfx::Shader *shader = nullptr;
            gfx::Buffer *vertexBuffer = nullptr;
            gfx::Buffer *indexBuffer = nullptr;
            gfx::InputAssembler *inputAssembler = nullptr;
            gfx::Sampler *sampler = nullptr;
            gfx::Texture *texture = nullptr;
            gfx::Buffer *uniformBuffer[TOTAL_BLEND] = { nullptr };
            gfx::BindingLayout *bindingLayout[TOTAL_BLEND] = { nullptr };
            gfx::PipelineState *pipelineState[TOTAL_BLEND] = { nullptr };

            Mat4 model;
        };

        struct BigTriangle : public gfx::Object {
            BigTriangle(gfx::Device *_device, gfx::Framebuffer* _fbo) : device(_device), fbo(_fbo) {
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
                CC_SAFE_DESTROY(bindingLayout);
                CC_SAFE_DESTROY(timeBuffer);
                CC_SAFE_DESTROY(texture);
                CC_SAFE_DESTROY(sampler);
            }

            void createShader() {
                gfx::ShaderStageList shaderStageList;
                gfx::ShaderStage vertexShaderStage;
                vertexShaderStage.type = gfx::ShaderType::VERTEX;

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

                gfx::ShaderStage fragmentShaderStage;
                fragmentShaderStage.type = gfx::ShaderType::FRAGMENT;

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

                gfx::AttributeList attributeList = { { "a_position", gfx::Format::RG32F, false, 0, false, 0 } };
                gfx::UniformList time = { {"u_time", gfx::Type::FLOAT, 1} };
                gfx::UniformBlockList uniformBlockList = {
                    {gfx::ShaderType::FRAGMENT, 0, "Time", time} };

                gfx::UniformSamplerList samplers = {
                    {gfx::ShaderType::FRAGMENT, 1, "u_texture", gfx::Type::SAMPLER2D, 1} };

                gfx::ShaderInfo shaderInfo;
                shaderInfo.name = "Blend Test: BigTriangle";
                shaderInfo.stages = std::move(shaderStageList);
                shaderInfo.attributes = std::move(attributeList);
                shaderInfo.blocks = std::move(uniformBlockList);
                shaderInfo.samplers = std::move(samplers);
                shader = device->createShader(shaderInfo);
            }

            void createVertexBuffer() {
                float ySign = device->getScreenSpaceSignY();
                float vertexData[] = { -1.0f,         4.0f * ySign, -1.0f,
                                      -1.0f * ySign, 4.0f,         -1.0f * ySign };

                // vertex buffer
                gfx::BufferInfo vertexBufferInfo = {
                    gfx::BufferUsage::VERTEX, gfx::MemoryUsage::HOST, 2 * sizeof(float),
                    sizeof(vertexData), gfx::BufferFlagBit::NONE };

                vertexBuffer = device->createBuffer(vertexBufferInfo);
                vertexBuffer->update(vertexData, 0, sizeof(vertexData));

                // uniform buffer
                gfx::BufferInfo uniformBufferInfo = {
                    gfx::BufferUsage::UNIFORM, gfx::MemoryUsage::HOST | gfx::MemoryUsage::DEVICE,
                    sizeof(float), sizeof(float), gfx::BufferFlagBit::NONE };
                timeBuffer = device->createBuffer(uniformBufferInfo);
            }

            void createInputAssembler() {
                gfx::Attribute position = { "a_position", gfx::Format::RG32F, false, 0, false };
                gfx::InputAssemblerInfo inputAssemblerInfo;
                inputAssemblerInfo.attributes.emplace_back(std::move(position));
                inputAssemblerInfo.vertexBuffers.emplace_back(vertexBuffer);
                inputAssembler = device->createInputAssembler(inputAssemblerInfo);
            }

            void createTexture() {
                gfx::TextureInfo textureInfo;
                textureInfo.usage = gfx::TextureUsage::SAMPLED | gfx::TextureUsage::TRANSFER_DST;
                textureInfo.format = gfx::Format::RGBA8;
                textureInfo.width = 128;
                textureInfo.height = 128;
                textureInfo.flags = gfx::TextureFlagBit::GEN_MIPMAP;
                textureInfo.mipLevel =
                    TestBaseI::getMipmapLevelCounts(textureInfo.width, textureInfo.height);
                texture = ::cc::createTexture(device, textureInfo, "background.png");

                // create sampler
                gfx::SamplerInfo samplerInfo;
                samplerInfo.name = "Background Generate mipmap";
                samplerInfo.mipFilter = gfx::Filter::LINEAR;
                samplerInfo.addressU = gfx::Address::WRAP;
                samplerInfo.addressV = gfx::Address::WRAP;
                sampler = device->createSampler(samplerInfo);
            }

            void createPipeline() {
                gfx::BindingLayoutInfo bindingLayoutInfo = { shader };
                bindingLayout = device->createBindingLayout(bindingLayoutInfo);

                bindingLayout->bindBuffer(0, timeBuffer);
                bindingLayout->bindSampler(1, sampler);
                bindingLayout->bindTexture(1, texture);
                bindingLayout->update();

                gfx::PipelineStateInfo pipelineInfo;
                pipelineInfo.primitive = gfx::PrimitiveMode::TRIANGLE_LIST;
                pipelineInfo.shader = shader;
                pipelineInfo.inputState = { inputAssembler->getAttributes() };
                pipelineInfo.renderPass = fbo->getRenderPass();

                pipelineState = device->createPipelineState(pipelineInfo);
            }

            gfx::Device *device = nullptr;
            gfx::Framebuffer* fbo = nullptr;
            gfx::Shader *shader = nullptr;
            gfx::Buffer *vertexBuffer = nullptr;
            gfx::Buffer *timeBuffer = nullptr;
            gfx::InputAssembler *inputAssembler = nullptr;
            gfx::Sampler *sampler = nullptr;
            gfx::BindingLayout *bindingLayout = nullptr;
            gfx::Texture *texture = nullptr;
            gfx::PipelineState *pipelineState = nullptr;
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

        gfx::Rect render_area = { 0, 0, _device->getWidth(), _device->getHeight() };
        gfx::Color clear_color = { 0.0f, 0, 0, 1.0f };

        _device->acquire();

        auto commandBuffer = _commandBuffers[0];
        commandBuffer->begin();
        commandBuffer->beginRenderPass(
            _fbo->getRenderPass(), _fbo, render_area,
            std::move(std::vector<gfx::Color>({ clear_color })), 1.0f, 0);

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
