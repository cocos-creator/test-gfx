#pragma once

#include <cstdint>

namespace application {

struct BoidsOptions {
    uint32_t boidCount;
    float    maxVelocity;
    float    alignmentForce;
    float    cohesionForce;
    float    separationForce;
    float    separationDistance;
    float    flockmateRadius;
    float    startingDelay;
};

class BoidsManager {
public:
    static void init(const BoidsOptions& options);
    static void tick(float globalTime);
    static void destroy();
};

} // namespace application
