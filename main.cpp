#include <windows.h>
#include "cube.h"
#include <math.h>


unsigned int base;
//Just rewrite the function for doing this entirely.
signed int __thiscall Creature__CalculateMaxExp(cube::Creature *creature){
    int level = creature->level;
    int result;
    if (level <= 14){
        //nerf early game exp values
        result = (int)(500.0/(1+pow(2.0, -0.5*(level-8))));
    }
    else {
        //leave lategame exp values alone. This is the original calculation.
        result = (int)( ( 1.0 - ( 1.0/( (level-1) * 0.05 +1.0) ) ) * 1000.0 + 50.0);
    }
    return result;
}

void WriteJMP(BYTE* location, BYTE* newFunction){
	DWORD dwOldProtection;
	VirtualProtect(location, 5, PAGE_EXECUTE_READWRITE, &dwOldProtection);
    location[0] = 0xE9; //jmp
    *((DWORD*)(location + 1)) = (DWORD)(( (unsigned INT32)newFunction - (unsigned INT32)location ) - 5);
	VirtualProtect(location, 5, dwOldProtection, &dwOldProtection);
}

extern "C" __declspec(dllexport) bool APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            base = (unsigned int)GetModuleHandle(NULL);
            //Overwrite the original function to calculate max exp.
            WriteJMP((BYTE*)(base + 0x44D60), (BYTE*)&Creature__CalculateMaxExp);
            break;

    }
    return true;
}
