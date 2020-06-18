#include "TestBase.h"

#if defined(USE_METAL)
#include "gfx-metal/GFXMTL.h"
#elif defined(USE_VULKAN)
#include "gfx-vulkan/GFXVulkan.h"
#elif defined(USE_GLES2)
#include "gfx-gles2/GFXGLES2.h"
#else
#include "gfx-gles3/GFXGLES3.h"
#endif

namespace cc {

    gfx::GFXDevice *TestBaseI::_device = nullptr;
    gfx::GFXFramebuffer *TestBaseI::_fbo = nullptr;
    gfx::GFXRenderPass *TestBaseI::_renderPass = nullptr;
    std::vector<gfx::GFXCommandBuffer *> TestBaseI::_commandBuffers(1, nullptr);

    TestBaseI::TestBaseI(const WindowInfo &info) {
        if (_device == nullptr) {
#if defined(USE_METAL)
            _device = CC_NEW(gfx::CCMTLDevice);
#else

#if defined(USE_VULKAN)
            _device = CC_NEW(gfx::CCVKDevice);
#elif defined(USE_GLES2)
            _device = CC_NEW(gfx::GLES2Device);
#else
            _device = CC_NEW(gfx::GLES3Device);
#endif

#endif // (CC_PLATFORM == CC_PLATFORM_MAC_OSX)

            gfx::GFXDeviceInfo dev_info;
            dev_info.windowHandle = info.windowHandle;
            dev_info.width = info.screen.width;
            dev_info.height = info.screen.height;
            dev_info.nativeWidth = info.physicalWidth;
            dev_info.nativeHeight = info.physicalHeight;
            _device->initialize(dev_info);
        }
        if (_fbo == nullptr) {
            gfx::GFXRenderPassInfo renderPassInfo;
            gfx::GFXColorAttachment colorAttachment;
            colorAttachment.format = _device->getColorFormat();
            colorAttachment.loadOp = gfx::GFXLoadOp::CLEAR;
            colorAttachment.storeOp = gfx::GFXStoreOp::STORE;
            colorAttachment.sampleCount = 1;
            colorAttachment.beginLayout = gfx::GFXTextureLayout::UNDEFINED;
            colorAttachment.endLayout = gfx::GFXTextureLayout::PRESENT_SRC;
            renderPassInfo.colorAttachments.emplace_back(colorAttachment);

            gfx::GFXDepthStencilAttachment &depthStencilAttachment = renderPassInfo.depthStencilAttachment;
            depthStencilAttachment.format = _device->getDepthStencilFormat();
            depthStencilAttachment.depthLoadOp = gfx::GFXLoadOp::CLEAR;
            depthStencilAttachment.depthStoreOp = gfx::GFXStoreOp::STORE;
            depthStencilAttachment.stencilLoadOp = gfx::GFXLoadOp::CLEAR;
            depthStencilAttachment.stencilStoreOp = gfx::GFXStoreOp::STORE;
            depthStencilAttachment.sampleCount = 1;
            depthStencilAttachment.beginLayout = gfx::GFXTextureLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            depthStencilAttachment.endLayout = gfx::GFXTextureLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            _renderPass = _device->createRenderPass(renderPassInfo);
            gfx::GFXFramebufferInfo fboInfo;
            fboInfo.renderPass = _renderPass;
            fboInfo.isOffscreen = false;
            _fbo = _device->createFramebuffer(fboInfo);
        }

        for (uint i = 0; i < static_cast<uint>(_commandBuffers.size()); i++) {
            if (_commandBuffers[i] != nullptr)
                continue;

            gfx::GFXCommandBufferInfo cmdBuffInfo;
            cmdBuffInfo.allocator = _device->getCommandAllocator();
            cmdBuffInfo.type = gfx::GFXCommandBufferType::PRIMARY;
            _commandBuffers[i] = _device->createCommandBuffer(cmdBuffInfo);
        }
    }

    void TestBaseI::destroyGlobal() {
        for (auto &cmdBuff : _commandBuffers) {
            CC_SAFE_DESTROY(cmdBuff);
        }
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

    void TestBaseI::modifyProjectionBasedOnDevice(Mat4 &projection) {
        Mat4 trans, scale;
        Mat4::createTranslation(0.0f, 0.0f, 1.0f + _device->getMinClipZ(), &trans);
        Mat4::createScale(1.0f, _device->getProjectionSignY(),
            0.5f - 0.5f * _device->getMinClipZ(), &scale);
        projection = scale * trans * projection;
    }

    float TestBaseI::getViewportTopBasedOnDevice(float top, float height) {
        float s = _device->getProjectionSignY();
        if (s > 0) return top;
        else return 1.0f - top - height;
    }

    uint TestBaseI::getMipmapLevelCounts(uint width, uint height) {
        return static_cast<uint>(std::floor(std::log2(std::max(width, height)))) + 1;
    }

} // namespace cc
