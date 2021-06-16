#include "ScriptTest.h"

#include "Boids.h"
#include "bindings/jswrapper/SeApi.h"
#include "jsb_boids_auto.h"
#include "jsb_chassis_auto.h"

namespace cc {

namespace {
constexpr std::chrono::nanoseconds LOGIC_THREAD_FRAME_TIME_LIMIT{1000000000 / 120};
} // namespace

void ScriptTest::onDestroy() {
    application::BoidsManager::destroy();
    TestBaseI::scriptEngineGC();
    experimental::RootManager::destroy();
}

bool ScriptTest::onInit() {
    se::AutoHandleScope scope;
    register_all_chassis(se::ScriptEngine::getInstance()->getGlobalObject());
    register_all_boids(se::ScriptEngine::getInstance()->getGlobalObject());

    se::ScriptEngine::getInstance()->runScript("gl-matrix.js");
    se::ScriptEngine::getInstance()->runScript("chassis.js");
    se::ScriptEngine::getInstance()->runScript("boids.js");

    return true;
}

void ScriptTest::onTick() {
    auto dt = std::chrono::duration_cast<std::chrono::nanoseconds>(logicThread.curTime - logicThread.prevTime);
    if (dt < LOGIC_THREAD_FRAME_TIME_LIMIT) std::this_thread::sleep_for(LOGIC_THREAD_FRAME_TIME_LIMIT - dt);

    printTime();

    tickScript();
}

void ScriptTest::onSpacePressed() {
    static bool mt    = true;
    auto *      agent = experimental::RootAgent::getInstance();
    if (agent) agent->setMultithreaded(mt = !mt);
}

} // namespace cc
