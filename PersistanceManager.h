////////////////////////////////////////////////////////////////////////////////
//                           PERSISTANCE - CLASS                              //
////////////////////////////////////////////////////////////////////////////////
//    DATE      PROG.                                                         //
// DD-MMM-YYYY INIT. SIR    MODIFICATION DESCRIPTION                          //
// ---------- ----- ------ -------------------------------------------------- //
// 08/02/2024  E.MF  Creation File                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef FOOTPRINTAGENT_PERSISTANCEMANAGER_H
#define FOOTPRINTAGENT_PERSISTANCEMANAGER_H

#include "DataStore.h"
#include "AppQueue.h"
#include "ServiceConfig.h"

class PersistanceManager
{
public:
    PersistanceManager();
    virtual ~PersistanceManager();

    void initialize(ServiceCfg* pCfg);

    AppQueue* getQueue() { return m_queue; }
    void stop();

    std::list<SystemInformationModel*>* loadData(kpi::Code kpiCode);
    std::map<std::string, std::list<SystemInformationModel*>*>* loadData();

    static void initDb(std::string dbPathName, std::list<std::string>& listStorages);

    std::list<std::string>* selectStorages();
    std::list<std::string>* loadStreams(const std::string& stgName);

private:
    static DWORD WINAPI storeThread(void* lparam);
    void saveData(SystemInformationModel* pData);

    DataStore* m_dataStore;     // Data Storage Interface
    AppQueue* m_queue;
    HANDLE m_hThread;
};

#endif //FOOTPRINTAGENT_PERSISTANCEMANAGER_H
