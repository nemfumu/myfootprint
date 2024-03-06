////////////////////////////////////////////////////////////////////////////////
//                            INFORMATION CLASSES                             //
////////////////////////////////////////////////////////////////////////////////
//    DATE      PROG.                                                         //
// DD-MMM-YYYY INIT. SIR    MODIFICATION DESCRIPTION                          //
// ---------- ----- ------ -------------------------------------------------- //
// 30/01/2024  E.MF  Creation.                                                //
////////////////////////////////////////////////////////////////////////////////

#ifndef FOOTPRINTAGENT_SYSTEMINFORMATION_H
#define FOOTPRINTAGENT_SYSTEMINFORMATION_H

#include <ctime>
#include <string>
#include <windows.h>
#include <list>
#include <map>

#include "EcogesteKpi.h"
#include "MessageData.h"

struct SystemInformationModel : public MessageData
{
public:
    explicit SystemInformationModel(kpi::Code code) : MessageData(code), m_time()
    {
        time(&m_time);
    }
    virtual ~SystemInformationModel()
    {}

    kpi::Code getModel() const { return MessageData::getKpiCode(); }
    time_t getTime() const { return m_time; };
    void setTime(time_t atime) { m_time = atime; }

    std::string getKey() const;
    std::string getTimeAsString() const;

    virtual std::string getDataAsString() = 0;
    virtual bool isEqual(SystemInformationModel* info) = 0;

    std::list<std::string>& getLogs() { return m_logs; }
    void log(const std::string& msg) {
        m_logs.push_back(msg);
    }

    // properties
    time_t m_time;
    std::list<std::string> m_logs;
};

struct ScreenBrightnessInfo : public SystemInformationModel
{
    explicit ScreenBrightnessInfo(DWORD minBrightness=0, DWORD maxBrightness=0, DWORD curBrightness=0);
    virtual ~ScreenBrightnessInfo()
    {};

    virtual std::string getDataAsString();
    virtual bool isEqual(SystemInformationModel* info);
    void update(DWORD minBrightness, DWORD maxBrightness, DWORD curBrightness);

    //Properties
    DWORD m_minBrightnessLevel;
    DWORD m_maxBrightnessLevel;
    DWORD m_curBrightnessLevel;
};

/*
 * Class EcogesteRuntimeData
 */
struct PluggedDeviceInfo : public SystemInformationModel
{
    explicit PluggedDeviceInfo();
    explicit PluggedDeviceInfo(ULONGLONG pcTime);
    virtual ~PluggedDeviceInfo()
    {}

    virtual std::string getDataAsString();
    virtual bool isEqual(SystemInformationModel* info);

    void setPcTime(ULONGLONG pctime);

    void reinit();

    // Properties
    ULONGLONG pc_time;
    int pc_hours;
    int pc_minutes;
    int pc_seconds;
    int pc_days;
};

/*
 * Class BrowsersInstancesInfos
 */
struct BrowsersInstancesInfos : public SystemInformationModel
{
    explicit BrowsersInstancesInfos() : SystemInformationModel(kpi::BROWSERS_INSTANCES), counters() {};
    virtual ~BrowsersInstancesInfos()
    {}

    virtual std::string getDataAsString();
    virtual bool isEqual(SystemInformationModel* info);

    // Properties
    std::map<std::string, int> counters;
};

/*
 * Class EcogesteClipboardData
 */
struct ClipboardInfo : public SystemInformationModel
{
    explicit ClipboardInfo();
    virtual ~ClipboardInfo()
    {}

    virtual std::string getDataAsString();
    virtual bool isEqual(SystemInformationModel* info);

    // Properties
    int cbSeqNumber;
    int dataLen;
};

/*
 * Battery power
 */
struct BatteryPowerInfo : public SystemInformationModel
{
    explicit BatteryPowerInfo();
    virtual ~BatteryPowerInfo() {};

    virtual std::string getDataAsString();
    virtual bool isEqual(SystemInformationModel* info);

    int acLineStatus;
    int batteryFlag;
    int batteryLifePercent;
    int systemStatusFlag;
    unsigned long batteryLifeTime;
    unsigned long batteryFullLifeTime;
};
#endif //FOOTPRINTAGENT_SYSTEMINFORMATION_H
