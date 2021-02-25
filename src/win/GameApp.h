#pragma once

#include "tests/TestBase.h"
#include <Windows.h>

namespace cc {

class GameApp : public cc::Object {
public:
    static GameApp *getInstance() { return _instance; }
    GameApp();

    void Run();

    LRESULT CALLBACK MessageHandler(HWND hWnd, DWORD msg, WPARAM wParam, LPARAM lParam);

private:
    void initialize();
    void destroy();

    static GameApp *_instance;

    bool _running = true;
    bool _paused = false;

    bool _fullScreen = false;
    std::wstring _appName;
    std::wstring _rootPath;

    HINSTANCE _hInstance = NULL;
    HWND _hWnd = NULL;
};

} // namespace cc
