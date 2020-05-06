#include "GameApp.h"
#include "base/ccMacros.h"
#include "platform/CCFileUtils.h"
#include "tests/ClearScreenTest.h"
#include "tests/BasicTriangleTest.h"
#include "tests/BasicTextureTest.h"
#include "tests/DepthTest.h"
#include "tests/StencilTest.h"
#include "tests/BlendTest.h"
#include "tests/ParticleTest.h"
#include "tests/BunnyTest.h"

NS_CC_BEGIN

GameApp *g_pApp = NULL;

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return g_pApp->MessageHandler(hWnd, msg, wParam, lParam);
}

//////////////////////////////////////////////////////////////////////////

GameApp::GameApp()
{
    g_pApp = this;

    TCHAR szRootPath[MAX_PATH];
    memset(szRootPath, 0, sizeof(szRootPath));
    GetCurrentDirectory(MAX_PATH, szRootPath);
    root_path_ = szRootPath;

    _isFullscreen = false;
    _windowInfo.screen.width = 1024;
    _windowInfo.screen.height = 768;
    is_paused_ = false;
    is_device_inited_ = false;
    std::vector<std::string> path = { "Resources" };
    FileUtils::getInstance()->setSearchPaths(path);
}

GameApp::~GameApp()
{
}

bool GameApp::Setup()
{
    // Initialize the windows API.
    if(!InitAppWindow(_windowInfo.screen.width, _windowInfo.screen.height, _isFullscreen))
        return false;

    if (!initialize()) {
        return false;
    }

    is_device_inited_ = true;

    OnResetDevice();

    return true;
}

void GameApp::Run()
{
    if(!Setup())
        return ;

    __int64 cntsPerSec = 0;
    QueryPerformanceFrequency((LARGE_INTEGER*)&cntsPerSec);
    float secsPerCnt = 1.0f / (float)cntsPerSec;

    __int64 prevTimeStamp = 0;
    QueryPerformanceCounter((LARGE_INTEGER*)&prevTimeStamp);

    MSG msg;
    ZeroMemory(&msg, sizeof(MSG));

    // Loop until there is a quit message from the window or the user.
    bool done = false;
    while(!done)
    {
        // Handle the windows messages.
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // If windows signals to end the application then exit out.
        if(msg.message == WM_QUIT)
        {
            done = true;
        }
        else
        {
            // If the application is paused then free some CPU 
            // cycles to other applications and then continue on
            // to the next frame.
            /*
            if( m_bPaused )
            {
            Sleep(20);
            continue;
            }
            */

            if(!IsDeviceLost())
            {
                __int64 currTimeStamp = 0;
                QueryPerformanceCounter((LARGE_INTEGER*)&currTimeStamp);
                float deltaTime = (currTimeStamp - prevTimeStamp)*secsPerCnt;

                FrameMove(deltaTime);
                _test->tick(deltaTime);

                // Prepare for next iteration: The current time stamp becomes
                // the previous time stamp for the next iteration.
                prevTimeStamp = currTimeStamp;
            }
        }
    }

    destroy();

    DestroyAppWindow();
}

bool GameApp::initialize()
{
    static bool first = true;
    if (first)
    {
        _tests = {
            ClearScreen::create,
            BasicTriangle::create,
            BasicTexture::create,
            DepthTexture::create,
            StencilTest::create,
	        BlendTest::create,
            ParticleTest::create,
            BunnyTest::create,
        };
        _test = _tests[_nextIndex](_windowInfo);
        if (_test == nullptr)
            return false;
        first = false;
    }

    return true;
}

void GameApp::destroy()
{
    CC_SAFE_DESTROY(_test);
    TestBaseI::destroyGlobal();
}

void GameApp::resize(uint width, uint height)
{
    TestBaseI::getDevice()->resize(width, height);
}

void GameApp::OnKeyDown(WPARAM keyCode)
{
}

void GameApp::OnMouseLDown(WORD x, WORD y)
{
    _nextIndex = (--_nextIndex + _tests.size()) % _tests.size();
    CC_SAFE_DESTROY(_test);
    _test = _tests[_nextIndex](_windowInfo);
}

void GameApp::OnMouseLUp(WORD x, WORD y)
{
}

void GameApp::OnMouseRDown(WORD x, WORD y)
{
    _nextIndex = (++_nextIndex) % _tests.size();
    CC_SAFE_DESTROY(_test);
    _test = _tests[_nextIndex](_windowInfo);
}

void GameApp::OnMouseRUp(WORD x, WORD y)
{
}

void GameApp::OnMouseMDown(WORD x, WORD y)
{
}

void GameApp::OnMouseMUp(WORD x, WORD y)
{
}

void GameApp::OnMouseMove(WORD x, WORD y)
{
}

void GameApp::OnMouseWheel(int delta)
{
}

bool GameApp::CheckDeviceCaps()
{
    return true;
}

void GameApp::OnLostDevice()
{
}

void GameApp::OnResetDevice()
{
}

void GameApp::FrameMove(float deltaTime)
{
}

void GameApp::Render()
{
}

