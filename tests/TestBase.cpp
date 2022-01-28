#include "TestBase.h"

#include "base/AutoreleasePool.h"
#include "base/threading/MessageQueue.h"
#include "platform/FileUtils.h"

#include "gfx-agent/DeviceAgent.h"

#include "bindings/jswrapper/SeApi.h"
#include "bindings/manual/jsb_classtype.h"
#include "bindings/manual/jsb_global_init.h"

#include "tests/BasicMipMapTest.h"
#include "tests/BasicTextureTest.h"
#include "tests/BasicTriangleTest.h"
#include "tests/BlendTest.h"
#include "tests/ClearScreenTest.h"
#include "tests/ComputeTest.h"
#include "tests/DepthTest.h"
#include "tests/FrameGraphTest.h"
#include "tests/ParticleTest.h"
#include "tests/ScriptTest/ScriptTest.h"
#include "tests/StencilTest.h"
#include "tests/StressTest.h"
#include "tests/SubpassTest.h"

#include "utils/Math.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#define DEFAULT_MATRIX_MATH

#include "GFXDeviceManager.h"
#include "bindings/event/CustomEventTypes.h"
#include "bindings/event/EventDispatcher.h"

namespace cc {

vector<WindowInfo> TestBaseI::windowInfos;

int         TestBaseI::curTestIndex           = -1;
int         TestBaseI::nextDirection          = 0;
TestBaseI * TestBaseI::test                   = nullptr;
const bool  TestBaseI::MANUAL_BARRIER         = true;
const float TestBaseI::NANOSECONDS_PER_SECOND = 1000000000.F;

gfx::Device *    TestBaseI::device     = nullptr;
gfx::RenderPass *TestBaseI::renderPass = nullptr;

vector<TestBaseI::createFunc> TestBaseI::tests = {
    BasicTriangle::create,
    SubpassTest::create,
    ScriptTest::create,
    ComputeTest::create,
    StressTest::create,
    FrameGraphTest::create,
    ClearScreen::create,
    BlendTest::create,
    ParticleTest::create,
// Need to fix lib jpeg on iOS
#if CC_PLATFORM != CC_PLATFORM_MAC_IOS
    DepthTexture::create,
    BasicMipMap::create,
    BasicTexture::create,
    StencilTest::create,
#endif // CC_PLATFORM != CC_PLATFORM_MAC_IOS
};

FrameRate                    TestBaseI::logicThread;
FrameRate                    TestBaseI::renderThread;
FrameRate                    TestBaseI::deviceThread;
vector<gfx::CommandBuffer *> TestBaseI::commandBuffers;
vector<gfx::Swapchain *>     TestBaseI::swapchains;
vector<gfx::Framebuffer *>   TestBaseI::fbos;

framegraph::FrameGraph TestBaseI::fg;

TestBaseI::TestBaseI() {
    if (!device) {
        EventDispatcher::init();

        se::ScriptEngine *se = se::ScriptEngine::getInstance();

        jsb_set_xxtea_key("");
        jsb_init_file_operation_delegate();

#if defined(CC_DEBUG) && (CC_DEBUG > 0)
        // Enable debugger here
        jsb_enable_debugger("0.0.0.0", 6086, false);
#endif

        se->setExceptionCallback([](const char *location, const char *message, const char *stack) {
            // Send exception information to server like Tencent Bugly.
            CC_LOG_ERROR("\nUncaught Exception:\n - location :  %s\n - msg : %s\n - detail : \n      %s\n", location, message, stack);
        });

        se->start();

        gfx::DeviceInfo deviceInfo;
        device = gfx::DeviceManager::create(deviceInfo);

        for (size_t i = 0; i < windowInfos.size(); ++i) {
            const auto &       info = windowInfos[i];
            gfx::SwapchainInfo swapchainInfo;
            swapchainInfo.windowHandle = info.windowHandle;
            swapchainInfo.width        = info.screen.width;
            swapchainInfo.height       = info.screen.height;
            swapchains.push_back(device->createSwapchain(swapchainInfo));

            if (!renderPass) {
                gfx::RenderPassInfo renderPassInfo;
                renderPassInfo.colorAttachments.emplace_back().format = swapchains[0]->getColorTexture()->getFormat();
                renderPassInfo.depthStencilAttachment.format          = swapchains[0]->getDepthStencilTexture()->getFormat();
                renderPass                                            = device->createRenderPass(renderPassInfo);
            }

            gfx::FramebufferInfo fboInfo;
            fboInfo.colorTextures.push_back(swapchains[i]->getColorTexture());
            fboInfo.depthStencilTexture = swapchains[i]->getDepthStencilTexture();
            fboInfo.renderPass          = renderPass;
            fbos.push_back(device->createFramebuffer(fboInfo));
        }
        EventDispatcher::addCustomEventListener(EVENT_DESTROY_WINDOW, [](const CustomEvent &e) -> void {
            for (auto *swapchain : swapchains) {
                if (e.args->ptrVal == swapchain->getWindowHandle()) {
                    swapchain->destroySurface();
                }
            }
        });
        EventDispatcher::addCustomEventListener(EVENT_RECREATE_WINDOW, [](const CustomEvent &e) -> void {
            for (auto *swapchain : swapchains) {
                if (!swapchain->getWindowHandle()) {
                    swapchain->createSurface(e.args->ptrVal);
                }
            }
        });

        CC_LOG_INFO(vmath::processorFeatures().c_str());

        commandBuffers.push_back(device->getCommandBuffer());
    }
}

void TestBaseI::tickScript() {
    EventDispatcher::dispatchTickEvent(0.F);
}

void TestBaseI::destroyGlobal() {
    CC_SAFE_DESTROY(test)
    framegraph::FrameGraph::gc(0);

    se::ScriptEngine::destroyInstance();

    for (auto *fbo : fbos) {
        CC_SAFE_DESTROY(fbo)
    }
    CC_SAFE_DESTROY(renderPass)

    for (auto *swapchain : swapchains) {
        CC_SAFE_DESTROY(swapchain)
    }

    CC_SAFE_DESTROY(device)
}

void TestBaseI::nextTest(bool backward) {
    nextDirection = backward ? -1 : 1;
}

void TestBaseI::toggleMultithread() {
    static bool multithreaded = true;
    auto *      deviceAgent   = gfx::DeviceAgent::getInstance();
    if (deviceAgent) {
        multithreaded = !multithreaded;
        deviceAgent->setMultithreaded(multithreaded);
    } else {
        multithreaded = false;
    }
}

void TestBaseI::onTouchEnd() {
    nextTest();
    // toggleMultithread();
}

void TestBaseI::update() {
    if (nextDirection) {
        CC_SAFE_DESTROY(test)
        static auto testCount = static_cast<int>(tests.size());
        if (nextDirection < 0) curTestIndex += testCount;
        curTestIndex  = (curTestIndex + nextDirection) % testCount;
        test          = tests[curTestIndex]();
        nextDirection = 0;
    }
    if (test) {
        test->tick();
    }
}

void TestBaseI::evalString(const String &code) {
    se::AutoHandleScope hs;
    se::ScriptEngine::getInstance()->evalString(code.c_str());
}

void TestBaseI::runScript(const String &file) {
    se::AutoHandleScope hs;
    se::ScriptEngine::getInstance()->runScript(file);
}

void TestBaseI::scriptEngineGC() {
    se::ScriptEngine::getInstance()->addAfterCleanupHook([]() {
        JSBClassType::cleanup();
    });
    se::ScriptEngine::getInstance()->init(); // cleanup & restart

    cc::EventDispatcher::destroy();
    cc::EventDispatcher::init();
}

unsigned char *TestBaseI::rgb2rgba(Image *img) {
    int                  size    = img->getWidth() * img->getHeight();
    const unsigned char *srcData = img->getData();
    auto *               dstData = new unsigned char[size * 4];
    for (int i = 0; i < size; i++) {
        dstData[i * 4]     = srcData[i * 3];
        dstData[i * 4 + 1] = srcData[i * 3 + 1];
        dstData[i * 4 + 2] = srcData[i * 3 + 2];
        dstData[i * 4 + 3] = 255U;
    }
    return dstData;
}

gfx::Texture *TestBaseI::createTextureWithFile(const gfx::TextureInfo &partialInfo, const String &imageFile) {
    auto *img = new cc::Image();
    img->autorelease();
    if (!img->initWithImageFile(imageFile)) return nullptr;

    unsigned char *imgData = nullptr;
    if (img->getRenderFormat() == gfx::Format::RGB8) {
        imgData = TestBaseI::rgb2rgba(img);
    } else {
        img->takeData(&imgData);
    }

    gfx::TextureInfo textureInfo = partialInfo;
    textureInfo.width            = img->getWidth();
    textureInfo.height           = img->getHeight();
    textureInfo.format           = gfx::Format::RGBA8;
    if (hasFlag(textureInfo.flags, gfx::TextureFlagBit::GEN_MIPMAP)) {
        textureInfo.levelCount = TestBaseI::getMipmapLevelCounts(textureInfo.width, textureInfo.height);
    }
    auto *texture = device->createTexture(textureInfo);

    gfx::BufferTextureCopy textureRegion;
    textureRegion.texExtent.width  = img->getWidth();
    textureRegion.texExtent.height = img->getHeight();
    device->copyBuffersToTexture({imgData}, texture, {textureRegion});
    delete[] imgData;
    return texture;
}

void TestBaseI::modifyProjectionBasedOnDevice(Mat4 *projection, gfx::Swapchain *swapchain) {
    float minZ        = device->getCapabilities().clipSpaceMinZ;
    float signY       = device->getCapabilities().clipSpaceSignY;
    auto  orientation = static_cast<float>(swapchain->getSurfaceTransform());

    Mat4 trans;
    Mat4 scale;
    Mat4 rot;
    Mat4::createTranslation(0.0F, 0.0F, 1.0F + minZ, &trans);
    Mat4::createScale(1.0F, signY, 0.5F - 0.5F * minZ, &scale);
    Mat4::createRotationZ(orientation * math::PI_DIV2, &rot);
    *projection = rot * scale * trans * (*projection);
}

#ifndef DEFAULT_MATRIX_MATH
constexpr float PRE_TRANSFORMS[4][4] = {
    {1, 0, 0, 1},   // GFXSurfaceTransform.IDENTITY
    {0, 1, -1, 0},  // GFXSurfaceTransform.ROTATE_90
    {-1, 0, 0, -1}, // GFXSurfaceTransform.ROTATE_180
    {0, -1, 1, 0},  // GFXSurfaceTransform.ROTATE_270
};
#endif

void TestBaseI::createOrthographic(float left, float right, float bottom, float top, float zNear, float zFar, Mat4 *dst, gfx::Swapchain *swapchain) {
#ifdef DEFAULT_MATRIX_MATH
    Mat4::createOrthographic(left, right, bottom, top, zNear, zFar, dst);
    TestBaseI::modifyProjectionBasedOnDevice(dst, swapchain);
#else
    float                 minZ         = device->getCapabilities().clipSpaceMinZ;
    float                 signY        = device->getCapabilities().clipSpaceSignY;
    gfx::SurfaceTransform orientation  = swapchain->getSurfaceTransform();
    const float *         preTransform = PRE_TRANSFORMS[static_cast<uint>(orientation)];

    memset(dst->m, 0, 16 * sizeof(float));

    float x  = 2.F / (right - left);
    float y  = 2.F / (top - bottom) * signY;
    float dx = (left + right) / (left - right);
    float dy = (bottom + top) / (bottom - top) * signY;

    dst->m[0]                          = x * preTransform[0];
    dst->m[1]                          = x * preTransform[1];
    dst->m[4]                          = y * preTransform[2];
    dst->m[5]                          = y * preTransform[3];
    dst->m[10]                         = (1.0F - minZ) / (zNear - zFar);
    dst->m[12]                         = dx * preTransform[0] + dy * preTransform[2];
    dst->m[13]                         = dx * preTransform[1] + dy * preTransform[3];
    dst->m[14]                         = (zNear - minZ * zFar) / (zNear - zFar);
    dst->m[15]                         = 1.0F;
#endif
}

void TestBaseI::createPerspective(float fov, float aspect, float zNear, float zFar, Mat4 *dst, gfx::Swapchain *swapchain) {
#ifdef DEFAULT_MATRIX_MATH
    Mat4::createPerspective(MATH_DEG_TO_RAD(fov), aspect, zNear, zFar, dst);
    TestBaseI::modifyProjectionBasedOnDevice(dst, swapchain);
#else
    float                 minZ         = device->getCapabilities().clipSpaceMinZ;
    float                 signY        = device->getCapabilities().clipSpaceSignY;
    gfx::SurfaceTransform orientation  = swapchain->getSurfaceTransform();
    const float *         preTransform = PRE_TRANSFORMS[static_cast<uint>(orientation)];

    memset(dst->m, 0, 16 * sizeof(float));

    float f  = 1.0F / std::tan(MATH_DEG_TO_RAD(fov * 0.5F));
    float nf = 1.0F / (zNear - zFar);

    float x = f / aspect;
    float y = f * signY;

    dst->m[0]  = x * preTransform[0];
    dst->m[1]  = x * preTransform[1];
    dst->m[4]  = y * preTransform[2];
    dst->m[5]  = y * preTransform[3];
    dst->m[10] = (zFar - minZ * zNear) * nf;
    dst->m[11] = -1.0F;
    dst->m[14] = zFar * zNear * nf * (1.0F - minZ);
#endif
}

gfx::Extent TestBaseI::getOrientedSurfaceSize(gfx::Swapchain *swapchain) {
    switch (swapchain->getSurfaceTransform()) {
        case gfx::SurfaceTransform::ROTATE_90:
        case gfx::SurfaceTransform::ROTATE_270:
            return {swapchain->getHeight(), swapchain->getWidth()};
        case gfx::SurfaceTransform::IDENTITY:
        case gfx::SurfaceTransform::ROTATE_180:
        default:
            return {swapchain->getWidth(), swapchain->getHeight()};
    }
}

gfx::Viewport TestBaseI::getViewportBasedOnDevice(const Vec4 &relativeArea, gfx::Swapchain *swapchain) {
    float x = relativeArea.x;
    float y = device->getCapabilities().clipSpaceSignY < 0.0F ? 1.F - relativeArea.y - relativeArea.w : relativeArea.y;
    float w = relativeArea.z;
    float h = relativeArea.w;

    gfx::Viewport viewport;

    auto deviceWidth  = static_cast<float>(swapchain->getWidth());
    auto deviceHeight = static_cast<float>(swapchain->getHeight());

    switch (swapchain->getSurfaceTransform()) {
        case gfx::SurfaceTransform::ROTATE_90:
            viewport.left   = static_cast<int>((1.F - y - h) * deviceWidth);
            viewport.top    = static_cast<int>(x * deviceHeight);
            viewport.width  = static_cast<uint>(h * deviceWidth);
            viewport.height = static_cast<uint>(w * deviceHeight);
            break;
        case gfx::SurfaceTransform::ROTATE_180:
            viewport.left   = static_cast<int>((1.F - x - w) * deviceWidth);
            viewport.top    = static_cast<int>((1.F - y - h) * deviceHeight);
            viewport.width  = static_cast<uint>(w * deviceWidth);
            viewport.height = static_cast<uint>(h * deviceHeight);
            break;
        case gfx::SurfaceTransform::ROTATE_270:
            viewport.left   = static_cast<int>(y * deviceWidth);
            viewport.top    = static_cast<int>((1.F - x - w) * deviceHeight);
            viewport.width  = static_cast<uint>(h * deviceWidth);
            viewport.height = static_cast<uint>(w * deviceHeight);
            break;
        case gfx::SurfaceTransform::IDENTITY:
            viewport.left   = static_cast<int>(x * deviceWidth);
            viewport.top    = static_cast<int>(y * deviceHeight);
            viewport.width  = static_cast<uint>(w * deviceWidth);
            viewport.height = static_cast<uint>(h * deviceHeight);
            break;
    }

    return viewport;
}

uint TestBaseI::getMipmapLevelCounts(uint width, uint height) {
    return static_cast<uint>(std::floor(std::log2(std::max(width, height)))) + 1;
}

uint TestBaseI::getUBOSize(uint size) {
    return (size + 15) / 16 * 16; // https://bugs.chromium.org/p/chromium/issues/detail?id=988988
}

uint TestBaseI::getAlignedUBOStride(uint stride) {
    uint alignment = device->getCapabilities().uboOffsetAlignment;
    return (stride + alignment - 1) / alignment * alignment;
}

void TestBaseI::createUberBuffer(const vector<uint> &sizes, gfx::Buffer **pBuffer, vector<gfx::Buffer *> *pBufferViews,
                                 vector<uint> *pBufferViewOffsets, vector<uint> *pAlignedBufferSizes, uint instances) {
    pBufferViewOffsets->assign(sizes.size() + 1, 0);
    if (pAlignedBufferSizes) pAlignedBufferSizes->resize(sizes.size());
    for (uint i = 0U; i < sizes.size(); ++i) {
        uint alignedSize = getAlignedUBOStride(sizes[i]);
        if (pAlignedBufferSizes) pAlignedBufferSizes->at(i) = alignedSize;
        pBufferViewOffsets->at(i + 1) = pBufferViewOffsets->at(i) + alignedSize * instances;
    }
    *pBuffer = device->createBuffer({
        gfx::BufferUsage::UNIFORM,
        gfx::MemoryUsage::DEVICE,
        TestBaseI::getUBOSize(pBufferViewOffsets->back()),
    });
    for (uint i = 0U; i < sizes.size(); ++i) {
        pBufferViews->push_back(device->createBuffer({
            *pBuffer,
            pBufferViewOffsets->at(i),
            sizes[i],
        }));
    }
}

tinyobj::ObjReader TestBaseI::loadOBJ(const String & /*path*/) {
    String objFile = FileUtils::getInstance()->getStringFromFile("bunny.obj");
    String mtlFile;

    tinyobj::ObjReader       obj;
    tinyobj::ObjReaderConfig config;
    config.vertex_color = false;
    obj.ParseFromString(objFile, mtlFile, config);
    CCASSERT(obj.Valid(), "file failed to load");

    return obj;
}

} // namespace cc
