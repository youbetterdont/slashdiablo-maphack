#include "../Hook.h"
#include "../../D2Ptrs.h"
#include "UITab.h"
#include "../../BH.h"
#include "../Basic/Texthook/Texthook.h"
#include "../Basic/Framehook/Framehook.h"

using namespace Drawing;

std::list<UI*> UI::UIs;
std::list<UI*> UI::Minimized;

UI::UI(std::string name, unsigned int xSize, unsigned int ySize) {
	InitializeCriticalSection(&crit);
	SetXSize(xSize);
	SetYSize(ySize);
	SetName(name);
	string path = BH::path + "UI.ini";
	int x = 6;
	SetX(x);
	int minX = 6;
	SetMinimizedX(minX);

	int y = BH::GetGameHeight() - 115 - ySize;
	SetY(y);
	int minY = BH::GetGameHeight() - 115 - TITLE_BAR_HEIGHT;
	SetMinimizedY(minY);


	SetMinimized(true);
	Minimized.push_back(this);

	SetActive(false);
	zOrder = UIs.size();
	UIs.push_back(this);
}
UI::~UI() {
	Lock();
	while(Tabs.size() > 0) {
		delete (*Tabs.begin());
	}
		
	UIs.remove(this);
	if (IsMinimized())
		Minimized.remove(this);

	Unlock();
	DeleteCriticalSection(&crit);
}

void UI::SetX(unsigned int newX) {
	if (newX >= 0 && newX <= Hook::GetScreenWidth()) {
		Lock();
		x = newX;
		Unlock();
	}
}

void UI::SetY(unsigned int newY) {
	if (newY >= 0 && newY <= BH::GetGameHeight()) {
		Lock();
		y = newY;
		Unlock();
	}
}

void UI::SetXSize(unsigned int newXSize) {
	//if (newXSize >= 0 && newXSize <= (Hook::GetScreenWidth() - GetX())) {
		Lock();
		xSize = newXSize;
		Unlock();
	//}
}

void UI::SetYSize(unsigned int newYSize) {
	//if (newYSize >= 0 && newYSize <= (BH::GetGameHeight() - GetY())) {
		Lock();
		ySize = newYSize;
		Unlock();
	//}
}

void UI::SetMinimizedX(unsigned int newX) {
	if (newX >= 0 && newX <= Hook::GetScreenWidth()) {
		Lock();
		minimizedX = newX;
		Unlock();
	}
}

void UI::SetMinimizedY(unsigned int newY) {
	if (newY >= 0 && newY <= BH::GetGameHeight()) {
		Lock();
		minimizedY = newY;
		Unlock();
	}
}

void UI::OnDraw() {
	if (IsMinimized()) {
		if (GetMinimizedY() != BH::GetGameHeight() - 115 - TITLE_BAR_HEIGHT)
		{
			int minY = BH::GetGameHeight() - 115 - TITLE_BAR_HEIGHT;
			SetMinimizedY(minY);
		}

		int n = 0;
		for (list<UI*>::iterator it = Minimized.begin(); it != Minimized.end(); it++, n++)
			if ((*it) == this)
				break;
		int yPos = GetMinimizedY();
		int xSize = Texthook::GetTextSize(GetName(), 0).x + 8;
		int inPos = InPos((*p_D2CLIENT_MouseX), (*p_D2CLIENT_MouseY), GetMinimizedX(), yPos, xSize, TITLE_BAR_HEIGHT);
		Framehook::Draw(GetMinimizedX(), yPos, xSize, TITLE_BAR_HEIGHT, 0, BTOneHalf);
		Texthook::Draw(GetMinimizedX() + 4, yPos + 3, false, 0, (inPos?Silver:White), GetName());
	} else {
		if (GetY() != BH::GetGameHeight() - 115 - GetYSize())
		{
			int y = BH::GetGameHeight() - 115 - GetYSize();
			SetY(y);
		}

		Framehook::Draw(GetX(), GetY(), GetXSize(), GetYSize(), 0, (IsActive()?BTNormal:BTOneHalf));
		Framehook::Draw(GetX(), GetY(), GetXSize(), TITLE_BAR_HEIGHT, 0, BTNormal);
		Texthook::Draw(GetX() + 4, GetY () + 3, false, 0, InTitle((*p_D2CLIENT_MouseX), (*p_D2CLIENT_MouseY))?Silver:White, GetName());
		for (list<UITab*>::iterator it = Tabs.begin(); it != Tabs.end(); it++)
			(*it)->OnDraw();
	}
}

void UI::SetDragged(bool state) { 
	Lock(); 
	dragged = false;
	Unlock(); 
}

