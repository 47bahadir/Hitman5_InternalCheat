// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <intrin.h>


DWORD ammoBackAddy;
void __declspec(naked) ammoFunc()
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

		code:
		mov[esi + 0x00000316], ax
		pop ecx
		jmp ammoBackAddy

    }
}


DWORD healthBackAddy;
void __declspec(naked) healthFunc()
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


DWORD WINAPI hackThread(HMODULE hModule)
{
    //get module base address,GetModuleHandle with null return address value
    uintptr_t baseAddress = (uintptr_t)GetModuleHandle(NULL);

    if (baseAddress)
    {
		//ammo change address : HMA.exe+2F5C80 -> 66 89 86 16 03 00 00 == mov [esi+00000316],ax
        int ammoLen = 7;
        void* ammoDst = (void*)(baseAddress + 0x2F5C80);
        ammoBackAddy = (DWORD)ammoDst + ammoLen;
        bool bAmmo = true;

		//health change address : HMA.exe+4082E4 -> D9 9E 18 02 00 00 == fstp dword ptr [esi+00000218]
		int healthLen = 6;
		void* healthDst = (void*)(baseAddress + 0x4082E4);
		healthBackAddy = (DWORD)healthDst + healthLen;
		bool bHealth = true;

        while (true)
        {

			if (GetAsyncKeyState(VK_NUMPAD1) & 1)
			{
				bAmmo = !bAmmo;
				if (!bAmmo)
				{
					mem::myHook(ammoDst, ammoFunc, ammoLen);
				}
				else
				{
					mem::writePatch((BYTE*)ammoDst, (BYTE*)"\x66\x89\x86\x16\x03\x00\x00", ammoLen);
				}
			}


			if (GetAsyncKeyState(VK_NUMPAD2) & 1)
			{
				bHealth = !bHealth;
				if (!bHealth)
				{
					mem::myHook(healthDst, healthFunc, healthLen);
				}
				else
				{
					mem::writePatch((BYTE*)healthDst, (BYTE*)"\xD9\x9E\x18\x02\x00\x00", healthLen);
				}
			}


			if (GetAsyncKeyState(VK_END) & 1)
			{
                mem::writePatch((BYTE*)ammoDst, (BYTE*)"\x66\x89\x86\x16\x03\x00\x00", ammoLen);
				mem::writePatch((BYTE*)healthDst, (BYTE*)"\xD9\x9E\x18\x02\x00\x00", healthLen);
                break;

			}
            Sleep(10);
        }
    }

    FreeLibraryAndExitThread(hModule, 0);
    return 0;
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CloseHandle(CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)hackThread, hModule, 0, NULL));
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

