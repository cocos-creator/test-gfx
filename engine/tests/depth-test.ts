import {
    AccessFlagBit, Attribute, BufferTextureCopy, Color, CullMode, DepthStencilAttachment, DrawInfo, Filter,
    Format, FormatFeatureBit, FramebufferInfo, GeneralBarrierInfo, IndirectBuffer, PolygonMode, PrimitiveMode, Rect,
    RenderPassInfo, SampleCount, SamplerInfo, ShadeModel, ShaderStageFlagBit, TextureFlagBit, TextureInfo,
    TextureType, TextureUsageBit, TextureViewInfo, Type,
} from 'gfx/base/define';
import { Device } from 'gfx/base/device';
import { RasterizerState } from 'gfx/base/pipeline-state';
import { Texture } from 'gfx/base/texture';
import { Sampler } from 'gfx/base/states/sampler';
import { RenderPass } from 'gfx/base/render-pass';
import { Framebuffer } from 'gfx/base/framebuffer';
import { Swapchain } from 'gfx/base/swapchain';
import { CommandBuffer } from 'gfx/base/command-buffer';
import { box } from '../primitive/box';
import { NULL_HANDLE, Program, ProgramBindings, ProgramInputs, IShaderExtension, IShaderExtensionType } from '../chassis';
import { TestBase } from '../test-base';
import { Vec4, Vec3, Mat4, IVec3Like, IVec4Like } from '../math';
import { GeneralBarrier } from 'gfx/base/states/general-barrier';

class DepthFrameBuffer {
    private _renderPass: RenderPass = null!;
    private _depthTexture: Texture = null!;
    private _depthTextureView: Texture = null!;
    private _framebuffer: Framebuffer = null!;

    private _lodLevel = 2;
    private _area: Rect = null!;

    constructor (device: Device, swapchain: Swapchain) {
        this._area = new Rect();
        const depthTextureInfo = new TextureInfo(
            TextureType.TEX2D,
            TextureUsageBit.DEPTH_STENCIL_ATTACHMENT | TextureUsageBit.SAMPLED,
            Format.DEPTH,
            swapchain.width, swapchain.height, TextureFlagBit.NONE, 1, Math.ceil(Math.log(Math.max(swapchain.width, swapchain.height))),
        );

        if (!(device.getFormatFeatures(Format.DEPTH) & FormatFeatureBit.RENDER_TARGET)) {
            console.log('Depth texture is not color renderable?');
        }

        this._depthTexture = device.createTexture(depthTextureInfo);

        const depthTextureViewInfo = new TextureViewInfo(
            this._depthTexture,
            TextureType.TEX2D,
            Format.DEPTH,
            this._lodLevel,
            1,
        );
        this._depthTextureView = device.createTexture(depthTextureViewInfo);

        this._area.width = swapchain.width >> this._lodLevel;
        this._area.height = swapchain.height >> this._lodLevel;

        const depthAttachment = new DepthStencilAttachment();

        depthAttachment.format = Format.DEPTH;
        depthAttachment.barrier = device.getGeneralBarrier(new GeneralBarrierInfo(
            AccessFlagBit.NONE,
            AccessFlagBit.FRAGMENT_SHADER_READ_TEXTURE,
        ));

        const renderPassInfo = new RenderPassInfo(
            [], depthAttachment,
        );
        this._renderPass = device.createRenderPass(renderPassInfo);

        const fboInfo = new FramebufferInfo();
        fboInfo.renderPass = this._renderPass;
        fboInfo.depthStencilTexture = this._depthTextureView;
        this._framebuffer = device.createFramebuffer(fboInfo);
    }

    get area () : Rect { return this._area; }

    get frameBuffer (): Framebuffer {
        return this._framebuffer;
    }
    get renderPass (): RenderPass {
        return this._renderPass;
    }

    get depthTexture (): Texture {
        return this._depthTexture;
    }

    get depthTextureView (): Texture {
        return this._depthTextureView;
    }

