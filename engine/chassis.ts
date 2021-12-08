import { CommandBuffer } from 'gfx/base/command-buffer';
import { Buffer } from 'gfx/base/buffer';
import {
    Attribute, BufferInfo, BufferUsageBit, DescriptorSetLayoutBinding, PipelineLayoutInfo,
    DescriptorType, Format, IndirectBuffer, InputAssemblerInfo, InputState, MemoryUsageBit,
    PrimitiveMode, RenderPassInfo, ShaderInfo, ShaderStage,
    ShaderStageFlagBit, Type, Uniform, UniformBlock, DescriptorSetLayoutInfo, DescriptorSetInfo, FormatInfos, API, GetTypeSize, UniformSamplerTexture, UniformInputAttachment,
} from 'gfx/base/define';

// import {
//     genHandle, getTypeFromHandle, getBindingFromHandle, getCountFromHandle,
//     getOffsetFromHandle, customizeType,
// } from 'renderer/core/pass-utils';
import { DescriptorSet } from 'gfx/base/descriptor-set';
import { DescriptorSetLayout } from 'gfx/base/descriptor-set-layout';
import { InputAssembler } from 'gfx/base/input-assembler';
import { PipelineLayout } from 'gfx/base/pipeline-layout';
import { BlendState, DepthStencilState, PipelineState, PipelineStateInfo, RasterizerState } from 'gfx/base/pipeline-state';
import { Shader } from 'gfx/base/shader';
import { Sampler } from 'gfx/base/states/sampler';
import { Texture } from 'gfx/base/texture';
import { assert } from 'platform/debug';
import { murmurhash2_32_gc } from 'utils/murmurhash2_gc';
import { RenderPass } from 'gfx/base/render-pass';
import { IMat4Like, IVec2Like, IVec4Like, Mat4, Vec2, Vec4 } from './math';
import { IShaderSources, IStandardShaderSource, TestBase } from './test-base';

export class TypeInfo {
    declare private _token: never; // to make sure all usages must be an instance of this exact class, not assembled from plain object

    constructor (
        public readonly name: string = '',
        public readonly glslName: string = '',
        public readonly size: number = 0, // not used
        public readonly count: number = 0, // not used
        public readonly type: Type = Type.UNKNOWN,
    ) { }
}

