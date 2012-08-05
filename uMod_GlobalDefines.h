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

#ifndef uMod_GlobalDefines_H_
#define uMod_GlobalDefines_H_


#define BIG_BUFSIZE 1<<24
#define SMALL_BUFSIZE 1<<10

/**
 * This struct is the basic of the communication between dll and GUI. For each command send through the pipe a control value is set to MsgStruct::Control.
 * The MsgStruct::Value can store various information (e.g. key values, boolean, or length of data).
 *
 * If mor date should be send, Control is set to one of the intended values and value to the amount of data in byte.
 * After the MsgStruct is send also the data is send through the pipe.
 */
typedef struct
{
  DWORD32 Control; //!< stores the control value
  DWORD64 Value; //!< various meanings
  DWORD64 Hash;
} MsgStruct;

/**
 *
 */
typedef struct
{
  HANDLE In; //!< Incoming pipe
  HANDLE Out; //!< Outgoning pipe
} PipeStruct;


#define uMod_APP_DX9 L"uMod_DX9.txt"
#define uMod_APP_DIR L"uMod"
#define uMod_VERSION_char "uMod V 2.0 alpha (r51)"
#define uMod_VERSION L"uMod V 2.0 alpha (r51)"

#define PIPE_uMod2Game L"\\\\.\\pipe\\uMod2Game"
#define PIPE_Game2uMod L"\\\\.\\pipe\\Game2uMod"

#define CONTROL_ADD_TEXTURE 1
#define CONTROL_FORCE_RELOAD_TEXTURE 2
#define CONTROL_REMOVE_TEXTURE 3
#define CONTROL_FORCE_RELOAD_TEXTURE_DATA 4
#define CONTROL_ADD_TEXTURE_DATA 5
#define CONTROL_MORE_TEXTURES 6
#define CONTROL_END_TEXTURES 7


#define CONTROL_SAVE_ALL 10
#define CONTROL_SAVE_SINGLE 11
#define CONTROL_SHOW_STRING 12
#define CONTROL_SHOW_TEXTURE 13
#define CONTROL_SET_DIR 14

#define CONTROL_KEY_BACK 20
#define CONTROL_KEY_SAVE 21
#define CONTROL_KEY_NEXT 22

#define CONTROL_FONT_COLOUR 30
#define CONTROL_TEXTURE_COLOUR 31

#define CONTROL_WIDTH_FILTER 40
#define CONTROL_HEIGHT_FILTER 41
#define CONTROL_DEPTH_FILTER 42
#define CONTROL_FORMAT_FILTER 43

#define CONTROL_SAVE_FORMAT 50

#define CONTROL_SUPPORT_TPF 60

#define CONTROL_GAME_EXIT 100
#define CONTROL_ADD_CLIENT 101
#define CONTROL_REMOVE_CLIENT 102

#define HOOK_INJECTION 1
#define DIRECT_INJECTION 2
#define NO_INJECTION 3
#define INVALID_GAME_PAGE 4

#define VERSION_DX9     90
#define VERSION_DX9EX   91
#define VERSION_DX10   100
#define VERSION_DX101  101


#define uMod_D3DXIFF_BMP 1ull<<0
#define uMod_D3DXIFF_JPG 1ull<<1
#define uMod_D3DXIFF_TGA 1ull<<2
#define uMod_D3DXIFF_PNG 1ull<<3
#define uMod_D3DXIFF_DDS 1ull<<4
#define uMod_D3DXIFF_PPM 1ull<<5




