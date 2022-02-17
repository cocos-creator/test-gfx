import {
    Color, CullMode, DrawInfo, Format, IndirectBuffer, PolygonMode, PrimitiveMode, ShadeModel,
    ShaderStageFlagBit, Type,
} from 'gfx/base/define';
import { NULL_HANDLE, Program, ProgramBindings, ProgramInputs, IPipelineStateInfo } from '../chassis';
import { TestBase } from '../test-base';
import { Vec4, Vec3, Mat4 } from '../math';

export class BasicTriangle extends TestBase {
    private _program: Program;
    private _bindings: ProgramBindings;
    private _inputs: ProgramInputs;

    private _clearColor = new Color(1, 0, 0, 1);
    private _colorHandles = [NULL_HANDLE];
    private _matrixHandle = NULL_HANDLE;
    constructor () {
        super();

        this._program = new Program({
            name: 'Basic Triangle',
            vert: `
                vec4 vert() {
                    frag_color = vec4(in_color, 1.0);
                    return  u_mvp * vec4(in_position, 0.0, 1.0);
                }
            `,
            frag: `
                void frag(out FragColor o) {
                    o.fragColor = vec4(u_color[0].x, u_color[1].y, u_color[2].z, 1.0);
                }
            `,
            attributes: [
                { name: 'in_position', format: Format.RG32F, stream: 0 },
                { name: 'in_color', format: Format.RGB32F, stream: 1 },
            ],
            blocks: [
                { name: 'MVP', members: [{ name: 'u_mvp', type: Type.MAT4 }], set: 0 },
                {
                    name: 'Color',
                    members: [
                        { name: 'u_color', type: Type.FLOAT4, count: 3 },
                    ],
                    set: 0,
                },
            ],
            varyings: [
                { name: 'frag_color', type: Type.FLOAT4 },
            ],
        });
        this._bindings = this._program.createBindings({});
        this._inputs = this._program.createInputs({
            maxVertexCount: 5,
            maxIndexCount: 9,
            maxIndirectDrawCount: 2,
        });

        this._inputs.updateVertexBuffer(new Float32Array([
            -0.5, 0.5,
            -0.5, -0.5,
            0.5, -0.5,
            0.0, 0.5,
            0.5, 0.5,
        ]), 0);
        this._inputs.updateVertexBuffer(new Float32Array([
            1, 0, 0,
            1, 0, 0,
            0, 1, 0,
            0, 0, 1,
            1, 0, 0,
        ]), 1);

        this._inputs.updateIndexBuffer(new Uint16Array([1, 3, 0, 1, 2, 3, 2, 4, 3]));
        this._inputs.updateIndirectBuffer(new IndirectBuffer([new DrawInfo(0, 0, 3, 3), new DrawInfo(0, 0, 3, 2)]));

        this._bindings.setUniform(this._program.getHandle('u_color', 0, Type.FLOAT4, 0), new Vec4(0, 0, 0, 1));
        this._bindings.setUniform(this._program.getHandle('u_color', 0, Type.FLOAT4, 1), new Vec4(0, 0, 0, 1));
        this._bindings.setUniform(this._program.getHandle('u_color', 0, Type.FLOAT4, 2), new Vec4(0, 0, 0, 1));
        this._colorHandles[0] = this._program.getHandle('u_color', 0, Type.FLOAT, 0); // will only modify the x component
        this._colorHandles[1] = this._program.getHandle('u_color', 1, Type.FLOAT, 1); // will only modify the x component
        this._colorHandles[2] = this._program.getHandle('u_color', 2, Type.FLOAT, 2); // will only modify the x component
        this._matrixHandle = this._program.getHandle('u_mvp');
    }

    public onTick () {
        this._bindings.setUniform(this._colorHandles[0], Math.abs(Math.sin(TestBase.cumulativeTime)));
        this._bindings.setUniform(this._colorHandles[1], Math.abs(Math.sin(TestBase.cumulativeTime)));
        this._bindings.setUniform(this._colorHandles[2], Math.abs(Math.sin(TestBase.cumulativeTime)));

        this._bindings.setUniform(this._matrixHandle, TestBase._getOrthographicMat4(-1, 1, -1, 1, -1, 1));
        this._bindings.update();

        TestBase._acquire();
        TestBase._beginOnscreenPass(this._clearColor);

        this._program.draw(TestBase.commandBuffers[0], this._bindings, this._inputs);

        TestBase._endOnscreenPass();
        TestBase._present();
    }

    public onDestroy () {
        this._inputs.destroy();
        this._bindings.destroy();
        this._program.destroy();
    }
}
