#include "ParticleTest.h"

namespace cc {

    namespace {
        static const float quadVerts[][2] = { {-1.0f, -1.0f}, {1.0f, -1.0f}, {1.0f, 1.0f}, {-1.0f, 1.0f} };

        void fillRectWithColor(uint8_t *buf, uint32_t totalWidth, uint32_t totalHeight,
            uint32_t x, uint32_t y, uint32_t width, uint32_t height,
            uint8_t r, uint8_t g, uint8_t b) {
            assert(x + width <= totalWidth);
            assert(y + height <= totalHeight);

            uint32_t y0 = totalHeight - (y + height);
            uint32_t y1 = totalHeight - y;
            uint8_t *p;
            for (uint32_t offsetY = y0; offsetY < y1; ++offsetY) {
                for (uint32_t offsetX = x; offsetX < (x + width); ++offsetX) {
                    p = buf + (totalWidth * offsetY + offsetX) * 4;
                    *p++ = r;
                    *p++ = g;
                    *p++ = b;
                    *p++ = 255;
                }
            }
        }

        /**
         * Generates a random vector with the given scale
         * @param scale Length of the resulting vector. If ommitted, a unit vector will
         * be returned
         */
        Vec3 vec3Random(float scale /* = 1.0f */) {
            Vec3 out;
            float r = cc::rand_0_1() * 2.0f * gfx::math::PI;
            float z = (cc::rand_0_1() * 2.0f) - 1.0f;
            float zScale = sqrtf(1.0f - z * z) * scale;

            out.x = cosf(r) * zScale;
            out.y = sinf(r) * zScale;
            out.z = z * scale;
            return out;
        };

        Vec3 vec3ScaleAndAdd(const Vec3 &a, const Vec3 &b, float scale) {
            Vec3 out;
            out.x = a.x + (b.x * scale);
            out.y = a.y + (b.y * scale);
            out.z = a.z + (b.z * scale);
            return out;
        };
    } // namespace

    void ParticleTest::destroy() {
        CC_SAFE_DESTROY(_shader);
        CC_SAFE_DESTROY(_vertexBuffer);
        CC_SAFE_DESTROY(_indexBuffer);
        CC_SAFE_DESTROY(_inputAssembler);
        CC_SAFE_DESTROY(_pipelineState);
        CC_SAFE_DESTROY(_descriptorSet);
        CC_SAFE_DESTROY(_descriptorSetLayout);
        CC_SAFE_DESTROY(_pipelineLayout);
        CC_SAFE_DESTROY(_uniformBuffer);
        CC_SAFE_DESTROY(_texture);
        CC_SAFE_DESTROY(_sampler);
    }

    bool ParticleTest::initialize() {
        createShader();
        createVertexBuffer();
        createInputAssembler();
        createTexture();
        createPipeline();
        return true;
    }

    void ParticleTest::createShader() {

        ShaderSources sources;
        sources.glsl4 = {
R"(
            layout(location = 0) in vec2 a_quad;
            layout(location = 1) in vec3 a_position;
            layout(location = 2) in vec4 a_color;
    
            layout(set = 0, binding = 0) uniform MVP_Matrix {
                mat4 u_model, u_view, u_projection;
            };
    
            layout(location = 0) out vec4 v_color;
            layout(location = 1) out vec2 v_texcoord;
    
            void main() {
                // billboard
                vec4 pos = u_view * u_model * vec4(a_position, 1);
                pos.xy += a_quad.xy;
                pos = u_projection * pos;
        
                v_texcoord = vec2(a_quad * -0.5 + 0.5);
        
                gl_Position = pos;
                gl_PointSize = 2.0;
                v_color = a_color;
            }
)", R"(
            layout(set = 0, binding = 1) uniform sampler2D u_texture;
    
            layout(location = 0) in vec4 v_color;
            layout(location = 1) in vec2 v_texcoord;
    
            layout(location = 0) out vec4 o_color;
            void main () {
                o_color = v_color * texture(u_texture, v_texcoord);
            }
)"
        };

