//
// Created by admin on 07/02/2024.
//

#include "TaskManager.h"
#include "SystemInformation.h"
#include "ServiceLogger.h"

#include <cstdlib>

TaskManager::TaskManager()
: m_tasksMap()
, m_queue(new AppQueue())
, m_hThread(INVALID_HANDLE_VALUE)
, m_persistanceMgr(new PersistanceManager())
{}

TaskManager::~TaskManager()
{
    // Wait all tasks terminated
    int nbTermate = 0;
    while (nbTermate != m_tasksMap.size()) {
        for (auto &it: m_tasksMap)
            nbTermate += (int)(it.second->isTerminate());
        Sleep(1000);
    }
    // Release thread
    CloseHandle(m_hThread);
}

void TaskManager::initialize(ServiceCfg* pCfg)
{
    createTasks();
    m_persistanceMgr->initialize(pCfg);
    m_hThread = CreateThread(nullptr, 0, TaskManager::taskThread, this, 0, nullptr);
}

void TaskManager::createTasks()
{
    size_t size = sizeof(kpi::kpiTable) / sizeof(kpi::KPI);
    for (int i=0; i < size; i++) {
        if (!kpi::kpiTable[i].isActif)
            continue;
        kpi::Code code = kpi::kpiTable[i].code;
        auto pTask = new ServiceTask(code, m_queue);
        m_tasksMap.insert(std::make_pair(code, pTask));
    }
    ServiceLogger::getInstance()->write("TaskManager::initialize() : all tasks created !\n");
}

void TaskManager::stop()
{
    ServiceLogger::getInstance()->write("TaskManager::stop() : stopping ...\n");

    for (auto &it : m_tasksMap) {
        ServiceTask* pTask = it.second;
        pTask->stop();
    }
    Sleep(2000);
    auto pMsg = new IMessage(nullptr, AGENT_THREAD_TERMINATE);
    m_queue->post(pMsg);
}

DWORD WINAPI TaskManager::taskThread(void* lparam)
{
    auto myManager = static_cast<TaskManager*>(lparam);
    PersistanceManager* pStorage = myManager->getPersistanceManager();

    ServiceLogger::getInstance()->write("TaskManager::taskThread() : running !\n");
    while (true) {
        try {
            IMessage *pMsg = myManager->getQueue()->pull();
            if (pMsg->getMessageId() == AGENT_THREAD_TERMINATE) {
                delete pMsg;
                break; // Arret
            }

            auto pData = static_cast<MessageData*>(pMsg->getData());
            if (pData != nullptr) {
                ServiceTask *pTask = myManager->getTask(pData->getKpiCode());
                if (pTask != nullptr) {
                    if (pMsg->getMessageId() == AGENT_RAW_DATA) {
                        // Send message to PeristanceManager
                        pStorage->getQueue()->post(pMsg);
                        continue;
                    } else {
                        pTask->run();
                    }
                } else {
                    char buffer[MAX_PATH];
                    sprintf(buffer, "TaskManager::taskThread() : No task ! unknown kpiCode id=%d\n", pData->getKpiCode());
                    ServiceLogger::getInstance()->write(buffer);
                }
                delete pMsg;
            } else {
                char buffer[MAX_PATH];
                sprintf(buffer, "TaskManager::taskThread() : unknown message id=%d\n", pMsg->getMessageId());
                ServiceLogger::getInstance()->write(buffer);
            }
        } catch(...) {
            ServiceLogger::getInstance()->write(Common::getLastErrorAsString("TaskManager::taskThread"));
        }
    }
    ServiceLogger::getInstance()->write("TaskManager::taskThread() : stopped !\n");
    return EXIT_SUCCESS;
}
