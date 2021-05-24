(() => {
    const JSB = typeof jsb !== 'undefined';

    const { vec3, quat } = JSB ? glMatrix : require('./gl-matrix');
    const { root } = JSB ? chassis : require('./chassis');

    const parent = root.createTransform();
    const tempVec3a = vec3.create();
    const tempVec3b = vec3.create();
    const tempQuat = quat.create();

    const UP = vec3.set(vec3.create(), 0, 1, 0);
    const HALF = vec3.set(vec3.create(), 0.5, 0.5, 0.5);

    const alignment = vec3.create();
    const cohesion = vec3.create();
    const separation = vec3.create();

    const maxVelocity = 0.4;
    const alignmentForce = 0.002;
    const cohesionForce = 0.002;
    const separationForce = 0.003;
    const separationDistance = 0.1;
    const flockmateRadius = 0.5;

    class Boid {
        model = root.createModel();
        transform = root.createTransform();

        acceleration = vec3.create();
        velocity = vec3.create();

        constructor() {
            this.transform.setParent(parent);
            this.model.setTransform(this.transform);

            const theta = Math.random() * Math.PI * 2;
            const phi = Math.random() * Math.PI * 0.5;

            this.transform.setPosition(
                Math.cos(theta) * Math.sin(phi),
                Math.sin(theta) * Math.sin(phi),
                Math.cos(phi));

            vec3.random(this.velocity, Math.random() * maxVelocity);

            this.update();
        }

        update() {
            vec3.normalize(tempVec3a, this.velocity);
            const dot = vec3.dot(UP, tempVec3a);
            vec3.cross(tempVec3b, UP, tempVec3a);
            quat.normalize(tempQuat, quat.set(tempQuat, tempVec3b[0], tempVec3b[1], tempVec3b[2], 1 + dot));
            this.transform.setRotation(tempQuat);

            // vec3.normalize(tempVec31, this.acceleration); // visualize acceleration
            vec3.scaleAndAdd(tempVec3a, HALF, tempVec3a, 0.5);
            this.model.setColor(tempVec3a[0], tempVec3a[1], tempVec3a[2], 1);
        }
    }

    // init
    const boids = [];
    for (let i = 0; i < 256; ++i) {
        boids.push(new Boid());
    }

    const wrapBound = (v) => {
        if (v[0] > 1.1) { v[0] -= 2.1; }
        else if (v[0] < -1.1) { v[0] += 2.1; }
        if (v[1] > 1.1) { v[1] -= 2.1; }
        else if (v[1] < -1.1) { v[1] += 2.1; }
        if (v[2] > 1.1) { v[2] -= 1; }
        else if (v[2] < -0.1) { v[2] += 1; }
    };
    const clampLength = (v, max) => {
        const l = vec3.length(v);
        if (l > max) { vec3.scale(v, v, max / l); }
    };
    const applyForce = (acc, vel, v, f, max) => {
        vec3.scale(tempVec3a, v, max / vec3.length(v));
        clampLength(vec3.subtract(tempVec3a, tempVec3a, vel), f);
        vec3.add(acc, acc, tempVec3a);
    };

    let lastTime = -1;

    function tick(gTimeInMS) {
        dt = (gTimeInMS - lastTime) / 1000;
        lastTime = gTimeInMS;
        let distance = 0;

        for (const b1 of boids) {
            vec3.set(alignment, 0, 0, 0);
            vec3.set(cohesion, 0, 0, 0);
            vec3.set(separation, 0, 0, 0);
            vec3.set(b1.acceleration, 0, 0, 0);
            let alignmentActive = false;
            let cohesionActive = false;
            let separationActive = false;

            for (const b2 of boids) {
                if (b1 === b2) { continue; }
                vec3.subtract(tempVec3a, b2.transform.getPosition(), b1.transform.getPosition());
                distance = Math.max(0.01, vec3.length(tempVec3a) - 0.1);

                if (distance < separationDistance) {
                    vec3.scale(tempVec3b, tempVec3a, -1 / distance);
                    vec3.add(separation, separation, tempVec3b);
                    separationActive = true;
                }

                if (distance < flockmateRadius) {
                    vec3.add(cohesion, cohesion, tempVec3a);
                    cohesionActive = true;
                    vec3.add(alignment, alignment, b2.velocity);
                    alignmentActive = true;
                }
            }

            if (alignmentActive) { applyForce(b1.acceleration, b1.velocity, alignment, alignmentForce, maxVelocity); }
            if (cohesionActive) { applyForce(b1.acceleration, b1.velocity, cohesion, cohesionForce, maxVelocity); }
            if (separationActive) { applyForce(b1.acceleration, b1.velocity, separation, separationForce, maxVelocity); }
        }

        for (const b of boids) {
            clampLength(vec3.add(b.velocity, b.velocity, b.acceleration), maxVelocity);
            wrapBound(vec3.scaleAndAdd(tempVec3a, b.transform.getPosition(), b.velocity, dt));
            b.transform.setPosition(tempVec3a[0], tempVec3a[1], tempVec3a[2]);
            b.update();
        }

        root.render();
    }

    if (JSB) { window.gameTick = tick; }
    else { tick(); }

})();
