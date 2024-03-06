//
// Created by admin on 07/02/2024.
//

#include "ServiceTimer.h"
#include "ServiceLogger.h"
/*
volatile LONG ServiceTimer::exitFlag = 0;

ServiceTimer::ServiceTimer(kpi::Code kpiCode, int interval, AppQueue* pQueue)
: m_hTimer(INVALID_HANDLE_VALUE)
, m_kpiCode(kpiCode)
, m_interval(interval)
, m_oldInterval()
, m_queue(pQueue)
{
    m_hTimer = CreateWaitableTimer(NULL, FALSE, NULL);
    if (m_hTimer) {
        if (setTimer(interval)) {
            char buffer[MAX_PATH];
            sprintf(buffer, "ServiceTimer::setTimer(%s) : created with interval %d second(s)!\n", kpi::kpiTable[getKpiCode()].name, m_interval);
            ServiceLogger::getInstance()->write(buffer);
        } else
            ServiceLogger::getInstance()->write(Common::getLastErrorAsString("ServiceTimer::SetWaitableTimer()"));
    } else
        ServiceLogger::getInstance()->write(Common::getLastErrorAsString("ServiceTimer::CreateWaitableTimer()"));
}

ServiceTimer::~ServiceTimer()
{
    if (m_hTimer != INVALID_HANDLE_VALUE)
        CloseHandle(m_hTimer);
}

void ServiceTimer::stop()
{
    CloseHandle(m_hTimer);
    m_hTimer = INVALID_HANDLE_VALUE;
}

bool ServiceTimer::setTimer(int elapse)
{
    m_oldInterval = m_interval;
    LARGE_INTEGER _liDueTime;
    _liDueTime.QuadPart = - (__int64)elapse * 10000;
    return SetWaitableTimer(m_hTimer,
                            &_liDueTime,
                            m_interval,
                            (PTIMERAPCROUTINE)TimerProc,
                            (LPVOID)this,
                            FALSE));
}

VOID CALLBACK ServiceTimer::TimerProc(LPVOID p, DWORD l, DWORD h )
{
    auto myTimer = (ServiceTimer*)p;
    if ( myTimer->m_interval > myTimer->m_oldInterval ) {
        myTimer->setTimer(myTimer->m_interval - myTimer->m_oldInterval);
        return;
    }
    auto pData = new MessageData(myTimer->getKpiCode());
    auto pMsg = new IMessage(pData, AGENT_TICK_MESSAGE);
    myTimer->getQueue()->post(pMsg);

    //char buffer[MAX_PATH];
    //sprintf(buffer, "ServiceTimer::TimerProc(%s) : new tick message !\n", kpi::kpiTable[myTimer->getKpiCode()].name);
    //ServiceLogger::getInstance()->write(buffer);

    if (myTimer->m_interval != myTimer->m_oldInterval ) {
        myTimer->setTimer(myTimer->m_interval );
    }
}

BOOL WINAPI ServiceTimer::cslHandler(DWORD cntrlEvent)
{
    InterlockedIncrement(&exitFlag);
    ServiceLogger::getInstance()->write("ServiceTimer::cslHandler() - Shutting down");
    return TRUE;
}
*/

ServiceTimer::ServiceTimer(kpi::Code kpiCode, int interval, AppQueue* pQueue)
: m_kpiCode(kpiCode)
, m_interval(interval)
, m_timeout(interval*1000)
, m_queryStop(false)
, m_queue(pQueue)
, m_hTimer(CreateThread(nullptr, 0, ServiceTimer::timerHandler, this, 0, nullptr))
{
    char buffer[MAX_PATH];
    sprintf(buffer, "ServiceTimer::setTimer(%s) : created with interval %d second(s)!\n", kpi::kpiTable[getKpiCode()].name, m_interval);
    ServiceLogger::getInstance()->write(buffer);
}

ServiceTimer::~ServiceTimer()
{
    if (m_hTimer != INVALID_HANDLE_VALUE)
        CloseHandle(m_hTimer);
}


DWORD WINAPI ServiceTimer::timerHandler(LPVOID lpParam)
{
    char buffer[MAX_PATH];
    auto myTimer = (ServiceTimer*)lpParam;
    sprintf(buffer, "ServiceTimer::timerHandler(%s) : running... !\n", kpi::kpiTable[myTimer->getKpiCode()].name);
    while (true) {
        Sleep(myTimer->getTimeout());
        if (myTimer->isStop())
            break;

        auto pData = new MessageData(myTimer->getKpiCode());
        auto pMsg = new IMessage(pData, AGENT_TICK_MESSAGE);
        myTimer->getQueue()->post(pMsg);

        //sprintf(buffer, "ServiceTimer::TimerProc(%s) : new tick message !\n", kpi::kpiTable[myTimer->getKpiCode()].name);
        //ServiceLogger::getInstance()->write(buffer);
    }
    sprintf(buffer, "ServiceTimer::timerHandler(%s) : stopped !\n", kpi::kpiTable[myTimer->getKpiCode()].name);
    return EXIT_SUCCESS;
}
