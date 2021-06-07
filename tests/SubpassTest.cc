#include "SubpassTest.h"
#include <cmath>
#include "gfx-base/GFXDef-common.h"
#include "tiny_obj_loader.h"

namespace cc {

namespace {
constexpr uint REPEAT = 1U;
}

enum class Binding : uint8_t {
    CAMERA,
    COLOR,
    MVP,
};

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

        float fAmb = 0.5 - N.y * 0.5;
        vec3 ambDiff = mix(cc_ambientSky.rgb, cc_ambientGround.rgb, fAmb) * cc_ambientSky.w;
        finalColor += (ambDiff.rgb * diffuse);

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
            vec4 cc_ambientSky; //xyz: sky illumination color, w: intensity
            vec4 cc_ambientGround; // xyz: ground albedo color
        };
    )",
    R"(
        layout(std140) uniform CCCamera {
            vec4 cc_cameraPos; // xyz: camera position
            vec4 cc_mainLitDir; // xyz: main direcitonal light direction
            vec4 cc_mainLitColor; // xyz: main direcitonal light color, w: intensity
            vec4 cc_ambientSky; //xyz: sky illumination color, w: intensity
            vec4 cc_ambientGround; // xyz: ground albedo color
        };
    )",
    R"(
        uniform vec4 cc_cameraPos; // xyz: camera position
        uniform vec4 cc_mainLitDir; // xyz: main direcitonal light direction
        uniform vec4 cc_mainLitColor; // xyz: main direcitonal light color, w: intensity
        uniform vec4 cc_ambientSky; //xyz: sky illumination color, w: intensity
        uniform vec4 cc_ambientGround; // xyz: ground albedo color
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
        {"cc_ambientSky", gfx::Type::FLOAT4, 1},
        {"cc_ambientGround", gfx::Type::FLOAT4, 1},
    },
    1,
};

String declExtensions = R"(
    #define GBUFFER_STORAGE_TEXTURE 0
    #define GBUFFER_STORAGE_PLS 1
    #define GBUFFER_STORAGE_FBF 2

    //#define GBUFFER_STORAGE GBUFFER_STORAGE_TEXTURE
    /* */
    #if defined(GL_EXT_shader_framebuffer_fetch)
    #   extension GL_EXT_shader_framebuffer_fetch: enable
    #   define GBUFFER_STORAGE GBUFFER_STORAGE_FBF
    #elif __VERSION__ > 100 && defined(GL_EXT_shader_pixel_local_storage)
    #   extension GL_EXT_shader_pixel_local_storage: enable
    #   define GBUFFER_STORAGE GBUFFER_STORAGE_PLS
    #else
    #   define GBUFFER_STORAGE GBUFFER_STORAGE_TEXTURE
    #endif
    /* */
)";

String declPLSData(gfx::MemoryAccess access = gfx::MemoryAccessBit::READ_WRITE) {
    String prefix;
    switch (access) {
        case gfx::MemoryAccess::READ_ONLY: prefix = "_in"; break;
        case gfx::MemoryAccess::WRITE_ONLY: prefix = "_out"; break;
        default: break;
    }
    return StringUtil::format(
        R"(
            // The guaranteed minimum available PLS size is 16 bytes
            layout(rgba8) __pixel_local%sEXT FragColor {
                vec4 gbuffer0;
                vec4 gbuffer1;
                vec4 gbuffer2;
                vec4 gbuffer3;
            };
        )",
        prefix.c_str());
}
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
            s.emissive = vec3(0);
            s.roughness = 0.3;
            s.metallic = 0.6;
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
    gbufferFrag.glsl3 = declExtensions + gbufferFrag.glsl3;
    gbufferFrag.glsl3 += R"(
        #if GBUFFER_STORAGE == GBUFFER_STORAGE_PLS
    )" + declPLSData(gfx::MemoryAccessBit::WRITE_ONLY);
    gbufferFrag.glsl3 += R"(
        #else
            layout(location = 0) out vec4 gbuffer0;
            layout(location = 1) out vec4 gbuffer1;
            layout(location = 2) out vec4 gbuffer2;
            layout(location = 3) out vec4 gbuffer3;
        #endif

        void main () {
            StandardSurface s; surf(s);

            gbuffer0 = s.albedo;
            gbuffer1 = vec4(s.position, s.roughness);
            gbuffer2 = vec4(s.normal, s.metallic);
            gbuffer3 = vec4(s.emissive, s.occlusion);
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
    _shaderForward.reset(device->createShader(shaderInfo));

    shaderInfo.name             = "Standard GBuffer";
    shaderInfo.stages[1].source = getAppropriateShaderSource(gbufferFrag);
    _shaderGBuffer.reset(device->createShader(shaderInfo));
}

