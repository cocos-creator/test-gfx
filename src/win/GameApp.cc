#include "GameApp.h"
#include "platform/FileUtils.h"

namespace cc {

GameApp *GameApp::_instance = nullptr;

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    return GameApp::getInstance()->MessageHandler(hWnd, msg, wParam, lParam);
}

//////////////////////////////////////////////////////////////////////////

GameApp::GameApp() {
    _instance = this;

    TCHAR szRootPath[MAX_PATH];
    memset(szRootPath, 0, sizeof(szRootPath));
    GetCurrentDirectory(MAX_PATH, szRootPath);

    std::vector<std::string> path = {"Resources"};
    FileUtils::getInstance()->setSearchPaths(path);
}

void GameApp::createWindow(const std::wstring &name, int width, int height, float x, float y) {
    WNDCLASSEX wc;

    // Setup the windows class with default settings.
    wc.style         = CS_HREDRAW | CS_VREDRAW /*| CS_OWNDC*/;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = _hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_WINLOGO);
    wc.hIconSm       = wc.hIcon;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = name.c_str();
    wc.cbSize        = sizeof(WNDCLASSEX);

    // Register the window class.
    RegisterClassEx(&wc);

    // Place the window in the middle of the screen.
    int posX = GetSystemMetrics(SM_CXSCREEN) * x;
    int posY = GetSystemMetrics(SM_CYSCREEN) * y;

    RECT rect{posX, posY, posX + width, posY + height};
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

    // Create the window with the screen settings and get the handle to it.
    HWND hWnd = CreateWindowEx(WS_EX_APPWINDOW, name.c_str(), name.c_str(),
                               WS_OVERLAPPEDWINDOW /*WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP*/,
                               posX, posY, rect.right - rect.left, rect.bottom - rect.top,
                               NULL, NULL, _hInstance, NULL);

    // Bring the window up on the screen and set it as main focus.
    ShowWindow(hWnd, SW_SHOW);
    SetForegroundWindow(hWnd);
    SetFocus(hWnd);

    _hWnds.push_back(hWnd);
    _windowNames.push_back(name);
}

void GameApp::initialize() {
    // Initialize the windows API.
    int screenWidth  = 1024;
    int screenHeight = 768;

    // Get the instance of this application.
    _hInstance = GetModuleHandle(NULL);

    // Give the application a name.
    createWindow(L"Cocos GFX Test", screenWidth, screenHeight, 0.1, 0.1);

    WindowInfo info;
    info.windowHandle  = _hWnds[0];
    info.screen.width  = screenWidth;
    info.screen.height = screenHeight;

    TestBaseI::setWindowInfo(info);

    /* Second Test Window *
    int testWidth = 800;
    int testHeight = 450;
    createWindow(L"Cocos GFX Test 2", testWidth, testHeight, 0.5, 0.5);

    info.windowHandle  = _hWnds[1];
    info.screen.width  = testWidth;
    info.screen.height = testHeight;
    TestBaseI::setWindowInfo(info);
    // */

    TestBaseI::nextTest();
}

void GameApp::destroy() {
    for (HWND hWnd : _hWnds) {
        DestroyWindow(hWnd);
    }
    _hWnds.clear();

    for (auto &name : _windowNames) {
        UnregisterClass(name.c_str(), _hInstance);
    }
    _windowNames.clear();

    _hInstance = NULL;
    _instance  = nullptr;
}

void GameApp::Run() {
    initialize();

    MSG msg;
    ZeroMemory(&msg, sizeof(MSG));

    while (_running) {
        // Handle the windows messages, including exit & destroy
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        if (!_paused) TestBaseI::update();
    }

    destroy();
}

LRESULT CALLBACK GameApp::MessageHandler(HWND hWnd, DWORD msg, WPARAM wParam, LPARAM lParam) {
    // Is the application in a minimized or maximized state?
    static bool minOrMaxed = false;
    uint        width      = 0u;
    uint        height     = 0u;

    switch (msg) {
        // WM_SIZE is sent when the user resizes the window.
        case WM_SIZE:
            width   = (uint)LOWORD(lParam);
            height  = (uint)HIWORD(lParam);
            _paused = !width || !height;
            TestBaseI::resizeGlobal(hWnd, width, height, gfx::SurfaceTransform::IDENTITY);
            break;
        // WM_CLOSE is sent when the user presses the 'X' button in the
        // caption bar menu.
        case WM_CLOSE:
            TestBaseI::destroyGlobal();
            _running = false;
            _paused  = true;
            break;
        case WM_KEYDOWN:
            if (wParam == VK_SPACE) {
                TestBaseI::spacePressed();
            }
            break;
        case WM_LBUTTONUP:
            TestBaseI::nextTest(true);
            break;
        case WM_RBUTTONUP:
            TestBaseI::onTouchEnd();
            break;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

} // namespace cc
