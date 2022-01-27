#include "Chassis.h"
#include "base/threading/MessageQueue.h"
#include "gfx-agent/DeviceAgent.h"
#include "gfx-validator/DeviceValidator.h"

namespace cc {
namespace experimental {

TransformX              TransformView::buffer;
vector<vmath::IndexX>   TransformView::childrenBuffers;
vector<TransformView *> TransformView::views;

TransformView::TransformView(vmath::Index idx) : _idx(idx) {
    auto &&children = childrenBuffers[_idx];
    if (vmath::slices(children) < 8) {
        vmath::setSlices(children, 8);
    }
}

TransformView::~TransformView() {
    if (_idx >= views.size() || views[_idx] != this) return;

    TransformView *last = views.back();
    views.pop_back();

    if (this != last) {
        vmath::slice(buffer, _idx) = vmath::slice(buffer, last->_idx);
        views[_idx]                = last;
        last->_idx                 = _idx;
    }

    vmath::slice(buffer, last->_idx) = TransformF{};
}

void TransformView::setParent(TransformView *value) {
    auto &&transform = vmath::slice(buffer, _idx);

    if ((!value && transform.parent < 0) || (value && transform.parent == value->_idx)) return;

    // remove this from former parent
    if (transform.parent >= 0) {
        auto &&parent   = vmath::slice(buffer, transform.parent);
        auto &&children = childrenBuffers[transform.parent];
        for (size_t i = 0; i < parent.childrenCount; ++i) {
            if (vmath::slice(children, i) != _idx) continue;
            vmath::slice(children, i) = vmath::slice(children, --parent.childrenCount);
            break;
        }
    }

    transform.parent = value ? value->_idx : -1;
    invalidateChildren(TransformFlagBit::TRS);

    if (!value) return;

    // add this to new parent
    auto &&newParent = vmath::slice(buffer, value->_idx);
    auto &&children  = childrenBuffers[value->_idx];
    if (vmath::slices(children) <= newParent.childrenCount) {
        vmath::setSlices<true>(children, newParent.childrenCount * 2);
    }
    children[newParent.childrenCount++] = _idx;
}

void TransformView::setPosition(float x, float y, float z) {
    auto &&transform = vmath::slice(buffer, _idx);

    transform.lpos.x() = x;
    transform.lpos.y() = y;
    transform.lpos.z() = z;

    invalidateChildren(TransformFlagBit::POSITION);
}

void TransformView::setRotation(float x, float y, float z, float w) {
    auto &&transform = vmath::slice(buffer, _idx);

    transform.lrot.x() = x;
    transform.lrot.y() = y;
    transform.lrot.z() = z;
    transform.lrot.w() = w;

    invalidateChildren(TransformFlagBit::ROTATION);
}

void TransformView::setRotationFromEuler(float angleX, float angleY, float angleZ) {
    auto &&transform = vmath::slice(buffer, _idx);

    transform.lrot = vmath::quatFromEuler(vmath::Vec3F{angleX, angleY, angleZ});

    invalidateChildren(TransformFlagBit::ROTATION);
}

void TransformView::setScale(float x, float y, float z) {
    auto &&transform = vmath::slice(buffer, _idx);

    transform.lscale.x() = x;
    transform.lscale.y() = y;
    transform.lscale.z() = z;

    invalidateChildren(TransformFlagBit::SCALE);
}

void TransformView::invalidateChildren(TransformFlagBit dirtyFlags) const {
    auto &&transform = vmath::slice(buffer, _idx);
    auto &&children  = childrenBuffers[_idx];

    if (hasAllFlags(transform.dirtyFlags, dirtyFlags)) {
        return;
    }

    transform.dirtyFlags |= dirtyFlags;
    TransformFlagBit newDirtyBits = dirtyFlags | TransformFlagBit::POSITION;

    for (size_t i = 0; i < transform.childrenCount; ++i) {
        views[children[i]]->invalidateChildren(newDirtyBits);
    }
}

namespace {
vector<vmath::Index> tempArray;
}

void TransformView::updateWorldTransform() const {
    if (buffer.dirtyFlags[_idx] == TransformFlagBit::NONE) {
        return;
    }

    vmath::Index curIdx = _idx;

    tempArray.clear();
    while (curIdx >= 0 && buffer.dirtyFlags[curIdx] != TransformFlagBit::NONE) {
        tempArray.push_back(curIdx);

        curIdx = buffer.parent[curIdx];
    }

    TransformFlagBit dirtyBits = TransformFlagBit::NONE;

    size_t i = tempArray.size();
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

ModelX              ModelView::buffer;
vector<ModelView *> ModelView::views;

ModelView::~ModelView() {
    if (_idx >= views.size() || views[_idx] != this) return;

    ModelView *last = views.back();
    views.pop_back();

    if (this != last) {
        vmath::slice(buffer, _idx) = vmath::slice(buffer, last->_idx);
        views[_idx]                = last;
        last->_idx                 = _idx;
    }

    vmath::slice(buffer, last->_idx) = ModelF{};
}

void ModelView::setColor(float r, float g, float b, float a) {
    auto &&model = vmath::slice(buffer, _idx);

    model.color.x() = r;
    model.color.y() = g;
    model.color.z() = b;
    model.color.w() = a;
}

void ModelView::setTransform(const TransformView *transform) {
    auto &&model = vmath::slice(buffer, _idx);

    model.transform = transform->getIdx();
}

void ModelView::setEnabled(bool enabled) {
    auto &&model = vmath::slice(buffer, _idx);

    model.enabled = enabled;
}

Root *Root::instance = nullptr;

Root::Root() {
    if (!instance) {
        instance = this;
    }

    vmath::setSlices(ModelView::buffer, INITIAL_CAPACITY);
    vmath::setSlices(TransformView::buffer, INITIAL_CAPACITY);
    TransformView::childrenBuffers.resize(INITIAL_CAPACITY);

    for (size_t i = 0; i < vmath::packets(ModelView::buffer); ++i) {
        vmath::packet(ModelView::buffer, i)     = ModelP{};
        vmath::packet(TransformView::buffer, i) = TransformP{};
    }
}

Root::~Root() {
    CCASSERT(TransformView::views.empty(), "Resources leaked");
    CCASSERT(ModelView::views.empty(), "Resources leaked");

    if (instance == this) {
        instance = nullptr;
    }
}

TransformView *Root::createTransform() {
    if (TransformView::views.size() >= vmath::slices(TransformView::buffer)) {
        TransformView::childrenBuffers.resize(TransformView::views.size() * 2);
        vmath::setSlices<true>(TransformView::buffer, TransformView::views.size() * 2);
    }
    auto *res = CC_NEW(TransformView(TransformView::views.size()));

    TransformView::views.push_back(res);
    return res;
}

ModelView *Root::createModel() {
    if (ModelView::views.size() >= vmath::slices(ModelView::buffer)) {
        vmath::setSlices<true>(ModelView::buffer, ModelView::views.size() * 2);
    }
    auto *res = CC_NEW(ModelView(ModelView::views.size()));

    ModelView::views.push_back(res);
    return res;
}

///////////////////// Agent /////////////////////

TransformAgent::~TransformAgent() {
    ENQUEUE_MESSAGE_1(
        RootAgent::getInstance()->getMessageQueue(), TransformDestruct,
        actor, getActor(),
        {
            CC_SAFE_DELETE(actor);
        })
}

void TransformAgent::setParent(TransformView *value) {
    ENQUEUE_MESSAGE_2(
        RootAgent::getInstance()->getMessageQueue(), TransformSetParent,
        actor, getActor(),
        value, static_cast<TransformAgent *>(value)->getActor(),
        {
            actor->setParent(value);
        })
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
        })
}

void TransformAgent::setRotation(float x, float y, float z, float w) {
    ENQUEUE_MESSAGE_5(
        RootAgent::getInstance()->getMessageQueue(), TransformSetRotation,
        actor, getActor(),
        x, x,
        y, y,
        z, z,
        w, w,
        {
            actor->setRotation(x, y, z, w);
        })
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
        })
}

