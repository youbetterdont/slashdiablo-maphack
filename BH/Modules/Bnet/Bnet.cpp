#include "Bnet.h"
#include "../../D2Ptrs.h"
#include "../../BH.h"

unsigned int Bnet::failToJoin;
std::string Bnet::lastName;

Patch* nextGame1 = new Patch(Call, D2MULTI, 0x14D29, (int)Bnet::NextGamePatch, 5);
Patch* nextGame2 = new Patch(Call, D2MULTI, 0x14A0B, (int)Bnet::NextGamePatch, 5);
Patch* ftjPatch = new Patch(Call, D2CLIENT, 0x4363E, (int)FailToJoin_Interception, 6);

void Bnet::OnLoad() {
	showLastGame = BH::config->ReadBoolean("Show Last Game", true);
	failToJoin = BH::config->ReadInt("Fail To Join", 4000);

	if (showLastGame) {
		nextGame1->Install();
		nextGame2->Install();
	}

	if (failToJoin > 0 && !D2CLIENT_GetPlayerUnit())
		ftjPatch->Install();		
}

void Bnet::OnUnload() {
	nextGame1->Remove();
	nextGame2->Remove();

	ftjPatch->Remove();
}

void Bnet::OnGameJoin(const string& name, const string& pass, int diff) {
	if (name.length() > 0)
		lastName = name;
	ftjPatch->Remove();

	nextGame1->Remove();
	nextGame2->Remove();
}

void Bnet::OnGameExit() {
	if (failToJoin > 0)
		ftjPatch->Install();

	if (showLastGame) {
		nextGame1->Install();
		nextGame2->Install();
	}
}

VOID __fastcall Bnet::NextGamePatch(Control* box, BOOL (__stdcall *FunCallBack)(Control*,DWORD,DWORD)) {
	unsigned int numberStartPosition, numberEndPosition;
	if ((numberEndPosition = Bnet::lastName.size()) == 0)
		return;

	// Incrememnt game number, if the last substring of the game name is a number.
	// Credits to Loli BH for inspiration and PhyRaX for requesting.
	std::string newGameName = Bnet::lastName.c_str();

	// Find the start and end of the game number.
	numberStartPosition = numberEndPosition;
	while (numberStartPosition > 0 && Bnet::lastName.at(numberStartPosition - 1) >= '0' && Bnet::lastName.at(numberStartPosition - 1) <= '9') {
		numberStartPosition--;
	}

	// If game number exists, then increment game number by one and replace
	// the string.
	if (numberStartPosition != numberEndPosition) {
		unsigned long long gameNumber = std::stoull(Bnet::lastName.substr(numberStartPosition, numberEndPosition - numberStartPosition));
		gameNumber++;
		newGameName = Bnet::lastName.substr(0, numberStartPosition) + std::to_string(gameNumber);
	}

	wchar_t *wszLastGameName = AnsiToUnicode(newGameName.c_str());

	D2WIN_SetControlText(box, wszLastGameName);
	D2WIN_SelectEditBoxText(box);
	// original code
	D2WIN_SetEditBoxProc(box, FunCallBack);
	delete [] wszLastGameName;
}

void __declspec(naked) FailToJoin_Interception()
{
	__asm
	{
		cmp esi, Bnet::failToJoin;
		ret;
	}
}