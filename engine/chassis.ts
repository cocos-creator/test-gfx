import { CommandBuffer } from 'gfx/base/command-buffer';
import { Buffer } from 'gfx/base/buffer';
import {
    Attribute, BufferInfo, BufferUsageBit, DescriptorSetLayoutBinding, PipelineLayoutInfo,
    DescriptorType, Format, IndirectBuffer, InputAssemblerInfo, InputState, MemoryUsageBit,
    PrimitiveMode, ShaderInfo, ShaderStage, ShaderStageFlagBit, Type,
    Uniform, UniformBlock, DescriptorSetLayoutInfo, DescriptorSetInfo, FormatInfos,
    API, GetTypeSize, UniformSamplerTexture, BufferViewInfo,
} from 'gfx/base/define';
import { DescriptorSet } from 'gfx/base/descriptor-set';
import { DescriptorSetLayout } from 'gfx/base/descriptor-set-layout';
import { InputAssembler } from 'gfx/base/input-assembler';
import { PipelineLayout } from 'gfx/base/pipeline-layout';
import { BlendState, DepthStencilState, PipelineState, PipelineStateInfo, RasterizerState } from 'gfx/base/pipeline-state';
import { Shader } from 'gfx/base/shader';
import { Sampler } from 'gfx/base/states/sampler';
import { Texture } from 'gfx/base/texture';
import { RenderPass } from 'gfx/base/render-pass';
import { IMat4Like, IVec2Like, IVec3Like, IVec4Like, Mat4, Vec2, Vec3, Vec4 } from './math';
import { IStandardShaderSource, TestBase } from './test-base';

import { SourceGenerator } from './generator';

export class TypeInfo {
    declare private _token: never;

    constructor (
        public readonly name: string = '',
        public readonly glslName: string = '',
        public readonly size: number = 0,
        public readonly type: Type = Type.UNKNOWN,
    ) { }
}

export const TypeInfos = Object.freeze([
    new TypeInfo('UNKNOWN', '', 0, Type.UNKNOWN),

    new TypeInfo('BOOL', 'bool', 4, Type.BOOL),
    new TypeInfo('BOOL2', 'bvec2', 8, Type.BOOL2),
    new TypeInfo('BOOL3', 'bvec3', 12, Type.BOOL3),
    new TypeInfo('BOOL4', 'bvec4', 16, Type.BOOL),

    new TypeInfo('INT', 'int', 4, Type.INT),
    new TypeInfo('INT2', 'ivec2', 8, Type.INT2),
    new TypeInfo('INT3', 'ivec3', 12, Type.INT3),
    new TypeInfo('INT4', 'ivec4', 16, Type.INT4),

    new TypeInfo('UINT', 'uint', 4, Type.UINT),
    new TypeInfo('UINT2', 'uvec2', 8, Type.UINT2),
    new TypeInfo('UINT3', 'uvec3', 12, Type.UINT3),
    new TypeInfo('UINT4', 'uvec4', 16, Type.UINT4),

    new TypeInfo('FLOAT', 'float', 4, Type.FLOAT),
    new TypeInfo('FLOAT2', 'vec2', 8, Type.FLOAT2),
    new TypeInfo('FLOAT3', 'vec3', 12, Type.FLOAT3),
    new TypeInfo('FLOAT4', 'vec4', 16, Type.FLOAT4),

    new TypeInfo('MAT2', 'mat2', 16, Type.MAT2),
    new TypeInfo('MAT2X3', 'mat2x3', 24, Type.MAT2X3),
    new TypeInfo('MAT2X4', 'mat2x4', 32, Type.MAT2X4),

    new TypeInfo('MAT3X2', 'mat3x2', 24, Type.MAT3X2),
    new TypeInfo('MAT3', 'mat3', 36, Type.MAT3),
    new TypeInfo('MAT3X4', 'mat3x4', 48, Type.MAT3X4),

    new TypeInfo('MAT4X2', 'mat4x2', 32, Type.MAT4X2),
    new TypeInfo('MAT4X3', 'mat4x3', 48, Type.MAT4X3),
    new TypeInfo('MAT4', 'mat4', 64, Type.MAT4),

    new TypeInfo('SAMPLER1D', 'sampler1D', 4, Type.SAMPLER1D),
    new TypeInfo('SAMPLER1D_ARRAY', 'sampler1DArray', 4, Type.SAMPLER1D_ARRAY),
    new TypeInfo('SAMPLER2D', 'sampler2D', 4, Type.SAMPLER2D),
    new TypeInfo('SAMPLER2D_ARRAY', 'sampler2DArray', 4, Type.SAMPLER2D_ARRAY),

    new TypeInfo('SAMPLER3D', 'sampler3D', 4, Type.SAMPLER3D),
    new TypeInfo('SAMPLER_CUBE', 'samplerCubeArray', 4, Type.SAMPLER_CUBE),

    new TypeInfo('SAMPLER', 'sampler', 0, Type.SAMPLER),

    new TypeInfo('TEXTURE1D', 'texture1D', 0, Type.TEXTURE1D),
    new TypeInfo('TEXTURE1D_ARRAY', 'texture1DArray', 0, Type.TEXTURE1D_ARRAY),
    new TypeInfo('TEXTURE2D', 'texture2D', 0, Type.TEXTURE2D),
    new TypeInfo('TEXTURE2D_ARRAY', 'texture2DArray', 0, Type.TEXTURE2D_ARRAY),
    new TypeInfo('TEXTURE3D', 'texture3D', 0, Type.TEXTURE3D),
    new TypeInfo('TEXTURE_CUBE', 'textureCube', 0, Type.TEXTURE_CUBE),

    new TypeInfo('IMAGE1D', 'image1D', 0, Type.IMAGE1D),
    new TypeInfo('IMAGE1D_ARRAY', 'image1DArray', 0, Type.IMAGE1D_ARRAY),
    new TypeInfo('IMAGE2D', 'image2D', 0, Type.IMAGE2D),
    new TypeInfo('IMAGE2D_ARRAY', 'image2DArray', 0, Type.IMAGE2D_ARRAY),
    new TypeInfo('IMAGE3D', 'image3D', 0, Type.IMAGE3D),
    new TypeInfo('IMAGE_CUBE', 'imageCube', 0, Type.IMAGE_CUBE),

    // // input attachment
    // SUBPASS_INPUT,
    // COUNT,
]);

