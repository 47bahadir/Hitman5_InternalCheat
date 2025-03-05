#include "pch.h"
#include "hook.h"

DWORD ammoBackAddy = 0;
DWORD healthBackAddy = 0;
DWORD damageBackAddy = 0;
float damageMultiplier = 1.0f;

void __declspec(naked) hook::ammoFunc()		//Change Ammo to 9999 And Lock
{
	__asm
	{
		push ecx
		mov ecx, [esi + 0x5E6]
		cmp ecx, 2
		je code
		mov ax, 0x3E7
		mov[esi + 0x00000316], ax
		pop ecx
		jmp ammoBackAddy

		code :
		mov[esi + 0x00000316], ax
		pop ecx
		jmp ammoBackAddy

	}
}


void __declspec(naked) hook::healthFunc()		//Change Health to 100 And Lock
{
	__asm
	{
		push 100
		fild dword ptr[esp]
		fstp dword ptr[esi + 0x00000218]
		add esp, 4
		jmp healthBackAddy
	}
}

void __declspec(naked) hook::damageFunc()			//Add Damage Multiplier
{
	__asm
	{
		xorps xmm4, xmm4
		movss xmm4, dword ptr[ebp + 0x08]
		mulss xmm4, [damageMultiplier]
		movss dword ptr[ebp + 0x08], xmm4
		fld dword ptr[ebp + 0x08]
		fadd dword ptr[ecx + 0x000000A8]
		jmp damageBackAddy
	}
}