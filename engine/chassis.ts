import { CommandBuffer } from 'gfx/base/command-buffer';
import { Format, IndirectBuffer, PrimitiveMode, Type } from 'gfx/base/define';
import { DescriptorSet } from 'gfx/base/descriptor-set';
import { InputAssembler } from 'gfx/base/input-assembler';
import { PipelineLayout } from 'gfx/base/pipeline-layout';
import { BlendState, DepthStencilState, RasterizerState } from 'gfx/base/pipeline-state';
import { Shader } from 'gfx/base/shader';
import { Sampler } from 'gfx/base/states/sampler';
import { Texture } from 'gfx/base/texture';
import { IMat4Like, IVec2Like, IVec4Like } from './math';

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
    dynamic?: boolean; // default to false
}

export interface IShaderSamplerTexture {
    name: string;
    type: Type;
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

export class Program {
    private _shader: Shader;
    private _layout: PipelineLayout;

    constructor (info: IProgramInfo) {
        this._shader = null!;
        this._layout = null!;
        this.setPipelineState(info.defaultStates || {});
    }

    destroy () {
    }

    createBindings (info: IProgramBindingInfo) {
        return new ProgramBindings(this, info);
    }

    createInputs (info: IProgramInputInfo) {
        return new ProgramInputs(this, info);
    }

    getHandle (name: string, offset = 0, type = Type.UNKNOWN): IProgramHandle {
        const handle = 0;
        return handle as unknown as IProgramHandle;
    }

    // update current state settings
    setPipelineState (info: IPipelineStateInfo) {
        // render pass & input states are hard-coded for now
    }

    draw (commandBuffer: CommandBuffer, bindings: ProgramBindings, inputs: ProgramInputs) {
    }
}

type ProgramBindingProperties = IMat4Like | IVec4Like | IVec2Like | number;

export class ProgramBindings {
    descriptorSets: DescriptorSet[] = [];
    currentInstance = 0;

    constructor (program: Program, info: IProgramBindingInfo) {
    }

    destroy () {
    }

    // pass -1 as idx to apply to all applicable fields
    setUniform (handle: IProgramHandle, v: ProgramBindingProperties, instanceIdx = 0, arrayIdx = 0) {
    }

    setSampler (handle: IProgramHandle, v: Sampler, arrayIdx = 0) {
    }

    setTexture (handle: IProgramHandle, v: Texture, arrayIdx = 0) {
    }

    update () {
    }
}

export class ProgramInputs {
    inputAssembler: InputAssembler;

    constructor (program: Program, info: IProgramInputInfo) {
        this.inputAssembler = null!;
    }

    destroy () {
    }

    updateVertexBuffer (buffer: Float32Array, index = 0) {
    }

    updateIndexBuffer (buffer: Uint16Array | Uint32Array) {
    }

    updateIndirectBuffer (buffer: IndirectBuffer) {
    }
}
