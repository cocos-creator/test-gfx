#include "TestBase.h"

#include "tests/BasicTextureTest.h"
#include "tests/BasicTriangleTest.h"
#include "tests/BlendTest.h"
#include "tests/BunnyTest.h"
#include "tests/ClearScreenTest.h"
#include "tests/ComputeTest.h"
#include "tests/DepthTest.h"
#include "tests/FrameGraphTest.h"
#include "tests/ParticleTest.h"
#include "tests/ScriptTest.h"
#include "tests/StencilTest.h"
#include "tests/StressTest.h"

#include "cocos/base/AutoreleasePool.h"
#include "cocos/bindings/auto/jsb_gfx_auto.h"
#include "cocos/bindings/dop/jsb_dop.h"
#include "cocos/bindings/event/CustomEventTypes.h"
#include "cocos/bindings/event/EventDispatcher.h"
#include "cocos/bindings/jswrapper/SeApi.h"
#include "cocos/bindings/manual/jsb_classtype.h"
#include "cocos/bindings/manual/jsb_gfx_manual.h"
#include "cocos/bindings/manual/jsb_global_init.h"

#define DEFAULT_MATRIX_MATH

//#undef CC_USE_VULKAN
//#undef CC_USE_GLES3
//#undef CC_USE_GLES2
#include "renderer/GFXDeviceManager.h"

namespace cc {

WindowInfo TestBaseI::_windowInfo;

int               TestBaseI::_curTestIndex          = -1;
int               TestBaseI::_nextDirection         = 0;
TestBaseI *       TestBaseI::_test                  = nullptr;
const bool        TestBaseI::MANUAL_BARRIER         = true;
const uint        TestBaseI::NANOSECONDS_PER_SECOND = 1000000000;
const uint        TestBaseI::NANOSECONDS_60FPS      = 16666667L;
gfx::Device *     TestBaseI::_device                = nullptr;
gfx::Framebuffer *TestBaseI::_fbo                   = nullptr;
gfx::RenderPass * TestBaseI::_renderPass            = nullptr;

std::vector<TestBaseI::createFunc> TestBaseI::_tests = {
    ComputeTest::create,
    ScriptTest::create,
    FrameGraphTest::create,
    StressTest::create,
    ClearScreen::create,
    BasicTriangle::create,
    DepthTexture::create,
    BlendTest::create,
    ParticleTest::create,
    BunnyTest::create,
// Need to fix lib jpeg on iOS
#if CC_PLATFORM != CC_PLATFORM_MAC_IOS
    BasicTexture::create,
    StencilTest::create,
#endif // CC_PLATFORM != CC_PLATFORM_MAC_IOS
};

FrameRate                                       TestBaseI::hostThread;
FrameRate                                       TestBaseI::deviceThread;
std::vector<gfx::CommandBuffer *>               TestBaseI::_commandBuffers;
std::unordered_map<uint, gfx::GlobalBarrier *>  TestBaseI::_globalBarrierMap;
std::unordered_map<uint, gfx::TextureBarrier *> TestBaseI::_textureBarrierMap;

TestBaseI::TestBaseI(const WindowInfo &info) {
    if (!_device) {
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
            PoolManager::getInstance()->getCurrentPool()->clear();
            se->garbageCollect();
            PoolManager::getInstance()->getCurrentPool()->clear();
        });

        se->addAfterCleanupHook([]() {
            PoolManager::getInstance()->getCurrentPool()->clear();
            JSBClassType::destroy();
        });

        se->addRegisterCallback(register_all_dop_bindings);

        se->start();

        gfx::DeviceInfo dev_info;
        dev_info.windowHandle = info.windowHandle;
        dev_info.width        = info.screen.width;
        dev_info.height       = info.screen.height;
        dev_info.nativeWidth  = info.physicalWidth;
        dev_info.nativeHeight = info.physicalHeight;

        _device = gfx::DeviceManager::create(dev_info);
    }

    if (!_renderPass) {
        gfx::RenderPassInfo  renderPassInfo;
        gfx::ColorAttachment colorAttachment;
        colorAttachment.format = _device->getColorFormat();
        renderPassInfo.colorAttachments.emplace_back(colorAttachment);

        gfx::DepthStencilAttachment &depthStencilAttachment = renderPassInfo.depthStencilAttachment;
        depthStencilAttachment.format                       = _device->getDepthStencilFormat();

        _renderPass = _device->createRenderPass(renderPassInfo);
    }

    if (!_fbo) {
        gfx::FramebufferInfo fboInfo;
        fboInfo.colorTextures.resize(1);
        fboInfo.renderPass = _renderPass;
        _fbo               = _device->createFramebuffer(fboInfo);
    }

    if (!_commandBuffers.size()) {
        _commandBuffers.push_back(_device->getCommandBuffer());
    }

    hostThread.prevTime   = std::chrono::steady_clock::now();
    deviceThread.prevTime = std::chrono::steady_clock::now();
}

