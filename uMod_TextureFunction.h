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
#ifndef uMod_TEXTUREFUNCTION_H_
#define uMod_TEXTUREFUNCTION_H_


unsigned int GetCRC32( char *pcDatabuf, unsigned int ulDatalen);
/*
    case D3DFMT_MULTI2_ARGB8:
    case D3DFMT_VERTEXDATA:
    */
inline int GetBitsFromFormat(D3DFORMAT format)
{
  switch(format) //switch trough the formats to calculate the size of the raw data
  {
  case D3DFMT_A1: // 1-bit monochrome.
  {
    return (1);
    break;
  }

  case D3DFMT_R3G3B2: // 8-bit RGB texture format using 3 bits for red, 3 bits for green, and 2 bits for blue.
  case D3DFMT_A8: // 8-bit alpha only.
  case D3DFMT_A8P8: // 8-bit color indexed with 8 bits of alpha.
  case D3DFMT_P8: // 8-bit color indexed.
  case D3DFMT_L8: // 8-bit luminance only.
  case D3DFMT_A4L4: // 8-bit using 4 bits each for alpha and luminance.
  case D3DFMT_FORCE_DWORD:
  case D3DFMT_S8_LOCKABLE: // A lockable 8-bit stencil buffer.
  {
    return (8);
    break;
  }

  case D3DFMT_D16_LOCKABLE: //16-bit z-buffer bit depth.
  case D3DFMT_D15S1: // 16-bit z-buffer bit depth where 15 bits are reserved for the depth channel and 1 bit is reserved for the stencil channel.
  case D3DFMT_L6V5U5: // 16-bit bump-map format with luminance using 6 bits for luminance, and 5 bits each for v and u.
  case D3DFMT_V8U8: // 16-bit bump-map format using 8 bits each for u and v data.
  case D3DFMT_CxV8U8: // 16-bit normal compression format. The texture sampler computes the C channel from: C = sqrt(1 - U2 - V2).
  case D3DFMT_R5G6B5: // 16-bit RGB pixel format with 5 bits for red, 6 bits for green, and 5 bits for blue.
  case D3DFMT_X1R5G5B5: // 16-bit pixel format where 5 bits are reserved for each color.
  case D3DFMT_A1R5G5B5: // 16-bit pixel format where 5 bits are reserved for each color and 1 bit is reserved for alpha.
  case D3DFMT_A4R4G4B4: // 16-bit ARGB pixel format with 4 bits for each channel.
  case D3DFMT_A8R3G3B2: // 16-bit ARGB texture format using 8 bits for alpha, 3 bits each for red and green, and 2 bits for blue.
  case D3DFMT_X4R4G4B4: // 16-bit RGB pixel format using 4 bits for each color.
  case D3DFMT_L16: // 16-bit luminance only.
  case D3DFMT_R16F: // 16-bit float format using 16 bits for the red channel.
  case D3DFMT_A8L8: // 16-bit using 8 bits each for alpha and luminance.
  case D3DFMT_D16: // 16-bit z-buffer bit depth.
  case D3DFMT_INDEX16: // 16-bit index buffer bit depth.
  case D3DFMT_G8R8_G8B8: // ??
  case D3DFMT_R8G8_B8G8: // ??
  case D3DFMT_UYVY: // ??
  case D3DFMT_YUY2: // ??
  {
    return (16);
    break;
  }


  case D3DFMT_R8G8B8: //24-bit RGB pixel format with 8 bits per channel.
  {
    return (24);
    break;
  }

  case D3DFMT_R32F: // 32-bit float format using 32 bits for the red channel.
  case D3DFMT_X8L8V8U8: // 32-bit bump-map format with luminance using 8 bits for each channel.
  case D3DFMT_A2W10V10U10: // 32-bit bump-map format using 2 bits for alpha and 10 bits each for w, v, and u.
  case D3DFMT_Q8W8V8U8: // 32-bit bump-map format using 8 bits for each channel.
  case D3DFMT_V16U16: // 32-bit bump-map format using 16 bits for each channel.
  case D3DFMT_A8R8G8B8: // 32-bit ARGB pixel format with alpha, using 8 bits per channel.
  case D3DFMT_X8R8G8B8: // 32-bit RGB pixel format, where 8 bits are reserved for each color.
  case D3DFMT_A2B10G10R10: // 32-bit pixel format using 10 bits for each color and 2 bits for alpha.
  case D3DFMT_A8B8G8R8: // 32-bit ARGB pixel format with alpha, using 8 bits per channel.
  case D3DFMT_X8B8G8R8: // 32-bit RGB pixel format, where 8 bits are reserved for each color.
  case D3DFMT_G16R16: // 32-bit pixel format using 16 bits each for green and red.
  case D3DFMT_G16R16F: // 32-bit float format using 16 bits for the red channel and 16 bits for the green channel.
  case D3DFMT_A2R10G10B10: // 32-bit pixel format using 10 bits each for red, green, and blue, and 2 bits for alpha.
  case D3DFMT_D32: // 32-bit z-buffer bit depth.
  case D3DFMT_D24S8: // 32-bit z-buffer bit depth using 24 bits for the depth channel and 8 bits for the stencil channel.
  case D3DFMT_D24X8: //32-bit z-buffer bit depth using 24 bits for the depth channel.
  case D3DFMT_D24X4S4: // 32-bit z-buffer bit depth using 24 bits for the depth channel and 4 bits for the stencil channel.
  case D3DFMT_D32F_LOCKABLE: // A lockable format where the depth value is represented as a standard IEEE floating-point number.
  case D3DFMT_D24FS8: // A non-lockable format that contains 24 bits of depth (in a 24-bit floating point format - 20e4) and 8 bits of stencil.
  case D3DFMT_D32_LOCKABLE: // A lockable 32-bit depth buffer.
  case D3DFMT_INDEX32: // 32-bit index buffer bit depth.
  //case : //
  //case : //
  //case : //
  //case : //
  {
    return (32);
    break;
  }

  case D3DFMT_G32R32F: // 64-bit float format using 32 bits for the red channel and 32 bits for the green channel.
  case D3DFMT_Q16W16V16U16: // 64-bit bump-map format using 16 bits for each component.
  case D3DFMT_A16B16G16R16: // 64-bit pixel format using 16 bits for each component.
  case D3DFMT_A16B16G16R16F: // 64-bit float format using 16 bits for the each channel (alpha, blue, green, red).
  {
    return (64);
    break;
  }

  case D3DFMT_A32B32G32R32F: // 128-bit float format using 32 bits for the each channel (alpha, blue, green, red).
  {
    return (128);
    break;
  }
  case D3DFMT_DXT2:
  case D3DFMT_DXT3:
  case D3DFMT_DXT4:
  case D3DFMT_DXT5:
  {
    return (8);
    break;
  }
  case D3DFMT_DXT1:
  {
    return (4);
    break;
  }
  default: //compressed formats
  {
    return (4);
    break;
  }
  }
}

#endif /* uMod_TEXTUREFUNCTION_H_ */
