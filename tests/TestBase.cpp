#include "TestBase.h"

#include "gfx-base/GFXDef-common.h"
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
#include "tests/DeferredTest.h"

#include "cocos/base/AutoreleasePool.h"
#include "cocos/bindings/auto/jsb_gfx_auto.h"
#include "cocos/bindings/dop/jsb_dop.h"
#include "cocos/bindings/event/CustomEventTypes.h"
#include "cocos/bindings/event/EventDispatcher.h"
#include "cocos/bindings/jswrapper/SeApi.h"
#include "cocos/bindings/manual/jsb_classtype.h"
#include "cocos/bindings/manual/jsb_gfx_manual.h"
#include "cocos/bindings/manual/jsb_global_init.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#define DEFAULT_MATRIX_MATH

//#undef CC_USE_VULKAN
//#undef CC_USE_GLES3
//#undef CC_USE_GLES2
#include "renderer/GFXDeviceManager.h"

namespace cc {

WindowInfo TestBaseI::windowInfo;

int               TestBaseI::curTestIndex           = -1;
int               TestBaseI::nextDirection          = 0;
TestBaseI *       TestBaseI::test                   = nullptr;
const bool        TestBaseI::MANUAL_BARRIER         = true;
const uint        TestBaseI::NANOSECONDS_PER_SECOND = 1000000000;
gfx::Device *     TestBaseI::device                 = nullptr;
gfx::Framebuffer *TestBaseI::fbo                    = nullptr;
gfx::RenderPass * TestBaseI::renderPass             = nullptr;

std::vector<TestBaseI::createFunc> TestBaseI::tests = {
    ScriptTest::create,
    SubpassTest::create,
    DeferredTest::create,
    ComputeTest::create,
    FrameGraphTest::create,
    StressTest::create,
    ClearScreen::create,
    BasicTriangle::create,
    DepthTexture::create,
    BlendTest::create,
    ParticleTest::create,
// Need to fix lib jpeg on iOS
#if CC_PLATFORM != CC_PLATFORM_MAC_IOS
    BasicTexture::create,
    StencilTest::create,
#endif // CC_PLATFORM != CC_PLATFORM_MAC_IOS
};

FrameRate                                       TestBaseI::hostThread;
FrameRate                                       TestBaseI::deviceThread;
std::vector<gfx::CommandBuffer *>               TestBaseI::commandBuffers;
std::unordered_map<uint, gfx::GlobalBarrier *>  TestBaseI::globalBarrierMap;
std::unordered_map<uint, gfx::TextureBarrier *> TestBaseI::textureBarrierMap;

framegraph::FrameGraph TestBaseI::fg;
framegraph::Texture    TestBaseI::fgBackBuffer;
framegraph::Texture    TestBaseI::fgDepthStencilBackBuffer;

TestBaseI::TestBaseI(const WindowInfo &info) {
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

        se->addBeforeInitHook([]() {
            JSBClassType::init();
        });

        se->addBeforeCleanupHook([se]() {
            se->garbageCollect();
        });

        se->addAfterCleanupHook([]() {
            JSBClassType::destroy();
        });

        se->addRegisterCallback(register_all_dop_bindings);

        se->start();

        gfx::DeviceInfo deviceInfo;
        // deviceInfo.isAntiAlias  = true;
        deviceInfo.windowHandle = info.windowHandle;
        deviceInfo.width        = info.screen.width;
        deviceInfo.height       = info.screen.height;
        deviceInfo.nativeWidth  = info.physicalWidth;
        deviceInfo.nativeHeight = info.physicalHeight;

        device = gfx::DeviceManager::create(deviceInfo);
    }

    if (!renderPass) {
        gfx::RenderPassInfo  renderPassInfo;
        gfx::ColorAttachment colorAttachment;
        colorAttachment.format = device->getColorFormat();
        renderPassInfo.colorAttachments.emplace_back(colorAttachment);

        gfx::DepthStencilAttachment &depthStencilAttachment = renderPassInfo.depthStencilAttachment;
        depthStencilAttachment.format                       = device->getDepthStencilFormat();

        renderPass = device->createRenderPass(renderPassInfo);
    }

    if (!fbo) {
        gfx::FramebufferInfo fboInfo;
        fboInfo.colorTextures.resize(1);
        fboInfo.renderPass = renderPass;
        fbo                = device->createFramebuffer(fboInfo);
    }

    if (commandBuffers.empty()) {
        commandBuffers.push_back(device->getCommandBuffer());
    }

    hostThread.prevTime   = std::chrono::steady_clock::now();
    deviceThread.prevTime = std::chrono::steady_clock::now();
}