void TransformAgent::setScale(float x, float y, float z){
    ENQUEUE_MESSAGE_4(
        RootAgent::getInstance()->getMessageQueue(), TransformSetScale,
        actor, getActor(),
        x, x,
        y, y,
        z, z,
        {
            actor->setScale(x, y, z);
        })}

ModelAgent::~ModelAgent() {
    ENQUEUE_MESSAGE_1(
        RootAgent::getInstance()->getMessageQueue(), ModelDestruct,
        actor, getActor(),
        {
            CC_SAFE_DELETE(actor);
        })
}

void ModelAgent::setColor(float r, float g, float b, float a) {
    ENQUEUE_MESSAGE_5(
        RootAgent::getInstance()->getMessageQueue(), ModelSetColor,
        actor, getActor(),
        r, r, g, g, b, b, a, a,
        {
            actor->setColor(r, g, b, a);
        })
}

void ModelAgent::setTransform(const TransformView *transform) {
    ENQUEUE_MESSAGE_2(
        RootAgent::getInstance()->getMessageQueue(), ModelSetTransform,
        actor, getActor(),
        transform, static_cast<const TransformAgent *>(transform)->getActor(),
        {
            actor->setTransform(transform);
        })
}

void ModelAgent::setEnabled(bool enabled){
    ENQUEUE_MESSAGE_2(
        RootAgent::getInstance()->getMessageQueue(), ModelSetEnabled,
        actor, getActor(),
        enabled, enabled,
        {
            actor->setEnabled(enabled);
        })}

