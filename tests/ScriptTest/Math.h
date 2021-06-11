#pragma once

#include "enoki/array_router.h"
#include "enoki/array_traits.h"
#include "enoki/dynamic.h"
#include "enoki/matrix.h"
#include "enoki/quaternion.h"
#include "enoki/random.h"
#include "supp_math.h"

#include <algorithm>
#include <sstream>
#include "base/Log.h"
#include "base/Macros.h"
#include "base/memory/Memory.h"

namespace cc {
namespace vmath {

#define CC_VMATH_CALL_SUPPORT_FRIEND ENOKI_CALL_SUPPORT_FRIEND

#define CC_VMATH_STRUCT         ENOKI_STRUCT
#define CC_VMATH_STRUCT_SUPPORT ENOKI_STRUCT_SUPPORT

#define CC_VMATH_CALL_SUPPORT_BEGIN  ENOKI_CALL_SUPPORT_BEGIN
#define CC_VMATH_CALL_SUPPORT_METHOD ENOKI_CALL_SUPPORT_METHOD
#define CC_VMATH_CALL_SUPPORT_GETTER ENOKI_CALL_SUPPORT_GETTER
#define CC_VMATH_CALL_SUPPORT_END    ENOKI_CALL_SUPPORT_END

#define CC_VMATH_STRUCT_SUPPORT_1(ns, Struct, ...) \
    } /* namespace ns */                           \
    ENOKI_STRUCT_SUPPORT(ns::Struct, __VA_ARGS__)  \
    namespace ns {

#define CC_VMATH_STRUCT_SUPPORT_2(ns1, ns2, Struct, ...) \
    } /* namespace ns2 */                                \
    } /* namespace ns1 */                                \
    ENOKI_STRUCT_SUPPORT(ns1::ns2::Struct, __VA_ARGS__)  \
    namespace ns1 {                                      \
    namespace ns2 {

#define CC_VMATH_CALL_SUPPORT_BEGIN_1(ns, Struct) \
    } /* namespace ns */                          \
    ENOKI_CALL_SUPPORT_BEGIN(ns::Struct)
#define CC_VMATH_CALL_SUPPORT_BEGIN_2(ns1, ns2, Struct) \
    } /* namespace ns2 */                               \
    } /* namespace ns1 */                               \
    ENOKI_CALL_SUPPORT_BEGIN(ns1::ns2::Struct)

