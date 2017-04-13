#include "StatsDisplay.h"
#include "../Basic/Texthook/Texthook.h"
#include "../Basic/Framehook/Framehook.h"
#include "../Basic/Boxhook/Boxhook.h"
#include "../../D2/D2Ptrs.h"
#include "../../BH.h"

using namespace Drawing;

StatsDisplay *StatsDisplay::display;

// Zon, Sorc, Nec, Pal, Barb, Druid, Sin
int fcrBreakPoints[9][12] = {
	{ 0,	7,		14,		22,		32,		48,		68,		99,		152,	999,	0,		0 },
	{ 0,	9,		20,		37,		63,		105,	200,	999,	0,		0,		0,		0 },
	{ 0,	9,		18,		30,		48,		75,		125,	999,	0,		0,		0,		0 },
	{ 0,	9,		18,		30,		48,		75,		125,	999,	0,		0,		0,		0 },
	{ 0,	9,		20,		37,		63,		105,	200,	999,	0,		0,		0,		0 },
	{ 0,	4,		10,		19,		30,		46,		68,		99,		163,	999,	0,		0 },
	{ 0,	8,		16,		27,		42,		65,		102,	174,	999,	0,		0,		0 },
	{ 0,	7,		15,		23,		35,		52,		78,		117,	194,	999,	0,		0 },	// Lightning
	{ 0,	6,		11,		18,		24,		35,		48,		65,		86,		120,	180,	999 },	// Vampire Necro
};
int fhrBreakPoints[9][12] = {
	{ 0,	6,		13,		20,		32,		52,		86,		174,	600,	999,	0,		0 },
	{ 0,	5,		9,		14,		20,		30,		42,		60,		86,		142,	280,	999 },
	{ 0,	5,		10,		16,		26,		39,		56,		86,		152,	377,	999,	0 },
	{ 0,	7,		15,		27,		48,		86,		200,	999,	0,		0,		0,		0 },
	{ 0,	7,		15,		27,		48,		86,		200,	999,	0,		0,		0,		0 },
	{ 0,	5,		10,		16,		26,		39,		56,		86,		152,	377,	999,	0 },
	{ 0,	7,		15,		27,		48,		86,		200,	999,	0,		0,		0,		0 },
	{ 0,	9,		20,		42,		86,		280,	999,	0,		0,		0,		0,		0 },	// Wolf
	{ 0,	5,		10,		16,		24,		37,		54,		86,		152,	360,	999,	0 },	// Bear
};
int fbrBreakPoints[9][12] = {
	{ 0,	13,		32,		86,		600,	999,	0,		0,		0,		0,		0,		0 },
	{ 0,	7,		15,		27,		48,		86,		200,	999,	0,		0,		0,		0 },	// Sorceress/Wolf
	{ 0,	6,		13,		20,		32,		52,		86,		174,	600,	999,	0,		0 },
	{ 0,	13,		32,		86,		600,	999,	0,		0,		0,		0,		0,		0 },
	{ 0,	9,		20,		42,		86,		280,	999,	0,		0,		0,		0,		0 },
	{ 0,	6,		13,		20,		32,		52,		86,		174,	600,	999,	0,		0 },
	{ 0,	13,		32,		86,		600,	999,	0,		0,		0,		0,		0,		0 },
	{ 0,	86,		999,	0,		0,		0,		0,		0,		0,		0,		0,		0 },	// Holy shield
	{ 0,	5,		10,		16,		27,		40,		65,		109,	223,	999,	0,		0 },	// Bear
};
//Base FCR, FHR, FBR
int baseFrames[9][3] = {
	{ 19,	11,		5 },	// Amazon
	{ 13,	15,		9 },	// Sorceress
	{ 15,	13,		11 },	// Necromancer
	{ 15,	9,		5 },	// Paladin
	{ 13,	9,		7 },	// Barbarian
	{ 18,	13,		11 },	// Druid
	{ 16,	9,		5 },	// Assassin
	{ 19,	16,		2 },	// Lightning fcr	/ wolf fhr	/ Holy shield fbr
	{ 23,	16,		0 },	// Vamp necro fcr	/ bear fhr
};

