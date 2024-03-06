////////////////////////////////////////////////////////////////////////////////
//                              SERVICE TIMER                                 //
////////////////////////////////////////////////////////////////////////////////
//    DATE      PROG.                                                         //
// DD-MMM-YYYY INIT. SIR    MODIFICATION DESCRIPTION                          //
// ---------- ----- ------ -------------------------------------------------- //
// 07/02/2024  E.MF  Creation.                                                //
////////////////////////////////////////////////////////////////////////////////

#ifndef FOOTPRINTAGENT_SERVICETIMER_H
#define FOOTPRINTAGENT_SERVICETIMER_H

#include <minwindef.h>
#include <cstring>

#include "AppQueue.h"
#include "EcogesteKpi.h"

/*
 * Do not work : callback never activate !!??
class ServiceTimer
{
public:
             ServiceTimer(kpi::Code kpiCode, int interval, AppQueue* pQueue);
    virtual ~ServiceTimer();

    kpi::Code getKpiCode() const { return m_kpiCode; }
    AppQueue* getQueue() { return m_queue; }

    void stop();
    static VOID WINAPI TimerProc(LPVOID, DWORD, DWORD);
    static BOOL WINAPI cslHandler(DWORD);


private:
    bool setTimer(int elapse);

    HANDLE m_hTimer;
    kpi::Code m_kpiCode;
    int m_interval;
    int m_oldInterval;
    AppQueue* m_queue;
    volatile static LONG exitFlag;
}
*/

class ServiceTimer
{
public:
    ServiceTimer(kpi::Code kpiCode, int interval, AppQueue* pQueue);
    virtual ~ServiceTimer();

    HANDLE getHandle() { return m_hTimer; }
    kpi::Code getKpiCode() const { return m_kpiCode; }
    AppQueue* getQueue() { return m_queue; }
    LONG getTimeout() const { return m_timeout; }

    void stop() { m_queryStop = true; };
    bool isStop() const { return m_queryStop; }

    static DWORD WINAPI timerHandler(LPVOID);

private:
    HANDLE m_hTimer;
    kpi::Code m_kpiCode;
    int m_interval;
    LONG m_timeout;
    bool m_queryStop;
    AppQueue* m_queue;
};

#endif //FOOTPRINTAGENT_SERVICETIMER_H
