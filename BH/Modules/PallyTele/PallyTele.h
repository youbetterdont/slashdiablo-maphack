#pragma once
#include "../../D2Structs.h"
#include "../Module.h"
#include "../../Config.h"


class PallyTele : public Module {
	private:
		map<std::string, Toggle> Toggles;
		bool IsPaladin;
		int PreviousSkill;
		bool SetSkill(WORD wSkillId, bool Left);
		bool GetSkill(WORD wSkillId);
		int GetClass();
	public:
	PallyTele() : Module("PallyTele") {}

	void OnLoad();
	void OnUnload();

	void OnRightClick(bool up, int x, int y, bool* block);
	void OnGameJoin(const string& name, const string& pass, int diff);
};