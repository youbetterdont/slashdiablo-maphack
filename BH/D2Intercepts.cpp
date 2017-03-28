#include "D2Handlers.h"
#include "D2Ptrs.h"

VOID __declspec(naked) GameDraw_Interception()
{
	__asm
	{
		call GameDraw;

		POP ESI
		POP EBX
		POP ECX
		RETN 4
	}
}

void __declspec(naked) GameAutomapDraw_Interception()
{
   __asm
   {
      push eax;
      call GameAutomapDraw;
      pop eax;
      pop edi;
      pop esi;
      ret;
   }
} 

VOID __declspec(naked) OOGDraw_Interception()
{
	__asm
	{
		PUSHAD
		CALL OOGDraw
		POPAD
		MOV ESI,DWORD PTR DS:[ESI+0x3C]
		TEST ESI, ESI
		RETN
	}
}

void __declspec(naked) GameLoop_Interception()
{
	__asm
	{
		pushad;
		call GameLoop;
		popad;
 
		pop eax;
		sub esp, 0x20;
		mov [esp + 0xC], ecx;
		push eax;
		ret;
	}
}

VOID __declspec(naked) GamePacketRecv_Interception() {
	__asm
	{
		pop ebp;
		pushad;

		call GamePacketRecv;
		test eax, eax;

		popad;
		jnz OldCode;

		mov edx, 0;

OldCode:
		call D2NET_ReceivePacket_I;

		push ebp;
		ret;
	}
}

VOID __declspec(naked) ChatPacketRecv_Interception()
{
   __asm
   {
      pushad;
      call ChatPacketRecv;
      TEST EAX,EAX;

      popad;
      jnz oldCall;

        MOV EAX,0;
      MOV DWORD PTR DS:[EBX+0x6FF3ED58],1
      ret;
oldCall:
        CALL eax;
        MOV DWORD PTR DS:[EBX+0x6FF3ED58],1
        ret;

   }
}

VOID __declspec(naked) RealmPacketRecv_Interception()
{
	__asm
	{
		LEA ECX,DWORD PTR SS:[ESP+4]
		PUSHAD
		CALL RealmPacketRecv
		CMP EAX, 0
		POPAD
		JE Block
		CALL EAX
Block:
		RETN
	}
}

void __declspec(naked) GameInput_Interception(void)
{
	__asm {
		pushad
		mov ecx, ebx
		call GameInput
		cmp eax, -1
		popad
		je BlockIt
		call D2CLIENT_InputCall_I
		ret

BlockIt:
		xor eax,eax
		ret
	}
}

void __declspec(naked) ChannelInput_Interception(void)
{
	__asm
	{
		push ecx
		mov ecx, esi

		call ChannelInput

		test eax, eax
		pop ecx

		jz SkipInput
		call D2MULTI_ChannelInput_I

SkipInput:
		ret
	}
}

void __declspec(naked) ChannelWhisper_Interception(void)
{
	__asm
	{
		push ecx
		push edx
		mov ecx, edi
		mov edx, ebx

		call ChatHandler

		test eax, eax
		pop edx
		pop ecx

		jnz SkipWhisper
		jmp D2MULTI_ChannelWhisper_I

SkipWhisper:
		ret 4
	}
}

void __declspec(naked) ChannelChat_Interception(void)
{
	__asm
	{
		push ecx
		push edx
		mov ecx, dword ptr ss:[esp+0xC]
		mov edx, dword ptr ss:[esp+0x10]

		call ChatHandler

		test eax, eax
		pop edx
		pop ecx

		jnz SkipChat
		sub esp, 0x408
		jmp D2MULTI_ChannelChat_I

SkipChat:
		ret 8
	}
}

void __declspec(naked) ChannelEmote_Interception(void)
{
	__asm
	{
		push ecx
		push edx
		mov ecx, dword ptr ss:[esp+0xC]
		mov edx, dword ptr ss:[esp+0x10]

		call ChatHandler

		test eax, eax
		pop edx
		pop ecx

		jnz SkipChat
		sub esp, 0x4F8
		jmp D2MULTI_ChannelEmote_I

SkipChat:
		ret 8
	}
}