export const TypeInfos = Object.freeze([
    new TypeInfo('UNKNOWN', '', 0, 0, Type.UNKNOWN),

    new TypeInfo('BOOL', 'bool', 0, 0, Type.BOOL),
    new TypeInfo('BOOL2', 'bvec2', 0, 0, Type.BOOL2),
    new TypeInfo('BOOL3', 'bvec3', 0, 0, Type.BOOL3),
    new TypeInfo('BOOL4', 'bvec4', 0, 0, Type.BOOL),

    new TypeInfo('INT', 'int', 0, 0, Type.INT),
    new TypeInfo('INT2', 'ivec2', 0, 0, Type.INT2),
    new TypeInfo('INT3', 'ivec3', 0, 0, Type.INT3),
    new TypeInfo('INT4', 'ivec4', 0, 0, Type.INT4),

    new TypeInfo('UINT', 'uint', 0, 0, Type.UINT),
    new TypeInfo('UINT2', 'uvec2', 0, 0, Type.UINT2),
    new TypeInfo('UINT3', 'uvec3', 0, 0, Type.UINT3),
    new TypeInfo('UINT4', 'uvec4', 0, 0, Type.UINT4),

    new TypeInfo('FLOAT', 'float', 0, 0, Type.FLOAT),
    new TypeInfo('FLOAT2', 'vec2', 0, 0, Type.FLOAT2),
    new TypeInfo('FLOAT3', 'vec3', 0, 0, Type.FLOAT3),
    new TypeInfo('FLOAT4', 'vec4', 0, 0, Type.FLOAT4),

    new TypeInfo('MAT2', 'mat2', 0, 0, Type.MAT2),
    new TypeInfo('MAT2X3', 'mat2x3', 0, 0, Type.MAT2X3),
    new TypeInfo('MAT2X4', 'mat2x4', 0, 0, Type.MAT2X4),

    new TypeInfo('MAT3X2', 'mat3x2', 0, 0, Type.MAT3X2),
    new TypeInfo('MAT3', 'mat3', 0, 0, Type.MAT3),
    new TypeInfo('MAT3X4', 'mat3x4', 0, 0, Type.MAT3X4),

    new TypeInfo('MAT4X2', 'mat4x2', 0, 0, Type.MAT4X2),
    new TypeInfo('MAT4X3', 'mat4x3', 0, 0, Type.MAT4X3),
    new TypeInfo('MAT4', 'mat4', 0, 0, Type.MAT4),

    new TypeInfo('SAMPLER1D', 'sampler1D', 0, 0, Type.SAMPLER1D),
    new TypeInfo('SAMPLER1D_ARRAY', 'sampler1DArray', 0, 0, Type.SAMPLER1D_ARRAY),
    new TypeInfo('SAMPLER2D', 'sampler2D', 0, 0, Type.SAMPLER2D),
    new TypeInfo('SAMPLER2D_ARRAY', 'sampler2DArray', 0, 0, Type.SAMPLER2D_ARRAY),

    new TypeInfo('SAMPLER3D', 'sampler3D', 0, 0, Type.SAMPLER3D),
    new TypeInfo('SAMPLER_CUBE', 'samplerCubeArray', 0, 0, Type.SAMPLER_CUBE),

    new TypeInfo('SAMPLER', 'sampler', 0, 0, Type.SAMPLER),

    new TypeInfo('TEXTURE1D', 'texture1d', 0, 0, Type.TEXTURE1D),
    new TypeInfo('TEXTURE1D_ARRAY', 'texture1dArray', 0, 0, Type.TEXTURE1D_ARRAY),
    new TypeInfo('TEXTURE2D', 'texture2d', 0, 0, Type.TEXTURE2D),
    new TypeInfo('TEXTURE2D_ARRAY', 'texture2dArray', 0, 0, Type.TEXTURE2D_ARRAY),
    new TypeInfo('TEXTURE3D', 'texture3d', 0, 0, Type.TEXTURE3D),
    new TypeInfo('TEXTURE_CUBE', 'textureCube', 0, 0, Type.TEXTURE_CUBE),

    new TypeInfo('IMAGE1D', 'image1d', 0, 0, Type.IMAGE1D),
    new TypeInfo('IMAGE1D_ARRAY', 'image1dArray', 0, 0, Type.IMAGE1D_ARRAY),
    new TypeInfo('IMAGE2D', 'image2d', 0, 0, Type.IMAGE2D),
    new TypeInfo('IMAGE2D_ARRAY', 'image2dArray', 0, 0, Type.IMAGE2D_ARRAY),
    new TypeInfo('IMAGE3D', 'image3d', 0, 0, Type.IMAGE3D),
    new TypeInfo('IMAGE_CUBE', 'imageCube', 0, 0, Type.IMAGE_CUBE),

    // // input attachment
    // SUBPASS_INPUT,
    // COUNT,
]);

export interface IShaderAttribute {
    name: string;
    format: Format;
    isNormalized?: boolean; // default to false
    isInstanced?: boolean;  // default to false
    stream?: number;        // default to 0
}

export interface IShaderBlockMember {
    name: string;
    type: Type;
    count?: number; // default to 1
}

export interface IShaderBlock {
    name: string;
    members: IShaderBlockMember[];
    set: number;
    dynamic?: boolean; // default to false
}

export interface IShaderSamplerTexture {
    name: string;
    type: Type;
    set: number;
    count?: number; // default to 1
}

export interface IShaderVaryings {
    name: string;
    type: Type;
}

