#include "ScriptTest.h"

#include "Boids.h"
#include "base/threading/Semaphore.h"
#include "bindings/jswrapper/SeApi.h"
#include "jsb_boids_auto.h"
#include "jsb_chassis_auto.h"
#include "tests/TestBase.h"

namespace cc {

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
    printTime();

    tickScript();
}

} // namespace cc
