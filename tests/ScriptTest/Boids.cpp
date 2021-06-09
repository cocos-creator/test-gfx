#include "Boids.h"
#include "tests/ScriptTest/Chassis.h"

using namespace cc::vmath; // NOLINT(google-build-using-namespace) when inside cpp files this is usually fine

namespace application {

BoidsOptions BoidsManager::options;
BoidX        BoidsManager::boids;

template <typename Value>
static Vec3<Value> sampleUniformSphere() {
    static PCG32<Value> rng;

    Value theta   = rng.next_float32() * M_PI * 2;
    Value phi     = rng.next_float32() * M_PI * 0.5;
    auto [s1, c1] = sincos(theta);
    auto [s2, c2] = sincos(phi);
    return Vec3<Value>{c1 * s2, s1 * s2, c2};
}

template <typename Value_, typename Value = expr_t<Value_>>
static void update(const Boid<Value_> &boid) {
    static const Vec3<Value> UP{0.F, 1.F, 0.F};
    auto dir = normalize(boid.velocity);
    auto rot = normalize(concat(cross(UP, dir), 1.F + dir.y()));
    // for (size_t i = 0U; i < Value::Size; ++i) {
    //     auto transform = slice(cc::TransformView::views, boid.transform[i]);
    //     transform->setRotation(rot[0].x());
    // }
}

void BoidsManager::init(const BoidsOptions &newOptions) {
    options = newOptions;

    cc::Root *         root   = cc::RootManager::getInstance();
    cc::TransformView *parent = root->createTransform();

    setSlices(boids, options.boidCount);
    for (uint32_t i = 0; i < options.boidCount; ++i) {
        cc::TransformView *transform = root->createTransform();
        cc::ModelView *    model     = root->createModel();
        transform->setParent(parent);
        model->setTransform(transform);

        slice(boids.model, i)     = model->getIdx();
        slice(boids.transform, i) = transform->getIdx();

        Vec3<float> pos = sampleUniformSphere<float>();
        transform->setPosition(pos.x(), pos.y(), 0.5F);
    }

    for (size_t i = 0; i < packets(boids); ++i) {
        auto &&p = packet(boids, i);
        p.velocity = sampleUniformSphere<FloatP>() * options.maxVelocity;
        update(p);
    }
}

void BoidsManager::tick(float globalTime) {
}

} // namespace application