export interface IProgramInfo {
    name: string;
    vert: string; // implementation only, entry function 'vec4 vert()'
    frag: string; // implementation only, entry function 'vec4 frag()'
    attributes?: IShaderAttribute[];           // default to empty array
    blocks?: IShaderBlock[];                   // default to empty array
    samplerTextures?: IShaderSamplerTexture[]; // default to empty array
    varyings?: IShaderVaryings[];              // default to empty array
    attachments?: number;                      // default to 1, number of render targets
    defaultStates?: IPipelineStateInfo;        // default to identity values
}

export interface IProgramBindingInfo {
    maxInstanceCount?: number; // default to 1
}

export interface IProgramInputInfo {
    maxVertexCount: number;
    maxIndexCount?: number;           // default to 0
    maxIndirectDrawCount?: number;    // default to 0
    vertexFrequentlyUpdated?: number; // default to 0 (bitwise mask)
    indexFrequentlyUpdated?: boolean; // default to false
    indexU32?: boolean;               // default to false
}

export interface IPipelineStateInfo {
    rasterizerState?: RasterizerState;     // default to default value
    depthStencilState?: DepthStencilState; // default to default value
    blendState?: BlendState;               // default to default value
    primitive?: PrimitiveMode;             // default to default value
}

export interface IProgramHandle extends Number {
    _: never;
}
export const NULL_HANDLE: IProgramHandle = 0 as unknown as IProgramHandle;
const setMask = 0xC0000000; // 2 bit => 4 sets
const bindingMask = 0x3f000000; //  6 bits => 64 types
const typeMask = 0x00fc0000; //  6 bits => 64 bindings
const countMask = 0x0003fc00; //  8 bits => 256 vectors
const offsetMask = 0x000003ff; // 10 bits => 1024 vectors

export const genHandle = (set: number, binding: number, type: Type, count: number, offset = 0): number => ((set << 30) & setMask)
    | ((binding << 24) & typeMask) | ((type << 18) & bindingMask) | (count << 10) & countMask | (offset & offsetMask);
export const getSetFromHandle = (handle: number): number => (handle & bindingMask) >>> 30;
export const getBindingFromHandle = (handle: number): number => (handle & bindingMask) >>> 24;
export const getTypeFromHandle = (handle: number): number => (handle & typeMask) >>> 18;
export const getCountFromHandle = (handle: number): number => (handle & countMask) >>> 10;
export const getOffsetFromHandle = (handle: number): number => (handle & offsetMask);
export const customizeType = (handle: number, type: Type): number => (handle & ~typeMask) | ((type << 18) & typeMask);

function hashObject (o: object) {
    let hash = 0;
    for (const _ in o) ++hash;
    for (const k in o) {
        const v = ((o as Record<string, unknown>)[k] || 0) as object | number; // convert null/undefined to 0
        const value = typeof v === 'object' ? hashObject(v) : v;
        hash ^= value + 0x9e3779b9 + (hash << 6) + (hash >> 2); // boost::hash_combine
    }
    return hash;
}

export class Program {
    private _shader: Shader;
    private _layout: PipelineLayout;
    private _bindingMap: Record<string, number>;
    private _pipelineState: Map<number, PipelineState>; // hash map
    private _currentPipelineState: number;

