#include "Chassis.h"
#include "tests/ScriptTest/Math.h"

namespace cc {

void Transform::setParent(Transform *value) {
    if (_parent == value) return;

    if (_parent) {
        _parent->_children.erase(
            std::remove(_parent->_children.begin(), _parent->_children.end(), this),
            _parent->_children.end());
    }
    _parent = value;

    if (std::find(value->_children.begin(), value->_children.end(), this) == value->_children.end()) {
        value->_children.push_back(this);
    }

    invalidateChildren(TransformFlagBit::TRS);
}

void Transform::setPosition(float x, float y, float z) {
    _lpos.x() = x;
    _lpos.y() = y;
    _lpos.z() = z;

    invalidateChildren(TransformFlagBit::POSITION);
}

void Transform::setRotation(const float *q) {
    _lrot = vmath::load<vmath::QuatF>(q);

    invalidateChildren(TransformFlagBit::ROTATION);
}

void Transform::setRotationFromEuler(float angleX, float angleY, float angleZ) {
    _lrot = vmath::quatFromEuler(vmath::Vec3F{angleX, angleY, angleZ});

    invalidateChildren(TransformFlagBit::ROTATION);
}

void Transform::setScale(float x, float y, float z) {
    _lscale.x() = x;
    _lscale.y() = y;
    _lscale.z() = z;

    invalidateChildren(TransformFlagBit::SCALE);
}

void Transform::invalidateChildren(TransformFlagBit dirtyFlags) {
    if ((_dirtyFlags & dirtyFlags) == dirtyFlags) {
        return;
    }

    _dirtyFlags |= dirtyFlags;
    TransformFlagBit newDirtyBit = dirtyFlags | TransformFlagBit::POSITION;

    for (auto &child : _children) {
        child->invalidateChildren(newDirtyBit);
    }
}

namespace {
vector<const Transform *> tempArray;
} // namespace

void Transform::updateWorldTransform() const {
    if (_dirtyFlags == TransformFlagBit::NONE) {
        return;
    }

    const Transform *cur = this;
    tempArray.clear();
    while (cur && cur->_dirtyFlags != TransformFlagBit::NONE) {
        tempArray.push_back(cur);
        cur = cur->_parent;
    }
    const Transform *child     = this;
    TransformFlagBit dirtyBits = TransformFlagBit::NONE;

    size_t i = tempArray.size();
    while (i) {
        child = tempArray[--i];
        dirtyBits |= child->_dirtyFlags;
        if (cur) {
            if (hasFlag(dirtyBits, TransformFlagBit::POSITION)) {
                child->_pos       = vmath::vec3TransformMat4(child->_lpos, cur->_mat);
                child->_mat[3][0] = child->_pos[0];
                child->_mat[3][1] = child->_pos[1];
                child->_mat[3][2] = child->_pos[2];
            }
            if (hasAnyFlags(dirtyBits, TransformFlagBit::RS)) {
                child->_mat = vmath::mat4FromRTS(child->_lrot, child->_lpos, child->_lscale);
                child->_mat = cur->_mat * child->_mat;
                if (hasFlag(dirtyBits, TransformFlagBit::ROTATION)) {
                    child->_rot = cur->_rot * child->_lrot;
                }
                auto tempMat4    = vmath::mat4FromQuat(vmath::conjugate(child->_rot)) * child->_mat;
                child->_scale[0] = tempMat4[0][0];
                child->_scale[1] = tempMat4[1][1];
                child->_scale[2] = tempMat4[2][2];
            }
        } else {
            if (hasFlag(dirtyBits, TransformFlagBit::POSITION)) {
                child->_pos       = child->_lpos;
                child->_mat[3][0] = child->_pos[0];
                child->_mat[3][1] = child->_pos[1];
                child->_mat[3][2] = child->_pos[2];
            }
            if (hasAnyFlags(dirtyBits, TransformFlagBit::RS)) {
                if (hasFlag(dirtyBits, TransformFlagBit::ROTATION)) {
                    child->_rot = child->_lrot;
                }
                if (hasFlag(dirtyBits, TransformFlagBit::SCALE)) {
                    child->_scale = child->_lscale;
                }
                child->_mat = vmath::mat4FromRTS(child->_rot, child->_pos, child->_scale);
            }
        }

        child->_dirtyFlags = TransformFlagBit::NONE;
        cur                = child;
    }
}

ModelX          Model::buffer;
vector<Model *> Model::views;

Model::~Model() {
    if (views[_idx] != this) return;

    Model *last = views.back();
    views.pop_back();
    vmath::slice(buffer, last->_idx) = ModelF{};

    if (this != last) {
        vmath::slice(buffer, _idx) = vmath::slice(buffer, last->_idx);
        views[_idx]                = last;
        last->_idx                 = _idx;
    }
}

void Model::setColor(float r, float g, float b, float a) {
    auto &&model    = vmath::slice(buffer, _idx);
    model.color.x() = r;
    model.color.y() = g;
    model.color.z() = b;
    model.color.w() = a;
}

void Model::setTransform(const Transform *transform) {
    auto &&model    = vmath::slice(buffer, _idx);
    model.transform = transform;
}

void Model::setEnabled(bool enabled) {
    auto &&model  = vmath::slice(buffer, _idx);
    model.enabled = enabled;
}

Root *Root::instance = nullptr;

Root::Root() {
    if (!instance) {
        instance = this;
    }
    vmath::setSlices(Model::buffer, vmath::FloatP::Size);
}

Root::~Root() {
    CCASSERT(Model::views.empty(), "Resources leaked");

    if (instance == this) {
        instance = nullptr;
    }
}

Transform *Root::createTransform() {
    return CC_NEW(Transform);
}

Model *Root::createModel() {
    if (Model::views.size() >= vmath::slices(Model::buffer)) {
        vmath::setSlices(Model::buffer, Model::views.size() * 2);
    }
    auto *res = CC_NEW(Model(Model::views.size()));
    Model::views.push_back(res);
    return res;
}

///////////////////// Agent /////////////////////

TransformAgent::~TransformAgent() {
    ENQUEUE_MESSAGE_1(
        RootAgent::getInstance()->getMessageQueue(), TransformDestruct,
        actor, getActor(),
        {
            CC_SAFE_DELETE(actor);
        });
}

void TransformAgent::setParent(Transform *value) {
    ENQUEUE_MESSAGE_2(
        RootAgent::getInstance()->getMessageQueue(), TransformSetParent,
        actor, getActor(),
        value, static_cast<TransformAgent *>(value)->getActor(),
        {
            actor->setParent(value);
        });
}

void TransformAgent::setPosition(float x, float y, float z) {
    ENQUEUE_MESSAGE_4(
        RootAgent::getInstance()->getMessageQueue(), TransformSetPosition,
        actor, getActor(),
        x, x,
        y, y,
        z, z,
        {
            actor->setPosition(x, y, z);
        });
}

void TransformAgent::setRotation(const float *q) {
    auto *actorQuat = RootAgent::getInstance()->getMainAllocator()->allocate<float>(4);
    memcpy(actorQuat, q, sizeof(float) * 4);

    ENQUEUE_MESSAGE_2(
        RootAgent::getInstance()->getMessageQueue(), TransformSetRotation,
        actor, getActor(),
        q, actorQuat,
        {
            actor->setRotation(q);
        });
}

void TransformAgent::setRotationFromEuler(float angleX, float angleY, float angleZ) {
    ENQUEUE_MESSAGE_4(
        RootAgent::getInstance()->getMessageQueue(), TransformSetRotationFromEuler,
        actor, getActor(),
        angleX, angleX,
        angleY, angleY,
        angleZ, angleZ,
        {
            actor->setRotationFromEuler(angleX, angleY, angleZ);
        });
}

void TransformAgent::setScale(float x, float y, float z) {
    ENQUEUE_MESSAGE_4(
        RootAgent::getInstance()->getMessageQueue(), TransformSetScale,
        actor, getActor(),
        x, x,
        y, y,
        z, z,
        {
            actor->setScale(x, y, z);
        });
}

ModelAgent::~ModelAgent() {
    ENQUEUE_MESSAGE_1(
        RootAgent::getInstance()->getMessageQueue(), ModelDestruct,
        actor, getActor(),
        {
            CC_SAFE_DELETE(actor);
        });
}

void ModelAgent::setColor(float r, float g, float b, float a) {
    ENQUEUE_MESSAGE_5(
        RootAgent::getInstance()->getMessageQueue(), ModelSetColor,
        actor, getActor(),
        r, r, g, g, b, b, a, a,
        {
            actor->setColor(r, g, b, a);
        });
}

void ModelAgent::setTransform(const Transform *transform) {
    ENQUEUE_MESSAGE_2(
        RootAgent::getInstance()->getMessageQueue(), ModelSetTransform,
        actor, getActor(),
        transform, static_cast<const TransformAgent *>(transform)->getActor(),
        {
            actor->setTransform(transform);
        });
}

void ModelAgent::setEnabled(bool enabled) {
    ENQUEUE_MESSAGE_2(
        RootAgent::getInstance()->getMessageQueue(), ModelSetEnabled,
        actor, getActor(),
        enabled, enabled,
        {
            actor->setEnabled(enabled);
        });
}

RootAgent *RootAgent::instance = nullptr;

RootAgent::RootAgent(Root *root) : Agent(root) {
    instance = this;
}

RootAgent::~RootAgent() {
    CC_SAFE_DELETE(_actor);
    instance = nullptr;
}

void RootAgent::initialize() {
    _mainMessageQueue = CC_NEW(MessageQueue);

    _allocatorPools.resize(MAX_CPU_FRAME_AHEAD + 1);
    for (uint i = 0U; i < MAX_CPU_FRAME_AHEAD + 1; ++i) {
        _allocatorPools[i] = CC_NEW(LinearAllocatorPool);
    }

    _actor->initialize();

    setMultithreaded(true);
}

void RootAgent::destroy() {
    ENQUEUE_MESSAGE_1(
        getMessageQueue(), RootDestroy,
        actor, getActor(),
        {
            actor->destroy();
        });

    _mainMessageQueue->terminateConsumerThread();
    CC_SAFE_DELETE(_mainMessageQueue);

    for (LinearAllocatorPool *pool : _allocatorPools) {
        CC_SAFE_DELETE(pool);
    }
    _allocatorPools.clear();
}

void RootAgent::render() {
    ENQUEUE_MESSAGE_2(
        _mainMessageQueue, RootRender,
        actor, getActor(),
        frameBoundarySemaphore, &_frameBoundarySemaphore,
        {
            actor->render();
            frameBoundarySemaphore->signal();
        });

    MessageQueue::freeChunksInFreeQueue(_mainMessageQueue);
    _mainMessageQueue->finishWriting();
    _currentIndex = (_currentIndex + 1) % (MAX_CPU_FRAME_AHEAD + 1);
    _frameBoundarySemaphore.wait();

    getMainAllocator()->reset();
}

void RootAgent::setMultithreaded(bool multithreaded) {
    if (multithreaded == _multithreaded) return;
    _multithreaded = multithreaded;

    if (multithreaded) {
        _mainMessageQueue->setImmediateMode(false);
        _mainMessageQueue->runConsumerThread();
        CC_LOG_INFO("Render thread detached.");
    } else {
        _mainMessageQueue->terminateConsumerThread();
        _mainMessageQueue->setImmediateMode(true);
        CC_LOG_INFO("Render thread joined.");
    }
}

Transform *RootAgent::createTransform() {
    auto *actor = _actor->createTransform();
    return CC_NEW(TransformAgent(actor));
}

Model *RootAgent::createModel() {
    auto *actor = _actor->createModel();
    return CC_NEW(ModelAgent(actor));
}

Root *RootManager::instance{nullptr};

} // namespace cc
