import { API, ColorAttachment, DepthStencilAttachment, DeviceInfo, FramebufferInfo,
    RenderPassInfo, SurfaceTransform, SwapchainInfo } from 'gfx/base/define';
import { EmptyDevice } from 'gfx/empty/empty-device';
import { WebGLDevice } from 'gfx/webgl/webgl-device';
import { WebGL2Device } from 'gfx/webgl2/webgl2-device';
import { TestBase } from './test-base';
import { testlist } from './tests';

export interface IApplicationInfo {
    api: API;
    canvas: HTMLCanvasElement;
    width: number;
    height: number;
}

export class Application {
    private _test: TestBase | null = null;
    private _currentIndex = 0;

    public initialize (info: IApplicationInfo) {
        const deviceInfo = new DeviceInfo();
        switch (info.api) {
        case API.WEBGL:
            TestBase.device = new WebGLDevice();
            break;
        case API.WEBGL2:
            TestBase.device = new WebGL2Device();
            break;
        default:
            TestBase.device = new EmptyDevice();
            break;
        }
        TestBase.device.initialize(deviceInfo);

        const swapchainInfo = new SwapchainInfo(info.canvas);
        swapchainInfo.width = info.width;
        swapchainInfo.height = info.height;
        TestBase.swapchains.push(TestBase.device.createSwapchain(swapchainInfo));

        TestBase.commandBuffers.push(TestBase.device.commandBuffer);

        const colorAttachment = new ColorAttachment(TestBase.swapchains[0].colorTexture.format);
        const depthAttachment = new DepthStencilAttachment(TestBase.swapchains[0].depthStencilTexture.format);
        TestBase.defaultRenderPass = TestBase.device.createRenderPass(new RenderPassInfo(
            [colorAttachment],
            depthAttachment,
        ));

        TestBase.defaultFramebuffer = TestBase.device.createFramebuffer(new FramebufferInfo(
            TestBase.defaultRenderPass,
            [TestBase.swapchains[0].colorTexture],
            TestBase.swapchains[0].depthStencilTexture,
        ));

        this._currentIndex = -1;
        this.nextTest();
    }

    public nextTest (backward = false) {
        this._currentIndex += backward ? -1 : 1;
        this._currentIndex -= Math.floor(this._currentIndex / testlist.length) * testlist.length;

        this._test?.destroy();
        this._test = new testlist[this._currentIndex]();
    }

    public tick () {
        this._test?.tick();
        return TestBase.running;
    }

    public onResize (width: number, height: number) {
        TestBase.swapchains[0].resize(width, height, SurfaceTransform.IDENTITY);
        this._test?.resize(TestBase.swapchains[0]);
    }

    public destroy () {
        this._test?.destroy();

        TestBase.defaultRenderPass?.destroy();
        TestBase.defaultFramebuffer?.destroy();

        for (const swapchain of TestBase.swapchains) {
            swapchain.destroy();
        }
        TestBase.device?.destroy();
    }
}
