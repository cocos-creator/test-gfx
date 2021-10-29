import { Swapchain } from 'gfx/base/swapchain';

export abstract class TestBase {
    public initialize () {
        this.onInit();
    }

    public resize (swapchain: Swapchain) {
        this.onResize(swapchain);
    }

    public tick () {
        this.onTick();
    }

    public destroy () {
        this.onDestroy();
    }

    protected abstract onInit (): void;
    protected abstract onTick (): void;
    protected abstract onResize (swapchain: Swapchain): void;
    protected abstract onDestroy (): void;
}
