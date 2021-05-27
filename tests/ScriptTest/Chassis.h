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

template <typename Value>
struct TransformT {
    static constexpr size_t MAX_CHILDREN_COUNT = 256;

    using Packet = TransformT<vmath::FloatP>;

    using Vec3 = vmath::Vec3<Value>;
    using Quat = vmath::Quat<Value>;
    using Mat4 = vmath::Mat4<Value>;

    using TransformFlags = vmath::replace_scalar_t<Value, TransformFlagBit>;
    using Index          = vmath::replace_scalar_t<Value, vmath::Index>;
    using IndexX         = vmath::Array<Index, MAX_CHILDREN_COUNT>;

    Vec3 lpos = vmath::zero<Vec3>();
    Quat lrot = vmath::identity<Quat>();
    Vec3 lscale{1.F, 1.F, 1.F};

    TransformFlags dirtyFlags{TransformFlagBit::NONE};
    Vec3           pos = vmath::zero<Vec3>();
    Quat           rot = vmath::identity<Quat>();
    Vec3           scale{1.F, 1.F, 1.F};
    Mat4           mat = vmath::identity<Mat4>();

    Index  parent{-1};
    Index  childrenCount{0};
    IndexX children{-1};

    // NOLINTNEXTLINE(google-explicit-constructor) false positive when involving __VA_ARGS__
    CC_VMATH_STRUCT(TransformT, lpos, lrot, lscale, dirtyFlags, pos, rot, scale, mat, parent, childrenCount, children)
};

CC_VMATH_STRUCT_SUPPORT_1(cc, TransformT, lpos, lrot, lscale, dirtyFlags, pos, rot, scale, mat, parent, childrenCount, children)

using TransformF = TransformT<float>;
using TransformP = TransformT<vmath::FloatP>;
using TransformX = TransformT<vmath::FloatX>;

class Transform {
public:
    using Ptr  = Transform *;
    using PtrP = vmath::Array<Ptr, vmath::PACKET_SIZE>;
    using PtrX = vmath::DynamicArray<PtrP>;

    Transform() = default;
    virtual ~Transform();
    Transform(const Transform &) noexcept = delete;
    Transform(Transform &&) noexcept      = delete;
    Transform &operator=(const Transform &) noexcept = delete;
    Transform &operator=(Transform &&) noexcept = delete;

    explicit Transform(vmath::Index idx) : _idx(idx) {}

    virtual void setParent(Transform *value);
    virtual void setPosition(float x, float y, float z);
    virtual void setRotation(const float *q);
    virtual void setRotationFromEuler(float angleX, float angleY, float angleZ);
    virtual void setScale(float x, float y, float z);

    static TransformX   buffer;
    static PtrX         views;
    static vmath::Index viewCount;

    void         updateWorldTransform() const;
    vmath::Index getIdx() const { return _idx; }

protected:
    void invalidateChildren(TransformFlagBit dirtyFlags) const;

    vmath::Index _idx{-1};
};

template <typename Value>
struct ModelT {
    using Packet = ModelT<vmath::FloatP>;

    using TransformIdx = vmath::replace_scalar_t<Value, vmath::Index>;
    using Vec4         = vmath::Vec4<Value>;
    using Bool         = vmath::mask_t<Value>;

    TransformIdx transform{-1};
    Vec4         color{1.F, 1.F, 1.F, 1.F};
    Bool         enabled{true};

    // NOLINTNEXTLINE(google-explicit-constructor) false positive when involving __VA_ARGS__
    CC_VMATH_STRUCT(ModelT, transform, color, enabled)
};

CC_VMATH_STRUCT_SUPPORT_1(cc, ModelT, transform, color, enabled)

using ModelF = ModelT<float>;
using ModelP = ModelT<vmath::FloatP>;
using ModelX = ModelT<vmath::FloatX>;

class Model {
public:
    using Ptr  = Model *;
    using PtrP = vmath::Array<Ptr, vmath::PACKET_SIZE>;
    using PtrX = vmath::DynamicArray<PtrP>;

    Model() = default;
    virtual ~Model();
    Model(const Model &) noexcept = delete;
    Model(Model &&) noexcept      = delete;
    Model &operator=(const Model &) noexcept = delete;
    Model &operator=(Model &&) noexcept = delete;

    explicit Model(vmath::Index idx) : _idx(idx) {}

    virtual void setColor(float r, float g, float b, float a);
    virtual void setTransform(const Transform *transform);
    virtual void setEnabled(bool enabled);

    static ModelX       buffer;
    static PtrX         views;
    static vmath::Index viewCount;

    vmath::Index getIdx() const { return _idx; }

protected:
    vmath::Index _idx{-1};
};

class Root {
public:
    static Root *getInstance() { return instance; }

    virtual ~Root();
    Root(const Root &) noexcept = delete;
    Root(Root &&) noexcept      = delete;
    Root &operator=(const Root &) noexcept = delete;
    Root &operator=(Root &&) noexcept = delete;

    virtual void initialize();
    virtual void destroy();

    virtual void render();

    virtual Transform *createTransform();
    virtual Model *    createModel();

protected:
    static Root *instance;
    friend class RootManager;

    Root();
};

///////////////////// Agent /////////////////////

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
    void setEnabled(bool enabled) override;
};

class RootAgent : public Agent<Root> {
public:
    static RootAgent *getInstance() { return instance; }

    ~RootAgent() override;

    void initialize() override;
    void destroy() override;

    void render() override;

    Transform *createTransform() override;
    Model *    createModel() override;

    void setMultithreaded(bool multithreaded);

    inline MessageQueue *       getMessageQueue() const { return _mainMessageQueue; }
    inline LinearAllocatorPool *getMainAllocator() const { return _allocatorPools[_currentIndex]; }

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
        root       = CC_NEW(RootAgent(root));

        root->initialize();
        instance = root;

        return root;
    }

    static void destroy() {
        CC_SAFE_DESTROY(instance);
    }

private:
    static Root *instance;
};

} // namespace cc