    constructor (info: IProgramInfo) {
        // generate shader sources
        const shaderSources: IStandardShaderSource = this._getAppropriateShaderSource(info);

        const shaderName = info.name;
        // set render stages
        const shaderStages = [
            new ShaderStage(ShaderStageFlagBit.VERTEX, shaderSources.vert),
            new ShaderStage(ShaderStageFlagBit.FRAGMENT, shaderSources.frag),
        ];
        // set vertex buffer attributes
        const shaderAttributes = info.attributes?.map((attribute, index) => new Attribute(
            attribute.name,
            attribute.format,
            attribute.isNormalized,
            attribute.stream,
            attribute.isInstanced,
            index,
        )) || [];
        // set the uniform buffer
        const shaderBlocks = info.blocks?.map((block, index) => new UniformBlock(
            block.set,
            index,
            block.name,
            block.members.map((member) => new Uniform(member.name, member.type, member.count)),
            1,
        )) || []; // block counts
        const samplerTextures = info.samplerTextures?.map((samplerTexture, index) => new UniformSamplerTexture(
            samplerTexture.set,
            index + (info.blocks?.length || 0),
            samplerTexture.name,
            samplerTexture.type,
            samplerTexture.count || 1,
        )) || [];

        this._shader = TestBase.device.createShader(new ShaderInfo(
            shaderName,
            shaderStages,
            shaderAttributes,
            shaderBlocks,
            [],
            samplerTextures, // what about other arguments?
        ));

        // create bindings
        // the number of layout should be the same as the required sets by one frame.
        // There can be more than one descriptorSet for each instance/pass
        const descriptorSetIds = this._descriptorSetIds(info.blocks, info.samplerTextures);
        const descriptorSetLayouts: DescriptorSetLayout[] = [];
        this._bindingMap = {};
        // functions to determine by which shader stage these uniform buffers are used
        const vert_search = (member: IShaderBlockMember) => info.vert.search(member.name) !== -1;
        const frag_search = (member: IShaderBlockMember) => info.frag.search(member.name) !== -1;

        // creates layout bindings and fill bindingMap.
        for (const descriptorSetId of descriptorSetIds) {
            const bindings: DescriptorSetLayoutBinding[] = [];
            const blocks = info.blocks?.filter((block) => block.set === descriptorSetId) || [];
            const textures = info.samplerTextures?.filter((texture) => texture.set === descriptorSetId) || [];

            for (const block of blocks) {
                let shaderStageFlag = ShaderStageFlagBit.NONE;
                if (block.members.some(vert_search)) shaderStageFlag |= ShaderStageFlagBit.VERTEX;
                else if (block.members.some(frag_search)) shaderStageFlag |= ShaderStageFlagBit.FRAGMENT;

                if (shaderStageFlag !== ShaderStageFlagBit.NONE) {
                    let offset = 0;
                    for (const uniform of block.members) {
                        this._bindingMap[uniform.name] = genHandle(descriptorSetId, bindings.length, uniform.type, uniform.count || 1, offset);
                        // no bit shift needed
                        offset += GetTypeSize(uniform.type) * (uniform.count || 1);
                    }
                    bindings.push(new DescriptorSetLayoutBinding(
                        bindings.length,
                        DescriptorType.UNIFORM_BUFFER,
                        1,
                        shaderStageFlag,
                    ));
                }
            }
            for (const samplerTexture of textures) {
                if (samplerTexture.set === descriptorSetId) {
                    this._bindingMap[samplerTexture.name] = genHandle(descriptorSetId,
                        bindings.length, samplerTexture.type, samplerTexture.count || 1);
                    bindings.push(new DescriptorSetLayoutBinding(bindings.length, DescriptorType.SAMPLER_TEXTURE, 1));
                }
            }
            descriptorSetLayouts.push(TestBase.device.createDescriptorSetLayout(new DescriptorSetLayoutInfo(bindings)));
        }

        this._layout = TestBase.device.createPipelineLayout(new PipelineLayoutInfo(descriptorSetLayouts));
        this._pipelineState = new Map<number, PipelineState>();
        this._currentPipelineState = 0;
        this.setPipelineState(info.defaultStates || {});
    }

    private _bufferStreamIds (attributes: IShaderAttribute[] = []): number[] {
        const streamIds: number[] = [];
        for (const attribute of attributes) {
            if (streamIds.indexOf(attribute.stream as number) === -1) streamIds.push(attribute.stream as number);
        }
        streamIds.sort((a, b) => a - b);
        return streamIds;
    }

