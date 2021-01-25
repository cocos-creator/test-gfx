#pragma once

#include "TestBase.h"

namespace cc {

class BlendTest : public TestBaseI {
public:
    DEFINE_CREATE_METHOD(BlendTest)
    BlendTest(const WindowInfo &info) : TestBaseI(info){};
    ~BlendTest() = default;

    bool onInit() override;
    void onTick() override;
    void onDestroy() override;
};

} // namespace cc
