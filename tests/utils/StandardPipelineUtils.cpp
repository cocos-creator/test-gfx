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
        {0, gfx::DescriptorType::DYNAMIC_UNIFORM_BUFFER, 1, gfx::ShaderStageFlagBit::FRAGMENT},
        {1, gfx::DescriptorType::DYNAMIC_UNIFORM_BUFFER, 1, gfx::ShaderStageFlagBit::FRAGMENT},
        {2, gfx::DescriptorType::DYNAMIC_UNIFORM_BUFFER, 1, gfx::ShaderStageFlagBit::VERTEX},
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
            vec4 cc_cameraPos; // xyz: camera position, w: lighting UV tiling x
            vec4 cc_mainLitDir; // xyz: main direcitonal light direction, w: lighting UV tiling y
            vec4 cc_mainLitColor; // xyz: main direcitonal light color, w: intensity
            vec4 cc_ambientSky; //xyz: sky illumination color, w: intensity
            vec4 cc_ambientGround; // xyz: ground albedo color
        };
    )",
    R"(
        layout(std140) uniform CCCamera {
            vec4 cc_cameraPos; // xyz: camera position, w: lighting UV tiling x
            vec4 cc_mainLitDir; // xyz: main direcitonal light direction, w: lighting UV tiling y
            vec4 cc_mainLitColor; // xyz: main direcitonal light color, w: intensity
            vec4 cc_ambientSky; //xyz: sky illumination color, w: intensity
            vec4 cc_ambientGround; // xyz: ground albedo color
        };
    )",
    R"(
        uniform vec4 cc_cameraPos; // xyz: camera position, w: lighting UV tiling x
        uniform vec4 cc_mainLitDir; // xyz: main direcitonal light direction, w: lighting UV tiling y
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

String extensions = R"(
    #if CC_DEVICE_CAN_BENEFIT_FROM_INPUT_ATTACHMENT
        #extension GL_EXT_shader_framebuffer_fetch: require
    #endif
)";
} // namespace

