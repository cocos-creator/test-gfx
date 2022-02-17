
#pragma once

#include <enoki/matrix.h>
#include <enoki/quaternion.h>

NAMESPACE_BEGIN(enoki)

// ZYX rotation order, same as quat_to_euler
template <typename T, typename E = expr_t<T>>
ENOKI_INLINE Quaternion<E> euler_to_quat(const Array<T, 3> &euler) {
    auto [sx, cx] = sincos(euler.x() * 0.5f);
    auto [sy, cy] = sincos(euler.y() * 0.5f);
    auto [sz, cz] = sincos(euler.z() * 0.5f);

    E cc = cx * cy;
    E sc = sx * cy;
    E cs = cx * sy;
    E ss = sx * sy;

    return Quaternion<E>(
        fmsub(sc, cz, cs * sz),
        fmadd(cs, cz, sc * sz),
        fmsub(cc, sz, ss * cz),
        fmadd(cc, cz, ss * sz)
    );
}

template <typename T, typename E = expr_t<T>>
ENOKI_INLINE Matrix<E, 4> rts_to_matrix(const Quaternion<T> &q_, const Array<T, 3> &v, const Array<T, 3> &s) {
    using Vector = Array<E, 3>;
    auto q = q_ * scalar_t<T>(M_SQRT2);

    E xx = q.x() * q.x(), yy = q.y() * q.y(), zz = q.z() * q.z();
    E xy = q.x() * q.y(), xz = q.x() * q.z(), yz = q.y() * q.z();
    E xw = q.x() * q.w(), yw = q.y() * q.w(), zw = q.z() * q.w();

    Vector row0(1.f - (yy + zz), xy + zw, xz - yw);
    Vector row1(xy - zw, 1.f - (xx + zz), yz + xw);
    Vector row2(xz + yw, yz - xw, 1.f - (xx + yy));

    return Matrix<E, 4>(
        concat(row0 * s, 0.f),
        concat(row1 * s, 0.f),
        concat(row2 * s, 0.f),
        Array<E, 4>{v.x(), v.y(), v.z(), 1.f}
    );
}

template <typename T, bool is_normal = false, typename E = expr_t<T>>
ENOKI_INLINE Array<E, 3> matrix_transform_vector(const Matrix<T, 4> &m, const Array<T, 3> &v) {
    E w;
    if constexpr (is_normal) {
        w = scalar_t<T>(0.f);
    } else {
        w = scalar_t<T>(1.f);
    }

    auto res = m * Array<E, 4>(v.x(), v.y(), v.z(), w);
    E rhw = select(res.w() == 0.f, scalar_t<T>(1.f), abs(rcp(res.w())));

    return Array<E, 3>(res.x() * rhw, res.y() * rhw, res.z() * rhw);
}

//////////// Variation: YZX rotation order ////////////

template <typename Vector, typename T, typename Expr = expr_t<T>>
ENOKI_INLINE Vector quat_to_euler_yzx(const Quaternion<T> &q) {
    // roll (x-axis rotation)
    Expr q_z_2 = sqr(q.z());

    Expr sinr_cosp = 2 * fmsub(q.w(), q.x(), q.y() * q.z());
    Expr cosr_cosp = fnmadd(2, fmadd(q.x(), q.x(), q_z_2), 1);
    Expr roll = atan2(sinr_cosp, cosr_cosp);

    // pitch (z-axis rotation)
    Expr sinp = 2 * fmadd(q.x(), q.y(), q.z() * q.w());
    Expr pitch;
    if (abs(sinp) >= 1)
        pitch = copysign(Expr(M_PI / 2), sinp); // use 90 degrees if out of range
    else
        pitch = asin(sinp);

    // yaw (y-axis rotation)
    Expr siny_cosp = 2 * fmsub(q.y(), q.w(), q.x() * q.z());
    Expr cosy_cosp = fnmadd(2, fmadd(q.y(), q.y(), q_z_2), 1);
    Expr yaw = atan2(siny_cosp, cosy_cosp);

    return Vector(roll, yaw, pitch);
}

template <typename T, typename E = expr_t<T>>
ENOKI_INLINE Quaternion<E> euler_to_quat_yzx(const Array<T, 3> &euler) {
    auto [sx, cx] = sincos(euler.x() * 0.5f);
    auto [sy, cy] = sincos(euler.y() * 0.5f);
    auto [sz, cz] = sincos(euler.z() * 0.5f);

    E cc = cx * cy;
    E sc = sx * cy;
    E cs = cx * sy;
    E ss = sx * sy;

    return Quaternion<E>(
        fmadd(sc, cz, cs * sz),
        fmadd(cs, cz, sc * sz),
        fmsub(cc, sz, ss * cz),
        fmsub(cc, cz, ss * sz)
    );
}

NAMESPACE_END(enoki)
