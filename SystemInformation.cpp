//
// Created by admin on 30/01/2024.
//

#include <sstream>
#include "SystemInformation.h"

std::string SystemInformationModel::getTimeAsString() const
{
    tm* timeinfo;
    timeinfo = localtime (&m_time);
    char tmBuffer[32];
    sprintf(tmBuffer, "%02d/%02d/%04d - %02d:%02d:%02d", timeinfo->tm_mday, timeinfo->tm_mon+1, timeinfo->tm_year+1900, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    return tmBuffer;
}

std::string SystemInformationModel::getKey() const
{
    char buffer[32];
    memset(buffer, 0, 32);
    sprintf(buffer, "%u", (unsigned int)m_time);
    return buffer;
}

/********************************************************
 * Class ScreenBrightness
 ********************************************************/
/*ScreenBrightnessInfo::ScreenBrightnessInfo()
: SystemInformationModel(kpi::SCREEN_BRIGHTNESS)
, m_minBrightnessLevel(0)
, m_maxBrightnessLevel(0)
, m_curBrightnessLevel(0)
{}*/

ScreenBrightnessInfo::ScreenBrightnessInfo(DWORD minBrightness, DWORD maxBrightness, DWORD curBrightness)
: SystemInformationModel(kpi::SCREEN_BRIGHTNESS)
, m_minBrightnessLevel(minBrightness)
, m_maxBrightnessLevel(maxBrightness)
, m_curBrightnessLevel(curBrightness)
{}

std::string ScreenBrightnessInfo::getDataAsString()
{
    char dBuffer[MAX_PATH];
    std::string tmeStr = SystemInformationModel::getTimeAsString();
    sprintf(dBuffer, "%s : min=%03d max=%03d current=%03d", tmeStr.c_str(), m_minBrightnessLevel, m_maxBrightnessLevel, m_curBrightnessLevel);
    return dBuffer;
}

bool ScreenBrightnessInfo::isEqual(SystemInformationModel* info)
{
    if (this == info)
        return true;
    auto other = dynamic_cast<ScreenBrightnessInfo*>(info);
    if (other == nullptr)
        return false;
    return m_minBrightnessLevel == other->m_minBrightnessLevel
           && m_maxBrightnessLevel == other->m_maxBrightnessLevel
           && m_curBrightnessLevel == other->m_curBrightnessLevel;
}

void ScreenBrightnessInfo::update(DWORD minBrightness, DWORD maxBrightness, DWORD curBrightness)
{
    m_minBrightnessLevel = minBrightness;
    m_maxBrightnessLevel = maxBrightness;
    m_curBrightnessLevel = curBrightness;
}

/*
 * Class PluggedDeviceInfo
 */
PluggedDeviceInfo::PluggedDeviceInfo()
: SystemInformationModel(kpi::UNUSED_PLUGGED_DEVICES)
, pc_time(0)
, pc_hours(0)
, pc_minutes(0)
, pc_seconds(0)
, pc_days(0)
{};

PluggedDeviceInfo::PluggedDeviceInfo(ULONGLONG pcTime)
: SystemInformationModel(kpi::UNUSED_PLUGGED_DEVICES)
, pc_time(pcTime)
, pc_hours(0)
, pc_minutes(0)
, pc_seconds(0)
, pc_days(0)
{
    reinit();
}

void PluggedDeviceInfo::reinit()
{
    pc_hours = pc_time / 3600;
    unsigned long rem = pc_time % 3600;
    pc_minutes = rem / 60;
    pc_seconds = rem % 60;
    pc_days = pc_hours / 24;
}

void PluggedDeviceInfo::setPcTime(ULONGLONG pcTime)
{
    pc_time = pcTime;
    reinit();
}

std::string PluggedDeviceInfo::getDataAsString()
{
    char dBuffer[MAX_PATH];
    std::string tmeStr = SystemInformationModel::getTimeAsString();
    sprintf(dBuffer, "%s : %d hour(s) %02d Minute(s) %02d Second(s) %d Day(s)", tmeStr.c_str(), pc_hours, pc_minutes, pc_seconds, pc_days);
    return dBuffer;
};

bool PluggedDeviceInfo::isEqual(SystemInformationModel* info)
{
    if (this == info)
        return true;
    auto other = dynamic_cast<PluggedDeviceInfo*>(info);
    return other && pc_time == other->pc_time;
}

/*
 * Class BrowsersInstancesInfos
 */

std::string BrowsersInstancesInfos::getDataAsString()
{
    std::ostringstream oss;
    std::string separator("");
    for (auto it : counters) {
        oss << separator << it.first << "=" << it.second;
        separator = ", ";
    }
    return oss.str();
}

bool BrowsersInstancesInfos::isEqual(SystemInformationModel* info)
{
    if (this == info)
        return true;
    auto other = dynamic_cast<BrowsersInstancesInfos*>(info);
    if (other == nullptr)
        return false;
    if (other->counters.size() != counters.size())
        return false;
    for (auto it : counters) {
        if (other->counters[it.first] != it.second)
            return false;
    }
    return true;
}

/*
 * Class EcogesteClipboardData
 */
ClipboardInfo::ClipboardInfo()
: SystemInformationModel(kpi::TRASH_EMPTIED)
, cbSeqNumber(0)
, dataLen(0)
{}

std::string ClipboardInfo::getDataAsString()
{
    char dBuffer[MAX_PATH];
    std::string tmeStr = SystemInformationModel::getTimeAsString();
    sprintf(dBuffer, "%s : size=%d", tmeStr.c_str(), dataLen);
    return dBuffer;
}

bool ClipboardInfo::isEqual(SystemInformationModel* info)
{
    if (this == info)
        return true;
    auto other = dynamic_cast<ClipboardInfo*>(info);
    return (other && other->dataLen == dataLen);
}

/*
 * Class BatteryPowerInfo
 */
BatteryPowerInfo::BatteryPowerInfo()
: SystemInformationModel(kpi::BATTERY_OPTIMIZATION)
, acLineStatus(0)
, batteryFlag(0)
, batteryLifePercent(0)
, systemStatusFlag(0)
, batteryLifeTime(0)
, batteryFullLifeTime(0)
{}

std::string BatteryPowerInfo::getDataAsString()
{
    char dBuffer[MAX_PATH];
    std::string tmeStr = SystemInformationModel::getTimeAsString();
    sprintf(dBuffer, "%s : ACL=%d, Flag=%d, LifePercent=%d, Status=%d, Lifetime%lu, FullLifetime%lu"
            , tmeStr.c_str()
            , acLineStatus
            , batteryFlag
            , batteryLifePercent
            , systemStatusFlag
            , batteryLifeTime
            , batteryFullLifeTime);
    return dBuffer;
}

bool BatteryPowerInfo::isEqual(SystemInformationModel* info)
{
    if (this == info)
        return true;
    auto other = dynamic_cast<BatteryPowerInfo*>(info);
    if (other == nullptr)
        return false;
    return acLineStatus == other->acLineStatus
           && batteryFlag == other->batteryFlag
           && batteryLifePercent == other->batteryLifePercent
           && systemStatusFlag == other->systemStatusFlag
           && batteryLifeTime == other->batteryLifeTime
           && batteryFullLifeTime == other->batteryFullLifeTime;
}
