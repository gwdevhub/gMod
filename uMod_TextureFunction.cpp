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


#include "uMod_Main.h"

/*
MyTypeHash GetHash(unsigned char *str, int len) // estimate the hash
{
  MyTypeHash hash = 0;
  for (int i=0; i<len; i++) hash = str[i] + (hash << 6) + (hash << 16) - hash;
  return (hash);
}
*/




/*
 *
 * BIG THANKS TO RS !!
 *
 * who gave me his hashing algorithm (well or crc32 algorithm^^)
 *
The hash function is CRC32 using polynomial 0xEDB88320.
However, the hashed data is calculated incorrectly in TexMod: it's simply BytesPerPixel * Width * Height, from the beginning of the data (that is mapped using LockRect).
The problem is that it doesn't take the pitch into account and BytesPerPixel may be wrong for some rare formats (not sure about that).
*/


#define CRC32POLY 0xEDB88320u /* CRC-32 Polynom */
#define ulCrc_in 0xffffffff

unsigned int GetCRC32( char *pcDatabuf, unsigned int ulDatalen)
{
  unsigned int crc = ulCrc_in;
  for (unsigned int idx = 0u; idx<ulDatalen; idx++)
  {
    unsigned int data = *pcDatabuf++;
    for (unsigned int bit = 0u; bit<8u; bit++, data >>=1)
    {
      crc = (crc >> 1) ^ (((crc ^ data) & 1) ? CRC32POLY : 0);
    }
  }
  return (crc);
}

