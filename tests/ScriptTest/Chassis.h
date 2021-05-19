#pragma once

#include "base/Agent.h"
#include "base/LinearAllocatorPool.h"
#include "base/Ref.h"
#include "base/TypeDef.h"
#include "base/memory/Memory.h"
#include "base/threading/MessageQueue.h"
#include "base/threading/Semaphore.h"

#include "gfx-base/GFXDevice.h"

#include "Math.h"

namespace cc {

enum class TransformFlagBit : uint32_t {
    NONE     = 0,
    POSITION = (1 << 0),
    ROTATION = (1 << 1),
    SCALE    = (1 << 2),
    RS       = static_cast<uint32_t>(TransformFlagBit::ROTATION) | static_cast<uint32_t>(TransformFlagBit::SCALE),
    TRS      = static_cast<uint32_t>(TransformFlagBit::POSITION) | static_cast<uint32_t>(TransformFlagBit::ROTATION) | static_cast<uint32_t>(TransformFlagBit::SCALE),
    TRS_MASK = ~static_cast<uint32_t>(TransformFlagBit::TRS),
};
CC_ENUM_OPERATORS(TransformFlagBit);

class Transform {
public:
    Transform()                           = default;
    virtual ~Transform()                  = default;
    Transform(const Transform &) noexcept = delete;
    Transform(Transform &&) noexcept      = delete;
    Transform &operator=(const Transform &) noexcept = delete;
    Transform &operator=(Transform &&) noexcept = delete;

    virtual void setParent(Transform *value);
    virtual void setPosition(float x, float y, float z);
    virtual void setRotation(const float *q);
    virtual void setRotationFromEuler(float angleX, float angleY, float angleZ);
    virtual void setScale(float x, float y, float z);

    inline const vmath::Vec3F &getPosition() const { return _lpos; }
    inline const vmath::QuatF &getRotation() const { return _lrot; }
    inline const vmath::Vec3F &getScale() const { return _lscale; }

    inline const vmath::Vec3F &getWorldPosition() const;
    inline const vmath::QuatF &getWorldRotation() const;
    inline const vmath::Vec3F &getWorldScale() const;
    inline const vmath::Mat4F &getWorldMatrix() const;

protected:
    CC_VMATH_CALL_SUPPORT_FRIEND()

    void invalidateChildren(TransformFlagBit dirtyFlags);
    void updateWorldTransform() const;

    vmath::Vec3F _lpos = vmath::zero<vmath::Vec3F>();
    vmath::QuatF _lrot = vmath::identity<vmath::QuatF>();
    vmath::Vec3F _lscale{1.F, 1.F, 1.F};

    mutable TransformFlagBit _dirtyFlags{TransformFlagBit::NONE};
    mutable vmath::Vec3F     _pos = vmath::zero<vmath::Vec3F>();
    mutable vmath::QuatF     _rot = vmath::identity<vmath::QuatF>();
    mutable vmath::Vec3F     _scale{1.F, 1.F, 1.F};
    mutable vmath::Mat4F     _mat = vmath::identity<vmath::Mat4F>();

    Transform *         _parent{nullptr};
    vector<Transform *> _children;
};

const vmath::Vec3F &Transform::getWorldPosition() const {
    updateWorldTransform();
    return _pos;
}

const vmath::QuatF &Transform::getWorldRotation() const {
    updateWorldTransform();
    return _rot;
}

const vmath::Vec3F &Transform::getWorldScale() const {
    updateWorldTransform();
    return _scale;
}

const vmath::Mat4F &Transform::getWorldMatrix() const {
    updateWorldTransform();
    return _mat;
}

class Model {
public:
    Model()                       = default;
    virtual ~Model()              = default;
    Model(const Model &) noexcept = delete;
    Model(Model &&) noexcept      = delete;
    Model &operator=(const Model &) noexcept = delete;
    Model &operator=(Model &&) noexcept = delete;

    virtual void setColor(float r, float g, float b, float a);
    virtual void setTransform(const Transform *transform);

    inline const vmath::Vec4F &getColor() const { return _color; }
    inline const Transform *   getTransform() const { return _transform; }

protected:
    const Transform *_transform{nullptr};
    vmath::Vec4F     _color{1.F, 1.F, 1.F, 1.F};

