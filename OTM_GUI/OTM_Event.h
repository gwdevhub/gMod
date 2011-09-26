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



#ifndef OTM_EVENTTYPE_H_
#define OTM_EVENTTYPE_H_
#include "OTM_Client.h"
#include "OTM_Main.h"


//we need our own event to pass send some arguments from the server or the client thread to the main thread
BEGIN_DECLARE_EVENT_TYPES()
DECLARE_EVENT_TYPE( OTM_EVENT_TYPE, -1)
END_DECLARE_EVENT_TYPES()

class OTM_Event : public wxCommandEvent
{
public:
  OTM_Event( wxEventType commandType = OTM_EVENT_TYPE, int id = 0 )
  :  wxCommandEvent(commandType, id) { }
  virtual ~OTM_Event(void) {}

  // You *must* copy here the data to be transported
  OTM_Event( const OTM_Event &event )
  :  wxCommandEvent(event) { this->SetText( event.GetText()); PipeIn=((OTM_Event&)event).GetPipeIn(); PipeOut=((OTM_Event&)event).GetPipeOut(); Name=((OTM_Event&)event).GetName(); Client=((OTM_Event&)event).GetClient();}

  // Required for sending with wxPostEvent()
  wxEvent* Clone() const { return new OTM_Event(*this); }

  wxString GetText() const { return m_Text; }
  void SetText( const wxString& text ) { m_Text = text; }

  wxString GetName(void) {return Name;}
  HANDLE GetPipeIn(void) {return PipeIn;}
  HANDLE GetPipeOut(void) {return PipeOut;}
  OTM_Client * GetClient(void) {return Client;}

  void SetName( wxString name) {Name=name;}
  void SetPipeIn( HANDLE pipe) {PipeIn=pipe;}
  void SetPipeOut( HANDLE pipe) {PipeOut=pipe;}
  void SetClient( OTM_Client *client) {Client=client;}

private:

  wxString Name;
  HANDLE PipeIn;
  HANDLE PipeOut;
  OTM_Client *Client;

  wxString m_Text;
};


#endif /* OTM_EVENTTYPE_H_ */
