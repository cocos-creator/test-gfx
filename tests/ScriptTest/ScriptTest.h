#pragma once

#include "../TestBase.h"

namespace cc {

class ScriptTest : public TestBaseI {
public:
    DEFINE_CREATE_METHOD(ScriptTest)

    bool onInit() override;
    void onTick() override;
    void onDestroy() override;

    void onSpacePressed() override;
};

} // namespace cc