#define CC_VMATH_CALL_SUPPORT_END_1(ns, Struct) \
    ENOKI_CALL_SUPPORT_END(ns::Struct)          \
    namespace ns {
#define CC_VMATH_CALL_SUPPORT_END_2(ns1, ns2, Struct) \
    ENOKI_CALL_SUPPORT_END(ns1::ns2::Struct)          \
    namespace ns1 {                                   \
    namespace ns2 {

/// Generic array class, which broadcasts from the outer to inner dimensions
template <typename Value, size_t Size>
using Array = enoki::Array<Value, Size>;

/// Generic array class, which broadcasts from the inner to outer dimensions
template <typename Value, size_t Size>
using Packet = enoki::Packet<Value, Size>;

/// Dynamically sized array
template <typename Value>
using DynamicArray = enoki::DynamicArray<Value>;

/// PCG32 pseudorandom number generator proposed by Melissa O'Neill
template <typename Value>
using PCG32 = enoki::PCG32<Value>;

template <typename Value>
using divisor = enoki::divisor<Value>;

/// Type trait to compute the type of an arithmetic expression involving Ts...
template <typename... Ts>
using expr_t = enoki::expr_t<Ts...>;

/// Type trait to access the base scalar type underlying a potentially nested array
template <typename T>
using scalar_t = enoki::scalar_t<T>;

/// Type trait to access the mask type underlying an array
template <typename T, bool CopyFlags = true>
using mask_t = enoki::mask_t<T, CopyFlags>;

/// Unsigned integer-based version of a given array class
template <typename T, bool CopyFlags = true>
using uint_array_t = enoki::uint_array_t<T, CopyFlags>;

/// Floating point-based version of a given array class
template <typename T, bool CopyFlags = true>
using float_array_t = enoki::float_array_t<T, CopyFlags>;

template <typename T, typename Value, bool CopyFlags = true>
using replace_scalar_t = enoki::replace_scalar_t<T, Value, CopyFlags>;

constexpr size_t PACKET_SIZE = enoki::array_default_size < 8U ? 16U : enoki::array_default_size * 2U;

using Index  = int;
using IndexP = Packet<Index, PACKET_SIZE>;
using IndexX = DynamicArray<IndexP>;

using FloatP = Packet<float, PACKET_SIZE>;
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

using MaskF = mask_t<float>;
using MaskP = mask_t<FloatP>;
using MaskX = mask_t<FloatX>;

using enoki::abs;         // NOLINT(misc-unused-using-decls)
using enoki::all;         // NOLINT(misc-unused-using-decls)
using enoki::all_inner;   // NOLINT(misc-unused-using-decls)
using enoki::andnot;      // NOLINT(misc-unused-using-decls)
using enoki::any;         // NOLINT(misc-unused-using-decls)
using enoki::any_inner;   // NOLINT(misc-unused-using-decls)
using enoki::ceil;        // NOLINT(misc-unused-using-decls)
using enoki::concat;      // NOLINT(misc-unused-using-decls)
using enoki::count;       // NOLINT(misc-unused-using-decls)
using enoki::count_inner; // NOLINT(misc-unused-using-decls)
using enoki::cross;       // NOLINT(misc-unused-using-decls)
using enoki::dot;         // NOLINT(misc-unused-using-decls)
using enoki::eq;          // NOLINT(misc-unused-using-decls)
using enoki::floor;       // NOLINT(misc-unused-using-decls)
using enoki::fmadd;       // NOLINT(misc-unused-using-decls)
using enoki::fmaddsub;    // NOLINT(misc-unused-using-decls)
using enoki::fmsub;       // NOLINT(misc-unused-using-decls)
using enoki::fmsubadd;    // NOLINT(misc-unused-using-decls)
using enoki::fnmadd;      // NOLINT(misc-unused-using-decls)
using enoki::fnmsub;      // NOLINT(misc-unused-using-decls)
using enoki::high;        // NOLINT(misc-unused-using-decls)
using enoki::hmax;        // NOLINT(misc-unused-using-decls)
using enoki::hmax_inner;  // NOLINT(misc-unused-using-decls)
using enoki::hmean;       // NOLINT(misc-unused-using-decls)
using enoki::hmean_inner; // NOLINT(misc-unused-using-decls)
using enoki::hmin;        // NOLINT(misc-unused-using-decls)
using enoki::hmin_inner;  // NOLINT(misc-unused-using-decls)
using enoki::hprod;       // NOLINT(misc-unused-using-decls)
using enoki::hprod_inner; // NOLINT(misc-unused-using-decls)
using enoki::hsum;        // NOLINT(misc-unused-using-decls)
using enoki::hsum_inner;  // NOLINT(misc-unused-using-decls)
using enoki::isinf;       // NOLINT(misc-unused-using-decls)
using enoki::isnan;       // NOLINT(misc-unused-using-decls)
using enoki::linspace;    // NOLINT(misc-unused-using-decls)
using enoki::low;         // NOLINT(misc-unused-using-decls)
using enoki::lzcnt;       // NOLINT(misc-unused-using-decls)
using enoki::max;         // NOLINT(misc-unused-using-decls)
using enoki::min;         // NOLINT(misc-unused-using-decls)
using enoki::mulhi;       // NOLINT(misc-unused-using-decls)
using enoki::neq;         // NOLINT(misc-unused-using-decls)
using enoki::norm;        // NOLINT(misc-unused-using-decls)
using enoki::normalize;   // NOLINT(misc-unused-using-decls)
using enoki::partition;   // NOLINT(misc-unused-using-decls)
using enoki::popcnt;      // NOLINT(misc-unused-using-decls)
using enoki::psum;        // NOLINT(misc-unused-using-decls)
using enoki::psum_inner;  // NOLINT(misc-unused-using-decls)
using enoki::rcp;         // NOLINT(misc-unused-using-decls)
using enoki::reverse;     // NOLINT(misc-unused-using-decls)
using enoki::rol;         // NOLINT(misc-unused-using-decls)
using enoki::rol_array;   // NOLINT(misc-unused-using-decls)
using enoki::ror;         // NOLINT(misc-unused-using-decls)
using enoki::ror_array;   // NOLINT(misc-unused-using-decls)
using enoki::round;       // NOLINT(misc-unused-using-decls)
using enoki::rsqrt;       // NOLINT(misc-unused-using-decls)
using enoki::select;      // NOLINT(misc-unused-using-decls)
using enoki::shuffle;     // NOLINT(misc-unused-using-decls)
using enoki::sign;        // NOLINT(misc-unused-using-decls)
using enoki::sl;          // NOLINT(misc-unused-using-decls)
using enoki::sqr;         // NOLINT(misc-unused-using-decls)
using enoki::sqrt;        // NOLINT(misc-unused-using-decls)
using enoki::sr;          // NOLINT(misc-unused-using-decls)
using enoki::trunc;       // NOLINT(misc-unused-using-decls)
using enoki::tzcnt;       // NOLINT(misc-unused-using-decls)

using enoki::acos;      // NOLINT(misc-unused-using-decls)
using enoki::asin;      // NOLINT(misc-unused-using-decls)
using enoki::atan;      // NOLINT(misc-unused-using-decls)
using enoki::atan2;     // NOLINT(misc-unused-using-decls)
using enoki::clamp;     // NOLINT(misc-unused-using-decls)
using enoki::cos;       // NOLINT(misc-unused-using-decls)
using enoki::cot;       // NOLINT(misc-unused-using-decls)
using enoki::exp;       // NOLINT(misc-unused-using-decls)
using enoki::fmod;      // NOLINT(misc-unused-using-decls)
using enoki::identity;  // NOLINT(misc-unused-using-decls)
using enoki::lerp;      // NOLINT(misc-unused-using-decls)
using enoki::log;       // NOLINT(misc-unused-using-decls)
using enoki::pow;       // NOLINT(misc-unused-using-decls)
using enoki::sin;       // NOLINT(misc-unused-using-decls)
using enoki::sincos;    // NOLINT(misc-unused-using-decls)
using enoki::tan;       // NOLINT(misc-unused-using-decls)
using enoki::transpose; // NOLINT(misc-unused-using-decls)
using enoki::zero;      // NOLINT(misc-unused-using-decls)

using enoki::packet;  // NOLINT(misc-unused-using-decls)
using enoki::packets; // NOLINT(misc-unused-using-decls)
using enoki::slice;   // NOLINT(misc-unused-using-decls)
using enoki::slices;  // NOLINT(misc-unused-using-decls)

using enoki::compress; // NOLINT(misc-unused-using-decls)
using enoki::gather;   // NOLINT(misc-unused-using-decls)
/// Scatter operations with an array or other data structure as target
using enoki::scatter; // NOLINT(misc-unused-using-decls)

template <bool keepValue = false, typename Value>
CC_INLINE void setSlices(Value &v, size_t size) {
    // by default previous values are not preserved when enlarging the array
    if constexpr (keepValue) {
        if (size > enoki::slices(v)) {
            auto temp = v;
            enoki::set_slices<Value>(v, size);
            for (size_t i = 0; i < enoki::packets(temp); ++i) {
                enoki::packet(v, i) = enoki::packet(temp, i);
            }
            for (size_t i = enoki::packets(temp); i < enoki::packets(v); ++i) {
                enoki::packet(v, i) = typename Value::Packet{};
            }
        } else {
            enoki::set_slices<Value>(v, size);
        }
    } else {
        enoki::set_slices<Value>(v, size);
    }
}

/// Conflict-free scatter-add update
template <size_t Stride = 0, bool Packed = true, bool IsPermute = false, typename Target, typename Index, typename Value,
          typename Mask = mask_t<Index>, enoki::enable_if_t<enoki::is_dynamic_v<Target>> = 0>
CC_INLINE void scatterAdd(Target &target, const Value &value, const Index &index, const enoki::identity_t<Mask> &mask = true) {
    enoki::scatter_add<Stride, Packed, IsPermute, Target, Index, Value, Mask>(target, value, index, mask);
}

template <typename Target, typename Source>
CC_INLINE Target reinterpret(const Source &src) {
    return enoki::reinterpret_array(src);
}

/// Load an array from unaligned memory
template <typename Value>
CC_INLINE Value load(const void *mem) {
    return enoki::load_unaligned<Value>(mem);
}

/// Store an array to unaligned memory
template <typename Value>
CC_INLINE void store(void *mem, const Value &value) {
    enoki::store_unaligned<Value>(mem, value);
}

/// Load an array from aligned memory
template <typename Value>
CC_INLINE Value loadAligned(const void *mem) {
    return enoki::load<Value>(mem);
}

/// Store an array to aligned memory
template <typename Value>
CC_INLINE void storeAligned(void *mem, const Value &value) {
    enoki::store<Value>(mem, value);
}

/// Load an array from unaligned memory
template <typename Value>
CC_INLINE Value load(const void *mem, const mask_t<Value> &mask) {
    return enoki::load_unaligned<Value>(mem, mask);
}

/// Store an array to unaligned memory
template <typename Value>
CC_INLINE void store(void *mem, const Value &value, const mask_t<Value> &mask) {
    enoki::store_unaligned<Value>(mem, value, mask);
}

/// Load an array from aligned memory
template <typename Value>
CC_INLINE Value loadAligned(const void *mem, const mask_t<Value> &mask) {
    return enoki::load<Value>(mem, mask);
}

/// Store an array to aligned memory
template <typename Value>
CC_INLINE void storeAligned(void *mem, const Value &value, const mask_t<Value> &mask) {
    enoki::store<Value>(mem, value, mask);
}

/// Conflict-free modification operation
template <typename Arg, size_t Stride = sizeof(enoki::scalar_t<Arg>),
          typename Func, typename Index, typename... Args>
CC_INLINE void transform(void *mem, const Index &index, Func &&func, Args &&...args) {
    enoki::transform<Arg, Stride, Func, Index, Args...>(mem, index, func, args...);
}

template <typename Value, typename Expr = enoki::expr_t<Value>>
CC_INLINE Expr conjugate(const Value &v) {
    return enoki::conj(v);
}

template <typename Value, typename Expr = enoki::expr_t<Value>>
CC_INLINE Vec3<Expr> quatToEuler(const Quat<Value> &q) {
    return enoki::rad_to_deg(enoki::quat_to_euler_yzx<Vec3<Value>, Value>(q));
}

template <typename Value, typename Expr = enoki::expr_t<Value>>
CC_INLINE Quat<Expr> quatFromEuler(const Vec3<Value> &v) {
    return enoki::euler_to_quat_yzx<Value>(enoki::deg_to_rad(v));
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

CC_INLINE std::string processorFeatures() {
    std::ostringstream oss;

    oss << "Enabled processor features:";

    if (enoki::has_avx512f) oss << " avx512f";
    if (enoki::has_avx512cd) oss << " avx512cd";
    if (enoki::has_avx512dq) oss << " avx512dq";
    if (enoki::has_avx512vl) oss << " avx512vl";
    if (enoki::has_avx512bw) oss << " avx512bw";
    if (enoki::has_avx512pf) oss << " avx512pf";
    if (enoki::has_avx512er) oss << " avx512er";
    if (enoki::has_avx512vpopcntdq) oss << " avx512vpopcntdq";
    if (enoki::has_avx2) oss << " avx2";
    if (enoki::has_avx) oss << " avx";
    if (enoki::has_fma) oss << " fma";
    if (enoki::has_f16c) oss << " f16c";
    if (enoki::has_sse42) oss << " sse4.2";
    if (enoki::has_x86_64) oss << " x86_64";
    if (enoki::has_x86_32) oss << " x86";
    if (enoki::has_neon) oss << " neon";
    if (enoki::has_arm_32) oss << " arm";
    if (enoki::has_arm_64) oss << " aarch64";

    return oss.str();
}

} // namespace vmath
} // namespace cc