    // fix-functioned resources just for test purposes
    static gfx::Shader *                shader;
    static gfx::Buffer *                vertexBuffer;
    static gfx::Buffer *                uniformBuffer;
    static gfx::Buffer *                uniformBufferView;
    static gfx::DescriptorSet *         descriptorSet;
    static gfx::DescriptorSetLayout *   descriptorSetLayout;
    static gfx::PipelineLayout *        pipelineLayout;
    static gfx::PipelineState *         pipelineState;
    static gfx::InputAssembler *        inputAssembler;
    static vector<gfx::GlobalBarrier *> globalBarriers;

    friend class Root;
};

class Root {
public:
    virtual ~Root();
    Root(const Root &) noexcept = delete;
    Root(Root &&) noexcept      = delete;
    Root &operator=(const Root &) noexcept = delete;
    Root &operator=(Root &&) noexcept = delete;

    virtual void initialize();
    virtual void destroy();

    virtual void render();

    virtual Transform *createTransform();

    virtual Model *createModel();
    virtual void   destroyModel(Model *model);

protected:
    static Root *instance;

    friend class RootManager;

    Root();

    vector<Model *> _models;
};

} // namespace cc

CC_VMATH_CALL_SUPPORT_BEGIN(cc::Transform)
CC_VMATH_CALL_SUPPORT_METHOD(setParent)
CC_VMATH_CALL_SUPPORT_METHOD(setPosition)
CC_VMATH_CALL_SUPPORT_METHOD(setRotation)
CC_VMATH_CALL_SUPPORT_METHOD(setScale)
CC_VMATH_CALL_SUPPORT_GETTER(getPosition, _lpos)
CC_VMATH_CALL_SUPPORT_GETTER(getRotation, _lrot)
CC_VMATH_CALL_SUPPORT_GETTER(getScale, _lscale)
CC_VMATH_CALL_SUPPORT_METHOD(getWorldPosition)
CC_VMATH_CALL_SUPPORT_METHOD(getWorldRotation)
CC_VMATH_CALL_SUPPORT_METHOD(getWorldScale)
CC_VMATH_CALL_SUPPORT_METHOD(getWorldMatrix)
CC_VMATH_CALL_SUPPORT_END(cc::Transform)

///////////////////// Agent /////////////////////

namespace cc {

class LinearAllocatorPool;
constexpr uint MAX_CPU_FRAME_AHEAD = 1U;

class TransformAgent : public Agent<Transform> {
public:
    using Agent::Agent;
    ~TransformAgent() override;

    void setParent(Transform *value) override;
    void setPosition(float x, float y, float z) override;
    void setRotation(const float *q) override;
    void setRotationFromEuler(float angleX, float angleY, float angleZ) override;
    void setScale(float x, float y, float z) override;
};

class ModelAgent : public Agent<Model> {
public:
    using Agent::Agent;
    ~ModelAgent() override;

    void setColor(float r, float g, float b, float a) override;
    void setTransform(const Transform *transform) override;
};

class RootAgent : public Agent<Root> {
public:
    static RootAgent *getInstance() { return instance; }

    ~RootAgent() override;

    void initialize() override;
    void destroy() override;

    void render() override;

    Transform *createTransform() override;

    void setMultithreaded(bool multithreaded);

    inline MessageQueue *       getMessageQueue() const { return _mainMessageQueue; }
    inline LinearAllocatorPool *getMainAllocator() const { return _allocatorPools[_currentIndex]; }

    Model *createModel() override;

protected:
    static RootAgent *instance;
    friend class RootManager;

    explicit RootAgent(Root *root);

    bool          _multithreaded{false};
    MessageQueue *_mainMessageQueue{nullptr};

    uint                          _currentIndex = 0U;
    vector<LinearAllocatorPool *> _allocatorPools;
    Semaphore                     _frameBoundarySemaphore{MAX_CPU_FRAME_AHEAD};
};

class RootManager {
public:
    static Root *create() {
        Root *root = CC_NEW(Root);

        root = CC_NEW(RootAgent(root));

        root->initialize();

        return root;
    }

    static void destroy() {
        CC_SAFE_DESTROY(Root::instance);
    }
};

} // namespace cc
