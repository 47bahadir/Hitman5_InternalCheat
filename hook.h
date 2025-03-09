#pragma once

extern DWORD ammoBackAddy;
extern DWORD healthBackAddy;
extern DWORD damageBackAddy;
extern float damageMultiplier;
extern DWORD scoreBackAddy;

namespace hook
{
	void ammoFunc();
	void healthFunc();
	void damageFunc();
	void scoreFunc();
}

