export enum Format {

    UNKNOWN,

    A8,
    L8,
    LA8,

    R8,
    R8SN,
    R8UI,
    R8I,
    R16F,
    R16UI,
    R16I,
    R32F,
    R32UI,
    R32I,

    RG8,
    RG8SN,
    RG8UI,
    RG8I,
    RG16F,
    RG16UI,
    RG16I,
    RG32F,
    RG32UI,
    RG32I,

    RGB8,
    SRGB8,
    RGB8SN,
    RGB8UI,
    RGB8I,
    RGB16F,
    RGB16UI,
    RGB16I,
    RGB32F,
    RGB32UI,
    RGB32I,

    RGBA8,
    BGRA8,
    SRGB8_A8,
    RGBA8SN,
    RGBA8UI,
    RGBA8I,
    RGBA16F,
    RGBA16UI,
    RGBA16I,
    RGBA32F,
    RGBA32UI,
    RGBA32I,

    // Special Format
    R5G6B5,
    R11G11B10F,
    RGB5A1,
    RGBA4,
    RGB10A2,
    RGB10A2UI,
    RGB9E5,

    // Depth-Stencil Format
    DEPTH,
    DEPTH_STENCIL,

    // Compressed Format

    // Block Compression Format, DDS (DirectDraw Surface)
    // DXT1: 3 channels (5:6:5), 1/8 origianl size, with 0 or 1 bit of alpha
    BC1,
    BC1_ALPHA,
    BC1_SRGB,
    BC1_SRGB_ALPHA,
    // DXT3: 4 channels (5:6:5), 1/4 origianl size, with 4 bits of alpha
    BC2,
    BC2_SRGB,
    // DXT5: 4 channels (5:6:5), 1/4 origianl size, with 8 bits of alpha
    BC3,
    BC3_SRGB,
    // 1 channel (8), 1/4 origianl size
    BC4,
    BC4_SNORM,
    // 2 channels (8:8), 1/2 origianl size
    BC5,
    BC5_SNORM,
    // 3 channels (16:16:16), half-floating point, 1/6 origianl size
    // UF16: unsigned float, 5 exponent bits + 11 mantissa bits
    // SF16: signed float, 1 signed bit + 5 exponent bits + 10 mantissa bits
    BC6H_UF16,
    BC6H_SF16,
    // 4 channels (4~7 bits per channel) with 0 to 8 bits of alpha, 1/3 original size
    BC7,
    BC7_SRGB,

    // Ericsson Texture Compression Format
    ETC_RGB8,
    ETC2_RGB8,
    ETC2_SRGB8,
    ETC2_RGB8_A1,
    ETC2_SRGB8_A1,
    ETC2_RGBA8,
    ETC2_SRGB8_A8,
    EAC_R11,
    EAC_R11SN,
    EAC_RG11,
    EAC_RG11SN,

    // PVRTC (PowerVR)
    PVRTC_RGB2,
    PVRTC_RGBA2,
    PVRTC_RGB4,
    PVRTC_RGBA4,
    PVRTC2_2BPP,
    PVRTC2_4BPP,

    // ASTC (Adaptive Scalable Texture Compression)
    ASTC_RGBA_4X4,
    ASTC_RGBA_5X4,
    ASTC_RGBA_5X5,
    ASTC_RGBA_6X5,
    ASTC_RGBA_6X6,
    ASTC_RGBA_8X5,
    ASTC_RGBA_8X6,
    ASTC_RGBA_8X8,
    ASTC_RGBA_10X5,
    ASTC_RGBA_10X6,
    ASTC_RGBA_10X8,
    ASTC_RGBA_10X10,
    ASTC_RGBA_12X10,
    ASTC_RGBA_12X12,

    // ASTC (Adaptive Scalable Texture Compression) SRGB
    ASTC_SRGBA_4X4,
    ASTC_SRGBA_5X4,
    ASTC_SRGBA_5X5,
    ASTC_SRGBA_6X5,
    ASTC_SRGBA_6X6,
    ASTC_SRGBA_8X5,
    ASTC_SRGBA_8X6,
    ASTC_SRGBA_8X8,
    ASTC_SRGBA_10X5,
    ASTC_SRGBA_10X6,
    ASTC_SRGBA_10X8,
    ASTC_SRGBA_10X10,
    ASTC_SRGBA_12X10,
    ASTC_SRGBA_12X12,

