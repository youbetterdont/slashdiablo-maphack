#include "D2Helpers.h"
#include "D2Ptrs.h"
#include "D2Stubs.h"
#include "../Common.h"
#include "../Constants.h"

int getWidthWithHD();
int getHeightWithHD();

RosterUnit* FindPlayerRoster(DWORD unitId) {
	for (RosterUnit* roster = (*p_D2CLIENT_PlayerUnitList); roster; roster = roster->pNext) {
		if (roster->dwUnitId == unitId)
			return roster;
	}
	return NULL;
}

int GetRelation(UnitAny* unit) {
	UnitAny* player = D2CLIENT_GetPlayerUnit();
	RosterUnit* roster;
	RosterUnit* playerRoster = FindPlayerRoster(player->dwUnitId);

	//Neutral = 2, Partied = 3, You = 1, Hostile = 4
	if (!unit || !player)
		return 2;

	switch(unit->dwType) {
		case UNIT_PLAYER://Player

			// Check if we are the unit.
			if (unit->dwUnitId == player->dwUnitId)
				return 1;//You
			
			// Check if we are in a party with unit.
			roster = FindPlayerRoster(unit->dwUnitId);
			if (playerRoster && roster && playerRoster->wPartyId == roster->wPartyId && roster->wPartyId != INVALID_PARTY_ID)
				return 3;//Partied
			
			// Check if unit is hostile towards us
			if (TestPvpFlag_STUB(unit->dwUnitId, player->dwUnitId, 8))
				return 4;

			return 2;
		break;
		case UNIT_MONSTER://Monster or NPC or Summon
		case UNIT_MISSILE://Missile

			if (unit->dwOwnerId < 0 || unit->dwOwnerType < 0 || unit->dwOwnerType > 5)
				return 4;

			if (unit->dwType == UNIT_MONSTER && unit->dwOwnerType == UNIT_MONSTER)
				return 4;
			
			// Find the owner of the unit.
			UnitAny* owner = D2CLIENT_FindServerSideUnit(unit->dwOwnerId, unit->dwOwnerType);
			if (!owner || owner->dwType != unit->dwOwnerType || owner->dwUnitId != unit->dwOwnerId)
				return 4;

			return GetRelation(owner);
		break;
	}

	return 2;//Neutral
}

bool IsValidMonster(UnitAny *pUnit)
{
	//Have we even been given a unit?
	if (!pUnit)
		return false;

	//Check to see if unit is dead.
	if ((pUnit->dwMode == 0) || (pUnit->dwMode == 12))
		return false;

	if ((pUnit->dwTxtFileNo == 608) && (pUnit->dwMode == 8))
		return false;
	
	if ((pUnit->dwTxtFileNo == 68) && (pUnit->dwMode == 14))// Ignore burrowing maggots
		return false;

	//Hide the water creepers
	if ((pUnit->dwTxtFileNo == 258 || (pUnit->dwTxtFileNo == 261)) && (pUnit->dwMode == 14))
		return false;

	DWORD badMonIds[] = {227, 283, 326, 327, 328, 329, 330, 410, 411, 412, 413, 414, 415, 416, 366, 406,
						 351, 352, 353, 266, 408, 516, 517, 518, 519, 522, 523, 543, 543, 545};

	for (DWORD n = 0; n < 30; n++)
	{
		if (pUnit->dwTxtFileNo == badMonIds[n])
			return false;
	}

	if (D2COMMON_GetUnitStat(pUnit, 172, 0) == 2) 
		return false;

	wchar_t* name = D2CLIENT_GetUnitName(pUnit);
	char* tmp = UnicodeToAnsi(name);

	if ((strcmp(tmp,"an evil force") == 0) || (strcmp(tmp, "dummy") == 0) || (strcmp(tmp, "Maggot") == 0)) {
		delete[] tmp;
		return false;
	}
	delete[] tmp;

	return true;
}

bool GotMultiResValue = false;
bool UsingMultiRes = false;

bool IsUsingMultiRes()
{
	/*
	HMODULE hMods[1024];
	HANDLE hProcess;
	DWORD cbNeeded;
	unsigned int i;

	// Get a handle to the process.
	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
	PROCESS_VM_READ,
	FALSE, GetCurrentProcessId());
	if (NULL == hProcess)
	return 0;

	// Get a list of all the modules in this process.

	if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
	{
	for (i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
	{
	TCHAR szModName[MAX_PATH];

	// Get the full path to the module's file.

	if (GetModuleFileNameEx(hProcess, hMods[i], szModName,
	sizeof(szModName) / sizeof(TCHAR)))
	{
	string modName = szModName;
	std:size_t found = modName.find("D2MultiRes");
	if (found != std::string::npos)
	{
	CloseHandle(hProcess);
	return true;
	}
	}
	}
	}

	// Release the handle to the process.

	CloseHandle(hProcess);
	return 0;
	*/
	if (GotMultiResValue)
		return UsingMultiRes;


	GotMultiResValue = true;
	try
	{
		if (GetModuleHandle("D2MultiRes_113c"))
		{
			UsingMultiRes = true;
			return true;
		}
	}
	catch (const std::exception&)
	{

	}
	return false;
}

unsigned int GetScreenHeight()
{
	RECT rect;
	int height;
	if (IsUsingMultiRes())
	{
		if (GetClientRect(D2GFX_GetHwnd(), &rect))
		{
			height = rect.bottom - rect.top;
		}
		else
		{
			height = getHeightWithHD();
		}
	}
	else
	{
		height = getHeightWithHD();
	}
	return height;
}

unsigned int GetScreenWidth()
{
	RECT rect;
	int width;

	if (IsUsingMultiRes())
	{
		if (GetClientRect(D2GFX_GetHwnd(), &rect))
		{
			width = rect.right - rect.left;
		}
		else
		{
			width = getWidthWithHD();
		}
	}
	else
	{
		width = getWidthWithHD();
	}
	return width;
}

int getWidthWithHD() {
	int width;
	switch(D2GFX_GetScreenSize()) {
	case 0:
		width = 640;
		break;

	case 2:
		width = 800;
		break;

	case 3:
		width = 1344;
		break;

	default:
		width = 640;
		break;
	}

	return width;
}

int getHeightWithHD() {
	int height;
	switch(D2GFX_GetScreenSize()) {
	case 0:
		height = 480;
		break;

	case 2:
		height = 600;
		break;

	case 3:
		height = 700;
		break;

	default:
		height = 480;
		break;
	}

	return height;
}
