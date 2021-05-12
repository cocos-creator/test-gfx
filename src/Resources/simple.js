// node gl-matrix-min.js simple.js
const { mat3, mat4, vec3, quat } = glMatrix;

const tempVector = [];
const tempQuat = quat.create();
const tempMat3a = mat3.create();
const tempMat3b = mat3.create();

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

    setRotation (angleX, angleY, angleZ) {
        quat.fromEuler(this._lrot, angleX, angleY, angleZ);
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
                    mat3.fromQuat(tempMat3a, quat.conjugate(tempQuat, child._rot));
                    mat3.multiply(tempMat3a, tempMat3a, mat3.fromMat4(tempMat3b, child._mat));
                    child._scale[0] = tempMat3a[0];
                    child._scale[1] = tempMat3a[4];
                    child._scale[2] = tempMat3a[8];
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
                        mat4.fromRotationTranslationScale(child._mat, child._rot, child._pos, child._scale);
                    }
                }
            }

            child._dirtyFlags = TransformFlagBit.NONE;
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
