#pragma once
#include <vector>


namespace mem
{
	uintptr_t findDMMAddy(uintptr_t addres, std::vector<unsigned int> offsets);

	void writePatch(BYTE* dst, BYTE* src, unsigned int size);

	void nopPatch(BYTE* dst, unsigned int size);

	bool myHook(void* dst, void* ourAddy, int len);

}