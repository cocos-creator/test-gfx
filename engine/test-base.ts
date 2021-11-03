import { Device } from 'gfx/base/device';
import { Swapchain } from 'gfx/base/swapchain';
import { CommandBuffer } from 'gfx/base/command-buffer';
import { RenderPass } from 'gfx/base/render-pass';
import { Framebuffer } from 'gfx/base/framebuffer';
import { API, Color, Rect } from 'gfx/base/define';
import { Mat4 } from './math';

export interface IStandardShaderSource {
    vert: string;
    frag: string;
}

export interface IShaderSources<T> {
    glsl4: T;
    glsl3: T;
    glsl1: T;
}

export class TestBase {
    public static device: Device;
    public static swapchains: Swapchain[] = [];
    public static commandBuffers: CommandBuffer[] = [];
    public static defaultRenderArea = new Rect(0, 0, 1, 1);
    // in seconds
    public static deltaTime = 0;
    public static accumulatedTime = 0;

    private static _previousTime = 0;

    constructor () {
        TestBase.defaultRenderArea.width = TestBase.swapchains[0].width;
        TestBase.defaultRenderArea.height = TestBase.swapchains[0].height;
    }

    public tick () {
        const curTime = Date.now();
        if (TestBase._previousTime) {
            TestBase.deltaTime = (curTime - TestBase._previousTime) / 1000;
            TestBase.accumulatedTime += TestBase.deltaTime;
        }

        this.onTick();
        TestBase._previousTime = curTime;
    }

    public destroy () {
        this.onDestroy();
    }

    public resize (swapchain: Swapchain) {
        if (swapchain === TestBase.swapchains[0]) {
            TestBase.defaultRenderArea.width = TestBase.swapchains[0].width;
            TestBase.defaultRenderArea.height = TestBase.swapchains[0].height;
        }
        this.onResize(swapchain);
    }

    // by default no-ops
    protected onResize (swapchain: Swapchain) {}
    protected onDestroy () {}
    protected onTick () {}

    // static utilities

    public static defaultRenderPass: RenderPass;
    public static defaultFramebuffer: Framebuffer;

    private static _defaultClearColor = new Color(0.2, 0.2, 0.2, 1);
    private static _clearColors: Color[] = [TestBase._defaultClearColor];
    private static _m4_1 = new Mat4();

    public static assert (cond: boolean, msg = '') {
        if (!cond) {
            console.assert(cond, msg);
            // (null! as Record<string, number>).x = 1;
        }
    }

    protected static _acquire () {
        TestBase.device.acquire(TestBase.swapchains);
    }

    protected static _beginOnscreenPass (clearColor = TestBase._defaultClearColor, clearDepth = 1, clearStencil = 0) {
        TestBase._clearColors[0] = clearColor;
        TestBase.commandBuffers[0].begin();
        TestBase.commandBuffers[0].beginRenderPass(TestBase.defaultRenderPass, TestBase.defaultFramebuffer,
            TestBase.defaultRenderArea, TestBase._clearColors, clearDepth, clearStencil);
    }

    protected static _endOnscreenPass () {
        TestBase.commandBuffers[0].endRenderPass();
        TestBase.commandBuffers[0].end();
    }

    protected static _present () {
        TestBase.device.flushCommands(TestBase.commandBuffers);
        TestBase.device.queue.submit(TestBase.commandBuffers);
        TestBase.device.present();
    }

    protected static _getAppropriateShaderSource<T> (sources: IShaderSources<T>) {
        switch (TestBase.device.gfxAPI) {
        case API.WEBGL:
        case API.GLES2:
            return sources.glsl1;
        case API.WEBGL2:
        case API.GLES3:
            return sources.glsl3;
        case API.METAL:
        case API.VULKAN:
            return sources.glsl4;
        default: break;
        }
        return sources.glsl4;
    }

    protected static _createPerspective (fov: number, aspect: number, near: number, far: number,
        dst: Float32Array, offset = 0, swapchain = TestBase.swapchains[0]) {
        Mat4.perspective(TestBase._m4_1, fov, aspect, near, far, true,
            TestBase.device.capabilities.clipSpaceMinZ,
            TestBase.device.capabilities.clipSpaceSignY,
            swapchain.surfaceTransform);
        Mat4.toArray(dst, TestBase._m4_1, offset);
    }

    protected static _createOrthographic (left: number, right: number, bottom: number, top: number, near: number, far: number,
        dst: Float32Array, offset = 0, swapchain = TestBase.swapchains[0]) {
        Mat4.ortho(TestBase._m4_1, left, right, bottom, top, near, far,
            TestBase.device.capabilities.clipSpaceMinZ,
            TestBase.device.capabilities.clipSpaceSignY,
            swapchain.surfaceTransform);
        Mat4.toArray(dst, TestBase._m4_1, offset);
    }
}
