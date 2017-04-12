#include "Pathing.h"
#include "../../BH.h"
#include "ATIncludes\CMapIncludes.h"
#include "ATIncludes\Vectors.h"
#include <algorithm>

#define VALIDPTR(x) ( (x) && (!IsBadReadPtr(x,sizeof(x))) )

int waypoints[] = {119,157,156,323,288,402,324,237,238,398,496,511,494};
int CSID = 0;
int CS[] = {392, 394, 396, 255};

using namespace Drawing;

void Pathing::OnLoad() {
	LoadConfig();

	settingsTab = new UITab("Pathing", BH::settingsUI);

	new Texthook(settingsTab, 60, 12, "Toggles");

	new Checkhook(settingsTab, 40, 27, &Toggles["Draw Destination"].state, "Draw Paths");

	//this doesn't change the path.  I can't figure out how to make it work either.
	//new Checkhook(settingsTab, 40, 42, &Toggles["CP to cave"].state, "CP to cave");

	new Texthook(settingsTab, 250, 12, "Path Colors");

	//Blank space to make it easier to click them
	new Colorhook(settingsTab, 250, 27, &Colors[1], "Next   ");

	new Colorhook(settingsTab, 250, 42, &Colors[2], "Other  ");

	new Colorhook(settingsTab, 250, 57, &Colors[3], "WP     ");

	new Colorhook(settingsTab, 250, 72, &Colors[4], "Prev   ");
}

void Pathing::LoadConfig() {
	Toggles["Draw Destination"] = BH::config->ReadToggle("Draw Destination", "None", true);

	Colors[1] = BH::config->ReadInt("Next Color", 0x97);
	Colors[2] = BH::config->ReadInt("Other Color", 0x0A);
	Colors[3] = BH::config->ReadInt("WP Color", 0x84);
	Colors[4] = BH::config->ReadInt("Prev Color", 0x5B);
}

void Pathing::OnAutomapDraw() {
	//if (WaitingForMapData()) {
	//	return;
	//}

	POINT MyPos, Pos;
	Drawing::Hook::ScreenToAutomap(&MyPos, D2CLIENT_GetUnitX(D2CLIENT_GetPlayerUnit()), D2CLIENT_GetUnitY(D2CLIENT_GetPlayerUnit()));

	if(Toggles["Draw Destination"].state) {
		for(int i = 0;i<4;i++) {
			if(Vectors[i].x && Vectors[i].y) {
				Drawing::Hook::ScreenToAutomap(&Pos, Vectors[i].x, Vectors[i].y);
				Drawing::Linehook::Draw(MyPos.x, MyPos.y, Pos.x, Pos.y, Colors[i+1]);
				//PrintText(1, "player x %d player y %d", MyPos.x, MyPos.y);
				Drawing::Crosshook::Draw(Pos.x, Pos.y, Colors[i+1]);
			}
		}
	}
}

void Pathing::OnLoop() {
	//if (WaitingForMapData()) {
	//	return;
	//}
	DWORD playerArea = GetPlayerArea();

	if(playerArea && LastArea != playerArea && D2CLIENT_GetPlayerUnit()) {
		LastArea = playerArea;
		if(LastArea != MAP_A4_THE_CHAOS_SANCTUARY)
			CSID = 0;

		
		GetVectors();
		//LoadHandle = CreateThread(0, 0, LoadNewArea, this, 0, 0);
		//return;
	}
}

void Pathing::OnGamePacketRecv(BYTE* packet, bool* block) {

	if(packet[0] == 0x15) { 
		if(*(DWORD*)&packet[2] == D2CLIENT_GetPlayerUnit()->dwUnitId) {
			packet[10] = 0;  

			//if(Toggles["Fast Teleport"].state) {
			//	UnitAny* Me = D2CLIENT_GetPlayerUnit();

			//	if(Me->dwMode == PLAYER_MODE_DEATH || Me->dwMode == PLAYER_MODE_STAND_OUTTOWN ||
			//		Me->dwMode == PLAYER_MODE_WALK_OUTTOWN || Me->dwMode == PLAYER_MODE_RUN || 
			//		Me->dwMode == PLAYER_MODE_STAND_INTOWN || Me->dwMode == PLAYER_MODE_WALK_INTOWN ||
			//		Me->dwMode == PLAYER_MODE_DEAD || Me->dwMode == PLAYER_MODE_SEQUENCE ||
			//		Me->dwMode == PLAYER_MODE_BEING_KNOCKBACK)
			//		return;

			//	Me->dwFrameRemain = 0;
			//}
			
		}
	}
	return;
}