LRESULT CALLBACK GameApp::MessageHandler(HWND hWnd, DWORD msg, WPARAM wParam, LPARAM lParam)
{
    // Is the application in a minimized or maximized state?
    static bool minOrMaxed = false;

    switch( msg )
    {

    // WM_ACTIVE is sent when the window is activated or deactivated.
    // We pause the game when the main window is deactivated and 
    // unpause it when it becomes active.
    case WM_ACTIVATE:
    {
        if (LOWORD(wParam) == WA_INACTIVE)
            is_paused_ = true;
        else
            is_paused_ = false;
        return 0;
    } break;
    // WM_SIZE is sent when the user resizes the window.  
    case WM_SIZE:
    {
        if(is_device_inited_)
        {
            uint width = (uint)LOWORD(lParam);
            uint height = (uint)HIWORD(lParam);
            resize(width, height);
            //Root::Instance()->onSize(width, height);
        }
    } break;
    // WM_EXITSIZEMOVE is sent when the user releases the resize bars.
    // Here we reset everything based on the new window dimensions.
    case WM_EXITSIZEMOVE:
    {
        RECT clientRect = { 0, 0, 0, 0 };
        GetClientRect((HWND)_windowInfo.windowHandle, &clientRect);
        OnLostDevice();
        OnResetDevice();
    } break;
    // WM_CLOSE is sent when the user presses the 'X' button in the
    // caption bar menu.
    case WM_CLOSE:
    {
         DestroyWindow((HWND)_windowInfo.windowHandle);
    } break;
    // WM_DESTROY is sent when the window is being destroyed.
    case WM_DESTROY:
    {
        PostQuitMessage(0);
    } break;
    case WM_KEYDOWN:
        OnKeyDown(wParam);
        break;
    case WM_LBUTTONDOWN:
        OnMouseLDown(LOWORD(lParam), HIWORD(lParam));
        break;
    case WM_LBUTTONUP:
        OnMouseLUp(LOWORD(lParam), HIWORD(lParam));
        break;
    case WM_RBUTTONDOWN:
        OnMouseRDown(LOWORD(lParam), HIWORD(lParam));
        break;
    case WM_RBUTTONUP:
        OnMouseRUp(LOWORD(lParam), HIWORD(lParam));
        break;
    case WM_MBUTTONDOWN:
        OnMouseMDown(LOWORD(lParam), HIWORD(lParam));
        break;
    case WM_MBUTTONUP:
        OnMouseMUp(LOWORD(lParam), HIWORD(lParam));
        break;
    case WM_MOUSEMOVE:
        OnMouseMove(LOWORD(lParam), HIWORD(lParam));
        break;
    case WM_MOUSEWHEEL:
        OnMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA);
        break;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

bool GameApp::IsDeviceLost()
{
    return false;
}

bool GameApp::InitAppWindow(int screenWidth, int screenHeight, bool bFullscreen)
{
    WNDCLASSEX wc;
    DEVMODE dmScreenSettings;
    int posX, posY;

    // Get the instance of this application.
    instance_handlw_ = GetModuleHandle(NULL);

    // Give the application a name.
    app_name_ = L"Cocos GFX Test";

    // Setup the windows class with default settings.
    wc.style         = CS_HREDRAW | CS_VREDRAW /*| CS_OWNDC*/;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = instance_handlw_;
    wc.hIcon         = LoadIcon(NULL, IDI_WINLOGO);
    wc.hIconSm       = wc.hIcon;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = app_name_.c_str();
    wc.cbSize        = sizeof(WNDCLASSEX);

    // Register the window class.
    RegisterClassEx(&wc);

    // Setup the screen settings depending on whether it is running in full screen or in windowed mode.
    if(bFullscreen)
    {
        // Determine the resolution of the clients desktop screen.
        screenWidth  = GetSystemMetrics(SM_CXSCREEN);
        screenHeight = GetSystemMetrics(SM_CYSCREEN);

        // If full screen set the screen to maximum size of the users desktop and 32bit.
        memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
        dmScreenSettings.dmSize       = sizeof(dmScreenSettings);
        dmScreenSettings.dmPelsWidth  = (DWORD)screenWidth;
        dmScreenSettings.dmPelsHeight = (DWORD)screenHeight;
        dmScreenSettings.dmBitsPerPel = 32;
        dmScreenSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

        // Change the display settings to full screen.
        ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

        // Set the position of the window to the top left corner.
        posX = posY = 0;
    }
    else
    {
        // Place the window in the middle of the screen.
        posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth)  / 2;
        posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
    }

    // Create the window with the screen settings and get the handle to it.
    _windowInfo.windowHandle = (intptr_t)CreateWindowEx(WS_EX_APPWINDOW, app_name_.c_str(), app_name_.c_str(),
                                    WS_OVERLAPPEDWINDOW /*WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP*/,
                                    posX, posY, screenWidth, screenHeight, 
                                    NULL, NULL, instance_handlw_, NULL);
    _windowInfo.screen.x = posX;
    _windowInfo.screen.y = posY;
    _windowInfo.physicalWidth = GetSystemMetrics(SM_CXSCREEN);
    _windowInfo.physicalHeight = GetSystemMetrics(SM_CYSCREEN);
    // Bring the window up on the screen and set it as main focus.
    ShowWindow((HWND)_windowInfo.windowHandle, SW_SHOW);
    SetForegroundWindow((HWND)_windowInfo.windowHandle);
    SetFocus((HWND)_windowInfo.windowHandle);

    // Hide the mouse cursor.
    //ShowCursor(false);

    return true;
}

void GameApp::DestroyAppWindow()
{
    // Show the mouse cursor.
    ShowCursor(true);

    // Fix the display settings if leaving full screen mode.
    if(_isFullscreen)
    {
        ChangeDisplaySettings(NULL, 0);
    }

    // Remove the window.
    DestroyWindow((HWND)_windowInfo.windowHandle);
    _windowInfo.windowHandle = NULL;

    // Remove the application instance.
    UnregisterClass(app_name_.c_str(), instance_handlw_);
    instance_handlw_ = NULL;

    // Release the pointer to this class.
    g_pApp = NULL;
}

NS_CC_END
