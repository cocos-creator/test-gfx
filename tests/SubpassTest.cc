#include "SubpassTest.h"
#include "tiny_obj_loader.h"

namespace cc {

enum class Binding : uint8_t {
    CAMERA,
    COLOR,
    MVP,
};

void SubpassTest::onDestroy() {
    CC_SAFE_DESTROY(_shaderForward)
    CC_SAFE_DESTROY(_shaderGBuffer)
    CC_SAFE_DESTROY(_vertexPositionBuffer)
    CC_SAFE_DESTROY(_vertexNormalBuffer)
    CC_SAFE_DESTROY(_indexBuffer)
    CC_SAFE_DESTROY(_inputAssembler)
    CC_SAFE_DESTROY(_descriptorSet)
    CC_SAFE_DESTROY(_descriptorSetLayout)
    CC_SAFE_DESTROY(_pipelineLayout)
    CC_SAFE_DESTROY(_pipelineState)

    for (auto *bufferView : _bufferViews) {
        CC_DESTROY(bufferView)
    }
    _bufferViews.clear();
    CC_SAFE_DESTROY(_rootUBO)

    CC_SAFE_DESTROY(_sampler)

    for (auto *gbuffer : _deferredGBuffers) {
        CC_DESTROY(gbuffer)
    }
    _deferredGBuffers.clear();
    CC_SAFE_DESTROY(_deferredGBufferDepthTexture)
    CC_SAFE_DESTROY(_deferredGBufferFramebuffer)
    CC_SAFE_DESTROY(_deferredGBufferRenderPass)
    CC_SAFE_DESTROY(_deferredGBufferPipelineState)

    CC_SAFE_DESTROY(_deferredRenderPass)
    CC_SAFE_DESTROY(_deferredShader)
    CC_SAFE_DESTROY(_deferredVB)
    CC_SAFE_DESTROY(_deferredDescriptorSet)
    CC_SAFE_DESTROY(_deferredDescriptorSetLayout)
    CC_SAFE_DESTROY(_deferredPipelineLayout)
    CC_SAFE_DESTROY(_deferredInputAssembler)
    CC_SAFE_DESTROY(_deferredPipelineState)
}

bool SubpassTest::onInit() {
    createShader();
    createBuffers();
    createInputAssembler();
    createPipelineState();

    createDeferredResources();
    return true;
}

namespace {
String standardStruct = R"(
    struct StandardSurface {
        vec4 albedo;
        vec3 position;
        vec3 normal;
        vec3 emissive;
        float roughness;
        float metallic;
        float occlusion;
    };
)";

String standardShading = R"(
    #define PI 3.14159265359

    float GGXMobile (float roughness, float NoH, vec3 H, vec3 N) {
        vec3 NxH = cross(N, H);
        float OneMinusNoHSqr = dot(NxH, NxH);
        float a = roughness * roughness;
        float n = NoH * a;
        float p = a / (OneMinusNoHSqr + n * n);
        return p * p;
    }

    float CalcSpecular (float roughness, float NoH, vec3 H, vec3 N) {
        return (roughness * 0.25 + 0.25) * GGXMobile(roughness, NoH, H, N);
    }

    vec3 BRDFApprox (vec3 specular, float roughness, float NoV) {
        const vec4 c0 = vec4(-1.0, -0.0275, -0.572, 0.022);
        const vec4 c1 = vec4(1.0, 0.0425, 1.04, -0.04);
        vec4 r = roughness * c0 + c1;
        float a004 = min(r.x * r.x, exp2(-9.28 * NoV)) * r.x + r.y;
        vec2 AB = vec2(-1.04, 1.04) * a004 + r.zw;
        AB.y *= clamp(50.0 * specular.g, 0.0, 1.0);
        return specular * AB.x + AB.y;
    }

    vec3 ACESToneMap (vec3 color) {
        color = min(color, vec3(8.0)); // guard against overflow
        const float A = 2.51;
        const float B = 0.03;
        const float C = 2.43;
        const float D = 0.59;
        const float E = 0.14;
        return (color * (A * color + B)) / (color * (C * color + D) + E);
    }

    vec3 LinearToSRGB(vec3 linear) {
        return sqrt(linear);
    }