export enum IShaderExtensionType {
    ENABLE,
    REQUIRE,
    WARN,
    DISABLE,
}

export const IShaderExtensionTypeName = [
    'enable',
    'require',
    'warn',
    'disable',
];

export interface IShaderExtension {
    name: string;
    type: IShaderExtensionType;
}

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
    extensions?: IShaderExtension[];           // default to empty array
    attributes?: IShaderAttribute[];           // default to empty array
    blocks?: IShaderBlock[];                   // default to empty array
    samplerTextures?: IShaderSamplerTexture[]; // default to empty array
    varyings?: IShaderVaryings[];              // default to empty array
    attachments?: number;                      // default to 1, number of render targets, up to 4 supported
    defaultStates?: IPipelineStateInfo;        // default to identity values
}

export interface IProgramBindingInfo {
    maxInstanceCount?: Record<string, number>; // default to 1
}

export interface IProgramInputInfo {
    maxVertexCount: number;
    maxIndexCount?: number;              // default to 0
    maxIndirectDrawCount?: number;       // default to 0
    vertexFrequentlyUpdated?: number;    // default to 0 (bitwise mask)
    indexFrequentlyUpdated?: boolean;    // default to false
    indirectFrequentlyUpdated?: boolean; // default to false
    indexU32?: boolean;                  // default to false
}

export interface IPipelineStateInfo {
    rasterizerState?: RasterizerState;     // default to default value
    depthStencilState?: DepthStencilState; // default to default value
    renderPass?: RenderPass;               // default to default value
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
const offsetMask = 0x000003ff; // 10 bits => 256 vector

export const genHandle = (set: number, binding: number, type: Type, count: number, offset = 0): number => ((set << 30) & setMask)
    | ((binding << 24) & bindingMask) | ((type << 18) & typeMask) | (count << 10) & countMask | (offset & offsetMask);
export const getSetFromHandle = (handle: number): number => (handle & setMask) >>> 30;
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

export const TEST_UBO_COUNTS: number[] = [5, 3, 0];
export const TEST_SAMPLER_COUNTS: number[] = [5, 3, 0];

export class Program {
    private _shader: Shader;
    private _inputState: InputState;
    private _layout: PipelineLayout;
    private _propertyMap: Record<string, number>;
    private _pipelineStateMap: Map<number, PipelineState>; // hash map
    private _currentPipelineState: PipelineState;

