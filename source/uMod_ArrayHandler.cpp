#include "uMod_Main.h"





uMod_FileHandler::uMod_FileHandler()
{
    Message("uMod_FileHandler(): %lu\n", this);
    Number = 0;
    FieldCounter = 0;
    Files = nullptr;
}

uMod_FileHandler::~uMod_FileHandler()
{
    Message("~uMod_FileHandler(): %lu\n", this);
    if (Files != nullptr) {
        for (int i = 0; i < FieldCounter; i++) {
            delete [] Files[i];
        }
        delete [] Files;
    }
}

int uMod_FileHandler::Add(TextureFileStruct* file)
{
    Message("uMod_FileHandler::Add(%lu): %lu\n", file, this);
    if (gl_ErrorState & uMod_ERROR_FATAL) {
        return RETURN_FATAL_ERROR;
    }

    if (file->Reference >= 0) {
        return RETURN_UPDATE_ALLREADY_ADDED;
    }

    if (Number / FieldLength == FieldCounter) // get more memory
    {
        TextureFileStruct*** temp = nullptr;
        try { temp = new TextureFileStruct**[FieldCounter + 10]; }
        catch (...) {
            gl_ErrorState |= uMod_ERROR_MEMORY | uMod_ERROR_TEXTURE;
            return RETURN_NO_MEMORY;
        }

        for (int i = 0; i < FieldCounter; i++) {
            temp[i] = Files[i]; //copy to new allocated memory
        }

        for (int i = FieldCounter; i < FieldCounter + 10; i++) {
            temp[i] = nullptr; // initialize unused parts to zero
        }

        FieldCounter += 10;

        delete [] Files;

        Files = temp;
    }
    if (Number % FieldLength == 0) // maybe we need to get more memory
    {
        try {
            if (Files[Number / FieldLength] == nullptr) {
                Files[Number / FieldLength] = new TextureFileStruct*[FieldLength];
            }
        }
        catch (...) {
            Files[Number / FieldLength] = nullptr;
            gl_ErrorState |= uMod_ERROR_MEMORY | uMod_ERROR_TEXTURE;
            return RETURN_NO_MEMORY;
        }
    }

    Files[Number / FieldLength][Number % FieldLength] = file;
    file->Reference = Number++; //set the reference for a fast deleting

    return RETURN_OK;
}


int uMod_FileHandler::Remove(TextureFileStruct* file)
{
    Message("uMod_FileHandler::Remove(%lu): %lu\n", file, this);
    if (gl_ErrorState & uMod_ERROR_FATAL) {
        return RETURN_FATAL_ERROR;
    }
    const int ref = file->Reference;

    if (ref < 0) {
        return RETURN_OK; // returning if no Reference is set
    }
    file->Reference = -1; //set reference outside of bound
    if (ref < (--Number)) //if reference is unequal to Number-1 we copy the last entry to the index "ref"
    {
        Files[ref / FieldLength][ref % FieldLength] = Files[Number / FieldLength][Number % FieldLength];
        Files[ref / FieldLength][ref % FieldLength]->Reference = ref; //set the new reference entry
    }
    return RETURN_OK;
}