    vec4 CCStandardShadingBase (StandardSurface s) {
        vec3 diffuse = s.albedo.rgb * (1.0 - s.metallic);
        vec3 specular = mix(vec3(0.04), s.albedo.rgb, s.metallic);

        // normals can be zero in deferred mode where nothing is drawn
        vec3 N = dot(s.normal, s.normal) > 0.0 ? normalize(s.normal) : vec3(0, 1, 0);
        vec3 V = normalize(cc_cameraPos.xyz - s.position);

        float NV = max(abs(dot(N, V)), 0.001);
        specular = BRDFApprox(specular, s.roughness, NV);

        vec3 L = normalize(-cc_mainLitDir.xyz);
        vec3 H = normalize(L + V);
        float NH = max(dot(N, H), 0.0);
        float NL = max(dot(N, L), 0.001);
        vec3 finalColor = NL * cc_mainLitColor.rgb * cc_mainLitColor.w;
        vec3 diffuseContrib = diffuse / PI;

        // Cook-Torrance Microfacet Specular BRDF
        vec3 specularContrib = specular * CalcSpecular(s.roughness, NH, H, N);

        finalColor *= diffuseContrib + specularContrib;

        finalColor = finalColor * s.occlusion;
        finalColor += s.emissive;

        finalColor = LinearToSRGB(ACESToneMap(finalColor));

        return vec4(finalColor, s.albedo.a);
    }
)";

ShaderSources<String> cameraUBO = {
    R"(
        layout(set = 0, binding = 0) uniform CCCamera {
            vec4 cc_cameraPos; // xyz: camera position
            vec4 cc_mainLitDir; // xyz: main direcitonal light direction
            vec4 cc_mainLitColor; // xyz: main direcitonal light color, w: intensity
        };
    )",
    R"(
        layout(std140) uniform CCCamera {
            vec4 cc_cameraPos; // xyz: camera position
            vec4 cc_mainLitDir; // xyz: main direcitonal light direction
            vec4 cc_mainLitColor; // xyz: main direcitonal light color, w: intensity
        };
    )",
    R"(
        uniform vec4 cc_cameraPos; // xyz: camera position
        uniform vec4 cc_mainLitDir; // xyz: main direcitonal light direction
        uniform vec4 cc_mainLitColor; // xyz: main direcitonal light color, w: intensity
    )",
};
gfx::UniformBlock cameraUBOInfo{
    0,
    static_cast<uint>(Binding::CAMERA),
    "CCCamera",
    {
        {"cc_cameraPos", gfx::Type::FLOAT4, 1},
        {"cc_mainLitDir", gfx::Type::FLOAT4, 1},
        {"cc_mainLitColor", gfx::Type::FLOAT4, 1},
    },
    1,
};
} // namespace

