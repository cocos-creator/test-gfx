#pragma once

#include "TestBase.h"

namespace cc {

class ClearScreen : public TestBaseI {
public:
    DEFINE_CREATE_METHOD(ClearScreen)
    ClearScreen(const WindowInfo &info) : TestBaseI(info){};
    ~ClearScreen() = default;

public:
    bool onInit() override;
    void onTick() override;
    void onDestroy() override;
};

} // namespace cc