    public onResize (width: number, height: number) {
        if (this._depthTexture.width === width && this._depthTexture.height === height) return;

        this._framebuffer.destroy();

        this._area.width = width;
        this._area.height = height;

        this._depthTexture.resize(width, height);

        const fboInfo = new FramebufferInfo();
        fboInfo.renderPass = this._renderPass;
        fboInfo.depthStencilTexture = this._depthTextureView;

        this._framebuffer.initialize(fboInfo);
    }

    public onDestroy () {
        this._framebuffer.destroy();
        this._depthTextureView.destroy();
        this._depthTexture.destroy();
        this._renderPass.destroy();
    }
}

class Cube {
    private _program: Program;
    private _framebuffer: Framebuffer = null!;
    private _inputs: ProgramInputs;
    private _bindings: ProgramBindings;

    constructor (device: Device, frameBuffer: Framebuffer) {
        this._framebuffer = frameBuffer;
        this._program = new Program({
            name: 'Basic Cube',
            vert: `
                vec4 vert() {
                    return  u_mvp * vec4(in_position, 1.0);
                }
            `,
            frag: `
                void frag(out FragColor o) {
                }
            `,
            extensions: [
                { name: 'GL_OES_standard_derivatives', type: IShaderExtensionType.ENABLE },
            ],
            attributes: [
                { name: 'in_position', format: Format.RGB32F, stream: 0 },
            ],
            blocks: [
                { name: 'MVP', members: [{ name: 'u_mvp', type: Type.MAT4 }], set: 1 },
                { name: 'Color', members: [{ name: 'u_color', type: Type.FLOAT4 }], set: 0 },
            ],
        });

        const cubeData = box({
            width: 1,
            length: 1,
            height: 2,
        });

        this._bindings = this._program.createBindings({});
        this._inputs = this._program.createInputs({
            maxVertexCount: cubeData.positions.length / 3,
            maxIndexCount: cubeData.indices?.length,
            maxIndirectDrawCount: 1,
            indexU32: true,
        });

        this._inputs.updateVertexBuffer(new Float32Array(cubeData.positions), 0);

        this._inputs.updateIndexBuffer(new Uint32Array(cubeData.indices || []));
        this._inputs.updateIndirectBuffer(new IndirectBuffer([new DrawInfo(0, 0, cubeData.indices?.length, 0)]));

        this._program.setPipelineState({ renderPass: this._framebuffer.renderPass });
    }

    get frameBuffer (): Framebuffer {
        return this._framebuffer;
    }

    get bindings (): ProgramBindings {
        return this._bindings;
    }

    get program (): Program {
        return this._program;
    }

    public draw (commandBuffer: CommandBuffer) {
        this._program.draw(commandBuffer, this._bindings, this._inputs);
    }

    public onDestroy () {
        this._program.destroy();
        this._inputs.destroy();
        this._bindings.destroy();
    }
}

export class DepthTest extends TestBase {
    private _program: Program;
    private _bindings: ProgramBindings;
    private _inputs: ProgramInputs;

    private _clearColor = new Color(1, 1, 1, 1);
    private _matrixHandle = NULL_HANDLE;

    private _model: Mat4 = new Mat4();
    private _view: Mat4 = new Mat4();
    private _proj: Mat4 = new Mat4();
    private _mvp: Mat4 = new Mat4();

    private _cube: Cube = null!;
    private _cubeFbo: DepthFrameBuffer = null!;

    private _sampler: Sampler = null!;

