/*
// Created by admin on 07/02/2024.
*/

#include "ServiceTask.h"

#include <sstream>
#include <iostream>
#include <string>

#include "SystemInfoHelper.h"
#include "ServiceLogger.h"

ServiceTask::ServiceTask(kpi::Code code, AppQueue* pMgrQueue)
: m_code(code)
, m_queue(new AppQueue())
, m_status(RUN_PENDING)
, m_hThread(CreateThread(nullptr, 0, ServiceTask::runThread, this, 0, nullptr))
, m_MgrQueue(pMgrQueue)
, m_lastInfo(nullptr)
{}

ServiceTask::~ServiceTask()
{
    stop();
    while (!isTerminate())
        Sleep(1000);            // wait 1s
    delete m_queue;
    CloseHandle(m_hThread);
    m_MgrQueue = nullptr;
    if (m_lastInfo)
        delete m_lastInfo;
}

void ServiceTask::run()
{
    auto pMsg = new IMessage(nullptr, AGENT_TASK_MESSAGE);
    m_queue->post(pMsg);
}

void ServiceTask::stop()
{
    auto pMsg = new IMessage(nullptr, AGENT_THREAD_TERMINATE);
    m_queue->post(pMsg);
}

void ServiceTask::onDataAvailable(SystemInformationModel* pInfo)
{
    // Signal only info not equal to last info
    if (!m_lastInfo || !m_lastInfo->isEqual(pInfo)) {
        m_MgrQueue->post(new IMessage(pInfo, AGENT_RAW_DATA));
        if (m_lastInfo)
            delete m_lastInfo;
        m_lastInfo = pInfo;
    } else
        delete pInfo;
}

DWORD WINAPI ServiceTask::runThread(void* lparam)
{
    auto myTask = static_cast<ServiceTask*>(lparam);

    char buffer[MAX_PATH];
    sprintf(buffer, "ServiceTask::runThread(%s) : running !\n", kpi::kpiTable[myTask->getKpiCode()].name);
    ServiceLogger::getInstance()->write(buffer);

    while (true) {
        myTask->setStatus(RUNNING);
        try {
            IMessage *pMsg = myTask->getQueue()->pull();
            if (pMsg->getMessageId() == AGENT_THREAD_TERMINATE) {
                myTask->setStatus(STOP_PENDING);
                delete pMsg;
                break;
            }
            myTask->setStatus(COLLECT_PENDING);
            #ifdef _DEBUG
            sprintf(buffer, "ServiceTask::runThread(%s) : collecting data...\n", kpi::kpiTable[myTask->getKpiCode()].name);
            ServiceLogger::getInstance()->write(buffer);
            #endif
            SystemInformationModel *pInfo = nullptr;
            switch (myTask->getKpiCode()) {
                case kpi::SCREEN_BRIGHTNESS:
                    pInfo = SystemInfoHelper::collectScreenBrightness();
                    break;
                case kpi::EXCHANGED_DATA:
                    pInfo = SystemInfoHelper::collectExchangeInfos();
                    break;
                case kpi::BROWSERS_INSTANCES:
                    pInfo = SystemInfoHelper::collectBrowsersInstancesInfos();
                    break;
                case kpi::BATTERY_OPTIMIZATION:
                    pInfo = SystemInfoHelper::collectBatteryPowerInfos();
                    break;
                default:
                    break;
            }
            if (pInfo)
                myTask->onDataAvailable(pInfo);
            myTask->setStatus(COLLECT_AVAILABLE);
            #ifdef _DEBUG
            sprintf(buffer, "ServiceTask::runThread(%s) : data collected !\n", kpi::kpiTable[myTask->getKpiCode()].name);
            ServiceLogger::getInstance()->write(buffer);
            #endif
            delete pMsg;
        } catch(...) {
            ServiceLogger::getInstance()->write(Common::getLastErrorAsString("ServiceTask::runThread"));
        }
    }
    myTask->setStatus(TERMINATE);
    sprintf(buffer, "ServiceTask::runThread(%s) : stpped !\n", kpi::kpiTable[myTask->getKpiCode()].name);
    ServiceLogger::getInstance()->write(buffer);
    return EXIT_SUCCESS;
}
