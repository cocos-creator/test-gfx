#include "BasicTextureTest.h"

namespace cc {

    void BasicTexture::destroy() {
        CC_SAFE_DESTROY(_shader);
        CC_SAFE_DESTROY(_vertexBuffer);
        CC_SAFE_DESTROY(_inputAssembler);
        CC_SAFE_DESTROY(_bindingLayout);
        CC_SAFE_DESTROY(_pipelineState);
        CC_SAFE_DESTROY(_uniformBuffer);
        CC_SAFE_DESTROY(_texture);
        CC_SAFE_DESTROY(_image);
        CC_SAFE_DESTROY(_sampler);
    }

    bool BasicTexture::initialize() {
        createShader();
        createVertexBuffer();
        createInputAssembler();
        createTexture();
        createPipeline();
        return true;
    }

    void BasicTexture::createShader() {

        ShaderSources sources;
        sources.glsl4 = {
R"(
            layout(location = 0) in vec2 a_position;
            layout(location = 0) out vec2 texcoord;
            layout(binding = 0) uniform MVP_Matrix
            {
                mat4 u_mvpMatrix;
            };
   
            void main()
            {
                gl_Position = u_mvpMatrix * vec4(a_position, 0, 1);
                texcoord = a_position * 0.5 + 0.5;
                texcoord = vec2(texcoord.x, 1.0 - texcoord.y);
            }
)", R"(
            layout(location = 0) in vec2 texcoord;
            layout(binding = 1) uniform sampler2D u_texture;
            layout(location = 0) out vec4 o_color;
            void main () {
                o_color = texture(u_texture, texcoord);
            }
)"
        };