void Pathing::GetVectors() {
	DWORD MyArea = GetPlayerArea();
	DWORD Areas[2] = {MyArea, 0};
	DWORD AreaCount = 1;
	BOOL buildCollisionMap = false;

	// Usually all markers/paths are in the current area (exceptions include River of Flame,
	// which has its destination marker in the Chaos Sanctuary). First scan the vectors to see
	// which areas we need to load.
	for (int i = 0; i < 4; i++) {
		Vector V = vVector[MyArea*4+i];
		if (V.dwType == EXIT || (MyArea == MAP_A2_CANYON_OF_THE_MAGI && V.dwType == 0 && V.Id == 0)) {
			if (V.Area) {
				Areas[1] = V.Area;
				AreaCount = 2;
			}
			buildCollisionMap = true;
		}
	}

	CCollisionMap g_collisionMap;
	if (buildCollisionMap) {
		buildCollisionMap = g_collisionMap.CreateMap(Areas, AreaCount);  //create a cmap for the current area
	}

	for(int i = 0;i<4;i++) {
		Vectors[i].x = 0;
		Vectors[i].y = 0;
		Vector V = vVector[MyArea*4+i];

		if(MyArea == MAP_A2_CANYON_OF_THE_MAGI) {
			if(V.dwType == 0 && V.Id == 0) {
				V.dwType = EXIT;
				V.Id = GetLevel(D2CLIENT_GetPlayerUnit()->pAct, MyArea)->pMisc->dwStaffTombLevel;
			}
		}

		if(V.dwType == UNIT_PLAYER)
			continue;

		if(V.dwType == XY) {
			Vectors[i].x = V.Id;
			Vectors[i].y = V.Id2;
		}

		if(V.dwType == UNIT_TILE || V.dwType == UNIT_OBJECT || V.dwType == UNIT_MONSTER) {
			if(V.Area)
				Vectors[i] = FindPresetLocation(V.dwType, V.Id, V.Area);
			else
				Vectors[i] = FindPresetLocation(V.dwType, V.Id, MyArea);
		}

		if(V.dwType == EXIT) {
			if (!buildCollisionMap) {
				continue;
			}

			LPLevelExit ExitArray[0x40];//declare an array of exits to store the exits in later

			int ExitCount = g_collisionMap.GetLevelExits(ExitArray); //getlevelexits returns the exitcount

			if(!ExitCount)//if there are 0 tele positions we can stop here :p
				continue;

			for(int j = 0;j<ExitCount;j++) {//loop over evey exit to see if it matches our target
				if(ExitArray[j]->dwTargetLevel == V.Id) {
					Vectors[i] = ExitArray[j]->ptPos;
				}
				delete ExitArray[j];
			}
		}
	}
}

Level* Pathing::GetLevel(Act* pAct, int level) {
	//Insure that the shit we are getting is good.
	if (level < 0 || !pAct)
		return NULL;

	//Loop all the levels in this act

	for(Level* pLevel = pAct->pMisc->pLevelFirst; pLevel; pLevel = pLevel->pNextLevel) {
		//Check if we have reached a bad level.
		if (!pLevel)
			break;

		//If we have found the level, return it!
		if (pLevel->dwLevelNo == level && pLevel->dwPosX > 0)
			return pLevel;
	}
	//Default old-way of finding level.
	return D2COMMON_GetLevel(pAct->pMisc, level);
}

DWORD Pathing::GetPlayerArea() {
	if(VALIDPTR(D2CLIENT_GetPlayerUnit())) {
		if(VALIDPTR(D2CLIENT_GetPlayerUnit()->pPath))
			if(VALIDPTR(D2CLIENT_GetPlayerUnit()->pPath->pRoom1))
				if(VALIDPTR(D2CLIENT_GetPlayerUnit()->pPath->pRoom1->pRoom2))
					if(VALIDPTR(D2CLIENT_GetPlayerUnit()->pPath->pRoom1->pRoom2->pLevel))
						return D2CLIENT_GetPlayerUnit()->pPath->pRoom1->pRoom2->pLevel->dwLevelNo;
	}
	return 0;
}

void Pathing::PrintText(DWORD Color, char *szText, ...) {
	/*
	if(Toggles["Display Messages"].state) {
		char szBuffer[152] = {0};
		va_list Args;
		va_start(Args, szText);
		vsprintf_s(szBuffer,152, szText, Args);
		va_end(Args); 
		wchar_t Buffer[0x130];
		MultiByteToWideChar(0, 1, szBuffer, 152, Buffer, 304);
			D2CLIENT_PrintGameString(Buffer, Color);
	}
	*/
}

