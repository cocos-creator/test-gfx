const JSB = typeof jsb !== 'undefined';

const { mat3, mat4, vec3, vec4, quat } = JSB ? glMatrix : require('./gl-matrix');

const tempVector = [];
const tempQuat = quat.create();
const tempMat4 = mat4.create();

const TransformFlagBit = {
    NONE: 0,
    POSITION: (1 << 0),
    ROTATION: (1 << 1),
    SCALE: (1 << 2),
};
TransformFlagBit.RS = TransformFlagBit.ROTATION | TransformFlagBit.SCALE;
TransformFlagBit.TRS = TransformFlagBit.POSITION | TransformFlagBit.ROTATION | TransformFlagBit.SCALE;
TransformFlagBit.TRS_MASK = ~TransformFlagBit.TRS;

class Transform {
    _lpos = vec3.create();
    _lrot = quat.create();
    _lscale = vec3.set(vec3.create(), 1, 1, 1);

    _dirtyFlags = TransformFlagBit.NONE;
    _pos = vec3.create();
    _rot = quat.create();
    _scale = vec3.set(vec3.create(), 1, 1, 1);
    _mat = mat4.create();

    _parent = null;
    _children = [];

    setParent (value) {
        if (this._parent === value) return;

        if (this._parent) {
            const idx = this._parent._children.indexOf(this);
            if (idx >= 0) this._parent._children.splice(idx, 1);
        }
        this._parent = value;

        const idx = value._children.indexOf(this);
        if (idx < 0) value._children.push(this);

        this._invalidateChildren(TransformFlagBit.TRS);
    }

    setPosition (x, y, z) {
        vec3.set(this._lpos, x, y, z);
        this._invalidateChildren(TransformFlagBit.POSITION);
    }

    setRotation (q) {
        quat.copy(this._lrot, q);
        this._invalidateChildren(TransformFlagBit.ROTATION);
    }

    setRotationFromEuler (angleX, angleY, angleZ) {
        quat.fromEuler(this._lrot, angleX, angleY, angleZ, 'yzx');
        this._invalidateChildren(TransformFlagBit.ROTATION);
    }

    setScale (x, y, z) {
        vec3.set(this._lscale, x, y, z);
        this._invalidateChildren(TransformFlagBit.SCALE);
    }

    getPosition () {
        return this._lpos;
    }

    getRotation () {
        return this._lrot;
    }

    getScale () {
        return this._lscale;
    }

    getWorldPosition () {
        this._updateWorldTransform();
        return this._pos;
    }

    getWorldRotation () {
        this._updateWorldTransform();
        return this._rot;
    }

    getWorldScale () {
        this._updateWorldTransform();
        return this._scale;
    }

    getWorldMatrix () {
        this._updateWorldTransform();
        return this._mat;
    }

    _invalidateChildren (dirtyFlags) {
        if ((this._dirtyFlags & dirtyFlags) === dirtyFlags) { return; }
        this._dirtyFlags |= dirtyFlags;
        const newDirtyBit = dirtyFlags | TransformFlagBit.POSITION;
        const len = this._children.length;
        for (let i = 0; i < len; ++i) {
            this._children[i]._invalidateChildren(newDirtyBit);
        }
    }