    constructor (info: IProgramInfo) {
        // create bindings
        // the number of layout should be the same as the required sets.
        // There can be more than one descriptorSet for each instance/pass
        const descriptorSetLayouts: DescriptorSetLayout[] = [];
        this._propertyMap = {};

        const shaderAttributes: Attribute[] = [];
        const samplerTextures: UniformSamplerTexture[] = [];
        const shaderBlocks: UniformBlock[] = [];

        const descriptorSetIds: number[] = this._descriptorSetIds(info.blocks || [], info.samplerTextures || []);
        TestBase.assert(descriptorSetIds.length < 4, 'too many descriptor set for one draw call, you can use 3 at most.');

        const layoutBindings: Record<number, DescriptorSetLayoutBinding[]> = descriptorSetIds.map((id) => []);
        const reg = '\\b';

        for (const block of info.blocks || []) {
            const bindings = layoutBindings[block.set];
            let shaderStageFlag = ShaderStageFlagBit.NONE;
            const descriptorType = DescriptorType.UNIFORM_BUFFER
                | (block.dynamic ? DescriptorType.DYNAMIC_UNIFORM_BUFFER : DescriptorType.UNKNOWN);

            const blockUniforms: Uniform[] = [];

            for (const member of block.members) {
                const memberRegex = new RegExp(`${reg}${member.name}${reg}`, `g`);
                let exist = false;

                if (info.vert.search(memberRegex) !== -1) {
                    shaderStageFlag |= ShaderStageFlagBit.VERTEX;
                    exist = true;
                }
                if (info.frag.search(memberRegex) !== -1) {
                    shaderStageFlag |= ShaderStageFlagBit.FRAGMENT;
                    exist = true;
                }
                if (exist) {
                    blockUniforms.push(new Uniform(member.name, member.type, member.count));
                }
            }

            if (blockUniforms.length > 0) {
                shaderBlocks.push(new UniformBlock(
                    block.set,
                    bindings.length,
                    block.name,
                    blockUniforms,
                    1,
                ));
                bindings.push(new DescriptorSetLayoutBinding(
                    bindings.length,
                    descriptorType,
                    1,
                    shaderStageFlag,
                ));
            }
        }

        for (const samplerTexture of info.samplerTextures || []) {
            const bindings = layoutBindings[samplerTexture.set];
            const regex = new RegExp(`${reg}${samplerTexture.name}${reg}`, `g`);
            let shaderStageFlag = ShaderStageFlagBit.NONE;
            if (info.frag.search(regex) !== -1) shaderStageFlag |= ShaderStageFlagBit.FRAGMENT;
            if (info.vert.search(regex) !== -1) shaderStageFlag |= ShaderStageFlagBit.VERTEX;

            if (shaderStageFlag === ShaderStageFlagBit.NONE) continue;
            samplerTextures.push(new UniformSamplerTexture(
                samplerTexture.set,
                bindings.length,
                samplerTexture.name,
                samplerTexture.type,
                samplerTexture.count || 1,
            ));
            // immutableSampler?
            bindings.push(new DescriptorSetLayoutBinding(
                bindings.length,
                DescriptorType.SAMPLER_TEXTURE,
                1,
                shaderStageFlag,
            ));
        }

        for (const block of shaderBlocks || []) {
            let offset = 0;
            this._propertyMap[block.name] = genHandle(block.set, block.binding, Type.UNKNOWN, 0, 0);
            for (const member of block.members) {
                this._propertyMap[member.name] = genHandle(
                    block.set,
                    block.binding,
                    member.type,
                    member.count || 1,
                    offset,
                );
                TestBase.assert(GetTypeSize(member.type) >= GetTypeSize(Type.UINT4) || (member.count || 1) <= 1,
                    `the width of ${TypeInfos[member.type].glslName} will be like vec4 in glsl std140, don't use array`);
                // convert unit to int/float/uint
                offset += (TypeInfos[member.type].size >> 2) * (member.count || 1);
            }
        }

        for (const samplerTexture of samplerTextures) {
            this._propertyMap[samplerTexture.name] = genHandle(
                samplerTexture.set,
                samplerTexture.binding,
                samplerTexture.type,
                samplerTexture.count || 1,
            );
        }

        for (const descriptorSetId of descriptorSetIds) {
            const blocks = shaderBlocks?.filter((block) => block.set === descriptorSetId) || [];
            const textures = samplerTextures?.filter((texture) => texture.set === descriptorSetId) || [];

            const SAMPLER_FLEXIBLE_LIM = TestBase.device.capabilities.maxTextureUnits - TEST_SAMPLER_COUNTS[0] - TEST_SAMPLER_COUNTS[1];
            const UBO_FLEXIBLE_LIM = TestBase.device.capabilities.maxUniformBufferBindings - TEST_UBO_COUNTS[0] - TEST_UBO_COUNTS[1];

            TestBase.assert(blocks.length < TEST_UBO_COUNTS[descriptorSetId] || TEST_UBO_COUNTS[descriptorSetId] === -1,
                `too many UBOs in set ${descriptorSetId}, only ${TEST_UBO_COUNTS[descriptorSetId]} slots exist.`);
            TestBase.assert(blocks.length < UBO_FLEXIBLE_LIM || TEST_UBO_COUNTS[descriptorSetId] !== -1,
                `too many UBOs in set ${descriptorSetId}, only ${UBO_FLEXIBLE_LIM} slots exist.`);

            TestBase.assert(textures.length < TEST_SAMPLER_COUNTS[descriptorSetId] || TEST_SAMPLER_COUNTS[descriptorSetId] === -1,
                `too many Textures in set ${descriptorSetId}, only ${TEST_SAMPLER_COUNTS[descriptorSetId]} slots exist.`);
            TestBase.assert(textures.length < SAMPLER_FLEXIBLE_LIM || TEST_SAMPLER_COUNTS[descriptorSetId] !== -1,
                `too many Textures in set ${descriptorSetId}, only ${SAMPLER_FLEXIBLE_LIM} slots exist.`);

            descriptorSetLayouts.push(TestBase.device.createDescriptorSetLayout(
                new DescriptorSetLayoutInfo(layoutBindings[descriptorSetId]),
            ));
        }

        // set vertex buffer attributes
        let attributeLocations = 0;
        for (const attribute of info.attributes || []) {
            const regex = new RegExp(`${reg}${attribute.name}${reg}`, `g`);
            if (!info.vert.search(regex)) continue;
            shaderAttributes.push(new Attribute(
                attribute.name,
                attribute.format,
                attribute.isNormalized,
                attribute.stream || 0,
                attribute.isInstanced,
                attributeLocations++,
            ));
        }

        // generate shader sources
        const generator = new SourceGenerator(TestBase.device.gfxAPI);
        const shaderSources: IStandardShaderSource = generator.genShaderSource(
            info.name,
            info.vert, info.frag,
            info.extensions || [],
            shaderAttributes,
            shaderBlocks,
            samplerTextures,
            info.varyings || [],
            info.attachments || 1,
            descriptorSetLayouts,
        );

        const shaderName = info.name;

        // set render stages
        const shaderStages = [
            new ShaderStage(ShaderStageFlagBit.VERTEX, shaderSources.vert),
            new ShaderStage(ShaderStageFlagBit.FRAGMENT, shaderSources.frag),
        ];

        this._shader = TestBase.device.createShader(new ShaderInfo(
            shaderName,
            shaderStages,
            shaderAttributes,
            shaderBlocks,
            [],
            samplerTextures,
        ));

        this._inputState = new InputState(shaderAttributes);
        this._layout = TestBase.device.createPipelineLayout(new PipelineLayoutInfo(descriptorSetLayouts));
        this._pipelineStateMap = new Map<number, PipelineState>();
        this._currentPipelineState = null!;
        this.setPipelineState(info.defaultStates || {});
    }

