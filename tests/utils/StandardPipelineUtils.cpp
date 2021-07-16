#pragma once

#include "StandardPipelineUtils.h"

namespace cc {

namespace {
gfx::AttributeList attributes{
    {"a_position", gfx::Format::RGB32F, false, 0, false, 0},
    {"a_normal", gfx::Format::RGB32F, false, 1, false, 1},
};
gfx::AttributeList lightingAttributes{
    {"a_position", gfx::Format::RG32F, false, 0, false, 0},
    {"a_texCoord", gfx::Format::RG32F, false, 0, false, 1},
};

gfx::DescriptorSetLayoutInfo descriptorSetLayoutInfo{
    {
        {0, gfx::DescriptorType::UNIFORM_BUFFER, 1, gfx::ShaderStageFlagBit::FRAGMENT},
        {1, gfx::DescriptorType::UNIFORM_BUFFER, 1, gfx::ShaderStageFlagBit::FRAGMENT},
        {2, gfx::DescriptorType::UNIFORM_BUFFER, 1, gfx::ShaderStageFlagBit::VERTEX},
    },
};

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
    static_cast<uint>(standard::CAMERA),
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

ShaderSources<String> fragBase{
    R"(
        precision highp float;

        layout(set = 0, binding = 1) uniform Color {
            vec4 u_color;
        };

        layout(location = 0) in vec3 v_position;
        layout(location = 1) in vec3 v_normal;
    )",
    R"(
        precision mediump float;

        layout(std140) uniform Color {
            vec4 u_color;
        };

        in vec3 v_position;
        in vec3 v_normal;
    )",
    R"(
        precision mediump float;

        uniform vec4 u_color;

        varying vec3 v_position;
        varying vec3 v_normal;
    )",
};

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
} // namespace

String declExtensions(bool useSubpass = true) {
    String str = R"(
        #define GBUFFER_STORAGE_TEXTURE 0
        #define GBUFFER_STORAGE_PLS 1
        #define GBUFFER_STORAGE_FBF 2
    )";
    if (!useSubpass) {
        return str + R"(
            #define GBUFFER_STORAGE GBUFFER_STORAGE_TEXTURE
        )";
    }
    return str + R"(
        #if defined(GL_EXT_shader_framebuffer_fetch)
        #   extension GL_EXT_shader_framebuffer_fetch: enable
        #   define GBUFFER_STORAGE GBUFFER_STORAGE_FBF
        #elif __VERSION__ > 100 && defined(GL_EXT_shader_pixel_local_storage)
        #   extension GL_EXT_shader_pixel_local_storage: enable
        #   define GBUFFER_STORAGE GBUFFER_STORAGE_PLS
        #else
        #   define GBUFFER_STORAGE GBUFFER_STORAGE_TEXTURE
        #endif
    )";
}

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

gfx::ShaderInfo getForwardShaderInfo() {
    String vertMain = R"(
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

    gfx::ShaderInfo shaderInfo;
    shaderInfo.name       = "Standard Forward";
    shaderInfo.attributes = attributes;
    shaderInfo.stages     = {
        {gfx::ShaderStageFlagBit::VERTEX, TestBaseI::getAppropriateShaderSource(vert)},
        {gfx::ShaderStageFlagBit::FRAGMENT, TestBaseI::getAppropriateShaderSource(forwardFrag)},
    };
    shaderInfo.blocks = {
        cameraUBOInfo,
        {
            0,
            static_cast<uint>(standard::MVP),
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
            static_cast<uint>(standard::COLOR),
            "Color",
            {
                {"u_color", gfx::Type::FLOAT4, 1},
            },
            1,
        },
    };
    return shaderInfo;
}

void createStandardShader(gfx::Device *device, StandardForwardPipeline *out) {
    gfx::ShaderInfo shaderInfo = getForwardShaderInfo();
    out->shader.reset(device->createShader(shaderInfo));
}

