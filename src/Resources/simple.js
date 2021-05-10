// node gl-matrix-min.js simple.js
const { mat3, mat4, vec3, quat } = glMatrix;

const array_a = [];
const qt_1 = quat.create();
const m3_1 = mat3.create();
const m3_2 = mat3.create();

const TransformBit = {
    NONE: 0,
    POSITION: (1 << 0),
    ROTATION: (1 << 1),
    SCALE: (1 << 2),
};
TransformBit.RS = TransformBit.ROTATION | TransformBit.SCALE;
TransformBit.TRS = TransformBit.POSITION | TransformBit.ROTATION | TransformBit.SCALE;
TransformBit.TRS_MASK = ~TransformBit.TRS;

class Transform {
    _lpos = vec3.create();
    _lrot = quat.create();
    _lscale = vec3.set(vec3.create(), 1, 1, 1);

    _dirtyFlags = TransformBit.NONE;
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

        this._invalidateChildren(TransformBit.TRS);
    }

    setPosition (x, y, z) {
        vec3.set(this._lpos, x, y, z);
        this._invalidateChildren(TransformBit.POSITION);
    }

    setRotation (angleX, angleY, angleZ) {
        quat.fromEuler(this._lrot, angleX, angleY, angleZ);
        this._invalidateChildren(TransformBit.ROTATION);
    }

    setScale (x, y, z) {
        vec3.set(this._lscale, x, y, z);
        this._invalidateChildren(TransformBit.SCALE);
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
        const newDirtyBit = dirtyFlags | TransformBit.POSITION;
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
            array_a[i++] = cur;
            cur = cur._parent;
        }
        let child = this;
        let dirtyBits = 0;

        while (i) {
            child = array_a[--i];
            dirtyBits |= child._dirtyFlags;
            if (cur) {
                if (dirtyBits & TransformBit.POSITION) {
                    vec3.transformMat4(child._pos, child._lpos, cur._mat);
                    child._mat[12] = child._pos[0];
                    child._mat[13] = child._pos[1];
                    child._mat[14] = child._pos[2];
                }
                if (dirtyBits & TransformBit.RS) {
                    mat4.fromRotationTranslationScale(child._mat, child._lrot, child._lpos, child._lscale);
                    mat4.multiply(child._mat, cur._mat, child._mat);
                    if (dirtyBits & TransformBit.ROTATION) {
                        quat.multiply(child._rot, cur._rot, child._lrot);
                    }
                    mat3.fromQuat(m3_1, quat.conjugate(qt_1, child._rot));
                    mat3.multiply(m3_1, m3_1, mat3.fromMat4(m3_2, child._mat));
                    child._scale[0] = m3_1[0];
                    child._scale[1] = m3_1[4];
                    child._scale[2] = m3_1[8];
                }
            } else {
                if (dirtyBits & TransformBit.POSITION) {
                    vec3.copy(child._pos, child._lpos);
                    child._mat[12] = child._pos[0];
                    child._mat[13] = child._pos[1];
                    child._mat[14] = child._pos[2];
                }
                if (dirtyBits & TransformBit.RS) {
                    if (dirtyBits & TransformBit.ROTATION) {
                        quat.copy(child._rot, child._lrot);
                    }
                    if (dirtyBits & TransformBit.SCALE) {
                        vec3.copy(child._scale, child._lscale);
                        mat4.fromRotationTranslationScale(child._mat, child._rot, child._pos, child._scale);
                    }
                }
            }

            child._dirtyFlags = TransformBit.NONE;
            cur = child;
        }
    }
}

class TransformAgent extends Transform {

    _actor = new jsb.Transform();

    setParent (value) {
        super.setParent(value);
        this._actor.setParent(value);
    }

    setPosition (x, y, z) {
        super.setPosition(x, y, z);
        this._actor.setPosition(x, y, z);
    }

    setRotation (angleX, angleY, angleZ) {
        super.setRotation(this._lrot, angleX, angleY, angleZ);
        this._actor.setRotation(this._lrot, angleX, angleY, angleZ);
    }

    setScale (x, y, z) {
        super.setScale(x, y, z);
        this._actor.setScale(x, y, z);
    }
}

function createTransform () {
    return new TransformAgent(); // native environment
    // return new Transform(); // browser environment
}

const t = createTransform();
const t2 = createTransform();
t2.setParent(t);
t.setPosition(1, 2, 3);
t.setRotation(10, 20, 30);
t.setScale(30, 20, 10);

function tick (gTimeInMS) {
}

if (typeof window !== 'undefined') window.gameTick = tick;
