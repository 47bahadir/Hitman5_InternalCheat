#pragma once

extern DWORD ammoBackAddy;
extern DWORD healthBackAddy;
extern DWORD damageBackAddy;
extern float damageMultiplier;

namespace hook
{
	void ammoFunc();
	void healthFunc();
	void damageFunc();
}

