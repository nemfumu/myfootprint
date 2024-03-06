//
// Created by admin on 30/01/2024.
//

#include "SystemInfoHelper.h"

#include <minwindef.h>
#include <processthreadsapi.h>

#include <windows.h>
#include <physicalmonitorenumerationapi.h>
#include <highlevelmonitorconfigurationapi.h>
#include <algorithm>

#include "SystemInformation.h"
#include "Common.h"
#include "ServiceInformationFactory.h"
#include "ServiceLogger.h"
#include <random>
#include <limits.h>
#include <fstream>
#include <regex>
#include <sstream>
#include <iterator>

#pragma comment(lib, "Dxva2.lib")

ScreenBrightnessInfo* SystemInfoHelper::collectScreenBrightness()
{
    HMONITOR hMonitor = NULL;
    DWORD cPhysicalMonitors;
    LPPHYSICAL_MONITOR pPhysicalMonitors = NULL;

    ScreenBrightnessInfo* screenBrightness = ServiceInformationFactory<ScreenBrightnessInfo>::create(); //ServiceInformationFactoryHelper::create(kpi::SCREEN_BRIGHTNESS);
    try {
        // Get the monitor handle.
        HWND hWnd = GetDesktopWindow(); // GetForegroundWindow(); //GetDesktopWindow();
        if (!hWnd) {
            screenBrightness->log(Common::getLastErrorAsString("GetDesktopWindow en échec"));
            return nullptr;
        }
        hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTOPRIMARY);
        if (!hMonitor)
            screenBrightness->log(Common::getLastErrorAsString("MonitorFromWindow en échec"));
        // Get the number of physical monitors.
        BOOL bSuccess = GetNumberOfPhysicalMonitorsFromHMONITOR(hMonitor, &cPhysicalMonitors);
        if (bSuccess) {
            // Allocate the array of PHYSICAL_MONITOR structures.
            pPhysicalMonitors = (LPPHYSICAL_MONITOR) malloc(cPhysicalMonitors * sizeof(PHYSICAL_MONITOR));
            if (pPhysicalMonitors != nullptr) {
                // Get the array.
                bSuccess = GetPhysicalMonitorsFromHMONITOR(hMonitor, cPhysicalMonitors, pPhysicalMonitors);
                if (bSuccess) {
                    // Use the monitor handles (not shown).
                    DWORD minBrightnessLevel = 0;
                    DWORD maxBrightnessLevel = 0;
                    DWORD curBrightnessLevel = 0;
                    HANDLE handle = pPhysicalMonitors[0].hPhysicalMonitor;
                    GetMonitorBrightness(handle, &minBrightnessLevel, &curBrightnessLevel, &maxBrightnessLevel);

                    screenBrightness->update(minBrightnessLevel, maxBrightnessLevel, curBrightnessLevel);
                    // Close the monitor handles.
                    DestroyPhysicalMonitors(cPhysicalMonitors, pPhysicalMonitors);
                }
                else
                    screenBrightness->log(Common::getLastErrorAsString("GetPhysicalMonitorsFromHMONITOR en échec"));
                // Free the array.
                free(pPhysicalMonitors);
            }
            else
                screenBrightness->log(Common::getLastErrorAsString("malloc en échec"));
        }
        else
            screenBrightness->log(Common::getLastErrorAsString("GetNumberOfPhysicalMonitorsFromHMONITOR en échec"));
    } catch(...) {
        screenBrightness->log(Common::getLastErrorAsString("SystemInfoHelper::collectScreenBrightness()"));
        return nullptr;
    }
    return screenBrightness;
}

SystemInformationModel* SystemInfoHelper::collectExchangeInfos()
{
    //TO_DO
    return nullptr;
}

PluggedDeviceInfo* SystemInfoHelper::collectPluggedDeviceInfos()
{
    //TO_DO
    PluggedDeviceInfo* pluggedInfo = ServiceInformationFactory<PluggedDeviceInfo>::create();
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<unsigned int> distributor(10000000, INT_MAX);
    pluggedInfo->setPcTime(distributor(mt));
    return pluggedInfo;
}

BrowsersInstancesInfos* SystemInfoHelper::collectBrowsersInstancesInfos()
{
    static std::vector<std::string> filters = { "chrome", "msedge", "explorer", "firefox" };
    static std::string tmpfile("tasksinfos.tmp");

    try {
        BrowsersInstancesInfos* pBIInfo = ServiceInformationFactory<BrowsersInstancesInfos>::create();
        for (int i = 0; i < filters.size(); i++)
            pBIInfo->counters[filters[i]] = 0;

        std::string syscmd("tasklist.exe /FO CSV > ");
        syscmd.append(tmpfile);
        int result = system(syscmd.c_str());
        std::ifstream myfile(tmpfile.c_str());
        if (!myfile.is_open()) {
            pBIInfo->log(Common::getLastErrorAsString("SystemInfoHelper::collectBrowsersInstancesInfos()"));
            return pBIInfo;
        }

        std::string line;
        while (std::getline(myfile, line)) {
            if (line.length() == 0)
                continue;

            const std::regex quote("[\"]");

            std::stringstream result;
            std::regex_replace(std::ostream_iterator<char>(result), line.begin(), line.end(), quote, "");

            std::string newline(result.str());
            size_t pos = newline.find_first_of(',');

            if (pos != std::string::npos) {
                std::string svcName = newline.substr(0, pos);
                pos = svcName.find(".exe");
                if (pos != std::string::npos) {
                    std::string name = svcName.substr(0, pos);
                    auto it = pBIInfo->counters.find(name);
                    if (it != pBIInfo->counters.end())
                        it->second++;
                }
            }
        }
        myfile.close();
        return pBIInfo;
    } catch(...) {
        ServiceLogger::getInstance()->write(Common::getLastErrorAsString("SystemInfoHelper::collectBrowsersInstancesInfos()"));
    }
    return nullptr;
}

BatteryPowerInfo* SystemInfoHelper::collectBatteryPowerInfos()
{
    BatteryPowerInfo* pBPInfo = ServiceInformationFactory<BatteryPowerInfo>::create();
    try {
        SYSTEM_POWER_STATUS systemPowerStatus;
        BOOL result = GetSystemPowerStatus((LPSYSTEM_POWER_STATUS) &systemPowerStatus);
        if (result) {
            pBPInfo->acLineStatus = systemPowerStatus.ACLineStatus;
            pBPInfo->batteryFlag = systemPowerStatus.BatteryFlag;
            pBPInfo->batteryLifePercent = systemPowerStatus.BatteryLifePercent;
            pBPInfo->systemStatusFlag = systemPowerStatus.SystemStatusFlag;
            pBPInfo->batteryLifeTime = systemPowerStatus.BatteryLifeTime;
            pBPInfo->batteryFullLifeTime = systemPowerStatus.BatteryFullLifeTime;
        }
    } catch(...) {
        ServiceLogger::getInstance()->write(Common::getLastErrorAsString("SystemInfoHelper::collectBatteryPowerInfos()"));
    }
    return pBPInfo;
}