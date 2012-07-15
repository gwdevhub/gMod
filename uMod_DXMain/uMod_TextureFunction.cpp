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


void GetHash( unsigned char *str, unsigned int len, DWORD64 &hash) // estimate the hash
{
  DWORD32 hash1 = hash & 0xFFFFFFFF;
  DWORD32 hash2 = hash>>32 & 0xFFFFFFFF;

  for (int i=0; i<len; i++)
  {
    hash1 = ((hash1 << 5) + hash1) + *str; // D.J. Bernsteins algorithm (djb2)
    hash2 = *str + (hash2<< 6) + (hash2 << 16) - hash2; // SDBM algorithm
    str++;
  }
  hash = hash2;
  hash<<=32;
  hash |= hash1;
}





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

DWORD32 GetCRC32( char *pcDatabuf, unsigned int ulDatalen)
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