    private _descriptorSetIds (blocks: IShaderBlock[] = [], samplerTextures: IShaderSamplerTexture[] = []): number[] {
        const descriptorSetIds: number[] = [];
        for (const block of blocks || []) { if (descriptorSetIds.indexOf(block.set) === -1) descriptorSetIds.push(block.set); }
        for (const texture of samplerTextures || []) { if (descriptorSetIds.indexOf(texture.set) === -1) descriptorSetIds.push(texture.set); }
        return descriptorSetIds.sort((a, b) => a - b);
    }

    private _shaderGenGlsl4 (info: IProgramInfo) {
        let vert_shader = 'precision highp float;\n';
        let frag_shader = 'precision highp float;\n';

        const streamIds: number[] = this._bufferStreamIds(info.attributes);
        for (const streamId of streamIds) {
            let i = 0;
            for (const attribute of info.attributes || []) {
                if (attribute.stream === streamId) {
                    vert_shader += `layout(binding = ${streamId}, location = ${i++}) in`
                        + ` vec${FormatInfos[attribute.format].count} ${attribute.name};\n`;
                }
            }
        }

        const descriptorSetIds: number[] = this._descriptorSetIds(info.blocks, info.samplerTextures);
        const vert_search = (member: IShaderBlockMember) => info.vert.search(member.name) !== -1;
        const frag_search = (member: IShaderBlockMember) => info.frag.search(member.name) !== -1;

        for (const descriptorSetId of descriptorSetIds) {
            let i = 0;
            const blocks = info.blocks?.filter((block) => block.set === descriptorSetId) || [];
            const textures = info.samplerTextures?.filter((texture) => texture.set === descriptorSetId) || [];

            for (const block of blocks) {
                if (block.members.some(vert_search)) {
                    vert_shader += `layout(set = ${descriptorSetId}, binding = ${i}) uniform ${block.name} {\n`;
                    for (const member of block.members) { vert_shader += `    ${TypeInfos[member.type].glslName} ${member.name};\n`; }
                    vert_shader += `};\n`;
                }
                if (block.members.some(frag_search)) {
                    frag_shader += `layout(set = ${descriptorSetId}, binding = ${i}) uniform ${block.name} {\n`;
                    for (const member of block.members) { frag_shader += `    ${TypeInfos[member.type].glslName} ${member.name};\n`; }
                    frag_shader += `};\n`;
                }
                i++;
            }
            for (const texture of textures) {
                frag_shader += `layout(set = ${descriptorSetId}, binding = ${i}) `
                    + `uniform ${TypeInfos[texture.type].glslName} ${texture.name};\n`;
                i++;
            }
        }

        let i = 0;
        for (const varying of info.varyings || []) {
            vert_shader += `layout(location = ${i}) out ${TypeInfos[varying.type].glslName} ${varying.name};\n`;
            frag_shader += `layout(location = ${i}) in ${TypeInfos[varying.type].glslName} ${varying.name};\n`;
            i++;
        }

        vert_shader += info.vert.replace(/vert/g, 'main');
        frag_shader += info.frag.replace(/frag/g, 'main');
        return { vert: vert_shader, frag: frag_shader };
    }

    private _shaderGenGlsl3 (info: IProgramInfo) {
        let vert_shader = '';
        let frag_shader = 'precision mediump float;\n';

        for (const attribute of info.attributes || []) {
            vert_shader += `in vec${FormatInfos[attribute.format].count} ${attribute.name};\n`;
        }

        const descriptorSetIds: number[] = this._descriptorSetIds(info.blocks, info.samplerTextures);
        const vert_search = (member: IShaderBlockMember) => info.vert.search(member.name) !== -1;
        const frag_search = (member: IShaderBlockMember) => info.frag.search(member.name) !== -1;

        for (const descriptorSetId of descriptorSetIds) {
            let i = 0;
            const blocks = info.blocks?.filter((block) => block.set === descriptorSetId) || [];
            const textures = info.samplerTextures?.filter((texture) => texture.set === descriptorSetId) || [];

            for (const block of blocks) {
                if (block.members.some(vert_search)) {
                    vert_shader += `layout(std140) uniform ${block.name} {\n`;
                    for (const member of block.members) { vert_shader += `    ${TypeInfos[member.type].glslName} ${member.name};\n`; }
                    vert_shader += `};\n`;
                }
                if (block.members.some(frag_search)) {
                    frag_shader += `layout(std140) uniform ${block.name} {\n`;
                    for (const member of block.members) { frag_shader += `    ${TypeInfos[member.type].glslName} ${member.name};\n`; }
                    frag_shader += `};\n`;
                }
                i++;
            }
            for (const texture of textures) {
                frag_shader += `layout(std140) uniform ${TypeInfos[texture.type].glslName} ${texture.name};\n`;
                i++;
            }
        }

        for (const varying of info.varyings || []) {
            vert_shader += `out ${TypeInfos[varying.type].glslName} ${varying.name};\n`;
            frag_shader += `in ${TypeInfos[varying.type].glslName} ${varying.name};\n`;
        }

        vert_shader += info.vert.replace(/vert/g, 'main');
        frag_shader += info.frag.replace(/frag/g, 'main');
        return { vert: vert_shader, frag: frag_shader };
    }