RootAgent *RootAgent::instance = nullptr;

RootAgent::RootAgent(Root *root) : Agent(root) {
    instance = this;
}

RootAgent::~RootAgent() {
    CC_SAFE_DELETE(_actor)
    instance = nullptr;
}

void RootAgent::initialize() {
    _mainMessageQueue = CC_NEW(MessageQueue);

    setMultithreaded(true);

    // Initialize directly in render thread
    // otherwise we would have issues when the device thread is joined.
    // e.g. transfer & submit in different threads is not allowed.
    ENQUEUE_MESSAGE_1(
        getMessageQueue(), RootInitialize,
        actor, getActor(),
        {
            actor->initialize();
        })
}

void RootAgent::destroy() {
    Semaphore  event;
    Semaphore *pEvent   = &event;
    bool *     pRunning = &_running;

    ENQUEUE_MESSAGE_3(
        getMessageQueue(), RootDestroy,
        actor, getActor(),
        pRunning, pRunning,
        pEvent, pEvent,
        {
            actor->destroy();
            *pRunning = false;
            pEvent->signal();
        })

    _mainMessageQueue->finishWriting();
    _pendingTickCount.increment();
    event.wait();

    CC_SAFE_DELETE(_mainMessageQueue)
}

/**
 * Render thread will be running completely asynchronously from
 * logic thread. (No semaphores guarding each frame boundaries)
 * This allows different refresh rates between RT(60 FPS, V-Synced)
 * and LT(e.g. 120 FPS, providing the best responsiveness).
 */
void RootAgent::renderThreadLoop() {
    while (true) {
        uint pendingTickCount = _pendingTickCount.get();
        if (pendingTickCount) {
            _pendingTickCount.subtract(pendingTickCount);
            while (pendingTickCount--) {
                _mainMessageQueue->flushMessages();
            }
            if (!_running) break;
            _actor->render();
        }
    }
}

void RootAgent::render() {
    if (_multithreaded) {
        _mainMessageQueue->finishWriting();
        _pendingTickCount.increment();
    } else {
        _actor->render();
    }
    cc::MessageQueue::freeChunksInFreeQueue(_mainMessageQueue);
}

void RootAgent::setMultithreaded(bool multithreaded) {
    if (multithreaded == _multithreaded) return;

    // Cannot detach if there is no device thread
    if (!gfx::DeviceAgent::getInstance()) return;
    // V8 doesn't support multithreaded invocations
    gfx::DeviceValidator::allowStacktraceJS = !multithreaded;

    _multithreaded = multithreaded;

    if (multithreaded) {
        _mainMessageQueue->setImmediateMode(false);
        _running = true;
        std::thread renderThread(&RootAgent::renderThreadLoop, this);
        renderThread.detach();
        CC_LOG_INFO("Render thread detached.");
    } else {
        Semaphore  event;
        Semaphore *pEvent   = &event;
        bool *     pRunning = &_running;

        ENQUEUE_MESSAGE_2(
            getMessageQueue(), RootJoinThread,
            pRunning, pRunning,
            pEvent, pEvent,
            {
                *pRunning = false;
                pEvent->signal();
            })

        _mainMessageQueue->finishWriting();
        _pendingTickCount.increment();
        event.wait();

        _mainMessageQueue->setImmediateMode(true);
        CC_LOG_INFO("Render thread joined.");
    }
}

TransformView *RootAgent::createTransform() {
    auto *actor = _actor->createTransform();
    return CC_NEW(TransformAgent(actor));
}

ModelView *RootAgent::createModel() {
    auto *actor = _actor->createModel();
    return CC_NEW(ModelAgent(actor));
}

Root *RootManager::instance{nullptr};

} // namespace experimental
} // namespace cc
