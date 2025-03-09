// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "speech.h"
#include <intrin.h>
#include "hook.h"


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

		//HMA.exe + 5FAD63 - D9 45 08          == fld dword ptr [ebp+08]
		//HMA.exe + 5FAD66 - D8 81 A8 00 00 00 == fadd dword ptr[ecx + 000000A8]
		int damageLen = 9;
		void* damageDst = (void*)(modBaseAddy + 0x5FAD63);
		damageBackAddy = (DWORD)damageDst + damageLen;
		bool bDamage = true;


		//HMA.exe + 3BD249 - 8B 46 14	== mov eax, [esi + 14]
		//HMA.exe + 3BD24C - 8B 16		== mov edx, [esi]
		int scoreLen = 5;
		void* scoreDst = (void*)(modBaseAddy + 0x3BD249);
		scoreBackAddy = (DWORD)scoreDst + scoreLen;
		bool bScore = true;

		bool bSpread = true, bRecoil = true, bNoStopShoot = true;

		SpeechManager::Speak(L"Hack on");
        while (true)
        {
			//Damage Multiplier -> Ctrl + Number
			if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
			{
				for (int i = 0; i <= 9; i++)
				{
					if (GetAsyncKeyState('0' + i) & 1)		//'0' ASCII is 48
					{
						damageMultiplier = static_cast<float>(i);
						
						SpeechManager::Speak(L"Damage " + std::to_wstring(i));
						break;
					}
				}
			}

			//Unlimited bullets
			if (GetAsyncKeyState(VK_NUMPAD1) & 1)
			{
				bAmmo = !bAmmo;
				if (!bAmmo)
				{
					mem::myHook(ammoDst, hook::ammoFunc, ammoLen);
					SpeechManager::Speak(L"Unlimited bullets on");
				}
				else
				{
					mem::writePatch((BYTE*)ammoDst, (BYTE*)"\x66\x89\x86\x16\x03\x00\x00", ammoLen);
					SpeechManager::Speak(L"Unlimited bullets off");
				}
			}

			//Unlimited health
			if (GetAsyncKeyState(VK_NUMPAD2) & 1)
			{
				bHealth = !bHealth;
				if (!bHealth)
				{
					mem::myHook(healthDst, hook::healthFunc, healthLen);
					SpeechManager::Speak(L"Unlimited health on");
				}
				else
				{
					mem::writePatch((BYTE*)healthDst, (BYTE*)"\xD9\x9E\x18\x02\x00\x00", healthLen);
					SpeechManager::Speak(L"Unlimited health off");
				}
			}

			//No recoil
			if (GetAsyncKeyState(VK_NUMPAD3) & 1)
			{
				bRecoil = !bRecoil;
				if (!bRecoil)
				{
					//HMA.exe+55ECFD - 90 90                 - nop nop
					mem::nopPatch((BYTE*)(modBaseAddy + 0x55ECFD), 2);

					//HMA.exe+55ED0B - 90 90                 - nop nop
					mem::nopPatch((BYTE*)(modBaseAddy + 0x55ED0B), 2);

					SpeechManager::Speak(L"No recoil on");
				}
				else
				{
					//HMA.exe+55ECFD - 7A 20                 - jp HMA.exe+55ED1F
					mem::writePatch((BYTE*)(modBaseAddy + 0x55ECFD), (BYTE*)"\x7A\x20", 2);

					//HMA.exe+55ED0B - 7A 12                 - jp HMA.exe+55ED1F
					mem::writePatch((BYTE*)(modBaseAddy + 0x55ED0B), (BYTE*)"\x7A\x12", 2);

					SpeechManager::Speak(L"No recoil off");
				}
			}

			//No spread
			if (GetAsyncKeyState(VK_NUMPAD4) & 1)
			{
				bSpread = !bSpread;
				if (!bSpread)
				{
					//HMA.exe+5ECA10 - 90 90 90 90 90 90        - nop * 6
					mem::nopPatch((BYTE*)(modBaseAddy + 0x5ECA10), 6);

					SpeechManager::Speak(L"No spread on");
				}
				else
				{
					//HMA.exe+5ECA10 - D9 9F 20 04 00 00        - fstp dword ptr [edi+00000420]
					mem::writePatch((BYTE*)(modBaseAddy + 0x5ECA10), (BYTE*)"\xD9\x9F\x20\x04\x00\x00", 6);

					SpeechManager::Speak(L"No spread off");
				}
			}

			//No pause shooting
			if (GetAsyncKeyState(VK_NUMPAD5) & 1)
			{
				bNoStopShoot = !bNoStopShoot;
				if (!bNoStopShoot)
				{
					//HMA.exe+5ECE7B - 90 90 90 90 90 90 90     - nop * 7
					mem::nopPatch((BYTE*)(modBaseAddy + 0x5ECE7B), 7);

					SpeechManager::Speak(L"No pause shooting on");
				}
				else
				{
					//HMA.exe+5ECE7B - 66 FF 87 04 04 00 00     - inc word ptr [edi+00000404]
					mem::writePatch((BYTE*)(modBaseAddy + 0x5ECE7B), (BYTE*)"\x66\xFF\x87\x04\x04\x00\x00", 7);

					SpeechManager::Speak(L"No pause shooting off");
				}
			}

			//Damage Changes
			if (GetAsyncKeyState(VK_NUMPAD6) & 1)
			{
				bDamage = !bDamage;
				if (!bDamage)
				{
					mem::myHook(damageDst, hook::damageFunc, damageLen);

					SpeechManager::Speak(L"Damage Changes on");
				}
				else
				{
					mem::writePatch((BYTE*)damageDst, (BYTE*)"\xD9\x45\x08\xD8\x81\xA8\x00\x00\x00", damageLen);

					SpeechManager::Speak(L"Damage Changes off");
				}
			}

			//Score Changes
			if (GetAsyncKeyState(VK_NUMPAD7) & 1)
			{
				bScore = !bScore;
				if (!bScore)
				{
					mem::myHook(scoreDst, hook::scoreFunc, scoreLen);

					SpeechManager::Speak(L"Score only rise on");
				}
				else
				{
					mem::writePatch((BYTE*)scoreDst, (BYTE*)"\x8B\x46\x14\x8B\x16", scoreLen);

					SpeechManager::Speak(L"Score only rise off");
				}
			}


			if (GetAsyncKeyState(VK_END) & 1)
			{
				SpeechManager::Speak(L"Hack off");
				Sleep(1000);

                mem::writePatch((BYTE*)ammoDst, (BYTE*)"\x66\x89\x86\x16\x03\x00\x00", ammoLen);
				mem::writePatch((BYTE*)healthDst, (BYTE*)"\xD9\x9E\x18\x02\x00\x00", healthLen);
				mem::writePatch((BYTE*)damageDst, (BYTE*)"\xD9\x45\x08\xD8\x81\xA8\x00\x00\x00", damageLen);
				//NoStopShoot
				mem::writePatch((BYTE*)(modBaseAddy + 0x5ECE7B), (BYTE*)"\x66\xFF\x87\x04\x04\x00\x00", 7);
				//NoSpread
				mem::writePatch((BYTE*)(modBaseAddy + 0x5ECA10), (BYTE*)"\xD9\x9F\x20\x04\x00\x00", 6);
				//NoRecoil
				mem::writePatch((BYTE*)(modBaseAddy + 0x55ECFD), (BYTE*)"\x7A\x20", 2);
				mem::writePatch((BYTE*)(modBaseAddy + 0x55ED0B), (BYTE*)"\x7A\x12", 2);
                break;

			}
            Sleep(10);
        }
    }

	SpeechManager::Cleanup();
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

