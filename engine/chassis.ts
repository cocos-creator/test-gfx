import { CommandBuffer } from 'gfx/base/command-buffer';
import { Buffer } from 'gfx/base/buffer';
import {
    Attribute, BufferInfo, BufferUsageBit, DescriptorSetLayoutBinding, PipelineLayoutInfo,
    DescriptorType, Format, IndirectBuffer, InputAssemblerInfo, InputState, MemoryUsageBit,
    PrimitiveMode, RenderPassInfo, ShaderInfo, ShaderStage,
    ShaderStageFlagBit, Type, Uniform, UniformBlock, DescriptorSetLayoutInfo, DescriptorSetInfo, FormatInfos,
} from 'gfx/base/define';
import { DescriptorSet } from 'gfx/base/descriptor-set';
import { DescriptorSetLayout } from 'gfx/base/descriptor-set-layout';
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
    // private _pipelineState: PipelineState; // hash map

    constructor (info: IProgramInfo) {
        const shaders = {
            glsl4: {
                vert: '',
                frag: '',
            },
            glsl3: {
                vert: '',
                frag: '',
            },
        };
        console.log('print shaders', shaders);
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
            )) || [],
            info.blocks?.map((block, index) => new UniformBlock(0,
                index,
                block.name,
                block.members.map((member) => new Uniform(member.name, member.type, member.count)),
                1)) || [],
        ));

        this._layout = TestBase.device.createPipelineLayout(new PipelineLayoutInfo([
            TestBase.device.createDescriptorSetLayout(new DescriptorSetLayoutInfo(
                info.blocks?.map((block, index) => new DescriptorSetLayoutBinding(
                    index,
                    DescriptorType.UNIFORM_BUFFER,
                    block.members.length,
                    ShaderStageFlagBit.VERTEX,
                )) || [],
            )),
        ]));
        // this._pipelineState = null!;

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
        // create renderPass
        // this._pipelineState = TestBase.device.createPipelineState(new PipelineStateInfo(
        //     this._shader,
        //     this._layout,
        //     TestBase.defaultRenderPass,
        //     new InputState(this._shader.attributes),
        // ));
    }

    draw (commandBuffer: CommandBuffer, bindings: ProgramBindings, inputs: ProgramInputs) {
        // commandBuffer.bindPipelineState(this._pipelineState);
        // commandBuffer.bindDescriptorSet(0, bindings.descriptorSets[bindings.currentInstance]);
        // commandBuffer.bindInputAssembler(inputs.inputAssembler);
        // commandBuffer.draw(inputs.inputAssembler);
    }
}

type ProgramBindingProperties = IMat4Like | IVec4Like | IVec2Like | number;

export class ProgramBindings {
    descriptorSets: DescriptorSet[] = [];
    currentInstance = 0;

    constructor (program: Program, info: IProgramBindingInfo) {
        // @ts-expect-error(2341) friend class access
        const { _layout: layout } = program; // destructuring assignment

        const instanceCount = info.maxInstanceCount || 1;

        for (let i = 0; i < instanceCount; i++) {
            this.descriptorSets.push(TestBase.device.createDescriptorSet(
                new DescriptorSetInfo(), // program.getHandle() layout.setLayout
            ));
        }
    }

    destroy () {
        this.descriptorSets.map((descriptorSet) => descriptorSet.destroy());
    }

    // pass -1 as idx to apply to all applicable fields
    setUniform (handle: IProgramHandle, v: ProgramBindingProperties, instanceIdx = 0, arrayIdx = 0) {
        if (instanceIdx === -1) {
            // implementation
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
// not really understand the usage of indirectBuffer

export class ProgramInputs {
    inputAssembler: InputAssembler;

    constructor (program: Program, info: IProgramInputInfo) {
        // @ts-expect-error(2341) friend class access
        const { _shader: shader } = program; // destructuring assignment

        const attributes: Attribute[] = shader.attributes;
        let vertexWidth = 0;
        for (let i = 0; i < attributes.length; i++) {
            vertexWidth += FormatInfos[attributes[i].format].size;
        }
        this.inputAssembler = TestBase.device.createInputAssembler(new InputAssemblerInfo(
            attributes,
            //! length of single vertex
            [TestBase.device.createBuffer(new BufferInfo(
                BufferUsageBit.VERTEX,
                MemoryUsageBit.DEVICE,
                info.maxVertexCount * vertexWidth,
                vertexWidth,
            ))],
            // attributes.map((attr) => TestBase.device.createBuffer(new BufferInfo(
            //     BufferUsageBit.VERTEX,
            //     MemoryUsageBit.DEVICE,
            //     info.maxVertexCount * 10, // length
            //     2,
            // ))),
            TestBase.device.createBuffer(new BufferInfo(
                BufferUsageBit.INDEX,
                MemoryUsageBit.DEVICE,
                (info.maxIndexCount ? info.maxIndexCount : 0) * (info.indexU32 ? Int32Array.BYTES_PER_ELEMENT : Int16Array.BYTES_PER_ELEMENT) || 0,
                info.indexU32 ? Int32Array.BYTES_PER_ELEMENT : Int16Array.BYTES_PER_ELEMENT,
            )),
            TestBase.device.createBuffer(new BufferInfo(
                BufferUsageBit.INDIRECT,
                MemoryUsageBit.DEVICE,
                info.maxIndirectDrawCount || 1,
            )),
        ));
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
        this.inputAssembler.indirectBuffer?.update(buffer);
    }
}
