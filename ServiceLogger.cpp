//
// Created by admin on 07/02/2024.
//

#include "ServiceLogger.h"

#include <windows.h>
#include <cassert>
#include <intsafe.h>
#include <iostream>
#include <sstream>

std::mutex ServiceLogger::m_mutex = std::mutex();
ServiceLogger* ServiceLogger::m_instance = nullptr;

ServiceLogger* ServiceLogger::getInstance()
{
    if (m_instance == nullptr) {
        m_mutex.lock();
        if (m_instance == nullptr)
            m_instance = new ServiceLogger();
        m_mutex.unlock();
    }
    return m_instance;
}

ServiceLogger::ServiceLogger()
: m_filename()
, m_handle(INVALID_HANDLE_VALUE)
{}

ServiceLogger::~ServiceLogger()
{
    if (m_handle != INVALID_HANDLE_VALUE)
        CloseHandle(m_handle);
}

void WINAPI ServiceLogger::initialize(ServiceCfg* pCfg)
{
    m_filename = pCfg->getLogPath() + "\\" + pCfg->getLogName();
    std::cout << "Log file : " << m_filename << std::endl;

    m_handle = CreateFile((LPTSTR)m_filename.c_str(),   // name of the write
                           FILE_APPEND_DATA,            // open for writing
                           FILE_SHARE_READ,             // do not share
                           nullptr,                     // default security
                           OPEN_ALWAYS,                 // create new file or open existing
                           FILE_ATTRIBUTE_NORMAL,       // normal file
                           nullptr);                    // no attr. template
}

std::string ServiceLogger::getTime()
{
    auto start = std::chrono::system_clock::now();
    std::time_t stime = std::chrono::system_clock::to_time_t(start);
    tm* timeinfo;
    timeinfo = localtime (&stime);
    char tmBuffer[MAX_PATH];
    memset(tmBuffer, 0, MAX_PATH);
    sprintf(tmBuffer, "%02d/%02d/%04d - %02d:%02d:%02d", timeinfo->tm_mday, timeinfo->tm_mon+1, timeinfo->tm_year+1900, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    return tmBuffer;
}

bool WINAPI ServiceLogger::write(const std::string& msgLog)
{
    if (m_handle == INVALID_HANDLE_VALUE)
        return FALSE;

    DWORD dwBytesWritten;
    DWORD dwBytesToWrite;

    std::string msgBuffer(getTime());
    msgBuffer.append(" : ").append(msgLog.c_str());

    SizeTToDWord(msgBuffer.length(), &dwBytesToWrite);
    bool bErrorFlag = WriteFile(m_handle,               // logs file handle
                                msgBuffer.c_str(),      // start of data to write
                                dwBytesToWrite,         // number of bytes to write
                                &dwBytesWritten,        // number of bytes that were written
                                nullptr);
    return bErrorFlag;
}