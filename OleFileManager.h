////////////////////////////////////////////////////////////////////////////////
//                     OLEFILEMANAGER - IMPLEMENTATION                        //
////////////////////////////////////////////////////////////////////////////////
//    DATE      PROG.                                                         //
// DD-MMM-YYYY INIT. SIR    MODIFICATION DESCRIPTION                          //
// ---------- ----- ------ -------------------------------------------------- //
// 04/02/2024  E.MF  Creation.                                                //
// 04/02/2024  E.MF  Prise en compte de l'extension de l'OS et ses Limites.   //
////////////////////////////////////////////////////////////////////////////////

#ifndef FOOTPRINTAGENT_OLEFILEMANAGER_H
#define FOOTPRINTAGENT_OLEFILEMANAGER_H

#include <stdio.h>
#include <ole2.h>
#include <list>
#include <minwindef.h>
#include <combaseapi.h>
#include <string>
#include "Resources.h"

// Implicitly link ole32.dll
#pragma comment( lib, "ole32.lib" )

#define MAX_OLECHAR   32

// defined Handlers
class OleStream;
class OleFileManager;

namespace STG {
   // Storage Mode
   typedef enum { SESSION=0,
                  TRANSACTION=1
                } MODE;
}

//============================================================================//
// STRUCTURE   :                     OleStream                                //
// DESCRIPTION : Representation abstraite d'une ligne de donnees d'un Storage.//
//============================================================================//
class OleStream
{
public:
    OleStream(char* argKey, char* argVal) : m_Key(argKey), m_Value(argVal)
    {};
    virtual ~OleStream()
    {};

      //------------------ Class Publics Methods ------------------
    const std::string GetKey()   const { return m_Key;   } ;
    const std::string GetValue() const { return m_Value; };

protected:
    std::string m_Key;
    std::string m_Value;
};

//============================================================================//
// STRUCTURE   :               OleFileManager                                 //
// DESCRIPTION : Implemente la gestion d'une base de donnees de type COMPOUND //
//               DOCUMENT FILE (STORAGE).                                     //
//============================================================================//
class OleFileManager
{
public:
          OleFileManager(const char* argDocFile, bool argbEncrypt=false);
    virtual ~OleFileManager();

    // Get Functions
    int   getRootCount()  const { return m_nStgCount; };
    char* getDocFile() { return m_docFile;   };

    // Storage functions
    LPSTORAGE openStorage(const char* argStgName);
    LPSTORAGE createStorage(const char* argStgName);
    LPSTORAGE openOrCreateStorage(const char* argStgName);
    bool removeStorage(const char* argStgName);

    // Stream functions
    bool existStream(LPSTORAGE pStorage, const char* argStmName);

    bool addStream(LPSTORAGE pStorage, const std::string& argStmName, void* pData, UINT dataLen);
    bool updateStream(LPSTORAGE pStorage, const std::string& stmName, void* pData, UINT argLen);
    bool removeStream(LPSTORAGE pStorage, const std::string& stmName);

    // Useful Functions
    std::list<std::string>* selectStorages(const char* argStgName= nullptr, short nDigit=0);
    std::list<OleStream*>* loadStreams(const char* argStgName);
    unsigned long retrieveStream(const char* argStgName, const char* argStmName, char* argData, UINT argDataLen);

    static void initDb(const std::string& dbPathName, std::list<std::string>& listStorages);
    bool  openDatabase();

protected:
      //-------------------- CLASS PROTECTED METHODS --------------------
    std::string analize(HRESULT hResult, std::string caller);
    std::string analize2(HRESULT hResult, std::string caller);

private:
    bool  isValid() const { return (m_pRootStg != NULL); };

    DWORD AfxRelease(LPUNKNOWN* plpUnknown);
    void  encrypt(BYTE* argpData, ULONG argSize);
    bool  createDatabase();
    void  closeDatabase();
    bool  defragmentFile();

    //-------------------- CLASS PRIVATE PROPERTIES --------------------
    int  m_nStgCount;
    bool m_bEncrypt;
    char m_docFile[MAX_PATH];

    STGOPTIONS* m_options;
    LPSTORAGE   m_pRootStg;
    Mutex m_mutex;
};

#endif


