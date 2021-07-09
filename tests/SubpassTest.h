#pragma once

#include "TestBase.h"
#include "utils/StandardPipelineUtils.h"

namespace cc {

class SubpassTest : public TestBaseI {
public:
    DEFINE_CREATE_METHOD(SubpassTest)
    using TestBaseI::TestBaseI;

    bool onInit() override;
    void onTick() override;

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

    gfx::ColorList _clearColors;

    bool _useDeferred = true;
};

} // namespace cc