void SubpassTest::createShader() {
    String                vertMain = R"(
        void main () {
            vec4 pos = u_model * vec4(a_position, 1.0);
            v_position = pos.xyz;
            v_normal = (u_model * vec4(a_normal, 0.0)).xyz;

            gl_Position = u_projection * u_view * pos;
        }
    )";
    ShaderSources<String> vert;
    vert.glsl4 = R"(
        precision highp float;

        layout(location = 0) in vec3 a_position;
        layout(location = 1) in vec3 a_normal;

        layout(set = 0, binding = 2) uniform MVP_Matrix {
            mat4 u_model, u_view, u_projection;
        };

        layout(location = 0) out vec3 v_position;
        layout(location = 1) out vec3 v_normal;
    )" + vertMain;
    vert.glsl3 = R"(
        in vec3 a_position;
        in vec3 a_normal;

        layout(std140) uniform MVP_Matrix {
            mat4 u_model, u_view, u_projection;
        };

        out vec3 v_position;
        out vec3 v_normal;
    )" + vertMain;
    vert.glsl1 = R"(
        attribute vec3 a_position;
        attribute vec3 a_normal;

        uniform mat4 u_model, u_view, u_projection;

        varying vec3 v_position;
        varying vec3 v_normal;
    )" + vertMain;

    ShaderSources<String> fragBase;
    fragBase.glsl4 = R"(
        precision highp float;

        layout(set = 0, binding = 1) uniform Color {
            vec4 u_color;
        };

        layout(location = 0) in vec3 v_position;
        layout(location = 1) in vec3 v_normal;
    )";
    fragBase.glsl3 = R"(
        precision mediump float;

        layout(std140) uniform Color {
            vec4 u_color;
        };

        in vec3 v_position;
        in vec3 v_normal;
    )";
    fragBase.glsl1 = R"(
        precision mediump float;

        uniform vec4 u_color;

        varying vec3 v_position;
        varying vec3 v_normal;
    )";

    String surf = R"(
        void surf (out StandardSurface s) {
            s.albedo = u_color;
            s.position = v_position;
            s.normal = v_normal;
            s.emissive = vec3(0.03);
            s.roughness = 0.2;
            s.metallic = 0.8;
            s.occlusion = 1.0;
        }
    )";

    ShaderSources<String> forwardFrag = fragBase + cameraUBO + standardStruct + standardShading + surf;
    forwardFrag.glsl4 += R"(
        layout(location = 0) out vec4 o_color;
        void main () {
            StandardSurface s; surf(s);
            o_color = CCStandardShadingBase(s);
        }
    )";
    forwardFrag.glsl3 += R"(
        out vec4 o_color;
        void main () {
            StandardSurface s; surf(s);
            o_color = CCStandardShadingBase(s);
        }
    )";
    forwardFrag.glsl1 += R"(
        void main () {
            StandardSurface s; surf(s);
            gl_FragColor = CCStandardShadingBase(s);
        }
    )";

    ShaderSources<String> gbufferFrag = fragBase + cameraUBO + standardStruct + surf;
    gbufferFrag.glsl4 += R"(
        layout(location = 0) out vec4 o_color0;
        layout(location = 1) out vec4 o_color1;
        layout(location = 2) out vec4 o_color2;
        layout(location = 3) out vec4 o_color3;

        void main () {
            StandardSurface s; surf(s);

            o_color0 = s.albedo;
            o_color1 = vec4(s.position, s.roughness);
            o_color2 = vec4(s.normal, s.metallic);
            o_color3 = vec4(s.emissive, s.occlusion);
        }
    )";
    gbufferFrag.glsl3 += R"(
        layout(location = 0) out vec4 o_color0;
        layout(location = 1) out vec4 o_color1;
        layout(location = 2) out vec4 o_color2;
        layout(location = 3) out vec4 o_color3;

        void main () {
            StandardSurface s; surf(s);

            o_color0 = s.albedo;
            o_color1 = vec4(s.position, s.roughness);
            o_color2 = vec4(s.normal, s.metallic);
            o_color3 = vec4(s.emissive, s.occlusion);
        }
    )";
    gbufferFrag.glsl1 = R"(
        #ifdef GL_EXT_draw_buffers
            #extension GL_EXT_draw_buffers: enable
        #endif
    )" + gbufferFrag.glsl1;
    gbufferFrag.glsl1 += R"(
        void main () {
            StandardSurface s; surf(s);

            gl_FragData[0] = s.albedo;
            gl_FragData[1] = vec4(s.position, s.roughness);
            gl_FragData[2] = vec4(s.normal, s.metallic);
            gl_FragData[3] = vec4(s.emissive, s.occlusion);
        }
    )";

    gfx::ShaderStageList shaderStageList{
        {gfx::ShaderStageFlagBit::VERTEX, getAppropriateShaderSource(vert)},
        {gfx::ShaderStageFlagBit::FRAGMENT, getAppropriateShaderSource(forwardFrag)},
    };
    gfx::AttributeList attributeList{
        {"a_position", gfx::Format::RGB32F, false, 0, false, 0},
        {"a_normal", gfx::Format::RGB32F, false, 1, false, 1},
    };
    gfx::UniformBlockList uniformBlockList{
        cameraUBOInfo,
        {
            0,
            static_cast<uint>(Binding::MVP),
            "MVP_Matrix",
            {
                {"u_model", gfx::Type::MAT4, 1},
                {"u_view", gfx::Type::MAT4, 1},
                {"u_projection", gfx::Type::MAT4, 1},
            },
            1,
        },
        {
            0,
            static_cast<uint>(Binding::COLOR),
            "Color",
            {
                {"u_color", gfx::Type::FLOAT4, 1},
            },
            1,
        },
    };

    gfx::ShaderInfo shaderInfo;
    shaderInfo.name       = "Standard Forward";
    shaderInfo.stages     = std::move(shaderStageList);
    shaderInfo.attributes = std::move(attributeList);
    shaderInfo.blocks     = std::move(uniformBlockList);
    _shaderForward        = _device->createShader(shaderInfo);

    shaderInfo.name             = "Standard GBuffer";
    shaderInfo.stages[1].source = getAppropriateShaderSource(gbufferFrag);
    _shaderGBuffer              = _device->createShader(shaderInfo);
}