void UI::SetMinimized(bool newState) { 
	if (newState == minimized) 
		return; 
	Lock();  
	if (newState)
		Minimized.push_back(this); 
	else
		Minimized.remove(this); 
	minimized = newState; 
	//WritePrivateProfileString(name.c_str(), "Minimized", to_string<bool>(newState).c_str(), string(BH::path + "UI.ini").c_str());
	Unlock(); 
};

bool UI::OnLeftClick(bool up, unsigned int mouseX, unsigned int mouseY) {
	if (IsMinimized()) {
		int n = 0;
		for (list<UI*>::iterator it = Minimized.begin(); it != Minimized.end(); it++, n++)
			if ((*it) == this)
				break;
		int yPos = GetMinimizedY() - (n * (TITLE_BAR_HEIGHT + 4));
		int xSize = Texthook::GetTextSize(GetName(), 0).x + 8;
		int inPos = InPos((*p_D2CLIENT_MouseX), (*p_D2CLIENT_MouseY), GetMinimizedX(), yPos, xSize, TITLE_BAR_HEIGHT);
		if (inPos /*&& GetAsyncKeyState(VK_CONTROL)*/) 
		{
			if(GetAsyncKeyState(VK_CONTROL))
			{
				if (up) {
					SetMinimized(false);
					Sort(this);
				}
				return true;
			}
			else
			{
				if(!up)
					PrintText(1, "Hold CTRL and click Settings to open" );
			}
		}
	}
	if (InTitle(mouseX, mouseY) && !IsMinimized()) {
		if (!up) 
		{
			SetDragged(true);
			dragX = mouseX - GetX();
			dragY = mouseY - GetY();
			startX = mouseX;
			startY = mouseY;
		} 
		else
		{
			SetDragged(false);
			if( startX == mouseX && startY == mouseY && GetAsyncKeyState(VK_CONTROL) )
			{
				PrintText(1, "Right Click to Close" );
			}
		}
		SetActive(true);
		Sort(this);
		return true;
	} else if (InWindow(mouseX, mouseY) && !IsMinimized()) {
		SetActive(true);
		Sort(this);
		if (up) {
			for (list<UITab*>::iterator it = Tabs.begin(); it != Tabs.end(); it++) {
				if ((*it)->IsHovering(mouseX, mouseY)) {
					SetCurrentTab(*it);
					return true;
				}
			}
		}
		return true;
	}
	SetActive(false);
	SetDragged(false);
	return false;
}

bool UI::OnRightClick(bool up, unsigned int mouseX, unsigned int mouseY) {
	if (InTitle(mouseX, mouseY) && !IsMinimized()) {
		if (up) 
			SetMinimized(true);
		return true;
	}
	return false;
}

void UI::Sort(UI* zero) {
	int zOrder = 1;
	for (list<UI*>::iterator it = UIs.begin(); it != UIs.end(); it++, zOrder++) {
		if ((*it) == zero) {
			(*it)->SetZOrder(0);
			zOrder--;
		} else {
			(*it)->SetZOrder(zOrder);
		}
	}
}

/* UI Helper functions 
 *	Static functions to aid in sending events to UIs.
 */
 
bool ZSortClick (UI* one, UI* two) {
	return one->GetZOrder() < two->GetZOrder();
}
bool ZSortDraw (UI* one, UI* two) {
	return one->GetZOrder() > two->GetZOrder();
}

void UI::Draw() {
	UIs.sort(ZSortDraw);
	for (list<UI*>::iterator it = UIs.begin(); it!=UIs.end(); ++it) {
			(*it)->Lock();
			(*it)->OnDraw();
			(*it)->Unlock();
	}
}	

bool UI::LeftClick(bool up, unsigned int mouseX, unsigned int mouseY) {
	UIs.sort(ZSortClick);
	for (list<UI*>::iterator it = UIs.begin(); it!=UIs.end(); ++it) {
		(*it)->Lock();
		if ((*it)->OnLeftClick(up, mouseX, mouseY)) {
			(*it)->Unlock();
			return true;
		}
		(*it)->Unlock();
	}
	return false;
}

bool UI::RightClick(bool up, unsigned int mouseX, unsigned int mouseY) {
	UIs.sort(ZSortClick);
	for (list<UI*>::iterator it = UIs.begin(); it!=UIs.end(); ++it) {
		(*it)->Lock();
		if ((*it)->OnRightClick(up, mouseX, mouseY)) {
			(*it)->Unlock();
			return true;
		}
		(*it)->Unlock();
	}
	return false;
}
