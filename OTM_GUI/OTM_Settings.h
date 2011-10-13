/*
This file is part of OpenTexMod.


OpenTexMod is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OpenTexMod is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with OpenTexMod.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef OTM_SETTINGS_H_
#define OTM_SETTINGS_H_


#include "OTM_Main.h"

class OTM_Settings
{
public:
  OTM_Settings(void);


  int XSize, YSize;
  wxString Language;

  int Load(void);
  int Save(void);

};


#endif /* OTM_SETTINGS_H_ */
