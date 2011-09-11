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




#include "OTM_Main.h"


OTM_Server::OTM_Server(OTM_Frame *frame) : wxThread(wxTHREAD_JOINABLE)
{
  MainFrame = frame;
}


OTM_Server::~OTM_Server(void)
{
}


void* OTM_Server::Entry(void)
{
  bool  fConnected = false;
  HANDLE pipe_in;
  HANDLE pipe_out;
  char buffer[BUFSIZE];
  wxString abort = ABORT_SERVER;

  while(1)
  {
/*
    Beep(300,100);
    Beep(600,100);
*/
    pipe_in = CreateNamedPipeW(
       PIPE_Game2OTM,             // pipe name
       PIPE_ACCESS_INBOUND,       // read access
       PIPE_TYPE_BYTE |       // byte type pipe
       PIPE_WAIT,                // blocking mode
       PIPE_UNLIMITED_INSTANCES, // max. instances
       BUFSIZE,                  // output buffer size
       BUFSIZE,                  // input buffer size
       0,                        // client time-out
       NULL);                    // default security attribute
    if (pipe_in == INVALID_HANDLE_VALUE) return NULL;

    pipe_out = CreateNamedPipeW(
       PIPE_OTM2Game,             // pipe name
       PIPE_ACCESS_OUTBOUND,       // write access
       PIPE_TYPE_BYTE |       // byte type pipe
       PIPE_WAIT,                // blocking mode
       PIPE_UNLIMITED_INSTANCES, // max. instances
       BUFSIZE,                  // output buffer size
       BUFSIZE,                  // input buffer size
       0,                        // client time-out
       NULL);                    // default security attribute
    if (pipe_out == INVALID_HANDLE_VALUE) return NULL;


    // at first connect to the incoming pipe !!!
    fConnected = ConnectNamedPipe(pipe_in, NULL) ?
      true : (GetLastError() == ERROR_PIPE_CONNECTED);
    /*
    Beep(900,100);
    Beep(600,100);
    Beep(300,100);
    */
    if (fConnected)
    {
      unsigned long num = 0;
      //read the name of the game
      bool fSuccess = ReadFile(
                 pipe_in,        // handle to pipe
                 buffer,    // buffer to receive data
                 BUFSIZE, // size of buffer
                 &num, // number of bytes read
                 NULL);        // not overlapped I/O

      if (fSuccess)
      {
        if (num>2)
        {
          buffer[num]=0;
          buffer[num-1]=0;
          wxString name = (wchar_t*) buffer;

          if (name==abort) // kill this server thread
          {
            //Beep(1200,300);
            CloseHandle(pipe_in);
            return NULL;
          }


          fConnected = ConnectNamedPipe(pipe_out, NULL) ?
            true : (GetLastError() == ERROR_PIPE_CONNECTED);
          if (fConnected)
          {
            OTM_Event event( OTM_EVENT_TYPE, ID_Add_Game);
            event.SetName(name);
            event.SetPipeIn(pipe_in);
            event.SetPipeOut(pipe_out);
            wxPostEvent( MainFrame, event);
          }
          else
          {
            CloseHandle(pipe_in);
            CloseHandle(pipe_out);
            return NULL;
          }
        }
      }
   }
   else CloseHandle(pipe_in);
  }
  return NULL;
}

