//
// Created by admin on 05/02/2024.
//

#include "../includes/ServiceInformationFactory.h"

SystemInformationModel* ServiceInformationFactoryHelper::create(kpi::Code kpiCode)
{
    SystemInformationModel* pInfo = nullptr;
    switch(kpiCode) {
        case kpi::SCREEN_BRIGHTNESS:
            pInfo = ServiceInformationFactory<ScreenBrightnessInfo>::create();
            break;
        case kpi::UNUSED_PLUGGED_DEVICES:
            pInfo = ServiceInformationFactory<PluggedDeviceInfo>::create();
            break;
        case kpi::TRASH_EMPTIED:
            pInfo = ServiceInformationFactory<ClipboardInfo>::create();
            break;
        case kpi::BATTERY_OPTIMIZATION:
            pInfo = ServiceInformationFactory<BatteryPowerInfo>::create();
            break;
        case kpi::BROWSERS_INSTANCES:
            pInfo = ServiceInformationFactory<BrowsersInstancesInfos>::create();
            break;
        default : break;
    }
    return pInfo;
}

PersistentModelBase* PersistentModelFactoryHelper::create(SystemInformationModel* pModel)
{
    switch(pModel->getKpiCode()) {
        case kpi::SCREEN_BRIGHTNESS: {
            PersistentScreenBrightness *pScrnInfo = PersistentModelFactory<PersistentScreenBrightness>::create();
            pScrnInfo->setData(dynamic_cast<ScreenBrightnessInfo*>(pModel));
            return pScrnInfo;
        } break;
        case kpi::TRASH_EMPTIED: {
            PersistentClipboardData* pClpInfo = PersistentModelFactory<PersistentClipboardData>::create();
            pClpInfo->setData(dynamic_cast<ClipboardInfo*>(pModel));
            return pClpInfo;
        } break;
        case kpi::UNUSED_PLUGGED_DEVICES: {
            PersistentPluggedDevice* pDeviceInfo = PersistentModelFactory<PersistentPluggedDevice>::create();
            pDeviceInfo->setData(dynamic_cast<PluggedDeviceInfo*>(pModel));
            return pDeviceInfo;
        } break;
        case kpi::BATTERY_OPTIMIZATION: {
            PersistentBatteryPower* pBatteryPower = PersistentModelFactory<PersistentBatteryPower>::create();
            pBatteryPower->setData(dynamic_cast<BatteryPowerInfo*>(pModel));
            return pBatteryPower;
        } break;
        case kpi::BROWSERS_INSTANCES: {
            PersistentBrowsersInstances* pBrowsersInfo = PersistentModelFactory<PersistentBrowsersInstances>::create();
            pBrowsersInfo->setData(dynamic_cast<BrowsersInstancesInfos*>(pModel));
            return pBrowsersInfo;
        }
        default : break;
    }
    return nullptr;
}

PersistentModelBase* PersistentModelFactoryHelper::create(kpi::Code kpiCode)
{
    PersistentModelBase* pInfo = nullptr;
    switch(kpiCode) {
        case kpi::SCREEN_BRIGHTNESS: {
            pInfo = PersistentModelFactory<PersistentScreenBrightness>::create();
        } break;
        case kpi::TRASH_EMPTIED: {
            pInfo = PersistentModelFactory<PersistentClipboardData>::create();
        } break;
        case kpi::UNUSED_PLUGGED_DEVICES: {
            pInfo = PersistentModelFactory<PersistentPluggedDevice>::create();
        } break;
        case kpi::BATTERY_OPTIMIZATION: {
            pInfo = PersistentModelFactory<PersistentBatteryPower>::create();
        } break;
        case kpi::BROWSERS_INSTANCES: {
            pInfo = PersistentModelFactory<PersistentBrowsersInstances>::create();
        } break;
        default : break;
    }
    return pInfo;
}