    private _descriptorSetIds (blocks: IShaderBlock[] = [],
        samplerTextures: IShaderSamplerTexture[] = []): number[] {
        const descriptorSetIds: number[] = [];
        for (const block of blocks || []) { if (descriptorSetIds.indexOf(block.set) === -1) descriptorSetIds.push(block.set); }
        for (const texture of samplerTextures || []) { if (descriptorSetIds.indexOf(texture.set) === -1) descriptorSetIds.push(texture.set); }
        return descriptorSetIds.sort((a, b) => a - b);
    }

    destroy () {
        this._pipelineStateMap.forEach((state, key) => state.destroy());
        this._layout.setLayouts.forEach((setLayout) => setLayout.destroy());

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
    getHandle (name: string, offset = 0, type = Type.UNKNOWN, arrayIdx = 0): IProgramHandle {
        let handle = this._propertyMap[name];
        TestBase.assert(handle !== undefined, `${name} is not a valid name for uniform resources`);
        const memberType = getTypeFromHandle(handle);
        if (type) {
            handle = customizeType(handle, type);
        } else if (offset) {
            handle = customizeType(handle, getTypeFromHandle(handle) - offset);
        }
        if (arrayIdx > 0) {
            handle += arrayIdx * (TypeInfos[memberType].size / 4);
        }
        handle += offset;
        return handle as unknown as IProgramHandle;
    }

    // update current state settings
    setPipelineState (info: IPipelineStateInfo) {
        const stateHash = hashObject(info);

        if (!this._pipelineStateMap.has(stateHash)) {
            this._pipelineStateMap.set(stateHash, TestBase.device.createPipelineState(new PipelineStateInfo(
                this._shader,
                this._layout,
                info.renderPass || TestBase.defaultRenderPass,
                this._inputState,
                info.rasterizerState,
                info.depthStencilState,
                info.blendState,
                info.primitive,
            )));
        }
        this._currentPipelineState = this._pipelineStateMap.get(stateHash)!;
    }

    draw (commandBuffer: CommandBuffer, bindings: ProgramBindings, inputs: ProgramInputs) {
        TestBase.assert(!!this._currentPipelineState, `no pipelineState specified`);
        commandBuffer.bindPipelineState(this._currentPipelineState);
        const offsets = bindings.instanceOffsets;

        for (let i = 0; i < bindings.descriptorSets.length; i++) {
            if (offsets[i].length > 0) {
                commandBuffer.bindDescriptorSet(i, bindings.descriptorSets[i], offsets[i]);
            } else {
                commandBuffer.bindDescriptorSet(i, bindings.descriptorSets[i]);
            }
        }

        commandBuffer.bindInputAssembler(inputs.inputAssembler);
        commandBuffer.draw(inputs.inputAssembler);
    }
}

type ProgramBindingProperties = IMat4Like | IVec4Like | IVec3Like | IVec2Like | number | boolean;

type UniformWriter = (a: Int32Array | Float32Array, v: ProgramBindingProperties, idx?: number) => void;

const type2writer: Record<number, UniformWriter> = {
    [Type.UNKNOWN]: ((a: Int32Array, v: number, idx = 0): void => { console.warn('illegal uniform handle'); }) as UniformWriter,
    [Type.INT]: ((a: Int32Array, v: number, idx = 0): void => { a[idx] = v; }) as UniformWriter,
    [Type.INT2]: ((a: Int32Array, v: IVec2Like, idx = 0): void => { Vec2.toArray(a, v, idx); }) as UniformWriter,
    [Type.INT3]: ((a: Int32Array, v: IVec3Like, idx = 0): void => { Vec3.toArray(a, v, idx); }) as UniformWriter,
    [Type.INT4]: ((a: Int32Array, v: IVec4Like, idx = 0): void => { Vec4.toArray(a, v, idx); }) as UniformWriter,
    [Type.FLOAT]: ((a: Float32Array, v: number, idx = 0): void => { a[idx] = v; }) as UniformWriter,
    [Type.FLOAT2]: ((a: Float32Array, v: IVec2Like, idx = 0): void => { Vec2.toArray(a, v, idx); }) as UniformWriter,
    [Type.FLOAT3]: ((a: Float32Array, v: IVec3Like, idx = 0): void => { Vec3.toArray(a, v, idx); }) as UniformWriter,
    [Type.FLOAT4]: ((a: Float32Array, v: IVec4Like, idx = 0): void => { Vec4.toArray(a, v, idx); }) as UniformWriter,
    [Type.MAT4]: ((a: Float32Array, v: IMat4Like, idx = 0): void => { Mat4.toArray(a, v, idx); }) as UniformWriter,
};

export class ProgramBindings {
    descriptorSets: DescriptorSet[] = [];