    private _shaderGenGlsl1 (info: IProgramInfo) {
        let vert_shader = '';
        let frag_shader = 'precision mediump float;\n';

        for (const attribute of info.attributes || []) {
            vert_shader += `attribute vec${FormatInfos[attribute.format].count} ${attribute.name};\n`;
        }

        const descriptorSetIds: number[] = this._descriptorSetIds(info.blocks, info.samplerTextures);
        const vert_search = (member: IShaderBlockMember) => info.vert.search(member.name) !== -1;
        const frag_search = (member: IShaderBlockMember) => info.frag.search(member.name) !== -1;

        for (const descriptorSetId of descriptorSetIds) {
            const blocks = info.blocks?.filter((block) => block.set === descriptorSetId) || [];
            const textures = info.samplerTextures?.filter((texture) => texture.set === descriptorSetId) || [];

            for (const block of blocks) {
                const vert_block_members = block.members.filter(vert_search);
                const frag_block_members = block.members.filter(frag_search);
                for (const member of vert_block_members) { vert_shader += `uniform ${TypeInfos[member.type].glslName} ${member.name};\n`; }
                for (const member of frag_block_members) { frag_shader += `uniform ${TypeInfos[member.type].glslName} ${member.name};\n`; }
            }
            for (const texture of textures) {
                frag_shader += `uniform ${TypeInfos[texture.type].glslName} ${texture.name};\n`;
            }
        }

        for (const varying of info.varyings || []) {
            vert_shader += `varying ${TypeInfos[varying.type].glslName} ${varying.name};\n`;
            frag_shader += `varying ${TypeInfos[varying.type].glslName} ${varying.name};\n`;
        }

        vert_shader += info.vert.replace(/vert/g, 'main');
        frag_shader += info.frag.replace(/frag/g, 'main');
        return { vert: vert_shader, frag: frag_shader };
    }

    private _getAppropriateShaderSource (info: IProgramInfo) {
        switch (TestBase.device.gfxAPI) {
        case API.WEBGL:
        case API.GLES2:
            return this._shaderGenGlsl1(info);
        case API.WEBGL2:
        case API.GLES3:
            return this._shaderGenGlsl3(info);
        case API.METAL:
        case API.VULKAN:
            return this._shaderGenGlsl4(info);
        default: break;
        }
        return this._shaderGenGlsl4(info);
    }

    destroy () {
        this._shader.destroy();
        this._layout.destroy();
    }

    createBindings (info: IProgramBindingInfo) {
        return new ProgramBindings(this, info);
    }

    createInputs (info: IProgramInputInfo) {
        return new ProgramInputs(this, info);
    }
    // related to uniform buffer
    getHandle (name: string, offset = 0, type = Type.UNKNOWN): IProgramHandle {
        let handle = this._bindingMap[name];
        if (!handle) { return 0 as unknown as IProgramHandle; }
        if (type) {
            handle = customizeType(handle, type);
        } else if (offset) {
            handle = customizeType(handle, getTypeFromHandle(handle) - offset);
        }
        handle += offset;
        return handle as unknown as IProgramHandle;
    }

