import { Color, CullMode, DrawInfo, Format, IndirectBuffer, PolygonMode, PrimitiveMode, ShadeModel, ShaderStageFlagBit, Type } from 'gfx/base/define';
import { NULL_HANDLE, Program, ProgramBindings, ProgramInputs, IPipelineStateInfo } from '../chassis';
import { TestBase } from '../test-base';
import { Vec4, Vec3, Mat4 } from '../math';

export class RotatingTriangle extends TestBase {
    private _program: Program;
    private _bindings: ProgramBindings;
    private _inputs: ProgramInputs;

    private _clearColor = new Color(1, 0, 0, 1);
    private _colorHandle = NULL_HANDLE;
    private _ucolorHandle = NULL_HANDLE;

    private _instanceRange = 0;

    private _modelHandle = NULL_HANDLE;
    private _viewHandle = NULL_HANDLE;
    private _projHandle = NULL_HANDLE;

    private _model: Mat4 = new Mat4();
    private _view: Mat4 = new Mat4();
    private _proj: Mat4 = new Mat4();
    private _mvp: Mat4 = new Mat4();

    private _instance = 0;

    constructor () {
        super();

        this._program = new Program({
            name: 'Rotating Triangle',
            vert: `
                vec4 vert() {
                    frag_color = vec4(in_color, 1.0);
                    return  u_proj * u_view * u_model * vec4(in_position, 0.0, 1.0);
                }
            `,
            frag: `
                void frag(out FragColor o) {
                    u_material * 0.01;
                    o.fragColor = u_color * 0.2 + frag_color * 0.8;
                }
            `,
            attributes: [
                { name: 'in_position', format: Format.RG32F },
                { name: 'in_color', format: Format.RGB32F },
            ],
            blocks: [
                {
                    name: 'MVP',
                    members: [
                        { name: 'u_model', type: Type.MAT4 },
                        { name: 'u_view', type: Type.MAT4 },
                        { name: 'u_proj', type: Type.MAT4 },
                    ],
                    set: 0,
                },
                { name: 'Color', members: [{ name: 'u_color', type: Type.FLOAT4 }], set: 0, dynamic: true },
                { name: 'Material', members: [{ name: 'u_material', type: Type.FLOAT4 }], set: 0, dynamic: true },
            ],
            varyings: [
                { name: 'frag_color', type: Type.FLOAT4 },
            ],
            attachments: 2,
        });
        this._bindings = this._program.createBindings({ maxInstanceCount: { Color: 2, Material: 5 } });
        this._inputs = this._program.createInputs({
            maxVertexCount: 5,
            maxIndexCount: 9,
            maxIndirectDrawCount: 1,
        });

        this._inputs.updateVertexBuffer(new Float32Array([
            -0.5, 0.5, 1, 0, 0,
            -0.5, -0.5, 1, 0, 0,
            0.5, -0.5, 0, 1, 0,
            0.0, 0.5, 0, 0, 1,
            0.5, 0.5, 1, 0, 0,
        ]));
        this._inputs.updateIndexBuffer(new Uint16Array([1, 3, 0, 1, 2, 3, 2, 4, 3]));
        this._inputs.updateIndirectBuffer(new IndirectBuffer([new DrawInfo(0, 0, 3, 3)]));

        this._bindings.setUniform(this._program.getHandle('u_color', 0, Type.FLOAT4), new Vec4(0, 1, 0, 1), 0);
        this._bindings.setUniform(this._program.getHandle('u_color', 0, Type.FLOAT4), new Vec4(0, 1, 0, 1), 1);
        this._ucolorHandle = this._program.getHandle('u_color', 1, Type.FLOAT); // will only modify the x component
        this._colorHandle = this._program.getHandle('Color'); // get the handle of the Color block
        this._instanceRange = this._bindings.getInstanceRange(this._colorHandle);

        const material = this._program.getHandle('Material');
        const materialRange = this._bindings.getInstanceRange(material);
        this._bindings.setBufferInstance(material, 1);
        this._bindings.setBufferInstance(material, 2);
        this._bindings.setBufferInstance(material, 3);
        this._bindings.setBufferInstance(material, 4);
        this._bindings.setBufferInstance(material, 0);

        this._modelHandle = this._program.getHandle('u_model');
        this._viewHandle = this._program.getHandle('u_view');
        this._projHandle = this._program.getHandle('u_proj');

        this._proj = TestBase._getOrthographicMat4(-1, 1, -1, 1, -1, 1) as Mat4;
        this._bindings.setUniform(this._viewHandle, this._view);
        this._bindings.setUniform(this._projHandle, this._proj);

        Mat4.lookAt(this._view,
            new Vec3(0.0, 0.0, 0.25),
            new Vec3(0.0, 0.0, 0.0),
            new Vec3(0.0, 1.0, 0.0));
    }

    public onTick () {
        this._bindings.setUniform(this._ucolorHandle, Math.abs(Math.sin(TestBase.cumulativeTime)), this._instance);

        this._model.rotate(0.02, new Vec3(0.0, 1.0, 0.0));

        this._bindings.setUniform(this._modelHandle, this._model);

        this._bindings.update();

        TestBase._acquire();
        TestBase._beginOnscreenPass(this._clearColor);

        this._bindings.setBufferInstance(this._colorHandle, this._instance);
        this._program.draw(TestBase.commandBuffers[0], this._bindings, this._inputs);

        TestBase._endOnscreenPass();
        TestBase._present();

        if (++this._instance >= this._instanceRange) this._instance = 0;
    }

    public onDestroy () {
        this._inputs.destroy();
        this._bindings.destroy();
        this._program.destroy();
    }
}
