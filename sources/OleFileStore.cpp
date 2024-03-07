/*============================================================================//
////////////////////////////////////////////////////////////////////////////////
//                      OLE FILE STORE - IMPLEMENTATION                       //
////////////////////////////////////////////////////////////////////////////////
//    DATE      PROG.                                                         //
// DD-MMM-YYYY INIT. SIR    MODIFICATION DESCRIPTION                          //
// ---------- ----- ------ -------------------------------------------------- //
// 04/02/2024  E.MF Creation                                                  //
//            E.MF  Prise en compte du mode Session dans les Storages.        //
////////////////////////////////////////////////////////////////////////////////
//============================================================================*/

#include "../includes/OleFileManager.h"
#include "../includes/OleFileStore.h"

#include <string>
#include <iostream>
#include <sstream>
#include "../includes/Common.h"

#include "../includes/ServiceInformationFactory.h"
#include "../includes/ServiceLogger.h"

/*============================================================================*
 * STRUCTURE   :                    OleFileStore                              *
 * DESCRIPTION : Pilote la persistence des Ordres dans une base de donn�es    *
 *               structur�e de type COMPOUND DOCUMENT FILE (STORAGE)          *
 *============================================================================*/
OleFileStore::OleFileStore(const std::string& dbName, const std::string& dbPath)
: DataStore(dbName, dbPath)
, m_oleFileMgr()
{
    char filename[MAX_PATH];
    memset(filename, 0, MAX_PATH);
    sprintf(filename, "%s\\%s", dbPath.c_str(), dbName.c_str());
    m_oleFileMgr = new OleFileManager(filename);
    m_oleFileMgr->openDatabase();
};

// DESTRUCTOR
OleFileStore::~OleFileStore()
{
   try {
      delete m_oleFileMgr;
      m_oleFileMgr = nullptr;
   } catch(...) {
   }
};

/*
 * The name must not exceed 31 characters in length, not including the string terminator.
 */
const char* OleFileStore::getStgName(kpi::Code code)
{
    char buffer[MAX_PATH];
    memset(buffer, 0, MAX_PATH);
    sprintf(buffer, "%02d-%s", (int)code, kpi::kpiTable[(int)code].name);

    char* pMem = new char[MAX_OLECHAR];
    Common::safeCopy(pMem, buffer, MAX_OLECHAR);
    return pMem;
}

// Save Container
void OleFileStore::saveDataInStorage(SystemInformationModel* pData, OleFileManager* argDbMgr)
{
   try {
      const char* StgName = getStgName(pData);

      OleFileManager* pDbMgr = argDbMgr ? argDbMgr : m_oleFileMgr;
       LPSTORAGE pStorage = pDbMgr->openOrCreateStorage(StgName);
      if (pStorage) {
            PersistentModelBase* pModel = PersistentModelFactoryHelper::create(pData);
            if (pModel) {
                std::string oleStream = pModel->getDataAsString();
                // Write data
                pDbMgr->addStream(pStorage, pData->getKey(), (void*)oleStream.c_str(), oleStream.length());
            } else {
                std::ostringstream oss;
                oss << "OleFileStore::saveDataInStorage() - Create persistent model failure - model="
                << kpi::kpiTable[pData->getModel()].name << "\n";
                ServiceLogger::getInstance()->write(oss.str());
            }
          pStorage->Release();
      } else {
          std::ostringstream oss;
          oss << "OleFileStore::saveDataInStorage() - " << StgName << " Failure !\n";
          ServiceLogger::getInstance()->write(oss.str());
      }
   } catch(...) {
       ServiceLogger::getInstance()->write(Common::getLastErrorAsString("OleFileStore::saveDataInStorage"));
   }
}

// UpDate Order
void OleFileStore::updateData(SystemInformationModel* pData)
{
   try {
      const char* StgName = getStgName(pData);
      LPSTORAGE pStorage = m_oleFileMgr->openStorage(StgName);
      if (pStorage) {
          PersistentModelBase *pModel = PersistentModelFactoryHelper::create(pData);
          std::string oleStream = pModel->getDataAsString();
          // Write data
          if (!m_oleFileMgr->updateStream(pStorage, pData->getKey().c_str(), (void *) oleStream.c_str(), oleStream.length())) {
              std::ostringstream oss;
              oss << "OleFileStore::updateData() - " << StgName << " Failure !";
              ServiceLogger::getInstance()->write(oss.str());
          }
          pStorage->Release();
      }
   } catch(...) {
       std::cout << Common::getLastErrorAsString("OleFileStore::updateData") << std::endl;
   }
}