    // update current state settings
    setPipelineState (info: IPipelineStateInfo) {
        // render pass & input states are hard-coded for now
        const hash = hashObject(info);
    }

    draw (commandBuffer: CommandBuffer, bindings: ProgramBindings, inputs: ProgramInputs) {
        const pipelineState = this._pipelineState.get(this._currentPipelineState);
        assert(pipelineState, `no such pipelineState found: ${this._currentPipelineState}`);
        commandBuffer.bindPipelineState(pipelineState);
        const descriptorSetBundle = bindings.descriptorSets[bindings.currentInstance];
        for (let i = 0; i < descriptorSetBundle.length; i++) {
            commandBuffer.bindDescriptorSet(i, descriptorSetBundle[i]);
        }
        commandBuffer.bindInputAssembler(inputs.inputAssembler);
        commandBuffer.draw(inputs.inputAssembler);
    }
}

type ProgramBindingProperties = IMat4Like | IVec4Like | IVec2Like | number | boolean;

type UniformWriter = (a: Int32Array | Float32Array, v: ProgramBindingProperties, idx?: number) => void;

// @ts-expect-error(2740) not all types are relevant here
const type2writer: Record<Type, UniformWriter> = {
    [Type.UNKNOWN]: ((a: Int32Array, v: number, idx = 0): void => { console.warn('illegal uniform handle'); }) as UniformWriter,
    [Type.INT]: ((a: Int32Array, v: number, idx = 0): void => { a[idx] = v; }) as UniformWriter,
    [Type.INT2]: ((a: Int32Array, v: IVec2Like, idx = 0): void => { Vec2.toArray(a, v, idx); }) as UniformWriter,
    [Type.INT4]: ((a: Int32Array, v: IVec4Like, idx = 0): void => { Vec4.toArray(a, v, idx); }) as UniformWriter,
    [Type.FLOAT]: ((a: Float32Array, v: number, idx = 0): void => { a[idx] = v; }) as UniformWriter,
    [Type.FLOAT2]: ((a: Float32Array, v: IVec2Like, idx = 0): void => { Vec2.toArray(a, v, idx); }) as UniformWriter,
    [Type.FLOAT4]: ((a: Float32Array, v: IVec4Like, idx = 0): void => { Vec4.toArray(a, v, idx); }) as UniformWriter,
    [Type.MAT4]: ((a: Float32Array, v: IMat4Like, idx = 0): void => { Mat4.toArray(a, v, idx); }) as UniformWriter,
};

export class ProgramBindings {
    descriptorSets: DescriptorSet[][] = [];
    currentInstance = 0;

    private _rootBuffer: Buffer[];

    private _uniform_buffer: Buffer[][] = [];
    private _sampler_buffer: Buffer[][] = [];

    constructor (program: Program, info: IProgramBindingInfo) {
        // @ts-expect-error(2341) friend class access
        const { _shader: shader, _layout: layout } = program; // destructuring assignment

        const bufferInfo = new BufferInfo(BufferUsageBit.UNIFORM | BufferUsageBit.TRANSFER_DST,
            MemoryUsageBit.DEVICE);
        // bufferInfo.size =

        const instanceCount = info.maxInstanceCount || 1;
        for (let i = 0; i < instanceCount; i++) {
            const descriptorSetBundle: DescriptorSet[] = layout.setLayouts.map(
                (setLayout) => TestBase.device.createDescriptorSet(new DescriptorSetInfo(setLayout)),
            );
            this.descriptorSets.push(descriptorSetBundle);

            const rootBuffer = TestBase.device.createBuffer();
        }
    }

    destroy () {
        for (const descriptorSetBundle of this.descriptorSets) {
            descriptorSetBundle.map((descriptorSet) => descriptorSet.destroy());
        }
    }