StatsDisplay::StatsDisplay(std::string name) {
	int xPos = 10;
	int yPos = 10;
	int width = 210;
	int height = 454;

	InitializeCriticalSection(&crit);
	SetX(xPos);
	SetY(yPos);
	SetXSize(width);
	SetYSize(height);
	SetName(name);
	SetActive(true);
	SetMinimized(true);

	statsKey = BH::config->ReadKey("Character Stats", "VK_8");
	display = this;
}

StatsDisplay::~StatsDisplay() {
	Lock();
	// Remove all hooks associated with the display
	while (Hooks.size() > 0) {
		delete (*Hooks.begin());
	}
	Unlock();
	DeleteCriticalSection(&crit);
}

void StatsDisplay::SetX(unsigned int newX) {
	if (newX >= 0 && newX <= BH::GetGameWidth()) {
		Lock();
		x = newX;
		Unlock();
	}
}

void StatsDisplay::SetY(unsigned int newY) {
	if (newY >= 0 && newY <= BH::GetGameHeight()) {
		Lock();
		y = newY;
		Unlock();
	}
}

void StatsDisplay::SetXSize(unsigned int newXSize) {
	//if (newXSize >= 0 && newXSize <= (BH::GetGameHeight() - GetX())) {
		Lock();
		xSize = newXSize;
		Unlock();
	//}
}

void StatsDisplay::SetYSize(unsigned int newYSize) {
	//if (newYSize >= 0 && newYSize <= (BH::GetGameHeight() - GetX())) {
		Lock();
		ySize = newYSize;
		Unlock();
	//}
}

bool StatsDisplay::InRange(unsigned int x, unsigned int y) {
	return IsActive() &&
		x >= GetX() && y >= GetY() &&
		x <= GetX() + GetXSize() && y <= GetY() + GetYSize();
}

void StatsDisplay::Draw() {
	display->Lock();
	display->OnDraw();
	display->Unlock();
}