void SubpassTest::createDeferredResources() {
    gfx::SamplerInfo samplerInfo;
    _sampler.reset(device->createSampler(samplerInfo));
    gfx::RenderPassInfo rpInfo;

    for (uint i = 0; i < 4; ++i) {
        // RGBA8 is suffice for albedo, emission & occlusion
        gfx::Format       format          = i % 3 ? gfx::Format::RGBA16F : gfx::Format::RGBA8;
        gfx::TextureUsage usage           = gfx::TextureUsageBit::INPUT_ATTACHMENT | gfx::TextureUsageBit::COLOR_ATTACHMENT;
        gfx::TextureFlags flags           = gfx::TextureFlagBit::NONE;
        gfx::StoreOp      storeOp         = gfx::StoreOp::DISCARD;
        gfx::AccessType   accessType      = gfx::AccessType::FRAGMENT_SHADER_READ_COLOR_INPUT_ATTACHMENT;
        bool              isGeneralLayout = false;
        if (i == 3) { // use the emission buffer as output
            usage |= gfx::TextureUsageBit::TRANSFER_SRC;
            flags |= gfx::TextureFlagBit::GENERAL_LAYOUT;
            storeOp         = gfx::StoreOp::STORE;
            accessType      = gfx::AccessType::TRANSFER_READ;
            isGeneralLayout = true;
        }
        _deferredGBuffers.emplace_back(device->createTexture({
            gfx::TextureType::TEX2D,
            usage,
            format,
            device->getWidth(),
            device->getHeight(),
            flags,
        }));
        rpInfo.colorAttachments.emplace_back();
        rpInfo.colorAttachments.back().storeOp         = storeOp;
        rpInfo.colorAttachments.back().format          = format;
        rpInfo.colorAttachments.back().endAccesses[0]  = accessType;
        rpInfo.colorAttachments.back().isGeneralLayout = isGeneralLayout;
    }

    rpInfo.depthStencilAttachment.format         = device->getDepthStencilFormat();
    rpInfo.depthStencilAttachment.depthStoreOp   = gfx::StoreOp::DISCARD;
    rpInfo.depthStencilAttachment.stencilStoreOp = gfx::StoreOp::DISCARD;

    _deferredGBufferDepthTexture.reset(device->createTexture({
        gfx::TextureType::TEX2D,
        gfx::TextureUsageBit::DEPTH_STENCIL_ATTACHMENT,
        device->getDepthStencilFormat(),
        device->getWidth(),
        device->getHeight(),
    }));

    rpInfo.subpasses.resize(2);
    rpInfo.subpasses[0].colors = {0, 1, 2, 3};
    rpInfo.subpasses[1].colors = {3};
    rpInfo.subpasses[1].inputs = {0, 1, 2, 3};

    rpInfo.dependencies.push_back({
        0,
        1,
        {gfx::AccessType::COLOR_ATTACHMENT_WRITE},
        {gfx::AccessType::FRAGMENT_SHADER_READ_COLOR_INPUT_ATTACHMENT},
    });

    rpInfo.dependencies.push_back({
        1,
        gfx::SUBPASS_EXTERNAL,
        {gfx::AccessType::COLOR_ATTACHMENT_WRITE},
        {gfx::AccessType::TRANSFER_READ},
    });

    _deferredGBufferRenderPass.reset(device->createRenderPass(rpInfo));
    _deferredGBufferFramebuffer.reset(device->createFramebuffer({
        _deferredGBufferRenderPass.get(),
        {
            _deferredGBuffers[0].get(),
            _deferredGBuffers[1].get(),
            _deferredGBuffers[2].get(),
            _deferredGBuffers[3].get(),
        },
        _deferredGBufferDepthTexture.get(),
    }));

    gfx::PipelineStateInfo gbufferPSOInfo;
    gbufferPSOInfo.primitive      = gfx::PrimitiveMode::TRIANGLE_LIST;
    gbufferPSOInfo.shader         = _shaderGBuffer.get();
    gbufferPSOInfo.inputState     = {_inputAssembler->getAttributes()};
    gbufferPSOInfo.renderPass     = _deferredGBufferRenderPass.get();
    gbufferPSOInfo.pipelineLayout = _pipelineLayout.get();
    gbufferPSOInfo.blendState.targets.resize(4);
    _deferredGBufferPipelineState.reset(device->createPipelineState(gbufferPSOInfo));

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

            layout(input_attachment_index = 0, set = 0, binding = 1) uniform subpassInput gbuffer0;
            layout(input_attachment_index = 1, set = 0, binding = 2) uniform subpassInput gbuffer1;
            layout(input_attachment_index = 2, set = 0, binding = 3) uniform subpassInput gbuffer2;
            layout(input_attachment_index = 3, set = 0, binding = 4) uniform subpassInput gbuffer3;
        )",
        declExtensions + R"(
            precision highp float;
            in vec2 v_texCoord;

            #if GBUFFER_STORAGE == GBUFFER_STORAGE_FBF
                layout(location = 0) inout vec4 gbuffer0;
                layout(location = 1) inout vec4 gbuffer1;
                layout(location = 2) inout vec4 gbuffer2;
                layout(location = 3) inout vec4 gbuffer3;
            #elif GBUFFER_STORAGE == GBUFFER_STORAGE_PLS
        )" + declPLSData(gfx::MemoryAccessBit::READ_ONLY) +
            R"(
                layout(location = 0) out vec4 o_color;
            #else
                uniform sampler2D gbuffer0;
                uniform sampler2D gbuffer1;
                uniform sampler2D gbuffer2;
                uniform sampler2D gbuffer3;
                layout(location = 0) out vec4 o_color;
            #endif
        )",
        declExtensions + R"(
            #if GBUFFER_STORAGE == GBUFFER_STORAGE_FBF
            #   ifdef GL_EXT_draw_buffers
            #       extension GL_EXT_draw_buffers: enable
            #   endif
            #endif

            precision highp float;
            varying vec2 v_texCoord;

            #if !GBUFFER_STORAGE
                uniform sampler2D gbuffer0;
                uniform sampler2D gbuffer1;
                uniform sampler2D gbuffer2;
                uniform sampler2D gbuffer3;
            #endif
        )",
    };
    frag += cameraUBO + standardStruct + standardShading;
    frag.glsl4 += R"(
        layout(location = 0) out vec4 o_color;
        void main() {

            vec4 g1 = subpassLoad(gbuffer0);
            vec4 g2 = subpassLoad(gbuffer1);
            vec4 g3 = subpassLoad(gbuffer2);
            vec4 g4 = subpassLoad(gbuffer3);

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
        void main() {

            #if GBUFFER_STORAGE
                // [PVRVFrame driver bug] Don't use PLS data inside vector constructors
                vec4 g0 = gbuffer0;
                vec4 g1 = gbuffer1;
                vec4 g2 = gbuffer2;
                vec4 g3 = gbuffer3;
            #else
                vec4 g0 = texture(gbuffer0, v_texCoord);
                vec4 g1 = texture(gbuffer1, v_texCoord);
                vec4 g2 = texture(gbuffer2, v_texCoord);
                vec4 g3 = texture(gbuffer3, v_texCoord);
            #endif

            StandardSurface s;
            s.albedo = g0;
            s.position = g1.xyz;
            s.roughness = g1.w;
            s.normal = g2.xyz;
            s.metallic = g2.w;
            s.emissive = g3.xyz;
            s.occlusion = g3.w;

            #if GBUFFER_STORAGE == GBUFFER_STORAGE_FBF
                gbuffer3 = CCStandardShadingBase(s);
            #else
                o_color = CCStandardShadingBase(s);
            #endif
        }
    )";
    frag.glsl1 += R"(
        void main() {

            #if GBUFFER_STORAGE == GBUFFER_STORAGE_FBF
                vec4 g0 = gl_LastFragData[0];
                vec4 g1 = gl_LastFragData[1];
                vec4 g2 = gl_LastFragData[2];
                vec4 g3 = gl_LastFragData[3];
            #else
                vec4 g0 = texture2D(gbuffer0, v_texCoord);
                vec4 g1 = texture2D(gbuffer1, v_texCoord);
                vec4 g2 = texture2D(gbuffer2, v_texCoord);
                vec4 g3 = texture2D(gbuffer3, v_texCoord);
            #endif

            StandardSurface s;
            s.albedo = g0;
            s.position = g1.xyz;
            s.roughness = g1.w;
            s.normal = g2.xyz;
            s.metallic = g2.w;
            s.emissive = g3.xyz;
            s.occlusion = g3.w;

            #if GBUFFER_STORAGE == GBUFFER_STORAGE_FBF
                gl_FragData[3] = CCStandardShadingBase(s);
            #else
                gl_FragColor = CCStandardShadingBase(s);
            #endif
        }
    )";

    gfx::ShaderInfo shaderInfo;

    shaderInfo.name = "Standard Deferred";
    shaderInfo.stages.push_back({gfx::ShaderStageFlagBit::VERTEX, getAppropriateShaderSource(vert)});
    shaderInfo.stages.push_back({gfx::ShaderStageFlagBit::FRAGMENT, getAppropriateShaderSource(frag)});

    shaderInfo.attributes.push_back({"a_position", gfx::Format::RG32F, false, 0, false, 0});
    shaderInfo.attributes.push_back({"a_texCoord", gfx::Format::RG32F, false, 0, false, 1});

    shaderInfo.blocks.push_back(cameraUBOInfo);

    shaderInfo.subpassInputs.push_back({0, 1, "gbuffer0", 1});
    shaderInfo.subpassInputs.push_back({0, 2, "gbuffer1", 1});
    shaderInfo.subpassInputs.push_back({0, 3, "gbuffer2", 1});
    shaderInfo.subpassInputs.push_back({0, 4, "gbuffer3", 1});

    _deferredShader.reset(device->createShader(shaderInfo));

    vector<float> vb{
        -1.F, -1.F, 0.F, 0.F,
        -1.F, 1.F, 0.F, 1.F,
        1.F, -1.F, 1.F, 0.F,
        1.F, 1.F, 1.F, 1.F};
    _deferredVB.reset(device->createBuffer({
        gfx::BufferUsage::VERTEX,
        gfx::MemoryUsage::DEVICE,
        static_cast<uint>(vb.size() * sizeof(float)),
        4 * sizeof(float),
    }));
    _deferredVB->update(vb.data(), vb.size() * sizeof(float));

    gfx::InputAssemblerInfo iaInfo;
    iaInfo.attributes = shaderInfo.attributes;
    iaInfo.vertexBuffers.emplace_back(_deferredVB.get());
    _deferredInputAssembler.reset(device->createInputAssembler(iaInfo));

    gfx::DescriptorSetLayoutInfo dslInfo;
    dslInfo.bindings.push_back({0, gfx::DescriptorType::UNIFORM_BUFFER, 1, gfx::ShaderStageFlagBit::FRAGMENT});
    dslInfo.bindings.push_back({1, gfx::DescriptorType::INPUT_ATTACHMENT, 1, gfx::ShaderStageFlagBit::FRAGMENT});
    dslInfo.bindings.push_back({2, gfx::DescriptorType::INPUT_ATTACHMENT, 1, gfx::ShaderStageFlagBit::FRAGMENT});
    dslInfo.bindings.push_back({3, gfx::DescriptorType::INPUT_ATTACHMENT, 1, gfx::ShaderStageFlagBit::FRAGMENT});
    dslInfo.bindings.push_back({4, gfx::DescriptorType::INPUT_ATTACHMENT, 1, gfx::ShaderStageFlagBit::FRAGMENT});
    _deferredDescriptorSetLayout.reset(device->createDescriptorSetLayout(dslInfo));

    _deferredPipelineLayout.reset(device->createPipelineLayout({{_deferredDescriptorSetLayout.get()}}));

    _deferredDescriptorSet.reset(device->createDescriptorSet({_deferredDescriptorSetLayout.get()}));

    _deferredDescriptorSet->bindBuffer(static_cast<uint>(Binding::CAMERA), _bufferViews[2].get());
    _deferredDescriptorSet->bindTexture(1, _deferredGBuffers[0].get());
    _deferredDescriptorSet->bindTexture(2, _deferredGBuffers[1].get());
    _deferredDescriptorSet->bindTexture(3, _deferredGBuffers[2].get());
    _deferredDescriptorSet->bindTexture(4, _deferredGBuffers[3].get());
    _deferredDescriptorSet->bindSampler(1, _sampler.get());
    _deferredDescriptorSet->bindSampler(2, _sampler.get());
    _deferredDescriptorSet->bindSampler(3, _sampler.get());
    _deferredDescriptorSet->bindSampler(4, _sampler.get());
    _deferredDescriptorSet->update();

    gfx::PipelineStateInfo pipelineStateInfo;
    pipelineStateInfo.primitive                    = gfx::PrimitiveMode::TRIANGLE_STRIP;
    pipelineStateInfo.shader                       = _deferredShader.get();
    pipelineStateInfo.inputState                   = {_deferredInputAssembler->getAttributes()};
    pipelineStateInfo.pipelineLayout               = _deferredPipelineLayout.get();
    pipelineStateInfo.renderPass                   = _deferredGBufferRenderPass.get();
    pipelineStateInfo.subpass                      = 1;
    pipelineStateInfo.depthStencilState.depthTest  = false;
    pipelineStateInfo.depthStencilState.depthWrite = false;
    pipelineStateInfo.rasterizerState.cullMode     = gfx::CullMode::NONE;
    _deferredPipelineState.reset(device->createPipelineState(pipelineStateInfo));
}

