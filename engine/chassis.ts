import { CommandBuffer } from 'gfx/base/command-buffer';
import { Buffer } from 'gfx/base/buffer';
import {
    Attribute, BufferInfo, BufferUsageBit, DescriptorSetLayoutBinding, PipelineLayoutInfo,
    DescriptorType, Format, IndirectBuffer, InputAssemblerInfo, InputState, MemoryUsageBit,
    PrimitiveMode, RenderPassInfo, ShaderInfo, ShaderStage,
    ShaderStageFlagBit, Type, Uniform, UniformBlock, DescriptorSetLayoutInfo, DescriptorSetInfo, FormatInfos, API,
} from 'gfx/base/define';
import { DescriptorSet } from 'gfx/base/descriptor-set';
import { DescriptorSetLayout } from 'gfx/base/descriptor-set-layout';
import { InputAssembler } from 'gfx/base/input-assembler';
import { PipelineLayout } from 'gfx/base/pipeline-layout';
import { BlendState, DepthStencilState, PipelineState, PipelineStateInfo, RasterizerState } from 'gfx/base/pipeline-state';
import { Shader } from 'gfx/base/shader';
import { Sampler } from 'gfx/base/states/sampler';
import { Texture } from 'gfx/base/texture';
import { assert } from 'platform/debug';
import { IMat4Like, IVec2Like, IVec4Like } from './math';
import { IShaderSources, IStandardShaderSource, TestBase } from './test-base';

export class TypeInfo {
    declare private _token: never; // to make sure all usages must be an instance of this exact class, not assembled from plain object

