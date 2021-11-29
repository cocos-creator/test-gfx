#include "GameApp.h"

INT WINAPI WinMain(__in HINSTANCE hInstance,
                   __in_opt HINSTANCE hPrevInstance,
                   __in LPSTR lpCmdLine,
                   __in int nShowCmd) {
    cc::GameApp app;
    app.Run();

    return EXIT_SUCCESS;
}