void TestBaseI::tickScript() {
    EventDispatcher::dispatchTickEvent(0.F);
}

void TestBaseI::destroyGlobal() {
    CC_SAFE_DESTROY(test)
    CC_SAFE_DESTROY(fbo)
    CC_SAFE_DESTROY(renderPass)
    framegraph::FrameGraph::gc(0);

    for (auto textureBarrier : textureBarrierMap) {
        CC_SAFE_DELETE(textureBarrier.second)
    }
    textureBarrierMap.clear();

    for (auto globalBarrier : globalBarrierMap) {
        CC_SAFE_DELETE(globalBarrier.second)
    }
    globalBarrierMap.clear();

    se::ScriptEngine::getInstance()->cleanup();

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
        if (nextDirection < 0) curTestIndex += tests.size();
        curTestIndex  = (curTestIndex + nextDirection) % static_cast<int>(tests.size());
        test          = tests[curTestIndex](windowInfo);
        nextDirection = 0;
    }
    if (test) {
        test->tick();
    }
}

void TestBaseI::evalString(const std::string &code) {
    se::AutoHandleScope hs;
    se::ScriptEngine::getInstance()->evalString(code.c_str());
}

void TestBaseI::runScript(const std::string &file) {
    se::AutoHandleScope hs;
    se::ScriptEngine::getInstance()->runScript(file);
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

gfx::Texture *TestBaseI::createTextureWithFile(const gfx::TextureInfo &partialInfo, const std::string &imageFile) {
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
    textureInfo.width  = img->getWidth();
    textureInfo.height = img->getHeight();
    textureInfo.format = gfx::Format::RGBA8;
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

void TestBaseI::modifyProjectionBasedOnDevice(Mat4 *projection) {
    float minZ        = device->getCapabilities().clipSpaceMinZ;
    float signY       = device->getCapabilities().clipSpaceSignY;
    auto  orientation = static_cast<float>(device->getSurfaceTransform());

    Mat4 trans;
    Mat4 scale;
    Mat4 rot;
    Mat4::createTranslation(0.0F, 0.0F, 1.0F + minZ, &trans);
    Mat4::createScale(1.0F, signY, 0.5F - 0.5F * minZ, &scale);
    Mat4::createRotationZ(orientation * MATH_PIOVER2, &rot);
    *projection = rot * scale * trans * (*projection);
}

#ifndef DEFAULT_MATRIX_MATH
constexpr float preTransforms[4][4] = {
    {1, 0, 0, 1},   // GFXSurfaceTransform.IDENTITY
    {0, 1, -1, 0},  // GFXSurfaceTransform.ROTATE_90
    {-1, 0, 0, -1}, // GFXSurfaceTransform.ROTATE_180
    {0, -1, 1, 0},  // GFXSurfaceTransform.ROTATE_270
};
#endif

void TestBaseI::createOrthographic(float left, float right, float bottom, float top, float zNear, float zFar, Mat4 *dst) {
#ifdef DEFAULT_MATRIX_MATH
    Mat4::createOrthographic(left, right, bottom, top, zNear, zFar, dst);
    TestBaseI::modifyProjectionBasedOnDevice(dst);
#else
    float                 minZ         = device->getCapabilities().clipSpaceMinZ;
    float                 signY        = device->getCapabilities().clipSpaceSignY;
    gfx::SurfaceTransform orientation  = device->getSurfaceTransform();
    const float *         preTransform = preTransforms[(uint)orientation];

    memset(dst, 0, MATRIX_SIZE);

    float x  = 2.f / (right - left);
    float y  = 2.f / (top - bottom) * signY;
    float dx = (left + right) / (left - right);
    float dy = (bottom + top) / (bottom - top) * signY;

    dst->m[0]  = x * preTransform[0];
    dst->m[1]  = x * preTransform[1];
    dst->m[4]  = y * preTransform[2];
    dst->m[5]  = y * preTransform[3];
    dst->m[10] = (1.0f - minZ) / (ZNear - ZFar);
    dst->m[12] = dx * preTransform[0] + dy * preTransform[2];
    dst->m[13] = dx * preTransform[1] + dy * preTransform[3];
    dst->m[14] = (ZNear - minZ * ZFar) / (ZNear - ZFar);
    dst->m[15] = 1.0f;
#endif
}

void TestBaseI::createPerspective(float fov, float aspect, float zNear, float zFar, Mat4 *dst) {
#ifdef DEFAULT_MATRIX_MATH
    Mat4::createPerspective(MATH_DEG_TO_RAD(fov), aspect, zNear, zFar, dst);
    TestBaseI::modifyProjectionBasedOnDevice(dst);
#else
    float                 minZ         = device->getCapabilities().clipSpaceMinZ;
    float                 signY        = device->getCapabilities().clipSpaceSignY;
    gfx::SurfaceTransform orientation  = device->getSurfaceTransform();
    const float *         preTransform = preTransforms[(uint)orientation];

    memset(dst, 0, MATRIX_SIZE);

    float f  = 1.0f / std::tan(MATH_DEG_TO_RAD(fov * 0.5f));
    float nf = 1.0f / (zNear - ZFar);

    float x = f / aspect;
    float y = f * signY;

    dst->m[0]  = x * preTransform[0];
    dst->m[1]  = x * preTransform[1];
    dst->m[4]  = y * preTransform[2];
    dst->m[5]  = y * preTransform[3];
    dst->m[10] = (ZFar - minZ * zNear) * nf;
    dst->m[11] = -1.0f;
    dst->m[14] = ZFar * zNear * nf * (1.0f - minZ);
#endif
}

gfx::Extent TestBaseI::getOrientedSurfaceSize() {
    switch (device->getSurfaceTransform()) {
        case gfx::SurfaceTransform::ROTATE_90:
        case gfx::SurfaceTransform::ROTATE_270:
            return {device->getHeight(), device->getWidth()};
        case gfx::SurfaceTransform::IDENTITY:
        case gfx::SurfaceTransform::ROTATE_180:
        default:
            return {device->getWidth(), device->getHeight()};
    }
}

gfx::Viewport TestBaseI::getViewportBasedOnDevice(const Vec4 &relativeArea) {
    float x = relativeArea.x;
    float y = device->getCapabilities().screenSpaceSignY < 0.0F ? 1.F - relativeArea.y - relativeArea.w : relativeArea.y;
    float w = relativeArea.z;
    float h = relativeArea.w;

    gfx::Extent   size{device->getWidth(), device->getHeight()};
    gfx::Viewport viewport;

    switch (device->getSurfaceTransform()) {
        case gfx::SurfaceTransform::ROTATE_90:
            viewport.left   = int((1.F - y - h) * size.width);
            viewport.top    = int(x * size.height);
            viewport.width  = uint(h * size.width);
            viewport.height = uint(w * size.height);
            break;
        case gfx::SurfaceTransform::ROTATE_180:
            viewport.left   = int((1.F - x - w) * size.width);
            viewport.top    = int((1.F - y - h) * size.height);
            viewport.width  = uint(w * size.width);
            viewport.height = uint(h * size.height);
            break;
        case gfx::SurfaceTransform::ROTATE_270:
            viewport.left   = int(y * size.width);
            viewport.top    = int((1.F - x - w) * size.height);
            viewport.width  = uint(h * size.width);
            viewport.height = uint(w * size.height);
            break;
        case gfx::SurfaceTransform::IDENTITY:
            viewport.left   = int(x * size.width);
            viewport.top    = int(y * size.height);
            viewport.width  = uint(w * size.width);
            viewport.height = uint(h * size.height);
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

void TestBaseI::createUberBuffer(const vector<uint> &sizes, gfx::Buffer **pBuffer,
                                 vector<gfx::Buffer *> *pBufferViews, vector<uint> *pBufferViewOffsets) {
    pBufferViewOffsets->assign(sizes.size() + 1, 0);
    for (uint i = 0U; i < sizes.size(); ++i) {
        uint alignedSize              = i == sizes.size() - 1 ? sizes[i] : getAlignedUBOStride(sizes[i]);
        pBufferViewOffsets->at(i + 1) = pBufferViewOffsets->at(i) + alignedSize;
    }
    *pBuffer = device->createBuffer({
        gfx::BufferUsage::UNIFORM,
        gfx::MemoryUsage::DEVICE | gfx::MemoryUsage::HOST,
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

gfx::GlobalBarrier *TestBaseI::getGlobalBarrier(const gfx::GlobalBarrierInfo &info) {
    uint hash = gfx::GlobalBarrier::computeHash(info);
    if (!globalBarrierMap.count(hash)) {
        globalBarrierMap[hash] = device->createGlobalBarrier(info);
    }
    return globalBarrierMap[hash];
}

gfx::TextureBarrier *TestBaseI::getTextureBarrier(const gfx::TextureBarrierInfo &info) {
    uint hash = gfx::TextureBarrier::computeHash(info);
    if (!textureBarrierMap.count(hash)) {
        textureBarrierMap[hash] = device->createTextureBarrier(info);
    }
    return textureBarrierMap[hash];
}

} // namespace cc
