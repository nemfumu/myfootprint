////////////////////////////////////////////////////////////////////////////////
//                       PERSISTENT OBJECT  - INTERFACE                       //
////////////////////////////////////////////////////////////////////////////////
//    DATE      PROG.                                                         //
// DD-MMM-YYYY INIT. SIR    MODIFICATION DESCRIPTION                          //
// ---------- ----- ------ -------------------------------------------------- //
// 08/02/2024  E.MF  Creation File                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef FOOTPRINTAGENT_PERSISTENT_OBJECT_H
#define FOOTPRINTAGENT_PERSISTENT_OBJECT_H

#include "ParserObject.h"
#include "MessageData.h"
#include "SystemInformation.h"

//============================================================================//
// CLASSE      :                  PersistentDataModel                         //
// DESCRIPTION : Classe definissant une zone memoire a Loguer.                //
//============================================================================//
class PersistentModelBase
{
public:
             PersistentModelBase() {};
    virtual ~PersistentModelBase()
    {};

    virtual std::string getDataAsString() = 0;
    virtual SystemInformationModel* createInstance(const char* pMemBuf) = 0;
};


template<class T_DATA>
class PersistentDataModel : public PersistentModelBase
{
public:
    PersistentDataModel(T_DATA* argData=nullptr)
    : PersistentModelBase()
    , m_data(argData)
    {};
    virtual ~PersistentDataModel()
    {
        m_data = nullptr;
    };

    T_DATA* getData() { return m_data; };
    void setData(T_DATA* pData) { m_data = pData; };

protected:
    T_DATA* m_data;
};


/*============================================================================
 * CLASSE      :          PersistentScreenBrightness                          *
 * DESCRIPTION : Classe definissant la zone memoire d'une donnee.             *
 *============================================================================*/
class PersistentScreenBrightness : public PersistentDataModel<ScreenBrightnessInfo>
{
public:
             PersistentScreenBrightness(ScreenBrightnessInfo* pInfo=nullptr);
    virtual ~PersistentScreenBrightness() {};

    virtual std::string getDataAsString();
    virtual SystemInformationModel* createInstance(const char* pMemBuf);
};

class PersistentBrowsersInstances : public PersistentDataModel<BrowsersInstancesInfos>
{
public:
    PersistentBrowsersInstances(BrowsersInstancesInfos* pInfo=nullptr);
    virtual ~PersistentBrowsersInstances() {};

    virtual std::string getDataAsString();
    virtual SystemInformationModel* createInstance(const char* pMemBuf);
};


/*============================================================================
 * CLASSE      :                      PersistentData                          *
 * DESCRIPTION : Classe definissant la zone memoire d'une donnee.             *
 *============================================================================*/
class PersistentPluggedDevice : public PersistentDataModel<PluggedDeviceInfo>
{
public:
             PersistentPluggedDevice(PluggedDeviceInfo* pInfo=nullptr);
    virtual ~PersistentPluggedDevice() {};

    virtual std::string getDataAsString();
    virtual SystemInformationModel* createInstance(const char* pMemBuf);
};

/*
 * Class PersistentClipboardData
 */
class PersistentClipboardData : public PersistentDataModel<ClipboardInfo>
{
public:
             PersistentClipboardData(ClipboardInfo* pInfo=nullptr);
    virtual ~PersistentClipboardData() {};

    virtual std::string getDataAsString();
    virtual SystemInformationModel* createInstance(const char* pMemBuf);
};

/*
 * Class PersistentBatteryPower
 */
class PersistentBatteryPower : public PersistentDataModel<BatteryPowerInfo>
{
public:
    explicit PersistentBatteryPower(BatteryPowerInfo* pInfo=nullptr);
    virtual ~PersistentBatteryPower() {};

    virtual std::string getDataAsString();
    virtual SystemInformationModel* createInstance(const char* pMemBuf);
};

#endif

