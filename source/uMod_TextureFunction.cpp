#include "uMod_Main.h"

/*
MyTypeHash GetHash(unsigned char *str, int len) // estimate the hash
{
  MyTypeHash hash = 0;
  for (int i=0; i<len; i++) hash = str[i] + (hash << 6) + (hash << 16) - hash;
  return hash);
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

unsigned int GetCRC32(char* pcDatabuf, unsigned int ulDatalen)
{
    unsigned int crc = ulCrc_in;
    for (unsigned int idx = 0u; idx < ulDatalen; idx++) {
        unsigned int data = *pcDatabuf++;
        for (unsigned int bit = 0u; bit < 8u; bit++, data >>= 1) {
            crc = crc >> 1 ^ ((crc ^ data) & 1 ? CRC32POLY : 0);
        }
    }
    return crc;
}
