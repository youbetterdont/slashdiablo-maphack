#pragma once
#ifndef _D2STRUCTS_H
#define _D2STRUCTS_H

#include <windows.h>

#pragma warning ( push )
#pragma warning ( disable: 4201 )
#pragma optimize ( "", off )

struct UnitAny;
struct Room1;
struct Room2;
struct Level;
struct Act;
struct ActMisc;
struct RosterUnit;
struct OverheadMsg;

struct InventoryInfo {
	int nLocation;
	int nMaxXCells;
	int nMaxYCells;
};

struct GameStructInfo {
	BYTE _1[0x1B];					//0x00
	char szGameName[0x18];			//0x1B
	char szGameServerIp[0x56];		//0x33
	char szAccountName[0x30];		//0x89
	char szCharName[0x18];			//0xB9
	char szRealmName[0x18];			//0xD1
	BYTE _2[0x158];					//0xE9
	char szGamePassword[0x18];		//0x241
};

struct AutomapCell {
	DWORD fSaved;					//0x00
	WORD nCellNo;					//0x04
	WORD xPixel;					//0x06
	WORD yPixel;					//0x08
	WORD wWeight;					//0x0A
	AutomapCell *pLess;				//0x0C
	AutomapCell *pMore;				//0x10
};

struct GfxCell {
	DWORD flags;					//0x00
	DWORD width;					//0x04
	DWORD height;					//0x08
	DWORD xoffs;					//0x0C
	DWORD yoffs;					//0x10
	DWORD _2;						//0x14
	DWORD lpParent;					//0x18
	DWORD length;					//0x1C
	BYTE cols;						//0x20
};

struct InteractStruct {
	DWORD dwMoveType;			//0x00
	UnitAny* lpPlayerUnit;		//0x04
	UnitAny* lpTargetUnit;		//0x08
	DWORD dwTargetX;			//0x0C
	DWORD dwTargetY;			//0x10
	DWORD _1;					//0x14
	DWORD _2;					//0x18
};

struct CellFile {
	DWORD dwVersion;				//0x00
	struct {
		WORD dwFlags;
		BYTE mylastcol;
		BYTE mytabno:1;
	};								//0x04
	DWORD eFormat;					//0x08
	DWORD termination;				//0x0C
	DWORD numdirs;					//0x10
	DWORD numcells;					//0x14
	GfxCell *cells[1];				//0x18
};

struct CellContext {
	DWORD nCellNo;					//0x00
	DWORD _1[12];					//0x04
	CellFile* pCellFile;			//0x34
};

struct AutomapLayer {
	DWORD nLayerNo;					//0x00
	DWORD fSaved;					//0x04
	AutomapCell *pFloors;			//0x08
	AutomapCell *pWalls;			//0x0C
	AutomapCell *pObjects;			//0x10
	AutomapCell *pExtras;			//0x14
	AutomapLayer *pNextLayer;		//0x18
};

struct AutomapLayer2 {
	DWORD _1[2];					//0x00
	DWORD nLayerNo;					//0x08
};

struct LevelText {
	DWORD dwLevelNo;				//0x00
	DWORD _1[60];					//0x04
	BYTE _2;						//0xF4
	char szName[40];				//0xF5
	char szEntranceText[40];		//0x11D
	char szLevelDesc[41];			//0x145
	wchar_t wName[40];				//0x16E
	wchar_t wEntranceText[40];		//0x1BE
	BYTE nObjGroup[8];				//0x196
	BYTE nObjPrb[8];				//0x19E
};

struct ControlPreferences
{
	DWORD dwType;//0x00
	// ..
};

struct ControlText {
	wchar_t* wText;		//0x00
	wchar_t* wText2;	//0x04
	DWORD _1[3];		//0x08
	DWORD dwColor;		//0x14
	DWORD _2;			//0x18
	ControlText* pNext;	//0x1C
};

struct Control {
   DWORD dwType;					//0x00
   CellFile *pCellFile;				//0x04
   DWORD dwDisabled;				//0x08
   DWORD dwPosX;					//0x0C
   DWORD dwPosY;					//0x10
   DWORD dwSizeX;					//0x14
   DWORD dwSizeY;					//0x18
   VOID (__fastcall *Initialize)(Control* pControl);//0x1c
   DWORD _3;						//0x20
   DWORD *_4;						//0x24
   DWORD *_5;						//0x28
   DWORD _6;						//0x2C
   DWORD *_7;						//0x30
   DWORD *_8;						//0x34
   DWORD _9;						//0x38
   Control* pNext;					//0x3C
   DWORD _10;						//0x40
   DWORD dwMaxLength;				//0x44
   union {
	   ControlText* pFirstText;		//0x48
	   DWORD dwScrollEntries;		//0x48
   };
   union {
       ControlText* pLastText;		//0x4C
	   DWORD dwScrollPosition;		//0x4C
   };
   ControlText* pSelectedText;		//0x50
   DWORD dwSelectEnd;				//0x54
   DWORD dwSelectStart;				//0x58
   wchar_t wText[0x1A];				//0x5C
   Control* pChildControl;			//0x90
};

