#include "ParticleTest.h"
#include "base/Random.h"

namespace cc {

namespace {
const float QUAD_VERTS[][2] = {{-1.0F, -1.0F}, {1.0F, -1.0F}, {1.0F, 1.0F}, {-1.0F, 1.0F}};

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
            p    = buf + (totalWidth * offsetY + offsetX) * 4;
            *p++ = r;
            *p++ = g;
            *p++ = b;
            *p++ = 255;
        }
    }
}

/**
 * Generates a random vector with the given scale
 * @param scale Length of the resulting vector. If omitted, a unit vector will
 * be returned
 */
Vec3 vec3Random(float scale /* = 1.0f */) {
    Vec3  out;
    float r      = rand0_1() * 2.0F * cc::math::PI;
    float z      = rand0_1() * 2.0F - 1.0F;
    float zScale = sqrtf(1.0F - z * z) * scale;

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

void ParticleTest::onDestroy() {
    CC_SAFE_DESTROY(_shader);
    CC_SAFE_DESTROY(_vertexBuffer);
    CC_SAFE_DESTROY(_indexBuffer);
    CC_SAFE_DESTROY(_inputAssembler);
    CC_SAFE_DESTROY(_pipelineState);
    CC_SAFE_DESTROY(_descriptorSet);
    CC_SAFE_DESTROY(_descriptorSetLayout);
    CC_SAFE_DESTROY(_pipelineLayout);
    CC_SAFE_DESTROY(_uniformBuffer);
    CC_SAFE_DESTROY(_sampler);
}

bool ParticleTest::onInit() {
    createShader();
    createVertexBuffer();
    createInputAssembler();
    createTexture();
    createPipeline();
    return true;
}

void ParticleTest::createShader() {
    ShaderSources<StandardShaderSource> sources;
    sources.glsl4 = {
        R"(
            precision highp float;
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
        )",
        R"(
            precision highp float;
            layout(set = 0, binding = 1) uniform sampler2D u_texture;

            layout(location = 0) in vec4 v_color;
            layout(location = 1) in vec2 v_texcoord;

            layout(location = 0) out vec4 o_color;
            void main () {
                o_color = v_color * texture(u_texture, v_texcoord);
            }
        )",
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
        )",
        R"(
            precision mediump float;
            uniform sampler2D u_texture;

            in vec4 v_color;
            in vec2 v_texcoord;

            out vec4 o_color;
            void main () {
                o_color = v_color * texture(u_texture, v_texcoord);
            }
        )",
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
        )",
        R"(
            precision mediump float;
            uniform sampler2D u_texture;

            varying vec4 v_color;
            varying vec2 v_texcoord;

            void main () {
                gl_FragColor = v_color * texture2D(u_texture, v_texcoord);
            }
        )",
    };

    StandardShaderSource &source = TestBaseI::getAppropriateShaderSource(sources);

    gfx::ShaderStageList shaderStageList;
    gfx::ShaderStage     vertexShaderStage;
    vertexShaderStage.stage  = gfx::ShaderStageFlagBit::VERTEX;
    vertexShaderStage.source = source.vert;
    shaderStageList.emplace_back(std::move(vertexShaderStage));

    gfx::ShaderStage fragmentShaderStage;
    fragmentShaderStage.stage  = gfx::ShaderStageFlagBit::FRAGMENT;
    fragmentShaderStage.source = source.frag;
    shaderStageList.emplace_back(std::move(fragmentShaderStage));

    gfx::AttributeList attributeList = {
        {"a_quad", gfx::Format::RG32F, false, 0, false, 0},
        {"a_position", gfx::Format::RGB32F, false, 0, false, 1},
        {"a_color", gfx::Format::RGBA32F, false, 0, false, 2},
    };
    gfx::UniformList mvpMatrix = {
        {"u_model", gfx::Type::MAT4, 1},
        {"u_view", gfx::Type::MAT4, 1},
        {"u_projection", gfx::Type::MAT4, 1},
    };
    gfx::UniformBlockList uniformBlockList = {{0, 0, "MVP_Matrix", mvpMatrix, 1}};

    gfx::UniformSamplerTextureList sampler = {{0, 1, "u_texture", gfx::Type::SAMPLER2D, 1}};

    gfx::ShaderInfo shaderInfo;
    shaderInfo.name            = "Particle Test";
    shaderInfo.stages          = std::move(shaderStageList);
    shaderInfo.attributes      = std::move(attributeList);
    shaderInfo.blocks          = std::move(uniformBlockList);
    shaderInfo.samplerTextures = std::move(sampler);
    _shader                    = device->createShader(shaderInfo);
}

