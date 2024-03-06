////////////////////////////////////////////////////////////////////////////////
//                          OLE FILE STORE - HEADER                           //
////////////////////////////////////////////////////////////////////////////////
//    DATE      PROG.                                                         //
// DD-MMM-YYYY INIT. SIR    MODIFICATION DESCRIPTION                          //
// ---------- ----- ------ -------------------------------------------------- //
// 04/02/2024  E.MF  Creation.                                                //
// 04/02/2024  E.MF  Prise en compte du mode Session dans les Storages.       //
////////////////////////////////////////////////////////////////////////////////

#ifndef FOOTPRINTAGENT_OLEFILE_STORE_H
#define FOOTPRINTAGENT_OLEFILE_STORE_H

#include <string>
#include <list>
#include <map>

#include "DataStore.h"
#include "OleFileManager.h"
#include "MessageData.h"

//============================================================================//
// STRUCTURE   :                    CICOleFileStore                           //
// DESCRIPTION : Pilote la persistence des donnees dans une base de donnees   //
//               structuree de type COMPOUND DOCUMENT FILE (STORAGE)          //
//============================================================================//
class OleFileStore : public DataStore
{
   public:
               OleFileStore(const std::string& dbName, const std::string& dbPath);
      virtual ~OleFileStore();

      //----------------------- CLASS PUBLICS METHODS -----------------------
      // Orders Log Functions
      virtual void saveData(SystemInformationModel* pData);                  // Save Data
      virtual void updateData(SystemInformationModel* pData);                // Update Data
      virtual void removeData(SystemInformationModel* pData);                // Delete Data

      // Retrieve Execution's Snapshot
      virtual std::map<std::string, std::list<SystemInformationModel*>*>* loadData();
      virtual std::list<SystemInformationModel*>* loadData(kpi::Code kpiCode);

      static void initDb(std::string dbPathName, std::list<std::string>& listStorages);

      virtual std::list<std::string>* selectStorages();
      virtual std::list<std::string>* loadStreams(const std::string& stgName);

   private:
      //----------------------- CLASS PRIVATE METHODS -----------------------
      // Getting Storage Name
      const char* getStgName(kpi::Code code);
      const char* getStgName(SystemInformationModel* pData) {
          return getStgName(pData->getModel());
      };

      // Storage Order Retrieving
      std::list<SystemInformationModel*>* retrieveData(char* argStgName, OleFileManager* argDbMgr= nullptr);

       // Saving Into Storage
      void saveDataInStorage(SystemInformationModel* pData, OleFileManager* argDbMgr= nullptr);
      
      // Loading From Storage
      std::map<std::string, std::list<SystemInformationModel*>*>* loadDataFromStorage(OleFileManager* argDbMgr= nullptr);

      //----------------------- CLASS PRIVATE ATTRIBUTES -----------------------
      OleFileManager* m_oleFileMgr;  // Data's Storage
};

//############################################################################//
//                       CICOleFileStore INLINE METHODS                       //
//############################################################################//
inline void OleFileStore::saveData(SystemInformationModel* pData)
{
   this->saveDataInStorage(pData);
}

// // Load Order's Book
inline std::map<std::string, std::list<SystemInformationModel*>*>* OleFileStore::loadData()
{
   return this->loadDataFromStorage();
}

#endif


