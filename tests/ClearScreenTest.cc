#include "ClearScreenTest.h"

namespace cc {

void ClearScreen::onDestroy() {
}

bool ClearScreen::onInit() {
    return true;
}

void ClearScreen::onTick() {
    auto *swapchain = swapchains[0];
    auto *fbo       = fbos[0];

    gfx::Color clearColor;
    clearColor.x = 1.0F;
    clearColor.y = std::abs(std::sin(_time));
    clearColor.z = 0.0F;
    clearColor.w = 1.0F;

    device->acquire(&swapchain, 1);

    gfx::Rect renderArea = {0, 0, swapchain->getWidth(), swapchain->getHeight()};

    auto *commandBuffer = commandBuffers[0];
    commandBuffer->begin();
    commandBuffer->beginRenderPass(fbo->getRenderPass(), fbo, renderArea, &clearColor, 1.0F, 0);
    commandBuffer->endRenderPass();
    commandBuffer->end();

    device->flushCommands(commandBuffers);
    device->getQueue()->submit(commandBuffers);
    device->present();
}

} // namespace cc
