#ifndef OTM_GlobalDefines_H_
#define OTM_GlobalDefines_H_

//#define MyTypeHash DWORD64
#define MyTypeHash DWORD32

#define BIG_BUFSIZE 1<<24
#define SMALL_BUFSIZE 1<<10

typedef struct
{
  unsigned int Control;
  unsigned int Value;
  MyTypeHash Hash;
} MsgStruct;

typedef struct
{
  HANDLE In;
  HANDLE Out;
} PipeStruct;


#define OTM_APP_DX9 L"OTM_DX9.txt"
#define OTM_APP_DIR L"OpenTexMod"

#define PIPE_OTM2Game L"\\\\.\\pipe\\OTM2Game"
#define PIPE_Game2OTM L"\\\\.\\pipe\\Game2OTM"

#define CONTROL_ADD_TEXTURE 1
#define CONTROL_FORCE_RELOAD_TEXTURE 2
#define CONTROL_REMOVE_TEXTURE 3
#define CONTROL_FORCE_RELOAD_TEXTURE_DATA 4
#define CONTROL_ADD_TEXTURE_DATA 5


#define CONTROL_SAVE_ALL 10
#define CONTROL_SAVE_SINGLE 11
#define CONTROL_SET_DIR 12

#define CONTROL_KEY_BACK 20
#define CONTROL_KEY_SAVE 21
#define CONTROL_KEY_NEXT 22

#define CONTROL_FONT_COLOUR 30
#define CONTROL_TEXTURE_COLOUR 31




#endif
