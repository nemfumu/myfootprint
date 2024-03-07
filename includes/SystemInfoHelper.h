////////////////////////////////////////////////////////////////////////////////
//                            INFORMATION HELPER                              //
////////////////////////////////////////////////////////////////////////////////
//    DATE      PROG.                                                         //
// DD-MMM-YYYY INIT. SIR    MODIFICATION DESCRIPTION                          //
// ---------- ----- ------ -------------------------------------------------- //
// 30/01/2024  E.MF  Creation.                                                //
////////////////////////////////////////////////////////////////////////////////

#ifndef FOOTPRINTAGENT_SYSTEMINFOHELPER_H
#define FOOTPRINTAGENT_SYSTEMINFOHELPER_H

#include "SystemInformation.h"

class SystemInfoHelper
{
public:
    static ScreenBrightnessInfo* collectScreenBrightness();
    static SystemInformationModel* collectExchangeInfos();
    static PluggedDeviceInfo* collectPluggedDeviceInfos();
    static BrowsersInstancesInfos* collectBrowsersInstancesInfos();
    static BatteryPowerInfo* collectBatteryPowerInfos();

private:
    explicit SystemInfoHelper();
};


#endif //FOOTPRINTAGENT_SYSTEMINFOHELPER_H
