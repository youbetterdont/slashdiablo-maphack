#pragma once
#include "../../Constants.h"
#include "../../D2Ptrs.h"
#include "../Module.h"
#include "../../Drawing.h"
#include "../../Config.h"
#include "ATIncludes\ArrayEx.h"

typedef struct Vector_t
{
	unsigned int dwType;
	unsigned int Id;
	unsigned int Id2;
	unsigned int Area;
} Vector;

typedef struct Chest_t {
	bool IsOpened;
	POINT Position;
	POINT pRoomPostion;
} Chest;
class Pathing : public Module {
	private:
		std::map<string, Toggle> Toggles;
		unsigned int NextKey, OtherKey, WPKey, PrevKey;
		unsigned int Colors[5];
		Drawing::UITab* settingsTab;

		int Try;
		POINT End, WaypointLoc;
		DWORD _timer, _timer2, _InteractTimer;
		bool SetInteract, SetTele, CastTele, TeleActive, DoInteract;
		DWORD InteractType, InteractId;
		Room2* InteractRoom;
		DWORD LastArea;
		POINT Vectors[4];
		CArrayEx <POINT, POINT> TPath;
		HANDLE LoadHandle;

		//functions
		DWORD GetPlayerArea();
		POINT FindPresetLocation(DWORD dwType, DWORD dwTxtFileNo, DWORD Area);
		void PrintText(DWORD Color, char *szText, ...);

		DWORD GetUnitByXY(DWORD X, DWORD Y, Room2* pRoom);
		UnitAny* GetUnitObjectByXY(DWORD X, DWORD Y, Room2* pRoom);
		bool WaitingForMapData();

	public:
		Pathing() : Module("Pathing"), LoadHandle(NULL) {};
		void OnLoad();
		void LoadConfig();
		void OnLoop();
		void OnAutomapDraw();
		void OnGamePacketRecv(BYTE* packet, bool* block);

		std::map<string, Toggle>* GetToggles() { return &Toggles; }
		void GetVectors();

		static Level* GetLevel(Act* pAct, int level);
		static DWORD GetDistanceSquared(DWORD x1, DWORD y1, DWORD x2, DWORD y2);
};

enum TeleType {
	Next = 0,
	Other,
	WP,
	Prev
};

DWORD WINAPI LoadNewArea(VOID* lpvoid);
