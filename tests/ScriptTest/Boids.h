#pragma once

#include "Chassis.h"

namespace application {

template <typename Value>
struct Boid {
    using Index = cc::vmath::replace_scalar_t<Value, cc::vmath::Index>;
    using Vec3  = cc::vmath::Vec3<Value>;

    Index model{-1};
    Index transform{-1};

    Vec3 acceleration = cc::vmath::zero<Vec3>();
    Vec3 velocity     = cc::vmath::zero<Vec3>();

    // NOLINTNEXTLINE(google-explicit-constructor) false positive when involving __VA_ARGS__
    CC_VMATH_STRUCT(Boid, model, transform, acceleration, velocity)
};

CC_VMATH_STRUCT_SUPPORT_1(application, Boid, model, transform, acceleration, velocity)

using BoidF = Boid<float>;
using BoidP = Boid<cc::vmath::FloatP>;
using BoidX = Boid<cc::vmath::FloatX>;

struct BoidsOptions {
    uint32_t boidCount;
    float    maxVelocity;
    float    alignmentForce;
    float    cohesionForce;
    float    separationForce;
    float    separationDistance;
    float    flockmateRadius;
};

class BoidsManager {
public:
    static void init(const BoidsOptions& options);
    static void tick(float globalTime);

private:
    static BoidsOptions options;
    static BoidX boids;
};

}  // namespace application