void SubpassTest::createBuffers() {
    auto obj = loadOBJ("bunny.obj");

    // vertex buffer
    const auto &positions = obj.GetAttrib().vertices;
    _vertexPositionBuffer.reset(device->createBuffer({
        gfx::BufferUsage::VERTEX,
        gfx::MemoryUsage::DEVICE,
        static_cast<uint>(positions.size() * sizeof(float)),
        3 * sizeof(float),
    }));
    _vertexPositionBuffer->update(positions.data(), positions.size() * sizeof(float));

    const auto &normals = obj.GetAttrib().normals;
    _vertexNormalBuffer.reset(device->createBuffer({
        gfx::BufferUsage::VERTEX,
        gfx::MemoryUsage::DEVICE,
        static_cast<uint>(normals.size() * sizeof(float)),
        3 * sizeof(float),
    }));
    _vertexNormalBuffer->update(normals.data(), normals.size() * sizeof(float));

    // index buffer
    const auto &     indicesInfo = obj.GetShapes()[0].mesh.indices;
    vector<uint16_t> indices;
    indices.reserve(indicesInfo.size());
    std::transform(indicesInfo.begin(), indicesInfo.end(), std::back_inserter(indices),
                   [](auto &&info) { return static_cast<uint16_t>(info.vertex_index); });

    _indexBuffer.reset(device->createBuffer({
        gfx::BufferUsage::INDEX,
        gfx::MemoryUsage::DEVICE,
        static_cast<uint>(indices.size() * sizeof(uint16_t)),
        sizeof(uint16_t),
    }));
    _indexBuffer->update(indices.data(), indices.size() * sizeof(uint16_t));

    vector<uint>          sizes{3 * sizeof(Mat4), sizeof(Vec4), 5 * sizeof(Vec4)};
    gfx::Buffer *         rootUBO = nullptr;
    vector<gfx::Buffer *> bufferViews;
    createUberBuffer(sizes, &rootUBO, &bufferViews, &_bufferViewOffsets);
    _rootUBO.reset(rootUBO);
    for (auto *bufferView : bufferViews) {
        _bufferViews.emplace_back(bufferView);
    }
    _rootBuffer.resize(_bufferViewOffsets.back() / sizeof(float));

    constexpr float cameraDistance = 5.F;

    Vec4 cameraPos{cameraDistance, cameraDistance * 0.5F, cameraDistance * 0.3F, 0.F};
    Vec4 color{252 / 255.F, 23 / 255.F, 3 / 255.F, 1.0F};
    Vec4 lightDir{0.F, -2.F, -1.F, 0.F};
    Vec4 lightColor{1.F, 1.F, 1.F, 1.7F};
    Vec4 skyColor{.2F, .5F, .8F, .5F};
    Vec4 groundColor{.2F, .2F, .2F, 1.F};
    Mat4 view;
    Mat4::createLookAt(Vec3(cameraPos.x, cameraPos.y, cameraPos.z),
                       Vec3(0.0F, 1.F, 0.0F), Vec3(0.0F, 1.0F, 0.F), &view);
    lightDir.normalize();

    std::copy(view.m, view.m + 16, &_rootBuffer[16]);
    std::copy(&color.x, &color.x + 4, &_rootBuffer[_bufferViewOffsets[1] / sizeof(float)]);
    std::copy(&cameraPos.x, &cameraPos.x + 4, &_rootBuffer[_bufferViewOffsets[2] / sizeof(float)]);
    std::copy(&lightDir.x, &lightDir.x + 4, &_rootBuffer[_bufferViewOffsets[2] / sizeof(float) + 4]);
    std::copy(&lightColor.x, &lightColor.x + 4, &_rootBuffer[_bufferViewOffsets[2] / sizeof(float) + 8]);
    std::copy(&skyColor.x, &skyColor.x + 4, &_rootBuffer[_bufferViewOffsets[2] / sizeof(float) + 12]);
    std::copy(&groundColor.x, &groundColor.x + 4, &_rootBuffer[_bufferViewOffsets[2] / sizeof(float) + 16]);
}