    // Total count
    COUNT,
}

export enum Type {
    UNKNOWN,
    BOOL,
    BOOL2,
    BOOL3,
    BOOL4,
    INT,
    INT2,
    INT3,
    INT4,
    UINT,
    UINT2,
    UINT3,
    UINT4,
    FLOAT,
    FLOAT2,
    FLOAT3,
    FLOAT4,
    MAT2,
    MAT2X3,
    MAT2X4,
    MAT3X2,
    MAT3,
    MAT3X4,
    MAT4X2,
    MAT4X3,
    MAT4,
    // combined image samplers
    SAMPLER1D,
    SAMPLER1D_ARRAY,
    SAMPLER2D,
    SAMPLER2D_ARRAY,
    SAMPLER3D,
    SAMPLER_CUBE,
    // sampler
    SAMPLER,
    // sampled textures
    TEXTURE1D,
    TEXTURE1D_ARRAY,
    TEXTURE2D,
    TEXTURE2D_ARRAY,
    TEXTURE3D,
    TEXTURE_CUBE,
    // storage images
    IMAGE1D,
    IMAGE1D_ARRAY,
    IMAGE2D,
    IMAGE2D_ARRAY,
    IMAGE3D,
    IMAGE_CUBE,
    // input attachment
    SUBPASS_INPUT,
    COUNT,
}

export enum FormatType {
    NONE,
    UNORM,
    SNORM,
    UINT,
    INT,
    UFLOAT,
    FLOAT,
}

export class FormatInfo {
    declare private _token: never; // to make sure all usages must be an instance of this exact class, not assembled from plain object

    constructor(
        public readonly name: string = '',
        public readonly size: number = 0,
        public readonly count: number = 0,
        public readonly type: FormatType = FormatType.NONE,
        public readonly hasAlpha: boolean = false,
        public readonly hasDepth: boolean = false,
        public readonly hasStencil: boolean = false,
        public readonly isCompressed: boolean = false,
    ) { }
}