#pragma pack(push)
#pragma pack(1)

struct RoomTile {
	Room2* pRoom2;				//0x00
	RoomTile* pNext; 			//0x04
	DWORD _2[2];				//0x08
	DWORD *nNum; 				//0x10
};

struct RosterUnit {
	char szName[16];				//0x00
	DWORD dwUnitId;					//0x10
	DWORD dwPartyLife;				//0x14
	DWORD _1;						//0x18
	DWORD dwClassId;                //0x1C
	WORD wLevel;					//0x20
	WORD wPartyId;  				//0x22
	DWORD dwLevelId;                //0x24
	DWORD Xpos;						//0x28
	DWORD Ypos;						//0x2C
	DWORD dwPartyFlags;				//0x30
	BYTE * _5;						//0x34
	DWORD _6[11];					//0x38
	WORD _7;						//0x64
	char szName2[16];				//0x66
	WORD _8;						//0x76
	DWORD _9[2];					//0x78
	RosterUnit * pNext;				//0x80
};

struct QuestInfo {
	void *pBuffer;					//0x00
	DWORD _1;						//0x04
};

struct Waypoint {
	BYTE flags;						//0x00
};

struct PlayerData {
	char szName[0x10];				//0x00
	QuestInfo *pNormalQuest;		//0x10
	QuestInfo *pNightmareQuest;		//0x14
	QuestInfo *pHellQuest;			//0x18
	Waypoint *pNormalWaypoint;		//0x1c
	Waypoint *pNightmareWaypoint;	//0x20
	Waypoint *pHellWaypoint;		//0x24 Lots of stuff omitted after this point
};

struct CollMap {
	DWORD dwPosGameX;				//0x00
	DWORD dwPosGameY;				//0x04
	DWORD dwSizeGameX;				//0x08
	DWORD dwSizeGameY;				//0x0C
	DWORD dwPosRoomX;				//0x10
	DWORD dwPosRoomY;				//0x14
	DWORD dwSizeRoomX;				//0x18
	DWORD dwSizeRoomY;				//0x1C
	WORD *pMapStart;				//0x20
	WORD *pMapEnd;					//0x22
};

struct PresetUnit {
	DWORD _1;						//0x00
	DWORD dwTxtFileNo;				//0x04
	DWORD dwPosX;					//0x08
	PresetUnit* pPresetNext;		//0x0C
	DWORD _3;						//0x10
	DWORD dwType;					//0x14
	DWORD dwPosY;					//0x18
};

struct Level {
	DWORD _1[4];			//0x00
	Room2* pRoom2First;		//0x10
	DWORD _2[2];			//0x14
	DWORD dwPosX;			//0x1C
	DWORD dwPosY;			//0x20
	DWORD dwSizeX;			//0x24
	DWORD dwSizeY;			//0x28
	DWORD _3[96];			//0x2C
	Level* pNextLevel;		//0x1AC
	DWORD _4;				//0x1B0
	ActMisc* pMisc;			//0x1B4
	DWORD _5[6];			//0x1BC
	DWORD dwLevelNo;		//0x1D0
};

struct Room2 {
	DWORD _1[2];			//0x00
	Room2** pRoom2Near;		//0x08
	DWORD _2[5];			//0x0C
	struct {
		DWORD dwRoomNumber; //0x00
		DWORD _1;			//0x04
		DWORD* pdwSubNumber;//0x08
	} *pType2Info;			//0x20
	Room2* pRoom2Next;		//0x24
	DWORD dwRoomFlags;		//0x28
	DWORD dwRoomsNear;		//0x2C
	Room1* pRoom1;			//0x30
	DWORD dwPosX;			//0x34
	DWORD dwPosY;			//0x38
	DWORD dwSizeX;			//0x3C
	DWORD dwSizeY;			//0x40
	DWORD _3;				//0x44
	DWORD dwPresetType;		//0x48
	RoomTile* pRoomTiles;	//0x4C
	DWORD _4[2];			//0x50
	Level* pLevel;			//0x58
	PresetUnit* pPreset;	//0x5C
};

#pragma pack(pop)

