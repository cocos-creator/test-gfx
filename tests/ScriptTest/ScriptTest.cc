#include "ScriptTest.h"

#include "base/threading/Semaphore.h"
#include "bindings/jswrapper/SeApi.h"
#include "jsb_transform_auto.h"

#define SEPARATE_RENDER_THREAD 1

namespace {
se::Value             sharedBuffer;
cc::gfx::Color *      pClearColor{nullptr};
std::atomic<bool>     shouldStop{false};
cc::Semaphore         sem{0};
} // namespace

namespace cc {

void ScriptTest::onDestroy() {
#if SEPARATE_RENDER_THREAD
    shouldStop.store(true, std::memory_order_relaxed);
    sem.wait();
#endif
}

bool ScriptTest::onInit() {
    se::AutoHandleScope scope;
    register_all_transform(se::ScriptEngine::getInstance()->getGlobalObject());

    se::ScriptEngine::getInstance()->runScript("gl-matrix-min.js");
    se::ScriptEngine::getInstance()->runScript("simple.js");

    // size_t size{0U};
    // se::ScriptEngine::getInstance()->getGlobalObject()->getProperty("sharedBuffer", &sharedBuffer);
    // sharedBuffer.toObject()->getTypedArrayData(reinterpret_cast<uint8_t **>(&pClearColor), &size);
    pClearColor = CC_NEW(gfx::Color);

#if SEPARATE_RENDER_THREAD
    std::thread renderThread([]() {
        while (!shouldStop.load(std::memory_order_relaxed)) {
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
    device->acquire();

    gfx::Rect renderArea = {0, 0, device->getWidth(), device->getHeight()};

    auto *commandBuffer = commandBuffers[0];
    commandBuffer->begin();
    commandBuffer->beginRenderPass(fbo->getRenderPass(), fbo, renderArea, pClearColor, 1.0F, 0);
    commandBuffer->endRenderPass();
    commandBuffer->end();

    device->flushCommands(commandBuffers);
    device->getQueue()->submit(commandBuffers);
    device->present();
}

} // namespace cc