export const FormatInfos = Object.freeze([

    new FormatInfo('UNKNOWN', 0, 0, FormatType.NONE, false, false, false, false),

    new FormatInfo('A8', 1, 1, FormatType.UNORM, true, false, false, false),
    new FormatInfo('L8', 1, 1, FormatType.UNORM, false, false, false, false),
    new FormatInfo('LA8', 1, 2, FormatType.UNORM, true, false, false, false),

    new FormatInfo('R8', 1, 1, FormatType.UNORM, false, false, false, false),
    new FormatInfo('R8SN', 1, 1, FormatType.SNORM, false, false, false, false),
    new FormatInfo('R8UI', 1, 1, FormatType.UINT, false, false, false, false),
    new FormatInfo('R8I', 1, 1, FormatType.INT, false, false, false, false),
    new FormatInfo('R16F', 2, 1, FormatType.FLOAT, false, false, false, false),
    new FormatInfo('R16UI', 2, 1, FormatType.UINT, false, false, false, false),
    new FormatInfo('R16I', 2, 1, FormatType.INT, false, false, false, false),
    new FormatInfo('R32F', 4, 1, FormatType.FLOAT, false, false, false, false),
    new FormatInfo('R32UI', 4, 1, FormatType.UINT, false, false, false, false),
    new FormatInfo('R32I', 4, 1, FormatType.INT, false, false, false, false),

    new FormatInfo('RG8', 2, 2, FormatType.UNORM, false, false, false, false),
    new FormatInfo('RG8SN', 2, 2, FormatType.SNORM, false, false, false, false),
    new FormatInfo('RG8UI', 2, 2, FormatType.UINT, false, false, false, false),
    new FormatInfo('RG8I', 2, 2, FormatType.INT, false, false, false, false),
    new FormatInfo('RG16F', 4, 2, FormatType.FLOAT, false, false, false, false),
    new FormatInfo('RG16UI', 4, 2, FormatType.UINT, false, false, false, false),
    new FormatInfo('RG16I', 4, 2, FormatType.INT, false, false, false, false),
    new FormatInfo('RG32F', 8, 2, FormatType.FLOAT, false, false, false, false),
    new FormatInfo('RG32UI', 8, 2, FormatType.UINT, false, false, false, false),
    new FormatInfo('RG32I', 8, 2, FormatType.INT, false, false, false, false),

    new FormatInfo('RGB8', 3, 3, FormatType.UNORM, false, false, false, false),
    new FormatInfo('SRGB8', 3, 3, FormatType.UNORM, false, false, false, false),
    new FormatInfo('RGB8SN', 3, 3, FormatType.SNORM, false, false, false, false),
    new FormatInfo('RGB8UI', 3, 3, FormatType.UINT, false, false, false, false),
    new FormatInfo('RGB8I', 3, 3, FormatType.INT, false, false, false, false),
    new FormatInfo('RGB16F', 6, 3, FormatType.FLOAT, false, false, false, false),
    new FormatInfo('RGB16UI', 6, 3, FormatType.UINT, false, false, false, false),
    new FormatInfo('RGB16I', 6, 3, FormatType.INT, false, false, false, false),
    new FormatInfo('RGB32F', 12, 3, FormatType.FLOAT, false, false, false, false),
    new FormatInfo('RGB32UI', 12, 3, FormatType.UINT, false, false, false, false),
    new FormatInfo('RGB32I', 12, 3, FormatType.INT, false, false, false, false),

    new FormatInfo('RGBA8', 4, 4, FormatType.UNORM, true, false, false, false),
    new FormatInfo('BGRA8', 4, 4, FormatType.UNORM, true, false, false, false),
    new FormatInfo('SRGB8_A8', 4, 4, FormatType.UNORM, true, false, false, false),
    new FormatInfo('RGBA8SN', 4, 4, FormatType.SNORM, true, false, false, false),
    new FormatInfo('RGBA8UI', 4, 4, FormatType.UINT, true, false, false, false),
    new FormatInfo('RGBA8I', 4, 4, FormatType.INT, true, false, false, false),
    new FormatInfo('RGBA16F', 8, 4, FormatType.FLOAT, true, false, false, false),
    new FormatInfo('RGBA16UI', 8, 4, FormatType.UINT, true, false, false, false),
    new FormatInfo('RGBA16I', 8, 4, FormatType.INT, true, false, false, false),
    new FormatInfo('RGBA32F', 16, 4, FormatType.FLOAT, true, false, false, false),
    new FormatInfo('RGBA32UI', 16, 4, FormatType.UINT, true, false, false, false),
    new FormatInfo('RGBA32I', 16, 4, FormatType.INT, true, false, false, false),

    new FormatInfo('R5G6B5', 2, 3, FormatType.UNORM, false, false, false, false),
    new FormatInfo('R11G11B10F', 4, 3, FormatType.FLOAT, false, false, false, false),
    new FormatInfo('RGB5A1', 2, 4, FormatType.UNORM, true, false, false, false),
    new FormatInfo('RGBA4', 2, 4, FormatType.UNORM, true, false, false, false),
    new FormatInfo('RGB10A2', 2, 4, FormatType.UNORM, true, false, false, false),
    new FormatInfo('RGB10A2UI', 2, 4, FormatType.UINT, true, false, false, false),
    new FormatInfo('RGB9E5', 2, 4, FormatType.FLOAT, true, false, false, false),

    new FormatInfo('DEPTH', 4, 1, FormatType.FLOAT, false, true, false, false),
    new FormatInfo('DEPTH_STENCIL', 5, 2, FormatType.FLOAT, false, true, true, false),

    new FormatInfo('BC1', 1, 3, FormatType.UNORM, false, false, false, true),
    new FormatInfo('BC1_ALPHA', 1, 4, FormatType.UNORM, true, false, false, true),
    new FormatInfo('BC1_SRGB', 1, 3, FormatType.UNORM, false, false, false, true),
    new FormatInfo('BC1_SRGB_ALPHA', 1, 4, FormatType.UNORM, true, false, false, true),
    new FormatInfo('BC2', 1, 4, FormatType.UNORM, true, false, false, true),
    new FormatInfo('BC2_SRGB', 1, 4, FormatType.UNORM, true, false, false, true),
    new FormatInfo('BC3', 1, 4, FormatType.UNORM, true, false, false, true),
    new FormatInfo('BC3_SRGB', 1, 4, FormatType.UNORM, true, false, false, true),
    new FormatInfo('BC4', 1, 1, FormatType.UNORM, false, false, false, true),
    new FormatInfo('BC4_SNORM', 1, 1, FormatType.SNORM, false, false, false, true),
    new FormatInfo('BC5', 1, 2, FormatType.UNORM, false, false, false, true),
    new FormatInfo('BC5_SNORM', 1, 2, FormatType.SNORM, false, false, false, true),
    new FormatInfo('BC6H_UF16', 1, 3, FormatType.UFLOAT, false, false, false, true),
    new FormatInfo('BC6H_SF16', 1, 3, FormatType.FLOAT, false, false, false, true),
    new FormatInfo('BC7', 1, 4, FormatType.UNORM, true, false, false, true),
    new FormatInfo('BC7_SRGB', 1, 4, FormatType.UNORM, true, false, false, true),

    new FormatInfo('ETC_RGB8', 1, 3, FormatType.UNORM, false, false, false, true),
    new FormatInfo('ETC2_RGB8', 1, 3, FormatType.UNORM, false, false, false, true),
    new FormatInfo('ETC2_SRGB8', 1, 3, FormatType.UNORM, false, false, false, true),
    new FormatInfo('ETC2_RGB8_A1', 1, 4, FormatType.UNORM, true, false, false, true),
    new FormatInfo('ETC2_SRGB8_A1', 1, 4, FormatType.UNORM, true, false, false, true),
    new FormatInfo('ETC2_RGBA8', 2, 4, FormatType.UNORM, true, false, false, true),
    new FormatInfo('ETC2_SRGB8_A8', 2, 4, FormatType.UNORM, true, false, false, true),
    new FormatInfo('EAC_R11', 1, 1, FormatType.UNORM, false, false, false, true),
    new FormatInfo('EAC_R11SN', 1, 1, FormatType.SNORM, false, false, false, true),
    new FormatInfo('EAC_RG11', 2, 2, FormatType.UNORM, false, false, false, true),
    new FormatInfo('EAC_RG11SN', 2, 2, FormatType.SNORM, false, false, false, true),

    new FormatInfo('PVRTC_RGB2', 2, 3, FormatType.UNORM, false, false, false, true),
    new FormatInfo('PVRTC_RGBA2', 2, 4, FormatType.UNORM, true, false, false, true),
    new FormatInfo('PVRTC_RGB4', 2, 3, FormatType.UNORM, false, false, false, true),
    new FormatInfo('PVRTC_RGBA4', 2, 4, FormatType.UNORM, true, false, false, true),
    new FormatInfo('PVRTC2_2BPP', 2, 4, FormatType.UNORM, true, false, false, true),
    new FormatInfo('PVRTC2_4BPP', 2, 4, FormatType.UNORM, true, false, false, true),

    new FormatInfo('ASTC_RGBA_4x4', 1, 4, FormatType.UNORM, true, false, false, true),
    new FormatInfo('ASTC_RGBA_5x4', 1, 4, FormatType.UNORM, true, false, false, true),
    new FormatInfo('ASTC_RGBA_5x5', 1, 4, FormatType.UNORM, true, false, false, true),
    new FormatInfo('ASTC_RGBA_6x5', 1, 4, FormatType.UNORM, true, false, false, true),
    new FormatInfo('ASTC_RGBA_6x6', 1, 4, FormatType.UNORM, true, false, false, true),
    new FormatInfo('ASTC_RGBA_8x5', 1, 4, FormatType.UNORM, true, false, false, true),
    new FormatInfo('ASTC_RGBA_8x6', 1, 4, FormatType.UNORM, true, false, false, true),
    new FormatInfo('ASTC_RGBA_8x8', 1, 4, FormatType.UNORM, true, false, false, true),
    new FormatInfo('ASTC_RGBA_10x5', 1, 4, FormatType.UNORM, true, false, false, true),
    new FormatInfo('ASTC_RGBA_10x6', 1, 4, FormatType.UNORM, true, false, false, true),
    new FormatInfo('ASTC_RGBA_10x8', 1, 4, FormatType.UNORM, true, false, false, true),
    new FormatInfo('ASTC_RGBA_10x10', 1, 4, FormatType.UNORM, true, false, false, true),
    new FormatInfo('ASTC_RGBA_12x10', 1, 4, FormatType.UNORM, true, false, false, true),
    new FormatInfo('ASTC_RGBA_12x12', 1, 4, FormatType.UNORM, true, false, false, true),

    new FormatInfo('ASTC_SRGBA_4x4', 1, 4, FormatType.UNORM, true, false, false, true),
    new FormatInfo('ASTC_SRGBA_5x4', 1, 4, FormatType.UNORM, true, false, false, true),
    new FormatInfo('ASTC_SRGBA_5x5', 1, 4, FormatType.UNORM, true, false, false, true),
    new FormatInfo('ASTC_SRGBA_6x5', 1, 4, FormatType.UNORM, true, false, false, true),
    new FormatInfo('ASTC_SRGBA_6x6', 1, 4, FormatType.UNORM, true, false, false, true),
    new FormatInfo('ASTC_SRGBA_8x5', 1, 4, FormatType.UNORM, true, false, false, true),
    new FormatInfo('ASTC_SRGBA_8x6', 1, 4, FormatType.UNORM, true, false, false, true),
    new FormatInfo('ASTC_SRGBA_8x8', 1, 4, FormatType.UNORM, true, false, false, true),
    new FormatInfo('ASTC_SRGBA_10x5', 1, 4, FormatType.UNORM, true, false, false, true),
    new FormatInfo('ASTC_SRGBA_10x6', 1, 4, FormatType.UNORM, true, false, false, true),
    new FormatInfo('ASTC_SRGBA_10x8', 1, 4, FormatType.UNORM, true, false, false, true),
    new FormatInfo('ASTC_SRGBA_10x10', 1, 4, FormatType.UNORM, true, false, false, true),
    new FormatInfo('ASTC_SRGBA_12x10', 1, 4, FormatType.UNORM, true, false, false, true),
    new FormatInfo('ASTC_SRGBA_12x12', 1, 4, FormatType.UNORM, true, false, false, true),
]);