void SubpassTest::createDeferredResources() {
    gfx::SamplerInfo samplerInfo;
    _sampler = _device->createSampler(samplerInfo);

    gfx::RenderPassInfo rpInfo;
    for (uint i = 0; i < 4; ++i) {
        _deferredGBuffers.push_back(_device->createTexture({
            gfx::TextureType::TEX2D,
            gfx::TextureUsageBit::SAMPLED | gfx::TextureUsageBit::COLOR_ATTACHMENT,
            gfx::Format::RGBA16F,
            _device->getWidth(),
            _device->getHeight(),
        }));
        rpInfo.colorAttachments.emplace_back();
        rpInfo.colorAttachments.back().format         = gfx::Format::RGBA16F;
        rpInfo.colorAttachments.back().endAccesses[0] = gfx::AccessType::FRAGMENT_SHADER_READ_TEXTURE;
    }
    _deferredGBufferDepthTexture         = _device->createTexture({
        gfx::TextureType::TEX2D,
        gfx::TextureUsageBit::DEPTH_STENCIL_ATTACHMENT,
        _device->getDepthStencilFormat(),
        _device->getWidth(),
        _device->getHeight(),
    });
    rpInfo.depthStencilAttachment.format = _device->getDepthStencilFormat();

    _deferredGBufferRenderPass  = _device->createRenderPass(rpInfo);
    _deferredGBufferFramebuffer = _device->createFramebuffer({
        _deferredGBufferRenderPass,
        _deferredGBuffers,
        _deferredGBufferDepthTexture,
    });

    gfx::PipelineStateInfo gbufferPSOInfo;
    gbufferPSOInfo.primitive      = gfx::PrimitiveMode::TRIANGLE_LIST;
    gbufferPSOInfo.shader         = _shaderGBuffer;
    gbufferPSOInfo.inputState     = {_inputAssembler->getAttributes()};
    gbufferPSOInfo.renderPass     = _deferredGBufferRenderPass;
    gbufferPSOInfo.pipelineLayout = _pipelineLayout;
    gbufferPSOInfo.blendState.targets.resize(4);
    _deferredGBufferPipelineState = _device->createPipelineState(gbufferPSOInfo);

    ShaderSources<String> vert = {
        R"(
            precision highp float;
            layout(location = 0) in vec2 a_position;
            layout(location = 1) in vec2 a_texCoord;

            layout(location = 0) out vec2 v_texCoord;

            void main() {
                v_texCoord = a_texCoord;
                gl_Position = vec4(a_position, 0, 1);
            }
        )",
        R"(
            in vec2 a_position;
            in vec2 a_texCoord;

            out vec2 v_texCoord;

            void main() {
                v_texCoord = a_texCoord;
                gl_Position = vec4(a_position, 0, 1);
            }
        )",
        R"(
            attribute vec2 a_position;
            attribute vec2 a_texCoord;

            varying vec2 v_texCoord;

            void main() {
                v_texCoord = a_texCoord;
                gl_Position = vec4(a_position, 0, 1);
            }
        )",
    };
    ShaderSources<String> frag = {
        R"(
            precision highp float;
            layout(location = 0) in vec2 v_texCoord;

            layout(set = 0, binding = 1) uniform sampler2D gbuffer1;
            layout(set = 0, binding = 2) uniform sampler2D gbuffer2;
            layout(set = 0, binding = 3) uniform sampler2D gbuffer3;
            layout(set = 0, binding = 4) uniform sampler2D gbuffer4;
        )",
        R"(
            precision highp float;
            in vec2 v_texCoord;

            uniform sampler2D gbuffer1;
            uniform sampler2D gbuffer2;
            uniform sampler2D gbuffer3;
            uniform sampler2D gbuffer4;
        )",
        R"(
            precision highp float;
            varying vec2 v_texCoord;

            uniform sampler2D gbuffer1;
            uniform sampler2D gbuffer2;
            uniform sampler2D gbuffer3;
            uniform sampler2D gbuffer4;
        )",
    };
    frag += cameraUBO + standardStruct + standardShading;
    frag.glsl4 += R"(
        layout(location = 0) out vec4 o_color;
        void main() {

            vec4 g1 = texture(gbuffer1, v_texCoord);
            vec4 g2 = texture(gbuffer2, v_texCoord);
            vec4 g3 = texture(gbuffer3, v_texCoord);
            vec4 g4 = texture(gbuffer4, v_texCoord);

            StandardSurface s;
            s.albedo = g1;
            s.position = g2.xyz;
            s.roughness = g2.w;
            s.normal = g3.xyz;
            s.metallic = g3.w;
            s.emissive = g4.xyz;
            s.occlusion = g4.w;

            o_color = CCStandardShadingBase(s);
        }
    )";
    frag.glsl3 += R"(
        out vec4 o_color;
        void main() {

            vec4 g1 = texture(gbuffer1, v_texCoord);
            vec4 g2 = texture(gbuffer2, v_texCoord);
            vec4 g3 = texture(gbuffer3, v_texCoord);
            vec4 g4 = texture(gbuffer4, v_texCoord);

            StandardSurface s;
            s.albedo = g1;
            s.position = g2.xyz;
            s.roughness = g2.w;
            s.normal = g3.xyz;
            s.metallic = g3.w;
            s.emissive = g4.xyz;
            s.occlusion = g4.w;

            o_color = CCStandardShadingBase(s);
        }
    )";
    frag.glsl1 += R"(
        void main() {

            vec4 g1 = texture2D(gbuffer1, v_texCoord);
            vec4 g2 = texture2D(gbuffer2, v_texCoord);
            vec4 g3 = texture2D(gbuffer3, v_texCoord);
            vec4 g4 = texture2D(gbuffer4, v_texCoord);

            StandardSurface s;
            s.albedo = g1;
            s.position = g2.xyz;
            s.roughness = g2.w;
            s.normal = g3.xyz;
            s.metallic = g3.w;
            s.emissive = g4.xyz;
            s.occlusion = g4.w;

            gl_FragColor = CCStandardShadingBase(s);
        }
    )";

    gfx::ShaderInfo shaderInfo;

    shaderInfo.name = "Standard Deferred";
    shaderInfo.stages.push_back({gfx::ShaderStageFlagBit::VERTEX, getAppropriateShaderSource(vert)});
    shaderInfo.stages.push_back({gfx::ShaderStageFlagBit::FRAGMENT, getAppropriateShaderSource(frag)});

    shaderInfo.attributes.push_back({"a_position", gfx::Format::RG32F, false, 0, false, 0});
    shaderInfo.attributes.push_back({"a_texCoord", gfx::Format::RG32F, false, 0, false, 1});

    shaderInfo.blocks.push_back(cameraUBOInfo);

    shaderInfo.samplerTextures.push_back({0, 1, "gbuffer1", gfx::Type::SAMPLER2D, 1});
    shaderInfo.samplerTextures.push_back({0, 2, "gbuffer2", gfx::Type::SAMPLER2D, 1});
    shaderInfo.samplerTextures.push_back({0, 3, "gbuffer3", gfx::Type::SAMPLER2D, 1});
    shaderInfo.samplerTextures.push_back({0, 4, "gbuffer4", gfx::Type::SAMPLER2D, 1});

    _deferredShader = _device->createShader(shaderInfo);

    float         ySign = _device->getCapabilities().screenSpaceSignY;
    vector<float> vb{
        -1.F, -1.F * ySign, 0.F, 1.F,
        1.F, -1.F * ySign, 1.F, 1.F,
        -1.F, 1.F * ySign, 0.F, 0.F,
        1.F, 1.F * ySign, 1.F, 0.F};
    _deferredVB = _device->createBuffer({
        gfx::BufferUsage::VERTEX,
        gfx::MemoryUsage::DEVICE,
        static_cast<uint>(vb.size() * sizeof(float)),
        4 * sizeof(float),
    });
    _deferredVB->update(vb.data(), vb.size() * sizeof(float));

    gfx::InputAssemblerInfo iaInfo;
    iaInfo.attributes = shaderInfo.attributes;
    iaInfo.vertexBuffers.emplace_back(_deferredVB);
    _deferredInputAssembler = _device->createInputAssembler(iaInfo);

    gfx::DescriptorSetLayoutInfo dslInfo;
    dslInfo.bindings.push_back({0, gfx::DescriptorType::UNIFORM_BUFFER, 1, gfx::ShaderStageFlagBit::FRAGMENT});
    dslInfo.bindings.push_back({1, gfx::DescriptorType::SAMPLER_TEXTURE, 1, gfx::ShaderStageFlagBit::FRAGMENT});
    dslInfo.bindings.push_back({2, gfx::DescriptorType::SAMPLER_TEXTURE, 1, gfx::ShaderStageFlagBit::FRAGMENT});
    dslInfo.bindings.push_back({3, gfx::DescriptorType::SAMPLER_TEXTURE, 1, gfx::ShaderStageFlagBit::FRAGMENT});
    dslInfo.bindings.push_back({4, gfx::DescriptorType::SAMPLER_TEXTURE, 1, gfx::ShaderStageFlagBit::FRAGMENT});
    _deferredDescriptorSetLayout = _device->createDescriptorSetLayout(dslInfo);

    _deferredPipelineLayout = _device->createPipelineLayout({{_deferredDescriptorSetLayout}});

    _deferredDescriptorSet = _device->createDescriptorSet({_deferredDescriptorSetLayout});

    _deferredDescriptorSet->bindBuffer(static_cast<uint>(Binding::CAMERA), _bufferViews[2]);
    _deferredDescriptorSet->bindTexture(1, _deferredGBuffers[0]);
    _deferredDescriptorSet->bindTexture(2, _deferredGBuffers[1]);
    _deferredDescriptorSet->bindTexture(3, _deferredGBuffers[2]);
    _deferredDescriptorSet->bindTexture(4, _deferredGBuffers[3]);
    _deferredDescriptorSet->bindSampler(1, _sampler);
    _deferredDescriptorSet->bindSampler(2, _sampler);
    _deferredDescriptorSet->bindSampler(3, _sampler);
    _deferredDescriptorSet->bindSampler(4, _sampler);
    _deferredDescriptorSet->update();

    gfx::PipelineStateInfo pipelineStateInfo;
    pipelineStateInfo.primitive                    = gfx::PrimitiveMode::TRIANGLE_STRIP;
    pipelineStateInfo.shader                       = _deferredShader;
    pipelineStateInfo.inputState                   = {_deferredInputAssembler->getAttributes()};
    pipelineStateInfo.renderPass                   = _fbo->getRenderPass();
    pipelineStateInfo.pipelineLayout               = _deferredPipelineLayout;
    pipelineStateInfo.depthStencilState.depthTest  = false;
    pipelineStateInfo.depthStencilState.depthWrite = false;
    _deferredPipelineState                         = _device->createPipelineState(pipelineStateInfo);

    gfx::RenderPassInfo deferredRPInfo;
    deferredRPInfo.colorAttachments.emplace_back();
    deferredRPInfo.colorAttachments.back().format        = _device->getColorFormat();
    deferredRPInfo.colorAttachments.back().loadOp        = gfx::LoadOp::DISCARD;
    deferredRPInfo.depthStencilAttachment.format         = _device->getDepthStencilFormat();
    deferredRPInfo.depthStencilAttachment.depthLoadOp    = gfx::LoadOp::DISCARD;
    deferredRPInfo.depthStencilAttachment.depthStoreOp   = gfx::StoreOp::DISCARD;
    deferredRPInfo.depthStencilAttachment.stencilLoadOp  = gfx::LoadOp::DISCARD;
    deferredRPInfo.depthStencilAttachment.stencilStoreOp = gfx::StoreOp::DISCARD;
    _deferredRenderPass                                  = _device->createRenderPass(deferredRPInfo);
}

