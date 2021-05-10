#pragma once

#include "base/TypeDef.h"
#include "base/memory/Memory.h"

#include "enoki/fwd.h"
#include "enoki/matrix.h"
#include "enoki/quaternion.h"

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
class Transform {
public:
    using Scalar         = enoki::scalar_t<Value>;

    using Vec3           = enoki::Array<Value, 3>;
    using Quat           = enoki::Quaternion<Value>;
    using Mat3           = enoki::Matrix<Value, 3>;
    using Mat4           = enoki::Matrix<Value, 4>;
    using TransformFlags = enoki::uint32_array_t<Value>;

    void setParent(Transform *value);
    void setPosition(float x, float y, float z);
    void setRotation(float angleX, float angleY, float angleZ);
    void setScale(float x, float y, float z);

    inline const Vec3 &getPosition() { return _lpos; }
    inline const Quat &getRotation() { return _lrot; }
    inline const Vec3 &getScale() { return _lscale; }

    inline const Vec3 &getWorldPosition() {
        updateWorldTransform();
        return _pos;
    }
    inline const Quat &getWorldRotation() {
        updateWorldTransform();
        return _rot;
    }
    inline const Vec3 &getWorldScale() {
        updateWorldTransform();
        return _scale;
    }
    inline const Mat4 &getWorldMatrix() {
        updateWorldTransform();
        return _mat;
    }

private:
    void updateWorldTransform();
    void invalidateChildren(TransformFlags dirtyFlags);

    Vec3 _lpos;
    Quat _lrot;
    Vec3 _lscale{1, 1, 1};

    TransformFlags _dirtyFlags;
    Vec3           _pos;
    Quat           _rot;
    Vec3           _scale{1, 1, 1};
    Mat4           _mat;

    vector<Transform<Scalar> *> _children;
    Transform<Scalar> *         _parent;

    // NOLINTNEXTLINE(google-explicit-constructor)
    ENOKI_STRUCT(Transform, _lpos, _lrot, _lscale, _dirtyFlags, _pos, _rot, _scale, _mat, _children, _parent)
};

template <typename Value>
void Transform<Value>::setParent(Transform<Value> *value) {
}

template <typename Value>
void Transform<Value>::setPosition(float x, float y, float z) {
    _lpos.x() = x;
    _lpos.y() = y;
    _lpos.z() = z;
}

template <typename Value>
void Transform<Value>::setRotation(float angleX, float angleY, float angleZ) {
}

template <typename Value>
void Transform<Value>::setScale(float x, float y, float z) {
    _lscale[0] = x;
    _lscale[1] = y;
    _lscale[2] = z;
}

template <typename Value>
void Transform<Value>::updateWorldTransform() {
}

template <typename Value>
void Transform<Value>::invalidateChildren(TransformFlags dirtyFlags) {
}

using FloatP     = enoki::Packet<float, 16>;
using FloatX     = enoki::DynamicArray<FloatP>;
using TransformF = Transform<float>;
using TransformP = Transform<FloatP>;
using TransformX = Transform<FloatX>;

} // namespace cc

ENOKI_STRUCT_SUPPORT(cc::Transform, _lpos, _lrot, _lscale, _dirtyFlags, _pos, _rot, _scale, _mat, _children, _parent)
