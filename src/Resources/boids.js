(() => {
    const JSB = typeof jsb !== 'undefined';

    const { vec3, quat } = JSB ? glMatrix : require('./gl-matrix');
    const { root } = JSB ? chassis : require('./chassis');

    const parent = root.createTransform();
    const v3_1 = vec3.create();
    const v3_2 = vec3.create();
    const qt_1 = quat.create();

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

        constructor () {
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

        update () {
            vec3.normalize(v3_1, this.velocity);
            const dot = vec3.dot(UP, v3_1);
            vec3.cross(v3_2, UP, v3_1);
            quat.normalize(qt_1, quat.set(qt_1, v3_2[0], v3_2[1], v3_2[2], 1 + dot));
            this.transform.setRotation(qt_1);

            // vec3.normalize(v3_1, this.acceleration);
            vec3.scaleAndAdd(v3_1, HALF, v3_1, 0.5);
            this.model.setColor(v3_1[0], v3_1[1], v3_1[2], 1);
        }
    }

    // init
    const boids = [];
    for (let i = 0; i < 256; ++i) {
        boids.push(new Boid());
    }

    const wrapBound = (v) => {
        if (v[0] > 1.1) v[0] -= 2.1;
        else if (v[0] < -1.1) v[0] += 2.1;
        if (v[1] > 1.1) v[1] -= 2.1;
        else if (v[1] < -1.1) v[1] += 2.1;
        if (v[2] > 1.1) v[2] -= 1;
        else if (v[2] < -0.1) v[2] += 1;
    };
    const clampLength = (v, max) => {
        const l = vec3.length(v);
        if (l > max) vec3.scale(v, v, max / l);
    };
    const applyForce = (acc, vel, v, f, max) => {
        vec3.scale(v3_1, v, max / vec3.length(v));
        clampLength(vec3.subtract(v3_1, v3_1, vel), f);
        vec3.add(acc, acc, v3_1);
    };

    let lastTime = -1;

    function tick (gTimeInMS) {
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
                if (b1 === b2) continue;
                vec3.subtract(v3_1, b2.transform.getPosition(), b1.transform.getPosition());
                distance = Math.max(0.01, vec3.length(v3_1) - 0.1);

                if (distance < separationDistance) {
                    vec3.scale(v3_2, v3_1, -1 / distance);
                    vec3.add(separation, separation, v3_2);
                    separationActive = true;
                }

                if (distance < flockmateRadius) {
                    vec3.add(cohesion, cohesion, v3_1);
                    cohesionActive = true;
                    vec3.add(alignment, alignment, b2.velocity);
                    alignmentActive = true;
                }
            }

            if (alignmentActive) applyForce(b1.acceleration, b1.velocity, alignment, alignmentForce, maxVelocity);
            if (cohesionActive) applyForce(b1.acceleration, b1.velocity, cohesion, cohesionForce, maxVelocity);
            if (separationActive) applyForce(b1.acceleration, b1.velocity, separation, separationForce, maxVelocity);
        }

        for (const b of boids) {
            clampLength(vec3.add(b.velocity, b.velocity, b.acceleration), maxVelocity);
            wrapBound(vec3.scaleAndAdd(v3_1, b.transform.getPosition(), b.velocity, dt));
            b.transform.setPosition(v3_1[0], v3_1[1], v3_1[2]);
            b.update();
        }

        root.render();
    }

    if (JSB) window.gameTick = tick;
    else tick();
})();
