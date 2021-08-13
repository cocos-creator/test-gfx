(() => {
    const JSB = typeof jsb !== 'undefined';

    const NATIVE_COMPUTATION = JSB && 1;
    const options = {
        boidCount: 8196,
        maxVelocity: 0.4,
        alignmentForce: 0.002,
        cohesionForce: 0.002,
        separationForce: 0.003,
        separationDistance: 0.1,
        flockmateRadius: 0.9,
        startingDelay: 2,
    };

    const { vec3, quat } = JSB ? glMatrix : require('./gl-matrix');
    const { root } = JSB ? chassis : require('./chassis');

    let tick = null;

    if (NATIVE_COMPUTATION) {

        jsb.BoidsManager.init(options);

        tick = (gTimeInMS) => {
            jsb.BoidsManager.tick(gTimeInMS);

            root.render();
        };

    } else {

        const parent = root.createTransform();
        const tempVec3a = vec3.create();
        const tempVec3b = vec3.create();
        const tempQuat = quat.create();
        const tempVec4 = quat.create();

        const HALF = vec3.set(vec3.create(), 0.5, 0.5, 0.5);

        const alignment = vec3.create();
        const cohesion = vec3.create();
        const separation = vec3.create();

        const getBoundaryFade = (v, clamp) => {
            return Math.min(clamp,
                1 - Math.abs(v[0]),
                1 - Math.abs(v[1]),
                1 - Math.abs(v[2])) / clamp;
        };

        const wrapBound = (v) => {
            if (v[0] > 1) { v[0] -= 2; }
            else if (v[0] < -1) { v[0] += 2; }
            if (v[1] > 1) { v[1] -= 2; }
            else if (v[1] < -1) { v[1] += 2; }
            if (v[2] > 1) { v[2] -= 2; }
            else if (v[2] < -1) { v[2] += 2; }
        };

        const clampLength = (v, max) => {
            const l = vec3.length(v);
            if (l > max) { vec3.scale(v, v, max / l); }
        };

        const applyForce = (acc, velocity, force, strength) => {
            vec3.scale(tempVec3a, force, options.maxVelocity / vec3.length(force));
            clampLength(vec3.subtract(tempVec3a, tempVec3a, velocity), strength);
            vec3.add(acc, acc, tempVec3a);
        };

        class Boid {
            model = root.createModel();
            transform = root.createTransform();

            acceleration = vec3.create();
            velocity = vec3.create();

            constructor() {
                this.transform.setParent(parent);
                this.model.setTransform(this.transform);

                vec3.random(tempVec3a);
                // tempVec3a[2] = Math.abs(tempVec3a[2]);
                this.transform.setPosition(tempVec3a);
                vec3.random(this.velocity, Math.random() * options.maxVelocity);

                this.update();
            }

            update() {
                vec3.normalize(tempVec4, this.velocity);
                quat.normalize(tempQuat, quat.set(tempQuat, tempVec4[2], 0, -tempVec4[0], 1 + tempVec4[1]));
                this.transform.setRotation(tempQuat);

                // vec3.normalize(tempVec4, this.acceleration); // visualize acceleration

                vec3.scaleAndAdd(tempVec4, HALF, tempVec4, 0.5);
                tempVec4[3] = getBoundaryFade(this.transform.getPosition(), 0.1);
                this.model.setColor(tempVec4);
            }
        }

        const boids = [];
        for (let i = 0; i < options.boidCount; ++i) {
            boids.push(new Boid());
        }

        let lastTime = -1;

        tick = (gTimeInMS) => {
            if (lastTime < options.startingDelay * 1000) {
                lastTime = gTimeInMS;
                root.render();
                return;
            }

            dt = (gTimeInMS - lastTime) / 1000;
            lastTime = gTimeInMS;
            let distance = 0;

            for (const b1 of boids) {
                vec3.set(alignment, 0, 0, 0);
                vec3.set(cohesion, 0, 0, 0);
                vec3.set(separation, 0, 0, 0);
                let alignmentActive = false;
                let cohesionActive = false;
                let separationActive = false;

                for (const b2 of boids) {
                    if (b1 === b2) { continue; }
                    vec3.subtract(tempVec3a, b2.transform.getPosition(), b1.transform.getPosition());
                    distance = Math.max(0.01, vec3.length(tempVec3a) - 0.1);

                    if (distance < options.separationDistance) {
                        vec3.scale(tempVec3b, tempVec3a, -1 / distance);
                        vec3.add(separation, separation, tempVec3b);
                        separationActive = true;
                    }

                    if (distance < options.flockmateRadius) {
                        vec3.add(cohesion, cohesion, tempVec3a);
                        cohesionActive = true;
                        vec3.add(alignment, alignment, b2.velocity);
                        alignmentActive = true;
                    }
                }

                const { acceleration: acc, velocity: vel } = b1;
                vec3.set(acc, 0, 0, 0);
                if (alignmentActive) { applyForce(acc, vel, alignment, options.alignmentForce); }
                if (cohesionActive) { applyForce(acc, vel, cohesion, options.cohesionForce); }
                if (separationActive) { applyForce(acc, vel, separation, options.separationForce); }
            }

            for (const b of boids) {
                clampLength(vec3.add(b.velocity, b.velocity, b.acceleration), options.maxVelocity);
                wrapBound(vec3.scaleAndAdd(tempVec3a, b.transform.getPosition(), b.velocity, dt));
                b.transform.setPosition(tempVec3a);
                b.update();
            }

            root.render();
        };
    }

    if (JSB) { window.gameTick = tick; }
    else { tick(); console.log('tick finished.'); }

})();
