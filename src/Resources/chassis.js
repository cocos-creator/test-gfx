const chassis = (() => {
    const JSB = typeof jsb !== 'undefined';

    const { mat4, vec3, vec4, quat } = JSB ? glMatrix : require('./gl-matrix');

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

        setParent(value) {
            if (this._parent === value) { return; }

            if (this._parent) {
                const idx = this._parent._children.indexOf(this);
                if (idx >= 0) { this._parent._children.splice(idx, 1); }
            }
            this._parent = value;

            const idx = value._children.indexOf(this);
            if (idx < 0) { value._children.push(this); }

            this._invalidateChildren(TransformFlagBit.TRS);
        }

        setPosition(x, y, z) {
            if (y !== undefined) {
                vec3.set(this._lpos, x, y, z);
            } else {
                vec3.copy(this._lpos, x);
            }
            this._invalidateChildren(TransformFlagBit.POSITION);
        }

        setRotation(x, y, z, w) {
            if (y !== undefined) {
                quat.set(this._lrot, x, y, z, w);
            } else {
                quat.copy(this._lrot, x);
            }
            this._invalidateChildren(TransformFlagBit.ROTATION);
        }

        setRotationFromEuler(x, y, z) {
            if (y !== undefined) {
                quat.fromEuler(this._lrot, x, y, z, 'yzx');
            } else {
                quat.fromEuler(this._lrot, x[0], x[1], x[2], 'yzx');
            }
            this._invalidateChildren(TransformFlagBit.ROTATION);
        }

        setScale(x, y, z) {
            if (y !== undefined) {
                vec3.set(this._lscale, x, y, z);
            } else {
                vec3.copy(this._lscale, x);
            }
            this._invalidateChildren(TransformFlagBit.SCALE);
        }

        getPosition() {
            return this._lpos;
        }

        getRotation() {
            return this._lrot;
        }

        getScale() {
            return this._lscale;
        }

        getWorldPosition() {
            this._updateWorldTransform();
            return this._pos;
        }

        getWorldRotation() {
            this._updateWorldTransform();
            return this._rot;
        }

        getWorldScale() {
            this._updateWorldTransform();
            return this._scale;
        }

        getWorldMatrix() {
            this._updateWorldTransform();
            return this._mat;
        }

        _invalidateChildren(dirtyFlags) {
            if ((this._dirtyFlags & dirtyFlags) === dirtyFlags) { return; }
            this._dirtyFlags |= dirtyFlags;
            const newDirtyBit = dirtyFlags | TransformFlagBit.POSITION;
            const len = this._children.length;
            for (let i = 0; i < len; ++i) {
                this._children[i]._invalidateChildren(newDirtyBit);
            }
        }

        _updateWorldTransform() {
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

        setTransform(transform) {
            this._transform = transform;
        }

        setColor(r, g, b, a) {
            if (g !== undefined) {
                vec4.set(this._color, r, g, b, a);
            } else {
                vec4.copy(this._color, r);
            }
        }

        getTransform() {
            return this._transform;
        }

        getColor() {
            return this._color;
        }
    }

    class Root {
        _models = [];

        // web implementation omitted
        initialize() {}
        destroy() {}
        render() {}

        createTransform() {
            return new Transform();
        }

        createModel() {
            const res = new Model();
            this._models.push(res);
            return res;
        }

        destroyModel(model) {
            const idx = this._models.indexOf(model);
            if (idx >= 0) { this._models.splice(idx, 1); }
        }
    }

    // ====================== Agent ======================

    class TransformAgent extends Transform {
        _actor = null;

        constructor(actor) {
            super();
            this._actor = actor;
        }

        setParent(value) {
            super.setParent(value);
            this._actor.setParent(value._actor);
        }

        setPosition(x, y, z) {
            if (y !== undefined) {
                super.setPosition(x, y, z);
                this._actor.setPosition(x, y, z);
            } else {
                super.setPosition(x);
                this._actor.setPosition(x);
            }
        }

        setRotation(x, y, z, w) {
            if (y !== undefined) {
                super.setRotation(x, y, z, w);
                this._actor.setRotation(x, y, z, w);
            } else {
                super.setRotation(x);
                this._actor.setRotation(x);
            }
        }

        setRotationFromEuler(x, y, z) {
            if (y !== undefined) {
                super.setRotationFromEuler(x, y, z);
                this._actor.setRotationFromEuler(x, y, z);
            } else {
                super.setRotationFromEuler(x);
                this._actor.setRotationFromEuler(x);
            }
        }

        setScale(x, y, z) {
            if (y !== undefined) {
                super.setScale(x, y, z);
                this._actor.setScale(x, y, z);
            } else {
                super.setScale(x);
                this._actor.setScale(x);
            }
        }
    }

    class ModelAgent extends Model {
        _actor = null;

        constructor(actor) {
            super();
            this._actor = actor;
        }

        setTransform(transform) {
            super.setTransform(transform);
            this._actor.setTransform(transform._actor);
        }

        setColor(r, g, b, a) {
            if (g !== undefined) {
                super.setColor(r, g, b, a);
                this._actor.setColor(r, g, b, a);
            } else {
                super.setColor(r);
                this._actor.setColor(r);
            }
        }
    }

    class RootAgent extends Root {
        _actor = null;

        constructor(actor) {
            super();
            this._actor = actor;
        }

        initialize() {
            // don't invoke super method here
            this._actor.initialize();
        }

        destroy() {
            // don't invoke super method here
            this._actor.destroy();
        }

        render() {
            // don't invoke super method here
            this._actor.render();
        }

        createTransform() {
            const actor = this._actor.createTransform();
            return new TransformAgent(actor);
        }

        createModel() {
            const res = new ModelAgent(this._actor.createModel());
            this._models.push(res);
            return res;
        }

        destroyModel(model) {
            super.destroyModel(model);
            // actor is destroyed through GC
        }
    }

    const chassis = { root: JSB ? new RootAgent(jsb.RootManager.create()) : new Root() };

    if (JSB) { return chassis; }
    else { module.exports = chassis; return null; }

})();
