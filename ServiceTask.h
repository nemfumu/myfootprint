////////////////////////////////////////////////////////////////////////////////
//                              SERVICE TASK                                  //
////////////////////////////////////////////////////////////////////////////////
//    DATE      PROG.                                                         //
// DD-MMM-YYYY INIT. SIR    MODIFICATION DESCRIPTION                          //
// ---------- ----- ------ -------------------------------------------------- //
// 07/02/2024  E.MF  Creation.                                                //
////////////////////////////////////////////////////////////////////////////////

#ifndef FOOTPRINTAGENT_SERVICETASK_H
#define FOOTPRINTAGENT_SERVICETASK_H


#include <windows.h>
#include <mstask.h>

#include "Common.h"
#include "EcogesteKpi.h"
#include "SystemInformation.h"
#include "AppQueue.h"
#include "ResourceLock.h"

enum TaskStatus {
    RUN_PENDING=0,
    RUNNING,
    STOP_PENDING,
    TERMINATE,
    COLLECT_PENDING,
    COLLECT_AVAILABLE
};

class ServiceTask
{
public:
    explicit ServiceTask(kpi::Code code, AppQueue* pQueue);
    virtual ~ServiceTask();

    kpi::Code getKpiCode() const { return m_code; }
    AppQueue* getQueue() const { return m_queue; }

    void setStatus(TaskStatus state) {
        m_status = state;
    }

    bool isTerminate() const { return m_status == TERMINATE; };

    void onDataAvailable(SystemInformationModel* pInfo);

    void run();
    void stop();

private:
    static DWORD WINAPI runThread(void* lparam);

    kpi::Code m_code;
    AppQueue* m_queue;
    TaskStatus m_status;
    HANDLE m_hThread;
    AppQueue* m_MgrQueue;
    SystemInformationModel* m_lastInfo;
};


#endif //FOOTPRINTAGENT_SERVICETASK_H