struct Room1 {
	Room1** pRoomsNear; 	//0x00
	DWORD _1[3];			//0x04
	Room2* pRoom2;			//0x10
	DWORD _2[3];			//0x14
	CollMap* Coll;			//0x20
	DWORD dwRoomsNear;		//0x24
	DWORD _3[9];			//0x28
	DWORD dwXStart;			//0x4C
	DWORD dwYStart;			//0x50
	DWORD dwXSize;			//0x54
	DWORD dwYSize;			//0x58
	DWORD _4[6];			//0x5C
	UnitAny* pUnitFirst;	//0x74
	DWORD _5;				//0x78
	Room1* pRoomNext;		//0x7C
};

struct ActMisc {
	DWORD _1[37];			//0x00
	DWORD dwStaffTombLevel; //0x94
	DWORD _2[245];			//0x98
	Act* pAct;				//0x46C
	DWORD _3[3];			//0x470
	Level* pLevelFirst;		//0x47C
};

struct Act {
	DWORD _1[3];			//0x00
	DWORD dwMapSeed;		//0x0C
	Room1* pRoom1;			//0x10
	DWORD dwAct;			//0x14
	DWORD _2[12];			//0x18
	ActMisc* pMisc;			//0x48
};

struct Path {
	WORD xOffset;					//0x00
	WORD xPos;						//0x02
	WORD yOffset;					//0x04
	WORD yPos;						//0x06
	DWORD _1[2];					//0x08
	WORD xTarget;					//0x10
	WORD yTarget;					//0x12
	DWORD _2[2];					//0x14
	Room1 *pRoom1;					//0x1C
	Room1 *pRoomUnk;				//0x20
	DWORD _3[3];					//0x24
	UnitAny *pUnit;					//0x30
	DWORD dwFlags;					//0x34
	DWORD _4;						//0x38
	DWORD dwPathType;				//0x3C
	DWORD dwPrevPathType;			//0x40
	DWORD dwUnitSize;				//0x44
	DWORD _5[4];					//0x48
	UnitAny *pTargetUnit;			//0x58
	DWORD dwTargetType;				//0x5C
	DWORD dwTargetId;				//0x60
	BYTE bDirection;				//0x64
};

struct ItemPath {
	DWORD _1[3];					//0x00
	DWORD dwPosX;					//0x0C
	DWORD dwPosY;					//0x10
	//Use Path for the rest
};

struct Stat {
	WORD wSubIndex;					//0x00
	WORD wStatIndex;				//0x02
	DWORD dwStatValue;				//0x04
};

// Credits to SVR
struct StatList {
	DWORD _1;						//0x00 nUnitId
	UnitAny* pUnit;					//0x04
	DWORD dwUnitType;				//0x08
	DWORD dwUnitId;					//0x0C
	DWORD dwFlags;					//0x10
	DWORD _2[4];					//0x14 id? and some others?
	Stat *pStat;					//0x24 
	WORD wStatCount1;				//0x28
	WORD wnSize;					//0x2A
	StatList *pPrevLink;			//0x2C
	DWORD _3;						//0x30
	StatList *pPrev;				//0x34
	DWORD _4;						//0x38
	StatList *pNext;				//0x3C
	StatList *pSetList;				//0x40
	DWORD _5;						//0x44 UnitAny pChar/pItem
	Stat *pSetStat;					//0x48
	WORD wSetStatCount;				//0x4C (unknown from here on. total size supposedly 0x64)
};

struct InventoryStore {
	DWORD pFirstItem;				//0x00
	DWORD pLastItem;				//0x04
	BYTE Width;						//0x08
	BYTE Height;					//0x09
	BYTE unk[2];					//0x0A
	DWORD pArray;					//0x0C UnitAny* [height][width]
};

struct Inventory {
	DWORD dwSignature;				//0x00 Some flags (?)
	BYTE *bGame1C;					//0x04
	UnitAny *pOwner;				//0x08
	UnitAny *pFirstItem;			//0x0C
	UnitAny *pLastItem;				//0x10
	DWORD _1[2];					//0x14 InventoryStore* [count], DWORD dwStoresCount
	DWORD dwLeftItemUid;			//0x1C
	UnitAny *pCursorItem;			//0x20
	DWORD dwOwnerId;				//0x24
	DWORD dwItemCount;				//0x28
};

struct Light {
	DWORD _1[3];					//0x00
	DWORD dwType;					//0x0C
	DWORD _2[7];					//0x10
	DWORD dwStaticValid;			//0x2C
	int *pnStaticMap;				//0x30
};

struct SkillInfo {
	WORD wSkillId;					//0x00
};

struct Skill {
	SkillInfo *pSkillInfo;			//0x00
	Skill *pNextSkill;				//0x04
	DWORD _1[8];					//0x08
	DWORD dwSkillLevel;				//0x28
	DWORD _2[2];					//0x2C
	DWORD dwFlags;					//0x30
};

