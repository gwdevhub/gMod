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



#ifndef OTM_CLIENT_H_
#define OTM_CLIENT_H_

#include "OTM_Main.h"

// an object of this class is created for each running game
// it reads out of the incoming pipe (it must run as thread)
// if the pipe is closed (Game is canceled) it send a messeage to the main thread
// it should read the error state from the dll, but this is not yet implemented
class OTM_Client : public wxThread
{
public:
  OTM_Client( PipeStruct &pipe, OTM_Frame *frame);
  virtual ~OTM_Client(void);

  void* Entry(void);


  PipeStruct Pipe;

private:
  OTM_Frame *MainFrame;
};

#endif /* OTM_CLIENT_H_ */