void ParticleTest::createVertexBuffer() {
    // vertex buffer: _vbufferArray[MAX_QUAD_COUNT][4][VERTEX_STRIDE];
    _vertexBuffer = device->createBuffer({
        gfx::BufferUsage::VERTEX,
        gfx::MemoryUsage::DEVICE | gfx::MemoryUsage::HOST,
        sizeof(_vbufferArray),
        VERTEX_STRIDE * sizeof(float),
    });

    // index buffer: _ibufferArray[MAX_QUAD_COUNT][6];
    uint16_t  dst = 0;
    uint16_t *p   = _ibufferArray[0];
    for (uint16_t i = 0; i < MAX_QUAD_COUNT; ++i) {
        uint16_t baseIndex = i * 4;
        p[dst++]           = baseIndex;
        p[dst++]           = baseIndex + 1;
        p[dst++]           = baseIndex + 2;
        p[dst++]           = baseIndex;
        p[dst++]           = baseIndex + 2;
        p[dst++]           = baseIndex + 3;
    }
    _indexBuffer = device->createBuffer({
        gfx::BufferUsage::INDEX,
        gfx::MemoryUsage::DEVICE,
        sizeof(_ibufferArray),
        sizeof(uint16_t),
    });
    _indexBuffer->update(_ibufferArray, sizeof(_ibufferArray));

    for (auto &particle : _particles) {
        particle.velocity = vec3Random(cc::random(0.1F, 10.0F));
        particle.age      = 0;
        particle.life     = cc::random(1.0F, 10.0F);
    }

    _uniformBuffer = device->createBuffer({
        gfx::BufferUsage::UNIFORM,
        gfx::MemoryUsage::DEVICE | gfx::MemoryUsage::HOST,
        TestBaseI::getUBOSize(3 * sizeof(Mat4)),
    });
    Mat4::createLookAt(Vec3(30.0F, 20.0F, 30.0F), Vec3(0.0F, 2.5F, 0.0F), Vec3(0.0F, 1.0F, 0.F), &_matrices[1]);
}

void ParticleTest::createInputAssembler() {
    gfx::Attribute          position = {"a_position", gfx::Format::RGB32F, false, 0, false};
    gfx::Attribute          quad     = {"a_quad", gfx::Format::RG32F, false, 0, false};
    gfx::Attribute          color    = {"a_color", gfx::Format::RGBA32F, false, 0, false};
    gfx::InputAssemblerInfo inputAssemblerInfo;
    inputAssemblerInfo.attributes.emplace_back(std::move(quad));
    inputAssemblerInfo.attributes.emplace_back(std::move(position));
    inputAssemblerInfo.attributes.emplace_back(std::move(color));
    inputAssemblerInfo.vertexBuffers.emplace_back(_vertexBuffer);
    inputAssemblerInfo.indexBuffer = _indexBuffer;
    _inputAssembler                = device->createInputAssembler(inputAssemblerInfo);
}