void TestBaseI::tickScript() {
    EventDispatcher::dispatchTickEvent(0.f);
    PoolManager::getInstance()->getCurrentPool()->clear();
}

void TestBaseI::destroyGlobal() {
    CC_SAFE_DESTROY(_test);
    CC_SAFE_DESTROY(_fbo);
    CC_SAFE_DESTROY(_renderPass);

    for (auto textureBarrier : _textureBarrierMap) {
        CC_SAFE_DELETE(textureBarrier.second);
    }
    _textureBarrierMap.clear();

    for (auto globalBarrier : _globalBarrierMap) {
        CC_SAFE_DELETE(globalBarrier.second);
    }
    _globalBarrierMap.clear();

    se::ScriptEngine::getInstance()->cleanup();

    CC_SAFE_DESTROY(_device);
}

void TestBaseI::nextTest(bool backward) {
    _nextDirection = backward ? -1 : 1;
}

void TestBaseI::toggleMultithread() {
    static bool multithreaded = true;
    multithreaded             = !multithreaded;
    ((gfx::DeviceAgent *)_device)->setMultithreaded(multithreaded);
}

void TestBaseI::onTouchEnd() {
    nextTest();
    // toggleMultithread();
}

void TestBaseI::update() {
    if (_nextDirection) {
        CC_SAFE_DESTROY(_test);
        if (_nextDirection < 0) _curTestIndex += _tests.size();
        _curTestIndex  = (_curTestIndex + _nextDirection) % _tests.size();
        _test          = _tests[_curTestIndex](_windowInfo);
        _nextDirection = 0;
    }
    if (_test) {
        _test->tick();
    }
}

void TestBaseI::evalString(std::string code) {
    se::AutoHandleScope hs;
    se::ScriptEngine::getInstance()->evalString(code.c_str());
}

void TestBaseI::runScript(std::string file) {
    se::AutoHandleScope hs;
    se::ScriptEngine::getInstance()->runScript(file.c_str());
}

unsigned char *TestBaseI::RGB2RGBA(Image *img) {
    int                  size    = img->getWidth() * img->getHeight();
    const unsigned char *srcData = img->getData();
    unsigned char *      dstData = new unsigned char[size * 4];
    for (int i = 0; i < size; i++) {
        dstData[i * 4]     = srcData[i * 3];
        dstData[i * 4 + 1] = srcData[i * 3 + 1];
        dstData[i * 4 + 2] = srcData[i * 3 + 2];
        dstData[i * 4 + 3] = 255u;
    }
    return dstData;
}

gfx::Texture *TestBaseI::createTextureWithFile(gfx::Device *device, gfx::TextureInfo &textureInfo, std::string imageFile) {
    auto img = new cc::Image();
    img->autorelease();
    if (!img->initWithImageFile(imageFile)) return nullptr;

    unsigned char *imgData = nullptr;
    if (img->getRenderFormat() == gfx::Format::RGB8) {
        imgData = TestBaseI::RGB2RGBA(img);
    } else {
        img->takeData(&imgData);
    }

    textureInfo.width  = img->getWidth();
    textureInfo.height = img->getHeight();
    textureInfo.format = gfx::Format::RGBA8;
    if (textureInfo.flags & gfx::TextureFlagBit::GEN_MIPMAP) {
        textureInfo.levelCount = TestBaseI::getMipmapLevelCounts(textureInfo.width, textureInfo.height);
    }
    auto texture = device->createTexture(textureInfo);

    gfx::BufferTextureCopy textureRegion;
    textureRegion.texExtent.width  = img->getWidth();
    textureRegion.texExtent.height = img->getHeight();
    device->copyBuffersToTexture({imgData}, texture, {textureRegion});
    delete[] imgData;
    return texture;
}

void TestBaseI::modifyProjectionBasedOnDevice(Mat4 &projection, bool isOffscreen) {
    float minZ        = _device->getCapabilities().clipSpaceMinZ;
    float signY       = _device->getCapabilities().screenSpaceSignY * (isOffscreen ? _device->getCapabilities().UVSpaceSignY : 1);
    float orientation = (float)_device->getSurfaceTransform();

    Mat4 trans, scale, rot;
    Mat4::createTranslation(0.0f, 0.0f, 1.0f + minZ, &trans);
    Mat4::createScale(1.0f, signY, 0.5f - 0.5f * minZ, &scale);
    Mat4::createRotationZ(orientation * MATH_PIOVER2, &rot);
    projection = rot * scale * trans * projection;
}

#ifndef DEFAULT_MATRIX_MATH
constexpr float preTransforms[4][4] = {
    {1, 0, 0, 1},   // GFXSurfaceTransform.IDENTITY
    {0, 1, -1, 0},  // GFXSurfaceTransform.ROTATE_90
    {-1, 0, 0, -1}, // GFXSurfaceTransform.ROTATE_180
    {0, -1, 1, 0},  // GFXSurfaceTransform.ROTATE_270
};
#endif

