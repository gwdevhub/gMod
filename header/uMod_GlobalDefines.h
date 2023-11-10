#pragma once

//#define MyTypeHash DWORD64
#define MyTypeHash DWORD32

#define BIG_BUFSIZE 1<<24
#define SMALL_BUFSIZE 1<<10

using MsgStruct = struct {
    unsigned int Control;
    unsigned int Value;
    MyTypeHash Hash;
};

using PipeStruct = struct {
    HANDLE In;
    HANDLE Out;
};


#define uMod_APP_DX9 L"uMod_DX9.txt"
#define uMod_APP_DIR L"uMod"
#define uMod_VERSION L"uMod V 1.0"

#define PIPE_uMod2Game L"\\\\.\\pipe\\uMod2Game"
#define PIPE_Game2uMod L"\\\\.\\pipe\\Game2uMod"

#define CONTROL_ADD_TEXTURE 1
#define CONTROL_FORCE_RELOAD_TEXTURE 2
#define CONTROL_REMOVE_TEXTURE 3
#define CONTROL_FORCE_RELOAD_TEXTURE_DATA 4
#define CONTROL_ADD_TEXTURE_DATA 5
#define CONTROL_MORE_TEXTURES 6


#define CONTROL_SAVE_ALL 10
#define CONTROL_SAVE_SINGLE 11
#define CONTROL_SET_DIR 12

#define CONTROL_KEY_BACK 20
#define CONTROL_KEY_SAVE 21
#define CONTROL_KEY_NEXT 22

#define CONTROL_FONT_COLOUR 30
#define CONTROL_TEXTURE_COLOUR 31
