#pragma once

#include "TestBase.h"

namespace cc {

class ScriptTest : public TestBaseI {
public:
    DEFINE_CREATE_METHOD(ScriptTest)
    ScriptTest(const WindowInfo &info) : TestBaseI(info){};
    ~ScriptTest() = default;

    bool onInit() override;
    void onTick() override;
    void onDestroy() override;
    void renderThreadLoop();

private:
    bool _working{false};
};

} // namespace cc
