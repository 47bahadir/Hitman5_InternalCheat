// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <intrin.h>


DWORD ammoBackAddy;
void __declspec(naked) ammoFunc()		//Change Ammo to 9999 And Lock
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
void __declspec(naked) healthFunc()		//Change Health to 100 And Lock
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
    uintptr_t modBaseAddy = (uintptr_t)GetModuleHandle(NULL);

    if (modBaseAddy)
    {
		//ammo change address : HMA.exe+2F5C80 -> 66 89 86 16 03 00 00 == mov [esi+00000316],ax
        int ammoLen = 7;
        void* ammoDst = (void*)(modBaseAddy + 0x2F5C80);
        ammoBackAddy = (DWORD)ammoDst + ammoLen;
        bool bAmmo = true;

		//health change address : HMA.exe+4082E4 -> D9 9E 18 02 00 00 == fstp dword ptr [esi+00000218]
		int healthLen = 6;
		void* healthDst = (void*)(modBaseAddy + 0x4082E4);
		healthBackAddy = (DWORD)healthDst + healthLen;
		bool bHealth = true;

		bool bSpread = true, bRecoil = true, bNoStopShoot = true;

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

			
			if (GetAsyncKeyState(VK_NUMPAD3) & 1)
			{
				bRecoil = !bRecoil;
				if (!bRecoil)
				{
					//HMA.exe+55ECFD - 90 90                 - nop nop
					mem::nopPatch((BYTE*)(modBaseAddy + 0x55ECFD), 2);

					//HMA.exe+55ED0B - 90 90                 - nop nop
					mem::nopPatch((BYTE*)(modBaseAddy + 0x55ED0B), 2);
				}
				else
				{
					//HMA.exe+55ECFD - 7A 20                 - jp HMA.exe+55ED1F
					mem::writePatch((BYTE*)(modBaseAddy + 0x55ECFD), (BYTE*)"\x7A\x20", 2);

					//HMA.exe+55ED0B - 7A 12                 - jp HMA.exe+55ED1F
					mem::writePatch((BYTE*)(modBaseAddy + 0x55ED0B), (BYTE*)"\x7A\x12", 2);
				}
			}


			if (GetAsyncKeyState(VK_NUMPAD4) & 1)
			{
				bSpread = !bSpread;
				if (!bSpread)
				{
					//HMA.exe+5ECA10 - 90 90 90 90 90 90        - nop * 6
					mem::nopPatch((BYTE*)(modBaseAddy + 0x5ECA10), 6);
				}
				else
				{
					//HMA.exe+5ECA10 - D9 9F 20 04 00 00        - fstp dword ptr [edi+00000420]
					mem::writePatch((BYTE*)(modBaseAddy + 0x5ECA10), (BYTE*)"\xD9\x9F\x20\x04\x00\x00", 6);
				}
			}


			if (GetAsyncKeyState(VK_NUMPAD5) & 1)
			{
				bNoStopShoot = !bNoStopShoot;
				if (!bNoStopShoot)
				{
					//HMA.exe+5ECE7B - 90 90 90 90 90 90 90     - nop * 7
					mem::nopPatch((BYTE*)(modBaseAddy + 0x5ECE7B), 7);
				}
				else
				{
					//HMA.exe+5ECE7B - 66 FF 87 04 04 00 00     - inc word ptr [edi+00000404]
					mem::writePatch((BYTE*)(modBaseAddy + 0x5ECE7B), (BYTE*)"\x66\xFF\x87\x04\x04\x00\x00", 7);
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
	{
		HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)hackThread, hModule, 0, NULL);
		if (hThread)
		{
			CloseHandle(hThread);
		}
	}
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

