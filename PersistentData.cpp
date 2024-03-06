//============================================================================//
////////////////////////////////////////////////////////////////////////////////
//                    PERSISTENT OBJECT  - IMPLEMENTATION                     //
////////////////////////////////////////////////////////////////////////////////
//    DATE      PROG.                                                         //
// DD-MMM-YYYY INIT. SIR    MODIFICATION DESCRIPTION                          //
// ---------- ----- ------ -------------------------------------------------- //
// 09/03/2004  E.MF  Creation File                                            //
////////////////////////////////////////////////////////////////////////////////
//============================================================================//

#include "PersistentData.h"

#include <sstream>
#include <iostream>
#include <cassert>

#include "Common.h"
#include "ServiceLogger.h"

/**************************************************************
 * Class PersistentScreenBrightness                           *
 **************************************************************/
PersistentScreenBrightness::PersistentScreenBrightness(ScreenBrightnessInfo* pInfo)
: PersistentDataModel<ScreenBrightnessInfo>(pInfo)
{}

std::string PersistentScreenBrightness::getDataAsString()
{
    try {
        std::ostringstream oss;
        ScreenBrightnessInfo* pData = getData();

        oss << pData->getTime()             << FIELD_SEPARATOR_CONST
            << pData->getKpiCode()          << FIELD_SEPARATOR_CONST
            << pData->m_minBrightnessLevel  << FIELD_SEPARATOR_CONST
            << pData->m_maxBrightnessLevel  << FIELD_SEPARATOR_CONST
            << pData->m_curBrightnessLevel;

        return oss.str();
    } catch(...) {
        ServiceLogger::getInstance()->write(Common::getLastErrorAsString("PersistentScreenBrightness::getDataAsString()"));
    };
    return "";
}

SystemInformationModel* PersistentScreenBrightness::createInstance(const char* pMemBuf)
{
    try {
        auto pData = new ScreenBrightnessInfo();
        IStringParser parser((char*)pMemBuf, (int)std::strlen(pMemBuf));
        int kpiCode;
        unsigned int atime;
        parser >> atime
               >> kpiCode
               >> pData->m_minBrightnessLevel
               >> pData->m_maxBrightnessLevel
               >> pData->m_curBrightnessLevel;
        pData->setTime((time_t)atime);
        assert((kpi::Code)kpiCode == pData->getModel());
        return pData;
    } catch(...) {
        ServiceLogger::getInstance()->write(Common::getLastErrorAsString("PersistentScreenBrightness::createInstance()"));
    }
    return nullptr;
}

/**************************************************************
 * Class PersistentBrowsersInstances                          *
 **************************************************************/
PersistentBrowsersInstances::PersistentBrowsersInstances(BrowsersInstancesInfos* pInfo)
        : PersistentDataModel<BrowsersInstancesInfos>(pInfo)
{}

std::string PersistentBrowsersInstances::getDataAsString()
{
    try {
        std::ostringstream oss;
        BrowsersInstancesInfos* pData = getData();

        oss << pData->getTime()     << FIELD_SEPARATOR_CONST
            << pData->getKpiCode()  << FIELD_SEPARATOR_CONST;

        for (auto it : pData->counters) {
            oss << it.first << ":" << it.second << FIELD_SEPARATOR_CONST;
        }
        return oss.str();
    } catch(...) {
        ServiceLogger::getInstance()->write(Common::getLastErrorAsString("PersistentScreenBrightness::getDataAsString"));
    }
    return "";
};

SystemInformationModel* PersistentBrowsersInstances::createInstance(const char* pMemBuf)
{
    try {
        auto pData = new BrowsersInstancesInfos();
        IStringParser parser((char*)pMemBuf, (int)std::strlen(pMemBuf));
        int kpiCode;
        unsigned int atime;
        parser >> atime
               >> kpiCode;

        char buffer[MAX_PATH];
        while(true) {
            memset(buffer, 0, MAX_PATH);
            parser >> buffer;
            std::string keyValue(buffer);
            size_t pos = keyValue.find(':');
            if (std::strlen(buffer) == 0 || pos == std::string::npos)
                break;
            size_t count = std::strtol(keyValue.substr(pos+1).c_str(), nullptr, 10);
            pData->counters.insert(std::pair<std::string, int>(keyValue.substr(0, pos), count));
        }
        pData->setTime((time_t)atime);
        assert((kpi::Code)kpiCode == pData->getModel());
        return pData;
    } catch(...) {
        ServiceLogger::getInstance()->write(Common::getLastErrorAsString("PersistentBrowsersInstances::createInstance()"));
    }
    return nullptr;
}

/**************************************************************
 * Class PersistentPluggedDevice                              *
 **************************************************************/
PersistentPluggedDevice::PersistentPluggedDevice(PluggedDeviceInfo* pInfo)
: PersistentDataModel<PluggedDeviceInfo>(pInfo)
{}

