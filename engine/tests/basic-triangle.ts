import { Color, DrawInfo, Format, IndirectBuffer, ShaderStageFlagBit, Type } from 'gfx/base/define';
import { NULL_HANDLE, Program, ProgramBindings, ProgramInputs } from '../chassis';
import { TestBase } from '../test-base';
import { Vec4 } from '../math';

export class BasicTriangle extends TestBase {
    private _program: Program;
    private _bindings: ProgramBindings;
    private _inputs: ProgramInputs;

    private _clearColor = new Color(1, 0, 0, 1);
    private _colorHandle = NULL_HANDLE;
    private _matrixHandle = NULL_HANDLE;

    constructor () {
        super();

        this._program = new Program({
            name: 'Basic Triangle',
            vert: `
                vec4 vert() {
                    return u_mvp * vec4(a_position, 0.0, 1.0);
                }
            `,
            frag: `
                void frag(out FragOutput o) {
                    o.fragColor = u_color;
                }
            `,
            attributes: [
                { name: 'a_position', format: Format.RG32F },
            ],
            blocks: [
                { name: 'Color', members: [{ name: 'u_color', type: Type.FLOAT4 }], set: 0 },
                { name: 'MVP', members: [{ name: 'u_mvp', type: Type.MAT4 }], set: 0 },
            ],
        });
        this._bindings = this._program.createBindings({});
        this._inputs = this._program.createInputs({
            maxVertexCount: 5,
            maxIndexCount: 9,
            maxIndirectDrawCount: 1,
        });

        this._inputs.updateVertexBuffer(new Float32Array([
            -0.5,  0.5,
            -0.5, -0.5,
            0.5, -0.5,
            0.0,  0.5,
            0.5,  0.5,
        ]));
        this._inputs.updateIndexBuffer(new Uint16Array([1, 3, 0, 1, 2, 3, 2, 4, 3]));
        this._inputs.updateIndirectBuffer(new IndirectBuffer([new DrawInfo(0, 0, 3, 3)]));

        this._bindings.setUniform(this._program.getHandle('u_color'), new Vec4(0, 1, 0, 1));
        this._colorHandle = this._program.getHandle('u_color', 0, Type.FLOAT); // will only modify the x component
        this._matrixHandle = this._program.getHandle('u_mvp');
    }

    public onTick () {
        this._bindings.setUniform(this._colorHandle, Math.abs(Math.sin(TestBase.cumulativeTime)));
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
