#include "GameApp.h"

#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
    #define NOMINMAX
#endif
#include <nn/nn_Windows.h>

#include <nn/vi.h>

#include <nn/fs.h>

#include <mmsystem.h>

#include "platform/FileUtils.h"
#include "tests/TestBase.h"

namespace cc {

GameApp* GameApp::instance{nullptr};

extern "C" {
// Force the NVidia GPU on Optimus systems
_declspec(dllexport) DWORD NvOptimusEnablement = 1;
}

namespace {
void* allocate(size_t size) {
    return std::malloc(size);
}

void deallocate(void* p, size_t size) {
    std::free(p);
}

HINSTANCE        hInstance{0};
HWND             hWnd{0};
nn::vi::Display* nnDisplay{nullptr};
nn::vi::Layer*   nnLayer{nullptr};
} // namespace

GameApp::GameApp() {
    instance = this;

    TCHAR szRootPath[MAX_PATH];
    memset(szRootPath, 0, sizeof(szRootPath));
    GetCurrentDirectory(MAX_PATH, szRootPath);

    std::vector<std::string> path = {"Resources"};
    FileUtils::getInstance()->setSearchPaths(path);
}

void GameApp::initialize() {
    hInstance = GetModuleHandle(nullptr);
    nn::fs::SetAllocator(allocate, deallocate);
    nn::vi::Initialize();

    nn::vi::OpenDefaultDisplay(&nnDisplay);
    nn::vi::CreateLayer(&nnLayer, nnDisplay);
    nn::vi::GetNativeWindow(reinterpret_cast<nn::vi::NativeWindowHandle*>(&hWnd), nnLayer);

    WindowInfo info;
    info.windowHandle  = hWnd;
    info.screen.width  = 1024;
    info.screen.height = 768;
    TestBaseI::setWindowInfo(info);

    TestBaseI::nextTest();
}

void GameApp::destroy() {
    nn::vi::DestroyLayer(nnLayer);
    nn::vi::CloseDisplay(nnDisplay);
    nn::vi::Finalize();

    instance = nullptr;
}

void GameApp::run() {
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

//LRESULT CALLBACK GameApp::MessageHandler(HWND hWnd, DWORD msg, WPARAM wParam, LPARAM lParam) {
//    // Is the application in a minimized or maximized state?
//    static bool minOrMaxed = false;
//    uint        width      = 0u;
//    uint        height     = 0u;
//
//    switch (msg) {
//        // WM_SIZE is sent when the user resizes the window.
//        case WM_SIZE:
//            width   = (uint)LOWORD(lParam);
//            height  = (uint)HIWORD(lParam);
//            _paused = !width || !height;
//            TestBaseI::resizeGlobal(hWnd, width, height, gfx::SurfaceTransform::IDENTITY);
//            break;
//        // WM_CLOSE is sent when the user presses the 'X' button in the
//        // caption bar menu.
//        case WM_CLOSE:
//            TestBaseI::destroyGlobal();
//            _running = false;
//            _paused  = true;
//            break;
//        case WM_KEYDOWN:
//            if (wParam == VK_SPACE) {
//                TestBaseI::spacePressed();
//            }
//            break;
//        case WM_LBUTTONUP:
//            TestBaseI::nextTest(true);
//            break;
//        case WM_RBUTTONUP:
//            TestBaseI::onTouchEnd();
//            break;
//    }
//
//    return DefWindowProc(hWnd, msg, wParam, lParam);
//}

} // namespace cc
