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


#ifndef OTM_ERROR_H_
#define OTM_ERROR_H_


// define return values, a value less than zero indicates an error
#define RETURN_OK 0

#define RETURN_FATAL_ERROR -1
#define RETURN_NO_MEMORY -2
#define RETURN_BAD_ARGUMENT -3

#define RETURN_NO_IDirect3DDevice9 -10

#define RETURN_TEXTURE_NOT_LOADED -20
#define RETURN_TEXTURE_NOT_SAVED -21
#define RETURN_TEXTURE_NOT_FOUND -22
#define RETURN_TEXTURE_ALLREADY_ADDED -23
#define RETURN_TEXTURE_NOT_SWITCHED -24

#define RETURN_LockRect_FAILED -30
#define RETURN_UnlockRect_FAILED -31
#define RETURN_GetLevelDesc_FAILED -32


#define RETURN_NO_MUTEX -40
#define RETURN_MUTEX_LOCK -41
#define RETURN_MUTEX_UNLOCK -42

#define RETURN_UPDATE_ALLREADY_ADDED -50
#define RETURN_FILE_NOT_LOADED -51

#define RETURN_PIPE_NOT_OPENED 60





// define error states
#define OTM_ERROR_FATAL 1u
#define OTM_ERROR_MUTEX 1u<<1
#define OTM_ERROR_PIPE 1u<<2
#define OTM_ERROR_MEMORY 1u<<3
#define OTM_ERROR_TEXTURE 1u<<4
#define OTM_ERROR_MULTIPLE_IDirect3D9 1u<<5
#define OTM_ERROR_MULTIPLE_IDirect3DDevice9 1u<<6
#define OTM_ERROR_UPDATE 1u<<7
#define OTM_ERROR_SERVER 1u<<8






#endif /* OTM_ERROR_H_ */