std::string PersistentPluggedDevice::getDataAsString()
{
   try {
      std::ostringstream oss;
      PluggedDeviceInfo* pData = getData();

      oss << pData->getTime()      << FIELD_SEPARATOR_CONST
          << pData->getKpiCode()   << FIELD_SEPARATOR_CONST
          << pData->pc_time        << FIELD_SEPARATOR_CONST
          << pData->pc_hours       << FIELD_SEPARATOR_CONST
          << pData->pc_minutes     << FIELD_SEPARATOR_CONST
          << pData->pc_seconds     << FIELD_SEPARATOR_CONST
          << pData->pc_days;

      return oss.str();
   } catch(...) {
       std::cout << Common::getLastErrorAsString("PersistentPluggedDevice::getDataAsString()") << std::endl;
   };
   return "";
}

SystemInformationModel* PersistentPluggedDevice::createInstance(const char* pMemBuf)
{
    try {
        auto pData = new PluggedDeviceInfo();
        IStringParser parser((char*)pMemBuf, (int)std::strlen(pMemBuf));
        int kpiCode;
        unsigned int atime;
        parser >> atime
               >> kpiCode
               >> pData->pc_time
               >> pData->pc_hours
               >> pData->pc_minutes
               >> pData->pc_seconds
               >> pData->pc_days;
        pData->setTime((time_t)atime);
        assert((kpi::Code)kpiCode == pData->getModel());
        return pData;
    } catch(...) {
        ServiceLogger::getInstance()->write(Common::getLastErrorAsString("PersistentPluggedDevice::createInstance()"));
    }
    return nullptr;
}

/**************************************************************
 * Class PersistentClipboardData                              *
 **************************************************************/
PersistentClipboardData::PersistentClipboardData(ClipboardInfo* pInfo)
: PersistentDataModel<ClipboardInfo>(pInfo)
{}

std::string PersistentClipboardData::getDataAsString()
{
    try {
        std::ostringstream oss;
        ClipboardInfo* pData = getData();

        oss << pData->getTime()     << FIELD_SEPARATOR_CONST
            << pData->getModel()    << FIELD_SEPARATOR_CONST
            << pData->cbSeqNumber   << FIELD_SEPARATOR_CONST
            << pData->dataLen       << FIELD_SEPARATOR_CONST;

        return oss.str();
    } catch(...) {
        ServiceLogger::getInstance()->write(Common::getLastErrorAsString("PersistentClipboardData::getDataAsString()"));
    };
    return "";
}

SystemInformationModel* PersistentClipboardData::createInstance(const char* pMemBuf)
{
    try {
        auto pData = new ClipboardInfo();
        IStringParser parser((char*)pMemBuf, std::strlen(pMemBuf));
        int kpiCode;
        unsigned int atime;
        parser >> atime
               >> kpiCode
               >> pData->cbSeqNumber
               >> pData->dataLen;
        pData->setTime((time_t)atime);
        assert((kpi::Code)kpiCode == pData->getModel());
        return pData;
    } catch(...) {
        ServiceLogger::getInstance()->write(Common::getLastErrorAsString("PersistentClipboardData::createInstance()"));
    }
    return nullptr;
}

/**************************************************************
 * Class PersistentBatteryPower                               *
 **************************************************************/
PersistentBatteryPower::PersistentBatteryPower(BatteryPowerInfo* pInfo)
: PersistentDataModel<BatteryPowerInfo>(pInfo)
{}

std::string PersistentBatteryPower::getDataAsString()
{
    try {
        std::ostringstream oss;
        BatteryPowerInfo* pData = getData();

        oss << pData->getTime()             << FIELD_SEPARATOR_CONST
            << pData->getModel()            << FIELD_SEPARATOR_CONST
            << pData->acLineStatus          << FIELD_SEPARATOR_CONST
            << pData->batteryFlag           << FIELD_SEPARATOR_CONST
            << pData->batteryLifePercent    << FIELD_SEPARATOR_CONST
            << pData->systemStatusFlag      << FIELD_SEPARATOR_CONST
            << pData->batteryLifeTime       << FIELD_SEPARATOR_CONST
            << pData->batteryFullLifeTime   << FIELD_SEPARATOR_CONST;
        return oss.str();
    } catch(...) {
        ServiceLogger::getInstance()->write(Common::getLastErrorAsString("PersistentBatteryPower::getDataAsString()"));
    };
    return "";
}

SystemInformationModel* PersistentBatteryPower::createInstance(const char* pMemBuf)
{
    try {
        auto pData = new BatteryPowerInfo();
        IStringParser parser((char*)pMemBuf, std::strlen(pMemBuf));
        int kpiCode;
        unsigned int atime;
        parser >> atime
               >> kpiCode
               >> pData->acLineStatus
               >> pData->batteryFlag
               >> pData->batteryLifePercent
               >> pData->systemStatusFlag
               >> pData->batteryLifeTime
               >> pData->batteryFullLifeTime;
        pData->setTime((time_t)atime);
        assert((kpi::Code)kpiCode == pData->getModel());
        return pData;
    } catch(...) {
        ServiceLogger::getInstance()->write(Common::getLastErrorAsString("PersistentBatteryPower::createInstance()"));
    }
    return nullptr;
}
