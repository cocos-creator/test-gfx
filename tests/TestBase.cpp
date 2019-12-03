#include "TestBase.h"
#include "gfx-gles2/GFXGLES2.h"
#include "gfx-gles3/GFXGLES3.h"

NS_CC_BEGIN
TestBaseI::TestBaseI(const WindowInfo& info)
{
#if 0
    device_ = CC_NEW(GLES3Device);
#else
    _device = CC_NEW(GLES2Device);
#endif

    GFXDeviceInfo dev_info;
    dev_info.window_handle = info.windowHandle;
    dev_info.width = info.screen.width;
    dev_info.height = info.screen.height;
    dev_info.native_width = info.physicalWidth;
    dev_info.native_height = info.physicalHeight;
    _device->Initialize(dev_info);

    GFXCommandBufferInfo cmd_buff_info;
    cmd_buff_info.allocator = _device->cmd_allocator();
    cmd_buff_info.type = GFXCommandBufferType::PRIMARY;
    _commandBuffer = _device->CreateGFXCommandBuffer(cmd_buff_info);

    _fbo = _device->window()->framebuffer();
}

void TestBaseI::Destroy()
{
    CC_SAFE_DESTROY(_device);
    CC_SAFE_DESTROY(_fbo);
    CC_SAFE_DESTROY(_commandBuffer);
}

NS_CC_END
