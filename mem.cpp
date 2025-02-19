#include "pch.h"
#include "mem.h"


uintptr_t mem::findDMMAddy(uintptr_t addres, std::vector<unsigned int> offsets)
{
	//uintptr_t retAddres = addres;
	for (int i = 0; i < offsets.size(); i++)
	{
		addres = *(uintptr_t*)addres;
		addres += offsets[i];
	}
	return addres;
}

void mem::writePatch(BYTE* dst, BYTE* src, unsigned int size)
{
	DWORD oldPertec = 0;
	VirtualProtect((BYTE*)dst, size, PAGE_EXECUTE_READWRITE, &oldPertec);
	memcpy((BYTE*)dst, (BYTE*)src, size);
	VirtualProtect((BYTE*)dst, size, oldPertec, &oldPertec);
}

void mem::nopPatch(BYTE* dst, unsigned int size)
{
	DWORD oldPertec = 0;
	VirtualProtect((BYTE*)dst, size, PAGE_EXECUTE_READWRITE, &oldPertec);
	memset((BYTE*)dst, 0x90,size);
	VirtualProtect((BYTE*)dst, size, oldPertec, &oldPertec);
}

//dst is target hook address,ourAddy is new function address,len is hook byte length
bool mem::myHook(void* dst, void* ourAddy, int len)
{
	if (len < 5)
	{
		return false;
	}

	//first thing is change dst address permission
	DWORD oldPertect;
	VirtualProtect(dst, len, PAGE_EXECUTE_READWRITE, &oldPertect);

	memset(dst, 0x90, len);

	DWORD relativeAddy = ((DWORD)ourAddy - (DWORD)dst) - 5;

	*(BYTE*)dst = 0xE9;
	*(DWORD*)((DWORD)dst + 1) = relativeAddy;

	VirtualProtect(dst, len, oldPertect, &oldPertect);

	return true;
}