const sp = {
    name: 'Basic Triangle',
    vert: `
                vec4 vert() {
                    return u_mvp * vec4(a_position, 0.0, 1.0);
                }
            `,
    frag: `
                vec4 frag() {
                    return u_color;
                }
            `,
    attributes: [
        { name: 'a_position', format: Format.RG32F },
        { name: 'a_color', format: Format.RGB32UI },
    ],
    blocks: [
        { name: 'Color', members: [{ name: 'u_color', type: Type.FLOAT4 }] },
        { name: 'MVP', members: [{ name: 'u_mvp', type: Type.MAT4 }] },
    ],
}

function attribute_format(format: Format): string {
    let res = "vec"
    res += FormatInfos[format].count
    return res
}

let vert_shader = "precision highp float;\n"
for (let i = 0; i < sp.attributes.length; i++) {
    if (sp.vert.search(sp.attributes[i].name) !== -1)
        vert_shader += "layout(location = " + i + ") in vec" + FormatInfos[sp.attributes[i].format].count + " " + sp.attributes[i].name + ";\n"
}

let frag_shader = "precision highp float;\n"
for (let i = 0; i < sp.attributes.length; i++) {
    if (sp.frag.search(sp.attributes[i].name) !== -1)
        frag_shader += "layout(location = " + i + ") in vec" + FormatInfos[sp.attributes[i].format].count + " " + sp.attributes[i].name + ";\n"
}

// bindings
for (let i = 0; i < sp.blocks.length; i++) {
    vert_shader += "layout(set = " + ""
}

console.log("------------------------------\nvert shader: \n------------------------------",);
console.log(vert_shader, "\n");

console.log("------------------------------\nfrag shader: \n------------------------------",);
console.log(frag_shader, "\n------------------------------");
