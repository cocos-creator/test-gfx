import {
    Attribute, Color, CullMode, DrawInfo, Format, IndirectBuffer, PolygonMode,
    PrimitiveMode, ShadeModel, ShaderStageFlagBit, Type,
} from 'gfx/base/define';
import { RasterizerState } from 'gfx/base/pipeline-state';
// import { sphere } from '../../../engine/cocos/primitive';
import { NULL_HANDLE, Program, ProgramBindings, ProgramInputs, IPipelineStateInfo } from '../chassis';
import { TestBase } from '../test-base';
import { Vec4, Vec3, Mat4 } from '../math';

export interface IGeometry {
    positions: number[];
    normals?: number[];
    uvs?: number[];
    tangents?: number[];
    colors?: number[];
    attributes?: Attribute[];
    customAttributes?: {
        attr: Attribute,
        values: number[],
    }[];

    boundingRadius?: number;
    minPos?: {
        x: number;
        y: number;
        z: number;
    };
    maxPos?: {
        x: number;
        y: number;
        z: number;
    };
    indices?: number[];
    primitiveMode?: PrimitiveMode;
    doubleSided?: boolean;
}

export interface ISphereOptions {
    includeNormal: boolean;
    includeUV: boolean;
    segments: number;
}

export default function sphere (radius = 0.5, opts: RecursivePartial<ISphereOptions> = {}): IGeometry {
    const segments = opts.segments !== undefined ? opts.segments : 32;

    // lat === latitude
    // lon === longitude

    const positions: number[] = [];
    const normals: number[] = [];
    const uvs: number[] = [];
    const indices: number[] = [];
    const minPos = new Vec3(-radius, -radius, -radius);
    const maxPos = new Vec3(radius, radius, radius);
    const boundingRadius = radius;

    for (let lat = 0; lat <= segments; ++lat) {
        const theta = lat * Math.PI / segments;
        const sinTheta = Math.sin(theta);
        const cosTheta = -Math.cos(theta);

        for (let lon = 0; lon <= segments; ++lon) {
            const phi = lon * 2 * Math.PI / segments - Math.PI / 2.0;
            const sinPhi = Math.sin(phi);
            const cosPhi = Math.cos(phi);

            const x = sinPhi * sinTheta;
            const y = cosTheta;
            const z = cosPhi * sinTheta;
            const u = lon / segments;
            const v = lat / segments;

            positions.push(x * radius, y * radius, z * radius);
            normals.push(x, y, z);
            uvs.push(u, v);

            if ((lat < segments) && (lon < segments)) {
                const seg1 = segments + 1;
                const a = seg1 * lat + lon;
                const b = seg1 * (lat + 1) + lon;
                const c = seg1 * (lat + 1) + lon + 1;
                const d = seg1 * lat + lon + 1;

                indices.push(a, d, b);
                indices.push(d, c, b);
            }
        }
    }

    return {
        positions,
        indices,
        normals,
        uvs,
        minPos,
        maxPos,
        boundingRadius,
    };
}

export class BasicSphere extends TestBase {
    private _program: Program;
    private _bindings: ProgramBindings;
    private _inputs: ProgramInputs;

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
            name: 'Basic Sphere',
            vert: `
                vec4 vert() {
                    frag_color = vec4(in_position.xzy, 1.0);
                    return  u_mvp * vec4(in_position, 1.0);
                }
            `,
            frag: `
                void frag(out FragColor o) {
                    o.fragColor0 = u_color * 0.2 + frag_color * 0.8;
                }
            `,
            attributes: [
                { name: 'in_position', format: Format.RGB32F },
            ],
            blocks: [
                { name: 'MVP', members: [{ name: 'u_mvp', type: Type.MAT4 }], set: 1 },
                { name: 'Color', members: [{ name: 'u_color', type: Type.FLOAT4 }], set: 0 },
            ],
            varyings: [
                { name: 'frag_color', type: Type.FLOAT4 },
            ],
        });

        const sphereData = sphere(0.7, { segments: 20 });

        this._bindings = this._program.createBindings({});
        this._inputs = this._program.createInputs({
            maxVertexCount: sphereData.positions.length / 3,
            maxIndexCount: sphereData.indices?.length,
            maxIndirectDrawCount: 1,
            indexU32: true,
        });

        this._inputs.updateVertexBuffer(new Float32Array(sphereData.positions));
        this._inputs.updateIndexBuffer(new Uint32Array(sphereData.indices || []));
        this._inputs.updateIndirectBuffer(new IndirectBuffer([new DrawInfo(0, 0, sphereData.indices?.length, 0)]));

        this._bindings.setUniform(this._program.getHandle('u_color'), new Vec4(0, 1, 0, 1));
        this._colorHandle = this._program.getHandle('u_color', 0, Type.FLOAT); // will only modify the x component
        this._matrixHandle = this._program.getHandle('u_mvp');

        this._proj = TestBase._getPerspectiveMat4(120, 1, 1, 20) as Mat4;

        Mat4.lookAt(this._view,
            new Vec3(0.0, 3.0, 4.0),
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

        this._model.rotate(0.02, new Vec3(0.0, 1.0, 0.0));

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
        this._inputs.destroy();
        this._bindings.destroy();
        this._program.destroy();
    }
}
