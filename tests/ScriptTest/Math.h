#pragma once

#include "enoki/matrix.h"
#include "enoki/quaternion.h"

#include "supp_math.h"

namespace cc {
namespace vmath {

template <typename Value>
using Vec3 = enoki::Array<Value, 3>;

template <typename Value>
using Vec4 = enoki::Array<Value, 4>;

template <typename Value>
using Quat = enoki::Quaternion<Value>;

template <typename Value>
using Mat3 = enoki::Matrix<Value, 3>;

template <typename Value>
using Mat4 = enoki::Matrix<Value, 4>;

using Vec3F = Vec3<float>;
using Vec4F = Vec4<float>;
using QuatF = Quat<float>;
using Mat3F = Mat3<float>;
using Mat4F = Mat4<float>;

template <typename Value>
Value zero() {
    return enoki::zero<Value>();
}

template <typename Value>
Value identity() {
    return enoki::identity<Value>();
}

template <typename Value>
auto quatToEuler = enoki::quat_to_euler<Vec3<Value>, Value>;

template <typename Value>
auto eulerToQuat = enoki::euler_to_quat<Quat<Value>, Value>;

template <typename Value>
auto quatToMat3 = enoki::quat_to_matrix<Mat3<Value>, Value>;

template <typename Value>
auto quatToMat4 = enoki::quat_to_matrix<Mat4<Value>, Value>;

template <typename Value>
auto mat3ToQuat = enoki::euler_to_quat<Value, 3>;

template <typename Value>
auto mat4ToQuat = enoki::euler_to_quat<Value, 4>;

} // namespace vmath
} // namespace cc
