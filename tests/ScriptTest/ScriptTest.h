#pragma once

#include "../TestBase.h"

namespace cc {

class ScriptTest : public TestBaseI {
public:
    DEFINE_CREATE_METHOD(ScriptTest)
    explicit ScriptTest(const WindowInfo &info) : TestBaseI(info){};

    bool onInit() override;
    void onTick() override;
    void onDestroy() override;
};

} // namespace cc
