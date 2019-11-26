#include "MyApp.h"

INT WINAPI WinMain( __in HINSTANCE hInstance, 
	__in_opt HINSTANCE hPrevInstance, 
	__in LPSTR lpCmdLine, 
	__in int nShowCmd )
{
	cocos2d::MyApp app;
	app.Run();

	return EXIT_SUCCESS;
}