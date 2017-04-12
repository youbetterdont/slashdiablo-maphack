#include "Bnet.h"
#include "../../D2Ptrs.h"
#include "../../BH.h"

unsigned int Bnet::failToJoin;
std::string Bnet::lastName;
std::string Bnet::lastPass;

Patch* nextGameName1 = new Patch(Call, D2MULTI, 0xADAB, (int)Bnet::NextGameNamePatch, 5);
Patch* nextGamePass1 = new Patch(Call, D2MULTI, 0xADE6, (int)Bnet::NextGamePasswordPatch, 5);
Patch* nextGameName2 = new Patch(Call, D2MULTI, 0xB5E9, (int)Bnet::NextGameNamePatch, 5);
Patch* nextGamePass2 = new Patch(Call, D2MULTI, 0xB624, (int)Bnet::NextGamePasswordPatch, 5);
Patch* ftjPatch = new Patch(Call, D2CLIENT, 0x443FE, (int)FailToJoin_Interception, 6);

void Bnet::OnLoad() {
	showLastGame = BH::config->ReadBoolean("Show Last Game", true);
	saveLastPass = BH::config->ReadBoolean("Save Last Password", true);
	failToJoin = BH::config->ReadInt("Fail To Join", 4000);

	if (showLastGame) {
		nextGameName1->Install();
		nextGameName2->Install();
	}

	if (saveLastPass) {
		nextGamePass1->Install();
		nextGamePass2->Install();
	}

	if (failToJoin > 0 && !D2CLIENT_GetPlayerUnit())
		ftjPatch->Install();		
}

void Bnet::OnUnload() {
	nextGameName1->Remove();
	nextGameName2->Remove();
	nextGamePass1->Remove();
	nextGamePass2->Remove();

	ftjPatch->Remove();
}

void Bnet::OnGameJoin(const string& name, const string& pass, int diff) {
	if (name.length() > 0)
		lastName = name;
	lastPass = pass;

	ftjPatch->Remove();

	nextGameName1->Remove();
	nextGameName2->Remove();
	nextGamePass1->Remove();
	nextGamePass2->Remove();
}

void Bnet::OnGameExit() {
	if (failToJoin > 0)
		ftjPatch->Install();

	if (showLastGame) {
		nextGameName1->Install();
		nextGameName2->Install();
	}

	if (saveLastPass) {
		nextGamePass1->Install();
		nextGamePass2->Install();
	}
}

VOID __fastcall Bnet::NextGameNamePatch(Control* box, BOOL(__stdcall *FunCallBack)(Control*, DWORD, DWORD)) {
	unsigned int numberStartPosition, numberEndPosition;
	if ((numberEndPosition = Bnet::lastName.size()) == 0)
		return;

	// Increment game number, if the last substring of the game name is a number.
	// Credits to Loli BH for inspiration and PhyRaX for requesting.
	std::string newGameName = Bnet::lastName;

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

VOID __fastcall Bnet::NextGamePasswordPatch(Control* box, BOOL(__stdcall *FunCallBack)(Control*, DWORD, DWORD)) {
	if (Bnet::lastPass.size() == 0)
		return;

	// Save the game password, because you don't want anyone to interrupt your
	// 50001th solo Lower Kurast run.
	// Credits to Loli BH for inspiration.

	wchar_t *wszLastGamePass = AnsiToUnicode(Bnet::lastPass.c_str());

	D2WIN_SetControlText(box, wszLastGamePass);
	D2WIN_SelectEditBoxText(box);

	// original code
	D2WIN_SetEditBoxProc(box, FunCallBack);

	delete[] wszLastGamePass;
}

void __declspec(naked) FailToJoin_Interception()
{
	__asm
	{
		cmp esi, Bnet::failToJoin;
		ret;
	}
}