    constructor () {
        super();

        this._cubeFbo = new DepthFrameBuffer(TestBase.device, TestBase.swapchains[0]);
        this._cube = new Cube(TestBase.device, this._cubeFbo.frameBuffer);

        this._program = new Program({
            name: 'Depth Test',
            vert: `
                vec4 vert() {
                    texCoord = in_uv;
                    return vec4(in_position, 0.0, 1.0);
                }
            `,
            frag: `
                void frag(out FragColor o) {
                    float z = texture(u_texture, texCoord).x;
                    float viewZ = (u_near * u_far) / ((u_far - u_near) * z - u_far);
                    float depth = (viewZ + u_near) / (u_near - u_far);
                    o.fragColor.rgb = vec3(depth);
                    o.fragColor.a = 1.0;
                }
            `,
            attributes: [
                { name: 'in_position', format: Format.RG32F, stream: 0 },
                { name: 'in_uv', format: Format.RG32F, stream: 1 },
            ],
            blocks: [
                { name: 'Color', members: [{ name: 'u_color', type: Type.FLOAT4 }], set: 0 },
                {
                    name: 'NFUniform',
                    members: [
                        { name: 'u_near', type: Type.FLOAT },
                        { name: 'u_far', type: Type.FLOAT },
                    ],
                    set: 0,
                },
            ],
            samplerTextures: [
                { name: 'u_texture', type: Type.SAMPLER2D, set: 1 },
            ],
            varyings: [
                { name: 'texCoord', type: Type.FLOAT2 },
            ],
        });
        this._bindings = this._program.createBindings({});
        this._inputs = this._program.createInputs({
            maxVertexCount: 5,
            maxIndexCount: 9,
            maxIndirectDrawCount: 2,
        });

        this._inputs.updateVertexBuffer(new Float32Array([
            -1, -1,
            1, -1,
            1, 1,
            -1, 1,
        ]), 0);
        this._inputs.updateVertexBuffer(new Float32Array([
            0, 1,
            1, 1,
            1, 0,
            0, 0,
        ]), 1);
        this._inputs.updateIndexBuffer(new Uint16Array([0, 1, 2, 2, 3, 0]));
        this._inputs.updateIndirectBuffer(new IndirectBuffer([new DrawInfo(0, 0, 6, 0)]));

        this._sampler = TestBase.device.getSampler(new SamplerInfo(Filter.POINT, Filter.POINT, Filter.POINT));

        const samplerHandle = this._program.getHandle('u_texture');
        this._bindings.setTexture(samplerHandle, this._cubeFbo.depthTextureView);
        this._bindings.setSampler(samplerHandle, this._sampler);

        this._bindings.setUniform(this._program.getHandle('u_near'), 1.0);
        this._bindings.setUniform(this._program.getHandle('u_far'), 100.0);

        this._matrixHandle = this._cube.program.getHandle('u_mvp');

        this._proj = TestBase._getPerspectiveMat4(120, 1, 1, 10) as Mat4;

        Mat4.lookAt(this._view,
            new Vec3(0.0, 1.5, 2),
            new Vec3(0.0, 0.0, 0.0),
            new Vec3(0.0, 1.0, 0.0));
    }

    public onTick () {
        Mat4.lookAt(this._view,
            new Vec3(0.0, 2 + Math.sin(TestBase.cumulativeTime) * 2.0, 4 + Math.cos(TestBase.cumulativeTime) * 2.0),
            new Vec3(0.0, 0.0, 0.0),
            new Vec3(0.0, 1.0, 0.0));

        this._mvp = new Mat4(1.0);
        this._mvp.multiply(this._proj);
        this._mvp.multiply(this._view);
        this._mvp.multiply(this._model);

        this._cube.bindings.setUniform(this._matrixHandle, this._mvp);
        this._cube.bindings.update();
        this._bindings.update();

        TestBase.device.acquire(TestBase.swapchains);
        const commandBuffer = TestBase.commandBuffers[0];
        commandBuffer.begin();
        // // draw cube
        commandBuffer.beginRenderPass(this._cubeFbo.renderPass, this._cubeFbo.frameBuffer,
            this._cubeFbo.area, [], 1, 0);
        this._cube.draw(commandBuffer);
        commandBuffer.endRenderPass();

        // draw depth image
        commandBuffer.beginRenderPass(TestBase.defaultRenderPass, TestBase.defaultFramebuffer,
            TestBase.defaultRenderArea, [this._clearColor], 1, 0);
        this._program.draw(commandBuffer, this._bindings, this._inputs);
        commandBuffer.endRenderPass();

        commandBuffer.end();
        TestBase.device.flushCommands(TestBase.commandBuffers);
        TestBase.device.queue.submit(TestBase.commandBuffers);
        TestBase.device.present();
    }

    public onDestroy () {
        this._inputs.destroy();
        this._bindings.destroy();
        this._program.destroy();
        this._cube.onDestroy();
        this._cubeFbo.onDestroy();
    }
}
