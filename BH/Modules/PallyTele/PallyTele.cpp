/* Maphack Module
 *
 */
#include "../../D2Ptrs.h"
#include "PallyTele.h"
#include "../../BH.h"
#include "../../Constants.h"
using namespace Drawing;

const int ConcSkillId = 113;



void PallyTele::OnLoad() {
	IsPaladin = false;

	if(D2CLIENT_GetPlayerUnit())
		GetClass();
	
}

void PallyTele::OnUnload() {

}

void PallyTele::OnGameJoin(const string& name, const string& pass, int diff) 
{
	GetClass();
}

void PallyTele::OnRightClick(bool up, int x, int y, bool* block) 
{
	if(!D2CLIENT_GetPlayerUnit())
		return;

	if(!IsPaladin)
		return;

	if(!GetSkill(0x36))
		return;

	int rightSkillId = D2CLIENT_GetPlayerUnit()->pInfo->pRightSkill->pSkillInfo->wSkillId;

	//We just pressed right click AND our set skill is Concentration
	if(!up && rightSkillId == ConcSkillId)
	{
		if(SetSkill(0x36, false))
			PreviousSkill = ConcSkillId;
	}
	//If our previous skill was concentration, current skill is tele AND we let up on right click, switch back to concentration.
	else if(up && PreviousSkill == ConcSkillId && rightSkillId == 0x36)
	{
		SetSkill(PreviousSkill, false);
		PreviousSkill = 0;
	}
}


bool PallyTele::SetSkill(WORD wSkillId, bool Left)
{
	if(!D2CLIENT_GetPlayerUnit())
		return 0;

	BYTE PutSkill[9] = {0x3c,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF};
	if(GetSkill(wSkillId))
	{
		*(WORD*)&PutSkill[1] = wSkillId;
		if(Left)
			*(BYTE*)&PutSkill[4] = 0x80;
		D2NET_SendPacket(9,0,PutSkill);
	}
	else return 0;

	return 1;
}

bool PallyTele::GetSkill(WORD wSkillId)
{
	if(!D2CLIENT_GetPlayerUnit()) 
		return 0;

	for(Skill* pSkill = D2CLIENT_GetPlayerUnit()->pInfo->pFirstSkill; pSkill; pSkill = pSkill->pNextSkill)
		if(pSkill->pSkillInfo->wSkillId == wSkillId)
			return 1;//D2COMMON_GetSkillLevel(D2CLIENT_GetPlayerUnit(), pSkill, TRUE);
	
	return 0;
}

int PallyTele::GetClass()
{
	UnitAny* player = D2CLIENT_GetPlayerUnit();
	int classID = player->dwTxtFileNo;
	IsPaladin = (classID == CLASS_PAL);

	return classID;
}