void SubpassTest::createInputAssembler() {
    gfx::InputAssemblerInfo inputAssemblerInfo;
    inputAssemblerInfo.attributes.push_back({"a_position", gfx::Format::RGB32F, false, 0, false});
    inputAssemblerInfo.attributes.push_back({"a_normal", gfx::Format::RGB32F, false, 1, false});
    inputAssemblerInfo.vertexBuffers.emplace_back(_vertexPositionBuffer.get());
    inputAssemblerInfo.vertexBuffers.emplace_back(_vertexNormalBuffer.get());
    inputAssemblerInfo.indexBuffer = _indexBuffer.get();
    _inputAssembler.reset(device->createInputAssembler(inputAssemblerInfo));
}

void SubpassTest::createPipelineState() {
    gfx::DescriptorSetLayoutInfo dslInfo;
    dslInfo.bindings.push_back({0, gfx::DescriptorType::UNIFORM_BUFFER, 1, gfx::ShaderStageFlagBit::FRAGMENT});
    dslInfo.bindings.push_back({1, gfx::DescriptorType::UNIFORM_BUFFER, 1, gfx::ShaderStageFlagBit::FRAGMENT});
    dslInfo.bindings.push_back({2, gfx::DescriptorType::UNIFORM_BUFFER, 1, gfx::ShaderStageFlagBit::VERTEX});
    _descriptorSetLayout.reset(device->createDescriptorSetLayout(dslInfo));

    _pipelineLayout.reset(device->createPipelineLayout({{_descriptorSetLayout.get()}}));

    _descriptorSet.reset(device->createDescriptorSet({_descriptorSetLayout.get()}));

    _descriptorSet->bindBuffer(static_cast<uint>(Binding::MVP), _bufferViews[0].get());
    _descriptorSet->bindBuffer(static_cast<uint>(Binding::COLOR), _bufferViews[1].get());
    _descriptorSet->bindBuffer(static_cast<uint>(Binding::CAMERA), _bufferViews[2].get());
    _descriptorSet->update();

    gfx::PipelineStateInfo pipelineStateInfo;
    pipelineStateInfo.primitive      = gfx::PrimitiveMode::TRIANGLE_LIST;
    pipelineStateInfo.shader         = _shaderForward.get();
    pipelineStateInfo.inputState     = {_inputAssembler->getAttributes()};
    pipelineStateInfo.renderPass     = fbo->getRenderPass();
    pipelineStateInfo.pipelineLayout = _pipelineLayout.get();
    _pipelineState.reset(device->createPipelineState(pipelineStateInfo));

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

    _textureBarriers.push_back(TestBaseI::getTextureBarrier({
        {},
        {
            gfx::AccessType::TRANSFER_WRITE,
        },
    }));

    _textureBarriers.push_back(TestBaseI::getTextureBarrier({
        {
            gfx::AccessType::TRANSFER_WRITE,
        },
        {
            gfx::AccessType::PRESENT,
        },
    }));

    _clearColors.assign(4, {0, 0, 0, 1});
}

