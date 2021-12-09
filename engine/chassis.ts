import { CommandBuffer } from 'gfx/base/command-buffer';
import { Buffer } from 'gfx/base/buffer';
import {
    Attribute, BufferInfo, BufferUsageBit, DescriptorSetLayoutBinding, PipelineLayoutInfo,
    DescriptorType, Format, IndirectBuffer, InputAssemblerInfo, InputState, MemoryUsageBit,
    PrimitiveMode, RenderPassInfo, ShaderInfo, ShaderStage, ShaderStageFlagBit, Type,
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
import { assert, errorID } from 'platform/debug';
import { RenderPass } from 'gfx/base/render-pass';
import { IMat4Like, IVec2Like, IVec4Like, Mat4, Vec2, Vec4 } from './math';
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

    new TypeInfo('TEXTURE1D', 'texture1d', 0, Type.TEXTURE1D),
    new TypeInfo('TEXTURE1D_ARRAY', 'texture1dArray', 0, Type.TEXTURE1D_ARRAY),
    new TypeInfo('TEXTURE2D', 'texture2d', 0, Type.TEXTURE2D),
    new TypeInfo('TEXTURE2D_ARRAY', 'texture2dArray', 0, Type.TEXTURE2D_ARRAY),
    new TypeInfo('TEXTURE3D', 'texture3d', 0, Type.TEXTURE3D),
    new TypeInfo('TEXTURE_CUBE', 'textureCube', 0, Type.TEXTURE_CUBE),

    new TypeInfo('IMAGE1D', 'image1d', 0, Type.IMAGE1D),
    new TypeInfo('IMAGE1D_ARRAY', 'image1dArray', 0, Type.IMAGE1D_ARRAY),
    new TypeInfo('IMAGE2D', 'image2d', 0, Type.IMAGE2D),
    new TypeInfo('IMAGE2D_ARRAY', 'image2dArray', 0, Type.IMAGE2D_ARRAY),
    new TypeInfo('IMAGE3D', 'image3d', 0, Type.IMAGE3D),
    new TypeInfo('IMAGE_CUBE', 'imageCube', 0, Type.IMAGE_CUBE),

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
    attachments?: number;                      // default to 1, number of render targets, up to 4 supported
    defaultStates?: IPipelineStateInfo;        // default to identity values
}

export interface IProgramBindingInfo {
    maxInstanceCount?: number; // default to 1
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

export class Program {
    private _shader: Shader;
    private _layout: PipelineLayout;
    private _propertyMap: Record<string, number>;
    private _pipelineState: Map<number, PipelineState>; // hash map
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

        const streamIds: number[] = this._bufferStreamIds(info.attributes || []);
        const descriptorSetIds: number[] = this._descriptorSetIds(info.blocks || [], info.samplerTextures || []);

        // creates layout bindings and fill bindingMap.
        for (const descriptorSetId of descriptorSetIds) {
            let i = 0;
            const bindings: DescriptorSetLayoutBinding[] = [];
            const blocks = info.blocks?.filter((block) => block.set === descriptorSetId) || [];
            const textures = info.samplerTextures?.filter((texture) => texture.set === descriptorSetId) || [];

            for (const block of blocks) {
                let shaderStageFlag = ShaderStageFlagBit.NONE;
                let offset = 0;
                const descriptorType = DescriptorType.UNIFORM_BUFFER
                    | (block.dynamic ? DescriptorType.DYNAMIC_UNIFORM_BUFFER : DescriptorType.UNKNOWN);
                const blockUniforms: Uniform[] = [];

                for (const member of block.members) {
                    const memberRegex = new RegExp(`( |\t)${member.name}( |;|,|\n)`, `g`);
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
                        this._propertyMap[member.name] = genHandle(descriptorSetId, bindings.length, member.type, member.count || 1, offset);
                        // no bit shift needed
                        if ((GetTypeSize(member.type) < GetTypeSize(Type.UINT4)) && (member.count || 1) > 1) {
                            TestBase.assert(false,
                                `the width of ${TypeInfos[member.type].glslName} will be like vec4 in glsl std140, don't use array`);
                        }
                        offset += (GetTypeSize(member.type) >> 2) * (member.count || 1);
                        blockUniforms.push(new Uniform(member.name, member.type, member.count));
                    }
                }

                if (shaderStageFlag !== ShaderStageFlagBit.NONE) {
                    bindings.push(new DescriptorSetLayoutBinding(
                        bindings.length,
                        descriptorType,
                        1,
                        shaderStageFlag,
                    ));
                    shaderBlocks.push(new UniformBlock(
                        block.set,
                        i++,
                        block.name,
                        blockUniforms,
                        1,
                    ));
                }
            }
            for (const samplerTexture of textures) {
                if (samplerTexture.set === descriptorSetId) {
                    const regex = new RegExp(`( |\t)${samplerTexture.name}( |;|,|\n)`, `g`);
                    if (info.frag.search(regex)) {
                        this._propertyMap[samplerTexture.name] = genHandle(descriptorSetId,
                            bindings.length, samplerTexture.type, samplerTexture.count || 1);
                        // offset ?????
                        bindings.push(new DescriptorSetLayoutBinding(bindings.length, DescriptorType.SAMPLER_TEXTURE, 1));
                        samplerTextures.push(new UniformSamplerTexture(
                            samplerTexture.set,
                            i++,
                            samplerTexture.name,
                            samplerTexture.type,
                            samplerTexture.count || 1,
                        ));
                    }
                }
            }
            descriptorSetLayouts.push(TestBase.device.createDescriptorSetLayout(new DescriptorSetLayoutInfo(bindings)));
        }

