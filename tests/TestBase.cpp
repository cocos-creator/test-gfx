#include "TestBase.h"

//#define USE_METAL
//#define USE_GLES3
//#define USE_GLES2

#if defined(USE_GLES2)
    #include "gfx-gles2/GFXGLES2.h"
#elif defined(USE_GLES3)
    #include "gfx-gles3/GFXGLES3.h"
#elif defined(USE_METAL)
    #include "gfx-metal/GFXMTL.h"
#else
    #include "gfx-vulkan/GFXVulkan.h"
#endif

namespace cc {

gfx::Device *TestBaseI::_device = nullptr;
gfx::Framebuffer *TestBaseI::_fbo = nullptr;
gfx::RenderPass *TestBaseI::_renderPass = nullptr;
std::vector<gfx::CommandBuffer *> TestBaseI::_commandBuffers;

TestBaseI::TestBaseI(const WindowInfo &info) {
    if (_device == nullptr) {
#if defined(USE_GLES2)
        _device = CC_NEW(gfx::GLES2Device);
#elif defined(USE_GLES3)
        _device = CC_NEW(gfx::GLES3Device);
#elif defined(USE_METAL)
        _device = CC_NEW(gfx::CCMTLDevice);
#else
        _device = CC_NEW(gfx::CCVKDevice);
#endif

        gfx::DeviceInfo dev_info;
        dev_info.windowHandle = info.windowHandle;
        dev_info.width = info.screen.width;
        dev_info.height = info.screen.height;
        dev_info.nativeWidth = info.physicalWidth;
        dev_info.nativeHeight = info.physicalHeight;
        _device->initialize(dev_info);
    }
    if (_fbo == nullptr) {
        gfx::RenderPassInfo renderPassInfo;
        gfx::ColorAttachment colorAttachment;
        colorAttachment.format = _device->getColorFormat();
        colorAttachment.loadOp = gfx::LoadOp::CLEAR;
        colorAttachment.storeOp = gfx::StoreOp::STORE;
        colorAttachment.sampleCount = 1;
        colorAttachment.beginLayout = gfx::TextureLayout::UNDEFINED;
        colorAttachment.endLayout = gfx::TextureLayout::PRESENT_SRC;
        renderPassInfo.colorAttachments.emplace_back(colorAttachment);

        gfx::DepthStencilAttachment &depthStencilAttachment = renderPassInfo.depthStencilAttachment;
        depthStencilAttachment.format = _device->getDepthStencilFormat();
        depthStencilAttachment.depthLoadOp = gfx::LoadOp::CLEAR;
        depthStencilAttachment.depthStoreOp = gfx::StoreOp::STORE;
        depthStencilAttachment.stencilLoadOp = gfx::LoadOp::CLEAR;
        depthStencilAttachment.stencilStoreOp = gfx::StoreOp::STORE;
        depthStencilAttachment.sampleCount = 1;
        depthStencilAttachment.beginLayout = gfx::TextureLayout::UNDEFINED;
        depthStencilAttachment.endLayout = gfx::TextureLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        _renderPass = _device->createRenderPass(renderPassInfo);
        gfx::FramebufferInfo fboInfo;
        fboInfo.colorTextures.resize(1);
        fboInfo.renderPass = _renderPass;
        _fbo = _device->createFramebuffer(fboInfo);
    }

    if (!_commandBuffers.size()) {
        _commandBuffers.push_back(_device->getCommandBuffer());
    }
}

void TestBaseI::destroyGlobal() {
    CC_SAFE_DESTROY(_fbo);
    CC_SAFE_DESTROY(_renderPass);
    CC_SAFE_DESTROY(_device);
}

unsigned char *TestBaseI::RGB2RGBA(Image *img) {
    int size = img->getWidth() * img->getHeight();
    const unsigned char *srcData = img->getData();
    unsigned char *dstData = new unsigned char[size * 4];
    for (int i = 0; i < size; i++) {
        dstData[i * 4] = srcData[i * 3];
        dstData[i * 4 + 1] = srcData[i * 3 + 1];
        dstData[i * 4 + 2] = srcData[i * 3 + 2];
        dstData[i * 4 + 3] = 255u;
    }
    return dstData;
}

void TestBaseI::modifyProjectionBasedOnDevice(Mat4 &projection, bool isOffscreen) {
    float minZ = _device->getClipSpaceMinZ();
    float signY = _device->getScreenSpaceSignY() * (isOffscreen ? _device->getUVSpaceSignY() : 1);

    Mat4 trans, scale;
    Mat4::createTranslation(0.0f, 0.0f, 1.0f + minZ, &trans);
    Mat4::createScale(1.0f, signY, 0.5f - 0.5f * minZ, &scale);
    projection = scale * trans * projection;
}

float TestBaseI::getViewportTopBasedOnDevice(float top, float height) {
    float s = _device->getScreenSpaceSignY();
    if (s > 0)
        return top;
    else
        return 1.0f - top - height;
}

uint TestBaseI::getMipmapLevelCounts(uint width, uint height) {
    return static_cast<uint>(std::floor(std::log2(std::max(width, height)))) + 1;
}

uint TestBaseI::getUBOSize(uint size) {
    return (size + 15) / 16 * 16;
}

ShaderSource &TestBaseI::getAppropriateShaderSource(gfx::Device *device, ShaderSources &sources) {
    switch (device->getGfxAPI()) {
        case gfx::API::GLES2:
            return sources.glsl1;
        case gfx::API::GLES3:
            return sources.glsl3;
        case gfx::API::METAL:
        case gfx::API::VULKAN:
            return sources.glsl4;
        default: break;
    }
    return sources.glsl4;
}

uint TestBaseI::getAlignedUBOStride(gfx::Device *device, uint stride) {
    uint alignment = device->getUboOffsetAlignment();
    return (stride + alignment - 1) / alignment * alignment;
}

} // namespace cc
