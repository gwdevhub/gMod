/*
 * uMod_DetourEntry.h
 *
 *  Created on: 13.01.2013
 *      Author: marts
 */

#ifndef UMOD_DETOURENTRY_H_
#define UMOD_DETOURENTRY_H_

#include "uMod_Main.h"

#ifdef __CDT_PARSER__
#define NULL 0
#endif

class uMod_Detour_Entry_Base
{
public:
  uMod_Detour_Entry_Base() {}
  virtual ~uMod_Detour_Entry_Base() {}

  virtual void TestLib(const char* lib_name, HMODULE plib) = 0;
  virtual void TestLib(const wchar_t* lib_name, HMODULE plib) = 0;
  virtual void FreeLib(HMODULE plib) = 0;


  virtual void Detour(void) = 0;
  virtual void Retour(void) = 0;
};




template <class T>
class uMod_Detour_Entry : public uMod_Detour_Entry_Base
{
public:
  uMod_Detour_Entry(const int num_bytes);
  virtual ~uMod_Detour_Entry();


  virtual void TestLib(const char* lib_name, HMODULE plib);
  virtual void TestLib(const wchar_t* lib_name, HMODULE plib);
  virtual void FreeLib(HMODULE plib);

  virtual void Detour(void);
  virtual void Retour(void);
  bool CheckForDetour();

  int SetFunctionName( const char* name) {return SetName( FunctionName, name);}
  int SetLibName( const char* name) {return SetName( LibName, name);}
  int SetFullLibName( const char* name) {return SetName( FullLibName, name);}

  int SetLibName( const wchar_t* name) {return SetName( LibNameW, name);}
  int SetFullLibName( const wchar_t* name) {return SetName( FullLibNameW, name);}

  void SetOrigFunction(T func) {OrigFunction = func;}
  void SetTargetFunction(T func) {TargetFunction = func;}

  T Function(void) const {return OrigFunction;}

private:


  int SetName( char* &member_var, const char* name);
  int SetName( wchar_t* &member_var, const wchar_t* name);

  char* FunctionName;
  char* LibName;
  char* FullLibName;

  wchar_t* LibNameW;
  wchar_t* FullLibNameW;

  HMODULE PLibrary;
  int RefCounter;



  const int NumBytes;
  T OrigFunction;
  T TargetFunction;

  BYTE* DetourFunction;
  bool IsDetoured;
  bool IsFailed;
};



template <class T>
uMod_Detour_Entry<T>::uMod_Detour_Entry(const int num_bytes) : FunctionName(NULL), LibName(NULL), FullLibName(NULL),
      LibNameW(NULL), FullLibNameW(NULL), PLibrary(NULL),
      NumBytes(num_bytes), OrigFunction(NULL), TargetFunction(NULL), DetourFunction(NULL), IsDetoured(false), IsFailed(false)
{
}


template <class T>
uMod_Detour_Entry<T>::~uMod_Detour_Entry()
{
  if (FunctionName!=NULL) delete [] FunctionName;
  if (LibName!=NULL) delete [] LibName;
  if (FullLibName!=NULL) delete [] FullLibName;
  if (LibNameW!=NULL) delete [] LibNameW;
  if (FullLibNameW!=NULL) delete [] FullLibNameW;
  if (DetourFunction!=NULL) delete [] DetourFunction;
}


template <class T>
void uMod_Detour_Entry<T>::TestLib(const char* lib_name, HMODULE plib)
{
  if (lib_name==NULL) return;
  if (plib==NULL) return;
  if (FunctionName==NULL) return;
  if (LibName==NULL) return;

  if (PLibrary==plib)
  {
    RefCounter++;
    //return;
  }

  int len=0;
  while (lib_name[len]) len++;
  while (len>=0 && (lib_name[len]!='/' && lib_name[len]!='\\')) len--;
  len++;

  if (_stricmp( &lib_name[len], LibName)==0)
  {
    bool set_detour = true;
    if (FullLibName!=NULL)
    {
      set_detour = false;
      GlobalDetour.Detour_LoadLibraryA.Retour();
      GlobalDetour.Detour_FreeLibrary.Retour();
      HMODULE test_handle = LoadLibraryA( FullLibName); //get handle of original library
      if (test_handle==NULL || test_handle==plib) set_detour = true; // test if library (loaded by the game) is the original one or a fake one
      //if (test_handle!=NULL) FreeLibrary( test_handle); // free the library (we don't want top disturb our own RefCounter)
      GlobalDetour.Detour_LoadLibraryA.Detour();
      GlobalDetour.Detour_FreeLibrary.Detour();
    }

    if (!set_detour) return; // it is not the official library (or FullLibName is not set)
    if (CheckForDetour()) return;

    OrigFunction = (T) GetProcAddress(plib, FunctionName);
    if (OrigFunction==NULL) return; // this library does not contain the function we want to detour


    Message("uMod_Detour_Entry<T>::TestLib( %s, %p ) set detour -> %s \n", lib_name, plib, FunctionName);

    RefCounter = 1;
    PLibrary = plib;

    IsDetoured = false;
    IsFailed = false;
    Detour();
  }
}

