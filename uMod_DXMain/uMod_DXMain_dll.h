/*
This file is part of Universal Modding Engine.


Universal Modding Engine is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Universal Modding Engine is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Universal Modding Engine.  If not, see <http://www.gnu.org/licenses/>.
*/



#ifndef uMod_DXMAIN_DLL_H_
#define uMod_DXMAIN_DLL_H_

#ifdef DEF_USE_DX9
#include "../uMod_DX9/uMod_DX9_dll.h"
#endif

#ifdef DEF_USE_DX10
#include "../uMod_DX10/uMod_DX10_dll.h"
#endif

/**
 * This function is called during the dll load sequence (dll entry)
 * @param[in] hModule
 */
void InitInstance(HINSTANCE hModule);

/**
 * This function is called, when the dll gets unloaded.
 */
void ExitInstance(void);

/**
 * Returns true if this game shall be injected (DirectX hook). It returns also the name and path of the game executable.
 * This function always return true, when compiled for "Direct Injection" of for "No Injection".
 * @param[out] ret buffer where the name and path of the game executable should be stored
 * @param[in] max_len length of the buffer
 * @return
 */
bool HookThisProgram( wchar_t *ret, const int max_len);

/**
 * This function is called, when the server thread is created. It only calls the mainloop function of the \a TextureServer.
 * @param[in] lpParam Pointer to the \a TextureServer.
 */
DWORD WINAPI ServerThread( LPVOID lpParam);



#if INJECTION_METHOD==DIRECT_INJECTION || INJECTION_METHOD==HOOK_INJECTION
/**
 * Detour a function to another function.
 * @param[in] src pointer to function which shall be detoured
 * @param[in] dst pointer to function which shall be called instead
 * @param[in] len number of byte to be stored
 * @return trampoline = pointer to detoured function
 */
void *DetourFunc(BYTE *src, const BYTE *dst, const int len);

/**
 * Retour the function.
 * @param src pointer to the detoured function
 * @param restore trampoline = pointer to the detoured function
 * @param len number of saved bytes
 * @return
 */
bool RetourFunc(BYTE *src, BYTE *restore, const int len);
#endif


#if INJECTION_METHOD==HOOK_INJECTION
/**
 * This function is insert into the hook.
 */
LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam);
/**
 * This function install the hook, our dll will be loaded into each running process (if we have sufficient permission).
 */
void InstallHook(void);
/**
 * This function remove the hook, our dll will get unloaded from each process.
 */
void RemoveHook(void);
#endif


#if INJECTION_METHOD==DIRECT_INJECTION
/**
 * Dummy function need for the "Direct Injection" mehtod.
 */
void Nothing(void);
#endif


#endif
