#include "ClearScreenTest.h"

namespace cc {

void ClearScreen::destroy() {
}

bool ClearScreen::initialize() {
    return true;
}

void ClearScreen::tick(float dt) {

    gfx::Rect render_area = {0, 0, _device->getWidth(), _device->getHeight()};
    _time += dt;
    gfx::Color clear_color;
    clear_color.x = 1.0f;
    clear_color.y = std::abs(std::sin(_time));
    clear_color.z = 0.0f;
    clear_color.w = 1.0f;

    _device->acquire();

    auto commandBuffer = _commandBuffers[0];
    commandBuffer->begin();
    commandBuffer->beginRenderPass(_fbo->getRenderPass(), _fbo, render_area, &clear_color, 1.0f, 0);
    commandBuffer->endRenderPass();
    commandBuffer->end();

    _device->getQueue()->submit(_commandBuffers);
    _device->present();
}

} // namespace cc