        sources.glsl3 = {
R"(
            in vec2 a_quad;
            in vec3 a_position;
            in vec4 a_color;
    
            layout(std140) uniform MVP_Matrix {
                mat4 u_model, u_view, u_projection;
            };
    
            out vec4 v_color;
            out vec2 v_texcoord;
    
            void main() {
                // billboard
                vec4 pos = u_view * u_model * vec4(a_position, 1);
                pos.xy += a_quad.xy;
                pos = u_projection * pos;
        
                v_texcoord = vec2(a_quad * -0.5 + 0.5);
        
                gl_Position = pos;
                gl_PointSize = 2.0;
                v_color = a_color;
            }
)", R"(
            precision mediump float;
            uniform sampler2D u_texture;
    
            in vec4 v_color;
            in vec2 v_texcoord;
    
            out vec4 o_color;
            void main () {
                o_color = v_color * texture(u_texture, v_texcoord);
            }
)"
        };

        sources.glsl1 = {
R"(
            attribute vec2 a_quad;
            attribute vec3 a_position;
            attribute vec4 a_color;
    
            uniform mat4 u_model, u_view, u_projection;
    
            varying vec4 v_color;
            varying vec2 v_texcoord;
    
            void main() {
                // billboard
                vec4 pos = u_view * u_model * vec4(a_position, 1);
                pos.xy += a_quad.xy;
                pos = u_projection * pos;
        
                v_texcoord = vec2(a_quad * -0.5 + 0.5);
        
                gl_Position = pos;
                gl_PointSize = 2.0;
                v_color = a_color;
            }
)", R"(
            precision mediump float;
            uniform sampler2D u_texture;
    
            varying vec4 v_color;
            varying vec2 v_texcoord;
    
            void main () {
                gl_FragColor = v_color * texture2D(u_texture, v_texcoord);
            }
)"
        };

        ShaderSource &source = TestBaseI::getAppropriateShaderSource(_device, sources);

        gfx::ShaderStageList shaderStageList;
        gfx::ShaderStage vertexShaderStage;
        vertexShaderStage.stage = gfx::ShaderStageFlagBit::VERTEX;
        vertexShaderStage.source = source.vert;
        shaderStageList.emplace_back(std::move(vertexShaderStage));

        gfx::ShaderStage fragmentShaderStage;
        fragmentShaderStage.stage = gfx::ShaderStageFlagBit::FRAGMENT;
        fragmentShaderStage.source = source.frag;
        shaderStageList.emplace_back(std::move(fragmentShaderStage));

        gfx::AttributeList attributeList = {
            { "a_quad", gfx::Format::RG32F, false, 0, false, 0 },
            { "a_position", gfx::Format::RGB32F, false, 0, false, 1 },
            { "a_color", gfx::Format::RGBA32F, false, 0, false, 2 },
        };
        gfx::UniformList mvpMatrix = {
            {"u_model", gfx::Type::MAT4, 1},
            {"u_view", gfx::Type::MAT4, 1},
            {"u_projection", gfx::Type::MAT4, 1}
        };
        gfx::UniformBlockList uniformBlockList = { { 0, 0, "MVP_Matrix", mvpMatrix, 1 } };

        gfx::UniformSamplerList sampler = { { 0, 1, "u_texture", gfx::Type::SAMPLER2D, 1 } };

        gfx::ShaderInfo shaderInfo;
        shaderInfo.name = "Particle Test";
        shaderInfo.stages = std::move(shaderStageList);
        shaderInfo.attributes = std::move(attributeList);
        shaderInfo.blocks = std::move(uniformBlockList);
        shaderInfo.samplers = std::move(sampler);
        _shader = _device->createShader(shaderInfo);
    }

    void ParticleTest::createVertexBuffer() {
        // vertex buffer: _vbufferArray[MAX_QUAD_COUNT][4][VERTEX_STRIDE];
        _vertexBuffer = _device->createBuffer({
            gfx::BufferUsage::VERTEX,
            gfx::MemoryUsage::DEVICE | gfx::MemoryUsage::HOST,
            VERTEX_STRIDE * sizeof(float),
            sizeof(_vbufferArray),
        });

        // index buffer: _ibufferArray[MAX_QUAD_COUNT][6];
        uint16_t dst = 0;
        uint16_t *p = _ibufferArray[0];
        for (uint16_t i = 0; i < MAX_QUAD_COUNT; ++i) {
            uint16_t baseIndex = i * 4;
            p[dst++] = baseIndex;
            p[dst++] = baseIndex + 1;
            p[dst++] = baseIndex + 2;
            p[dst++] = baseIndex;
            p[dst++] = baseIndex + 2;
            p[dst++] = baseIndex + 3;
        }
        _indexBuffer = _device->createBuffer({
            gfx::BufferUsage::INDEX,
            gfx::MemoryUsage::DEVICE,
            sizeof(uint16_t),
            sizeof(_ibufferArray),
        });
        _indexBuffer->update(_ibufferArray, 0, sizeof(_ibufferArray));

        for (size_t i = 0; i < PARTICLE_COUNT; ++i) {
            _particles[i].velocity = vec3Random(cc::random(0.1f, 10.0f));
            _particles[i].age = 0;
            _particles[i].life = cc::random(1.0f, 10.0f);
        }

        _uniformBuffer = _device->createBuffer({
            gfx::BufferUsage::UNIFORM,
            gfx::MemoryUsage::DEVICE,
            0,
            TestBaseI::getUBOSize(3 * sizeof(Mat4)),
        });
        Mat4 model, view, projection;
        Mat4::createLookAt(Vec3(30.0f, 20.0f, 30.0f), Vec3(0.0f, 2.5f, 0.0f), Vec3(0.0f, 1.0f, 0.f), &view);
        Mat4::createPerspective(60.0f, 1.0f * _device->getWidth() / _device->getHeight(), 0.01f, 1000.0f, &projection);
        TestBaseI::modifyProjectionBasedOnDevice(projection);
        _uniformBuffer->update(model.m, 0, sizeof(model));
        _uniformBuffer->update(view.m, sizeof(model), sizeof(view));
        _uniformBuffer->update(projection.m, sizeof(model) + sizeof(view), sizeof(projection));
    }

    void ParticleTest::createInputAssembler() {
        gfx::Attribute position = { "a_position", gfx::Format::RGB32F, false, 0, false };
        gfx::Attribute quad = { "a_quad", gfx::Format::RG32F, false, 0, false };
        gfx::Attribute color = { "a_color", gfx::Format::RGBA32F, false, 0, false };
        gfx::InputAssemblerInfo inputAssemblerInfo;
        inputAssemblerInfo.attributes.emplace_back(std::move(quad));
        inputAssemblerInfo.attributes.emplace_back(std::move(position));
        inputAssemblerInfo.attributes.emplace_back(std::move(color));
        inputAssemblerInfo.vertexBuffers.emplace_back(_vertexBuffer);
        inputAssemblerInfo.indexBuffer = _indexBuffer;
        _inputAssembler = _device->createInputAssembler(inputAssemblerInfo);
    }

    void ParticleTest::createPipeline() {
        gfx::DescriptorSetLayoutInfo dslInfo;
        dslInfo.bindings.push_back({ gfx::DescriptorType::UNIFORM_BUFFER, 1, gfx::ShaderStageFlagBit::VERTEX });
        dslInfo.bindings.push_back({ gfx::DescriptorType::SAMPLER, 1, gfx::ShaderStageFlagBit::FRAGMENT });
        _descriptorSetLayout = _device->createDescriptorSetLayout(dslInfo);

        _pipelineLayout = _device->createPipelineLayout({ { _descriptorSetLayout } });

        _descriptorSet = _device->createDescriptorSet({ _descriptorSetLayout });

        _descriptorSet->bindBuffer(0, _uniformBuffer);
        _descriptorSet->bindSampler(1, _sampler);
        _descriptorSet->bindTexture(1, _texture);
        _descriptorSet->update();

        gfx::PipelineStateInfo pipelineInfo;
        pipelineInfo.primitive = gfx::PrimitiveMode::TRIANGLE_LIST;
        pipelineInfo.shader = _shader;
        pipelineInfo.inputState = { _inputAssembler->getAttributes() };
        pipelineInfo.renderPass = _fbo->getRenderPass();
        pipelineInfo.blendState.targets[0].blend = true;
        pipelineInfo.blendState.targets[0].blendEq = gfx::BlendOp::ADD;
        pipelineInfo.blendState.targets[0].blendAlphaEq = gfx::BlendOp::ADD;
        pipelineInfo.blendState.targets[0].blendSrc = gfx::BlendFactor::SRC_ALPHA;
        pipelineInfo.blendState.targets[0].blendDst = gfx::BlendFactor::ONE_MINUS_SRC_ALPHA;
        pipelineInfo.blendState.targets[0].blendSrcAlpha = gfx::BlendFactor::ONE;
        pipelineInfo.blendState.targets[0].blendDstAlpha = gfx::BlendFactor::ONE;
        pipelineInfo.pipelineLayout = _pipelineLayout;

        _pipelineState = _device->createPipelineState(pipelineInfo);
    }

    void ParticleTest::createTexture() {
        const size_t LINE_WIDHT = 128;
        const size_t LINE_HEIGHT = 128;
        const size_t BUFFER_SIZE = LINE_WIDHT * LINE_HEIGHT * 4;
        uint8_t *imageData = (uint8_t *)CC_MALLOC(BUFFER_SIZE);
        fillRectWithColor(imageData, LINE_WIDHT, LINE_HEIGHT, 0, 0, 128, 128, 0xD0, 0xD0, 0xD0);
        fillRectWithColor(imageData, LINE_WIDHT, LINE_HEIGHT, 0, 0, 64, 64, 0x50, 0x50, 0x50);
        fillRectWithColor(imageData, LINE_WIDHT, LINE_HEIGHT, 32, 32, 32, 32, 0xFF, 0x00, 0x00);
        fillRectWithColor(imageData, LINE_WIDHT, LINE_HEIGHT, 64, 64, 64, 64, 0x00, 0xFF, 0x00);
        fillRectWithColor(imageData, LINE_WIDHT, LINE_HEIGHT, 96, 96, 32, 32, 0x00, 0x00, 0xFF);

        gfx::TextureInfo textureInfo;
        textureInfo.usage = gfx::TextureUsage::SAMPLED | gfx::TextureUsage::TRANSFER_DST;
        textureInfo.format = gfx::Format::RGBA8;
        textureInfo.width = LINE_WIDHT;
        textureInfo.height = LINE_HEIGHT;
        textureInfo.flags = gfx::TextureFlagBit::GEN_MIPMAP;
        textureInfo.levelCount = TestBaseI::getMipmapLevelCounts(textureInfo.width, textureInfo.height);
        _texture = _device->createTexture(textureInfo);

        gfx::BufferTextureCopy textureRegion;
        textureRegion.buffTexHeight = 0;
        textureRegion.texExtent.width = LINE_WIDHT;
        textureRegion.texExtent.height = LINE_HEIGHT;
        textureRegion.texExtent.depth = 1;

        gfx::BufferTextureCopyList regions;
        regions.push_back(std::move(textureRegion));

        gfx::BufferDataList imageBuffer = { imageData };
        _device->copyBuffersToTexture(imageBuffer, _texture, regions);
        CC_SAFE_FREE(imageData);

        // create sampler
        gfx::SamplerInfo samplerInfo;
        samplerInfo.addressU = gfx::Address::WRAP;
        samplerInfo.addressV = gfx::Address::WRAP;
        samplerInfo.mipFilter = gfx::Filter::LINEAR;
        _sampler = _device->createSampler(samplerInfo);
    }

    void ParticleTest::tick(float dt) {

        gfx::Rect render_area = { 0, 0, _device->getWidth(), _device->getHeight() };
        gfx::Color clear_color = { 0, 0, 0, 1.0f };

        // update particles
        for (size_t i = 0; i < PARTICLE_COUNT; ++i) {
            ParticleData &p = _particles[i];
            p.position = std::move(vec3ScaleAndAdd(p.position, p.velocity, dt));
            p.age += dt;

            if (p.age >= p.life) {
                p.age = 0;
                p.position = Vec3::ZERO;
            }
        }

        // update vertex-buffer
        float *pVbuffer = &_vbufferArray[0][0][0];
        for (size_t i = 0; i < PARTICLE_COUNT; ++i) {
            ParticleData &p = _particles[i];
            for (size_t v = 0; v < 4; ++v) {
                size_t offset = VERTEX_STRIDE * (4 * i + v);

                // quad
                pVbuffer[offset + 0] = quadVerts[v][0];
                pVbuffer[offset + 1] = quadVerts[v][1];

                // pos
                pVbuffer[offset + 2] = p.position.x;
                pVbuffer[offset + 3] = p.position.y;
                pVbuffer[offset + 4] = p.position.z;

                // color
                pVbuffer[offset + 5] = 1;
                pVbuffer[offset + 6] = 1;
                pVbuffer[offset + 7] = 1;
                pVbuffer[offset + 8] = 1.0f - p.age / p.life;
            }
        }

        _device->acquire();

        _vertexBuffer->update(_vbufferArray, 0, sizeof(_vbufferArray));

        auto commandBuffer = _commandBuffers[0];
        commandBuffer->begin();
        commandBuffer->beginRenderPass(_fbo->getRenderPass(), _fbo, render_area,
            std::move(std::vector<gfx::Color>({ clear_color })), 1.0f, 0);
        commandBuffer->bindInputAssembler(_inputAssembler);
        commandBuffer->bindPipelineState(_pipelineState);
        commandBuffer->bindDescriptorSet(0, _descriptorSet);
        commandBuffer->draw(_inputAssembler);
        commandBuffer->endRenderPass();
        commandBuffer->end();

        _device->getQueue()->submit(_commandBuffers);
        _device->present();
    }

} // namespace cc
