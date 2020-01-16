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

void ParticleTest::Destroy()
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

    shaderStageList.emplace_back(std::move(vertexShaderStage));

    GFXShaderStage fragmentShaderStage;
    fragmentShaderStage.type = GFXShaderType::FRAGMENT;
    
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
    _shader = _device->CreateGFXShader(shaderInfo);
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

    _vertexBuffer = _device->CreateGFXBuffer(vertexBufferInfo);
    
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
    
    _indexBuffer = _device->CreateGFXBuffer(indexBufferInfo);
    _indexBuffer->Update(_ibufferArray, 0, sizeof(_ibufferArray));
    
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
     _uniformBuffer = _device->CreateGFXBuffer(uniformBufferInfo);
    Mat4 model, view, projection;
    Mat4::createPerspective(60.0f, 1.0f * _device->width() / _device->height(), 0.01f, 1000.0f, &projection);
    Mat4::createLookAt(Vec3(30.0f , 20.0f, 30.0f), Vec3(0.0f, 2.5f, 0.0f), Vec3(0.0f, 1.0f, 0.f), &view);
    _uniformBuffer->Update(model.m, 0, sizeof(model));
    _uniformBuffer->Update(view.m, sizeof(model), sizeof(view));
    _uniformBuffer->Update(projection.m, sizeof(model) + sizeof(view), sizeof(projection));
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
    inputAssemblerInfo.vertex_buffers.emplace_back(_vertexBuffer);
    inputAssemblerInfo.index_buffer = _indexBuffer;
    _inputAssembler = _device->CreateGFXInputAssembler(inputAssemblerInfo);
}

void ParticleTest::createPipeline()
{
    GFXBindingList bindingList = {
        {0, GFXBindingType::UNIFORM_BUFFER, "MVP_Martix"},
        {1, GFXBindingType::SAMPLER, "Texture"}
    };
    GFXBindingLayoutInfo bindingLayoutInfo = { bindingList };
    _bindingLayout = _device->CreateGFXBindingLayout(bindingLayoutInfo);
    
    _bindingLayout->BindBuffer(0, _uniformBuffer);
    _bindingLayout->BindSampler(1, _sampler);
    _bindingLayout->BindTextureView(1, _texView);
    _bindingLayout->Update();

    GFXPipelineLayoutInfo pipelineLayoutInfo;
    pipelineLayoutInfo.layouts = { _bindingLayout };
    auto pipelineLayout = _device->CreateGFXPipelieLayout(pipelineLayoutInfo);

    GFXPipelineStateInfo pipelineInfo;
    pipelineInfo.primitive = GFXPrimitiveMode::TRIANGLE_LIST;
    pipelineInfo.shader = _shader;
    pipelineInfo.is = { _inputAssembler->attributes() };
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.render_pass = _device->window()->render_pass();
    pipelineInfo.bs.targets[0].is_blend = true;
    pipelineInfo.bs.targets[0].blend_eq = GFXBlendOp::ADD;
    pipelineInfo.bs.targets[0].blend_alpha_eq = GFXBlendOp::ADD;
    pipelineInfo.bs.targets[0].blend_src = GFXBlendFactor::SRC_ALPHA;
    pipelineInfo.bs.targets[0].blend_dst = GFXBlendFactor::ONE_MINUS_SRC_ALPHA;
    pipelineInfo.bs.targets[0].blend_src_alpha = GFXBlendFactor::ONE;
    pipelineInfo.bs.targets[0].blend_dst_alpha = GFXBlendFactor::ONE;

    _pipelineState = _device->CreateGFXPipelineState(pipelineInfo);

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
    
    GFXBufferInfo imageBufferInfo = {
        GFXBufferUsage::TRANSFER_SRC,
        GFXMemoryUsage::NONE,
        1,
        static_cast<uint>(BUFFER_SIZE),
        GFXBufferFlagBit::NONE };
    
    auto image = _device->CreateGFXBuffer(imageBufferInfo);
    image->Update(imageData, 0, BUFFER_SIZE);
    CC_SAFE_FREE(imageData);
    
    GFXTextureInfo textureInfo;
    textureInfo.usage = GFXTextureUsage::SAMPLED;
    textureInfo.format = GFXFormat::RGB8;
    textureInfo.width = LINE_WIDHT;
    textureInfo.height = LINE_HEIGHT;
    textureInfo.flags = GFXTextureFlagBit::GEN_MIPMAP;
    
    _texture = _device->CreateGFXTexture(textureInfo);
    
    GFXBufferTextureCopy textureRegion;
    textureRegion.buff_tex_height = 0;
    textureRegion.tex_extent.width = LINE_WIDHT;
    textureRegion.tex_extent.height = LINE_HEIGHT;
    textureRegion.tex_extent.depth = 1;
    
    GFXBufferTextureCopyList regions;
    regions.push_back(std::move(textureRegion));
    
    _device->CopyBuffersToTexture(image, _texture, regions);
    CC_SAFE_DESTROY(image);
    
    //create sampler
    GFXSamplerInfo samplerInfo;
    samplerInfo.address_u = GFXAddress::WRAP;
    samplerInfo.address_v = GFXAddress::WRAP;
    samplerInfo.mip_filter = GFXFilter::LINEAR;
    _sampler = _device->CreateGFXSampler(samplerInfo);
    
    GFXTextureViewInfo texViewInfo;
    texViewInfo.texture = _texture;
    texViewInfo.format = GFXFormat::RGB8;
    _texView = _device->CreateGFXTextureView(texViewInfo);
}

void ParticleTest::tick(float dt) {

    GFXRect render_area = {0, 0, _device->width(), _device->height() };
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
    _vertexBuffer->Update(_vbufferArray, 0, sizeof(_vbufferArray));
    
    _commandBuffer->Begin();
    _commandBuffer->BeginRenderPass(_fbo, render_area, GFXClearFlagBit::ALL, &clear_color, 1, 1.0f, 0);
    _commandBuffer->BindInputAssembler(_inputAssembler);
    _commandBuffer->BindBindingLayout(_bindingLayout);
    _commandBuffer->BindPipelineState(_pipelineState);
    _commandBuffer->Draw(_inputAssembler);
    _commandBuffer->EndRenderPass();
    _commandBuffer->End();

    _device->queue()->submit(&_commandBuffer, 1);
    _device->Present();
}

NS_CC_END
