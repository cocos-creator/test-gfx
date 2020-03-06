#include "ParticleTest.h"

#if (CC_PLATFORM == CC_PLATFORM_MAC_OSX)
#include "gfx-metal/GFXMTL.h"
#else
#include "gfx-gles2/GFXGLES2.h"
#include "gfx-gles3/GFXGLES3.h"
#endif

#include "base/ccRandom.h"
NS_CC_BEGIN

namespace {
    static const float quadVerts[][2] = {
        { -1.0f, -1.0f },
        {  1.0f, -1.0f },
        {  1.0f,  1.0f },
        { -1.0f,  1.0f }
    };
    
    void fillRectWithColor(uint8_t* buf, uint32_t totalWidth, uint32_t totalHeight, uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint8_t r, uint8_t g, uint8_t b)
    {
        assert(x + width <= totalWidth);
        assert(y + height <=  totalHeight);
        
        uint32_t y0 = totalHeight - (y + height);
        uint32_t y1 = totalHeight - y;
        uint8_t* p;
        for (uint32_t offsetY = y0; offsetY < y1; ++offsetY)
        {
            for (uint32_t offsetX = x; offsetX < (x + width); ++offsetX)
            {
                p = buf + (totalWidth * offsetY + offsetX) * 3;
                *p++ = r;
                *p++ = g;
                *p++ = b;
            }
        }
    }
    
    /**
     * Generates a random vector with the given scale
     * @param scale Length of the resulting vector. If ommitted, a unit vector will be returned
     */
    Vec3 vec3Random(float scale/* = 1.0f */)
    {
        Vec3 out;
        float r = cocos2d::rand_0_1() * 2.0f * math::PI;
        float z = (cocos2d::rand_0_1() * 2.0f) - 1.0f;
        float zScale = sqrtf(1.0f - z * z) * scale;
        
        out.x = cosf(r) * zScale;
        out.y = sinf(r) * zScale;
        out.z = z * scale;
        return out;
    };
    
    Vec3 vec3ScaleAndAdd(const Vec3& a, const Vec3& b, float scale)
    {
        Vec3 out;
        out.x = a.x + (b.x * scale);
        out.y = a.y + (b.y * scale);
        out.z = a.z + (b.z * scale);
        return out;
    };
}

void ParticleTest::destroy()
{
    CC_SAFE_DESTROY(_shader);
    CC_SAFE_DESTROY(_vertexBuffer);
    CC_SAFE_DESTROY(_indexBuffer);
    CC_SAFE_DESTROY(_inputAssembler);
    CC_SAFE_DESTROY(_pipelineState);
    CC_SAFE_DESTROY(_bindingLayout);
    CC_SAFE_DESTROY(_uniformBuffer);
    CC_SAFE_DESTROY(_texture);
    CC_SAFE_DESTROY(_sampler);
    CC_SAFE_DESTROY(_texView);
}

bool ParticleTest::initialize()
{
    createShader();
    createVertexBuffer();
    createInputAssembler();
    createTexture();
    createPipeline();
    return true;
}

void ParticleTest::createShader()
{
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
        float4 v_color [[user(locn0)]];
        float2 v_texcoord [[user(locn1)]];
        float4 gl_Position [[position]];
        float gl_PointSize [[point_size]];
    };
    
    struct main0_in
    {
        float2 a_quad [[attribute(0)]];
        float3 a_position [[attribute(1)]];
        float4 a_color [[attribute(2)]];
    };
    
    vertex main0_out main0(main0_in in [[stage_in]], constant MVP_Matrix& _13 [[buffer(0)]])
    {
        main0_out out = {};
        float4 pos = (_13.u_view * _13.u_model) * float4(in.a_position, 1.0);
        float2 _39 = pos.xy + in.a_quad;
        pos = float4(_39.x, _39.y, pos.z, pos.w);
        pos = _13.u_projection * pos;
        out.v_texcoord = float2((in.a_quad * (-0.5)) + float2(0.5));
        out.gl_Position = pos;
        out.gl_PointSize = 2.0;
        out.v_color = in.a_color;
        return out;
    }
    )";
#else
    
#ifdef USE_GLES2
    vertexShaderStage.source = R"(
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
    )";
