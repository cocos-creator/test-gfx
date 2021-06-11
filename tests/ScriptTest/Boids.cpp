#include "Boids.h"

#include <vector>
#include "Chassis.h"
#include "base/Log.h"

using namespace cc::vmath; // NOLINT(google-build-using-namespace) when inside cpp files this is usually fine

namespace application {

template <typename Value>
struct Boid {
    using Index = cc::vmath::replace_scalar_t<Value, cc::vmath::Index>;
    using Vec3  = cc::vmath::Vec3<Value>;

    Index model{-1};
    Index transform{-1};

    Vec3 acceleration = cc::vmath::zero<Vec3>();
    Vec3 velocity     = cc::vmath::zero<Vec3>();

    // cache states
    Vec3 position = cc::vmath::zero<Vec3>();

    // NOLINTNEXTLINE(google-explicit-constructor) false positive when involving __VA_ARGS__
    CC_VMATH_STRUCT(Boid, model, transform, acceleration, velocity, position)
};

CC_VMATH_STRUCT_SUPPORT_1(application, Boid, model, transform, acceleration, velocity, position)

using BoidF = Boid<float>;
using BoidP = Boid<cc::vmath::FloatP>;
using BoidX = Boid<cc::vmath::FloatX>;

namespace {
std::vector<cc::TransformView *> transformViews;
std::vector<cc::ModelView *>     modelViews;
BoidsOptions                     options;
BoidX                            boids;
} // namespace

template <typename Value>
static Vec3<Value> sampleUniformSphere(PCG32<Value> &rng) {
    Value theta  = rng.next_float32() * cc::math::PI * 2.F;
    Value cosPhi = rng.next_float32() * 2.F - 1.F;
    Value sinPhi = sqrt(1.F - sqr(cosPhi));
    auto [s, c]  = sincos(theta);
    return Vec3<Value>{c * sinPhi, s * sinPhi, cosPhi};
}

template <typename Value_, typename Value = expr_t<Value_>>
static float getBoundaryFade(Vec3<Value_> v, float clamp) {
    static const Vec3<Value> OFFSET{0.F, 0.F, -0.5F};
    static const Vec3<Value> SCALE{1.F, 1.F, 2.F};

    return hmin(concat(1.F - abs(v + OFFSET) * SCALE, clamp)) / clamp;
}

template <typename Value_, typename Value = expr_t<Value_>>
static void update(const Boid<Value_> &boid) {
    auto dir = normalize(boid.velocity);
    auto rot = normalize(Vec4<Value>{dir.z(), 0.F, -dir.x(), 1.F + dir.y()});

    // visualize acceleration
    // dir = normalize(boid.acceleration);

    dir = fmadd(dir, 0.5F, 0.5F);
    for (size_t i = 0U; i < Value::Size; ++i) {
        if (!boid.transform[i]) break; // after the last one
        auto * transformView = transformViews[boid.transform[i]];
        auto * modelView     = modelViews[boid.model[i]];
        auto &&pos           = slice(boid.position, i);
        transformView->setRotation(slice(rot, i));
        transformView->setPosition(pos);
        modelView->setColor(concat(slice(dir, i), getBoundaryFade(pos, 0.1F)));
    }
}

template <typename Value_, typename Value = expr_t<Value_>>
static Vec3<Value> &&wrapBound(Vec3<Value_> &&v) {
    v.x()[v.x() > 1.F] -= 2.F;
    v.x()[v.x() < -1.F] += 2.F;
    v.y()[v.y() > 1.F] -= 2.F;
    v.y()[v.y() < -1.F] += 2.F;
    v.z()[v.z() > 1.F] -= 1.F;
    v.z()[v.z() < 0.F] += 1.F;
    return std::move(v);
}

template <typename Value_, typename Value = expr_t<Value_>>
static Vec3<Value> clampLength(const Vec3<Value_> &v, float max) {
    auto l = dot(v, v);
    return select(l > sqr(max), v * max * rsqrt(l), v);
}

template <typename Value_, typename Value = expr_t<Value_>>
static Vec3<Value> applyForce(const Vec3<Value_> &velocity, const Vec3<Value> &force, float strength) {
    return clampLength(fmsub(normalize(force), options.maxVelocity, velocity), strength);
}

void BoidsManager::init(const BoidsOptions &newOptions) {
    options = newOptions;

    cc::Root *         root   = cc::RootManager::getInstance();
    cc::TransformView *parent = root->createTransform();
    transformViews.push_back(parent);

    setSlices(boids, options.boidCount);
    for (uint32_t i = 0; i < options.boidCount; ++i) {
        cc::TransformView *transform = root->createTransform();
        cc::ModelView *    model     = root->createModel();

        transform->setParent(parent);
        model->setTransform(transform);

        slice(boids.model, i)     = static_cast<Index>(modelViews.size());
        slice(boids.transform, i) = static_cast<Index>(transformViews.size());

        transformViews.push_back(transform);
        modelViews.push_back(model);
    }

    CC_LOG_INFO("Boids count: %d", options.boidCount);

    PCG32<FloatP> rng(std::time(nullptr)); // make every run unique
    for (size_t i = 0; i < packets(boids); ++i) {
        auto &&p       = packet(boids, i);
        p.position     = sampleUniformSphere(rng);
        p.position.z() = abs(p.position.z());
        p.velocity     = sampleUniformSphere(rng) * options.maxVelocity;
        update(p);
    }
}

void BoidsManager::destroy() {
    for (auto *transform : transformViews) {
        CC_SAFE_DELETE(transform);
    }
    transformViews.clear();

    for (auto *model : modelViews) {
        CC_SAFE_DELETE(model);
    }
    modelViews.clear();
}

void BoidsManager::tick(float globalTime) {
    static float lastTime = -1.F;
    if (lastTime < options.startingDelay * 1000.F) {
        lastTime = globalTime;
        return;
    }

    float dt = (globalTime - lastTime) / 1000.F;
    lastTime = globalTime;

    static const auto ZERO = zero<Vec3P>();

    static Vec3P alignment;
    static Vec3P cohesion;
    static Vec3P separation;
    static MaskP alignmentActive;
    static MaskP cohesionActive;
    static MaskP separationActive;

    for (size_t i = 0; i < packets(boids); ++i) {
        auto &&b1        = packet(boids, i);
        alignment        = zero<Vec3P>();
        cohesion         = zero<Vec3P>();
        separation       = zero<Vec3P>();
        alignmentActive  = false;
        cohesionActive   = false;
        separationActive = false;

        for (size_t j = 0; j < slices(boids); ++j) {
            auto &&b2        = slice(boids, j);
            Vec3P  dir       = b2.position - b1.position;
            FloatP lengthSq  = dot(dir, dir);
            FloatP distance  = max(0.01F, sqrt(lengthSq) - 0.1F);
            MaskP  allOthers = lengthSq > 0.F;

            MaskP shouldSeparate = allOthers & (distance < options.separationDistance);
            separation += select(shouldSeparate, dir * rcp(-distance), ZERO);
            separationActive |= shouldSeparate;

            MaskP shouldConnect = allOthers & (distance < options.flockmateRadius);
            cohesion += select(shouldConnect, dir, ZERO);
            cohesionActive |= shouldConnect;
            alignment += select(shouldConnect, b2.velocity, ZERO);
            alignmentActive |= shouldConnect;
        }

        b1.acceleration = select(alignmentActive, applyForce(b1.velocity, alignment, options.alignmentForce), ZERO);
        b1.acceleration += select(cohesionActive, applyForce(b1.velocity, cohesion, options.cohesionForce), ZERO);
        b1.acceleration += select(separationActive, applyForce(b1.velocity, separation, options.separationForce), ZERO);
    }

    for (size_t i = 0; i < packets(boids); ++i) {
        auto &&b   = packet(boids, i);
        b.velocity = clampLength(b.velocity + b.acceleration, options.maxVelocity);
        b.position = wrapBound(b.position + b.velocity * dt);
        update(b);
    }
}

} // namespace application
