#pragma once

#include "base/Agent.h"
#include "base/TypeDef.h"
#include "base/memory/Memory.h"
#include "base/threading/Semaphore.h"
#include "base/threading/ThreadSafeCounter.h"

#include "gfx-base/GFXDevice.h"

#include "Math.h"

namespace cc {

class MessageQueue;

namespace experimental {

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
struct Transform {
    using Packet = Transform<vmath::FloatP>;

    /* AoS */
    using Vec3 = vmath::replace_scalar_t<Value, vmath::Vec3F>;
    using Quat = vmath::replace_scalar_t<Value, vmath::QuatF>;
    using Mat4 = vmath::replace_scalar_t<Value, vmath::Mat4F>;
    // */

    /* SoA *
    using Vec3 = vmath::Vec3<Value>;
    using Quat = vmath::Quat<Value>;
    using Mat4 = vmath::Mat4<Value>;
    // */

    using TransformFlags = vmath::replace_scalar_t<Value, TransformFlagBit>;
    using Index          = vmath::replace_scalar_t<Value, vmath::Index>;

    Vec3 lpos   = vmath::zero<vmath::Vec3F>();
    Quat lrot   = vmath::identity<vmath::QuatF>();
    Vec3 lscale = vmath::Vec3F{1.F, 1.F, 1.F};

    TransformFlags dirtyFlags{TransformFlagBit::NONE};
    Vec3           pos   = vmath::zero<vmath::Vec3F>();
    Quat           rot   = vmath::identity<vmath::QuatF>();
    Vec3           scale = vmath::Vec3F{1.F, 1.F, 1.F};
    Mat4           mat   = vmath::identity<vmath::Mat4F>();

    Index parent{-1};
    Index childrenCount{0};

    // NOLINTNEXTLINE(google-explicit-constructor) false positive when involving __VA_ARGS__
    CC_VMATH_STRUCT(Transform, lpos, lrot, lscale, dirtyFlags, pos, rot, scale, mat, parent, childrenCount)
};

CC_VMATH_STRUCT_SUPPORT_2(cc, experimental, Transform, lpos, lrot, lscale, dirtyFlags, pos, rot, scale, mat, parent, childrenCount)

using TransformF = Transform<float>;
using TransformP = Transform<vmath::FloatP>;
using TransformX = Transform<vmath::FloatX>;

class TransformView {
public:
    TransformView() = default;
    virtual ~TransformView();
    TransformView(const TransformView &) noexcept = delete;
    TransformView(TransformView &&) noexcept      = delete;
    TransformView &operator=(const TransformView &) noexcept = delete;
    TransformView &operator=(TransformView &&) noexcept = delete;

    explicit TransformView(vmath::Index idx);

    virtual void setParent(TransformView *value);
    virtual void setPosition(float x, float y, float z);
    virtual void setRotation(float x, float y, float z, float w);
    virtual void setRotationFromEuler(float angleX, float angleY, float angleZ);
    virtual void setScale(float x, float y, float z);

    template <typename Value>
    inline void setPosition(const Value &v) { setPosition(v[0], v[1], v[2]); }
    inline void setPosition(const float *v) { setPosition(v[0], v[1], v[2]); }
    template <typename Value>
    inline void setRotation(const Value &q) { setRotation(q[0], q[1], q[2], q[3]); }
    inline void setRotation(const float *q) { setRotation(q[0], q[1], q[2], q[3]); }
    template <typename Value>
    inline void setScale(const Value &v) { setScale(v[0], v[1], v[2]); }
    inline void setScale(const float *v) { setScale(v[0], v[1], v[2]); }

    static TransformX              buffer;
    static vector<vmath::IndexX>   childrenBuffers;
    static vector<TransformView *> views;

    void         updateWorldTransform() const;
    vmath::Index getIdx() const { return _idx; }

protected:
    void invalidateChildren(TransformFlagBit dirtyFlags) const;

    vmath::Index _idx{-1};
};

template <typename Value>
struct Model {
    using Packet = Model<vmath::FloatP>;

    using Index = vmath::replace_scalar_t<Value, vmath::Index>;
    using Vec4  = vmath::replace_scalar_t<Value, vmath::Vec4F>;
    using Bool  = vmath::mask_t<Value>;

    Index transform{-1};
    Vec4  color = vmath::Vec4F{1.F, 1.F, 1.F, 1.F};
    Bool  enabled{true};

    // NOLINTNEXTLINE(google-explicit-constructor) false positive when involving __VA_ARGS__
    CC_VMATH_STRUCT(Model, transform, color, enabled)
};

CC_VMATH_STRUCT_SUPPORT_2(cc, experimental, Model, transform, color, enabled)

using ModelF = Model<float>;
using ModelP = Model<vmath::FloatP>;
using ModelX = Model<vmath::FloatX>;

class ModelView {
public:
    ModelView() = default;
    virtual ~ModelView();
    ModelView(const ModelView &) noexcept = delete;
    ModelView(ModelView &&) noexcept      = delete;
    ModelView &operator=(const ModelView &) noexcept = delete;
    ModelView &operator=(ModelView &&) noexcept = delete;

    explicit ModelView(vmath::Index idx) : _idx(idx) {}

    virtual void setColor(float r, float g, float b, float a);
    virtual void setTransform(const TransformView *transform);
    virtual void setEnabled(bool enabled);

    template <typename Value>
    inline void setColor(const Value &v) { setColor(v[0], v[1], v[2], v[3]); }
    inline void setColor(const float *v) { setColor(v[0], v[1], v[2], v[3]); }

    static ModelX              buffer;
    static vector<ModelView *> views;

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

    virtual TransformView *createTransform();
    virtual ModelView *    createModel();

    static constexpr size_t INITIAL_CAPACITY = 256;

protected:
    static Root *instance;
    friend class RootManager;

    Root();
};

///////////////////// Agent /////////////////////

class TransformAgent : public Agent<TransformView> {
public:
    explicit TransformAgent(TransformView *const actor)
    : Agent<TransformView>(actor) {
        _idx = static_cast<TransformAgent *>(actor)->_idx;
    }
    ~TransformAgent() override;

    void setParent(TransformView *value) override;
    void setPosition(float x, float y, float z) override;
    void setRotation(float x, float y, float z, float w) override;
    void setRotationFromEuler(float angleX, float angleY, float angleZ) override;
    void setScale(float x, float y, float z) override;
};

class ModelAgent : public Agent<ModelView> {
public:
    explicit ModelAgent(ModelView *const actor)
    : Agent<ModelView>(actor) {
        _idx = static_cast<ModelAgent *>(actor)->_idx;
    }
    ~ModelAgent() override;

    void setColor(float r, float g, float b, float a) override;
    void setTransform(const TransformView *transform) override;
    void setEnabled(bool enabled) override;
};

class RootAgent : public Agent<Root> {
public:
    static RootAgent *getInstance() { return instance; }

    ~RootAgent() override;

    void initialize() override;
    void destroy() override;

    void render() override;
    void renderThreadLoop();

    TransformView *createTransform() override;
    ModelView *    createModel() override;

    void setMultithreaded(bool multithreaded);

    inline cc::MessageQueue *getMessageQueue() const { return _mainMessageQueue; }

protected:
    static RootAgent *instance;
    friend class RootManager;

    explicit RootAgent(Root *root);

    bool          _multithreaded{false};
    MessageQueue *_mainMessageQueue{nullptr};

    bool                    _running{false};
    ThreadSafeCounter<uint> _pendingTickCount;
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

    static Root *getInstance() { return instance; }

private:
    static Root *instance;
};

} // namespace experimental
} // namespace cc
