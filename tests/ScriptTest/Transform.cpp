#include "Transform.h"
#include "math/Math.h"

namespace cc {

void Transform::setParent(Transform *value) {
    if (_parent == value) return;

    if (_parent) {
        _parent->_children.erase(
            std::remove(_parent->_children.begin(), _parent->_children.end(), this),
            _parent->_children.end());
    }
    _parent = value;

    if (std::find(value->_children.begin(), value->_children.end(), this) == value->_children.end()) {
        value->_children.push_back(this);
    }

    invalidateChildren(TransformFlagBit::TRS);
}

void Transform::setPosition(float x, float y, float z) {
    _lpos[0] = x;
    _lpos[1] = y;
    _lpos[2] = z;

    invalidateChildren(TransformFlagBit::POSITION);
}

void Transform::setRotation(float angleX, float angleY, float angleZ) {
    static const float HALF_TO_RAD = 0.5F * math::PI / 180.F;

    angleX *= HALF_TO_RAD;
    angleY *= HALF_TO_RAD;
    angleZ *= HALF_TO_RAD;

    float sx = sinf(angleX);
    float cx = cosf(angleX);
    float sy = sinf(angleY);
    float cy = cosf(angleY);
    float sz = sinf(angleZ);
    float cz = cosf(angleZ);

    _lrot[0] = sx * cy * cz + cx * sy * sz;
    _lrot[1] = cx * sy * cz + sx * cy * sz;
    _lrot[2] = cx * cy * sz - sx * sy * cz;
    _lrot[3] = cx * cy * cz - sx * sy * sz;

    invalidateChildren(TransformFlagBit::ROTATION);
}

void Transform::setScale(float x, float y, float z) {
    _lscale[0] = x;
    _lscale[1] = y;
    _lscale[2] = z;

    invalidateChildren(TransformFlagBit::SCALE);
}

void Transform::invalidateChildren(TransformFlagBit dirtyFlags) {
    if ((_dirtyFlags & dirtyFlags) == dirtyFlags) {
        return;
    }

    _dirtyFlags |= dirtyFlags;
    TransformFlagBit newDirtyBit = dirtyFlags | TransformFlagBit::POSITION;

    for (auto &child : _children) {
        child->invalidateChildren(newDirtyBit);
    }
}

namespace {
vector<Transform *> tempArray;
vmath::QuatF        tempQuat;
vmath::Mat3F        tempMat3a;
vmath::Mat3F        tempMat3b;
} // namespace

void Transform::updateWorldTransform() {
    if (_dirtyFlags == TransformFlagBit::NONE) {
        return;
    }

    Transform *cur = this;
    tempArray.clear();
    while (cur && cur->_dirtyFlags != TransformFlagBit::NONE) {
        tempArray.push_back(cur);
        cur = cur->_parent;
    }
    Transform *      child     = this;
    TransformFlagBit dirtyBits = TransformFlagBit::NONE;

    // size_t i = tempArray.size();
    // while (i) {
    //     child = tempArray[--i];
    //     dirtyBits |= child->_dirtyFlags;
    //     if (cur) {
    //         if (hasFlag(dirtyBits, TransformFlagBit::POSITION)) {
    //             child->_pos     = child->_lpos * cur->_mat;
    //             child->_mat[12] = child->_pos[0];
    //             child->_mat[13] = child->_pos[1];
    //             child->_mat[14] = child->_pos[2];
    //         }
    //         if (hasFlag(dirtyBits, TransformFlagBit::RS)) {
    //             mat4.fromRotationTranslationScale(child->_mat, child->_lrot, child->_lpos, child->_lscale);
    //             mat4.multiply(child->_mat, cur->_mat, child->_mat);
    //             if (hasFlag(dirtyBits, TransformFlagBit::ROTATION)) {
    //                 quat.multiply(child->_rot, cur->_rot, child->_lrot);
    //             }
    //             mat3.fromQuat(tempMat3a, quat.conjugate(tempQuat, child->_rot));
    //             mat3.multiply(tempMat3a, tempMat3a, mat3.fromMat4(tempMat3b, child->_mat));
    //             child->_scale[0] = tempMat3a[0];
    //             child->_scale[1] = tempMat3a[4];
    //             child->_scale[2] = tempMat3a[8];
    //         }
    //     } else {
    //         if (hasFlag(dirtyBits, TransformFlagBit::POSITION)) {
    //             vec3.copy(child->_pos, child->_lpos);
    //             child->_mat[12] = child->_pos[0];
    //             child->_mat[13] = child->_pos[1];
    //             child->_mat[14] = child->_pos[2];
    //         }
    //         if (hasFlag(dirtyBits, TransformFlagBit::RS)) {
    //             if (hasFlag(dirtyBits, TransformFlagBit::ROTATION)) {
    //                 quat.copy(child->_rot, child->_lrot);
    //             }
    //             if (hasFlag(dirtyBits, TransformFlagBit::SCALE)) {
    //                 vec3.copy(child->_scale, child->_lscale);
    //                 mat4.fromRotationTranslationScale(child->_mat, child->_rot, child->_pos, child->_scale);
    //             }
    //         }
    //     }

    //     child->_dirtyFlags = TransformFlagBit::NONE;
    //     cur                = child;
    // }
}

} // namespace cc
