//
// Created by admin on 08/02/2024.
//

#include <processenv.h>
#include <libloaderapi.h>
#include <iostream>
#include "../includes/ServiceConfig.h"

#define DEFAULT_PATH "D:\\Ecogestes\\FootPrintAgent\\cmake-build-debug"
#define DEFAULT_NAME "FootPrintAgent"

#define DEFAULT_PORT "19001"

ServiceCfg::ServiceCfg()
: m_dbPath(DEFAULT_PATH)
, m_dbName(DEFAULT_NAME)
, m_logPath(DEFAULT_PATH)
, m_logName(DEFAULT_NAME)
, m_serverPort(DEFAULT_PORT)
{
    TCHAR szFileName[MAX_PATH];
    GetModuleFileName(nullptr, szFileName, MAX_PATH);

    std::string svcFile;
    #ifndef UNICODE
        svcFile = szFileName;
    #else
        std::wstring wStr = szFileName;
        svcFile = std::string(wStr.begin(), wStr.end());
    #endif
    size_t deb = svcFile.find_last_of('\\');
    size_t fin = svcFile.find_last_of('.');

    if (deb != std::string::npos && fin != std::string::npos && fin > deb) {
        std::string modulePath = svcFile.substr(0, deb);
        std::string moduleName = svcFile.substr(deb + 1, fin-deb-1);
        m_dbPath = m_logPath = modulePath;
        m_dbName = m_logName = moduleName;
    }
    m_dbName += ".dat";
    m_logName += ".log";
}