#pragma once

#include "tests/TestBase.h"
#include <Windows.h>

namespace cc {

class GameApp : public cc::CCObject {
public:
    static GameApp *getInstance() { return _instance; }
    GameApp();

    void Run();

    LRESULT CALLBACK MessageHandler(HWND hWnd, DWORD msg, WPARAM wParam, LPARAM lParam);

private:
    void initialize();
    bool destroy() override;

    void createWindow(const std::wstring &name, int width, int height, float x, float y);

    static GameApp *_instance;

    bool _running = true;
    bool _paused = false;

    HINSTANCE _hInstance = NULL;

    ccstd::vector<HWND> _hWnds;
    ccstd::vector<std::wstring> _windowNames;
};

} // namespace cc