String getInputAttachmentMacro(gfx::Device *device) {
    return StringUtil::format(
        R"(
            #define CC_DEVICE_CAN_BENEFIT_FROM_INPUT_ATTACHMENT %d
        )",
        StandardDeferredPipeline::USE_SUBPASS && device->hasFeature(gfx::Feature::INPUT_ATTACHMENT_BENEFIT));
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
    gbufferFrag.glsl3 = getInputAttachmentMacro(device) + extensions + gbufferFrag.glsl3;
    gbufferFrag.glsl3 += R"(
        layout(location = 0) out vec4 gbuffer0;
        layout(location = 1) out vec4 gbuffer1;
        layout(location = 2) out vec4 gbuffer2;
        layout(location = 3) out vec4 gbuffer3;

        void main () {
            StandardSurface s; surf(s);

            gbuffer0 = s.albedo;
            gbuffer1 = vec4(s.position, s.roughness);
            gbuffer2 = vec4(s.normal, s.metallic);
            gbuffer3 = vec4(s.emissive, s.occlusion);
        }
    )";
    gbufferFrag.glsl1 = R"(
        #extension GL_EXT_draw_buffers: require
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
        )",
        R"(
            in vec2 a_position;
            in vec2 a_texCoord;

            out vec2 v_texCoord;
        )",
        R"(
            attribute vec2 a_position;
            attribute vec2 a_texCoord;

            varying vec2 v_texCoord;
        )",
    };
    lightingVert += cameraUBO + R"(
        void main() {
            v_texCoord = a_texCoord * vec2(cc_cameraPos.w, cc_mainLitDir.w);
            gl_Position = vec4(a_position, 1, 1);
        }
    )";
    auto lightingFrag =
        getInputAttachmentMacro(device) +
        ShaderSources<String>{
            R"(
            precision highp float;
            layout(location = 0) in vec2 v_texCoord;

            #if CC_DEVICE_CAN_BENEFIT_FROM_INPUT_ATTACHMENT
                layout(input_attachment_index = 0, set = 0, binding = 1) uniform subpassInput gbuffer0;
                layout(input_attachment_index = 1, set = 0, binding = 2) uniform subpassInput gbuffer1;
                layout(input_attachment_index = 2, set = 0, binding = 3) uniform subpassInput gbuffer2;
                layout(input_attachment_index = 3, set = 0, binding = 4) uniform subpassInput gbuffer3;
            #else
                layout(set = 0, binding = 1) uniform sampler2D gbuffer0;
                layout(set = 0, binding = 2) uniform sampler2D gbuffer1;
                layout(set = 0, binding = 3) uniform sampler2D gbuffer2;
                layout(set = 0, binding = 4) uniform sampler2D gbuffer3;
            #endif
        )",
            extensions + R"(
            precision highp float;
            in vec2 v_texCoord;

            #if CC_DEVICE_CAN_BENEFIT_FROM_INPUT_ATTACHMENT
                layout(location = 0) inout vec4 gbuffer0;
                layout(location = 1) inout vec4 gbuffer1;
                layout(location = 2) inout vec4 gbuffer2;
                layout(location = 3) inout vec4 gbuffer3;
            #else
                uniform sampler2D gbuffer0;
                uniform sampler2D gbuffer1;
                uniform sampler2D gbuffer2;
                uniform sampler2D gbuffer3;
                layout(location = 0) out vec4 o_color;
            #endif
        )",
            extensions + R"(
            #if CC_DEVICE_CAN_BENEFIT_FROM_INPUT_ATTACHMENT
                #extension GL_EXT_draw_buffers: require
            #endif

            precision highp float;
            varying vec2 v_texCoord;

            #if !CC_DEVICE_CAN_BENEFIT_FROM_INPUT_ATTACHMENT
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

            #if CC_DEVICE_CAN_BENEFIT_FROM_INPUT_ATTACHMENT
                vec4 g0 = subpassLoad(gbuffer0);
                vec4 g1 = subpassLoad(gbuffer1);
                vec4 g2 = subpassLoad(gbuffer2);
                vec4 g3 = subpassLoad(gbuffer3);
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

            o_color = CCStandardShadingBase(s);
        }
    )";
    lightingFrag.glsl3 += R"(
        void main() {

            #if CC_DEVICE_CAN_BENEFIT_FROM_INPUT_ATTACHMENT
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

            #if CC_DEVICE_CAN_BENEFIT_FROM_INPUT_ATTACHMENT
                gbuffer3 = CCStandardShadingBase(s);
            #else
                o_color = CCStandardShadingBase(s);
            #endif
        }
    )";
    lightingFrag.glsl1 += R"(
        void main() {

            #if CC_DEVICE_CAN_BENEFIT_FROM_INPUT_ATTACHMENT
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

            #if CC_DEVICE_CAN_BENEFIT_FROM_INPUT_ATTACHMENT
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
    if constexpr (StandardDeferredPipeline::USE_SUBPASS) {
        lightingShaderInfo.subpassInputs.push_back({0, 1, "gbuffer0", 1});
        lightingShaderInfo.subpassInputs.push_back({0, 2, "gbuffer1", 1});
        lightingShaderInfo.subpassInputs.push_back({0, 3, "gbuffer2", 1});
        lightingShaderInfo.subpassInputs.push_back({0, 4, "gbuffer3", 1});
    } else {
        lightingShaderInfo.samplerTextures.push_back({0, 1, "gbuffer0", gfx::Type::SAMPLER2D, 1});
        lightingShaderInfo.samplerTextures.push_back({0, 2, "gbuffer1", gfx::Type::SAMPLER2D, 1});
        lightingShaderInfo.samplerTextures.push_back({0, 3, "gbuffer2", gfx::Type::SAMPLER2D, 1});
        lightingShaderInfo.samplerTextures.push_back({0, 4, "gbuffer3", gfx::Type::SAMPLER2D, 1});
    }
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

float *StandardUniformBuffers::getBuffer(uint binding, uint instance) {
    uint offset = _bufferViewOffsets[binding];
    offset += instance * _alignedBufferSizes[binding];
    return &_rootBuffer[offset / sizeof(float)];
}

void StandardUniformBuffers::update(gfx::CommandBuffer *cmdBuff) {
    if (cmdBuff) {
        cmdBuff->updateBuffer(_rootUBO.get(), _rootBuffer.data(), _rootBuffer.size() * sizeof(float));
    } else {
        _rootUBO->update(_rootBuffer.data(), _rootBuffer.size() * sizeof(float));
    }
}

void StandardUniformBuffers::bindDescriptorSet(gfx::CommandBuffer *cmdBuff, uint set, uint instance) {
    for (uint i = 0; i < _alignedBufferSizes.size(); ++i) {
        _dynamicOffsets[i] = _alignedBufferSizes[i] * instance;
    }
    cmdBuff->bindDescriptorSet(set, descriptorSet.get(), _dynamicOffsets);
}

void createStandardUniformBuffers(gfx::Device *device, StandardUniformBuffers *out, uint instances) {
    static vector<uint>          sizes{5 * sizeof(Vec4), sizeof(Vec4), 3 * sizeof(Mat4)};
    static vector<gfx::Buffer *> views;

    gfx::Buffer *rootUBO = nullptr;
    views.clear();
    TestBaseI::createUberBuffer(sizes, &rootUBO, &views, &out->_bufferViewOffsets, &out->_alignedBufferSizes, instances);

    out->_instances = instances;
    out->_rootUBO.reset(rootUBO);
    out->_rootBuffer.resize(out->_bufferViewOffsets.back() / sizeof(float));
    out->_dynamicOffsets.resize(out->_bufferViewOffsets.size());
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

    gfx::PipelineStateInfo pipelineStateInfo;
    pipelineStateInfo.primitive      = gfx::PrimitiveMode::TRIANGLE_LIST;
    pipelineStateInfo.shader         = out->shader.get();
    pipelineStateInfo.inputState     = {attributes};
    pipelineStateInfo.renderPass     = TestBaseI::renderPass;
    pipelineStateInfo.pipelineLayout = StandardUniformBuffers::pipelineLayout.get();
    out->pipelineState.reset(device->createPipelineState(pipelineStateInfo));
}

void StandardForwardPipeline::recordCommandBuffer(gfx::Device * /*device*/, gfx::CommandBuffer *commandBuffer, gfx::Framebuffer *framebuffer,
                                                  const gfx::Rect &renderArea, const gfx::Color &clearColor, const std::function<void()> &execute) const {
    commandBuffer->beginRenderPass(TestBaseI::renderPass, framebuffer, renderArea, &clearColor, 1.0F, 0);
    commandBuffer->bindPipelineState(pipelineState.get());

    execute();

    commandBuffer->endRenderPass();
}

void createStandardPipelineResources(gfx::Device *device, StandardDeferredPipeline *out, const StandardUniformBuffers &ubos) {
    createStandardShader(device, out);

    gfx::DescriptorSetLayoutInfo lightingDescriptorSetLayoutInfo;

    auto descriptorType{StandardDeferredPipeline::USE_SUBPASS ? gfx::DescriptorType::INPUT_ATTACHMENT : gfx::DescriptorType::SAMPLER_TEXTURE};
    lightingDescriptorSetLayoutInfo.bindings.push_back({0, gfx::DescriptorType::UNIFORM_BUFFER, 1, gfx::ShaderStageFlagBit::VERTEX | gfx::ShaderStageFlagBit::FRAGMENT});
    lightingDescriptorSetLayoutInfo.bindings.push_back({1, descriptorType, 1, gfx::ShaderStageFlagBit::FRAGMENT});
    lightingDescriptorSetLayoutInfo.bindings.push_back({2, descriptorType, 1, gfx::ShaderStageFlagBit::FRAGMENT});
    lightingDescriptorSetLayoutInfo.bindings.push_back({3, descriptorType, 1, gfx::ShaderStageFlagBit::FRAGMENT});
    lightingDescriptorSetLayoutInfo.bindings.push_back({4, descriptorType, 1, gfx::ShaderStageFlagBit::FRAGMENT});
    out->lightingDescriptorSetLayout.reset(device->createDescriptorSetLayout(lightingDescriptorSetLayoutInfo));

    out->lightingPipelineLayout.reset(device->createPipelineLayout({{out->lightingDescriptorSetLayout.get()}}));

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
    auto *           sampler = device->getSampler(samplerInfo);

    out->lightingDescriptorSet.reset(device->createDescriptorSet({out->lightingDescriptorSetLayout.get()}));

    out->lightingDescriptorSet->bindBuffer(standard::CAMERA, ubos.bufferViews[standard::CAMERA].get());
    out->lightingDescriptorSet->bindSampler(1, sampler);
    out->lightingDescriptorSet->bindSampler(2, sampler);
    out->lightingDescriptorSet->bindSampler(3, sampler);
    out->lightingDescriptorSet->bindSampler(4, sampler);
}

gfx::PipelineState *StandardDeferredPipeline::getPipelineState(gfx::Device *device, const gfx::PipelineStateInfo &info) {
    uint32_t hash = info.shader->getTypedID(); // over-simplification, should never do this
    if (!_pipelineStatePool.count(hash)) {
        _pipelineStatePool[hash].reset(device->createPipelineState(info));
    }
    return _pipelineStatePool[hash].get();
}

gfx::DescriptorSet *StandardDeferredPipeline::getDescriptorSet(gfx::Device *device, uint32_t hash) {
    static gfx::SamplerInfo samplerInfo;
    static gfx::Sampler *   sampler{device->getSampler(samplerInfo)};

    if (!_descriptorSetPool.count(hash)) {
        auto *descriptorSet = device->createDescriptorSet({lightingDescriptorSetLayout.get()});
        descriptorSet->bindBuffer(standard::CAMERA, lightingDescriptorSet->getBuffer(standard::CAMERA));
        descriptorSet->bindSampler(1, sampler);
        descriptorSet->bindSampler(2, sampler);
        descriptorSet->bindSampler(3, sampler);
        descriptorSet->bindSampler(4, sampler);
        _descriptorSetPool[hash].reset(descriptorSet);
    }

    return _descriptorSetPool[hash].get();
}

void StandardDeferredPipeline::recordCommandBuffer(gfx::Device *device, gfx::CommandBuffer *commandBuffer, gfx::Framebuffer *framebuffer,
                                                   const gfx::Rect &renderArea, const gfx::Color &clearColor, const std::function<void()> &execute) {
    // Logic passes
    static const framegraph::StringHandle GBUFFER_PASS_NAME = framegraph::FrameGraph::stringToHandle("GBufferPass");
    static const framegraph::StringHandle SHADING_PASS_NAME = framegraph::FrameGraph::stringToHandle("ShadingPass");
    // Transient Resources
    static const framegraph::StringHandle GBUFFER_NAMES[]{
        framegraph::FrameGraph::stringToHandle("GBuffer0"),
        framegraph::FrameGraph::stringToHandle("GBuffer1"),
        framegraph::FrameGraph::stringToHandle("GBuffer2"),
        framegraph::FrameGraph::stringToHandle("GBuffer3"),
    };
    static const framegraph::StringHandle DEPTH_STENCIL_NAME   = framegraph::FrameGraph::stringToHandle("DepthStencil");
    static const framegraph::StringHandle LIGHTING_OUTPUT_NAME = framegraph::FrameGraph::stringToHandle("LightingOutput");

    struct GBufferData {
        framegraph::TextureHandle gbuffers[4];
        framegraph::TextureHandle depthStencil;
    };

    auto *output = framebuffer->getColorTextures()[0];

    auto gbufferPassSetup = [&](framegraph::PassNodeBuilder &builder, GBufferData &data) {
        auto usages     = gfx::TextureUsageBit::COLOR_ATTACHMENT | gfx::TextureUsageBit::SAMPLED;
        auto accessType = gfx::AccessType::FRAGMENT_SHADER_READ_TEXTURE;

        if constexpr (USE_SUBPASS) {
            builder.subpass();
            usages     = gfx::TextureUsageBit::COLOR_ATTACHMENT | gfx::TextureUsageBit::INPUT_ATTACHMENT;
            accessType = gfx::AccessType::FRAGMENT_SHADER_READ_COLOR_INPUT_ATTACHMENT;
        }

        for (uint i = 0; i < 4; ++i) {
            framegraph::Texture::Descriptor gbufferInfo;
            // RGBA8 is suffice for albedo, emission & occlusion
            gbufferInfo.format = i % 3 ? gfx::Format::RGBA16F : gfx::Format::RGBA8;
            gbufferInfo.usage  = usages;
            gbufferInfo.width  = output->getWidth();
            gbufferInfo.height = output->getHeight();
            data.gbuffers[i]   = builder.create<framegraph::Texture>(GBUFFER_NAMES[i], gbufferInfo);

            // Attachment Setup
            framegraph::RenderTargetAttachment::Descriptor gbufferAttachmentInfo;
            gbufferAttachmentInfo.slot          = i;
            gbufferAttachmentInfo.loadOp        = gfx::LoadOp::CLEAR;
            gbufferAttachmentInfo.beginAccesses = {accessType};
            gbufferAttachmentInfo.endAccesses   = {accessType};

            data.gbuffers[i] = builder.write(data.gbuffers[i], gbufferAttachmentInfo);
            builder.writeToBlackboard(GBUFFER_NAMES[i], data.gbuffers[i]);
        }
        framegraph::Texture::Descriptor depthStencilInfo;
        depthStencilInfo.type   = gfx::TextureType::TEX2D;
        depthStencilInfo.usage  = gfx::TextureUsageBit::DEPTH_STENCIL_ATTACHMENT;
        depthStencilInfo.format = gfx::Format::DEPTH_STENCIL;
        depthStencilInfo.width  = output->getWidth();
        depthStencilInfo.height = output->getHeight();
        data.depthStencil       = builder.create<framegraph::Texture>(DEPTH_STENCIL_NAME, depthStencilInfo);

        // Attachment Setup
        framegraph::RenderTargetAttachment::Descriptor depthStencilAttachmentInfo;
        depthStencilAttachmentInfo.loadOp        = gfx::LoadOp::CLEAR;
        depthStencilAttachmentInfo.beginAccesses = {gfx::AccessType::DEPTH_STENCIL_ATTACHMENT_WRITE};
        depthStencilAttachmentInfo.endAccesses   = {gfx::AccessType::DEPTH_STENCIL_ATTACHMENT_WRITE};
        depthStencilAttachmentInfo.usage         = framegraph::RenderTargetAttachment::Usage::DEPTH_STENCIL;

        data.depthStencil = builder.write(data.depthStencil, depthStencilAttachmentInfo);
        builder.writeToBlackboard(DEPTH_STENCIL_NAME, data.depthStencil);

        gfx::Viewport vp{renderArea.x, renderArea.y, renderArea.width, renderArea.height, 0, 1};
        gfx::Rect     rect{renderArea.x, renderArea.y, renderArea.width, renderArea.height};
        builder.setViewport(vp, rect);
    };

    auto gbufferPassExec = [=](const GBufferData & /*data*/, const framegraph::DevicePassResourceTable &table) {
        static gfx::PipelineStateInfo pipelineStateInfo;
        pipelineStateInfo.shader         = gbufferShader.get();
        pipelineStateInfo.pipelineLayout = StandardUniformBuffers::pipelineLayout.get();
        pipelineStateInfo.inputState     = {attributes};
        pipelineStateInfo.primitive      = gfx::PrimitiveMode::TRIANGLE_LIST;

        pipelineStateInfo.renderPass = table.getRenderPass();
        commandBuffer->bindPipelineState(getPipelineState(device, pipelineStateInfo));

        execute();
    };

    struct ShadingData {
        framegraph::TextureHandle gbuffers[4];
        framegraph::TextureHandle lightingOutput;
        framegraph::TextureHandle depthStencil;
    };

    auto shadingPassSetup = [&](framegraph::PassNodeBuilder &builder, ShadingData &data) {
        if constexpr (USE_SUBPASS) builder.subpass(true);

        for (uint i = 0; i < 4; ++i) {
            data.gbuffers[i] = framegraph::TextureHandle(builder.readFromBlackboard(GBUFFER_NAMES[i]));
            data.gbuffers[i] = builder.read(data.gbuffers[i]);
        }

        framegraph::Texture::Descriptor lightingOutputInfo;
        lightingOutputInfo.type   = gfx::TextureType::TEX2D;
        lightingOutputInfo.usage  = gfx::TextureUsageBit::COLOR_ATTACHMENT | gfx::TextureUsageBit::TRANSFER_SRC;
        lightingOutputInfo.format = gfx::Format::RGBA8;
        lightingOutputInfo.width  = output->getWidth();
        lightingOutputInfo.height = output->getHeight();
        data.lightingOutput       = builder.create<framegraph::Texture>(LIGHTING_OUTPUT_NAME, lightingOutputInfo);

        framegraph::RenderTargetAttachment::Descriptor lightingAttachmentInfo;
        lightingAttachmentInfo.loadOp        = gfx::LoadOp::CLEAR;
        lightingAttachmentInfo.clearColor    = clearColor;
        lightingAttachmentInfo.beginAccesses = {gfx::AccessType::TRANSFER_READ};
        lightingAttachmentInfo.endAccesses   = {gfx::AccessType::TRANSFER_READ};

        builder.writeToBlackboard(LIGHTING_OUTPUT_NAME, builder.write(data.lightingOutput, lightingAttachmentInfo));

        framegraph::RenderTargetAttachment::Descriptor depthStencilAttachmentInfo;
        depthStencilAttachmentInfo.loadOp        = gfx::LoadOp::LOAD;
        depthStencilAttachmentInfo.beginAccesses = {gfx::AccessType::DEPTH_STENCIL_ATTACHMENT_WRITE};
        depthStencilAttachmentInfo.endAccesses   = {gfx::AccessType::DEPTH_STENCIL_ATTACHMENT_WRITE};
        depthStencilAttachmentInfo.usage         = framegraph::RenderTargetAttachment::Usage::DEPTH_STENCIL;

        data.depthStencil = framegraph::TextureHandle(builder.readFromBlackboard(DEPTH_STENCIL_NAME));
        data.depthStencil = builder.write(data.depthStencil, depthStencilAttachmentInfo);
        builder.writeToBlackboard(DEPTH_STENCIL_NAME, data.depthStencil);

        gfx::Viewport vp{renderArea.x, renderArea.y, renderArea.width, renderArea.height, 0, 1};
        gfx::Rect     rect{renderArea.x, renderArea.y, renderArea.width, renderArea.height};
        builder.setViewport(vp, rect);
    };

    auto shadingPassExec = [=](const ShadingData &data, const framegraph::DevicePassResourceTable &table) {
        static gfx::PipelineStateInfo pipelineStateInfo;
        pipelineStateInfo.shader                       = lightingShader.get();
        pipelineStateInfo.pipelineLayout               = lightingPipelineLayout.get();
        pipelineStateInfo.inputState                   = {lightingAttributes};
        pipelineStateInfo.rasterizerState.cullMode     = gfx::CullMode::NONE;
        pipelineStateInfo.depthStencilState.depthFunc  = gfx::ComparisonFunc::GREATER;
        pipelineStateInfo.depthStencilState.depthWrite = false;
        pipelineStateInfo.primitive                    = gfx::PrimitiveMode::TRIANGLE_STRIP;
        pipelineStateInfo.subpass                      = table.getSubpassIndex();

        auto *descriptorSet = getDescriptorSet(device, table.getRead(data.gbuffers[0])->getTypedID());
        descriptorSet->bindTexture(1, table.getRead(data.gbuffers[0]));
        descriptorSet->bindTexture(2, table.getRead(data.gbuffers[1]));
        descriptorSet->bindTexture(3, table.getRead(data.gbuffers[2]));
        descriptorSet->bindTexture(4, table.getRead(data.gbuffers[3]));
        descriptorSet->update();

        pipelineStateInfo.renderPass = table.getRenderPass();
        commandBuffer->bindPipelineState(getPipelineState(device, pipelineStateInfo));
        commandBuffer->bindDescriptorSet(0, descriptorSet);

        commandBuffer->bindInputAssembler(lightingInputAssembler.get());
        commandBuffer->draw(lightingInputAssembler.get());
    };

    auto &fg = TestBaseI::fg;

    // same insertion point if the same render pass
    fg.addPass<GBufferData>(99 + USE_SUBPASS, GBUFFER_PASS_NAME, gbufferPassSetup, gbufferPassExec);
    fg.addPass<ShadingData>(100, SHADING_PASS_NAME, shadingPassSetup, shadingPassExec);

    fg.presentFromBlackboard(LIGHTING_OUTPUT_NAME, output);
}

} // namespace cc
