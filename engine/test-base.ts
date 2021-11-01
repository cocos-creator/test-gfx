import { Device } from 'gfx/base/device';
import { Swapchain } from 'gfx/base/swapchain';
import { CommandBuffer } from 'gfx/base/command-buffer';
import { RenderPass } from 'gfx/base/render-pass';
import { Framebuffer } from 'gfx/base/framebuffer';
import { Color, Rect } from 'gfx/base/define';

export abstract class TestBase {
    public static device: Device;
    public static swapchains: Swapchain[] = [];
    public static commandBuffers: CommandBuffer[] = [];
    public static defaultRenderPass: RenderPass;
    public static defaultFramebuffer: Framebuffer;

    public static defaultRenderArea = new Rect(0, 0, 1, 1);
    // in seconds
    public static deltaTime = 0;
    public static accumulatedTime = 0;

    private static _previousTime = 0;
    private static _defaultClearColor = new Color(0.2, 0.2, 0.2, 1);
    private static _clearColors: Color[] = [this._defaultClearColor];

    public initialize () {
        TestBase.defaultRenderArea.width = TestBase.swapchains[0].width;
        TestBase.defaultRenderArea.height = TestBase.swapchains[0].height;
        this.onInit();
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

    // by default no-op
    protected onInit () {}
    protected onResize (swapchain: Swapchain) {}
    protected onDestroy () {}

    protected abstract onTick (): void;

    protected _beginOnscreenPass (clearColor = TestBase._defaultClearColor, clearDepth = 1, clearStencil = 0) {
        TestBase._clearColors[0] = clearColor;

        TestBase.device.acquire(TestBase.swapchains);
        TestBase.commandBuffers[0].begin();
        TestBase.commandBuffers[0].beginRenderPass(TestBase.defaultRenderPass, TestBase.defaultFramebuffer,
            TestBase.defaultRenderArea, TestBase._clearColors, clearDepth, clearStencil);
    }

    protected _endOnscreenPass () {
        TestBase.commandBuffers[0].endRenderPass();
        TestBase.commandBuffers[0].end();
    }

    protected _present () {
        TestBase.device.flushCommands(TestBase.commandBuffers);
        TestBase.device.queue.submit(TestBase.commandBuffers);
        TestBase.device.present();
    }
}
