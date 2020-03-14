#include "Party.h"
#include "../../D2Ptrs.h"
#include "../../BH.h"
#include "../../D2Stubs.h"
#include "../../D2Helpers.h"

using namespace std;
using namespace Drawing;
Drawing::Hook* PartyHook;
Drawing::Hook* LootHook;

void Party::OnLoad() {
	BH::config->ReadToggle("Party Enabled", "None", true, Toggles["Enabled"]);
	BH::config->ReadToggle("Looting Enabled", "None", true, Toggles["LootEnabled"]);
	c = 0;

	PartyHook = new Drawing::Checkhook(Drawing::InGame, 100, 100, &Toggles["Enabled"].state, "Autoparty Enabled");
	PartyHook->SetActive(0);
	LootHook = new Drawing::Checkhook(Drawing::InGame, 240, 100, &Toggles["LootEnabled"].state, "Autoloot Enabled");
	LootHook->SetActive(0);
}

void Party::OnGameJoin() {
	min_party_id = 0xFFFF;
	min_party_id_valid = false;
}

void Party::OnUnload() {
	
}

void Party::OnLoop() {
	if(Toggles["Enabled"].state || Toggles["LootEnabled"].state)
		CheckParty();
	if(D2CLIENT_GetUIState(0x16) && PartyHook->IsActive() == 0) {
		PartyHook->SetBaseX(*p_D2CLIENT_PanelOffsetX + 20);
		PartyHook->SetActive(1);
	}
	else if(D2CLIENT_GetUIState(0x16) == 0 && PartyHook->IsActive())
		PartyHook->SetActive(0);

	BnetData* pData = (*p_D2LAUNCH_BnData);
	if(!pData)
		return;
	if(D2CLIENT_GetUIState(0x16) && LootHook->IsActive() == 0 && pData->nCharFlags & PLAYER_TYPE_HARDCORE) {
		LootHook->SetBaseX(*p_D2CLIENT_PanelOffsetX + 160);
		LootHook->SetActive(1);
	}
	else if(D2CLIENT_GetUIState(0x16) == 0 && LootHook->IsActive())
		LootHook->SetActive(0);
}

void Party::CheckParty() {
	if(c == 0) {
		bool valid = true;
		std::map<std::string, bool> CurrentParty;
		UnitAny* Me = *p_D2CLIENT_PlayerUnit;
		RosterUnit* MyRoster = FindPlayerRoster(Me->dwUnitId);
		BnetData* pData = (*p_D2LAUNCH_BnData);

		bool local_min_party_id_valid = true;
		bool master_party_exists = false;
		for(RosterUnit* Party = (*p_D2CLIENT_PlayerUnitList);Party;Party = Party->pNext) {
			if(!_stricmp(Party->szName, MyRoster->szName))
				continue;
			if(!Party->wLevel || !Party) {
				c++;
				return;
			}
			if ((Party->wPartyId == INVALID_PARTY_ID && Party->dwPartyFlags & PARTY_IN_PARTY) ||
				(Party->wPartyId != INVALID_PARTY_ID && Party->dwPartyFlags & PARTY_NOT_IN_PARTY)) {
				// Avoid crashing when multiple players in a game have auto-party enabled
				// (there seems to be a brief window in which the party data can be invalid)
				valid = false;
				continue;
			}
			if (pData && pData->nCharFlags & PLAYER_TYPE_HARDCORE) {
				CurrentParty[Party->szName] = true;
				if (Toggles["LootEnabled"].state) {
					string s(Party->szName);
					if (LootingPermission.find(s) == LootingPermission.end()) {
						BYTE PacketData[7] = {0x5d,1,1,0,0,0,0};
						*reinterpret_cast<int*>(PacketData + 3) = Party->dwUnitId;
						D2NET_SendPacket(7, 1, PacketData);
						LootingPermission[s] = true;
					}
				}
			}
			if (!min_party_id_valid) {
				if (Party->wPartyId != INVALID_PARTY_ID) {
					min_party_id = min(Party->wPartyId, min_party_id); // track the minimum party id
				} else {
					local_min_party_id_valid = false; // if any are invalid, don't try to fix anything yet
				}
			}
			else { // if (Party->wPartyId != INVALID_PARTY_ID && Party->wPartyId < min_party_id) { 
				// the original party must have disbanded
				if (Party->wPartyId == min_party_id) master_party_exists = true;
			}
			if ((Party->wPartyId == INVALID_PARTY_ID || Party->wPartyId != MyRoster->wPartyId) && Toggles["Enabled"].state) {
				PrintText(1, "Party->wPartyID=%hu, MyRoster->wPartyId=%hu, min_party_id=%hu, min_party_id_valid=%hu", 
						Party->wPartyId, MyRoster->wPartyId, min_party_id, (WORD)(min_party_id_valid));
				if(Party->dwPartyFlags & PARTY_INVITED_YOU) {
					if (min_party_id_valid) {
						if (Party->wPartyId == min_party_id) {
							PrintText(1, "Found the right party");
							D2CLIENT_ClickParty(Party, 2);
						}
					} else {
						PrintText(1, "PARTY_INVITED_YOU, clicking party");
						D2CLIENT_ClickParty(Party, 2);
					}
					c++;
					return;
				}
				if(Party->wPartyId == INVALID_PARTY_ID) {
					PrintText(1, "INVALID_PARTY_ID");
					if(Party->dwPartyFlags & PARTY_INVITED_BY_YOU) {
						PrintText(1, "PARTY_INVITED_BY_YOU");
						continue;
					}
					PrintText(1, "Clicking Party");
					D2CLIENT_ClickParty(Party, 2);
					c++;
					return;
				}
			}
		}
		if (min_party_id_valid && !master_party_exists) {
			PrintText(1, "Master party disbanded. Resetting min_party_id.");
			min_party_id_valid = false;
			min_party_id = 0xFFFF;
			c++;
			return;
		}
		min_party_id_valid = local_min_party_id_valid;
		if (Toggles["Enabled"].state && min_party_id_valid && MyRoster->wPartyId != min_party_id) {
			PrintText(1, "Not in the right party!");
			PrintText(1, "min_party_id=%hu, MyRoster->wPartyId=%hu", min_party_id, MyRoster->wPartyId);
			D2CLIENT_LeaveParty();
			c++;
			return;
		}
		if (valid) {
			for (auto it = LootingPermission.cbegin(); it != LootingPermission.cend(); ) {
				if (CurrentParty.find((*it).first) == CurrentParty.end()) {
					LootingPermission.erase(it++);
				} else {
					++it;
				}
			}
		}
	}
	c++;
	if(c>=25)
		c = 0;
}

void Party::OnKey(bool up, BYTE key, LPARAM lParam, bool* block)  {
	if(!D2CLIENT_GetPlayerUnit())
		return;

	if (D2CLIENT_GetUIState(UI_INVENTORY) || D2CLIENT_GetUIState(UI_STASH) || D2CLIENT_GetUIState(UI_CUBE)) {
		// Avoid toggling state when pressing number keys to drop/withdraw/deposit gold.
		// There is no UI state for the gold dialogs, so we have to disable whenever
		// stash/inventory are open.
		return;
	}

	for (map<string,Toggle>::iterator it = Toggles.begin(); it != Toggles.end(); it++) {
		if (key == (*it).second.toggle) {
			*block = true;
			if (up) {
				(*it).second.state = !(*it).second.state;
			}
			return;
		}
	}
}

void Party::OnGameExit() {
	LootingPermission.clear();
}
