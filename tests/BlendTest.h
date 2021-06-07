#pragma once

#include "TestBase.h"

namespace cc {

class BlendTest : public TestBaseI {
public:
    DEFINE_CREATE_METHOD(BlendTest)
    using TestBaseI::TestBaseI;

    bool onInit() override;
    void onTick() override;
    void onDestroy() override;
};

} // namespace cc