POINT Pathing::FindPresetLocation(DWORD dwType, DWORD dwTxtFileNo, DWORD Area)
{
	Level* pLevel = GetLevel(D2CLIENT_GetPlayerUnit()->pAct, Area);

	POINT loc;
	loc.x = 0;
	loc.y = 0;
	DoInteract = 0;

	if(!VALIDPTR(pLevel))
		return loc;

	bool bAddedRoom = false;
	bool stoploop = false;

	for(Room2 *pRoom = pLevel->pRoom2First; pRoom; pRoom = pRoom->pRoom2Next)
	{
		bAddedRoom = false;
		if(!VALIDPTR(pRoom->pPreset) && !VALIDPTR(pRoom->pRoomTiles) && !VALIDPTR(pRoom->pRoom1))
		{
			D2COMMON_AddRoomData(D2CLIENT_GetPlayerUnit()->pAct, pLevel->dwLevelNo, pRoom->dwPosX, pRoom->dwPosY, D2CLIENT_GetPlayerUnit()->pPath->pRoom1);
			bAddedRoom = true;
		}

		for(PresetUnit* pUnit = pRoom->pPreset; pUnit; pUnit = pUnit->pPresetNext)
		{
			//PrintText(4, "X: %d, Y: %d, TxtFileNo: %d, Type: %d", pUnit->dwPosX, pUnit->dwPosY, pUnit->dwTxtFileNo, pUnit->dwType);
			if(((dwType == NULL) || (dwType == pUnit->dwType)) && (dwTxtFileNo == pUnit->dwTxtFileNo))
			{
				if(dwType == UNIT_TILE || (dwType == UNIT_OBJECT && dwTxtFileNo == 298) || (dwType == UNIT_OBJECT && dwTxtFileNo == 580))
				{
					InteractRoom = pRoom;
					InteractType = dwType;
					//DoInteract = 1;
				}
				if(dwType == UNIT_OBJECT)
				{
					for(int i = 0;i <= 13;i++)
					{
						if(waypoints[i] == dwTxtFileNo)
						{
							InteractRoom = pRoom;
							InteractType = dwType;
							//DoInteract = 1;
							stoploop = 1;//stop looping over the rooms
							WaypointLoc.x = (pUnit->dwPosX) + (pRoom->dwPosX * 5);
							WaypointLoc.y = (pUnit->dwPosY) + (pRoom->dwPosY * 5);
							break;
						}
					}
				}

				loc.x = (pUnit->dwPosX) + (pRoom->dwPosX * 5);
				loc.y = (pUnit->dwPosY) + (pRoom->dwPosY * 5);

				stoploop = 1;//stop looping over the rooms
				break;
			}
		}

		if(bAddedRoom)
		{
			D2COMMON_RemoveRoomData(D2CLIENT_GetPlayerUnit()->pAct, pLevel->dwLevelNo, pRoom->dwPosX, pRoom->dwPosY, D2CLIENT_GetPlayerUnit()->pPath->pRoom1);
			bAddedRoom = false;
		}
		if(stoploop)
		{
			stoploop = 0;
			break;
		}
	}

	return loc;
}

DWORD Pathing::GetDistanceSquared(DWORD x1, DWORD y1, DWORD x2, DWORD y2) {
	return (DWORD)sqrt((double)( ((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2)))); }


DWORD Pathing::GetUnitByXY(DWORD X, DWORD Y, Room2* pRoom) {
	UnitAny* pUnit;

	if(!VALIDPTR(pRoom))
		return 0;

	if(!VALIDPTR(pRoom->pRoom1))
		return 0;

	if(!VALIDPTR(pRoom->pRoom1->pUnitFirst))
		return 0;

	pUnit = pRoom->pRoom1->pUnitFirst;

	if(!VALIDPTR(pUnit))
		return 0;

	if(!VALIDPTR(pUnit->pObjectPath))
		return 0;

	while(pUnit) {
		if(pUnit->dwType != UNIT_PLAYER)
			if(X == pUnit->pObjectPath->dwPosX)
				if(Y == pUnit->pObjectPath->dwPosY)
					return pUnit->dwUnitId;
		pUnit=pUnit->pListNext;
	}
	return 0;
}

UnitAny* Pathing::GetUnitObjectByXY(DWORD X, DWORD Y, Room2* pRoom) {
	UnitAny* pUnit;

	if(!VALIDPTR(pRoom))
		return 0;

	if(!VALIDPTR(pRoom->pRoom1))
		return 0;

	if(!VALIDPTR(pRoom->pRoom1->pUnitFirst))
		return 0;

	pUnit = pRoom->pRoom1->pUnitFirst;

	if(!VALIDPTR(pUnit))
		return 0;

	if(!VALIDPTR(pUnit->pObjectPath))
		return 0;

	while(pUnit) {
		if(pUnit->dwType != UNIT_PLAYER)
			if(X == pUnit->pObjectPath->dwPosX)
				if(Y == pUnit->pObjectPath->dwPosY)
					return pUnit;
		pUnit=pUnit->pListNext;
	}
	return 0;
}

bool Pathing::WaitingForMapData() {
	if (LoadHandle) {
		DWORD wait = WaitForSingleObject(LoadHandle, 0);
		if (wait == WAIT_TIMEOUT) {
			return true;
		}
		CloseHandle(LoadHandle);
		LoadHandle = NULL;
	}
	return false;
}

DWORD WINAPI LoadNewArea(VOID* lpvoid) {
	Pathing *at = (Pathing*)lpvoid;
	at->GetVectors();
	return 0;
}