void TestBaseI::createOrthographic(float left, float right, float bottom, float top, float ZNear, float ZFar, Mat4 *dst, bool isOffscreen) {
#ifdef DEFAULT_MATRIX_MATH
    Mat4::createOrthographic(left, right, bottom, top, ZNear, ZFar, dst);
    TestBaseI::modifyProjectionBasedOnDevice(*dst, isOffscreen);
#else
    float                 minZ         = _device->getCapabilities().clipSpaceMinZ;
    float                 signY        = _device->getCapabilities().screenSpaceSignY * (isOffscreen ? _device->getCapabilities().UVSpaceSignY : 1);
    gfx::SurfaceTransform orientation  = _device->getSurfaceTransform();
    const float *         preTransform = preTransforms[(uint)orientation];

    memset(dst, 0, MATRIX_SIZE);

    float x  = 2.f / (right - left);
    float y  = 2.f / (top - bottom) * signY;
    float dx = (left + right) / (left - right);
    float dy = (bottom + top) / (bottom - top) * signY;

    dst->m[0]                          = x * preTransform[0];
    dst->m[1]                          = x * preTransform[1];
    dst->m[4]                          = y * preTransform[2];
    dst->m[5]                          = y * preTransform[3];
    dst->m[10]                         = (1.0f - minZ) / (ZNear - ZFar);
    dst->m[12]                         = dx * preTransform[0] + dy * preTransform[2];
    dst->m[13]                         = dx * preTransform[1] + dy * preTransform[3];
    dst->m[14]                         = (ZNear - minZ * ZFar) / (ZNear - ZFar);
    dst->m[15]                         = 1.0f;
#endif
}

void TestBaseI::createPerspective(float fov, float aspect, float zNear, float ZFar, Mat4 *dst, bool isOffscreen) {
#ifdef DEFAULT_MATRIX_MATH
    Mat4::createPerspective(MATH_DEG_TO_RAD(fov), aspect, zNear, ZFar, dst);
    TestBaseI::modifyProjectionBasedOnDevice(*dst, isOffscreen);
#else
    float                 minZ         = _device->getCapabilities().clipSpaceMinZ;
    float                 signY        = _device->getCapabilities().screenSpaceSignY * (isOffscreen ? _device->getCapabilities().UVSpaceSignY : 1);
    gfx::SurfaceTransform orientation  = _device->getSurfaceTransform();
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
    float y = _device->getCapabilities().screenSpaceSignY < 0.0f ? 1.f - relativeArea.y - relativeArea.w : relativeArea.y;
    float w = relativeArea.z;
    float h = relativeArea.w;

    gfx::Extent   size{_device->getWidth(), _device->getHeight()};
    gfx::Viewport viewport;

    switch (_device->getSurfaceTransform()) {
        case gfx::SurfaceTransform::ROTATE_90:
            viewport.left   = uint((1.f - y - h) * size.width);
            viewport.top    = uint(x * size.height);
            viewport.width  = uint(h * size.width);
            viewport.height = uint(w * size.height);
            break;
        case gfx::SurfaceTransform::ROTATE_180:
            viewport.left   = uint((1.f - x - w) * size.width);
            viewport.top    = uint((1.f - y - h) * size.height);
            viewport.width  = uint(w * size.width);
            viewport.height = uint(h * size.height);
            break;
        case gfx::SurfaceTransform::ROTATE_270:
            viewport.left   = uint(y * size.width);
            viewport.top    = uint((1.f - x - w) * size.height);
            viewport.width  = uint(h * size.width);
            viewport.height = uint(w * size.height);
            break;
        case gfx::SurfaceTransform::IDENTITY:
            viewport.left   = uint(x * size.width);
            viewport.top    = uint(y * size.height);
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

uint TestBaseI::getAlignedUBOStride(gfx::Device *device, uint stride) {
    uint alignment = device->getCapabilities().uboOffsetAlignment;
    return (stride + alignment - 1) / alignment * alignment;
}

gfx::GlobalBarrier *TestBaseI::getGlobalBarrier(const gfx::GlobalBarrierInfo &info) {
    uint hash = gfx::GlobalBarrier::computeHash(info);
    if (!_globalBarrierMap.count(hash)) {
        _globalBarrierMap[hash] = _device->createGlobalBarrier(info);
    }
    return _globalBarrierMap[hash];
}

gfx::TextureBarrier *TestBaseI::getTextureBarrier(const gfx::TextureBarrierInfo &info) {
    uint hash = gfx::TextureBarrier::computeHash(info);
    if (!_textureBarrierMap.count(hash)) {
        _textureBarrierMap[hash] = _device->createTextureBarrier(info);
    }
    return _textureBarrierMap[hash];
}

} // namespace cc