        // set vertex buffer attributes
        for (const streamId of streamIds) {
            let i = 0;
            for (const attribute of info.attributes || []) {
                const regex = new RegExp(`( |\t)${attribute.name}( |;|,|\n)`, `g`);
                if (info.vert.search(regex) && (attribute.stream || 0) === streamId) {
                    shaderAttributes.push(new Attribute(
                        attribute.name,
                        attribute.format,
                        attribute.isNormalized,
                        attribute.stream,
                        attribute.isInstanced,
                        i++,
                    ));
                }
            }
        }

        // generate shader sources
        const generator = new SourceGenerator(TestBase.device.gfxAPI);
        const shaderSources: IStandardShaderSource = generator.genShaderSource(
            shaderAttributes,
            shaderBlocks,
            samplerTextures,
            info.varyings || [],
            info.vert, info.frag,
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

        this._layout = TestBase.device.createPipelineLayout(new PipelineLayoutInfo(descriptorSetLayouts));
        this._pipelineState = new Map<number, PipelineState>();
        this._currentPipelineState = null!;
        this.setPipelineState(info.defaultStates || {});
    }

    private _bufferStreamIds (attributes: IShaderAttribute[] = []): number[] {
        const streamIds: number[] = [];
        let id = 0;
        for (const attribute of attributes) {
            id = attribute.stream || 0;
            if (streamIds.indexOf(id) === -1) streamIds.push(id);
        }
        return streamIds.sort((a, b) => a - b);
    }

