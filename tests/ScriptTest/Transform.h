#pragma once

#include "base/TypeDef.h"
#include "base/memory/Memory.h"

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
    ENOKI_CALL_SUPPORT_FRIEND()
public:
    Transform() = default;

    void setParent(Transform *value);
    void setPosition(float x, float y, float z);
    void setRotation(float angleX, float angleY, float angleZ);
    void setScale(float x, float y, float z);

    inline const vmath::Vec3F &getPosition() { return _lpos; }
    inline const vmath::QuatF &getRotation() { return _lrot; }
    inline const vmath::Vec3F &getScale() { return _lscale; }

    inline const vmath::Vec3F &getWorldPosition();
    inline const vmath::QuatF &getWorldRotation();
    inline const vmath::Vec3F &getWorldScale();
    inline const vmath::Mat4F &getWorldMatrix();

private:
    void invalidateChildren(TransformFlagBit dirtyFlags);
    void updateWorldTransform();

    vmath::Vec3F _lpos = vmath::zero<vmath::Vec3F>();
    vmath::QuatF _lrot = vmath::identity<vmath::QuatF>();
    vmath::Vec3F _lscale{1.F, 1.F, 1.F};

    TransformFlagBit _dirtyFlags{TransformFlagBit::NONE};
    vmath::Vec3F     _pos = vmath::zero<vmath::Vec3F>();
    vmath::QuatF     _rot = vmath::identity<vmath::QuatF>();
    vmath::Vec3F     _scale{1.F, 1.F, 1.F};
    vmath::Mat4F     _mat = vmath::identity<vmath::Mat4F>();

    Transform *         _parent{nullptr};
    vector<Transform *> _children;
};

const vmath::Vec3F &Transform::getWorldPosition() {
    updateWorldTransform();
    return _pos;
}

const vmath::QuatF &Transform::getWorldRotation() {
    updateWorldTransform();
    return _rot;
}

const vmath::Vec3F &Transform::getWorldScale() {
    updateWorldTransform();
    return _scale;
}

const vmath::Mat4F &Transform::getWorldMatrix() {
    updateWorldTransform();
    return _mat;
}

} // namespace cc

ENOKI_CALL_SUPPORT_BEGIN(cc::Transform)
ENOKI_CALL_SUPPORT_METHOD(setParent)
ENOKI_CALL_SUPPORT_METHOD(setPosition)
ENOKI_CALL_SUPPORT_METHOD(setRotation)
ENOKI_CALL_SUPPORT_METHOD(setScale)
ENOKI_CALL_SUPPORT_GETTER(getPosition, _lpos)
ENOKI_CALL_SUPPORT_GETTER(getRotation, _lrot)
ENOKI_CALL_SUPPORT_GETTER(getScale, _lscale)
ENOKI_CALL_SUPPORT_METHOD(getWorldPosition)
ENOKI_CALL_SUPPORT_METHOD(getWorldRotation)
ENOKI_CALL_SUPPORT_METHOD(getWorldScale)
ENOKI_CALL_SUPPORT_METHOD(getWorldMatrix)
ENOKI_CALL_SUPPORT_END(cc::Transform)
