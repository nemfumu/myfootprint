////////////////////////////////////////////////////////////////////////////////
//                              TASKS MANAGER                                 //
////////////////////////////////////////////////////////////////////////////////
//    DATE      PROG.                                                         //
// DD-MMM-YYYY INIT. SIR    MODIFICATION DESCRIPTION                          //
// ---------- ----- ------ -------------------------------------------------- //
// 07/02/2024  E.MF  Creation.                                                //
////////////////////////////////////////////////////////////////////////////////

#ifndef FOOTPRINTAGENT_TASKMANAGER_H
#define FOOTPRINTAGENT_TASKMANAGER_H

#include <cstring>
#include <list>
#include <map>

#include "Common.h"
#include "AppQueue.h"
#include "EcogesteKpi.h"
#include "ServiceConfig.h"
#include "ServiceTask.h"
#include "PersistanceManager.h"

class TaskManager
{
public:
    TaskManager();
    virtual ~TaskManager();

    void stop();

    AppQueue* getQueue() { return m_queue; }
    ServiceTask* getTask(kpi::Code code) { return m_tasksMap.at(code); }
    PersistanceManager* getPersistanceManager() { return m_persistanceMgr; }

    static DWORD WINAPI taskThread(void* lpParam);

    void initialize(ServiceCfg* pCfg);

private:
    void createTasks();

    std::map<kpi::Code, ServiceTask*> m_tasksMap;
    AppQueue* m_queue;
    HANDLE m_hThread;
    PersistanceManager* m_persistanceMgr;
};

#endif //FOOTPRINTAGENT_TASKMANAGER_H