    private _instanceOffsets: Record<number, number[]> = [];
    private _dynamicOffsets: Record<number, number[]> = []; // default to -1
    private _maxInstanceCount: Record<number, number[]> = [];
    private _dynamicBindings: Record<number, number[]> = [];

    private _rootBuffers: Buffer[] = [];
    private _rootBlocks: ArrayBuffer[] = [];
    private _rootBufferDirties: boolean[] = [];
    private _blocks: Record<number, Float32Array[]> = [];
    private _blocksInt: Record<number, Int32Array[]> = [];
    private _blockSizes: Record<number, number[]> = [];
    private _uniformBuffers: Record<number, Buffer[]> = [];

    constructor (program: Program, info: IProgramBindingInfo) {
        // @ts-expect-error(2341) friend class access
        const { _shader: shader, _layout: layout } = program; // destructuring assignment
        const maxInstanceCount: Record<string, number> = info.maxInstanceCount || {};

        // create descriptor sets
        this.descriptorSets = layout.setLayouts.map(
            (setLayout) => TestBase.device.createDescriptorSet(new DescriptorSetInfo(setLayout)),
        );
        const dynamicOffsets: Record<number, number[]> = [];
        for (let i = 0; i < layout.setLayouts.length; i++) {
            this._instanceOffsets[i] = [];
            this._dynamicOffsets[i] = [];
            this._maxInstanceCount[i] = [];
            this._dynamicBindings[i] = [];
            this._blocks[i] = [];
            this._blocksInt[i] = [];
            this._blockSizes[i] = [];
            this._uniformBuffers[i] = [];
        }

        // calculate total size
        const startOffsets: number[][] = [];
        const alignment = TestBase.device.capabilities.uboOffsetAlignment;
        let instanceCount = 0;

        for (let set = 0; set < this.descriptorSets.length; set++) {
            startOffsets.push([]);
            let lastOffset = 0; let lastSize = 0; let dynamicBinding = 0;
            for (const block of shader.blocks) {
                if (block.set !== set) continue;
                const binding = block.binding;
                const isDynamic = layout.setLayouts[set].bindings[binding].descriptorType & DescriptorType.DYNAMIC_UNIFORM_BUFFER;
                startOffsets[set][binding] = (lastOffset);
                lastSize = block.members.reduce((pv, cv) => (pv + TypeInfos[cv.type].size) * (cv.count || 1), 0);
                lastSize = Math.ceil(lastSize / alignment) * alignment;
                this._blockSizes[set].push(lastSize);

                instanceCount = maxInstanceCount[block.name];
                instanceCount = instanceCount !== undefined ? instanceCount : 1;
                this._maxInstanceCount[set][binding] = instanceCount;

                if (isDynamic) {
                    this._dynamicOffsets[set].push(lastSize);
                    this._instanceOffsets[set].push(0);
                    this._dynamicBindings[set][binding] = dynamicBinding;
                    dynamicBinding += 1;

                    lastSize *= instanceCount;
                } else {
                    this._dynamicBindings[set][binding] = -1;
                }
                lastOffset += lastSize;
            }
            // create gfx buffer
            const rootSize = lastOffset;
            const bufferInfo = new BufferInfo(
                BufferUsageBit.UNIFORM | BufferUsageBit.TRANSFER_DST,
                MemoryUsageBit.DEVICE,
            );
            bufferInfo.size = Math.ceil(rootSize / 16) * 16;
            this._rootBuffers.push(TestBase.device.createBuffer(bufferInfo));
            this._rootBlocks.push(new ArrayBuffer(rootSize));
        }

        // create buffer views
        for (let i = 0; i < shader.blocks.length; i++) {
            const set = shader.blocks[i].set;
            const binding = shader.blocks[i].binding;
            const isDynamic = layout.setLayouts[set].bindings[binding].descriptorType & DescriptorType.DYNAMIC_UNIFORM_BUFFER;

            let size = this._blockSizes[set][binding];
            if (isDynamic) size *= this._maxInstanceCount[set][binding];
            const bufferViewInfo = new BufferViewInfo(
                this._rootBuffers[set],
                startOffsets[set][binding],
                size,
            );
            const bufferView = this._uniformBuffers[set][binding] = TestBase.device.createBuffer(bufferViewInfo);

            this._blocks[set][binding] = new Float32Array(
                this._rootBlocks[set],
                bufferViewInfo.offset,
                size / Float32Array.BYTES_PER_ELEMENT,
            );
            this._blocksInt[set][binding] = new Int32Array(
                this._blocks[set][binding].buffer,
                this._blocks[set][binding].byteOffset,
                this._blocks[set][binding].length,
            );

            this._blockSizes[set][binding] /= Float32Array.BYTES_PER_ELEMENT;
            this.descriptorSets[set].bindBuffer(binding, bufferView);
        }
    }

