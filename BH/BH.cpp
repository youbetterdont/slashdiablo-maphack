#define _DEFINE_PTRS
#include "BH.h"
#include <Shlwapi.h>
#include "D2/D2Ptrs.h"
#include "D2/D2Intercepts.h"
#include "D2/D2Handlers.h"
#include "D2/D2Helpers.h"
#include "Modules.h"

string BH::path;
HINSTANCE BH::instance;
ModuleManager* BH::moduleManager;
Config* BH::config;
Config* BH::itemFilter;
Drawing::UI* BH::settingsUI;
Drawing::StatsDisplay* BH::statsDisplay;
bool BH::cGuardLoaded;
WNDPROC BH::OldWNDPROC;
map<string, Toggle>* BH::ItemToggles;

unsigned int BH::gameHeight;
unsigned int BH::gameWidth;

Patch* patches[] = {
	
	new Patch(Call, D2CLIENT, 0x44230, (int)GameLoop_Interception, 7),
	
	new Patch(Jump, D2CLIENT, 0xC3DB4,	(int)GameDraw_Interception, 6),
	new Patch(Jump, D2CLIENT, 0x626C9, (int)GameAutomapDraw_Interception, 5),
	
	new Patch(Call, BNCLIENT, 0xEABC, (int)ChatPacketRecv_Interception, 12),
	new Patch(Call, D2MCPCLIENT, 0x69D7, (int)RealmPacketRecv_Interception, 5),
	new Patch(Call, D2CLIENT, 0xACE61, (int)GamePacketRecv_Interception, 5),
	new Patch(Call, D2CLIENT, 0x70B75, (int)GameInput_Interception, 5),
	new Patch(Call, D2MULTI, 0xD753, (int)ChannelInput_Interception, 5),
	new Patch(Call, D2MULTI, 0x10781, (int)ChannelWhisper_Interception, 5),
	new Patch(Jump, D2MULTI, 0x108A0, (int)ChannelChat_Interception, 6),
	new Patch(Jump, D2MULTI, 0x107A0, (int)ChannelEmote_Interception, 6),
	new Patch(NOP, D2CLIENT, 0x3CB7C, 0, 9),
	new Patch(Call, D2CLIENT, 0xC3A11, (int)PositionMenuTab_Interception, 12),

	// Nice looking UI patches
	// new Patch(Jump, D2CLIENT, 0x29267, (int)DisplayExpansionUI_Interception, 5),
	//new Patch(Jump, D2CLIENT, 0x2720F, (int)DisplayExpansionUILeftTabTopRight_Interception, 11),
	//new Patch(Jump, D2CLIENT, 0x2722D, (int)DisplayExpansionUILeftTabMiddleTop_Interception, 9),
	//new Patch(Jump, D2CLIENT, 0x2724A, (int)DisplayExpansionUILeftTabBottomLeft_Interception, 9),
	//new Patch(Jump, D2CLIENT, 0x27267, (int)DisplayExpansionUILeftTabBottomRight_Interception, 14),
};

Patch* BH::oogDraw = new Patch(Call, D2WIN, 0x18911, (int)OOGDraw_Interception, 5);

unsigned int index = 0;
bool BH::Startup(HINSTANCE instance, VOID* reserved) {
	
	BH::instance = instance;

	if (reserved != NULL) {
		cGuardModule* pModule = (cGuardModule*)reserved;
		if (!pModule)
			return FALSE;
		path.assign(pModule->szPath);
		cGuardLoaded = true;
	}
	else {
		char szPath[MAX_PATH];
		GetModuleFileName(BH::instance, szPath, MAX_PATH);
		PathRemoveFileSpec(szPath);
		path.assign(szPath);
		path += "\\";
		cGuardLoaded = false;
	}
	

	moduleManager = new ModuleManager();
	config = new Config("BH.cfg");
	config->Parse();

	itemFilter = new Config("ItemDisplay.cfg");
	itemFilter->Parse();

	if(D2GFX_GetHwnd()) {
		BH::OldWNDPROC = (WNDPROC)GetWindowLong(D2GFX_GetHwnd(),GWL_WNDPROC);
		SetWindowLong(D2GFX_GetHwnd(),GWL_WNDPROC,(LONG)GameWindowEvent);
	}

	settingsUI = new Drawing::UI("Settings", 350, 200);

	statsDisplay = new Drawing::StatsDisplay("Stats");
	
	new Maphack();
	new ScreenInfo();
	new Bnet();
	new Item();
	new Pathing();
	new Party();
	new ItemMover();
	new Resolution();

	moduleManager->LoadModules();
	
	ItemToggles = ((Item*)moduleManager->Get("item"))->GetToggles();
	
	// Injection would occasionally deadlock (I only ever saw it when using Tabbed Diablo
	// but theoretically it could happen during regular injection):
	// Worker thread in DllMain->LoadModules->AutoTele::OnLoad->UITab->SetCurrentTab->Lock()
	// Main thread in GameDraw->UI::OnDraw->D2WIN_SetTextSize->GetDllOffset->GetModuleHandle()
	// GetModuleHandle can invoke the loader lock which causes the deadlock, so delay patch
	// installation until after all startup initialization is done.
	
	for (int n = 0; n < (sizeof(patches) / sizeof(Patch*)); n++) {
		patches[n]->Install();
	}
	
	if (!D2CLIENT_GetPlayerUnit())
	{
		oogDraw->Install();
		
		gameWidth = 800;
		gameHeight = 600;
	}
	else
	{
		gameHeight = GetScreenHeight();
		gameWidth = GetScreenWidth();
	}

	// GameThread can potentially run oogDraw->Install, so create the thread after all
	// loading/installation finishes.
	CreateThread(0, 0, GameThread, 0, 0, 0);

	return true;
}

void BH::ReloadConfig() {
	config = new Config("BH.cfg");
	config->Parse();

	itemFilter = new Config("ItemDisplay.cfg");
	itemFilter->Parse();

	moduleManager->ReloadConfig();

	if (!D2CLIENT_GetPlayerUnit())
		D2MULTI_PrintChannelText("Configuration file successfully reloaded.", White);
	else
		PrintText(0, "Configuration file successfully reloaded.");
}

bool BH::Shutdown() {

	moduleManager->UnloadModules();
	
	delete moduleManager;
	delete settingsUI;
	delete statsDisplay;

	SetWindowLong(D2GFX_GetHwnd(),GWL_WNDPROC,(LONG)BH::OldWNDPROC);


	for (int n = 0; n < (sizeof(patches) / sizeof(Patch*)); n++) {
		patches[n]->Remove();
		delete patches[n];
	}

	oogDraw->Remove();
	
	delete config;
	
	return true;
}


unsigned int BH::GetGameHeight()
{
	if(IsUsingMultiRes())
		return BH::gameHeight;

	return GetScreenHeight();
}

unsigned int BH::GetGameWidth()
{
	if (IsUsingMultiRes())
		return BH::gameWidth;

	return GetScreenWidth();
}