void ParticleTest::createTexture() {
    const size_t lineWidht  = 128;
    const size_t lineHeight = 128;
    const size_t bufferSize = lineWidht * lineHeight * 4;
    auto *       imageData  = static_cast<uint8_t *>(CC_MALLOC(bufferSize));
    fillRectWithColor(imageData, lineWidht, lineHeight, 0, 0, 128, 128, 0xD0, 0xD0, 0xD0);
    fillRectWithColor(imageData, lineWidht, lineHeight, 0, 0, 64, 64, 0x50, 0x50, 0x50);
    fillRectWithColor(imageData, lineWidht, lineHeight, 32, 32, 32, 32, 0xFF, 0x00, 0x00);
    fillRectWithColor(imageData, lineWidht, lineHeight, 64, 64, 64, 64, 0x00, 0xFF, 0x00);
    fillRectWithColor(imageData, lineWidht, lineHeight, 96, 96, 32, 32, 0x00, 0x00, 0xFF);

    gfx::TextureInfo textureInfo;
    textureInfo.usage      = gfx::TextureUsage::SAMPLED | gfx::TextureUsage::TRANSFER_DST;
    textureInfo.format     = gfx::Format::RGBA8;
    textureInfo.width      = lineWidht;
    textureInfo.height     = lineHeight;
    textureInfo.flags      = gfx::TextureFlagBit::GEN_MIPMAP;
    textureInfo.levelCount = TestBaseI::getMipmapLevelCounts(textureInfo.width, textureInfo.height);
    _textures.push_back(device->createTexture(textureInfo));

    gfx::BufferTextureCopy textureRegion;
    textureRegion.buffTexHeight    = 0;
    textureRegion.texExtent.width  = lineWidht;
    textureRegion.texExtent.height = lineHeight;
    textureRegion.texExtent.depth  = 1;

    gfx::BufferTextureCopyList regions;
    regions.push_back(textureRegion);

    gfx::BufferDataList imageBuffer = {imageData};
    device->copyBuffersToTexture(imageBuffer, _textures[0], regions);
    CC_SAFE_FREE(imageData);

    // create sampler
    gfx::SamplerInfo samplerInfo;
    samplerInfo.addressU  = gfx::Address::WRAP;
    samplerInfo.addressV  = gfx::Address::WRAP;
    samplerInfo.mipFilter = gfx::Filter::LINEAR;
    _sampler              = device->createSampler(samplerInfo);
}

void ParticleTest::createPipeline() {
    gfx::DescriptorSetLayoutInfo dslInfo;
    dslInfo.bindings.push_back({0, gfx::DescriptorType::UNIFORM_BUFFER, 1, gfx::ShaderStageFlagBit::VERTEX});
    dslInfo.bindings.push_back({1, gfx::DescriptorType::SAMPLER_TEXTURE, 1, gfx::ShaderStageFlagBit::FRAGMENT});
    _descriptorSetLayout = device->createDescriptorSetLayout(dslInfo);

    _pipelineLayout = device->createPipelineLayout({{_descriptorSetLayout}});

    _descriptorSet = device->createDescriptorSet({_descriptorSetLayout});

    _descriptorSet->bindBuffer(0, _uniformBuffer);
    _descriptorSet->bindSampler(1, _sampler);
    _descriptorSet->bindTexture(1, _textures[0]);
    _descriptorSet->update();

    gfx::PipelineStateInfo pipelineInfo;
    pipelineInfo.primitive                           = gfx::PrimitiveMode::TRIANGLE_LIST;
    pipelineInfo.shader                              = _shader;
    pipelineInfo.inputState                          = {_inputAssembler->getAttributes()};
    pipelineInfo.renderPass                          = fbo->getRenderPass();
    pipelineInfo.blendState.targets[0].blend         = true;
    pipelineInfo.blendState.targets[0].blendEq       = gfx::BlendOp::ADD;
    pipelineInfo.blendState.targets[0].blendAlphaEq  = gfx::BlendOp::ADD;
    pipelineInfo.blendState.targets[0].blendSrc      = gfx::BlendFactor::SRC_ALPHA;
    pipelineInfo.blendState.targets[0].blendDst      = gfx::BlendFactor::ONE_MINUS_SRC_ALPHA;
    pipelineInfo.blendState.targets[0].blendSrcAlpha = gfx::BlendFactor::ONE;
    pipelineInfo.blendState.targets[0].blendDstAlpha = gfx::BlendFactor::ONE;
    pipelineInfo.pipelineLayout                      = _pipelineLayout;

    _pipelineState = device->createPipelineState(pipelineInfo);

    _globalBarriers.push_back(TestBaseI::getGlobalBarrier({
        {
            gfx::AccessType::TRANSFER_WRITE,
        },
        {
            gfx::AccessType::VERTEX_SHADER_READ_UNIFORM_BUFFER,
            gfx::AccessType::VERTEX_BUFFER,
            gfx::AccessType::INDEX_BUFFER,
        },
    }));

    _globalBarriers.push_back(TestBaseI::getGlobalBarrier({
        {
            gfx::AccessType::TRANSFER_WRITE,
        },
        {
            gfx::AccessType::VERTEX_SHADER_READ_UNIFORM_BUFFER,
        },
    }));

    _textureBarriers.push_back(TestBaseI::getTextureBarrier({
        {
            gfx::AccessType::TRANSFER_WRITE,
        },
        {
            gfx::AccessType::FRAGMENT_SHADER_READ_TEXTURE,
        },
        false,
    }));
}

