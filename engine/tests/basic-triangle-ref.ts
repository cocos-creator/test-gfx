import { Attribute, BufferInfo, BufferUsageBit, Color, DescriptorSetInfo, DescriptorSetLayoutBinding,
    DescriptorSetLayoutInfo, DescriptorType, DrawInfo, Format, IndirectBuffer, InputAssemblerInfo, InputState, MemoryUsageBit,
    PipelineLayoutInfo, ShaderInfo, ShaderStage, ShaderStageFlagBit, Type, Uniform, UniformBlock } from 'gfx/base/define';
import { Buffer } from 'gfx/base/buffer';
import { InputAssembler } from 'gfx/base/input-assembler';
import { PipelineState, PipelineStateInfo } from 'gfx/base/pipeline-state';
import { Shader } from 'gfx/base/shader';
import { DescriptorSetLayout } from 'gfx/base/descriptor-set-layout';
import { PipelineLayout } from 'gfx/base/pipeline-layout';
import { DescriptorSet } from 'gfx/base/descriptor-set';
import { IShaderSources, IStandardShaderSource, TestBase } from '../test-base';
import { Mat4 } from '../math';

export class BasicTriangle extends TestBase {
    private _shader: Shader;

    private _vertexBuffer: Buffer;
    private _indexBuffer: Buffer;
    private _indirectBuffer: Buffer;
    private _inputAssembler: InputAssembler;

    private _descriptorSetLayout: DescriptorSetLayout;
    private _descriptorSet: DescriptorSet;
    private _uniformColorBuffer: Buffer;
    private _uniformMVPBuffer: Buffer;

    private _pipelineLayout: PipelineLayout;
    private _pipelineState: PipelineState;

    private _clearColor = new Color(1, 0, 0, 1);
    private _triangleColor = new Float32Array([0, 1, 0, 1]);
    private _mvpBuffer = new Float32Array(16);