void SubpassTest::createBuffers() {
    auto obj = loadOBJ("bunny.obj");

    // vertex buffer
    const auto &positions = obj.GetAttrib().vertices;
    _vertexPositionBuffer = _device->createBuffer({
        gfx::BufferUsage::VERTEX,
        gfx::MemoryUsage::DEVICE,
        static_cast<uint>(positions.size() * sizeof(float)),
        3 * sizeof(float),
    });
    _vertexPositionBuffer->update(positions.data(), positions.size() * sizeof(float));

    const auto &normals = obj.GetAttrib().normals;
    _vertexNormalBuffer = _device->createBuffer({
        gfx::BufferUsage::VERTEX,
        gfx::MemoryUsage::DEVICE,
        static_cast<uint>(normals.size() * sizeof(float)),
        3 * sizeof(float),
    });
    _vertexNormalBuffer->update(normals.data(), normals.size() * sizeof(float));

    // index buffer
    const auto &     indicesInfo = obj.GetShapes()[0].mesh.indices;
    vector<uint16_t> indices;
    indices.reserve(indicesInfo.size());
    std::transform(indicesInfo.begin(), indicesInfo.end(), std::back_inserter(indices),
                   [](auto &&info) { return static_cast<uint16_t>(info.vertex_index); });

    _indexBuffer = _device->createBuffer({
        gfx::BufferUsage::INDEX,
        gfx::MemoryUsage::DEVICE,
        static_cast<uint>(indices.size() * sizeof(uint16_t)),
        sizeof(uint16_t),
    });
    _indexBuffer->update(indices.data(), indices.size() * sizeof(uint16_t));

    vector<uint> sizes{3 * sizeof(Mat4), sizeof(Vec4), 3 * sizeof(Vec4)};
    createUberBuffer(_device, sizes, &_rootUBO, &_bufferViews, &_bufferViewOffsets);
    _rootBuffer.resize(_bufferViewOffsets.back() / sizeof(float));

    constexpr float cameraDistance = 5.F;

    Vec4 cameraPos{cameraDistance, cameraDistance * 0.5F, cameraDistance * 0.3F, 0.F};
    Vec4 color{201 / 255.F, 140 / 255.F, 34 / 255.F, 1.0F};
    Vec4 lightDir{0.F, -2.F, -1.F, 0.F};
    Vec4 lightColor{252 / 255.F, 123 / 255.F, 3 / 255.F, 30.0F};
    Mat4 view;
    Mat4::createLookAt(Vec3(cameraPos.x, cameraPos.y, cameraPos.z),
                       Vec3(0.0F, 1.F, 0.0F), Vec3(0.0F, 1.0F, 0.F), &view);
    lightDir.normalize();

    std::copy(view.m, view.m + 16, &_rootBuffer[16]);
    std::copy(&color.x, &color.x + 4, &_rootBuffer[_bufferViewOffsets[1] / sizeof(float)]);
    std::copy(&cameraPos.x, &cameraPos.x + 4, &_rootBuffer[_bufferViewOffsets[2] / sizeof(float)]);
    std::copy(&lightDir.x, &lightDir.x + 4, &_rootBuffer[_bufferViewOffsets[2] / sizeof(float) + 4]);
    std::copy(&lightColor.x, &lightColor.x + 4, &_rootBuffer[_bufferViewOffsets[2] / sizeof(float) + 8]);
}

