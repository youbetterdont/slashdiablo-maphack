#pragma once
#include "../Module.h"
#include <regex>

struct Control;

class Bnet : public Module {
	private:
		bool showLastGame;
		bool showLastPass;
		bool nextInstead;
		bool keepDesc;
		static unsigned int failToJoin;
		static std::string lastName;
		static std::string lastPass;
		static std::string lastDesc;
		static std::regex reg;

	public:

		Bnet() : Module("Bnet") {};

		void OnLoad();
		void OnUnload();
		void LoadConfig();

		void OnGameJoin(const string& name, const string& pass, int diff);
		void OnGameExit();

		static VOID __fastcall NextGamePatch(Control* box, BOOL (__stdcall *FunCallBack)(Control*, DWORD, DWORD));
		static VOID __fastcall NextPassPatch(Control* box, BOOL(__stdcall *FunCallBack)(Control*, DWORD, DWORD));
		static VOID __fastcall GameDescPatch(Control* box, BOOL(__stdcall *FunCallBack)(Control*, DWORD, DWORD));
		static void RemovePassPatch();
};

void FailToJoin_Interception();
void RemovePass_Interception();