void SubpassTest::onSpacePressed() {
    _useDeferred = !_useDeferred;
    CC_LOG_INFO("Shading mode switched to: %s", _useDeferred ? "Deferred" : "Forward");
}

void SubpassTest::onTick() {
    uint          globalBarrierIdx = _frameCount ? 1 : 0;
    gfx::Texture *backBuffer       = nullptr;
    printTime();

    gfx::Extent orientedSize = TestBaseI::getOrientedSurfaceSize();
    Mat4::createRotationY(_time, &_worldMatrix);
    TestBaseI::createPerspective(60.0F,
                                 static_cast<float>(orientedSize.width) / static_cast<float>(orientedSize.height),
                                 0.01F, 1000.0F, &_projectionMatrix);

    std::copy(_worldMatrix.m, _worldMatrix.m + 16, &_rootBuffer[0]);
    std::copy(_projectionMatrix.m, _projectionMatrix.m + 16, &_rootBuffer[32]);

    device->acquire();

    _rootUBO->update(_rootBuffer.data(), _rootBuffer.size() * sizeof(float));
    gfx::Rect renderArea = {0, 0, device->getWidth(), device->getHeight()};

    auto *commandBuffer = commandBuffers[0];
    commandBuffer->begin();

    if (TestBaseI::MANUAL_BARRIER) {
        commandBuffer->pipelineBarrier(_globalBarriers[globalBarrierIdx]);
    }

    if (_useDeferred) {
        for (uint i = 0U; i < REPEAT; ++i) {
            commandBuffer->beginRenderPass(_deferredGBufferRenderPass.get(), _deferredGBufferFramebuffer.get(), renderArea, _clearColors.data(), 1.0F, 0);

            commandBuffer->bindInputAssembler(_inputAssembler.get());
            commandBuffer->bindPipelineState(_deferredGBufferPipelineState.get());
            commandBuffer->bindDescriptorSet(0, _descriptorSet.get());
            commandBuffer->draw(_inputAssembler.get());

            commandBuffer->nextSubpass();

            commandBuffer->bindInputAssembler(_deferredInputAssembler.get());
            commandBuffer->bindPipelineState(_deferredPipelineState.get());
            commandBuffer->bindDescriptorSet(0, _deferredDescriptorSet.get());
            commandBuffer->draw(_deferredInputAssembler.get());

            commandBuffer->endRenderPass();

            commandBuffer->pipelineBarrier(nullptr, &_textureBarriers[0], &backBuffer, 1);

            gfx::TextureBlit region;
            region.srcExtent.width  = device->getWidth();
            region.srcExtent.height = device->getHeight();
            region.dstExtent.width  = device->getWidth();
            region.dstExtent.height = device->getHeight();
            commandBuffer->blitTexture(_deferredGBuffers[3].get(), nullptr, &region, 1, gfx::Filter::POINT);

            commandBuffer->pipelineBarrier(nullptr, &_textureBarriers[1], &backBuffer, 1);
        }
    } else {
        commandBuffer->beginRenderPass(fbo->getRenderPass(), fbo, renderArea, _clearColors.data(), 1.0F, 0);

        commandBuffer->bindInputAssembler(_inputAssembler.get());
        commandBuffer->bindPipelineState(_pipelineState.get());
        commandBuffer->bindDescriptorSet(0, _descriptorSet.get());
        commandBuffer->draw(_inputAssembler.get());

        commandBuffer->endRenderPass();
    }

    commandBuffer->end();

    device->flushCommands(commandBuffers);
    device->getQueue()->submit(commandBuffers);
    device->present();
}

} // namespace cc
