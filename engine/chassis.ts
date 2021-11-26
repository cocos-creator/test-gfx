import { CommandBuffer } from 'gfx/base/command-buffer';
import {
    Attribute, BufferInfo, BufferUsageBit, Format, IndirectBuffer, InputAssemblerInfo, InputState, MemoryUsageBit, PrimitiveMode, RenderPassInfo, ShaderInfo, ShaderStage,
    ShaderStageFlagBit, Type, Uniform, UniformBlock,
} from 'gfx/base/define';
import { DescriptorSet } from 'gfx/base/descriptor-set';
import { InputAssembler } from 'gfx/base/input-assembler';
import { PipelineLayout } from 'gfx/base/pipeline-layout';
import { BlendState, DepthStencilState, PipelineState, PipelineStateInfo, RasterizerState } from 'gfx/base/pipeline-state';
import { Shader } from 'gfx/base/shader';
import { Sampler } from 'gfx/base/states/sampler';
import { Texture } from 'gfx/base/texture';
import { IMat4Like, IVec2Like, IVec4Like } from './math';
import { TestBase } from './test-base';

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
    private _pipelineState: PipelineState;

    constructor (info: IProgramInfo) {
        this._shader = TestBase.device.createShader(new ShaderInfo(
            info.name,
            [
                new ShaderStage(ShaderStageFlagBit.VERTEX, info.vert),
                new ShaderStage(ShaderStageFlagBit.FRAGMENT, info.frag),
            ],
            info.attributes?.map((attribute, index) => new Attribute(
                attribute.name,
                attribute.format,
                attribute.isNormalized,
                attribute.stream,
                attribute.isInstanced,
                index,
            )),
            info.blocks?.map((block, index) => new UniformBlock(0,
                index,
                block.name,
                block.members.map((member) => new Uniform(member.name, member.type, member.count)),
                1)),
        ));
        this._layout = null!;
        this.setPipelineState(info.defaultStates || {});
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

    getHandle (name: string, offset = 0, type = Type.UNKNOWN): IProgramHandle {
        const handle = 0;
        return handle as unknown as IProgramHandle;
    }

    // update current state settings
    setPipelineState (info: IPipelineStateInfo) {
        // render pass & input states are hard-coded for now
        this._pipelineState = TestBase.device.createPipelineState(new PipelineStateInfo(
            this._shader,
            this._layout,
            TestBase.defaultRenderPass,
            new InputState(this._shader.attributes),
        ));
    }

    draw(commandBuffer: CommandBuffer, bindings: ProgramBindings, inputs: ProgramInputs) {
        commandBuffer.bindPipelineState(this.);
        commandBuffer.bindDescriptorSet(bindings.descriptorSets[bindings.currentInstance]);
        commandBuffer.bindInputAssembler(inputs.inputAssembler);
        commandBuffer.draw(inputs.inputAssembler);
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
        this.inputAssembler = TestBase.device.createInputAssembler(new InputAssemblerInfo(
            [],
            [],
            TestBase.device.createBuffer(new BufferInfo(
                BufferUsageBit.INDEX,
                MemoryUsageBit.DEVICE,
            )),
            TestBase.device.createBuffer(new BufferInfo(
                BufferUsageBit.INDIRECT,
                MemoryUsageBit.DEVICE,
                1,
            )),
        ));
    }

    destroy() {
        for (let i = 0; ; i++) {
            if (this.inputAssembler.getVertexBuffer(i) !== null)
                this.inputAssembler.getVertexBuffer(i).destroy();
        }
        inputAssembler.destroy();
    }

    updateVertexBuffer(buffer: Float32Array, index = 0) {
        this.inputAssembler.getVertexBuffer(0).update(buffer);
    }

    updateIndexBuffer(buffer: Uint16Array | Uint32Array) {
        this.inputAssembler.indexBuffer.update(buffer);
    }

    updateIndirectBuffer(buffer: IndirectBuffer) {
        this.inputAssembler.indirectBuffer.update(buffer);
    }
}