    // pass -1 as idx to apply to all applicable fields
    setUniform (handle: IProgramHandle, v: ProgramBindingProperties, instanceIdx = 0, arrayIdx = 0) {
        const handleNum = handle as unknown as number;
        if (instanceIdx === -1) {
            // set for all
            for (const descriptorSetBundle of this.descriptorSets) {
                const setId = getSetFromHandle(handleNum);
                const bindingId = getBindingFromHandle(handleNum);
                const type = getTypeFromHandle(handleNum);
                const offset = getOffsetFromHandle(handleNum);
                const count = getCountFromHandle(handleNum);
                // descriptorSet.bindBuffer(1, v, handle as unknown as number);
            }
        }
    }

    setSampler (handle: IProgramHandle, v: Sampler, arrayIdx = 0) {
    }

    setTexture (handle: IProgramHandle, v: Texture, arrayIdx = 0) {
    }

    update () {
    }
}

// ----------------------------------------------------------------
// not really understand how to use indirect buffer

export class ProgramInputs {
    inputAssembler: InputAssembler;

    constructor (program: Program, info: IProgramInputInfo) {
        // @ts-expect-error(2341) friend class access
        const { _shader: shader } = program; // destructuring assignment

        // set the value of attributes
        const attributes: Attribute[] = shader.attributes;

        // create Vertex buffer
        // create a single vertex buffer for attributes who has the same streamId
        const vertexBuffers: Buffer[] = [];
        const streamIds: number[] = [];
        for (const attribute of attributes) { if (streamIds.indexOf(attribute.stream) === -1) streamIds.push(attribute.stream); }
        for (const streamId of streamIds) {
            let vertexWidth = 0;
            for (const attribute of attributes) {
                if (attribute.stream === streamId) { vertexWidth += FormatInfos[attribute.format].size; }
            }
            vertexBuffers.push(TestBase.device.createBuffer(new BufferInfo(
                BufferUsageBit.VERTEX,
                MemoryUsageBit.DEVICE,
                info.maxVertexCount * vertexWidth,
                vertexWidth,
            )));
        }
        // create index buffer
        const indexBuffer: Buffer = TestBase.device.createBuffer(new BufferInfo(
            BufferUsageBit.INDEX,
            MemoryUsageBit.DEVICE,
            (info.maxIndexCount ? info.maxIndexCount : 0) * (info.indexU32 ? Int32Array.BYTES_PER_ELEMENT : Int16Array.BYTES_PER_ELEMENT) || 0,
            info.indexU32 ? Int32Array.BYTES_PER_ELEMENT : Int16Array.BYTES_PER_ELEMENT,
        ));
        // create indirect buffer
        const indirectBuffer: Buffer = TestBase.device.createBuffer(new BufferInfo(
            BufferUsageBit.INDIRECT,
            MemoryUsageBit.DEVICE,
            info.maxIndirectDrawCount || 1,
        ));
        // create input assembler
        this.inputAssembler = TestBase.device.createInputAssembler(new InputAssemblerInfo(attributes, vertexBuffers, indexBuffer, indirectBuffer));
    }

    destroy () {
        this.inputAssembler.vertexBuffers.map((v) => v.destroy());
        this.inputAssembler.indexBuffer?.destroy();
        this.inputAssembler.indirectBuffer?.destroy();
        this.inputAssembler.destroy();
    }

    updateVertexBuffer (buffer: Float32Array, index = 0) {
        if (index >= 0 && index < this.inputAssembler.vertexBuffers.length) this.inputAssembler.vertexBuffers[index].update(buffer);
    }

    updateIndexBuffer (buffer: Uint16Array | Uint32Array) {
        this.inputAssembler.indexBuffer?.update(buffer);
    }

    updateIndirectBuffer (buffer: IndirectBuffer) {
        assert(this.inputAssembler.indirectBuffer, 'no indirect buffer specified');
        this.inputAssembler.indirectBuffer?.update(buffer);
    }
}