struct Info {
	BYTE *pGame1C;					//0x00
	Skill *pFirstSkill;				//0x04
	Skill *pLeftSkill;				//0x08
	Skill *pRightSkill;				//0x0C
};

struct ItemData {
	DWORD dwQuality;				//0x00 see constants.h -> item quality
	DWORD _1[2];					//0x04 seedLow + seedHi?
	DWORD dwItemFlags;				//0x0C 1 = Owned by player, 0xFFFFFFFF = Not owned
	DWORD _2[2];					//0x10 seedStarting + command flags
	DWORD dwFlags;					//0x18 item flags
	DWORD _3[3];					//0x1C global unique id 1-3? action stamp?
	DWORD dwQuality2;				//0x28
	DWORD dwItemLevel;				//0x2C
	WORD version;					//0x30 2 in classic, 101 in LoD. Items brought from classic stay 2
	WORD rarePrefix;				//0x32 rare name prefix
	WORD rareSuffix;				//0x34 rare name suffix
	WORD autoPref;					//0x36 0 unless there is an automod roll. +skills dont count, but +psn on nec head/life on sorc orb does
	WORD wPrefix[3];				//0x38 prefix[0] is non-zero if an item has at least 1 prefix, [2] is 0 unless it has 3
	WORD wSuffix[3];				//0x3E same for suffix
	BYTE BodyLocation;				//0x44
	BYTE ItemLocation;				//0x45 Non-body/belt location (Body/Belt == 0xFF)
	BYTE EarLevel;					//0x46
	BYTE VarGfx;					//0x47
	CHAR personalizedName[16];		//0x4A
	WORD _10;						//0x5A
	Inventory *pOwnerInventory;		//0x5C
	UnitAny *pPrevInvItem;			//0x60
	UnitAny *pNextInvItem;			//0x64
	BYTE _11;						//0x68 Unknown
	BYTE NodePage;					//0x69 Actual location, this is the most reliable by far
	WORD _12;						//0x6A
	DWORD _13[6];					//0x6C
	UnitAny *pOwner;				//0x84 actually at 0x80??
};

struct ItemText {
	wchar_t szName2[0x40];			//0x00
	union {
		DWORD dwCode;
		char szCode[4];
	};								//0x40
	BYTE _uncharted1[0x54];			//0x44
	DWORD speed;					//0x98
	BYTE _uncharted2[0x18];			//0x9C
	WORD nLocaleTxtNo;				//0xB4 around 2000 for items available in classic, 22000 for 
	BYTE _uncharted3[0x0E];			//0xB6
	WORD rangeadder;				//0xC4
	BYTE _uncharted4[0x09];			//0xC6
	BYTE xSize;						//0xCF
	BYTE ySize;						//0xD0
	BYTE _uncharted5[0x0d];			//0xD1
	BYTE nType;						//0xDE
	BYTE _uncharted6[0x0d];			//0xDF
	BYTE fQuest;					//0xEC
	BYTE _uncharted7[0x12];			//0xED
	BYTE reqlvl;					//0xFF 
	BYTE magiclvl;					//0x100 continues for a long while with lots of vendor info (but also some maybe useful info?)
};

struct MonsterText {
	BYTE _1[0x6];					//0x00
	WORD nLocaleTxtNo;				//0x06
	WORD flag;						//0x08
	WORD _1a;						//0x0A
	union {
		DWORD flag1;				//0x0C
		struct {
			BYTE flag1a;			//0x0C
			BYTE flag1b;			//0x0D
			BYTE flag1c[2];			//0x0E
		};
	};
	BYTE _2[0x22];					//0x10
	WORD velocity;					//0x32
	BYTE _2a[0x52];					//0x34
	WORD tcs[3][4];					//0x86
	BYTE _2b[0x52];					//0x9E
	wchar_t szDescriptor[0x3c];		//0xF0
	BYTE _3[0x1a0];					//0x12C
};

struct MonsterData {
	BYTE _1[22];					//0x00 WRONG OFFSETS
	struct
	{
		BYTE fUnk:1;
		BYTE fNormal:1;
		BYTE fChamp:1;
		BYTE fBoss:1;
		BYTE fMinion:1;
	};								//0x16
	BYTE _2[5];						//	   WORD szName in here?
	BYTE anEnchants[9];				//0x1C probably smaller than this
	WORD wUniqueNo;					//0x26
	DWORD _5;						//0x28
	struct {
		wchar_t wName[28];
	};								//0x2C
};