    constructor () {
        super();

        const sources: IShaderSources<IStandardShaderSource> = {
            glsl4: {
                vert: `
                    precision highp float;
                    layout(location = 0) in vec2 a_position;
                    layout(set = 0, binding = 1) uniform MVP { mat4 u_mvp; };

                    void main() {
                        gl_Position = u_mvp * vec4(a_position, 0.0, 1.0);
                    }
                `,
                frag: `
                    precision highp float;
                    layout(set = 0, binding = 0) uniform Color {
                        vec4 u_color;
                    };
                    layout(location = 0) out vec4 o_color;

                    void main() {
                        o_color = u_color;
                    }
                `,
            },
            glsl3: {
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
            },
            glsl1: {
                vert: `
                    attribute vec2 a_position;
                    uniform mat4 u_mvp;

                    void main() {
                        gl_Position = u_mvp * vec4(a_position, 0.0, 1.0);
                    }
                `,
                frag: `
                    precision mediump float;
                    uniform vec4 u_color;
                    void main() {
                        gl_FragColor = u_color;
                    }
                `,
            },
        };
        const source = TestBase._getAppropriateShaderSource(sources);
        this._shader = TestBase.device.createShader(new ShaderInfo(
            'Basic Triangle',
            [
                new ShaderStage(ShaderStageFlagBit.VERTEX, source.vert),
                new ShaderStage(ShaderStageFlagBit.FRAGMENT, source.frag),
            ],
            [
                new Attribute('a_position', Format.RG32F, false, 0, false, 0),
            ],
            [
                new UniformBlock(0, 0, 'Color', [new Uniform('u_color', Type.FLOAT4, 1)], 1),
                new UniformBlock(0, 1, 'MVP', [new Uniform('u_mvp', Type.MAT4, 1)], 1),
            ],
        ));

        const vertexData = new Float32Array([
            -0.5,  0.5,
            -0.5, -0.5,
            0.5, -0.5,
            0.0,  0.5,
            0.5,  0.5,
        ]);
        this._vertexBuffer = TestBase.device.createBuffer(new BufferInfo(
            BufferUsageBit.VERTEX,
            MemoryUsageBit.DEVICE,
            vertexData.length * vertexData.BYTES_PER_ELEMENT,
            2 * vertexData.BYTES_PER_ELEMENT,
        ));
        this._vertexBuffer.update(vertexData);

        const indices = new Uint16Array([1, 3, 0, 1, 2, 3, 2, 4, 3]);
        this._indexBuffer = TestBase.device.createBuffer(new BufferInfo(
            BufferUsageBit.INDEX,
            MemoryUsageBit.DEVICE,
            indices.length * indices.BYTES_PER_ELEMENT,
            indices.BYTES_PER_ELEMENT,
        ));
        this._indexBuffer.update(indices);

        const indirectBuffer = new IndirectBuffer([new DrawInfo()]);
        indirectBuffer.drawInfos[0].firstIndex = 3;
        indirectBuffer.drawInfos[0].indexCount = 3;
        this._indirectBuffer = TestBase.device.createBuffer(new BufferInfo(
            BufferUsageBit.INDIRECT,
            MemoryUsageBit.DEVICE,
            1,
        ));
        this._indirectBuffer.update(indirectBuffer);

        this._inputAssembler = TestBase.device.createInputAssembler(new InputAssemblerInfo(
            this._shader.attributes,
            [this._vertexBuffer],
            this._indexBuffer,
            this._indirectBuffer,
        ));

        this._descriptorSetLayout = TestBase.device.createDescriptorSetLayout(new DescriptorSetLayoutInfo([
            new DescriptorSetLayoutBinding(0, DescriptorType.UNIFORM_BUFFER, 1, ShaderStageFlagBit.FRAGMENT),
            new DescriptorSetLayoutBinding(1, DescriptorType.UNIFORM_BUFFER, 1, ShaderStageFlagBit.VERTEX),
        ]));
        this._descriptorSet = TestBase.device.createDescriptorSet(new DescriptorSetInfo(this._descriptorSetLayout));

        this._uniformColorBuffer = TestBase.device.createBuffer(new BufferInfo(
            BufferUsageBit.UNIFORM,
            MemoryUsageBit.DEVICE | MemoryUsageBit.HOST,
            4 * Float32Array.BYTES_PER_ELEMENT,
        ));

        this._uniformMVPBuffer = TestBase.device.createBuffer(new BufferInfo(
            BufferUsageBit.UNIFORM,
            MemoryUsageBit.DEVICE | MemoryUsageBit.HOST,
            16 * Float32Array.BYTES_PER_ELEMENT,
        ));

        this._descriptorSet.bindBuffer(0, this._uniformColorBuffer);
        this._descriptorSet.bindBuffer(1, this._uniformMVPBuffer);
        this._descriptorSet.update();

        this._pipelineLayout = TestBase.device.createPipelineLayout(new PipelineLayoutInfo([this._descriptorSetLayout]));
        this._pipelineState = TestBase.device.createPipelineState(new PipelineStateInfo(
            this._shader,
            this._pipelineLayout,
            TestBase.defaultRenderPass,
            new InputState(this._shader.attributes),
        ));
    }

    public onTick () {
        this._triangleColor[0] = Math.abs(Math.sin(TestBase.cumulativeTime));
        this._uniformColorBuffer.update(this._triangleColor);

        Mat4.toArray(this._mvpBuffer, TestBase._getOrthographicMat4(-1, 1, -1, 1, -1, 1));
        this._uniformMVPBuffer.update(this._mvpBuffer);

        TestBase._acquire();
        TestBase._beginOnscreenPass(this._clearColor);

        TestBase.commandBuffers[0].bindPipelineState(this._pipelineState);
        TestBase.commandBuffers[0].bindDescriptorSet(0, this._descriptorSet);
        TestBase.commandBuffers[0].bindInputAssembler(this._inputAssembler);
        TestBase.commandBuffers[0].draw(this._inputAssembler);

        TestBase._endOnscreenPass();
        TestBase._present();
    }

    public onDestroy () {
        this._shader.destroy();
        this._vertexBuffer.destroy();
        this._indexBuffer.destroy();
        this._indirectBuffer.destroy();
        this._inputAssembler.destroy();
        this._descriptorSetLayout.destroy();
        this._descriptorSet.destroy();
        this._uniformColorBuffer.destroy();
        this._uniformMVPBuffer.destroy();
        this._pipelineLayout.destroy();
        this._pipelineState.destroy();
    }
}
