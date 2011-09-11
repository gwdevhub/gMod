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
along with FooOpenTexMod.  If not, see <http://www.gnu.org/licenses/>.
*/



#ifndef OTM_SERVER_H_
#define OTM_SERVER_H_
#include "OTM_Main.h"

// an object of this class should be created only once
// it waits for incomming connections (a starting game)
// and if so, it send a message to the mainthread

// Note the server thread can only be killed, if one connect to it and send "OTM_ABORT" as game name
class OTM_Server : public wxThread
{
public:
  OTM_Server(OTM_Frame *frame);
  virtual ~OTM_Server(void);


  void* Entry(void);

private:

  OTM_Frame *MainFrame;
};


#endif /* OTM_SERVER_H_ */
