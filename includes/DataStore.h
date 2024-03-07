////////////////////////////////////////////////////////////////////////////////////
//                               DATA STORE - INTERFACE
////////////////////////////////////////////////////////////////////////////////////

#ifndef FOOTPRINTAGENT_DATASTORE_H
#define FOOTPRINTAGENT_DATASTORE_H

#include <map>
#include "MessageData.h"
#include "PersistentData.h"
#include "SystemInformation.h"

using namespace std;

//============================================================================//
// STRUCTURE   :                 CICDataStore                                 //
// DESCRIPTION : Classe Generique pilotant la persistence des Donnees.        //
//============================================================================//
class DataStore
{
   public:
               DataStore(const std::string& dbName, const std::string& dbPath);
      virtual ~DataStore();

      // Publics Methods
      virtual void saveData(SystemInformationModel* pData)    = 0;       // Save
      virtual void updateData(SystemInformationModel* pData)  = 0;       // Update data
      virtual void removeData(SystemInformationModel* pData)  = 0;       // Delete

      // Data Loading
      virtual std::map<std::string, std::list<SystemInformationModel*>*>* loadData() = 0; // Load all data
      virtual std::list<SystemInformationModel*>* loadData(kpi::Code kpiCode) = 0; // Load kpi data

      virtual std::list<std::string>* selectStorages() = 0;
      virtual std::list<std::string>* loadStreams(const std::string& stgName) = 0;

   protected:
      std::string m_dbName;
      std::string m_dbPath;
      char* m_swapDataBuffer;
};

#endif