    constructor(
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

export class Program {
    private _shader: Shader;
    private _layout: PipelineLayout;
    private _pipelineState: PipelineState; // hash map

    constructor (info: IProgramInfo) {
        // const shaderSources: IStandardShaderSource = this._getAppropriateShaderSource(info);
        const shaderSources: IStandardShaderSource = {
            vert: `
                    in vec2 a_position;
                    layout(std140) uniform MVP { mat4 u_mvp; };

                    void main() {
                        gl_Position = u_mvp * vec4(a_position, 0.0, 1.0);
                    }
                `,
            frag: `
                    precision mediump float;
                    layout(std140) uniform Color {
                        vec4 u_color;
                    };

                    out vec4 o_color;
                    void main() {
                        o_color = u_color;
                    }
                `,
        };

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
        const shaderBlocks = info.blocks?.map((block, index) => new UniformBlock(0,
            index,
            block.name,
            block.members.map((member) => new Uniform(member.name, member.type, member.count)),
            1)) || [];

        this._shader = TestBase.device.createShader(new ShaderInfo(shaderName, shaderStages, shaderAttributes, shaderBlocks));

        // create bindings
        // the number of layout should be the same as the required sets by one frame.
        // There can be more than one descriptorSet for each instance/pass
        const descriptorSetIds = this._descriptorSetIds(info.blocks, info.samplerTextures);
        const descriptorSetLayouts: DescriptorSetLayout[] = [];

        for (const descriptorSetId of descriptorSetIds) {
            const bindings: DescriptorSetLayoutBinding[] = [];
            for (const block of info.blocks || []) {
                if (block.set === descriptorSetId) {
                    for (const member of block.members) {
                        bindings.push(new DescriptorSetLayoutBinding(
                            bindings.length,
                            DescriptorType.UNIFORM_BUFFER,
                            member.count,
                            info.vert.search(member.name) === -1
                                ? ShaderStageFlagBit.FRAGMENT : ShaderStageFlagBit.VERTEX,
                        ));
                    }
                }
            }

            for (const samplerTexture of info.samplerTextures || []) {
                if (samplerTexture.set === descriptorSetId) {
                    bindings.push(new DescriptorSetLayoutBinding(bindings.length, DescriptorType.SAMPLER_TEXTURE, 1));
                }
            }
            descriptorSetLayouts.push(TestBase.device.createDescriptorSetLayout(new DescriptorSetLayoutInfo(bindings)));
        }

        this._layout = TestBase.device.createPipelineLayout(new PipelineLayoutInfo(descriptorSetLayouts));
        this._pipelineState = null!;

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
        // shader's header
        let vert_shader = 'precision highp float;\n';
        let frag_shader = 'precision highp float;\n';

        // shader's vertex buffer input
        //! the old version which also considered the stream binding
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

        // shader's uniform input
        const descriptorSets: number[] = this._descriptorSetIds(info.blocks, info.samplerTextures);
        const vert_search = (member: IShaderBlockMember) => info.vert.search(member.name) !== -1;
        const frag_search = (member: IShaderBlockMember) => info.frag.search(member.name) !== -1;

        for (const descriptorSet of descriptorSets) {
            let i = 0;
            const blocks = info.blocks?.filter((block) => block.set === descriptorSet) || [];
            const textures = info.samplerTextures?.filter((texture) => texture.set === descriptorSet) || [];

            for (const block of blocks) {
                if (block.members.some(vert_search)) {
                    vert_shader += `layout(set = ${descriptorSet}, binding = ${i}) uniform ${block.name} {\n`;
                    for (const member of block.members) { vert_shader += `    ${TypeInfos[member.type].glslName} ${member.name};\n`; }
                    vert_shader += `};\n`;
                }
                if (block.members.some(frag_search)) {
                    frag_shader += `layout(set = ${descriptorSet}, binding = ${i}) uniform ${block.name} {\n`;
                    for (const member of block.members) { frag_shader += `    ${TypeInfos[member.type].glslName} ${member.name};\n`; }
                    frag_shader += `};\n`;
                }
                i++;
            }
            for (const texture of textures) {
                frag_shader += `layout(set = ${descriptorSet}, binding = ${i}) `
                    + `uniform ${TypeInfos[texture.type].glslName} ${texture.name};\n`;
                i++;
            }
        }

        // shader's varying input&output
        let i = 0;
        for (const varying of info.varyings || []) {
            vert_shader += `layout(location = ${i}) out ${TypeInfos[varying.type].glslName} ${varying.name};\n`;
            frag_shader += `layout(location = ${i}) in ${TypeInfos[varying.type].glslName} ${varying.name};\n`;
            i++;
        }

        // shader's functions and eof
        vert_shader += info.vert.replace(/vert/g, 'main');
        frag_shader += info.frag.replace(/frag/g, 'main');

        return { vert: vert_shader, frag: frag_shader };
    }

    private _shaderGenGlsl3 (info: IProgramInfo) {
        return { vert: 'vert_shader', frag: 'frag_shader' };
    }

    private _shaderGenGlsl1 (info: IProgramInfo) {
        return { vert: 'vert_shader', frag: 'frag_shader' };
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
        const handle = 0;
        return handle as unknown as IProgramHandle;
    }

    // update current state settings
    setPipelineState (info: IPipelineStateInfo) {
        // create renderPass

        this._pipelineState = TestBase.device.createPipelineState(new PipelineStateInfo(
            this._shader,
            this._layout,
            TestBase.defaultRenderPass,
            new InputState(this._shader.attributes),
            info.rasterizerState,
            info.depthStencilState,
            info.blendState,
            info.primitive,
        ));
    }

    draw (commandBuffer: CommandBuffer, bindings: ProgramBindings, inputs: ProgramInputs) {
        commandBuffer.bindPipelineState(this._pipelineState);
        const descriptorSetBundle = bindings.descriptorSets[bindings.currentInstance];
        for (let i = 0; i < descriptorSetBundle.length; i++) {
            commandBuffer.bindDescriptorSet(i, descriptorSetBundle[i]);
        }
        commandBuffer.bindInputAssembler(inputs.inputAssembler);
        commandBuffer.draw(inputs.inputAssembler);
    }
}

type ProgramBindingProperties = IMat4Like | IVec4Like | IVec2Like | number;

export class ProgramBindings {
    descriptorSets: DescriptorSet[][] = [];
    currentInstance = 0;

    private _uniform_buffer: Buffer[] = [];
    private _sampler_buffer: Buffer[] = [];

    constructor (program: Program, info: IProgramBindingInfo) {
        // @ts-expect-error(2341) friend class access
        const { _layout: layout } = program; // destructuring assignment

        const instanceCount = info.maxInstanceCount || 1;
        for (let i = 0; i < instanceCount; i++) {
            const descriptorSetBundle: DescriptorSet[] = layout.setLayouts.map(
                (setLayout) => TestBase.device.createDescriptorSet(new DescriptorSetInfo(setLayout)),
            );
            this.descriptorSets.push(descriptorSetBundle);
        }
    }

    destroy () {
        for (const descriptorSetBundle of this.descriptorSets) {
            descriptorSetBundle.map((descriptorSet) => descriptorSet.destroy());
        }
    }

    // pass -1 as idx to apply to all applicable fields
    setUniform (handle: IProgramHandle, v: ProgramBindingProperties, instanceIdx = 0, arrayIdx = 0) {
        if (instanceIdx === -1) {
            // set for all
            for (const descriptorSetBundle of this.descriptorSets) {
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
// not really understand the usage of indirectBuffer

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
