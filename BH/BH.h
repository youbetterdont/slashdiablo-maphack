#pragma once
#include <string>
#include <Windows.h>
#include "Modules/ModuleManager.h"
#include "Config.h"
#include "Drawing.h"
#include "Patch.h"

using namespace std;

struct cGuardModule
{	
	union {
		HMODULE hModule;
		DWORD dwBaseAddress;
	};
	DWORD _1;
	char szPath[MAX_PATH];
};

namespace BH {
		extern unsigned int gameWidth;
		extern unsigned int gameHeight;

		extern string path;
		extern HINSTANCE instance;
		extern ModuleManager* moduleManager;
		extern Config* config;
		extern Config* itemFilter;
		extern Drawing::UI* settingsUI;
		extern Drawing::StatsDisplay* statsDisplay;
		extern WNDPROC OldWNDPROC;
		extern map<string, Toggle>* ItemToggles;
		extern bool cGuardLoaded;
		extern Patch* oogDraw;

		extern bool Startup(HINSTANCE instance, VOID* reserved);
		extern bool Shutdown();
		extern void ReloadConfig();
		extern unsigned int GetGameWidth();
		extern unsigned int GetGameHeight();
};