void createStandardShader(gfx::Device *device, StandardDeferredPipeline *out) {
    gfx::ShaderInfo shaderInfo = getForwardShaderInfo();

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
    gbufferFrag.glsl3 = declExtensions() + gbufferFrag.glsl3;
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

    shaderInfo.name             = "Standard GBuffer";
    shaderInfo.stages[1].source = TestBaseI::getAppropriateShaderSource(gbufferFrag);
    out->gbufferShader.reset(device->createShader(shaderInfo));

    ShaderSources<String> lightingVert = {
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
    ShaderSources<String> lightingFrag = {
        R"(
            precision highp float;
            layout(location = 0) in vec2 v_texCoord;

            layout(input_attachment_index = 0, set = 0, binding = 1) uniform subpassInput gbuffer0;
            layout(input_attachment_index = 1, set = 0, binding = 2) uniform subpassInput gbuffer1;
            layout(input_attachment_index = 2, set = 0, binding = 3) uniform subpassInput gbuffer2;
            layout(input_attachment_index = 3, set = 0, binding = 4) uniform subpassInput gbuffer3;
        )",
        declExtensions() + R"(
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
        declExtensions() + R"(
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
    lightingFrag += cameraUBO + standardStruct + standardShading;
    lightingFrag.glsl4 += R"(
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
    lightingFrag.glsl3 += R"(
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
    lightingFrag.glsl1 += R"(
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

    gfx::ShaderInfo lightingShaderInfo;
    lightingShaderInfo.name       = "Standard Deferred";
    lightingShaderInfo.attributes = lightingAttributes;
    lightingShaderInfo.stages.push_back({gfx::ShaderStageFlagBit::VERTEX, TestBaseI::getAppropriateShaderSource(lightingVert)});
    lightingShaderInfo.stages.push_back({gfx::ShaderStageFlagBit::FRAGMENT, TestBaseI::getAppropriateShaderSource(lightingFrag)});
    lightingShaderInfo.blocks.push_back(cameraUBOInfo);
    lightingShaderInfo.subpassInputs.push_back({0, 1, "gbuffer0", 1});
    lightingShaderInfo.subpassInputs.push_back({0, 2, "gbuffer1", 1});
    lightingShaderInfo.subpassInputs.push_back({0, 3, "gbuffer2", 1});
    lightingShaderInfo.subpassInputs.push_back({0, 4, "gbuffer3", 1});
    out->lightingShader.reset(device->createShader(lightingShaderInfo));
}

std::unique_ptr<gfx::DescriptorSetLayout> StandardUniformBuffers::descriptorSetLayout{nullptr};
std::unique_ptr<gfx::PipelineLayout>      StandardUniformBuffers::pipelineLayout{nullptr};
uint                                      StandardUniformBuffers::refCount{0U};

StandardUniformBuffers::StandardUniformBuffers() {
    refCount++;
}

StandardUniformBuffers::~StandardUniformBuffers() {
    if (!--refCount) {
        descriptorSetLayout.reset(nullptr);
        pipelineLayout.reset(nullptr);
    }
}

void createStandardUniformBuffers(gfx::Device *device, StandardUniformBuffers *out) {
    static vector<uint>          sizes{5 * sizeof(Vec4), sizeof(Vec4), 3 * sizeof(Mat4)};
    static vector<gfx::Buffer *> views;

    gfx::Buffer *rootUBO = nullptr;
    views.clear();
    TestBaseI::createUberBuffer(sizes, &rootUBO, &views, &out->_bufferViewOffsets);

    out->_rootUBO.reset(rootUBO);
    out->_rootBuffer.resize(out->_bufferViewOffsets.back() / sizeof(float));
    for (auto *bufferView : views) out->bufferViews.emplace_back(bufferView);

    if (!StandardUniformBuffers::descriptorSetLayout) {
        StandardUniformBuffers::descriptorSetLayout.reset(device->createDescriptorSetLayout(descriptorSetLayoutInfo));
        StandardUniformBuffers::pipelineLayout.reset(device->createPipelineLayout({{StandardUniformBuffers::descriptorSetLayout.get()}}));
    }

    out->descriptorSet.reset(device->createDescriptorSet({StandardUniformBuffers::descriptorSetLayout.get()}));
    out->descriptorSet->bindBuffer(standard::CAMERA, out->bufferViews[standard::CAMERA].get());
    out->descriptorSet->bindBuffer(standard::COLOR, out->bufferViews[standard::COLOR].get());
    out->descriptorSet->bindBuffer(standard::MVP, out->bufferViews[standard::MVP].get());
    out->descriptorSet->update();
}

void createStandardPipelineResources(gfx::Device *device, StandardForwardPipeline *out, const StandardUniformBuffers & /*ubos*/) {
    createStandardShader(device, out);

    gfx::Swapchain *swapchain = TestBaseI::swapchain;

    gfx::RenderPassInfo forwardRenderPassInfo;
    forwardRenderPassInfo.colorAttachments.emplace_back().format = swapchain->getColorTexture()->getFormat();
    forwardRenderPassInfo.depthStencilAttachment.format          = swapchain->getDepthStencilTexture()->getFormat();
    out->renderPass.reset(device->createRenderPass(forwardRenderPassInfo));

    gfx::FramebufferInfo forwardFramebufferInfo;
    forwardFramebufferInfo.colorTextures.push_back(swapchain->getColorTexture());
    forwardFramebufferInfo.depthStencilTexture = swapchain->getDepthStencilTexture();
    forwardFramebufferInfo.renderPass = out->renderPass.get();
    out->framebuffer.reset(device->createFramebuffer(forwardFramebufferInfo));

    gfx::PipelineStateInfo pipelineStateInfo;
    pipelineStateInfo.primitive      = gfx::PrimitiveMode::TRIANGLE_LIST;
    pipelineStateInfo.shader         = out->shader.get();
    pipelineStateInfo.inputState     = {attributes};
    pipelineStateInfo.renderPass     = out->renderPass.get();
    pipelineStateInfo.pipelineLayout = StandardUniformBuffers::pipelineLayout.get();
    out->pipelineState.reset(device->createPipelineState(pipelineStateInfo));
}

void createStandardPipelineResources(gfx::Device *device, StandardDeferredPipeline *out, const StandardUniformBuffers &ubos) {
    createStandardShader(device, out);

    gfx::Swapchain *swapchain = TestBaseI::swapchain;

    gfx::RenderPassInfo deferredRenderPassInfo;
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
        out->gbufferTextures.emplace_back(device->createTexture({
            gfx::TextureType::TEX2D,
            usage,
            format,
            swapchain->getWidth(),
            swapchain->getHeight(),
            flags,
        }));
        deferredRenderPassInfo.colorAttachments.emplace_back();
        deferredRenderPassInfo.colorAttachments.back().storeOp         = storeOp;
        deferredRenderPassInfo.colorAttachments.back().format          = format;
        deferredRenderPassInfo.colorAttachments.back().endAccesses     = {accessType};
        deferredRenderPassInfo.colorAttachments.back().isGeneralLayout = isGeneralLayout;
    }

    deferredRenderPassInfo.depthStencilAttachment.format         = swapchain->getDepthStencilTexture()->getFormat();
    deferredRenderPassInfo.depthStencilAttachment.depthStoreOp   = gfx::StoreOp::DISCARD;
    deferredRenderPassInfo.depthStencilAttachment.stencilStoreOp = gfx::StoreOp::DISCARD;

    out->gbufferDepthStencilTexture.reset(device->createTexture({
        gfx::TextureType::TEX2D,
        gfx::TextureUsageBit::DEPTH_STENCIL_ATTACHMENT,
        swapchain->getDepthStencilTexture()->getFormat(),
        swapchain->getWidth(),
        swapchain->getHeight(),
    }));

    deferredRenderPassInfo.subpasses.resize(2);
    deferredRenderPassInfo.subpasses[0].colors = {0, 1, 2, 3};
    deferredRenderPassInfo.subpasses[1].colors = {3};
    deferredRenderPassInfo.subpasses[1].inputs = {0, 1, 2, 3};

    deferredRenderPassInfo.dependencies.push_back({
        0,
        1,
        {gfx::AccessType::COLOR_ATTACHMENT_WRITE},
        {gfx::AccessType::FRAGMENT_SHADER_READ_COLOR_INPUT_ATTACHMENT},
    });

    deferredRenderPassInfo.dependencies.push_back({
        1,
        gfx::SUBPASS_EXTERNAL,
        {gfx::AccessType::COLOR_ATTACHMENT_WRITE},
        {gfx::AccessType::TRANSFER_READ},
    });

    out->gbufferRenderPass.reset(device->createRenderPass(deferredRenderPassInfo));
    out->gbufferFramebuffer.reset(device->createFramebuffer({
        out->gbufferRenderPass.get(),
        {
            out->gbufferTextures[0].get(),
            out->gbufferTextures[1].get(),
            out->gbufferTextures[2].get(),
            out->gbufferTextures[3].get(),
        },
        out->gbufferDepthStencilTexture.get(),
    }));

    gfx::PipelineStateInfo gbufferPipelineStateInfo;
    gbufferPipelineStateInfo.primitive             = gfx::PrimitiveMode::TRIANGLE_LIST;
    gbufferPipelineStateInfo.shader                = out->gbufferShader.get();
    gbufferPipelineStateInfo.renderPass            = out->gbufferRenderPass.get();
    gbufferPipelineStateInfo.pipelineLayout        = StandardUniformBuffers::pipelineLayout.get();
    gbufferPipelineStateInfo.inputState.attributes = attributes;
    gbufferPipelineStateInfo.blendState.targets.resize(4);
    out->gbufferPipelineState.reset(device->createPipelineState(gbufferPipelineStateInfo));

    ///////////////////////////////////////////////////////////////////////////

    gfx::DescriptorSetLayoutInfo lightingDescriptorSetLayoutInfo;
    lightingDescriptorSetLayoutInfo.bindings.push_back({0, gfx::DescriptorType::UNIFORM_BUFFER, 1, gfx::ShaderStageFlagBit::FRAGMENT});
    lightingDescriptorSetLayoutInfo.bindings.push_back({1, gfx::DescriptorType::INPUT_ATTACHMENT, 1, gfx::ShaderStageFlagBit::FRAGMENT});
    lightingDescriptorSetLayoutInfo.bindings.push_back({2, gfx::DescriptorType::INPUT_ATTACHMENT, 1, gfx::ShaderStageFlagBit::FRAGMENT});
    lightingDescriptorSetLayoutInfo.bindings.push_back({3, gfx::DescriptorType::INPUT_ATTACHMENT, 1, gfx::ShaderStageFlagBit::FRAGMENT});
    lightingDescriptorSetLayoutInfo.bindings.push_back({4, gfx::DescriptorType::INPUT_ATTACHMENT, 1, gfx::ShaderStageFlagBit::FRAGMENT});
    out->lightingDescriptorSetLayout.reset(device->createDescriptorSetLayout(lightingDescriptorSetLayoutInfo));

    out->lightingPipelineLayout.reset(device->createPipelineLayout({{out->lightingDescriptorSetLayout.get()}}));

    gfx::PipelineStateInfo lightingPipelineStateInfo;
    lightingPipelineStateInfo.primitive                    = gfx::PrimitiveMode::TRIANGLE_STRIP;
    lightingPipelineStateInfo.shader                       = out->lightingShader.get();
    lightingPipelineStateInfo.inputState                   = {lightingAttributes};
    lightingPipelineStateInfo.pipelineLayout               = out->lightingPipelineLayout.get();
    lightingPipelineStateInfo.renderPass                   = out->gbufferRenderPass.get();
    lightingPipelineStateInfo.subpass                      = 1;
    lightingPipelineStateInfo.depthStencilState.depthTest  = false;
    lightingPipelineStateInfo.depthStencilState.depthWrite = false;
    lightingPipelineStateInfo.rasterizerState.cullMode     = gfx::CullMode::NONE;
    out->lightingPipelineState.reset(device->createPipelineState(lightingPipelineStateInfo));

    vector<float> lightingVertexBufferData{
        -1.F, -1.F, 0.F, 0.F,
        -1.F, 1.F, 0.F, 1.F,
        1.F, -1.F, 1.F, 0.F,
        1.F, 1.F, 1.F, 1.F};
    out->lightingVertexBuffer.reset(device->createBuffer({
        gfx::BufferUsage::VERTEX,
        gfx::MemoryUsage::DEVICE,
        static_cast<uint>(lightingVertexBufferData.size() * sizeof(float)),
        4 * sizeof(float),
    }));
    out->lightingVertexBuffer->update(lightingVertexBufferData.data(), lightingVertexBufferData.size() * sizeof(float));

    gfx::InputAssemblerInfo lightingInputAssemblerInfo;
    lightingInputAssemblerInfo.attributes = lightingAttributes;
    lightingInputAssemblerInfo.vertexBuffers.emplace_back(out->lightingVertexBuffer.get());
    out->lightingInputAssembler.reset(device->createInputAssembler(lightingInputAssemblerInfo));

    gfx::SamplerInfo samplerInfo;
    out->sampler.reset(device->createSampler(samplerInfo));

    out->lightingDescriptorSet.reset(device->createDescriptorSet({out->lightingDescriptorSetLayout.get()}));

    out->lightingDescriptorSet->bindBuffer(standard::CAMERA, ubos.bufferViews[standard::CAMERA].get());
    out->lightingDescriptorSet->bindTexture(1, out->gbufferTextures[0].get());
    out->lightingDescriptorSet->bindTexture(2, out->gbufferTextures[1].get());
    out->lightingDescriptorSet->bindTexture(3, out->gbufferTextures[2].get());
    out->lightingDescriptorSet->bindTexture(4, out->gbufferTextures[3].get());
    out->lightingDescriptorSet->bindSampler(1, out->sampler.get());
    out->lightingDescriptorSet->bindSampler(2, out->sampler.get());
    out->lightingDescriptorSet->bindSampler(3, out->sampler.get());
    out->lightingDescriptorSet->bindSampler(4, out->sampler.get());
    out->lightingDescriptorSet->update();
}

} // namespace cc
