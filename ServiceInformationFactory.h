////////////////////////////////////////////////////////////////////////////////
//                         INFORMATION FACTORIES                              //
////////////////////////////////////////////////////////////////////////////////
//    DATE      PROG.                                                         //
// DD-MMM-YYYY INIT. SIR    MODIFICATION DESCRIPTION                          //
// ---------- ----- ------ -------------------------------------------------- //
// 05/02/2024  E.MF  Creation.                                                //
////////////////////////////////////////////////////////////////////////////////

#ifndef FOOTPRINTAGENT_SERVICEINFORMATIONFACTORY_H
#define FOOTPRINTAGENT_SERVICEINFORMATIONFACTORY_H

#include "MessageData.h"
#include "PersistentData.h"

template<class T_Obj>
class ServiceInformationFactory
{
public:
    virtual ~ServiceInformationFactory()
    {};

    static T_Obj* create()
    {
        return new T_Obj();
    };

protected:
    ServiceInformationFactory();
    ServiceInformationFactory(ServiceInformationFactory* factory);
};

template<class P_Obj>
class PersistentModelFactory
{
public:
    virtual ~PersistentModelFactory() {}

    static P_Obj* create()
    {
        return new P_Obj();
    };

protected:
    PersistentModelFactory();
    PersistentModelFactory(PersistentModelFactory* factory);
};

/*
 * Class ServiceInformationFactoryHelper
 */
class ServiceInformationFactoryHelper
{
public:
    virtual ~ServiceInformationFactoryHelper() {};

    static SystemInformationModel* create(kpi::Code model);

private:
    ServiceInformationFactoryHelper();
    ServiceInformationFactoryHelper(ServiceInformationFactoryHelper* hlper);
};

/*
 * Class PersistentModelFactoryHelper
 */
class PersistentModelFactoryHelper
{
public:
    virtual ~PersistentModelFactoryHelper() {};

    static PersistentModelBase* create(kpi::Code kpiCode);
    static PersistentModelBase* create(SystemInformationModel* pSysInfo);

private:
    PersistentModelFactoryHelper();
    PersistentModelFactoryHelper(PersistentModelFactoryHelper* hlper);
};


#endif //FOOTPRINTAGENT_SERVICEINFORMATIONFACTORY_H
