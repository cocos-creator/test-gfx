#include "TestBase.h"

//#undef USE_METAL

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

#define DEFAULT_MATRIX_MATH

namespace cc {

gfx::Device *TestBaseI::_device = nullptr;
gfx::Framebuffer *TestBaseI::_fbo = nullptr;
gfx::RenderPass *TestBaseI::_renderPass = nullptr;
std::vector<gfx::CommandBuffer *> TestBaseI::_commandBuffers;

FrameRate TestBaseI::hostThread;
FrameRate TestBaseI::deviceThread;

TestBaseI::TestBaseI(const WindowInfo &info)
{
    if (_device == nullptr) {
#if defined(USE_GLES2)
        _device = CC_NEW(gfx::DeviceProxy(CC_NEW(gfx::GLES2Device), nullptr));
#elif defined(USE_GLES3)
        _device = CC_NEW(gfx::DeviceProxy(CC_NEW(gfx::GLES3Device), nullptr));
#elif defined(USE_METAL)
        _device = CC_NEW(gfx::DeviceProxy(CC_NEW(gfx::CCMTLDevice), nullptr));
#else
        _device = CC_NEW(gfx::DeviceProxy(CC_NEW(gfx::CCVKDevice), nullptr));
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

    hostThread.prevTime = std::chrono::steady_clock::now();
    deviceThread.prevTime = std::chrono::steady_clock::now();
}

void TestBaseI::destroyGlobal() {
    CC_SAFE_DESTROY(_fbo);
    CC_SAFE_DESTROY(_renderPass);
    CC_SAFE_DESTROY(_device);
}

void TestBaseI::toggleMultithread() {
    static bool multithreaded = true;
    multithreaded = !multithreaded;
    ((gfx::DeviceProxy *)_device)->setMultithreaded(multithreaded);
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
    float orientation = (float)_device->getSurfaceTransform();

    Mat4 trans, scale, rot;
    Mat4::createTranslation(0.0f, 0.0f, 1.0f + minZ, &trans);
    Mat4::createScale(1.0f, signY, 0.5f - 0.5f * minZ, &scale);
    Mat4::createRotationZ(orientation * MATH_PIOVER2, &rot);
    projection = rot * scale * trans * projection;
}

constexpr float preTransforms[4][4] = {
    {1, 0, 0, 1},   // GFXSurfaceTransform.IDENTITY
    {0, 1, -1, 0},  // GFXSurfaceTransform.ROTATE_90
    {-1, 0, 0, -1}, // GFXSurfaceTransform.ROTATE_180
    {0, -1, 1, 0},  // GFXSurfaceTransform.ROTATE_270
};

void TestBaseI::createOrthographic(float left, float right, float bottom, float top, float ZNear, float ZFar, Mat4 *dst, bool isOffscreen) {
#ifdef DEFAULT_MATRIX_MATH
    Mat4::createOrthographic(left, right, bottom, top, ZNear, ZFar, dst);
    TestBaseI::modifyProjectionBasedOnDevice(*dst, isOffscreen);
#else
    float minZ = _device->getClipSpaceMinZ();
    float signY = _device->getScreenSpaceSignY() * (isOffscreen ? _device->getUVSpaceSignY() : 1);
    gfx::SurfaceTransform orientation = _device->getSurfaceTransform();
    const float *preTransform = preTransforms[(uint)orientation];

    memset(dst, 0, MATRIX_SIZE);

    float x = 2.f / (right - left);
    float y = 2.f / (top - bottom) * signY;
    float dx = (left + right) / (left - right);
    float dy = (bottom + top) / (bottom - top) * signY;

    dst->m[0] = x * preTransform[0];
    dst->m[1] = x * preTransform[1];
    dst->m[4] = y * preTransform[2];
    dst->m[5] = y * preTransform[3];
    dst->m[10] = (1.0f - minZ) / (ZNear - ZFar);
    dst->m[12] = dx * preTransform[0] + dy * preTransform[2];
    dst->m[13] = dx * preTransform[1] + dy * preTransform[3];
    dst->m[14] = (ZNear - minZ * ZFar) / (ZNear - ZFar);
    dst->m[15] = 1.0f;
#endif
}

void TestBaseI::createPerspective(float fov, float aspect, float zNear, float ZFar, Mat4 *dst, bool isOffscreen) {
#ifdef DEFAULT_MATRIX_MATH
    Mat4::createPerspective(MATH_DEG_TO_RAD(fov), aspect, zNear, ZFar, dst);
    TestBaseI::modifyProjectionBasedOnDevice(*dst, isOffscreen);
#else
    float minZ = _device->getClipSpaceMinZ();
    float signY = _device->getScreenSpaceSignY() * (isOffscreen ? _device->getUVSpaceSignY() : 1);
    gfx::SurfaceTransform orientation = _device->getSurfaceTransform();
    const float *preTransform = preTransforms[(uint)orientation];

    memset(dst, 0, MATRIX_SIZE);

    float f = 1.0f / std::tan(MATH_DEG_TO_RAD(fov * 0.5f));
    float nf = 1.0f / (zNear - ZFar);

    float x = f / aspect;
    float y = f * signY;

    dst->m[0] = x * preTransform[0];
    dst->m[1] = x * preTransform[1];
    dst->m[4] = y * preTransform[2];
    dst->m[5] = y * preTransform[3];
    dst->m[10] = (ZFar - minZ * zNear) * nf;
    dst->m[11] = -1.0f;
    dst->m[14] = ZFar * zNear * nf * (1.0f - minZ);
#endif
}

gfx::Extent TestBaseI::getOrientedSurfaceSize() {
    switch (_device->getSurfaceTransform()) {
        case gfx::SurfaceTransform::ROTATE_90:
        case gfx::SurfaceTransform::ROTATE_270:
            return {_device->getHeight(), _device->getWidth()};
        case gfx::SurfaceTransform::IDENTITY:
        case gfx::SurfaceTransform::ROTATE_180:
        default:
            return {_device->getWidth(), _device->getHeight()};
    }
}

gfx::Viewport TestBaseI::getViewportBasedOnDevice(const Vec4 &relativeArea) {
    float x = relativeArea.x;
    float y = _device->getScreenSpaceSignY() < 0.0f ? 1.f - relativeArea.y - relativeArea.w : relativeArea.y;
    float w = relativeArea.z;
    float h = relativeArea.w;

    gfx::Extent size{_device->getWidth(), _device->getHeight()};
    gfx::Viewport viewport;

    switch (_device->getSurfaceTransform()) {
        case gfx::SurfaceTransform::ROTATE_90:
            viewport.left = uint((1.f - y - h) * size.width);
            viewport.top = uint(x * size.height);
            viewport.width = uint(h * size.width);
            viewport.height = uint(w * size.height);
            break;
        case gfx::SurfaceTransform::ROTATE_180:
            viewport.left = uint((1.f - x - w) * size.width);
            viewport.top = uint((1.f - y - h) * size.height);
            viewport.width = uint(w * size.width);
            viewport.height = uint(h * size.height);
            break;
        case gfx::SurfaceTransform::ROTATE_270:
            viewport.left = uint(y * size.width);
            viewport.top = uint((1.f - x - w) * size.height);
            viewport.width = uint(h * size.width);
            viewport.height = uint(w * size.height);
            break;
        case gfx::SurfaceTransform::IDENTITY:
            viewport.left = uint(x * size.width);
            viewport.top = uint(y * size.height);
            viewport.width = uint(w * size.width);
            viewport.height = uint(h * size.height);
            break;
    }

    return viewport;
}

uint TestBaseI::getMipmapLevelCounts(uint width, uint height) {
    return static_cast<uint>(std::floor(std::log2(std::max(width, height)))) + 1;
}

uint TestBaseI::getUBOSize(uint size) {
    return (size + 15) / 16 * 16;
}

ShaderSource &TestBaseI::getAppropriateShaderSource(ShaderSources &sources) {
    switch (_device->getGfxAPI()) {
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