struct ObjectTxt {
	char szName[0x40];				//0x00
	wchar_t wszName[0x40];			//0x40
	BYTE _1[4];						//0xC0
	BYTE nSelectable0;				//0xC4
	BYTE _2[0x87];					//0xC5
	BYTE nOrientation;				//0x14C
	BYTE _2b[0x19];					//0x14D
	BYTE nSubClass;					//0x166
	BYTE _3[0x11];					//0x167
	BYTE nParm0;					//0x178
	BYTE _4[0x39];					//0x179
	BYTE nPopulateFn;				//0x1B2
	BYTE nOperateFn;				//0x1B3
	BYTE _5[8];						//0x1B4
	DWORD nAutoMap;					//0x1BB
};

struct ObjectData {
	ObjectTxt *pTxt;				//0x00
	union {
		BYTE Type;					//0x04 (0x0F would be a Exp Shrine)
		struct {
			BYTE _1:7;
			BYTE ChestLocked:1;
		};
	};
	DWORD _2[8];					//0x08
	char szOwner[0x10];				//0x28
};

struct ObjectPath {
	Room1 *pRoom1;					//0x00
	DWORD _1[2];					//0x04
	DWORD dwPosX;					//0x0C
	DWORD dwPosY;					//0x10
	//Leaving rest undefined, use Path
};

struct UnitAny {
	DWORD dwType;					//0x00 0 = player, 1 = monster, (2 = object, 3 = missile?), 4 = item
	DWORD dwTxtFileNo;				//0x04 0-6 = zon-sin, 291 = Iron Golem, 357 = Valkerie, 418 = Shadow Master
	DWORD _1;						//0x08 void* pMemoryPool?
	DWORD dwUnitId;					//0x0C
	DWORD dwMode;					//0x10 1 when standing still, 2 when moving, 8 when doing random animations(charsi hammer/drognan showing his scroll)
	union
	{
		PlayerData *pPlayerData;
		ItemData *pItemData;
		MonsterData *pMonsterData;
		ObjectData *pObjectData;
	};								//0x14
	DWORD dwAct;					//0x18
	Act *pAct;						//0x1C
	DWORD dwSeed[2];				//0x20
	DWORD _2;						//0x28 DWORD dwInitSeed?
	union
	{
		Path *pPath;
		ItemPath *pItemPath;
		ObjectPath *pObjectPath;
	};								//0x2C
	DWORD _3[5];					//0x30 animation stuff?
	DWORD dwGfxFrame;				//0x44
	DWORD dwFrameRemain;			//0x48
	WORD wFrameRate;				//0x4C
	WORD _4;						//0x4E wActionFrame?
	BYTE *pGfxUnk;					//0x50
	DWORD *pGfxInfo;				//0x54
	DWORD _5;						//0x58 pGfxDataCopy?
	StatList *pStats;				//0x5C pStatListEx?
	Inventory *pInventory;			//0x60
	Light *ptLight;					//0x64
	DWORD _6[9];					//0x68 lots of stuff
	WORD wX;						//0x8C
	WORD wY;						//0x8E
	DWORD _7;						//0x90
	DWORD dwOwnerType;				//0x94
	DWORD dwOwnerId;				//0x98
	DWORD _8[2];					//0x9C
	OverheadMsg* pOMsg;				//0xA4
	Info *pInfo;					//0xA8
	DWORD _9[6];					//0xAC skillList? combatList? 4 more dwords
	DWORD dwFlags;					//0xC4
	DWORD dwFlags2;					//0xC8
	DWORD _10[5];					//0xCC some weird stuff
	UnitAny *pChangedNext;			//0xE0
	UnitAny *pRoomNext;				//0xE4
	UnitAny *pListNext;				//0xE8 -> 0xD8
};

struct BnetData {
	DWORD dwId;					//0x00
	DWORD dwId2;				//0x04	
	BYTE _12[13];				//0xC0
	//DWORD dwId3;				//0x14
	//WORD Unk3;				//0x18	
	BYTE _13[6];				//0xC0
	char szGameName[22];		//0x1A
	char szGameIP[16];			//0x30
	DWORD _2[15];				//0x40
	DWORD dwId4;				//0x80
	BYTE _3[5];					//0x84
	char szAccountName[48];		//0x88
	char szPlayerName[24];		//0xB8
	char szRealmName[8];		//0xD0
	BYTE _4[273];				//0xD8
	BYTE nCharClass;			//0x1E9
	BYTE nCharFlags;			//0x1EA
	BYTE nMaxLvlDifference;		//0x1EB
	BYTE _5[31];				//0x1EC
	BYTE nDifficulty;			//0x20B
	void *_6;					//0x20C
	DWORD _7[3];				//0x210
	WORD _8;					//0x224
	BYTE _9[7];					//0x226
	char szRealmName2[24];		//0x227
	char szGamePass[24];		//0x23F
	char szGameDesc[256];		//0x257
	WORD _10;					//0x348
	BYTE _11;					//0x34B
};


