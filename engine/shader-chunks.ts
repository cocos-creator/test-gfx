export const shaderChunks: Record<string, string> = {
    texture1: `
vec4 tex(sampler2D tex, vec2 p) {
    return texture2D(tex, p);
}
vec4 tex(sampler2D tex, vec2 p, float bias) {
    return texture2D(tex, p, bias);
}
vec4 tex(samplerCube tex, vec3 p) {
    return textureCube(tex, p);
}
vec4 tex(samplerCube tex, vec3 p, float bias) {
    return textureCube(tex, p, bias);
}
vec4 textureProj(sampler2D tex, vec3 p) {
    return texture2DProj(tex, p);
}
vec4 textureProj(sampler2D tex, vec3 p, float bias) {
    return texture2DProj(tex, p, bias);
}
vec4 textureProj(sampler2D tex, vec4 p) {
    return texture2DProj(tex, p);
}
vec4 textureProj(sampler2D tex, vec4 p, float bias) {
    return texture2DProj(tex, p, bias);
}
`,
    vertTextureLod1: `
vec4 textureLod(sampler2D tex, vec2 p, float lod) {
    return texture2DLod(tex, p, lod);
}
vec4 textureLod(samplerCube tex, vec3 p, float lod) {
    return textureCubeLod(tex, p, lod);
}
vec4 textureProjLod(sampler2D tex, vec3 p, float lod) {
    return texture2DProjLod(tex, p, lod);
}
vec4 textureProjLod(sampler2D tex, vec4 p, float lod) {
    return texture2DProjLod(tex, p, lod);
}
`,
    fragTextureLod1: `
vec4 textureLod(sampler2D tex, vec2 p, float lod) {
    #ifdef GL_EXT_shader_texture_lod
        return texture2DLodEXT(tex, p, lod);
    #else
        return texture2D(tex, p, lod); // fallback to bias
    #endif
}

vec4 textureLod(samplerCube tex, vec3 p, float lod) {
    #ifdef GL_EXT_shader_texture_lod
        return textureCubeLodEXT(tex, p, lod);
    #else
        return textureCube(tex, p, lod); // fallback to bias
    #endif
}

vec4 textureGrad(sampler2D tex, vec2 p, vec2 dPdx, vec2 dPdy) {
    #ifdef GL_EXT_shader_texture_lod
        return texture2DGradEXT(tex, p, dPdx, dPdy);
    #else
        return texture2D(tex, p);
    #endif
}

vec4 textureGrad(samplerCube tex, vec3 p, vec3 dPdx, vec3 dPdy) {
    #ifdef GL_EXT_shader_texture_lod
        return textureCubeGradEXT(tex, p, dPdx, dPdy);
    #else
        return textureCube(tex, p);
    #endif
}
    `,
};