#else
    vertexShaderStage.source = R"(#version 300 es
    in vec2 a_quad;
    in vec3 a_position;
    in vec4 a_color;
    
    layout(std140) uniform MVP_Matrix
    {
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
    
    struct main0_out
    {
        float4 o_color [[color(0)]];
    };
    
    struct main0_in
    {
        float4 v_color [[user(locn0)]];
        float2 v_texcoord [[user(locn1)]];
    };
    
    fragment main0_out main0(main0_in in [[stage_in]], texture2d<float> u_texture [[texture(0)]], sampler u_textureSmplr [[sampler(0)]])
    {
        main0_out out = {};
        out.o_color = in.v_color * u_texture.sample(u_textureSmplr, in.v_texcoord);
        return out;
    }
    )";
#else
#ifdef USE_GLES2
    fragmentShaderStage.source = R"(
    #ifdef GL_ES
    precision highp float;
    #endif
    uniform sampler2D u_texture;
    
    varying vec4 v_color;
    varying vec2 v_texcoord;
    
    void main () {
        gl_FragColor = v_color * texture2D(u_texture, v_texcoord);
    }
    )";
#else
    fragmentShaderStage.source = R"(#version 300 es
    #ifdef GL_ES
    precision highp float;
    #endif
    uniform sampler2D u_texture;
    
    in vec4 v_color;
    in vec2 v_texcoord;
    
    out vec4 o_color;
    void main () {
        o_color = v_color * texture(u_texture, v_texcoord);
    }
    )";
#endif // USE_GLES2
#endif // (CC_PLATFORM == CC_PLATFORM_MAC_OSX)

    shaderStageList.emplace_back(std::move(fragmentShaderStage));

    GFXUniformList mvpMatrix = {
            { "u_model", GFXType::MAT4, 1},
            { "u_view", GFXType::MAT4, 1},
            { "u_projection", GFXType::MAT4, 1}
    };
    GFXUniformBlockList uniformBlockList = { {0, "MVP_Matrix", mvpMatrix} };
    
    GFXUniformSamplerList sampler = { {1, "u_texture", GFXType::SAMPLER2D, 1} };

    GFXShaderInfo shaderInfo;
    shaderInfo.name = "Particle Test";
    shaderInfo.stages = std::move(shaderStageList);
    shaderInfo.blocks = std::move(uniformBlockList);
    shaderInfo.samplers = std::move(sampler);
    _shader = _device->createShader(shaderInfo);
}

void ParticleTest::createVertexBuffer()
{
    //vertex buffer: _vbufferArray[MAX_QUAD_COUNT][4][VERTEX_STRIDE];
    GFXBufferInfo vertexBufferInfo = {
          GFXBufferUsage::VERTEX,
          GFXMemoryUsage::NONE,
          VERTEX_STRIDE * sizeof(float),
          sizeof(_vbufferArray),
          GFXBufferFlagBit::NONE };

    _vertexBuffer = _device->createBuffer(vertexBufferInfo);
    
    //index buffer: _ibufferArray[MAX_QUAD_COUNT][6];
    uint16_t dst = 0;
    uint16_t* p = _ibufferArray[0];
    for (uint16_t i = 0; i < MAX_QUAD_COUNT; ++i) {
        uint16_t baseIndex = i * 4;
        p[dst++] = baseIndex;
        p[dst++] = baseIndex + 1;
        p[dst++] = baseIndex + 2;
        p[dst++] = baseIndex;
        p[dst++] = baseIndex + 2;
        p[dst++] = baseIndex + 3;
    }
    GFXBufferInfo indexBufferInfo = {
        GFXBufferUsage::INDEX,
        GFXMemoryUsage::NONE,
        sizeof(uint16_t),
        sizeof(_ibufferArray),
        GFXBufferFlagBit::NONE };
    
    _indexBuffer = _device->createBuffer(indexBufferInfo);
    _indexBuffer->update(_ibufferArray, 0, sizeof(_ibufferArray));
    
    for (size_t i = 0; i < PARTICLE_COUNT; ++i)
    {
        _particles[i].velocity = vec3Random(cocos2d::random(0.1f, 10.0f));
        _particles[i].age = 0;
        _particles[i].life = cocos2d::random(1.0f, 10.0f);
    }

    GFXBufferInfo uniformBufferInfo = {
           GFXBufferUsage::UNIFORM,
           GFXMemoryUsage::NONE,
           sizeof(Mat4),
           3 * sizeof(Mat4),
           GFXBufferFlagBit::NONE };
     _uniformBuffer = _device->createBuffer(uniformBufferInfo);
    Mat4 model, view, projection;
    Mat4::createPerspective(60.0f, 1.0f * _device->getWidth() / _device->getHeight(), 0.01f, 1000.0f, &projection);
    Mat4::createLookAt(Vec3(30.0f , 20.0f, 30.0f), Vec3(0.0f, 2.5f, 0.0f), Vec3(0.0f, 1.0f, 0.f), &view);
    _uniformBuffer->update(model.m, 0, sizeof(model));
    _uniformBuffer->update(view.m, sizeof(model), sizeof(view));
    _uniformBuffer->update(projection.m, sizeof(model) + sizeof(view), sizeof(projection));
}

