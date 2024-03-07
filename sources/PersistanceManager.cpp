//
// Created by admin on 08/02/2024.
//

#include "../includes/PersistanceManager.h"
#include "../includes/Message.h"
#include "../includes/AppQueue.h"
#include "../includes/ServiceLogger.h"
#include "../includes/OleFileStore.h"
#include "../includes/Common.h"

PersistanceManager::PersistanceManager()
: m_dataStore(nullptr)
, m_queue(new AppQueue())
, m_hThread(INVALID_HANDLE_VALUE)
{}

PersistanceManager::~PersistanceManager()
{
    if (m_hThread != INVALID_HANDLE_VALUE)
        CloseHandle(m_hThread);
}

void PersistanceManager::initialize(ServiceCfg* pCfg)
{
    m_dataStore = new OleFileStore(pCfg->getDbName(), pCfg->getDbPath());
    m_hThread = CreateThread(nullptr, 0, PersistanceManager::storeThread, this, 0, nullptr);
}

void PersistanceManager::stop()
{
    auto pMsg = new IMessage(nullptr, AGENT_THREAD_TERMINATE);
    m_queue->post(pMsg);
}

DWORD WINAPI PersistanceManager::storeThread(void* lparam)
{
    char buffer[MAX_PATH];
    auto myManager = static_cast<PersistanceManager*>(lparam);
    while (true) {
        try {
            IMessage *pMsg = myManager->getQueue()->pull();
            if (pMsg->getMessageId() == AGENT_THREAD_TERMINATE) {
                delete pMsg;
                break; // Arret
            }

            auto pData = dynamic_cast<SystemInformationModel*>(pMsg->getData());
            if (pData != nullptr) {
                myManager->saveData(pData);
                sprintf(buffer, "PersistanceManager::storeThread() : save %s data.\n", kpi::kpiTable[pData->getKpiCode()].name);
                ServiceLogger::getInstance()->write(buffer);
                pMsg->detachData();
            } else {
                sprintf(buffer, "PersistanceManager::storeThread : unknown message id=%d", pMsg->getMessageId());
                ServiceLogger::getInstance()->write(buffer);
            }
            delete pMsg;
        } catch(...) {
            ServiceLogger::getInstance()->write(Common::getLastErrorAsString("PersistanceManager::storeThread()"));
        }
    }
    return EXIT_SUCCESS;
}

void PersistanceManager::saveData(SystemInformationModel* pData)
{
    m_dataStore->saveData(pData);
}

std::map<std::string, std::list<SystemInformationModel*>*>* PersistanceManager::loadData()
{
    return m_dataStore->loadData();
}

std::list<SystemInformationModel*>* PersistanceManager::loadData(kpi::Code kpiCode)
{
    return m_dataStore->loadData(kpiCode);
}

void PersistanceManager::initDb(std::string dbPathName, std::list<std::string>& listStorages)
{
    OleFileStore::initDb(dbPathName, listStorages);
}

std::list<std::string>* PersistanceManager::selectStorages()
{
    return m_dataStore->selectStorages();
}

std::list<std::string>* PersistanceManager::loadStreams(const std::string& stgName)
{
    return m_dataStore->loadStreams(stgName);
}