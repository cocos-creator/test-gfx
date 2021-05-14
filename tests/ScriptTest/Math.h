#pragma once

#include "supp_math.h"

#include <sstream>
#include "base/Log.h"
#include "base/Macros.h"
#include "base/memory/Memory.h"
#include "math/Math.h"

namespace cc {
namespace vmath {

#define CC_VMATH_CALL_SUPPORT_FRIEND ENOKI_CALL_SUPPORT_FRIEND

#define CC_VMATH_CALL_SUPPORT_BEGIN  ENOKI_CALL_SUPPORT_BEGIN
#define CC_VMATH_CALL_SUPPORT_METHOD ENOKI_CALL_SUPPORT_METHOD
#define CC_VMATH_CALL_SUPPORT_GETTER ENOKI_CALL_SUPPORT_GETTER
#define CC_VMATH_CALL_SUPPORT_END    ENOKI_CALL_SUPPORT_END

#define CC_VMATH_STRUCT         ENOKI_STRUCT
#define CC_VMATH_STRUCT_SUPPORT ENOKI_STRUCT_SUPPORT

template <typename Value, size_t Size>
using Array = enoki::Array<Value, Size>;
template <typename Value, size_t Size>
using Packet = enoki::Packet<Value, Size>;
template <typename Value>
using DynamicArray = enoki::DynamicArray<Value>;

template <typename... Ts>
using expr_t = enoki::expr_t<Ts...>;
template <typename T>
using scalar_t = enoki::scalar_t<T>;
template <typename T, bool CopyFlags = true>
using mask_t = enoki::mask_t<T, CopyFlags>;
template <typename T, typename Value, bool CopyFlags = true>
using replace_scalar_t = enoki::replace_scalar_t<T, Value, CopyFlags>;

using FloatP = Packet<float, 16>;
using FloatX = DynamicArray<FloatP>;

template <typename Value>
using Vec3  = Array<Value, 3>;
using Vec3F = Vec3<float>;
using Vec3P = Vec3<FloatP>;
using Vec3X = Vec3<FloatX>;

template <typename Value>
using Vec4  = Array<Value, 4>;
using Vec4F = Vec4<float>;
using Vec4P = Vec4<FloatP>;
using Vec4X = Vec4<FloatX>;

template <typename Value>
using Quat  = enoki::Quaternion<Value>;
using QuatF = Quat<float>;
using QuatP = Quat<FloatP>;
using QuatX = Quat<FloatX>;

template <typename Value>
using Mat3  = enoki::Matrix<Value, 3>;
using Mat3F = Mat3<float>;
using Mat3P = Mat3<FloatP>;
using Mat3X = Mat3<FloatX>;

template <typename Value>
using Mat4  = enoki::Matrix<Value, 4>;
using Mat4F = Mat4<float>;
using Mat4P = Mat4<FloatP>;
using Mat4X = Mat4<FloatX>;

template <typename Value>
CC_INLINE Value zero() {
    return enoki::zero<Value>();
}

template <typename Value>
CC_INLINE Value identity() {
    return enoki::identity<Value>();
}

template <typename Value>
CC_INLINE Value load(const void *mem) {
    return enoki::load_unaligned<Value>(mem);
}

template <typename Value>
CC_INLINE void store(void *mem, const Value &value) {
    enoki::store_unaligned<Value>(mem, value);
}

template <typename Value>
CC_INLINE Value loadAligned(const void *mem) {
    return enoki::load<Value>(mem);
}

template <typename Value>
CC_INLINE void storeAligned(void *mem, const Value &value) {
    enoki::store<Value>(mem, value);
}

template <typename Value>
CC_INLINE Value load(const void *mem, const mask_t<Value> &mask) {
    return enoki::load_unaligned<Value>(mem, mask);
}

template <typename Value>
CC_INLINE void store(void *mem, const Value &value, const mask_t<Value> &mask) {
    enoki::store_unaligned<Value>(mem, value, mask);
}

template <typename Value>
CC_INLINE Value loadAligned(const void *mem, const mask_t<Value> &mask) {
    return enoki::load<Value>(mem, mask);
}

template <typename Value>
CC_INLINE void storeAligned(void *mem, const Value &value, const mask_t<Value> &mask) {
    enoki::store<Value>(mem, value, mask);
}

template <typename Value>
CC_INLINE Value conjugate(const Value &v) {
    return enoki::conj(v);
}

template <typename Value, typename Expr = enoki::expr_t<Value>>
CC_INLINE Vec3<Expr> quatToEuler(const Quat<Value> &q) {
    static const Vec3<Expr> RAD_TO_DEG{math::RAD_TO_DEG};
    return enoki::quat_to_euler_yzx<Vec3<Value>, Value>(q) * RAD_TO_DEG;
}

template <typename Value, typename Expr = enoki::expr_t<Value>>
CC_INLINE Quat<Expr> quatFromEuler(const Vec3<Value> &v) {
    static const Vec3<Expr> DEG_TO_RAD{math::DEG_TO_RAD};
    return enoki::euler_to_quat_yzx<Value>(v * DEG_TO_RAD);
}

template <typename Value, typename Expr = enoki::expr_t<Value>>
CC_INLINE Quat<Expr> quatFromMat3(const Mat3<Value> &m) {
    return enoki::matrix_to_quat<Value, 3>(m);
}

template <typename Value, typename Expr = enoki::expr_t<Value>>
CC_INLINE Quat<Expr> quatFromMat4(const Mat4<Value> &m) {
    return enoki::matrix_to_quat<Value, 4>(m);
}

template <typename Value, typename Expr = enoki::expr_t<Value>>
CC_INLINE Mat3<Expr> mat3FromQuat(const Quat<Value> &q) {
    return enoki::quat_to_matrix<Mat3<Value>, Value>(q);
}

template <typename Value, typename Expr = enoki::expr_t<Value>>
CC_INLINE Mat4<Expr> mat4FromQuat(const Quat<Value> &q) {
    return enoki::quat_to_matrix<Mat4<Value>, Value>(q);
}

template <typename Value, typename Expr = enoki::expr_t<Value>>
CC_INLINE Mat4<Expr> mat4FromRTS(const Quat<Value> &q, const Vec3<Value> &v, const Vec3<Value> &s) {
    return enoki::rts_to_matrix<Value>(q, v, s);
}

template <typename Value, typename Expr = enoki::expr_t<Value>>
CC_INLINE Vec3<Expr> vec3TransformMat4(const Vec3<Value> &v, const Mat4<Value> &m) {
    return enoki::matrix_transform_vector<Value, false>(m, v);
}

template <typename Value, typename Expr = enoki::expr_t<Value>>
CC_INLINE Vec3<Expr> vec3TransformMat4Normal(const Vec3<Value> &v, const Mat4<Value> &m) {
    return enoki::matrix_transform_vector<Value, true>(m, v);
}

///////////////////// Debugging /////////////////////

template <typename Value>
CC_INLINE void print(const Value &v, const String &prefix = "") {
    std::stringstream ss;
    ss << v;
    CC_LOG_DEBUG("%s %s\n", prefix.c_str(), ss.str().c_str());
}

} // namespace vmath
} // namespace cc