    private _descriptorSetIds (blocks: IShaderBlock[] = [], samplerTextures: IShaderSamplerTexture[] = []): number[] {
        const descriptorSetIds: number[] = [];
        for (const block of blocks || []) { if (descriptorSetIds.indexOf(block.set) === -1) descriptorSetIds.push(block.set); }
        for (const texture of samplerTextures || []) { if (descriptorSetIds.indexOf(texture.set) === -1) descriptorSetIds.push(texture.set); }
        return descriptorSetIds.sort((a, b) => a - b);
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
    getHandle (name: string, offset = 0, type = Type.UNKNOWN, arrayIdx = 0): IProgramHandle {
        let handle = this._propertyMap[name];
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
        const stateHash = hashObject(info);

        if (!this._pipelineState.has(stateHash)) {
            this._currentPipelineState = TestBase.device.createPipelineState(new PipelineStateInfo(
                this._shader,
                this._layout,
                TestBase.defaultRenderPass,
                new InputState(this._shader.attributes),
                info.rasterizerState,
                info.depthStencilState,
                info.blendState,
                info.primitive,
            ));
            this._pipelineState.set(stateHash, this._currentPipelineState);
        }
    }

    draw (commandBuffer: CommandBuffer, bindings: ProgramBindings, inputs: ProgramInputs) {
        TestBase.assert(!!this._currentPipelineState, `no pipelineState specified`);
        commandBuffer.bindPipelineState(this._currentPipelineState);
        for (let i = 0; i < bindings.descriptorSets.length; i++) {
            commandBuffer.bindDescriptorSet(i, bindings.descriptorSets[i]);
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
    descriptorSets: DescriptorSet[] = [];
    currentInstance = 0;
    maxInstanceCount = 0;

    private _rootBuffer: Buffer;
    private _rootBlock: ArrayBuffer;
    private _rootBufferDirty = false;

    private _uniformBuffers: Buffer[] = [];
    private _blocks: Float32Array[] = [];
    private _blockSizes: number[] = [];
    private _bindings: Record<number, number[]> = [];

    private _samplerBuffers: Buffer[] = [];

    constructor (program: Program, info: IProgramBindingInfo) {
        // @ts-expect-error(2341) friend class access
        const { _shader: shader, _layout: layout } = program; // destructuring assignment
        this.maxInstanceCount = info.maxInstanceCount || 1;

        // create descriptor sets
        this.descriptorSets = layout.setLayouts.map(
            (setLayout) => TestBase.device.createDescriptorSet(new DescriptorSetInfo(setLayout)),
        );
        for (let i = 0; i < layout.setLayouts.length; i++) {
            this._bindings[i] = [];
        }

        // calculate total size
        const startOffsets: number[] = [];
        const alignment = TestBase.device.capabilities.uboOffsetAlignment;
        let lastOffset = 0; let lastSize = 0;
        for (const block of shader.blocks) {
            startOffsets.push(lastOffset);
            lastSize = block.members.reduce((pv, cv) => (pv + TypeInfos[cv.type].size) * (cv.count || 1), 0);
            lastSize *= (layout.setLayouts[block.set].bindings[block.binding].descriptorType & DescriptorType.DYNAMIC_UNIFORM_BUFFER)
                ? this.maxInstanceCount : 1;
            this._blockSizes.push(lastSize);
            lastOffset += Math.ceil(lastSize / alignment) * alignment;
        }
        // create gfx buffer
        const rootSize = startOffsets[shader.blocks.length - 1] + lastSize;
        const bufferInfo = new BufferInfo(
            BufferUsageBit.UNIFORM | BufferUsageBit.TRANSFER_DST,
            MemoryUsageBit.DEVICE,
        );
        bufferInfo.size = Math.ceil(rootSize / 16) * 16;
        this._rootBuffer = TestBase.device.createBuffer(bufferInfo);
        this._rootBlock = new ArrayBuffer(rootSize);

        // create buffer views
        const bufferViewInfo = new BufferViewInfo();
        for (let i = 0; i < shader.blocks.length; i++) {
            const size = this._blockSizes[i];
            const block = shader.blocks[i];
            bufferViewInfo.buffer = this._rootBuffer;
            bufferViewInfo.offset = startOffsets[i];
            bufferViewInfo.range = Math.ceil(size / 16) * 16;
            const bufferView = this._uniformBuffers[i] = TestBase.device.createBuffer(bufferViewInfo);
            this._blocks.push(new Float32Array(this._rootBlock, bufferViewInfo.offset,
                size / Float32Array.BYTES_PER_ELEMENT));

            this._bindings[block.set].push(i);
            this.descriptorSets[block.set].bindBuffer(block.binding, bufferView);
        }
    }

    destroy () {
        this.descriptorSets.map((descriptorSet) => descriptorSet.destroy());

        this._uniformBuffers.map((buffer) => buffer.destroy());

        this._samplerBuffers.map((samplerBuffer) => samplerBuffer.destroy());

        this._rootBuffer.destroy();
    }

    // pass -1 as idx to apply to all applicable fields
    setUniform (handle: IProgramHandle, v: ProgramBindingProperties, instanceIdx = 0) {
        const handleNum = handle as unknown as number;
        const setId = getSetFromHandle(handleNum);
        const bindingId = getBindingFromHandle(handleNum);
        const type = getTypeFromHandle(handleNum);
        const idx = this._bindings[setId][bindingId];
        const blockWidth = this._blockSizes[idx];
        const count = getCountFromHandle(handleNum);
        const offset = instanceIdx * blockWidth // instance offset
                    + getOffsetFromHandle(handleNum); // value offset

        const block = this._blocks[idx];
        // write data
        // @ts-expect-error(7053) restrict type check
        type2writer[type](block, v, offset);
        this._rootBufferDirty = true;
    }

    setUniformArray (handle: IProgramHandle, v: ProgramBindingProperties[], instanceIdx = 0) {
        const handleNum = handle as unknown as number;
        const setId = getSetFromHandle(handleNum);
        const bindingId = getBindingFromHandle(handleNum);
        const type = getTypeFromHandle(handleNum);
        const stride = TypeInfos[type].size;
        const idx = this._bindings[setId][bindingId];
        const blockWidth = this._blockSizes[idx];
        let offset = instanceIdx * blockWidth // instance offset
            + getOffsetFromHandle(handleNum); // value offset
        // write data
        const block = this._blocks[idx];
        for (let i = 0; i < v.length; i++, offset += stride) {
            if (v[i] === null) continue;
            // @ts-expect-error(7053) restrict type check
            type2writer[type](block, v, offset);
        }
        this._rootBufferDirty = true;
    }

    setSampler (handle: IProgramHandle, v: Sampler, arrayIdx = 0) {
        this._rootBufferDirty = true;
    }

    setTexture (handle: IProgramHandle, v: Texture, arrayIdx = 0) {
        this._rootBufferDirty = true;
    }

    nextInstance (currentInstance: number): number {
        const nextInstance = currentInstance + 1;
        return nextInstance === this.maxInstanceCount ? 0 : nextInstance;
    }

    update () {
        TestBase.assert(this.descriptorSets.length !== 0, 'at least one descriptor set is required');
        TestBase.assert(!!this._rootBlock, '_rootBlock is not valid');
        TestBase.assert(!!this._rootBuffer, '_rootBuffer is not valid');
        if (this._rootBufferDirty && this._rootBuffer) {
            this._rootBuffer.update(this._rootBlock);
            this._rootBufferDirty = false;
        }
        for (const descriptorSet of this.descriptorSets) {
            descriptorSet.update();
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

        // create Vertex buffer
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
        TestBase.assert(!!this.inputAssembler.indirectBuffer, 'no indirect buffer specified');
        this.inputAssembler.indirectBuffer?.update(buffer);
    }
}