void StatsDisplay::OnDraw() {
	UnitAny *unit = D2CLIENT_GetPlayerUnit();
	if (!unit)
		return;

	if (!IsMinimized()) {
		for(std::list<Hook*>::iterator it = Hooks.begin(); it != Hooks.end(); it++)
			(*it)->OnDraw();

		if (y != 10)
			SetY(10);

		if (x != 10)
			SetX(10);

		int y = GetY();

		RECT pRect;
		pRect.left = GetX();
		pRect.top = GetY();
		pRect.right = x + GetXSize();
		pRect.bottom = y + GetYSize();
		if (unit->dwTxtFileNo == 1) {
			pRect.bottom += 16;
		}

		Drawing::Boxhook::Draw(pRect.left, pRect.top, pRect.right - x, pRect.bottom - y, White, Drawing::BTBlack);
		Drawing::Framehook::DrawRectStub(&pRect);
		Texthook::Draw(105, (y += 8), Center, 1, Silver, "Character Stats");
		Texthook::Draw(20, (y += 24), None, 6, Gold, "ÿc4Name:ÿc0 %s", unit->pPlayerData->szName);
		Texthook::Draw(20, (y += 16), None, 6, Gold, "ÿc4Level:ÿc0 %d", (int)D2COMMON_GetUnitStat(unit, STAT_LEVEL, 0));
		y += 8;

		BnetData* pData = (*p_D2LAUNCH_BnData);
		int xPacMultiplier = pData->nCharFlags & PLAYER_TYPE_EXPANSION ? 2 : 1;
		int resPenalty[3] = { RES_PENALTY_CLS_NORM, RES_PENALTY_CLS_NM, RES_PENALTY_CLS_HELL };
		int penalty = resPenalty[D2CLIENT_GetDifficulty()] * xPacMultiplier;
		int fMax = (int)D2COMMON_GetUnitStat(unit, STAT_MAXFIRERESIST, 0) + 75;
		int cMax = (int)D2COMMON_GetUnitStat(unit, STAT_MAXCOLDRESIST, 0) + 75;
		int lMax = (int)D2COMMON_GetUnitStat(unit, STAT_MAXLIGHTNINGRESIST, 0) + 75;
		int pMax = (int)D2COMMON_GetUnitStat(unit, STAT_MAXPOISONRESIST, 0) + 75;
		Texthook::Draw(20, (y += 16), None, 6, Red, "ÿc4Fire Resist:ÿc1 %d ÿc0/ %d", (int)D2COMMON_GetUnitStat(unit, STAT_FIRERESIST, 0) + penalty, fMax);
		Texthook::Draw(20, (y += 16), None, 6, Blue, "ÿc4Cold Resist:ÿc3 %d ÿc0/ %d", (int)D2COMMON_GetUnitStat(unit, STAT_COLDRESIST, 0) + penalty, cMax);
		Texthook::Draw(20, (y += 16), None, 6, Yellow, "ÿc4Light Resist:ÿc9 %d ÿc0/ %d", (int)D2COMMON_GetUnitStat(unit, STAT_LIGHTNINGRESIST, 0) + penalty, lMax);
		Texthook::Draw(20, (y += 16), None, 6, Green, "ÿc4Poison Resist:ÿc2 %d ÿc0/ %d", (int)D2COMMON_GetUnitStat(unit, STAT_POISONRESIST, 0) + penalty, pMax);
		y += 8;

		int fAbsorb = (int)D2COMMON_GetUnitStat(unit, STAT_FIREABSORB, 0);
		int fAbsorbPct = (int)D2COMMON_GetUnitStat(unit, STAT_FIREABSORBPERCENT, 0);
		int cAbsorb = (int)D2COMMON_GetUnitStat(unit, STAT_COLDABSORB, 0);
		int cAbsorbPct = (int)D2COMMON_GetUnitStat(unit, STAT_COLDABSORBPERCENT, 0);
		int lAbsorb = (int)D2COMMON_GetUnitStat(unit, STAT_LIGHTNINGABSORB, 0);
		int lAbsorbPct = (int)D2COMMON_GetUnitStat(unit, STAT_LIGHTNINGABSORBPERCENT, 0);
		int mAbsorb = (int)D2COMMON_GetUnitStat(unit, STAT_MAGICABSORB, 0);
		int mAbsorbPct = (int)D2COMMON_GetUnitStat(unit, STAT_MAGICABSORBPERCENT, 0);
		int dmgReduction = (int)D2COMMON_GetUnitStat(unit, STAT_DMGREDUCTION, 0);
		int dmgReductionPct = (int)D2COMMON_GetUnitStat(unit, STAT_DMGREDUCTIONPCT, 0);
		int magReduction = (int)D2COMMON_GetUnitStat(unit, STAT_MAGICDMGREDUCTION, 0);
		int magReductionPct = (int)D2COMMON_GetUnitStat(unit, STAT_MAGICDMGREDUCTIONPCT, 0);
		Texthook::Draw(20, (y += 16), None, 6, Red, "ÿc4Absorption: ÿc1%dÿc0/ÿc1%d%c ÿc3%dÿc0/ÿc3%d%c ÿc9%dÿc0/ÿc9%d%c ÿc8%dÿc0/ÿc8%d%c", fAbsorb, fAbsorbPct, '%', cAbsorb, cAbsorbPct, '%', lAbsorb, lAbsorbPct, '%', mAbsorb, mAbsorbPct, '%');
		Texthook::Draw(20, (y += 16), None, 6, Tan, "ÿc4Damage Reduction: ÿc7%dÿc0/ÿc7%d%c ÿc8%dÿc0/ÿc8%d%c", dmgReduction, dmgReductionPct, '%', magReduction, magReductionPct, '%');
		y += 8;
		
		int playerClass = unit->dwTxtFileNo;
		// FCR
		int vampire = D2COMMON_GetUnitState(unit, 142);
		bpInfo fcrBP;
		if (vampire > 0) {
			fcrBP = GetCurrentBP(8, 0, (int)D2COMMON_GetUnitStat(unit, STAT_FASTERCAST, 0));
		} else {
			fcrBP = GetCurrentBP(playerClass, 0, (int)D2COMMON_GetUnitStat(unit, STAT_FASTERCAST, 0));
		}
		bpInfo lightFcrBP = GetCurrentBP(7, 0, (int)D2COMMON_GetUnitStat(unit, STAT_FASTERCAST, 0));
		
		// FHR
		int wolf = D2COMMON_GetUnitState(unit, 139);
		int bear = D2COMMON_GetUnitState(unit, 140);
		bpInfo fhrBP;
		if (wolf > 0) {
			fhrBP = GetCurrentBP(7, 1, (int)D2COMMON_GetUnitStat(unit, STAT_FASTERHITRECOVERY, 0));
		} else if (bear > 0) {
			fhrBP = GetCurrentBP(8, 1, (int)D2COMMON_GetUnitStat(unit, STAT_FASTERHITRECOVERY, 0));
		} else {
			fhrBP = GetCurrentBP(playerClass, 1, (int)D2COMMON_GetUnitStat(unit, STAT_FASTERHITRECOVERY, 0));
		}

		// FBR
		int holyshield = D2COMMON_GetUnitState(unit, 101);
		bpInfo fbrBP;
		if (holyshield > 0) {
			fbrBP = GetCurrentBP(7, 2, (int)D2COMMON_GetUnitStat(unit, STAT_FASTERBLOCK, 0));
		} else if (wolf > 0) {
			fbrBP = GetCurrentBP(1, 2, (int)D2COMMON_GetUnitStat(unit, STAT_FASTERBLOCK, 0));
		} else if (bear > 0) {
			fbrBP = GetCurrentBP(8, 2, (int)D2COMMON_GetUnitStat(unit, STAT_FASTERBLOCK, 0));
		} else {
			fbrBP = GetCurrentBP(playerClass, 2, (int)D2COMMON_GetUnitStat(unit, STAT_FASTERBLOCK, 0));
		}

		Texthook::Draw(20, (y += 16), None, 6, Gold, "ÿc4Faster Cast Rate:ÿc0 %d%s", (int)D2COMMON_GetUnitStat(unit, STAT_FASTERCAST, 0), "%");		
		Texthook::Draw(20, (y += 16), None, 6, Gold, "ÿc4Faster Hit Recovery:ÿc0 %d%s", (int)D2COMMON_GetUnitStat(unit, STAT_FASTERHITRECOVERY, 0), "%");		
		Texthook::Draw(20, (y += 16), None, 6, Gold, "ÿc4Faster Block Rate:ÿc0 %d%s", (int)D2COMMON_GetUnitStat(unit, STAT_FASTERBLOCK, 0), "%");		
		if (fcrBP.nextBP == 999) {
			Texthook::Draw(20, (y += 16), None, 6, Gold, "ÿc4FCR Breakpoint:ÿc0 %d frames %d - max", fcrBP.frames, fcrBP.currentBP);
		} else {
			Texthook::Draw(20, (y += 16), None, 6, Gold, "ÿc4FCR Breakpoint:ÿc0 %d frames %d - %d", fcrBP.frames, fcrBP.currentBP, fcrBP.nextBP);
		}
		if (playerClass == 1) {
			if (lightFcrBP.nextBP == 999) {
				Texthook::Draw(20, (y += 16), None, 6, Gold, "ÿc4Lightning BP:ÿc0 %d frames %d - max", lightFcrBP.frames, lightFcrBP.currentBP);
			} else {
				Texthook::Draw(20, (y += 16), None, 6, Gold, "ÿc4Lightning BP:ÿc0 %d frames %d - %d", lightFcrBP.frames, lightFcrBP.currentBP, lightFcrBP.nextBP);
			}
		};
		if (fhrBP.nextBP == 999) {
			Texthook::Draw(20, (y += 16), None, 6, Gold, "ÿc4FHR Breakpoint:ÿc0 %d frames %d - max", fhrBP.frames, fhrBP.currentBP);
		} else {
			Texthook::Draw(20, (y += 16), None, 6, Gold, "ÿc4FHR Breakpoint:ÿc0 %d frames %d - %d", fhrBP.frames, fhrBP.currentBP, fhrBP.nextBP);
		}
		if (fbrBP.nextBP == 999) {
			Texthook::Draw(20, (y += 16), None, 6, Gold, "ÿc4FBR Breakpoint:ÿc0 %d frames %d - max", fbrBP.frames, fbrBP.currentBP);
		} else {
			Texthook::Draw(20, (y += 16), None, 6, Gold, "ÿc4FBR Breakpoint:ÿc0 %d frames %d - %d", fbrBP.frames, fbrBP.currentBP, fbrBP.nextBP);
		}
		y += 8;

		Texthook::Draw(20, (y += 16), None, 6, Gold, "ÿc4Increased Attack Speed:ÿc0 %d", (int)D2COMMON_GetUnitStat(unit, STAT_IAS, 0));
		Texthook::Draw(20, (y += 16), None, 6, Gold, "ÿc4Crushing Blow:ÿc0 %d", (int)D2COMMON_GetUnitStat(unit, STAT_CRUSHINGBLOW, 0));
		Texthook::Draw(20, (y += 16), None, 6, Gold, "ÿc4Life Leech:ÿc0 %d", (int)D2COMMON_GetUnitStat(unit, STAT_LIFELEECH, 0));
		Texthook::Draw(20, (y += 16), None, 6, Gold, "ÿc4Mana Leech:ÿc0 %d", (int)D2COMMON_GetUnitStat(unit, STAT_MANALEECH, 0));
		Texthook::Draw(20, (y += 16), None, 6, Gold, "ÿc4Repl Life:ÿc0 %d / %.2f life/sec", (int)D2COMMON_GetUnitStat(unit, STAT_REPLENISHLIFE, 0), ((int)D2COMMON_GetUnitStat(unit, STAT_REPLENISHLIFE, 0) * 25.0) / 256);
		y += 8;

		int cowKingKilled = D2COMMON_GetQuestFlag(D2CLIENT_GetQuestInfo(), 4, 10);
		Texthook::Draw(20, (y += 16), None, 6, Gold, "ÿc4Faster Run/Walk:ÿc0 %d", (int)D2COMMON_GetUnitStat(unit, STAT_FASTERRUNWALK, 0));
		Texthook::Draw(20, (y += 16), None, 6, Gold, "ÿc4Magic Find:ÿc0 %d", (int)D2COMMON_GetUnitStat(unit, STAT_MAGICFIND, 0));
		Texthook::Draw(20, (y += 16), None, 6, Gold, "ÿc4Gold Find:ÿc0 %d", (int)D2COMMON_GetUnitStat(unit, STAT_GOLDFIND, 0));
		Texthook::Draw(20, (y += 16), None, 6, Gold, "ÿc4Stash Gold:ÿc0 %d", (int)D2COMMON_GetUnitStat(unit, STAT_GOLDBANK, 0));
		Texthook::Draw(20, (y += 16), None, 6, Gold, "ÿc4Cow King:ÿc0 %s", cowKingKilled ? "killed" : "alive");
	}
}
//playerClass: 0-6, bpType: 0-2 fcr, fhr, fbr
bpInfo GetCurrentBP(int playerClass, int bpType, int statValue) {
	bpInfo result;

	int currentBP = 0;

	if (bpType == 0) {
		while (statValue >= fcrBreakPoints[playerClass][currentBP + 1]) {
			currentBP++;
		}
		result.nextBP = fcrBreakPoints[playerClass][currentBP + 1];
		result.currentBP = fcrBreakPoints[playerClass][currentBP];

	} else if (bpType == 1) {
		while (statValue >= fhrBreakPoints[playerClass][currentBP + 1]) {
			currentBP++;
		}
		result.nextBP = fhrBreakPoints[playerClass][currentBP + 1];
		result.currentBP = fhrBreakPoints[playerClass][currentBP];

	} else if (bpType == 2) {
		while (statValue >= fbrBreakPoints[playerClass][currentBP + 1]) {
			currentBP++;
		}
		result.nextBP = fbrBreakPoints[playerClass][currentBP + 1];
		result.currentBP = fbrBreakPoints[playerClass][currentBP];
	}

	result.frames = baseFrames[playerClass][bpType] - currentBP;

	return result;
}

bool StatsDisplay::KeyClick(bool bUp, BYTE bKey, LPARAM lParam) {
	display->Lock();
	bool block = display->OnKey(bUp, bKey, lParam);
	display->Unlock();
	return block;
}

bool StatsDisplay::OnKey(bool up, BYTE kkey, LPARAM lParam) {
	UnitAny *unit = D2CLIENT_GetPlayerUnit();
	if (!unit)
		return false;

	if (IsMinimized()) {
		if (!up && kkey == statsKey) {
			SetMinimized(false);
			return true;
		}
	} else {
		if (!up && (kkey == statsKey || kkey == VK_ESCAPE)) {
			SetMinimized(true);
			return true;
		}
	}
	return false;
}

bool StatsDisplay::Click(bool up, unsigned int mouseX, unsigned int mouseY) {
	display->Lock();
	bool block = display->OnClick(up, mouseX, mouseY);
	display->Unlock();
	return block;
}

bool StatsDisplay::OnClick(bool up, unsigned int x, unsigned int y) {
	UnitAny *unit = D2CLIENT_GetPlayerUnit();
	if (!unit)
		return false;

	if (!IsMinimized() && InRange(x, y)) {
		SetMinimized(true);
		return true;
	}
	return false;
}
