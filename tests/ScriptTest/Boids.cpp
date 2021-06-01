#include "Boids.h"
#include "tests/ScriptTest/Chassis.h"

using cc::vmath::setSlices;

namespace application {

BoidsOptions BoidsManager::options;
BoidX        BoidsManager::boids;

void BoidsManager::init(const BoidsOptions& newOptions) {
    options = newOptions;

    cc::Root* root = cc::RootManager::getInstance();

    setSlices(boids, options.boidCount);
    for (size_t i = 0; i < slices(boids); ++i) {
        packet(boids, i) = BoidP{};
    }

    cc::TransformView *parent = root->createTransform();
    for (uint32_t i = 0; i < options.boidCount; ++i) {
        cc::TransformView *transform = root->createTransform();
        cc::ModelView *model = root->createModel();
        transform->setParent(parent);
        model->setTransform(transform);

        slice(boids.model, i) = model->getIdx();
        slice(boids.transform, i) = transform->getIdx();
    }
}

void BoidsManager::tick(float globalTime) {
}

}  // namespace application
