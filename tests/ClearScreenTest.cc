#include "ClearScreenTest.h"

namespace cc {

void ClearScreen::onDestroy() {
}

bool ClearScreen::onInit() {
    return true;
}

void ClearScreen::onTick() {
    gfx::Color clearColor;
    clearColor.x = 1.0f;
    clearColor.y = std::abs(std::sin(_time));
    clearColor.z = 0.0f;
    clearColor.w = 1.0f;

    device->acquire();

    gfx::Rect renderArea = {0, 0, device->getWidth(), device->getHeight()};

    auto commandBuffer = commandBuffers[0];
    commandBuffer->begin();
    commandBuffer->beginRenderPass(fbo->getRenderPass(), fbo, renderArea, &clearColor, 1.0f, 0);
    commandBuffer->endRenderPass();
    commandBuffer->end();

    device->flushCommands(commandBuffers);
    device->getQueue()->submit(commandBuffers);
    device->present();
}

} // namespace cc
