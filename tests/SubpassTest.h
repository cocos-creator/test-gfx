#pragma once

#include "TestBase.h"
#include "utils/StandardPipelineUtils.h"

namespace cc {

class SubpassTest : public TestBaseI {
public:
    DEFINE_CREATE_METHOD(SubpassTest)
    using TestBaseI::TestBaseI;
    ~SubpassTest() = default;

    bool onInit() override;
    void onTick() override;
    void onDestroy() override;

protected:
    void onSpacePressed() override;

    StandardUniformBuffers   _ubos;
    StandardForwardPipeline  _forward;
    StandardDeferredPipeline _deferred;

    std::unique_ptr<gfx::Buffer>         _vertexPositionBuffer{nullptr};
    std::unique_ptr<gfx::Buffer>         _vertexNormalBuffer{nullptr};
    std::unique_ptr<gfx::Buffer>         _indexBuffer{nullptr};
    std::unique_ptr<gfx::InputAssembler> _inputAssembler{nullptr};

    Mat4 _worldMatrix;
    Mat4 _projectionMatrix;

    gfx::Color _clearColor{0.1F, 0.1F, 0.1F, 1.F};

    bool _useDeferred{true};
};

} // namespace cc
