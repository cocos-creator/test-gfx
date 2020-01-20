#pragma once

#include <Windows.h>
#include <vector>
#include "Core.h"
#include "tests/TestBase.h"

NS_CC_BEGIN

class GameApp: public Object
{
public:
    GameApp();
    virtual ~GameApp();

public:
    bool Setup();
    void Run();

    // Framework methods.  Derived client class overrides these methods to 
    // implement specific application requirements.
    virtual bool Initialize();
    virtual void destroy();
    virtual void Resize(uint width, uint height);
    virtual void OnKeyDown(WPARAM keyCode);
    virtual void OnMouseLDown(WORD x, WORD y);
    virtual void OnMouseLUp(WORD x, WORD y);
    virtual void OnMouseRDown(WORD x, WORD y);
    virtual void OnMouseRUp(WORD x, WORD y);
    virtual void OnMouseMDown(WORD x, WORD y);
    virtual void OnMouseMUp(WORD x, WORD y);
    virtual void OnMouseMove(WORD x, WORD y);
    virtual void OnMouseWheel(int delta);
    virtual bool CheckDeviceCaps();
    virtual void OnLostDevice();
    virtual void OnResetDevice();
    virtual void FrameMove(float deltaTime);
    virtual void Render();

    LRESULT CALLBACK MessageHandler(HWND hWnd, DWORD msg, WPARAM wParam, LPARAM lParam);
    bool IsDeviceLost();

protected:
    bool InitAppWindow(int screenWidth, int screenHeight, bool bFullscreen = false);
    void DestroyAppWindow();

protected:
    std::wstring              app_name_;
    std::wstring              root_path_;
    HINSTANCE           instance_handlw_;
    bool                is_fullscreen_;
    bool                is_paused_;
    bool                is_device_inited_;
    WindowInfo          _windowInfo;

private:
    using createFunc = TestBaseI * (*)(const WindowInfo& info);
    std::vector<createFunc> _tests;
    TestBaseI* _test    = nullptr;
    int _nextIndex      = 0;
};

extern GameApp *g_pApp;

NS_CC_END