void SubpassTest::createInputAssembler() {
    gfx::InputAssemblerInfo inputAssemblerInfo;
    inputAssemblerInfo.attributes.push_back({"a_position", gfx::Format::RGB32F, false, 0, false});
    inputAssemblerInfo.attributes.push_back({"a_normal", gfx::Format::RGB32F, false, 1, false});
    inputAssemblerInfo.vertexBuffers.emplace_back(_vertexPositionBuffer);
    inputAssemblerInfo.vertexBuffers.emplace_back(_vertexNormalBuffer);
    inputAssemblerInfo.indexBuffer = _indexBuffer;
    _inputAssembler                = _device->createInputAssembler(inputAssemblerInfo);
}

void SubpassTest::createPipelineState() {
    gfx::DescriptorSetLayoutInfo dslInfo;
    dslInfo.bindings.push_back({0, gfx::DescriptorType::UNIFORM_BUFFER, 1, gfx::ShaderStageFlagBit::FRAGMENT});
    dslInfo.bindings.push_back({1, gfx::DescriptorType::UNIFORM_BUFFER, 1, gfx::ShaderStageFlagBit::FRAGMENT});
    dslInfo.bindings.push_back({2, gfx::DescriptorType::UNIFORM_BUFFER, 1, gfx::ShaderStageFlagBit::VERTEX});
    _descriptorSetLayout = _device->createDescriptorSetLayout(dslInfo);

    _pipelineLayout = _device->createPipelineLayout({{_descriptorSetLayout}});

    _descriptorSet = _device->createDescriptorSet({_descriptorSetLayout});

    _descriptorSet->bindBuffer(static_cast<uint>(Binding::MVP), _bufferViews[0]);
    _descriptorSet->bindBuffer(static_cast<uint>(Binding::COLOR), _bufferViews[1]);
    _descriptorSet->bindBuffer(static_cast<uint>(Binding::CAMERA), _bufferViews[2]);
    _descriptorSet->update();

    gfx::PipelineStateInfo pipelineStateInfo;
    pipelineStateInfo.primitive      = gfx::PrimitiveMode::TRIANGLE_LIST;
    pipelineStateInfo.shader         = _shaderForward;
    pipelineStateInfo.inputState     = {_inputAssembler->getAttributes()};
    pipelineStateInfo.renderPass     = _fbo->getRenderPass();
    pipelineStateInfo.pipelineLayout = _pipelineLayout;
    _pipelineState                   = _device->createPipelineState(pipelineStateInfo);

    _globalBarriers.push_back(TestBaseI::getGlobalBarrier({
        {
            gfx::AccessType::TRANSFER_WRITE,
        },
        {
            gfx::AccessType::VERTEX_SHADER_READ_UNIFORM_BUFFER,
            gfx::AccessType::FRAGMENT_SHADER_READ_UNIFORM_BUFFER,
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
            gfx::AccessType::FRAGMENT_SHADER_READ_UNIFORM_BUFFER,
        },
    }));
}

