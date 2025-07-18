﻿#include "Game/App.hpp"
#include "Game/Gamecommon.hpp"

#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in VERY few places (and .CPPs only)

extern App* g_theApp;


int WINAPI WinMain(HINSTANCE applicationInstanceHandle, HINSTANCE, LPSTR commandLineString, int)
{
	UNUSED(applicationInstanceHandle);
	UNUSED( commandLineString );

	g_theApp = new App(); 
	g_theApp->Startup();
	g_theApp->RunMainLoop();
	g_theApp->Shutdown();
	delete g_theApp;
	g_theApp = nullptr;

	return 0;
}
 