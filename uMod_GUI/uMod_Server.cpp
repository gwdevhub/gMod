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


uMod_Server::uMod_Server(uMod_Frame *frame) : wxThread(wxTHREAD_JOINABLE)
{
  MainFrame = frame;
}


uMod_Server::~uMod_Server(void)
{
}


void* uMod_Server::Entry(void)
{
  bool  fConnected = false;
  HANDLE pipe_in;
  HANDLE pipe_out;
  char buffer[SMALL_BUFSIZE];
  wxString abort = ABORT_SERVER;

  while(1)
  {
    pipe_in = CreateNamedPipeW(
       PIPE_Game2uMod,             // pipe name
       PIPE_ACCESS_INBOUND,       // read access
       PIPE_TYPE_BYTE |       // byte type pipe
       PIPE_WAIT,                // blocking mode
       PIPE_UNLIMITED_INSTANCES, // max. instances
       SMALL_BUFSIZE,                  // output buffer size
       SMALL_BUFSIZE,                  // input buffer size
       0,                        // client time-out
       NULL);                    // default security attribute
    if (pipe_in == INVALID_HANDLE_VALUE) return NULL;

    pipe_out = CreateNamedPipeW(
       PIPE_uMod2Game,             // pipe name
       PIPE_ACCESS_OUTBOUND,       // write access
       PIPE_TYPE_BYTE |       // byte type pipe
       PIPE_WAIT,                // blocking mode
       PIPE_UNLIMITED_INSTANCES, // max. instances
       BIG_BUFSIZE,                  // output buffer size
       BIG_BUFSIZE,                  // input buffer size
       0,                        // client time-out
       NULL);                    // default security attribute
    if (pipe_out == INVALID_HANDLE_VALUE) return NULL;


    // at first connect to the incoming pipe !!!
    fConnected = ConnectNamedPipe(pipe_in, NULL) ?
      true : (GetLastError() == ERROR_PIPE_CONNECTED);

    if (fConnected)
    {
      unsigned long num = 0;
      //read the name of the game
      bool fSuccess = ReadFile(
                 pipe_in,        // handle to pipe
                 buffer,    // buffer to receive data
                 SMALL_BUFSIZE, // size of buffer
                 &num, // number of bytes read
                 NULL);        // not overlapped I/O

      if (fSuccess)
      {
        if (num>4)
        {
          buffer[num]=0;
          buffer[num-1]=0;
          int injection_method = * ((int*)buffer);
          wxString name = (wchar_t*) &buffer[sizeof(int)];

          if (name==abort) // kill this server thread
          {
            CloseHandle(pipe_in);
            return NULL;
          }


          fConnected = ConnectNamedPipe(pipe_out, NULL) ?
            true : (GetLastError() == ERROR_PIPE_CONNECTED);
          if (fConnected)
          {
            uMod_Event *event = new uMod_Event( uMod_EVENT_TYPE, ID_Add_Game);
            event->SetName(name.c_str());
            event->SetValue(injection_method);
            event->SetPipeIn(pipe_in);
            event->SetPipeOut(pipe_out);
            wxQueueEvent( MainFrame, (wxEvent*) event);
            //wxPostEvent( MainFrame, event);
          }
          else
          {
            CloseHandle(pipe_in);
            CloseHandle(pipe_out);
            return NULL;
          }
        }
        else
        {
          wxMessageBox("less than 4 bytes");
        }
      }
      else
      {
        wxMessageBox("no success");
      }
   }
   else CloseHandle(pipe_in);
  }
  return NULL;
}