struct WardenClientRegion_t {
	DWORD cbAllocSize;				//+00
	DWORD offsetFunc1;				//+04
	DWORD offsetRelocAddressTable;	//+08
	DWORD nRelocCount;				//+0c
	DWORD offsetWardenSetup;		//+10
	DWORD _2[2];
	DWORD offsetImportAddressTable; //+1c
	DWORD nImportDllCount;			//+20
	DWORD nSectionCount;			//+24
};

struct SMemBlock_t {
	DWORD _1[6];
	DWORD cbSize;	//+18
	DWORD _2[31];
	BYTE data[1];	//+98
};

struct WardenClient_t {
	WardenClientRegion_t* pWardenRegion;	//+00
	DWORD cbSize;							//+04
	DWORD nModuleCount;						//+08
	DWORD param;							//+0c
	DWORD fnSetupWarden;					//+10
};

struct WardenIATInfo_t {
	DWORD offsetModuleName;
	DWORD offsetImportTable;
};

struct AttackStruct {
	DWORD dwAttackType;			//0x00
	UnitAny* lpPlayerUnit;		//0x04
	UnitAny* lpTargetUnit;		//0x08
	DWORD dwTargetX;			//0x0C
	DWORD dwTargetY;			//0x10
	DWORD _1;					//0x14
	DWORD _2;					//0x18
};

#pragma pack(push)
#pragma pack(1)

struct NPCMenu {
	DWORD dwNPCClassId;
	DWORD dwEntryAmount;
	WORD wEntryId1;
	WORD wEntryId2;
	WORD wEntryId3;
	WORD wEntryId4;	
	WORD _1;
	DWORD dwEntryFunc1;
	DWORD dwEntryFunc2; 
	DWORD dwEntryFunc3; 
	DWORD dwEntryFunc4;
	BYTE _2[5];
};

struct OverheadMsg {
	DWORD _1;
	DWORD dwTrigger;
	DWORD _2[2];
	char Msg[232];
};

#pragma pack(pop)

struct D2MSG {
	HWND myHWND;
	char lpBuf[256];
};


struct InventoryLayout {
	BYTE SlotWidth;
	BYTE SlotHeight;
	BYTE unk1;
	BYTE unk2;
	DWORD Left;
	DWORD Right;
	DWORD Top;
	DWORD Bottom;
	BYTE SlotPixelWidth;
	BYTE SlotPixelHeight;
};

struct MpqTable; 

