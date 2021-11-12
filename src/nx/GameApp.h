#pragma once

#include "base/Object.h"

namespace cc {

class GameApp : public cc::Object {
public:
    static GameApp *getInstance() { return instance; }
    GameApp();

    void run();

private:
    void initialize();
    void destroy();

    static GameApp *instance;

    bool _running = true;
    bool _paused  = false;
};

} // namespace cc
