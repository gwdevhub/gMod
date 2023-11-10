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

#pragma once
#include<string>
#include<Windows.h>

void ReplaceAll(std::wstring& str, const std::wstring& from, const std::wstring& to) {
    if (from.empty()) {
        return;
    }
    size_t startPos = 0;
    while ((startPos = str.find(from, startPos)) != std::wstring::npos) {
        str.replace(startPos, from.length(), to);
        startPos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

void ReplaceAll(std::string& str, const std::string& from, const std::string& to) {
    if (from.empty()) {
        return;
    }
    size_t startPos = 0;
    while ((startPos = str.find(from, startPos)) != std::string::npos) {
        str.replace(startPos, from.length(), to);
        startPos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

std::wstring AfterFirst(const std::wstring& str, wchar_t delimiter) {
    size_t pos = str.find(delimiter);
    if (pos != std::wstring::npos) {
        // Return the substring after the delimiter
        return str.substr(pos + 1);
    }
    // If the delimiter is not found, return an empty string
    return L"";
}

std::string AfterFirst(const std::string& str, char delimiter) {
    size_t pos = str.find(delimiter);
    if (pos != std::string::npos) {
        // Return the substring after the delimiter
        return str.substr(pos + 1);
    }
    // If the delimiter is not found, return an empty string
    return "";
}

std::wstring BeforeFirst(const std::wstring& str, wchar_t delimiter) {
    size_t pos = str.find(delimiter);
    return pos != std::wstring::npos ? str.substr(0, pos) : str;
}

std::string BeforeFirst(const std::string& str, char delimiter) {
    size_t pos = str.find(delimiter);
    return pos != std::string::npos ? str.substr(0, pos) : str;
}

std::string BeforeLast(const std::string& file_path, char separator) {
    // Find the last occurrence of '.'
    std::size_t last_dot_pos = file_path.find_last_of(separator);

    // If there is a dot, and it is not at the beginning of the filename
    if (last_dot_pos != std::string::npos && last_dot_pos != 0) {
        // Extract the substring from the dot to the end of the string
        return file_path.substr(0, last_dot_pos);
    }
    // If the dot is not found, or is the first character, return an empty string
    return "";
}

std::wstring BeforeLast(const std::wstring& file_path, wchar_t separator) {
    // Find the last occurrence of '.'
    std::size_t last_dot_pos = file_path.find_last_of(separator);

    // If there is a dot, and it is not at the beginning of the filename
    if (last_dot_pos != std::wstring::npos && last_dot_pos != 0) {
        // Extract the substring from the dot to the end of the string
        return file_path.substr(0, last_dot_pos);
    }
    // If the dot is not found, or is the first character, return an empty string
    return L"";
}

std::string AfterLast(const std::string& file_path, char separator) {
    // Find the last occurrence of '.'
    std::size_t last_dot_pos = file_path.find_last_of(separator);

    // If there is a dot, and it is not at the beginning of the filename
    if (last_dot_pos != std::string::npos && last_dot_pos != 0) {
        // Extract the substring from the dot to the end of the string
        return file_path.substr(last_dot_pos + 1);
    }
    // If the dot is not found, or is the first character, return an empty string
    return "";
}

std::wstring AfterLast(const std::wstring& file_path, wchar_t separator) {
    // Find the last occurrence of '.'
    std::size_t last_dot_pos = file_path.find_last_of(separator);

    // If there is a dot, and it is not at the beginning of the filename
    if (last_dot_pos != std::wstring::npos && last_dot_pos != 0) {
        // Extract the substring from the dot to the end of the string
        return file_path.substr(last_dot_pos + 1);
    }
    // If the dot is not found, or is the first character, return an empty string
    return L"";
}

std::string GetFileExtension(const std::string& file_path) {
    // Find the last occurrence of '.'
    std::size_t last_dot_pos = file_path.find_last_of(".");

    // If there is a dot, and it is not at the beginning of the filename
    if (last_dot_pos != std::string::npos && last_dot_pos != 0) {
        // Extract the substring from the dot to the end of the string
        return file_path.substr(last_dot_pos + 1);
    }
    // If the dot is not found, or is the first character, return an empty string
    return "";
}

std::string WideStringToString(const std::wstring& wstr) {
    if (wstr.empty()) return std::string();

    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}


std::wstring StringToWString(const std::string& str) {
    if (str.empty()) return std::wstring();

    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), nullptr, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}