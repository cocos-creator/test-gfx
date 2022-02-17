import { Color, CullMode, DrawInfo, Format, IndirectBuffer, PolygonMode, PrimitiveMode, ShadeModel, ShaderStageFlagBit, Type } from 'gfx/base/define';
import { NULL_HANDLE, Program, ProgramBindings, ProgramInputs, IPipelineStateInfo } from '../chassis';
import { TestBase } from '../test-base';
import { Vec4, Vec3, Mat4, Vec2 } from '../math';

export class JuliaSet extends TestBase {
    private _program: Program;
    private _bindings: ProgramBindings;
    private _inputs: ProgramInputs;

    private _clearColor = new Color(0, 0, 0, 0);
    private _offsetHandle = NULL_HANDLE;
    private _resolutionHandle = NULL_HANDLE;
    private _matrixHandle = NULL_HANDLE;
    constructor () {
        super();

        this._program = new Program({
            name: 'Julia Set',
            vert: `
                vec4 vert() {
                    return  u_mvp * vec4(in_position, 0.0, 1.0);
                }
            `,
            frag: `
                const int MAX_ITERATIONS = 256;

                struct complex {
                    float real;
                    float imaginary;
                };

                int fractal(complex c, complex z) {
                    for (int iteration = 0; iteration < MAX_ITERATIONS; iteration++) {

                        // z <- z^2 + c
                        float real = z.real * z.real - z.imaginary * z.imaginary + c.real;
                        float imaginary = 2.0 * z.real * z.imaginary + c.imaginary;

                        z.real = real;
                        z.imaginary = imaginary;

                        if (z.real * z.real + z.imaginary * z.imaginary > 4.0) {
                            return iteration;
                        }
                    }
                    return 0;
                }

                int mandelbrot(vec2 coordinate) {
                    complex c = complex(coordinate.x, coordinate.y);
                    complex z = complex(0.0, 0.0);

                    return fractal(c, z);
                }

                int julia(vec2 coordinate, vec2 offset) {
                    complex c = complex(offset.x, offset.y);
                    complex z = complex(coordinate.x, coordinate.y);

                    return fractal(c, z);
                }

                vec2 fragCoordToXY(vec2 fragCoord, vec2 res) {
                    vec2 relativePosition = fragCoord.xy / res.xy;
                    float aspectRatio = res.x / res.y;

                    vec2 cartesianPosition = (relativePosition - 0.5) * 4.0;
                    cartesianPosition.x *= aspectRatio;

                    return cartesianPosition;
                }

                void frag(out FragColor o) {
                    vec2 pos = fragCoordToXY(gl_FragCoord.xy, u_offset_size.zw);
                    vec2 offset = u_offset_size.xy;
                    int val = julia(pos, offset);
                    // int val = mandelbrot(pos);

                    o.fragColor = vec4(1.0) * 5.0 * float(val) / float(MAX_ITERATIONS);
                }
            `,
            attributes: [
                { name: 'in_position', format: Format.RG32F, stream: 0 },
            ],
            blocks: [
                { name: 'MVP', members: [{ name: 'u_mvp', type: Type.MAT4 }], set: 0 },
                { name: 'JULIA', members: [{ name: 'u_offset_size', type: Type.FLOAT4 }], set: 0 },
            ],
        });
        this._bindings = this._program.createBindings({});
        this._inputs = this._program.createInputs({
            maxVertexCount: 4,
            maxIndexCount: 6,
            maxIndirectDrawCount: 1,
        });

        this._inputs.updateVertexBuffer(new Float32Array([
            -1, -1,
            1, -1,
            1, 1,
            -1, 1,
        ]), 0);

        this._inputs.updateIndexBuffer(new Uint16Array([0, 1, 2, 2, 3, 0]));
        this._inputs.updateIndirectBuffer(new IndirectBuffer([new DrawInfo(0, 0, 6, 0)]));
        this._bindings.setUniform(this._program.getHandle('u_offset_size', 0, Type.FLOAT4, 0), new Vec4(0, 0, 0, 0));

        this._offsetHandle = this._program.getHandle('u_offset_size', 0, Type.FLOAT2, 0);
        this._resolutionHandle = this._program.getHandle('u_offset_size', 2, Type.FLOAT2, 0);
        this._bindings.setUniform(this._resolutionHandle, new Vec2(1280, 960));

        this._matrixHandle = this._program.getHandle('u_mvp');
    }

    public onTick () {
        this._bindings.setUniform(this._offsetHandle,
            new Vec2(Math.sin(TestBase.cumulativeTime * 0.1), Math.cos(TestBase.cumulativeTime * 0.1) * 0.5));

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
