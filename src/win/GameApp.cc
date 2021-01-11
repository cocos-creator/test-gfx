#include "GameApp.h"
#include "base/Macros.h"
#include "platform/FileUtils.h"
#include "tests/BasicTextureTest.h"
#include "tests/BasicTriangleTest.h"
#include "tests/BlendTest.h"
#include "tests/BunnyTest.h"
#include "tests/ClearScreenTest.h"
#include "tests/DepthTest.h"
#include "tests/ParticleTest.h"
#include "tests/StencilTest.h"
#include "tests/StressTest.h"

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
    _rootPath = szRootPath;

    std::vector<std::string> path = {"Resources"};
    FileUtils::getInstance()->setSearchPaths(path);
}

void GameApp::initialize() {
    // Initialize the windows API.
    WNDCLASSEX wc;
    DEVMODE dmScreenSettings;
    int posX, posY;
    int screenWidth = 1024;
    int screenHeight = 768;

    // Get the instance of this application.
    _hInstance = GetModuleHandle(NULL);

    // Give the application a name.
    _appName = L"Cocos GFX Test";

    // Setup the windows class with default settings.
    wc.style = CS_HREDRAW | CS_VREDRAW /*| CS_OWNDC*/;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = _hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
    wc.hIconSm = wc.hIcon;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = _appName.c_str();
    wc.cbSize = sizeof(WNDCLASSEX);

    // Register the window class.
    RegisterClassEx(&wc);

    // Setup the screen settings depending on whether it is running in full screen or in windowed mode.
    if (_fullScreen) {
        // Determine the resolution of the clients desktop screen.
        screenWidth = GetSystemMetrics(SM_CXSCREEN);
        screenHeight = GetSystemMetrics(SM_CYSCREEN);

        // If full screen set the screen to maximum size of the users desktop and 32bit.
        memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
        dmScreenSettings.dmSize = sizeof(dmScreenSettings);
        dmScreenSettings.dmPelsWidth = (DWORD)screenWidth;
        dmScreenSettings.dmPelsHeight = (DWORD)screenHeight;
        dmScreenSettings.dmBitsPerPel = 32;
        dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

        // Change the display settings to full screen.
        ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

        // Set the position of the window to the top left corner.
        posX = posY = 0;
    } else {
        // Place the window in the middle of the screen.
        posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
        posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
    }

    RECT rect{posX, posY, posX + screenWidth, posY + screenHeight};
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
    // Create the window with the screen settings and get the handle to it.
    _hWnd = CreateWindowEx(WS_EX_APPWINDOW, _appName.c_str(), _appName.c_str(),
                           WS_OVERLAPPEDWINDOW /*WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP*/,
                           posX, posY, rect.right - rect.left, rect.bottom - rect.top,
                           NULL, NULL, _hInstance, NULL);

    // Bring the window up on the screen and set it as main focus.
    ShowWindow(_hWnd, SW_SHOW);
    SetForegroundWindow(_hWnd);
    SetFocus(_hWnd);

    WindowInfo info;
    info.windowHandle = (intptr_t)_hWnd;
    info.screen.x = posX;
    info.screen.y = posY;
    info.screen.width = info.physicalWidth = screenWidth;
    info.screen.height = info.physicalHeight = screenHeight;

    TestBaseI::setWindowInfo(info);
    TestBaseI::nextTest();
}

void GameApp::Run() {
    initialize();

    MSG msg;
    ZeroMemory(&msg, sizeof(MSG));

    while (true) {
        // Handle the windows messages, including exit & destroy
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        if (!_minimized) TestBaseI::onTick();
    }
}

LRESULT CALLBACK GameApp::MessageHandler(HWND hWnd, DWORD msg, WPARAM wParam, LPARAM lParam) {
    // Is the application in a minimized or maximized state?
    static bool minOrMaxed = false;
    uint width = 0u;
    uint height = 0u;

    switch (msg) {
        // WM_SIZE is sent when the user resizes the window.
        case WM_SIZE:
            width = (uint)LOWORD(lParam);
            height = (uint)HIWORD(lParam);
            _minimized = !width || !height;
            TestBaseI::resizeGlobal(width, height);
            break;
        // WM_CLOSE is sent when the user presses the 'X' button in the
        // caption bar menu.
        case WM_CLOSE:
            TestBaseI::destroyGlobal();

            // Fix the display settings if leaving full screen mode.
            if (_fullScreen) {
                ChangeDisplaySettings(NULL, 0);
            }

            // Remove the window.
            DestroyWindow(_hWnd);

            // Remove the application instance.
            UnregisterClass(_appName.c_str(), _hInstance);

            // Release handles.
            _hWnd = NULL;
            _hInstance = NULL;

            _instance = nullptr;
            exit(0);
            break;
        case WM_KEYDOWN:
            if (wParam == VK_SPACE) {
                TestBaseI::toggleMultithread();
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
