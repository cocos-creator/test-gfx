
#pragma once

#include <enoki/matrix.h>
#include <enoki/quaternion.h>

NAMESPACE_BEGIN(enoki)

template <typename Quat, typename T, typename Expr = expr_t<T>>
ENOKI_INLINE Quat euler_to_quat(const Array<T> &euler) {
    auto [sx, cx] = sincos(euler.x() * 0.5f);
    auto [sy, cy] = sincos(euler.y() * 0.5f);
    auto [sz, cz] = sincos(euler.z() * 0.5f);

    Expr cc = cx * cy;
    Expr sc = sx * cy;
    Expr cs = cx * sy;
    Expr ss = sx * sy;

    Expr x = fmadd(sc, cz, cs * sz);
    Expr y = fmadd(cs, cz, sc * sz);
    Expr z = fmsub(cc, sz, ss * cz);
    Expr w = fmsub(cc, cz, ss * sz);

    return Quat(x, y, z, w);
}

NAMESPACE_END(enoki)