    destroy () {
        const length = this.descriptorSets.length;
        for (let i = 0; i < length; i++) {
            this.descriptorSets[i].destroy();
            this._uniformBuffers[i].map((buffer) => buffer.destroy());
            this._rootBuffers[i].destroy();
        }
    }

    // pass -1 as idx to apply to all applicable fields
    setUniform (handle: IProgramHandle, v: ProgramBindingProperties, instanceIdx = 0): void {
        const handleNum = handle as unknown as number;
        const set = getSetFromHandle(handleNum);
        const binding = getBindingFromHandle(handleNum);
        const maxInstanceCount = this._maxInstanceCount[set][binding];
        if (instanceIdx === -1) {
            for (let i = 0; i < maxInstanceCount; i++) {
                this.setUniform(handle, v, i);
            }
            return;
        }
        TestBase.assert(instanceIdx < maxInstanceCount && instanceIdx > -1, 'instanceIdx is out of range');
        const type = getTypeFromHandle(handleNum);
        const blockWidth = this._blockSizes[set][binding];

        const offset = (instanceIdx * blockWidth) // instance offset
            + getOffsetFromHandle(handleNum); // value offset

        let block: Float32Array | Int32Array = null!;
        if (type < Type.FLOAT) block = this._blocksInt[set][binding];
        else block = this._blocks[set][binding];

        TestBase.assert(block.length > offset, 'memory error');
        type2writer[type](block, v, offset);

        this._rootBufferDirties[set] = true;
    }

