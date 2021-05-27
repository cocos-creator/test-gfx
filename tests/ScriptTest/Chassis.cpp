#include "Chassis.h"
#include "tests/ScriptTest/Math.h"

namespace cc {

TransformX      Transform::buffer;
Transform::PtrX Transform::views;
vmath::Index    Transform::viewCount{0};

Transform::~Transform() {
    if (vmath::slice(views, _idx) != this) return;

    Transform *last                  = vmath::slice(views, viewCount--);
    vmath::slice(buffer, last->_idx) = TransformF{};

    if (this != last) {
        vmath::slice(buffer, _idx) = vmath::slice(buffer, last->_idx);
        vmath::slice(views, _idx)  = last;
        last->_idx                 = _idx;
    }
}

void Transform::setParent(Transform *value) {
    auto &&transform = vmath::slice(buffer, _idx);

    if ((!value && transform.parent < 0) || (transform.parent == value->_idx)) return;

    if (transform.parent >= 0) {
        auto &&     parent = vmath::slice(buffer, transform.parent);
        const auto *end    = std::remove(parent.children.begin(), parent.children.end(), _idx);
        CCASSERT(end == parent.children.end() - 1, "Mismatching parent/children?");
        --parent.childrenCount;
    }

    transform.parent = value ? value->_idx : -1;
    invalidateChildren(TransformFlagBit::TRS);

    if (!value) return;

    auto &&newParent = vmath::slice(buffer, value->_idx);
    CCASSERT(std::find(newParent.children.begin(), newParent.children.end(), _idx) == newParent.children.end(), "Mismatching parent/children?");
    newParent.children[newParent.childrenCount++] = _idx;
}

void Transform::setPosition(float x, float y, float z) {
    auto &&transform = vmath::slice(buffer, _idx);

    transform.lpos.x() = x;
    transform.lpos.y() = y;
    transform.lpos.z() = z;

    invalidateChildren(TransformFlagBit::POSITION);
}

void Transform::setRotation(const float *q) {
    auto &&transform = vmath::slice(buffer, _idx);

    transform.lrot = vmath::load<vmath::QuatF>(q);

    invalidateChildren(TransformFlagBit::ROTATION);
}

void Transform::setRotationFromEuler(float angleX, float angleY, float angleZ) {
    auto &&transform = vmath::slice(buffer, _idx);

    transform.lrot = vmath::quatFromEuler(vmath::Vec3F{angleX, angleY, angleZ});

    invalidateChildren(TransformFlagBit::ROTATION);
}

void Transform::setScale(float x, float y, float z) {
    auto &&transform = vmath::slice(buffer, _idx);

    transform.lscale.x() = x;
    transform.lscale.y() = y;
    transform.lscale.z() = z;

    invalidateChildren(TransformFlagBit::SCALE);
}

void Transform::invalidateChildren(TransformFlagBit dirtyFlags) const {
    auto &&transform = vmath::slice(buffer, _idx);

    if (hasAllFlags(transform.dirtyFlags, dirtyFlags)) {
        return;
    }

    transform.dirtyFlags |= dirtyFlags;
    TransformFlagBit newDirtyBits = dirtyFlags | TransformFlagBit::POSITION;

    for (size_t i = 0; i < transform.childrenCount; ++i) {
        vmath::slice(views, transform.children[i])->invalidateChildren(newDirtyBits);
    }
}

namespace {
TransformF::IndexX tempArray;
}

void Transform::updateWorldTransform() const {
    if (vmath::slice(buffer.dirtyFlags, _idx) == TransformFlagBit::NONE) {
        return;
    }

    vmath::Index curIdx = _idx;

    size_t i = 0U;
    while (curIdx >= 0 && vmath::slice(buffer.dirtyFlags, curIdx) != TransformFlagBit::NONE) {
        tempArray[i++] = curIdx;

        curIdx = vmath::slice(buffer.parent, curIdx);
    }

    TransformFlagBit dirtyBits = TransformFlagBit::NONE;

    while (i) {
        vmath::Index childIdx = tempArray[--i];
        auto &&      child    = vmath::slice(buffer, childIdx);
        dirtyBits |= child.dirtyFlags;
        if (curIdx >= 0) {
            auto &&cur = vmath::slice(buffer, curIdx);
            if (hasFlag(dirtyBits, TransformFlagBit::POSITION)) {
                child.pos       = vmath::vec3TransformMat4(child.lpos, cur.mat);
                child.mat[3][0] = child.pos[0];
                child.mat[3][1] = child.pos[1];
                child.mat[3][2] = child.pos[2];
            }
            if (hasAnyFlags(dirtyBits, TransformFlagBit::RS)) {
                child.mat = vmath::mat4FromRTS(child.lrot, child.lpos, child.lscale);
                child.mat = cur.mat * child.mat;
                if (hasFlag(dirtyBits, TransformFlagBit::ROTATION)) {
                    child.rot = cur.rot * child.lrot;
                }
                auto tempMat4  = vmath::mat4FromQuat(vmath::conjugate(child.rot)) * child.mat;
                child.scale[0] = tempMat4[0][0];
                child.scale[1] = tempMat4[1][1];
                child.scale[2] = tempMat4[2][2];
            }
        } else {
            if (hasFlag(dirtyBits, TransformFlagBit::POSITION)) {
                child.pos       = child.lpos;
                child.mat[3][0] = child.pos[0];
                child.mat[3][1] = child.pos[1];
                child.mat[3][2] = child.pos[2];
            }
            if (hasAnyFlags(dirtyBits, TransformFlagBit::RS)) {
                if (hasFlag(dirtyBits, TransformFlagBit::ROTATION)) {
                    child.rot = child.lrot;
                }
                if (hasFlag(dirtyBits, TransformFlagBit::SCALE)) {
                    child.scale = child.lscale;
                }
                child.mat = vmath::mat4FromRTS(child.rot, child.pos, child.scale);
            }
        }

        child.dirtyFlags = TransformFlagBit::NONE;

        curIdx = childIdx;
    }
}

ModelX       Model::buffer;
Model::PtrX  Model::views;
vmath::Index Model::viewCount{0};

Model::~Model() {
    if (vmath::slice(views, _idx) != this) return;

    Model *last                      = vmath::slice(views, viewCount--);
    vmath::slice(buffer, last->_idx) = ModelF{};

    if (this != last) {
        vmath::slice(buffer, _idx) = vmath::slice(buffer, last->_idx);
        vmath::slice(views, _idx)  = last;
        last->_idx                 = _idx;
    }
}

void Model::setColor(float r, float g, float b, float a) {
    auto &&model = vmath::slice(buffer, _idx);

    model.color.x() = r;
    model.color.y() = g;
    model.color.z() = b;
    model.color.w() = a;
}

void Model::setTransform(const Transform *transform) {
    auto &&model = vmath::slice(buffer, _idx);

    model.transform = transform->getIdx();
}

void Model::setEnabled(bool enabled) {
    auto &&model = vmath::slice(buffer, _idx);

    model.enabled = enabled;
}

Root *Root::instance = nullptr;

Root::Root() {
    if (!instance) {
        instance = this;
    }

    vmath::setSlices(Model::buffer, 256);
    vmath::setSlices(Model::views, 256);
    vmath::setSlices(Transform::buffer, 256);
    vmath::setSlices(Transform::views, 256);

    for (size_t i = 0; i < vmath::packets(Model::buffer); ++i) {
        vmath::packet(Model::buffer, i) = ModelP{};
        vmath::packet(Model::views, i)  = Model::PtrP{};
        vmath::packet(Transform::buffer, i) = TransformP{};
        vmath::packet(Transform::views, i)  = Transform::PtrP{};
    }
}

Root::~Root() {
    CCASSERT(Model::views.empty(), "Resources leaked");

    if (instance == this) {
        instance = nullptr;
    }
}

Transform *Root::createTransform() {
    if (Transform::viewCount >= vmath::slices(Transform::views)) {
        vmath::setSlices<true>(Transform::views, Transform::viewCount * 2);
        vmath::setSlices<true>(Transform::buffer, Transform::viewCount * 2);
    }
    auto *res = CC_NEW(Transform(Transform::viewCount));

    vmath::slice(Transform::views, Transform::viewCount++) = res;
    return res;
}

Model *Root::createModel() {
    if (Model::viewCount >= vmath::slices(Model::views)) {
        vmath::setSlices<true>(Model::views, Model::viewCount * 2);
        vmath::setSlices<true>(Model::buffer, Model::viewCount * 2);
    }
    auto *res = CC_NEW(Model(Model::viewCount));

    vmath::slice(Model::views, Model::viewCount++) = res;
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
