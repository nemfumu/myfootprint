//
// Created by admin on 05/02/2024.
//

#include "../includes/Common.h"

#include <windows.h>
#include <minwindef.h>
#include <algorithm>

std::string Common::getLastErrorAsString(const char* text, int dwError)
{
    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = (dwError != -1) ? dwError : GetLastError();

    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                  NULL,
                  dw,
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                  (LPTSTR) &lpMsgBuf,
                  0,
                  NULL );
    char buffer[255];
    sprintf(buffer, "%s : %s\n", text, (char*)lpMsgBuf);
    return buffer;
};

void Common::safeCopy(char* argDest, char* argSrc, int argDestLen)
{
    memset(argDest, 0, argDestLen);
    size_t nSize = std::min((int)argDestLen-1, (int)strlen(argSrc));
    strncpy(argDest, argSrc, nSize);
};