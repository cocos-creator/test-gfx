#include "ClearScreenTest.h"

#if (CC_PLATFORM == CC_PLATFORM_MAC_OSX)
#include "gfx-metal/GFXMTL.h"
#else
#include "gfx-gles2/GFXGLES2.h"
#include "gfx-gles3/GFXGLES3.h"
#endif

NS_CC_BEGIN

void ClearScreen::destroy()
{
}

bool ClearScreen::initialize()
{
    return true;
}

void ClearScreen::tick(float dt) {

    GFXRect render_area = {0, 0, _device->getWidth(), _device->getHeight() };
    _time += dt;
    GFXColor clear_color;
    clear_color.r = 1.0f;
    clear_color.g = std::abs(std::sin(_time));
    clear_color.b = 0.0f;
    clear_color.a = 1.0f;

    for(auto commandBuffer : _commandBuffers)
    {
        commandBuffer->begin();
        commandBuffer->beginRenderPass(_fbo, render_area, GFXClearFlagBit::ALL, std::move(std::vector<GFXColor>({clear_color})), 1.0f, 0);
        commandBuffer->endRenderPass();
        commandBuffer->end();
    }
    _device->getQueue()->submit(_commandBuffers);
    _device->present();
}

NS_CC_END