void ParticleTest::createInputAssembler()
{
    GFXAttribute position = {"a_position", GFXFormat::RGB32F, false, 0, false};
    GFXAttribute quad = {"a_quad", GFXFormat::RG32F, false, 0, false};
    GFXAttribute color = {"a_color", GFXFormat::RGBA32F, false, 0, false};
    GFXInputAssemblerInfo inputAssemblerInfo;
    inputAssemblerInfo.attributes.emplace_back(std::move(quad));
    inputAssemblerInfo.attributes.emplace_back(std::move(position));
    inputAssemblerInfo.attributes.emplace_back(std::move(color));
    inputAssemblerInfo.vertexBuffers.emplace_back(_vertexBuffer);
    inputAssemblerInfo.indexBuffer = _indexBuffer;
    _inputAssembler = _device->createInputAssembler(inputAssemblerInfo);
}

void ParticleTest::createPipeline()
{
    GFXBindingList bindingList = {
        {0, GFXBindingType::UNIFORM_BUFFER, "MVP_Martix"},
        {1, GFXBindingType::SAMPLER, "u_texture"}
    };
    GFXBindingLayoutInfo bindingLayoutInfo = { bindingList };
    _bindingLayout = _device->createBindingLayout(bindingLayoutInfo);
    
    _bindingLayout->bindBuffer(0, _uniformBuffer);
    _bindingLayout->bindSampler(1, _sampler);
    _bindingLayout->bindTextureView(1, _texView);
    _bindingLayout->update();

    GFXPipelineLayoutInfo pipelineLayoutInfo;
    pipelineLayoutInfo.layouts = { _bindingLayout };
    auto pipelineLayout = _device->createPipelineLayout(pipelineLayoutInfo);

    GFXPipelineStateInfo pipelineInfo;
    pipelineInfo.primitive = GFXPrimitiveMode::TRIANGLE_LIST;
    pipelineInfo.shader = _shader;
    pipelineInfo.inputState = { _inputAssembler->getAttributes() };
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = _device->getMainWindow()->getRenderPass();
    pipelineInfo.blendState.targets[0].blend = true;
    pipelineInfo.blendState.targets[0].blendEq = GFXBlendOp::ADD;
    pipelineInfo.blendState.targets[0].blendAlphaEq = GFXBlendOp::ADD;
    pipelineInfo.blendState.targets[0].blendSrc = GFXBlendFactor::SRC_ALPHA;
    pipelineInfo.blendState.targets[0].blendDst = GFXBlendFactor::ONE_MINUS_SRC_ALPHA;
    pipelineInfo.blendState.targets[0].blendSrcAlpha = GFXBlendFactor::ONE;
    pipelineInfo.blendState.targets[0].blendDstAlpha = GFXBlendFactor::ONE;

    _pipelineState = _device->createPipelineState(pipelineInfo);

    CC_SAFE_DESTROY(pipelineLayout);
}

