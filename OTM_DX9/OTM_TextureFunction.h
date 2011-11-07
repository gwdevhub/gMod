/*
 * OTM_TextureFunction.h
 *
 *  Created on: 28.10.2011
 *      Author: marts
 */

#ifndef OTM_TEXTUREFUNCTION_H_
#define OTM_TEXTUREFUNCTION_H_


unsigned int GetCRC32( char *pcDatabuf, unsigned int ulDatalen);

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
  {
    return (8);
    break;
  }

  case D3DFMT_L6V5U5: // 16-bit bump-map format with luminance using 6 bits for luminance, and 5 bits each for v and u.
  case D3DFMT_V8U8: // 16-bit bump-map format using 8 bits each for u and v data.
  case D3DFMT_R5G6B5: // 16-bit RGB pixel format with 5 bits for red, 6 bits for green, and 5 bits for blue.
  case D3DFMT_X1R5G5B5: // 16-bit pixel format where 5 bits are reserved for each color.
  case D3DFMT_A1R5G5B5: // 16-bit pixel format where 5 bits are reserved for each color and 1 bit is reserved for alpha.
  case D3DFMT_A4R4G4B4: // 16-bit ARGB pixel format with 4 bits for each channel.
  case D3DFMT_A8R3G3B2: // 16-bit ARGB texture format using 8 bits for alpha, 3 bits each for red and green, and 2 bits for blue.
  case D3DFMT_X4R4G4B4: // 16-bit RGB pixel format using 4 bits for each color.
  case D3DFMT_L16: // 16-bit luminance only.
  case D3DFMT_A8L8: // 16-bit using 8 bits each for alpha and luminance.
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
  case D3DFMT_A2R10G10B10: // 32-bit pixel format using 10 bits each for red, green, and blue, and 2 bits for alpha.
  {
    return (32);
    break;
  }

  case D3DFMT_G32R32F: // 64-bit float format using 32 bits for the red channel and 32 bits for the green channel.
  case D3DFMT_Q16W16V16U16: // 64-bit bump-map format using 16 bits for each component.
  case D3DFMT_A16B16G16R16: // 64-bit pixel format using 16 bits for each component.
  {
    return (64);
    break;
  }

  case D3DFMT_A32B32G32R32F: // 128-bit float format using 32 bits for the each channel (alpha, blue, green, red).
  {
    return (128);
    break;
  }
  default: //compressed formats
  {
    return (4);
    break;
  }
  }
}

#endif /* OTM_TEXTUREFUNCTION_H_ */