    setUniformArray (handle: IProgramHandle, v: ProgramBindingProperties[], instanceIdx = 0): void {
        const handleNum = handle as unknown as number;
        const set = getSetFromHandle(handleNum);
        const binding = getBindingFromHandle(handleNum);
        const maxInstanceCount = this._maxInstanceCount[set][binding];
        if (instanceIdx === -1) {
            for (let i = 0; i < maxInstanceCount; i++) {
                this.setUniformArray(handle, v, i);
            }
            return;
        }
        TestBase.assert(instanceIdx < maxInstanceCount && instanceIdx > -1, 'instanceIdx is out of range');
        const type = getTypeFromHandle(handleNum);
        const stride = TypeInfos[type].size;
        const blockWidth = this._blockSizes[set][binding];
        let offset = (instanceIdx * blockWidth) // instance offset
            + getOffsetFromHandle(handleNum); // value offset

        let block: Float32Array | Int32Array = null!;
        if (type < Type.FLOAT) block = this._blocksInt[set][binding];
        else block = this._blocks[set][binding];

        // write data
        for (let i = 0; i < v.length; i++, offset += stride) {
            if (v[i] === null) continue;
            type2writer[type](block, v[i], offset);
        }
        this._rootBufferDirties[set] = true;
    }

    setSampler (handle: IProgramHandle, v: Sampler, arrayIdx = 0): void {
        const handleNum = handle as unknown as number;
        const set = getSetFromHandle(handleNum);
        const binding = getBindingFromHandle(handleNum);
        const type = getTypeFromHandle(handleNum);
        const offset = TypeInfos[type].size * arrayIdx;
        this.descriptorSets[set].bindSampler(binding, v, offset);
    }

