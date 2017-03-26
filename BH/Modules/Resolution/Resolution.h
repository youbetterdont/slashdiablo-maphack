#pragma once
#include "../Module.h"
#include "../../Config.h"

class Resolution : public Module {
private:
	int newWidth;
	int newHeight;
	bool isInGame;
	void SetResolution(int x, int y);
	int GetMode(int height);

public:
	Resolution() : Module("Resolution") {};
	void OnLoad();
	void OnUnload();
	void OnDraw();
	void OnKey(bool up, BYTE key, LPARAM lParam, bool* block);
	void OnGameJoin(const string& name, const string& pass, int diff);
	void OnGameExit();
};