#define uMod_D3DFMT_R8G8B8 1ull<<0
#define uMod_D3DFMT_A8R8G8B8 1ull<<1
#define uMod_D3DFMT_X8R8G8B8 1ull<<2
#define uMod_D3DFMT_R5G6B5 1ull<<3
#define uMod_D3DFMT_X1R5G5B5 1ull<<4
#define uMod_D3DFMT_A1R5G5B5 1ull<<5
#define uMod_D3DFMT_A4R4G4B4 1ull<<6
#define uMod_D3DFMT_R3G3B2 1ull<<7
#define uMod_D3DFMT_A8 1ull<<8
#define uMod_D3DFMT_A8R3G3B2 1ull<<9
#define uMod_D3DFMT_X4R4G4B4 1ull<<10
#define uMod_D3DFMT_A2B10G10R10 1ull<<11
#define uMod_D3DFMT_A8B8G8R8 1ull<<12
#define uMod_D3DFMT_X8B8G8R8 1ull<<13
#define uMod_D3DFMT_G16R16 1ull<<14
#define uMod_D3DFMT_A2R10G10B10 1ull<<15
#define uMod_D3DFMT_A16B16G16R16 1ull<<16

#define uMod_D3DFMT_A8P8 1ull<<17
#define uMod_D3DFMT_P8 1ull<<18

#define uMod_D3DFMT_L8 1ull<<19
#define uMod_D3DFMT_A8L8 1ull<<20
#define uMod_D3DFMT_A4L4 1ull<<21

#define uMod_D3DFMT_V8U8 1ull<<22
#define uMod_D3DFMT_L6V5U5 1ull<<23
#define uMod_D3DFMT_X8L8V8U8 1ull<<24
#define uMod_D3DFMT_Q8W8V8U8 1ull<<25
#define uMod_D3DFMT_V16U16 1ull<<26
#define uMod_D3DFMT_A2W10V10U10 1ull<<27


#define uMod_D3DFMT_D16_LOCKABLE 1ull<<28
#define uMod_D3DFMT_D32 1ull<<29
#define uMod_D3DFMT_D15S1 1ull<<30
#define uMod_D3DFMT_D24S8 1ull<<31
#define uMod_D3DFMT_D24X8 1ull<<32
#define uMod_D3DFMT_D24X4S4 1ull<<33
#define uMod_D3DFMT_D16 1ull<<34
#define uMod_D3DFMT_L16 1ull<<35

#define uMod_D3DFMT_D32F_LOCKABLE 1ull<<36
#define uMod_D3DFMT_D24FS8 1ull<<37

#define uMod_D3DFMT_D32_LOCKABLE 1ull<<38
#define uMod_D3DFMT_S8_LOCKABLE 1ull<<39


#define uMod_D3DFMT_VERTEXDATA 1ull<<40
#define uMod_D3DFMT_INDEX16 1ull<<41
#define uMod_D3DFMT_INDEX32 1ull<<42

#define uMod_D3DFMT_Q16W16V16U16 1ull<<43


#define uMod_D3DFMT_R16F 1ull<<44
#define uMod_D3DFMT_G16R16F 1ull<<45
#define uMod_D3DFMT_A16B16G16R16F 1ull<<46

#define uMod_D3DFMT_R32F 1ull<<47
#define uMod_D3DFMT_G32R32F 1ull<<48
#define uMod_D3DFMT_A32B32G32R32F 1ull<<49

#define uMod_D3DFMT_CxV8U8 1ull<<50

#define uMod_D3DFMT_A1 1ull<<51
#define uMod_D3DFMT_A2B10G10R10_XR_BIAS 1ull<<52
#define uMod_D3DFMT_BINARYBUFFER  1ull<<53


#define uMod_D3DFMT_DXT1 1ull<<54
#define uMod_D3DFMT_DXT2 1ull<<55
#define uMod_D3DFMT_DXT3 1ull<<56
#define uMod_D3DFMT_DXT4 1ull<<57
#define uMod_D3DFMT_DXT5 1ull<<58
#define uMod_D3DFMT_G8R8_G8B8 1ull<<59
#define uMod_D3DFMT_R8G8_B8G8 1ull<<60
#define uMod_D3DFMT_MULTI2_ARGB8 1ull<<61
#define uMod_D3DFMT_UYVY 1ull<<62
#define uMod_D3DFMT_YUY2 1ull<<63
    

#endif