    _updateWorldTransform () {
        if (!this._dirtyFlags) { return; }
        let cur = this;
        let i = 0;
        while (cur && cur._dirtyFlags) {
            tempVector[i++] = cur;
            cur = cur._parent;
        }
        let child = this;
        let dirtyBits = 0;

        while (i) {
            child = tempVector[--i];
            dirtyBits |= child._dirtyFlags;
            if (cur) {
                if (dirtyBits & TransformFlagBit.POSITION) {
                    vec3.transformMat4(child._pos, child._lpos, cur._mat);
                    child._mat[12] = child._pos[0];
                    child._mat[13] = child._pos[1];
                    child._mat[14] = child._pos[2];
                }
                if (dirtyBits & TransformFlagBit.RS) {
                    mat4.fromRotationTranslationScale(child._mat, child._lrot, child._lpos, child._lscale);
                    mat4.multiply(child._mat, cur._mat, child._mat);
                    if (dirtyBits & TransformFlagBit.ROTATION) {
                        quat.multiply(child._rot, cur._rot, child._lrot);
                    }
                    mat4.fromQuat(tempMat4, quat.conjugate(tempQuat, child._rot));
                    mat4.multiply(tempMat4, tempMat4, child._mat);
                    child._scale[0] = tempMat4[0];
                    child._scale[1] = tempMat4[5];
                    child._scale[2] = tempMat4[10];
                }
            } else {
                if (dirtyBits & TransformFlagBit.POSITION) {
                    vec3.copy(child._pos, child._lpos);
                    child._mat[12] = child._pos[0];
                    child._mat[13] = child._pos[1];
                    child._mat[14] = child._pos[2];
                }
                if (dirtyBits & TransformFlagBit.RS) {
                    if (dirtyBits & TransformFlagBit.ROTATION) {
                        quat.copy(child._rot, child._lrot);
                    }
                    if (dirtyBits & TransformFlagBit.SCALE) {
                        vec3.copy(child._scale, child._lscale);
                    }
                    mat4.fromRotationTranslationScale(child._mat, child._rot, child._pos, child._scale);
                }
            }

            child._dirtyFlags = TransformFlagBit.NONE;
            cur = child;
        }
    }
}

class Model {
    _transform = null;
    _color = vec4.set(vec4.create(), 1, 1, 1, 1);

    setTransform (transform) {
        this._transform = transform;
    }

    setColor (r, g, b, a) {
        vec4.set(this._color, r, g, b, a);
    }

    getTransform () {
        return this._transform;
    }

    getColor () {
        return this._color;
    }
}

class Root {
    _models = [];

    // web implementation omitted
    initialize () {}
    destroy () {}
    render () {}

    createTransform () {
        return new Transform();
    }

    createModel () {
        const res = new Model();
        this._models.push(res);
        return res;
    }

    destroyModel (model) {
        const idx = this._models.indexOf(model);
        if (idx >= 0) this._models.splice(idx, 1);
    }
}

///////////////////// Agent /////////////////////

class TransformAgent extends Transform {
    _actor = null;

    constructor (actor) {
        super();
        this._actor = actor;
    }

    setParent (value) {
        super.setParent(value);
        this._actor.setParent(value._actor);
    }

    setPosition (x, y, z) {
        super.setPosition(x, y, z);
        this._actor.setPosition(x, y, z);
    }

    setRotation (q) {
        super.setRotation(q);
        this._actor.setRotation(q);
    }

    setRotationFromEuler (angleX, angleY, angleZ) {
        super.setRotationFromEuler(angleX, angleY, angleZ);
        this._actor.setRotationFromEuler(angleX, angleY, angleZ);
    }

    setScale (x, y, z) {
        super.setScale(x, y, z);
        this._actor.setScale(x, y, z);
    }
}

class ModelAgent extends Model {
    _actor = null;

    constructor (actor) {
        super();
        this._actor = actor;
    }

    setTransform (transform) {
        super.setTransform(transform);
        this._actor.setTransform(transform._actor);
    }

    setColor (r, g, b, a) {
        super.setColor(r, g, b, a);
        this._actor.setColor(r, g, b, a);
    }
}

class RootAgent extends Root {
    _actor = null;

    constructor (actor) {
        super();
        this._actor = actor;
    }

    initialize () {
        // don't invoke super method here
        this._actor.initialize();
    }

    destroy () {
        // don't invoke super method here
        this._actor.destroy();
    }

    render () {
        // don't invoke super method here
        this._actor.render();
    }

    createTransform () {
        const actor = this._actor.createTransform();
        return new TransformAgent(actor);
    }

    createModel () {
        const res = new ModelAgent(this._actor.createModel());
        this._models.push(res);
        return res;
    }

    destroyModel (model) {
        super.destroyModel(model);
        this._actor.destroyModel(model);
    }
}

///////////////////// Client /////////////////////

const root = JSB ? new RootAgent(jsb.RootManager.create()) : new Root();
const parent = root.createTransform();
const v3_1 = vec3.create();
const v3_2 = vec3.create();

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
        quat.normalize(tempQuat, quat.set(tempQuat, v3_2[0], v3_2[1], v3_2[2], 1 + dot));
        this.transform.setRotation(tempQuat);

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
