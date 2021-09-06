#pragma once

#include "TestBase.h"

namespace cc {

class DepthTexture : public TestBaseI {
public:
    DEFINE_CREATE_METHOD(DepthTexture)
    using TestBaseI::TestBaseI;

    bool onInit() override;
    void onDestroy() override;
    void onTick() override;
    void onResize(gfx::Swapchain *swapchain) override;

private:
    Mat4 _bunnyMatrices[3];
    Vec3 _eye;
    Vec3 _center;
    Vec3 _up;
};

} // namespace cc
