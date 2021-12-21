import {
    Attribute, BufferTextureCopy, Color, CullMode, DrawInfo, Filter, Format, FramebufferInfo, IndirectBuffer, PolygonMode,
    PrimitiveMode, SampleCount, SamplerInfo, ShadeModel, ShaderStageFlagBit, TextureFlagBit, TextureInfo, TextureType, TextureUsageBit, Type,
} from 'gfx/base/define';
import { RasterizerState } from 'gfx/base/pipeline-state';
import { Texture } from 'gfx/base/texture';
import { Sampler } from 'gfx/base/states/sampler';
import { box } from '../primitive/box';
import { NULL_HANDLE, Program, ProgramBindings, ProgramInputs, IShaderExtension, IShaderExtensionType } from '../chassis';
import { TestBase } from '../test-base';
import { Vec4, Vec3, Mat4, IVec3Like, IVec4Like } from '../math';

export class EngineTest extends TestBase {
    private _program: Program;
    private _bindings: ProgramBindings;
    private _inputs: ProgramInputs;
    private _texture: Texture;
    private _sampler: Sampler;

    private _clearColor = new Color(1, 0, 0, 1);
    private _colorHandle = NULL_HANDLE;
    private _matrixHandle = NULL_HANDLE;

    private _model: Mat4 = new Mat4();
    private _view: Mat4 = new Mat4();
    private _proj: Mat4 = new Mat4();
    private _mvp: Mat4 = new Mat4();

    constructor () {
        super();

        this._program = new Program({
            name: 'Engine Test',
            vert: `
                vec4 vert() {
                    texCoord = in_uv;
                    return  u_mvp * vec4(in_position, 1.0);
                }
            `,
            frag: `
                void frag(out FragColor o) {
                    u_color * 0.0;
                    o.fragColor = texture(cube, texCoord);
                }
            `,
            extensions: [
                { name: 'GL_OES_standard_derivatives', type: IShaderExtensionType.ENABLE },
            ],
            attributes: [
                { name: 'in_position', format: Format.RGB32F, stream: 0 },
                { name: 'in_uv', format: Format.RG32F, stream: 1 },
            ],
            blocks: [
                { name: 'MVP', members: [{ name: 'u_mvp', type: Type.MAT4 }], set: 1 },
                { name: 'Color', members: [{ name: 'u_color', type: Type.FLOAT4 }], set: 0 },
            ],
            samplerTextures: [
                { name: 'cube', type: Type.SAMPLER2D, set: 1 },
            ],
            varyings: [
                { name: 'texCoord', type: Type.FLOAT2 },
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
        this._inputs.updateVertexBuffer(new Float32Array(cubeData.uvs || []), 1);

        this._inputs.updateIndexBuffer(new Uint32Array(cubeData.indices || []));
        this._inputs.updateIndirectBuffer(new IndirectBuffer([new DrawInfo(0, 0, cubeData.indices?.length, 0)]));

        const rgbWidth = 4;
        const width = 100;
        const height = 100;

        const buffWriteView = new Uint8Array(width * height * rgbWidth);
        const c = new Vec4(0, 0, 0, 255);
        const temp = width + height;

        for (let i = 0; i < height; i++) {
            for (let j = 0; j < width; j++) {
                c.x = i / height * 255;
                c.y = j / width * 255;
                c.z = (1 - (i + j) / temp) * 255;
                Vec4.toArray(buffWriteView, c, i * width * rgbWidth + j * rgbWidth);
            }
        }

        const textureInfo = new TextureInfo(
            TextureType.TEX2D,
            TextureUsageBit.SAMPLED | TextureUsageBit.TRANSFER_DST,
            Format.RGBA8,
            width,
            height,
        );

        this._texture = TestBase.device.createTexture(textureInfo);

        this._sampler = TestBase.device.getSampler(new SamplerInfo(
            Filter.POINT, Filter.POINT,
        ));

        // create a framebuffer with swapChain's texture and destroy it.
        const frameBuffer = TestBase.device.createFramebuffer(new FramebufferInfo(
            TestBase.defaultRenderPass,
            [TestBase.swapchains[0].colorTexture],
            TestBase.swapchains[0].depthStencilTexture,
        ));
        frameBuffer.destroy();

        const samplerHandle = this._program.getHandle('cube');

        // create a texture and bind it to gpu, then destroy it.
        const tempTexture = TestBase.device.createTexture(textureInfo);
        this._bindings.setTexture(samplerHandle, tempTexture);
        tempTexture.destroy();

        this._bindings.setTexture(samplerHandle, this._texture);
        this._bindings.setSampler(samplerHandle, this._sampler);

        const region = new BufferTextureCopy();
        region.texExtent.width = width;
        region.texExtent.height = height;
        TestBase.device.copyBuffersToTexture([buffWriteView], this._texture, [region]);

        this._bindings.setUniform(this._program.getHandle('u_color'), new Vec4(0, 1, 0, 1));
        this._colorHandle = this._program.getHandle('u_color', 0, Type.FLOAT); // will only modify the x component
        this._matrixHandle = this._program.getHandle('u_mvp');

        this._proj = TestBase._getPerspectiveMat4(120, 1, 1, 10) as Mat4;

        Mat4.lookAt(this._view,
            new Vec3(0.0, 1.5, 2),
            new Vec3(0.0, 0.0, 0.0),
            new Vec3(0.0, 1.0, 0.0));

        this._program.setPipelineState({
            rasterizerState: new RasterizerState(
                false,
                PolygonMode.FILL,
                ShadeModel.GOURAND,
                CullMode.BACK,
                true,
                false,
                0,
                0.0,
                0.0,
                true,
                false,
                2.0,
            ),
        });
    }

    public onTick () {
        this._bindings.setUniform(this._colorHandle, Math.abs(Math.sin(TestBase.cumulativeTime)));

        Mat4.lookAt(this._view,
            new Vec3(0.0, 2 + Math.sin(TestBase.cumulativeTime) * 2.0, 4 + Math.cos(TestBase.cumulativeTime) * 2.0),
            new Vec3(0.0, 0.0, 0.0),
            new Vec3(0.0, 1.0, 0.0));

        this._mvp = new Mat4(1.0);
        this._mvp.multiply(this._proj);
        this._mvp.multiply(this._view);
        this._mvp.multiply(this._model);

        this._bindings.setUniform(this._matrixHandle, this._mvp);
        this._bindings.update();

        TestBase._acquire();
        TestBase._beginOnscreenPass(this._clearColor);

        this._program.draw(TestBase.commandBuffers[0], this._bindings, this._inputs);

        TestBase._endOnscreenPass();
        TestBase._present();
    }

    public onDestroy () {
        // test the destruction of gpu buffers/input assemblers
        this._inputs.destroy();
        // test the destruction of gpu uniform buffers
        this._bindings.destroy();
        // test the destruction of gpu shaders
        this._program.destroy();
        // test the destruction of textures
        this._texture.destroy();
    }
}
