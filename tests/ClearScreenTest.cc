#include "ClearScreenTest.h"
#include "gfx-gles2/GFXGLES2.h"
#include "gfx-gles3/GFXGLES3.h"

NS_CC_BEGIN

void ClearScreen::Destroy()
{
}

bool ClearScreen::initialize()
{
    return true;
}

void ClearScreen::tick(float dt) {

    GFXRect render_area = {0, 0, _device->width(), _device->height() };
    _time += dt;
    GFXColor clear_color;
    clear_color.r = 1.0f;
    clear_color.g = std::abs(std::sin(_time));
    clear_color.b = 0.0f;
    clear_color.a = 1.0f;

    _commandBuffer->Begin();
    _commandBuffer->BeginRenderPass(_fbo, render_area, GFXClearFlagBit::ALL, &clear_color, 1, 1.0f, 0);
    _commandBuffer->EndRenderPass();
    _commandBuffer->End();

    _device->queue()->submit(&_commandBuffer, 1);
    _device->Present();
}

NS_CC_END
