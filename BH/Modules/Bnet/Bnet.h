#pragma once
#include "../Module.h"

struct Control;

class Bnet : public Module {
	private:
		bool showLastGame;
		bool saveLastPass;
		static unsigned int failToJoin;
		static std::string lastName;
		static std::string lastPass;

	public:

		Bnet() : Module("Bnet") {};

		void OnLoad();
		void OnUnload();

		void OnGameJoin(const string& name, const string& pass, int diff);
		void OnGameExit();

		static VOID __fastcall NextGameNamePatch(Control* box, BOOL(__stdcall *FunCallBack)(Control*, DWORD, DWORD));
		static VOID __fastcall NextGamePasswordPatch(Control* box, BOOL(__stdcall *FunCallBack)(Control*, DWORD, DWORD));
};

void FailToJoin_Interception();