void SubpassTest::onSpacePressed() {
    _useDeferred = !_useDeferred;
    CC_LOG_INFO("Shading mode switched to: %s", _useDeferred ? "Deferred" : "Forward");
}

void SubpassTest::onTick() {
    uint globalBarrierIdx = _frameCount ? 1 : 0;

    gfx::Extent orientedSize = TestBaseI::getOrientedSurfaceSize();
    Mat4::createRotationY(_time, &_worldMatrix);
    TestBaseI::createPerspective(60.0F, 1.0F * orientedSize.width / orientedSize.height, 0.01F, 1000.0F, &_projectionMatrix, _useDeferred);

    std::copy(_worldMatrix.m, _worldMatrix.m + 16, &_rootBuffer[0]);
    std::copy(_projectionMatrix.m, _projectionMatrix.m + 16, &_rootBuffer[32]);

    static const gfx::ColorList CLEAR_COLOR = {{0, 0, 0, 1}, {0, 0, 0, 1}, {0, 0, 0, 1}, {0, 0, 0, 1}};

    _device->acquire();

    _rootUBO->update(_rootBuffer.data(), _rootBuffer.size() * sizeof(float));
    gfx::Rect renderArea = {0, 0, _device->getWidth(), _device->getHeight()};

    auto *commandBuffer = _commandBuffers[0];
    commandBuffer->begin();

    if (TestBaseI::MANUAL_BARRIER) {
        commandBuffer->pipelineBarrier(_globalBarriers[globalBarrierIdx]);
    }

    if (_useDeferred) {
        commandBuffer->beginRenderPass(_deferredGBufferRenderPass, _deferredGBufferFramebuffer, renderArea, CLEAR_COLOR.data(), 1.0F, 0);

        commandBuffer->bindInputAssembler(_inputAssembler);
        commandBuffer->bindPipelineState(_deferredGBufferPipelineState);
        commandBuffer->bindDescriptorSet(0, _descriptorSet);
        commandBuffer->draw(_inputAssembler);

        commandBuffer->endRenderPass();

        commandBuffer->beginRenderPass(_deferredRenderPass, _fbo, renderArea, CLEAR_COLOR.data(), 1.0F, 0);

        commandBuffer->bindInputAssembler(_deferredInputAssembler);
        commandBuffer->bindPipelineState(_deferredPipelineState);
        commandBuffer->bindDescriptorSet(0, _deferredDescriptorSet);
        commandBuffer->draw(_deferredInputAssembler);

        commandBuffer->endRenderPass();
    } else {
        commandBuffer->beginRenderPass(_fbo->getRenderPass(), _fbo, renderArea, CLEAR_COLOR.data(), 1.0F, 0);

        commandBuffer->bindInputAssembler(_inputAssembler);
        commandBuffer->bindPipelineState(_pipelineState);
        commandBuffer->bindDescriptorSet(0, _descriptorSet);
        commandBuffer->draw(_inputAssembler);

        commandBuffer->endRenderPass();
    }

    commandBuffer->end();

    _device->flushCommands(_commandBuffers);
    _device->getQueue()->submit(_commandBuffers);
    _device->present();
}

} // namespace cc