struct sgptDataTable {
	MpqTable*	pPlayerClass;			//0x00
	DWORD		dwPlayerClassRecords;	//0x04
	MpqTable*	pBodyLocs;				//0x08
	DWORD		dwBodyLocsRecords;		//0x0c
	MpqTable*	pStorePage;				//0x10
	DWORD		dwStorePageRecords;		//0x04
	MpqTable*	pElemTypes;				//0x08 //All BYTE* are probably mpqTable*, nothing tested
	DWORD		dwElemTypesRecords;     //0x1C
	BYTE*		pHitClass;				//0x20
	DWORD		dwHitClassRecords;      //0x24
	BYTE*		pMonMode;				//0x28
	DWORD		dwMonModeTxt;			//0x2C
	BYTE*		pPlrMode;				//0x30
	DWORD		dwPlrModeRecords;		//0x34
	BYTE*		pSkillCalc;				//0x38
	DWORD		dwSkillCalcRecords;     //0x3C
	BYTE*		pSkillCalcCache;		//0x40
	DWORD		dwSkillCalcCacheRecs;   //0x44
	DWORD		dwSkillCalcCacheDelta;  //0x48
	BYTE*		pSkillDescCalcCache;	//0x4C
	DWORD		dwSkillDescCalcCacheRecs;//0x50
	DWORD		dwSkillDescCalcCacheDelta;//0x54
	BYTE*		pMissCalc;				//0x58
	DWORD		dwMissCalcRecords;      //0x5C
	BYTE*		pMissCalcCache;         //0x60
	DWORD		dwMissCalcCacheRecs;   //0x64
	DWORD		dwMissCalcCacheDelta;   //0x68
	BYTE*		pSkillCodes;         //0x6C
	DWORD		dwSkillCodesRecs;      //0x70
	BYTE*		pEvents;            //0x74
	DWORD		dwEventsRecs;         //0x78
	BYTE*		pCompCodes;            //0x7C
	DWORD		dwCompCodesRecs;      //0x80
	DWORD		dwCompCodes;         //0x84
	BYTE*		pMonAi;               //0x88
	DWORD		dwMonAiRecs;         //0x8C
	DWORD		dwMonAi;            //0x90
	BYTE*		pItem;               //0x94
	BYTE*		pItemCalcCache;         //0x98
	DWORD		dwItemCalcCacheRecs;   //0x9C
	DWORD		dwItemCalcCache;      //0xA0
	BYTE*		pProperties;         //0xA4
	DWORD		dwPropertiesRecs;      //0xA8
	DWORD		dwProporties;         //0xAC
	BYTE*		pRunes;               //0xB0
	BYTE*		pHireDescs;            //0xB4
	DWORD		dwHireDescsRecs;      //0xB8
	BYTE*		pStates;            //0xBC
	DWORD		dwStatesRecs;         //0xC0
	DWORD		dwStates;            //0xC4
	BYTE*		pStateMaskFirst;      //0xC8
	BYTE*		pStateMaskArr[40];      //0xCC
	BYTE*		pProgressiveStates;      //0x16C
	DWORD		dwProgressiveStatesRecs;//0x170
	BYTE*		pCurseStates;         //0x174
	DWORD		dwCurseStatesRecs;      //0x178
	BYTE*		pTransStates;         //0x17C
	DWORD		dwTransStatesRecs;      //0x180
	BYTE*		pActionStates;         //0x184
	DWORD		dwActionStatesRecs;      //0x188
	BYTE*		pColorStates;         //0x18C
	DWORD		dwColorStatesRecs;      //0x190
	BYTE*		pSoundCodes;         //0x194
	BYTE*		pSoundCodesTxt;         //0x198
	DWORD		dwSoundCodesRecs;      //0x19C
	BYTE*		pHirelings;            //0x1A0
	DWORD		dwHirelingsRecs;      //0x1A4
	DWORD		HireNameStart[256];      //0x1A8
	DWORD		HireNameEnd[256];      //0x5A8
	BYTE*		pNpcs;               //0x9A8
	DWORD		dwNpcsRecs;            //0x9AC
	BYTE*		pColorsTxt;            //0x9B0
	BYTE*		pColors;            //0x9B4
	BYTE*		pTreasureClassEx;      //0x9B8
	BYTE*		pTreasureClassExTxt;   //0x9BC
	DWORD		dwTreasureClassExRecs;   //0x9C0
	BYTE*		pChestTreasureClassLst[45];   //0x9C4
	MonsterText* pMonStatsTxt;      //0xA78
	BYTE*		pMonStats;            //0xA7C
	DWORD		dwMonStatsRecs;         //0xA80
	BYTE*		pMonSoundsTxt;         //0xA84
	BYTE*		pMonSounds;            //0xA88
	DWORD		dwMonSoundsRecs;      //0xA8C
	BYTE*		pMonStats2Txt;         //0xA90
	BYTE*		pMonStats2;            //0xA94
	DWORD		dwMonStats2Recs;      //0xA98
	BYTE*		pMonPlaceTxt;         //0xA9C
	BYTE*		pMonPlace;            //0xAA0
	DWORD		dwMonPlaceRecs;         //0xAA4
	BYTE*		pMonPresetTxt;         //0xAA8
	BYTE*		pMonPresetActLst[5];   //0xAAC
	DWORD		dwMonPresetActRecs[5];   //0xAC0
	BYTE*		pSuperUniquesTxt;      //0xAD4
	BYTE*		pSuperUniques;         //0xAD8
	DWORD		dwSuperUniquesRecs;      //0xADC
	WORD		SuperUniqeIdxList[66];   //0xAE0
	BYTE*		pMissilesTxt;         //0xB64
	BYTE*		pMissiles;            //0xB68
	DWORD		dwMissilesRecs;         //0xB6C
	BYTE*		pMonLvl;            //0xB70
	DWORD		dwMonLvlRecs;         //0xB74
	BYTE*		pMonSeqTxt;            //0xB78
	BYTE*		pMonSeq;            //0xB7C
	DWORD		dwMonSeqRecs;         //0xB80
	BYTE*		pMonSequences;         //0xB84
	DWORD		dwMonSequencesRecs;      //0xB88
	BYTE*		pSkillDescTxt;         //0xB8C
	BYTE*		pSkillDesc;            //0xB90
	DWORD		dwSkillDescRecs;      //0xB94
	BYTE*		pSkillsTxt;         //0xB98
	BYTE*		pSkills;            //0xB9C
	DWORD		dwSkillsRecs;         //0xBA0
	BYTE*		pPlayerSkillCount;      //0xBA4
	DWORD		dwPlayerSkillCount;      //0xBA8
	BYTE*		pPlayerSkillList;      //0xBAC
	DWORD		dwPassiveSkillCount;   //0xBB0
	BYTE*		pPasssiveSkillList;      //0xBB4
	BYTE*		pOverlayTxt;         //0xBB8
	BYTE*		pOverlay;            //0xBBC
	DWORD		dwOverlayRecs;         //0xBC0
	BYTE*		pCharStatsTxt;   //0xBC4
	DWORD		dwCharsStatsRecs;      //0xBC8
	BYTE*		pItemStatCostTxt;      //0xBCC
	BYTE*		pItemStatCost;         //0xBD0
	DWORD		dwItemStatCostRecs;      //0xBD4
	BYTE*		pOpStatNesting;         //0xBD8
	DWORD		dwOpStatNestingRecs;   //0xBDC
	BYTE*		pMonEquip;            //0xBE0
	DWORD		dwMonEquipRecs;         //0xBE4
	BYTE*		pPetTypesTxt;         //0xBE8
	BYTE*		pPetTypes;            //0xBEC
	DWORD		dwPetTypesRecs;         //0xBF0
	BYTE*		pItemsType;            //0xBF4
	BYTE*		pItemsTypeTxt;         //0xBF8
	DWORD		dwItemsTypeRecs;      //0xBFC
	DWORD		dwItemsTypeNesting;      //0xC00
	BYTE*		pItemsTypeNesting;      //0xC04
	BYTE*		pSets;               //0xC08
	BYTE*		pSetsTxt;            //0xC0C
	DWORD		dwSetsRecs;            //0xC10
	BYTE*		pSetItems;            //0xC14
	BYTE*		pSetItemsTxt;         //0xC18
	DWORD		dwSetItemsRecs;         //0xC1C
	BYTE*		pUniqueItems;         //0xC20
	BYTE*		pUniqueItemsTxt;      //0xC24
	DWORD		dwUniqItemsRecs;      //0xC28
	BYTE*		pMonProp;            //0xC2C
	BYTE*		pMonPropTxt;         //0xC30
	DWORD		dwMonPropRecs;         //0xC34
	BYTE*		pMonType;            //0xC38
	BYTE*		pMonTypeTxt;         //0xC3C
	DWORD		dwMonTypeRecs;         //0xC40
	BYTE*		pMonTypeNesting;      //0xC44
	DWORD		dwMonTypeNestingRecs;   //0xC48
	BYTE*		pMonUMod;            //0xC4C
	BYTE*		pMonUModTxt;         //0xC50
	DWORD		dwMonUModRecs;         //0xC54
	BYTE*		pLevels;            //0xC58
	DWORD		dwLevelsRecs;         //0xC5C
	BYTE*		pLvlDefs;            //0xC60
	BYTE*		pLvlPrest;            //0xC64
	DWORD		dwLvlPrestRecs;         //0xC68
	DWORD		ItemStatCostStuff[2];   //0xC6C
	BYTE*		pAnimTables;         //0xC74
	BYTE*		pExperience;         //0xC78
	BYTE*		pDiffLvlsTxt;//0xC7C
	DWORD		dwDiffLvlsRecs;         //0xC80
	BYTE*		pExpFieldD2;         //0xC84
	DWORD		ExpFieldData[10];      //0xC88
	BYTE*		pLvlSubExtra;         //0xCB0
	DWORD		dwLvlSubExtraRecs;      //0xCB4
	BYTE*		pCharTemp;            //0xCB8
	DWORD		dwCharTempRecs;         //0xCBC
	BYTE*		pArena;            //0xCC0
	BYTE*		pLvlTypes;            //0xCC4
	BYTE*		pWaypoints;            //0xCC8
	DWORD		dwWaypointsRecs;      //0xCCC
	DWORD		dwLvlTypes;            //0xCD0
	BYTE*		pLvlWarp;            //0xCD4
	DWORD		dwLvlWarpRecs;         //0xCD8
	BYTE*		pLvlMaze;            //0xCDC
	DWORD		dwLvlMazeRecs;         //0xCE0
	BYTE*		pLvlSub;            //0xCE4
	DWORD		dwLvlSubRecs;         //0xCE8
	BYTE*		pLvlSubCache;         //0xCEC
	DWORD		_1[3];               //0xCF0
	BYTE*		pMapCache;            //0xCFC
	DWORD		dwMapCacheRecs;         //0xD00
	BYTE*		pCubeMain;			//0xD04
	DWORD		dwCubeMainRecs;         //0xD08
	BOOL		bWriteBinFiles;         //0xD0C
};

#pragma warning ( pop )
#pragma optimize ( "", on )

#endif