void ParticleTest::onTick() {
    printTime();

    uint globalBarrierIdx = _frameCount ? 1 : 0;
    uint textureBarriers  = _frameCount ? 0 : _textureBarriers.size();

    gfx::Color clearColor = {0.2F, 0.2F, 0.2F, 1.0F};

    // update particles
    for (auto &p : _particles) {
        p.position = vec3ScaleAndAdd(p.position, p.velocity, logicThread.dt);
        p.age += logicThread.dt;

        if (p.age >= p.life) {
            p.age      = 0;
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
            pVbuffer[offset + 0] = QUAD_VERTS[v][0];
            pVbuffer[offset + 1] = QUAD_VERTS[v][1];

            // pos
            pVbuffer[offset + 2] = p.position.x;
            pVbuffer[offset + 3] = p.position.y;
            pVbuffer[offset + 4] = p.position.z;

            // color
            pVbuffer[offset + 5] = 1;
            pVbuffer[offset + 6] = 1;
            pVbuffer[offset + 7] = 1;
            pVbuffer[offset + 8] = 1.0F - p.age / p.life;
        }
    }

    gfx::Extent orientedSize = TestBaseI::getOrientedSurfaceSize();
    TestBaseI::createPerspective(60.0F,
                                 static_cast<float>(orientedSize.width) / static_cast<float>(orientedSize.height),
                                 0.01F, 1000.0F, &_matrices[2]);

    device->acquire();

    _uniformBuffer->update(_matrices, sizeof(_matrices));
    _vertexBuffer->update(_vbufferArray, sizeof(_vbufferArray));
    gfx::Rect renderArea = {0, 0, device->getWidth(), device->getHeight()};

    auto *commandBuffer = commandBuffers[0];
    commandBuffer->begin();

    if (TestBaseI::MANUAL_BARRIER) {
        commandBuffer->pipelineBarrier(_globalBarriers[globalBarrierIdx], _textureBarriers.data(), _textures.data(), textureBarriers);
    }

    commandBuffer->beginRenderPass(fbo->getRenderPass(), fbo, renderArea, &clearColor, 1.0F, 0);
    commandBuffer->bindInputAssembler(_inputAssembler);
    commandBuffer->bindPipelineState(_pipelineState);
    commandBuffer->bindDescriptorSet(0, _descriptorSet);
    commandBuffer->draw(_inputAssembler);
    commandBuffer->endRenderPass();
    commandBuffer->end();

    device->flushCommands(commandBuffers);
    device->getQueue()->submit(commandBuffers);
    device->present();
}

} // namespace cc
