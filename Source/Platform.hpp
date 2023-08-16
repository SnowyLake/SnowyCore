#pragma once

// TODO: Multi Platform Support
#if defined(_WIN32)
#include <Windows.h>
#else
#error "Platform is not support!"
#endif // defined(_WIN32)


namespace Snowy
{
class Platform
{
public:
    static char* WideStringToAnsi(const wchar_t* wStr, const int wSize)
    {
    #if defined(_WIN32)
        // 获取缓冲区大小，并申请空间，缓冲区大小按字节计算
        int aSize = WideCharToMultiByte(CP_ACP, 0, wStr, wSize, NULL, 0, NULL, NULL);
        char* buffer = new char[aSize + 1];
        // 宽字节编码转换成多字节编码
        WideCharToMultiByte(CP_ACP, 0, wStr, wSize, buffer, aSize, NULL, NULL);
        buffer[aSize] = '\0';
        return buffer;
    #else
        #error "Platform is not support!"
    #endif 
    }
    static wchar_t* AnsiStringToWide(const char* aStr, const int aSize)
    {
    #if defined(_WIN32)
        // 获取缓冲区大小，并申请空间，缓冲区大小按字节计算
        int wSize = MultiByteToWideChar(CP_ACP, 0, aStr, aSize, NULL, 0);
        wchar_t* buffer = new wchar_t[wSize + 1];
        // 多字节编码转换成宽字节编码
        MultiByteToWideChar(CP_ACP, 0, aStr, aSize, buffer, wSize);
        buffer[wSize] = '\0';
        return buffer;
    #else
        #error "Platform is not support!"
    #endif 
    }
};


}