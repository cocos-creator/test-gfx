#include "ScriptTest.h"

#include "base/threading/Semaphore.h"
#include "bindings/jswrapper/SeApi.h"

#define SEPARATE_RENDER_THREAD 1

namespace {
se::Value             sharedBuffer;
cc::gfx::Color *      pClearColor{nullptr};
std::atomic<bool>     shouldStop{false};
cc::Semaphore         sem;
} // namespace

namespace cc {

void ScriptTest::onDestroy() {
    shouldStop.store(true, std::memory_order_release);
    sem.wait();
}

bool ScriptTest::onInit() {
    se::AutoHandleScope scope;
    se::ScriptEngine::getInstance()->runScript("gl-matrix-min.js");
    se::ScriptEngine::getInstance()->runScript("main.js");

    size_t size{0u};
    se::ScriptEngine::getInstance()->getGlobalObject()->getProperty("sharedBuffer", &sharedBuffer);
    sharedBuffer.toObject()->getTypedArrayData((uint8_t **)&pClearColor, &size);

#if SEPARATE_RENDER_THREAD
    std::thread renderThread([this]() {
        while (!shouldStop) {
            renderThreadTick();
        }
        sem.signal();
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