void __declspec(naked) PositionMenuTab_Interception(void) {
	__asm
	{
		; Check that the mode is set to HD, otherwise run 640 mode code
		cmp eax, 03

		pushad
		mov eax, p_D2CLIENT_TabXOffset
		jne PositionMenuTab_Mode640

		; HD positions
		; Position X offset
		mov dword ptr ds:[eax], 352
		; Position Y offset
		mov dword ptr ds:[eax+4], -110

		jmp PositionMenuTab_RepositionEnd

PositionMenuTab_Mode640:
		mov dword ptr ds:[eax], 0
		mov dword ptr ds:[eax+4], 0

PositionMenuTab_RepositionEnd:
		popad
		ret
	}
}

void DisplayExpansionUI_Interception(void) {
	__asm
	{
		; Check that we are in HD mode,
		cmp eax, 01
		je DisplayExpansionUI_Mode640

		mov eax, 0x6FAD926C
		jmp DisplayExpansionUI_DisplayExpansionUIInterceptEnd

DisplayExpansionUI_Mode640:
		mov eax, 0x6FAD9294

DisplayExpansionUI_DisplayExpansionUIInterceptEnd:
		pop ebp
		jmp eax
	}
}

void DisplayExpansionUILeftTabTopRight_Interception(void) {
	__asm
	{
		mov eax, ds:[0x6FA80000+0x11260]
		cmp eax, 03
		pop ebp
		jne DisplayExpansionUILeftTabTopRight_OriginalValues

		; New Values

		; Unknown
		push 5

		; Darkness color filter (0 - 255) higher=less dark, lower=darker
		push 255

		; Shift position Y, height is 40
		push 113	; The height difference is 100, so add 50 to original

		; Shift position X, width is 144
		push 528	; Subtract 144 from half the width

		jmp DisplayExpansionUILeftTabTopRight_End
		
DisplayExpansionUILeftTabTopRight_OriginalValues:
		push 05		; 05
		push -01	; 255
		push 63		; 3F
		push 256	; 0x00000100

DisplayExpansionUILeftTabTopRight_End:
		mov eax, 0x6FAD721A
		jmp eax
	}
}

void DisplayExpansionUILeftTabMiddleTop_Interception(void) {
	__asm
	{
		mov eax, ds:[0x6FA80000+0x11260]
		cmp eax, 03
		pop ebp
		jne DisplayExpansionUILeftTabMiddleTop_OriginalValues

		; New Values
		push 0x05
		push -01

		; Shift position X, width is 144
		push 0x000001E4
		jmp DisplayExpansionUILeftTabMiddleTop_End
		
DisplayExpansionUILeftTabMiddleTop_OriginalValues:
		push 0x05	; 05
		push -01	; 255
		push 484	; 0x000001E4

DisplayExpansionUILeftTabMiddleTop_End:
		mov eax, 0x6FAB0000+0x27236
		jmp eax
	}
}

void DisplayExpansionUILeftTabBottomLeft_Interception(void) {
	__asm
	{
		mov eax, ds:[0x6FA80000+0x11260]
		cmp eax, 03
		pop ebp
		jne DisplayExpansionUILeftTabBottomLeft_OriginalValues

		; New Values
		push 5
		push -01
		push 553
		jmp DisplayExpansionUILeftTabBottomLeft_End
		
DisplayExpansionUILeftTabBottomLeft_OriginalValues:
		push 0x05
		push -01
		push 553

DisplayExpansionUILeftTabBottomLeft_End:
		mov eax, 0x6FAB0000+0x27253
		jmp eax
	}
}

void DisplayExpansionUILeftTabBottomRight_Interception(void) {
	__asm
	{
		mov eax, ds:[0x6FA80000+0x11260]
		cmp eax, 03
		pop ebp
		jne DisplayExpansionUILeftTabBottomRight_OriginalValues

		; New Values
		push 0x05
		push -01
		push 0x00000229
		push 0x00000100
		jmp DisplayExpansionUILeftTabBottomRight_End
		
DisplayExpansionUILeftTabBottomRight_OriginalValues:
		push 0x05
		push -01
		push 0x00000229
		push 0x00000100

DisplayExpansionUILeftTabBottomRight_End:
		mov eax, 0x6FAB0000+0x27275
		jmp eax
	}
}