        sources.glsl3 = {
R"(
            in vec2 a_position;
            out vec2 texcoord;
            layout(std140) uniform MVP_Matrix
            {
                mat4 u_mvpMatrix;
            };
   
            void main()
            {
                gl_Position = u_mvpMatrix * vec4(a_position, 0, 1);
                texcoord = a_position * 0.5 + 0.5;
                texcoord = vec2(texcoord.x, 1.0 - texcoord.y);
            }
)", R"(
            precision mediump float;
            in vec2 texcoord;
            uniform sampler2D u_texture;
            out vec4 o_color;
            void main () {
                o_color = texture(u_texture, texcoord);
            }
)"
        };

        sources.glsl1 = {
R"(
            attribute vec2 a_position;
            uniform mat4 u_mvpMatrix;
            varying vec2 v_texcoord;
            void main ()
            {
                gl_Position = u_mvpMatrix * vec4(a_position, 0, 1);
                v_texcoord = a_position * 0.5 + 0.5;
                v_texcoord = vec2(v_texcoord.x, 1.0 - v_texcoord.y);
            }
)", R"(
            precision mediump float;
            uniform sampler2D u_texture;
            varying vec2 v_texcoord;
            void main () {
                gl_FragColor = texture2D(u_texture, v_texcoord);
            }
)"
        };

        ShaderSource &source = TestBaseI::getAppropriateShaderSource(_device, sources);


        gfx::ShaderStageList shaderStageList;
        gfx::ShaderStage vertexShaderStage;
        vertexShaderStage.type = gfx::ShaderType::VERTEX;
        vertexShaderStage.source = source.vert;
        shaderStageList.emplace_back(std::move(vertexShaderStage));

        gfx::ShaderStage fragmentShaderStage;
        fragmentShaderStage.type = gfx::ShaderType::FRAGMENT;
        fragmentShaderStage.source = source.frag;
        shaderStageList.emplace_back(std::move(fragmentShaderStage));

        gfx::AttributeList attributeList = { { "a_position", gfx::Format::RG32F, false, 0, false, 0 } };
        gfx::UniformList mvpMatrix = { { "u_mvpMatrix", gfx::Type::MAT4, 1} };
        gfx::UniformBlockList uniformBlockList = { { gfx::ShaderType::VERTEX, 0, "MVP_Matrix", mvpMatrix} };
        gfx::UniformSamplerList sampler = { { gfx::ShaderType::FRAGMENT, 1, "u_texture", gfx::Type::SAMPLER2D, 1} };

        gfx::ShaderInfo shaderInfo;
        shaderInfo.name = "Basic Texture";
        shaderInfo.stages = std::move(shaderStageList);
        shaderInfo.attributes = std::move(attributeList);
        shaderInfo.blocks = std::move(uniformBlockList);
        shaderInfo.samplers = std::move(sampler);
        _shader = _device->createShader(shaderInfo);
    }

    void BasicTexture::createVertexBuffer() {
        float vertexData[] = {
            -1.0f, -1.0f,
             1.0f, -1.0f,
             1.0f,  1.0f,

             1.0f,  1.0f,
            -1.0f,  1.0f,
            -1.0f, -1.0f
        };

        _vertexBuffer = _device->createBuffer({
              gfx::BufferUsage::VERTEX,
              gfx::MemoryUsage::DEVICE,
              2 * sizeof(float),
              sizeof(vertexData),
        });
        _vertexBuffer->update(vertexData, 0, sizeof(vertexData));

        _uniformBuffer = _device->createBuffer({
               gfx::BufferUsage::UNIFORM,
               gfx::MemoryUsage::DEVICE,
               sizeof(Mat4),
               sizeof(Mat4),
        });
    }

    void BasicTexture::createInputAssembler() {
        gfx::Attribute position = { "a_position", gfx::Format::RG32F, false, 0, false };
        gfx::InputAssemblerInfo inputAssemblerInfo;
        inputAssemblerInfo.attributes.emplace_back(std::move(position));
        inputAssemblerInfo.vertexBuffers.emplace_back(_vertexBuffer);
        _inputAssembler = _device->createInputAssembler(inputAssemblerInfo);
    }

    void BasicTexture::createPipeline() {
        gfx::BindingLayoutInfo bindingLayoutInfo = { _shader };
        _bindingLayout = _device->createBindingLayout(bindingLayoutInfo);

        Mat4 mvpMatrix;
        TestBaseI::modifyProjectionBasedOnDevice(mvpMatrix);
        _uniformBuffer->update(&mvpMatrix, 0, sizeof(mvpMatrix));
        _bindingLayout->bindBuffer(0, _uniformBuffer);
        _bindingLayout->bindSampler(1, _sampler);
        _bindingLayout->bindTexture(1, _texture);
        _bindingLayout->update();

        gfx::PipelineStateInfo pipelineInfo;
        pipelineInfo.primitive = gfx::PrimitiveMode::TRIANGLE_LIST;
        pipelineInfo.shader = _shader;
        pipelineInfo.inputState = { _inputAssembler->getAttributes() };
        pipelineInfo.renderPass = _fbo->getRenderPass();

        _pipelineState = _device->createPipelineState(pipelineInfo);
    }

    void BasicTexture::createTexture() {
        auto img = new cc::Image();
        img->autorelease();
        bool valid = img->initWithImageFile("uv_checker_01.jpg");
        CCASSERT(valid, "BasicTexture load image failed");

        auto data = TestBaseI::RGB2RGBA(img);

        gfx::TextureInfo textureInfo;
        textureInfo.usage = gfx::TextureUsage::SAMPLED | gfx::TextureUsage::TRANSFER_DST;
        textureInfo.format = gfx::Format::RGBA8;
        textureInfo.width = img->getWidth();
        textureInfo.height = img->getHeight();
        _texture = _device->createTexture(textureInfo);

        gfx::BufferTextureCopy textureRegion;
        textureRegion.buffTexHeight = img->getHeight();
        textureRegion.texExtent.width = img->getWidth();
        textureRegion.texExtent.height = img->getHeight();
        textureRegion.texExtent.depth = 1;

        gfx::BufferTextureCopyList regions;
        regions.push_back(std::move(textureRegion));

        gfx::BufferDataList imageBuffer = { data };
        _device->copyBuffersToTexture(imageBuffer, _texture, regions);

        //create sampler
        gfx::SamplerInfo samplerInfo;
        _sampler = _device->createSampler(samplerInfo);

        delete[] data;
    }

    void BasicTexture::tick(float dt) {

        gfx::Rect render_area = { 0, 0, _device->getWidth(), _device->getHeight() };
        gfx::Color clear_color = { 0, 0, 0, 1.0f };

        _device->acquire();

        auto commandBuffer = _commandBuffers[0];
        commandBuffer->begin();
        commandBuffer->beginRenderPass(_fbo->getRenderPass(), _fbo, render_area, std::move(std::vector<gfx::Color>({ clear_color })), 1.0f, 0);
        commandBuffer->bindInputAssembler(_inputAssembler);
        commandBuffer->bindPipelineState(_pipelineState);
        commandBuffer->bindBindingLayout(_bindingLayout);
        commandBuffer->draw(_inputAssembler);
        commandBuffer->endRenderPass();
        commandBuffer->end();

        _device->getQueue()->submit(_commandBuffers);
        _device->present();
    }

} // namespace cc