template <class T>
void uMod_Detour_Entry<T>::TestLib(const wchar_t* lib_name, HMODULE plib)
{
  if (lib_name==NULL) return;
  if (plib==NULL) return;
  if (FunctionName==NULL) return;
  if (LibNameW==NULL) return;

  if (PLibrary==plib)
  {
    RefCounter++;
//    return;
  }

  int len=0;
  while (lib_name[len]) len++;
  while (len>=0 && (lib_name[len]!='/' && lib_name[len]!='\\' )) len--;
  len++;

  if (_wcsicmp( &lib_name[len], LibNameW)==0)
  {
    bool set_detour = true;
    if (FullLibNameW!=NULL)
    {
      set_detour = false;

      GlobalDetour.Detour_LoadLibraryW.Retour();
      GlobalDetour.Detour_FreeLibrary.Retour();
      HMODULE test_handle = LoadLibraryW( FullLibNameW); //get handle of original library
      if (test_handle==NULL || test_handle==plib) set_detour = true; // test if library (loaded by the game) is the original one or a fake one
      if (test_handle!=NULL) FreeLibrary( test_handle); // free the library (we don't want top disturb our own RefCounter)

      GlobalDetour.Detour_LoadLibraryW.Detour();
      GlobalDetour.Detour_FreeLibrary.Detour();
    }

    if (!set_detour) return; // it is not the official library (or FullLibName is not set)
    if (CheckForDetour()) return;

    OrigFunction = (T) GetProcAddress(plib, FunctionName);
    if (OrigFunction==NULL) return; // this library does not contain the function we want to detour


    Message("uMod_Detour_Entry<T>::TestLib( %ls, %p ) set detour -> %s \n", lib_name, plib, FunctionName);

    RefCounter = 1;
    PLibrary = plib;

    IsDetoured = false;
    IsFailed = false;
    Detour();
  }
}

template <class T>
void uMod_Detour_Entry<T>::FreeLib(HMODULE plib)
{
  if (plib==NULL) return;
  if (plib==PLibrary) RefCounter--;
  if (RefCounter<=0) PLibrary = NULL;
}

template <class T>
void uMod_Detour_Entry<T>::Detour(void)
{
  if (IsDetoured || IsFailed) return;
  if (DetourFunction==NULL)
  {
    DetourFunction = new BYTE[NumBytes+5];
  }
  BYTE *jmp = DetourFunction;
  BYTE *src = (BYTE*) OrigFunction;
  const BYTE *dst = (BYTE*) TargetFunction;
  const int len = NumBytes;

  DWORD dwback = 0;
  //(jmp, len+5, PAGE_EXECUTE_READWRITE, &dwback); //This is the addition needed for Windows 7 RC
  if(!VirtualProtect(src, NumBytes, PAGE_READWRITE, &dwback)) IsFailed = true;
  if(!memcpy(jmp, src, len)) IsFailed = true;
  jmp += len;
  jmp[0] = 0xE9;
  *(DWORD*)(jmp+1) = (DWORD)(src+len - jmp) - 5;
  //memset(src, 0x90, len);
  src[0] = 0xE9;
  *(DWORD*)(src+1) = (DWORD)(dst - src) - 5;
  if(!VirtualProtect(src, len, dwback, &dwback)) IsFailed = true;

  IsDetoured = true;
}

template <class T>
void uMod_Detour_Entry<T>::Retour(void)
{
  if (!IsDetoured || IsFailed) return;

  BYTE *src = (BYTE*) OrigFunction;
  BYTE *restore = DetourFunction;
  const int len = NumBytes;
  DWORD dwback;
  if(!VirtualProtect(src, len, PAGE_READWRITE, &dwback)) IsFailed = true;
  if(!memcpy(src, restore, len))  IsFailed = true;
  restore[0] = 0xE9;
  *(DWORD*)(restore+1) = (DWORD)(src - restore) - 5;

  if(!VirtualProtect(src, len, dwback, &dwback)) IsFailed = true;

  IsDetoured = false;
}


template <class T>
bool uMod_Detour_Entry<T>::CheckForDetour()
{
  if (DetourFunction==NULL) return false;

  //BYTE *jmp = DetourFunction;
  BYTE *src = (BYTE*) OrigFunction;
  const BYTE *dst = (BYTE*) TargetFunction;
  //const int len = NumBytes;

  if (src[0] != 0xE9) return false;
  if ( *(DWORD*)(src+1) != (DWORD)(dst - src) - 5) return false;

  return true;
}

template <class T>
int uMod_Detour_Entry<T>::SetName( char* &member_var, const char* name)
{
  if (member_var!=NULL) delete [] member_var;
  member_var = NULL;
  int len=0;
  while (name[len]) len++;
  len++;

  member_var = new char[len];
  len = 0;

  while (name[len]) {member_var[len] = name[len]; len++;}
  member_var[len] = 0;
  return 0;
}

template <class T>
int uMod_Detour_Entry<T>::SetName( wchar_t* &member_var, const wchar_t* name)
{
  if (member_var!=NULL) delete [] member_var;
  member_var = NULL;
  int len=0;
  while (name[len]) len++;
  len++;

  member_var = new wchar_t[len];
  len = 0;

  while (name[len]) {member_var[len] = name[len]; len++;}
  member_var[len] = 0;
  return 0;
}

#endif /* UMOD_DETOURENTRY_H_ */
