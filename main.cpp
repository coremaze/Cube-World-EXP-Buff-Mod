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
        result = (int)( ( 1.0 - ( 1.0/( (level-1) * 0.05 + 1.0) ) ) * 1000.0 + 50.0);
    }
    return result;
}
unsigned int Creature__CalculateMaxExp_ptr = (unsigned int)&Creature__CalculateMaxExp;


unsigned int inlinefix1_jmp_back;
void __declspec(naked) __declspec(noinline) inlinefix1(){
    asm("call [_Creature__CalculateMaxExp_ptr]");
    asm("jmp [_inlinefix1_jmp_back]");
}

unsigned int inlinefix2_jmp_back;
void __declspec(naked) __declspec(noinline) inlinefix2(){
    asm("push ecx");
    asm("mov ecx, esi");
    asm("call [_Creature__CalculateMaxExp_ptr]");
    asm("pop ecx");
    asm("jmp [_inlinefix2_jmp_back]");
}

unsigned int inlinefix3_jmp_back;
void __declspec(naked) __declspec(noinline) inlinefix3(){
    asm("push ecx");
    asm("mov ecx, esi");
    asm("call [_Creature__CalculateMaxExp_ptr]");
    asm("pop ecx");
    asm("jmp [_inlinefix3_jmp_back]");
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


            //There's a function for leveling up which has been hit by function inlining for some reason.
            //We gotta clean it up and stop the inlining
            inlinefix1_jmp_back = base + 0x47B5E;
            WriteJMP((BYTE*)(base + 0x47B11), (BYTE*)&inlinefix1);

            inlinefix2_jmp_back = base + 0x47BA3;
            WriteJMP((BYTE*)(base + 0x47B7A), (BYTE*)&inlinefix2);

            inlinefix3_jmp_back = base + 0x47C10;
            WriteJMP((BYTE*)(base + 0x47BBF), (BYTE*)&inlinefix3);

            break;

    }
    return true;
}
