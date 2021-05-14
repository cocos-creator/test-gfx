#include "ScriptTest.h"

#include "base/threading/Semaphore.h"
#include "bindings/jswrapper/SeApi.h"
#include "jsb_transform_auto.h"
#include "tests/TestBase.h"

namespace cc {

void ScriptTest::onDestroy() {
    TestBaseI::scriptEngineGC();
    RootManager::destroy();
}

bool ScriptTest::onInit() {
    se::AutoHandleScope scope;
    register_all_transform(se::ScriptEngine::getInstance()->getGlobalObject());

    se::ScriptEngine::getInstance()->runScript("gl-matrix.js");
    se::ScriptEngine::getInstance()->runScript("boids.js");

    return true;
}

void ScriptTest::onTick() {
    tickScript();
}

} // namespace cc