    setTexture (handle: IProgramHandle, v: Texture, arrayIdx = 0): void {
        const handleNum = handle as unknown as number;
        const set = getSetFromHandle(handleNum);
        const binding = getBindingFromHandle(handleNum);
        const type = getTypeFromHandle(handleNum);
        const offset = TypeInfos[type].size * arrayIdx;
        this.descriptorSets[set].bindTexture(binding, v, offset);
    }

    setBufferInstance (handle: IProgramHandle, instanceIdx = 0): void {
        const handleNum = handle as unknown as number;
        const set = getSetFromHandle(handleNum);
        const binding = getBindingFromHandle(handleNum);
        const dynamicBinding = this._dynamicBindings[set][binding];
        TestBase.assert(dynamicBinding !== -1, 'block is not dynamic');
        const maxInstanceCount = this._maxInstanceCount[set][binding];
        TestBase.assert(instanceIdx < maxInstanceCount && instanceIdx > -1, 'instanceIdx is out of range');
        this._instanceOffsets[set][dynamicBinding] = this._dynamicOffsets[set][dynamicBinding] * instanceIdx;
    }

    get instanceOffsets () {
        return this._instanceOffsets;
    }

    update () {
        TestBase.assert(this.descriptorSets.length !== 0, 'at least one descriptor set is required');

        for (let i = 0; i < this.descriptorSets.length; i++) {
            TestBase.assert(!!this._rootBlocks[i], '_rootBlock is not valid');
            TestBase.assert(!!this._rootBuffers[i], '_rootBuffer is not valid');
            if (this._rootBufferDirties[i] && this._rootBuffers[i]) {
                this._rootBuffers[i].update(this._rootBlocks[i]);
                this._rootBufferDirties[i] = false;
            }
            this.descriptorSets[i].update();
        }
    }
}

export class ProgramInputs {
    inputAssembler: InputAssembler;

    constructor (program: Program, info: IProgramInputInfo) {
        // @ts-expect-error(2341) friend class access
        const { _shader: shader } = program; // destructuring assignment

        // set the value of attributes
        const attributes: Attribute[] = shader.attributes;

        TestBase.assert(info.maxVertexCount > 0, 'failed to create inputAssembler, no vertex data provided');

        // create Vertex buffer
        const vertexBuffers: Buffer[] = [];
        const streamIds: number[] = [];
        let indexBuffer: Buffer | undefined = null!;
        let indirectBuffer: Buffer | undefined = null!;
        for (const attribute of attributes) {
            const id = attribute.stream || 0;
            if (streamIds.indexOf(id) === -1) streamIds.push(id);
        }
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
        if ((info.maxIndexCount || 0) > 0) {
            indexBuffer = TestBase.device.createBuffer(new BufferInfo(
                BufferUsageBit.INDEX,
                MemoryUsageBit.DEVICE,
                (info.maxIndexCount ? info.maxIndexCount : 0) * (info.indexU32 ? Int32Array.BYTES_PER_ELEMENT : Int16Array.BYTES_PER_ELEMENT) || 0,
                info.indexU32 ? Int32Array.BYTES_PER_ELEMENT : Int16Array.BYTES_PER_ELEMENT,
            ));
        }
        // create indirect buffer
        if ((info.maxIndirectDrawCount || 0) > 0) {
            indirectBuffer = TestBase.device.createBuffer(new BufferInfo(
                BufferUsageBit.INDIRECT,
                MemoryUsageBit.DEVICE,
                info.maxIndirectDrawCount,
            ));
        }
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
        TestBase.assert(index >= 0 && index < this.inputAssembler.vertexBuffers.length, 'failed to upload data to vertexBuffer, index out of range');
        this.inputAssembler.vertexBuffers[index].update(buffer);
    }

    updateIndexBuffer (buffer: Uint16Array | Uint32Array) {
        TestBase.assert(!!this.inputAssembler.indexBuffer, 'no index buffer specified');
        this.inputAssembler.indexBuffer?.update(buffer);
    }

    updateIndirectBuffer (buffer: IndirectBuffer) {
        TestBase.assert(!!this.inputAssembler.indirectBuffer, 'no indirect buffer specified');
        this.inputAssembler.indirectBuffer?.update(buffer);
    }
}