void OleFileStore::removeData(SystemInformationModel* pData)
{
   try {
      std::ostringstream oss;
      const char* StgName = getStgName(pData);
      LPSTORAGE pStorage = m_oleFileMgr->openStorage(StgName);
      if (pStorage) {
          if (!m_oleFileMgr->removeStream(pStorage, pData->getKey())) {
              oss << "OleFileStore::removeData() - " << StgName << " failure !";
              ServiceLogger::getInstance()->write(oss.str());
          }
          pStorage->Release();
      }
   } catch(...) {
       ServiceLogger::getInstance()->write(Common::getLastErrorAsString("OleFileStore::removeData"));
   }
}

// Load all data
std::map<std::string, std::list<SystemInformationModel*>*>* OleFileStore::loadDataFromStorage(OleFileManager* argDbMgr)
{
   auto result = new std::map<std::string, std::list<SystemInformationModel*>*>();
   try {
      std::list<std::string>* pStgList = nullptr;
      pStgList = argDbMgr ? argDbMgr->selectStorages() : m_oleFileMgr->selectStorages();
      while (!pStgList->empty()) {
         const char* stgName = pStgList->front().c_str();
         std::list<SystemInformationModel*>* lesData = retrieveData((char*)stgName, argDbMgr);
         result->insert(std::pair<std::string, std::list<SystemInformationModel*>*>(stgName, lesData));
         // Free Memory
         pStgList->pop_front();
         delete [] stgName;
      }
      delete pStgList;
   } catch(...) {
       ServiceLogger::getInstance()->write(Common::getLastErrorAsString("OleFileStore::loadDataFromStorage"));
   }
   return result;
};

// Retrieve data
std::list<SystemInformationModel*>* OleFileStore::retrieveData(char* argStgName, OleFileManager* argDbMgr)
{
   auto result = new std::list<SystemInformationModel*>();
   try {
      std::string kpi(argStgName);
      std::list<OleStream*>* pLesStream = argDbMgr ? argDbMgr->loadStreams(argStgName) : m_oleFileMgr->loadStreams(argStgName);
      if (pLesStream) {
          size_t pos = kpi.find('-');
          kpi::Code model = (kpi::Code)strtol(kpi.substr(0, pos).c_str(), nullptr, 10);
          for (OleStream* oleStram : *pLesStream) {
              PersistentModelBase* pModel = PersistentModelFactoryHelper::create(model);
              SystemInformationModel* pData = pModel->createInstance(oleStram->GetValue().c_str());
              result->push_back(pData);
              delete oleStram;
          }
          // Free Memory
          pLesStream->clear();
          delete pLesStream;
      } else {
          std::ostringstream oss;
          oss << "OleFileStore::retrieveData() - " << argStgName << " failure !";
          ServiceLogger::getInstance()->write(oss.str());
      }
   } catch(...) {
       ServiceLogger::getInstance()->write(Common::getLastErrorAsString("OleFileStore::retrieveData"));
   }
   return result;
};

std::list<SystemInformationModel*>* OleFileStore::loadData(kpi::Code kpiCode)
{
    const char* stgName = getStgName(kpiCode);
    return retrieveData((char*)stgName);
}

void OleFileStore::initDb(std::string dbPathName, std::list<std::string>& listStorages)
{
    OleFileManager::initDb(dbPathName, listStorages);
}

std::list<std::string>* OleFileStore::selectStorages()
{
    return m_oleFileMgr->selectStorages();
}

std::list<std::string>* OleFileStore::loadStreams(const std::string& stgName)
{
    auto pResult = new std::list<std::string>();
    std::list<OleStream*>* pLesStreams = m_oleFileMgr->loadStreams(stgName.c_str());
    for (auto it : *pLesStreams)
        pResult->push_back(it->GetValue());
    return pResult;
}
