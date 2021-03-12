#include "ScriptTest.h"

#include "bindings/jswrapper/SeApi.h"

#define SEPARATE_RENDER_THREAD 1

namespace {
se::Value       sharedBuffer;
cc::gfx::Color *pClearColor{nullptr};
} // namespace

namespace cc {

void ScriptTest::onDestroy() {
    _shouldStop = true;
    _cv.wait();
}

bool ScriptTest::onInit() {
    se::AutoHandleScope scope;
    se::ScriptEngine::getInstance()->runScript("main.js");
    se::ScriptEngine::getInstance()->getGlobalObject()->getProperty("sharedBuffer", &sharedBuffer);

    size_t size{0u};
    sharedBuffer.toObject()->getTypedArrayData((uint8_t **)&pClearColor, &size);

#if SEPARATE_RENDER_THREAD
    std::thread renderThread([this]() {
        while (!_shouldStop) {
            renderThreadTick();
        }
        _cv.signal();
    });
    renderThread.detach();
#endif

    return true;
}

void ScriptTest::onTick() {
    tickScript();

#if !SEPARATE_RENDER_THREAD
    renderThreadTick();
#endif
}

void ScriptTest::renderThreadTick() {
    _device->acquire();

    gfx::Rect renderArea = {0, 0, _device->getWidth(), _device->getHeight()};

    auto commandBuffer = _commandBuffers[0];
    commandBuffer->begin();
    commandBuffer->beginRenderPass(_fbo->getRenderPass(), _fbo, renderArea, pClearColor, 1.0f, 0);
    commandBuffer->endRenderPass();
    commandBuffer->end();

    _device->flushCommands(_commandBuffers);
    _device->getQueue()->submit(_commandBuffers);
    _device->present();
}

} // namespace cc