void ParticleTest::createTexture()
{
    const size_t LINE_WIDHT = 128;
    const size_t LINE_HEIGHT = 128;
    const size_t BUFFER_SIZE = LINE_WIDHT * LINE_HEIGHT * 3;
    uint8_t* imageData = (uint8_t*)CC_MALLOC(BUFFER_SIZE);
    fillRectWithColor(imageData, LINE_WIDHT, LINE_HEIGHT, 0, 0, 128, 128, 0xD0, 0xD0, 0xD0);
    fillRectWithColor(imageData, LINE_WIDHT, LINE_HEIGHT, 0, 0, 64, 64, 0x50, 0x50, 0x50);
    fillRectWithColor(imageData, LINE_WIDHT, LINE_HEIGHT, 32, 32, 32, 32, 0xFF, 0x00, 0x00);
    fillRectWithColor(imageData, LINE_WIDHT, LINE_HEIGHT, 64, 64, 64, 64, 0x00, 0xFF, 0x00);
    fillRectWithColor(imageData, LINE_WIDHT, LINE_HEIGHT, 96, 96, 32, 32, 0x00, 0x00, 0xFF);
    
    GFXTextureInfo textureInfo;
    textureInfo.usage = GFXTextureUsage::SAMPLED;
    textureInfo.format = GFXFormat::RGB8;
    textureInfo.width = LINE_WIDHT;
    textureInfo.height = LINE_HEIGHT;
    textureInfo.flags = GFXTextureFlagBit::GEN_MIPMAP;
    _texture = _device->createTexture(textureInfo);
    
    GFXBufferTextureCopy textureRegion;
    textureRegion.buffTexHeight = 0;
    textureRegion.texExtent.width = LINE_WIDHT;
    textureRegion.texExtent.height = LINE_HEIGHT;
    textureRegion.texExtent.depth = 1;
    
    GFXBufferTextureCopyList regions;
    regions.push_back(std::move(textureRegion));
    
    GFXDataArray imageBuffer = { { imageData } };
    _device->copyBuffersToTexture(imageBuffer, _texture, regions);
    CC_SAFE_FREE(imageData);
    
    //create sampler
    GFXSamplerInfo samplerInfo;
    samplerInfo.addressU = GFXAddress::WRAP;
    samplerInfo.addressV = GFXAddress::WRAP;
    samplerInfo.mipFilter = GFXFilter::LINEAR;
    _sampler = _device->createSampler(samplerInfo);
    
    GFXTextureViewInfo texViewInfo;
    texViewInfo.texture = _texture;
    texViewInfo.format = GFXFormat::RGB8;
    _texView = _device->createTextureView(texViewInfo);
}

void ParticleTest::tick(float dt) {

    GFXRect render_area = {0, 0, _device->getWidth(), _device->getHeight() };
    GFXColor clear_color = {0, 0, 0, 1.0f};
    
    // update particles
    for (size_t i = 0; i < PARTICLE_COUNT; ++i)
    {
        ParticleData& p = _particles[i];
        p.position = std::move(vec3ScaleAndAdd(p.position, p.velocity, dt));
        p.age += dt;
        
        if (p.age >= p.life)
        {
            p.age = 0;
            p.position = Vec3::ZERO;
        }
    }

    // update vertex-buffer
    float* pVbuffer = &_vbufferArray[0][0][0];
    for (size_t i = 0; i < PARTICLE_COUNT; ++i)
    {
        ParticleData& p = _particles[i];
        for (size_t v = 0; v < 4; ++v)
        {
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
            pVbuffer[offset + 8] = 1.0 - p.age / p.life;
        }
    }
    _vertexBuffer->update(_vbufferArray, 0, sizeof(_vbufferArray));
    
    for(auto commandBuffer : _commandBuffers)
    {
        commandBuffer->begin();
        commandBuffer->beginRenderPass(_fbo, render_area, GFXClearFlagBit::ALL, std::move(std::vector<GFXColor>({clear_color})), 1.0f, 0);
        commandBuffer->bindInputAssembler(_inputAssembler);
        commandBuffer->bindBindingLayout(_bindingLayout);
        commandBuffer->bindPipelineState(_pipelineState);
        commandBuffer->draw(_inputAssembler);
        commandBuffer->endRenderPass();
        commandBuffer->end();
    }

    _device->getQueue()->submit(_commandBuffers);
    _device->present();
}

NS_CC_END
