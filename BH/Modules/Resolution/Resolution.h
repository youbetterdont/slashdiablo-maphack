#pragma once
#include "../Module.h"
#include "../../Config.h"

class Resolution : public Module {
private:
	int newWidth;
	int newHeight;
	bool isInGame;
	std::map<string, Toggle> Toggles;
	void SetResolution(int x, int y);
	int GetMode(int height);

public:
	Resolution() : Module("Resolution") {};
	void LoadConfig();
	void OnLoad();
	void OnUnload();
	void OnDraw();
	void OnKey(bool up, BYTE key, LPARAM lParam, bool* block);
	void OnGameJoin(const string& name, const string& pass, int diff);
	void OnGameExit();
};
