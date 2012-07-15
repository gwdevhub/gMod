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

#ifndef UMOD_UTILS_H_
#define UMOD_UTILS_H_


template <class T>
int GetMemory(T* &array, int num)
{
  if (array!=(T*)0) delete [] array;
  try {array = new T[num];}
  catch (...) {array = (T*) 0; return -1;}
  return 0;
}

template <class T>
int GetMemory(T* &array, int num, T init)
{
  if (array!=(T*)0) delete [] array;
  try {array = new T[num];}
  catch (...) {array = (T*) 0; return -1;}
  for (int i=0; i<num; i++) array[i]=init;
  return 0;
}

template <class T>
int GetMoreMemory(T* &old_array, int old_num, int new_num)
{
  if (new_num<=old_num) return 0;
  T* new_array;
  try {new_array = new T[new_num];}
  catch (...) {return -1;}
  if (old_array!=(T*)0)
  {
    for (int i=0; i<old_num; i++) new_array[i]=old_array[i];
    delete [] old_array;
  }
  old_array = new_array;
  return 0;
}




#endif /* UMOD_UTILS_H_ */
