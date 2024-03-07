/*****************************************************************************
*                     OLEFILEMANAGER - IMPLEMENTATION                        *
******************************************************************************
*                                                                            *
* The compound file implementation of compound files is subject to the       *
* following implementation constraints.                                      *
*                                                                            *
* Limit Compound file                                                        *
* File size limits:                                                          *
*       512: 2 gigabytes (GB)                                                *
*       4096: Up to file system limits                                       *
*                                                                            *
* Maximum heap size required for open elements:                              *
*       512: 4 megabytes (MB)                                                *
*       4096: Up to virtual memory limits                                    *
*                                                                            *
* Concurrent root opens (opens of the same file):                            *
*       If STGM_READ and STGM_SHARE_DENY_WRITE are specified, limits are     *
*       dictated by the file-system limits. Otherwise, there is a limit of   *
*       20 concurrent root opens of the same file.                           *
*                                                                            *
* Number of elements in a file:                                              *
*       512: Unlimited, but performance may degrade if elements number in    *
*            the thousands                                                   *
*       4096: Unlimited                                                      *
* STGOPTIONS :                                                               *
*       usVersion : Specifies the version of the STGOPTIONS structure. It is *
*       set to STGOPTIONS_VERSION.                                           *
*       Note  When usVersion is set to 1, the ulSectorSize member can be set.*
*       This is useful when creating a large-sector documentation file.      *
*       However, when usVersion is set to 1, the pwcsTemplateFile member     *
*       cannot be used.                                                      *
* In Windows 2000 & later: STGOPTIONS_VERSION can be set to 1 for version 1. *
* In Windows XP & later: STGOPTIONS_VERSION can be set to 2 for version 2.   *
******************************************************************************
*    DATE      PROG.                                                         *
* DD-MMM-YYYY INIT. SIR    MODIFICATION DESCRIPTION                          *
* ---------- ----- ------ -------------------------------------------------- *
* 09/02/2024  E.MF  Creation.                                                *
* 09/02/2024  E.MF  Prise en compte de l'extension de l'OS et Limites.       *
******************************************************************************/

#include <cstddef>
#include <cassert>
#include <cstring>
#include <iostream>

#include "../includes/OleFileManager.h"
#include "../includes/ServiceLogger.h"
#include "../includes/Common.h"
#include "../includes/ResourceLock.h"
#include "../includes/FootPrintException.h"

#pragma warn -8004

/******************************************************************************
 * STRUCTURE   :               OleFileManager                                 *
 * DESCRIPTION : Impl�mente la gestion d'une base de donn�es de type COMPOUND *
 *               DOCUMENT FILE (STORAGE).                                     *
 ******************************************************************************/
OleFileManager::OleFileManager(const char* argDocFile, bool argbEncrypt)
: m_nStgCount(0)
, m_bEncrypt(argbEncrypt)
, m_docFile()
, m_options(nullptr)
, m_pRootStg(nullptr)
, m_mutex()
{
   std::memset(m_docFile, 0, MAX_PATH);
   std::strcpy(m_docFile, argDocFile);
   // Make sure the ole libraries have been initialized before creating the doc
   #ifdef _DEBUG
   LPMALLOC lpMalloc = nullptr;
   if (::CoGetMalloc(MEMCTX_TASK, &lpMalloc) != NOERROR) {
      throw "AfxOleInit() not called yet";
   }
   AfxRelease((LPUNKNOWN*)&lpMalloc);
   #endif

   #if !defined(EXCH_WINNT)
   // Windows Version
   OSVERSIONINFO theOS;
   theOS.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
   if (GetVersionEx(&theOS) && (theOS.dwPlatformId >= VER_PLATFORM_WIN32_NT) && (theOS.dwMajorVersion > 4)) {
      // Create Storage Options
      m_options = new STGOPTIONS();
      m_options->usVersion        = STGOPTIONS_VERSION; // Versions 1 and 2 supported
      m_options->reserved         = 0;                  //must be 0 for padding
      m_options->ulSectorSize     = 4096;               //docfile header sector size (default = 512)
   }
   #endif
}

OleFileManager::~OleFileManager()
{
    closeDatabase();
    delete m_options;
}

DWORD OleFileManager::AfxRelease(LPUNKNOWN* plpUnknown)
{
    DWORD dwResult = 0;
    // Make sure the pointer and the contents are valid
    if (plpUnknown && *plpUnknown ) {
        dwResult = (*plpUnknown)->Release();
        *plpUnknown = nullptr;
    }
    return dwResult;
}

void OleFileManager::closeDatabase()
{
    if (m_pRootStg)
        AfxRelease((LPUNKNOWN *) &m_pRootStg);
    // Reset
    m_pRootStg = nullptr;
    m_nStgCount = 0;
}

std::string OleFileManager::analize(HRESULT hResult, std::string caller)
{
    char Buffer[MAX_PATH];
    switch(hResult) {
        case    STG_E_INVALIDPOINTER: strcpy(Buffer, "Indicates a non-valid pointer in the ppObjectOpen parameter."); break;
        case  STG_E_INVALIDPARAMETER: strcpy(Buffer, "Indicates a non-valid value for the grfAttrs, reserved1, reserved2, grfMode, or stgfmt parameters."); break;
        case       STG_E_INVALIDFLAG: strcpy(Buffer, "Indicates a non-valid flag combination in the grfMode pointer."); break;
        case       STG_E_INVALIDNAME: strcpy(Buffer, "Indicates a non-valid name in the pwcsName parameter."); break;
        case STG_E_FILEALREADYEXISTS: strcpy(Buffer, "Indicates that the compound file already exists and grfMode is set to STGM_FAILIFTHERE."); break;
        case     STG_E_LOCKVIOLATION: strcpy(Buffer, "Access denied because another caller has the file open and locked."); break;
        case    STG_E_SHAREVIOLATION: strcpy(Buffer, "Access denied because another caller has the file open and locked."); break;
            //case           STG_E_NOTIMPL: strcpy(Buffer, "Indicates that the StgCreateStorageEx function is not implemented by the operating system. In this case, use the StgCreateDocfile function."); break;
        case      STG_E_ACCESSDENIED: strcpy(Buffer, "Indicates that the file could not be created because the underlying storage device does not allow such access to the current user."); break;
        default: strcpy(Buffer, "Impossible to open or create Storage Database."); break;
    }
    caller.append(" - ").append(Buffer).append("\n");
    return caller;
}

std::string OleFileManager::analize2(HRESULT hResult, std::string caller)
{
    char Buffer[MAX_PATH];
    switch(hResult) {
        case                     S_OK: strcpy(Buffer, "The storage object was created successfully."); break;
        case                E_PENDING: strcpy(Buffer, "Asynchronous Storage only: Part or all of the necessary data is currently unavailable."); break;
        case       STG_E_ACCESSDENIED: strcpy(Buffer, "Not enough permissions to create storage object."); break;
        case  STG_E_FILEALREADYEXISTS: strcpy(Buffer, "The name specified for the storage object already exists in the storage object and the grfMode parameter includes the flag STGM_FAILIFTHERE."); break;
        case STG_E_INSUFFICIENTMEMORY: strcpy(Buffer, "The storage object was not created due to a lack of memory."); break;
        case        STG_E_INVALIDFLAG: strcpy(Buffer, "The value specified for the grfMode parameter is not a valid STGM constant value. The value specified for the grfMode parameter is not a valid"); break;
        case    STG_E_INVALIDFUNCTION: strcpy(Buffer, "The specified combination of flags in the grfMode parameter is not supported."); break;
        case        STG_E_INVALIDNAME: strcpy(Buffer, "Not a valid value for pwcsName."); break;
        case     STG_E_INVALIDPOINTER: strcpy(Buffer, "The pointer specified for the storage object was not valid."); break;
        case   STG_E_INVALIDPARAMETER: strcpy(Buffer, "One of the parameters was not valid."); break;
        case           STG_E_REVERTED: strcpy(Buffer, "The storage object has been invalidated by a revert operation above it in the transaction tree."); break;
        case   STG_E_TOOMANYOPENFILES: strcpy(Buffer, "The storage object was not created because there are too many open files."); break;
        case          STG_S_CONVERTED: strcpy(Buffer, "The existing stream with the specified name was replaced with a new storage object containing a single stream called CONTENTS. The new storage object will be added."); break;
        default: strcpy(Buffer, "Impossible to open or create Storage."); break;
    }
    caller.append(" - ").append(Buffer).append("\n");
    return caller;
}

/*############################################################################*
 *                  COUMPOUND DOCUMENT FILE CREATE FUNCTIONS                  *
 *############################################################################*/
bool OleFileManager::createDatabase()
{
   try {
      HRESULT hResult;
      LPSTORAGE pRootStorage;
      OLECHAR pwcsval[MAX_PATH];

      mbstowcs(pwcsval, m_docFile, MAX_PATH);
      DWORD dwMode = STGM_CREATE | STGM_READWRITE | STGM_DIRECT | STGM_SHARE_EXCLUSIVE;
      #ifdef EXCH_WINNT
      hResult = StgCreateDocfile(pwcsval, dwMode, 0, &pRootStorage);
      #else
      if (m_options)
          hResult = StgCreateStorageEx(pwcsval, dwMode, STGFMT_DOCFILE, 0, m_options, nullptr, IID_IStorage, reinterpret_cast<void**>(&pRootStorage));
      else
          hResult = StgCreateDocfile(pwcsval, dwMode, 0, &pRootStorage);
      #endif

      if ((hResult != NOERROR) || (pRootStorage == nullptr)) {
         std::string buffer=analize(hResult, "OleFileManager::createDatabase()");
         throw footprint_exception(buffer.c_str());
      }
      m_pRootStg = pRootStorage;
      //m_pRootStg->Commit(STGC_ONLYIFCURRENT);
      ServiceLogger::getInstance()->write("OleFileManager::createDatabase() - DB create OK.\n");
   } catch(...) {
       ServiceLogger::getInstance()->write(Common::getLastErrorAsString("OleFileManager::createDatabase() - Error: "));
       throw;
   }
   return true;
}


/*############################################################################*
 *                             STORAGE FUNCTIONS                              *
 *############################################################################*/
// Create a storage to the root stream
LPSTORAGE OleFileManager::createStorage(const char* argStgName)
{
    assert(m_pRootStg != nullptr);
    try {
        LPSTORAGE pStorage = nullptr;
        OLECHAR pwcsval[MAX_PATH];
        mbstowcs(pwcsval, argStgName, MAX_PATH);

        DWORD dwMode = STGM_CREATE | STGM_READWRITE | STGM_DIRECT | STGM_SHARE_EXCLUSIVE;
        HRESULT hResult = m_pRootStg->CreateStorage(pwcsval, dwMode, 0, 0, &pStorage);
        if (hResult == S_OK) {
            m_nStgCount++;
            m_pRootStg->Commit(STGC_ONLYIFCURRENT); // Commit the root storage
            return pStorage;
        }
        else
            ServiceLogger::getInstance()->write(analize2(hResult, "OleFileManager::createStorage()"));
    } catch(...) {
        ServiceLogger::getInstance()->write(Common::getLastErrorAsString("OleFileManager::createStorage() - Error: "));
    }
    return nullptr;
}

// Open an existing Structured Storage
bool OleFileManager::openDatabase()
{
   bool bResult = false;
   try {
      OLECHAR pwcsval[MAX_PATH];
      m_pRootStg = nullptr;

      mbstowcs(pwcsval, m_docFile, MAX_PATH);
      //DWORD dwMode = STGM_DIRECT_SWMR | STGM_READWRITE | STGM_SHARE_DENY_WRITE;
      DWORD dwMode = STGM_DIRECT | STGM_READWRITE | STGM_SHARE_EXCLUSIVE;
      HRESULT hResult = S_OK;
      #ifdef EXCH_WINNT
      hResult = StgOpenStorage(pwcsval, nullptr, dwMode, 0, 0, &pStorage);
      #else
      if (m_options)
          hResult = StgOpenStorageEx(pwcsval, dwMode, STGFMT_DOCFILE, 0, m_options, nullptr, IID_IStorage, reinterpret_cast<void**>(&m_pRootStg));
      else
          hResult = StgOpenStorage(pwcsval, nullptr, dwMode, nullptr, 0, &m_pRootStg);
      #endif

      if (hResult != NOERROR || m_pRootStg == nullptr) {
          ServiceLogger::getInstance()->write(analize(hResult, "OleFileManager::openDatabase()"));
          bResult = false;
      }
      else {
         bResult = true;
         ServiceLogger::getInstance()->write("OleFileManager::openDatabase() - DB opened !\n");
      }
   } catch(...) {
       ServiceLogger::getInstance()->write(Common::getLastErrorAsString("OleFileManager::openDatabase() - Error: "));
       throw;
   }
   return bResult;
}

// Encrypt/Decrypt Data
void OleFileManager::encrypt(BYTE* pData, ULONG argSize)
{
   // Do a replacing XOR with F(Hex) of each byte
   for (ULONG index = 0; (index < argSize); index++)
      pData[index] = pData[index]^0xF;
}

// Open Or Create Storage
LPSTORAGE OleFileManager::openOrCreateStorage(const char* argStgName)
{
    // We are sure the root storage exist
    assert(m_pRootStg != nullptr);
    try {
        LPSTORAGE pStorage = nullptr;
        OLECHAR pwcsval[MAX_PATH];

        //DWORD dwMode = STGM_DIRECT_SWMR | STGM_READWRITE | STGM_SHARE_EXCLUSIVE;
        DWORD dwMode = STGM_DIRECT | STGM_READWRITE | STGM_SHARE_EXCLUSIVE;
        mbstowcs(pwcsval, argStgName, MAX_PATH);
        HRESULT hResult = m_pRootStg->OpenStorage(pwcsval, nullptr, dwMode, nullptr, 0, &pStorage);
        if (hResult != S_OK || pStorage == nullptr)
            return createStorage(argStgName);
        return pStorage;
    } catch(...) {
        ServiceLogger::getInstance()->write(Common::getLastErrorAsString("OleFileManager::openOrCreateStorage()"));
    }
    return nullptr;
}

/*############################################################################*
 *                          STORAGE SESSION FUNCTIONS                         *
 *############################################################################*/
LPSTORAGE OleFileManager::openStorage(const char* argStgName)
{
   // We are sure the root storage exist
   assert(m_pRootStg != nullptr);
   try {
       LPSTORAGE pStorage = nullptr;
      OLECHAR pwcsval[MAX_PATH];
      mbstowcs(pwcsval, argStgName, MAX_PATH);

      //DWORD dwMode = STGM_DIRECT_SWMR | STGM_READWRITE | STGM_SHARE_EXCLUSIVE;
      DWORD dwMode = STGM_DIRECT | STGM_READWRITE | STGM_SHARE_EXCLUSIVE;
      HRESULT hresult = m_pRootStg->OpenStorage(pwcsval, nullptr, dwMode, nullptr, 0, &pStorage);
      if (hresult == S_OK) {
         return pStorage;
      } else
          ServiceLogger::getInstance()->write(analize2(hresult, "OleFileManager::openStorage()"));
   } catch(...) {
       ServiceLogger::getInstance()->write(Common::getLastErrorAsString("OleFileManager::openStorage()"));
   }
   return nullptr;
}

void analyze3(HRESULT hResult)
{
    switch(hResult) {
        case S_OK	: std::cout << "Le nouveau flux a été créé avec succès." << std::endl; break;
        case E_PENDING	: std::cout << "Stockage asynchrone uniquement : une partie ou la totalité des données nécessaires n’est actuellement pas disponible." << std::endl; break;
        case STG_E_ACCESSDENIED	: std::cout << "Autorisations insuffisantes pour créer un flux." << std::endl; break;
        case STG_E_FILEALREADYEXISTS	: std::cout << "Le nom spécifié pour le flux existe déjà dans l’objet de stockage et le paramètre grfMode inclut la valeur STGM_FAILIFTHERE." << std::endl; break;
        case STG_E_INSUFFICIENTMEMORY	: std::cout << "Le flux n’a pas été créé en raison d’un manque de mémoire." << std::endl; break;
        case STG_E_INVALIDFLAG	: std::cout << "La valeur spécifiée pour le paramètre grfMode n’est pas une valeur de constantes STGM valide." << std::endl; break;
        case STG_E_INVALIDFUNCTION	: std::cout << "La combinaison spécifiée d’indicateurs dans le paramètre grfMode n’est pas prise en charge ; par exemple, lorsque cette méthode est appelée sans l’indicateur STGM_SHARE_EXCLUSIVE." << std::endl; break;
        case STG_E_INVALIDNAME	: std::cout << "Valeur non valide pour pwcsName." << std::endl; break;
        case STG_E_INVALIDPOINTER	: std::cout << "Le pointeur spécifié pour l’objet stream n’était pas valide." << std::endl; break;
        case STG_E_INVALIDPARAMETER	: std::cout << "L'un des paramètres n'était pas valide." << std::endl; break;
        case STG_E_REVERTED	: std::cout << "L’objet de stockage a été invalidé par une opération de restauration au-dessus de lui dans l’arborescence des transactions." << std::endl; break;
        case STG_E_TOOMANYOPENFILES	: std::cout << "Le flux n’a pas été créé, car il y a trop de fichiers ouverts." << std::endl; break;
        default: std::cout << "unknown return code." << std::endl;
    }
}

bool OleFileManager::addStream(LPSTORAGE pStorage, const std::string& StmName, void* pData, UINT dataLen)
{
    assert(pStorage != nullptr);
    try {
        LPSTREAM pStream;
        OLECHAR pwcsval[MAX_PATH];

        std::memset(pwcsval, 0, MAX_PATH);
        mbstowcs(pwcsval, StmName.c_str(), StmName.length());

        DWORD grfMode = STGM_CREATE | STGM_WRITE | STGM_SHARE_EXCLUSIVE;
        IResourceLock locker(&m_mutex);
        HRESULT hResult = pStorage->CreateStream(pwcsval, grfMode, 0, 0, &pStream);
        if (hResult == S_OK) {
            if (m_bEncrypt)
                encrypt((BYTE*)pData, dataLen);
            ULONG wSize = 0;
            pStream->Write(pData, dataLen, &wSize);
            pStorage->Commit(STGC_ONLYIFCURRENT);
            pStream->Release();
            return true;
        } else {
            analyze3(hResult);
            ServiceLogger::getInstance()->write(analize2(hResult, "OleFileManager::addStream()"));
        }
    } catch(...) {
    }
    return false;
}

// Select Storages
std::list<std::string>* OleFileManager::selectStorages(const char* argStgName, short nDigit)
{
   // Make sure the root is set
   assert(m_pRootStg != nullptr);

   // Enumerate the storages under lpRootStg
   auto pStgNameList = new std::list<std::string>();
   try {
      LPENUMSTATSTG lpEnum;
      // Get the enumerator (Assume valid )
      IResourceLock locker(&m_mutex);
      if (m_pRootStg->EnumElements(0, nullptr, 0, &lpEnum) == NOERROR) {
         STATSTG ss;
         ULONG   ulcount;
         int nCount = 0;
         // Poll the enumeration until storage found or the end is encountered
         while (lpEnum->Next(1, &ss, &ulcount) == S_OK) {
            // possible values for ss.type are: STGTY_...STREAM|STORAGE|LOCKBYTES|PROPERTY
            if (ss.type == STGTY_STORAGE) {
               // Get the current storage name
               char ssName[MAX_PATH];
               wcstombs(ssName, ss.pwcsName, MAX_PATH);
               if (!argStgName) {
                  pStgNameList->push_back(ssName);
                  nCount++;
               }
               else {
                  // Check Named Storage: Compare Storage name with given argument
                  if (nDigit > 0) {
                     if (strncmp(argStgName, ssName, nDigit) == 0) {
                        pStgNameList->push_back(ssName);
                        nCount++;
                     }
                  }
                  else if (strcmp(argStgName, ssName) == 0) {
                     pStgNameList->push_back(ssName);
                     nCount++;
                  }
               }
            }
            // Free up the storage name
            LPMALLOC pIMalloc;
            CoGetMalloc(MEMCTX_TASK, &pIMalloc );
            pIMalloc->Free((LPVOID)ss.pwcsName);
            pIMalloc->Release();
         }
         lpEnum->Release();
         if (nCount == 0) {
            delete pStgNameList;
            pStgNameList = nullptr;
         }
      }
   } catch (...) {
   }
   return pStgNameList;
}

bool OleFileManager::removeStorage(const char* argStgName)
{
   bool bResult = false;
   try {
      OLECHAR pwcsval[MAX_PATH];
      mbstowcs(pwcsval, argStgName, MAX_PATH);
      IResourceLock locker(&m_mutex);
      if (m_pRootStg->DestroyElement(pwcsval) == S_OK) {
         bResult = true;
         m_pRootStg->Commit(STGC_ONLYIFCURRENT);
         m_nStgCount--;
         // Must now defrag the file to free up the space
         //DefragmentFile();
      }
   } catch(...) {
   }
   return bResult;
}

/*############################################################################*
 *                              STREAM FUNCTIONS                              *
 *############################################################################*/
// Check Stream
bool OleFileManager::existStream(LPSTORAGE pStorage, const char* argStmName)
{
    assert(pStorage != nullptr);
    try {
        OLECHAR pwcsval[MAX_PATH];
        LPSTREAM pStream;

        mbstowcs(pwcsval, argStmName, MAX_PATH);
        DWORD grfMode = STGM_READ | STGM_SHARE_DENY_WRITE;
        if (pStorage->OpenStream(pwcsval, nullptr, grfMode, 0, &pStream) == S_OK) {
            pStream->Release();
            return true;
        }
   } catch(...) {
   }
   return false;
}

// Update Stream Into Current Storage
bool OleFileManager::updateStream(LPSTORAGE pStorage, const std::string& stmName, void* pData, UINT argLen)
{
    assert(pStorage != nullptr);
    bool bResult = false;
    try {
        LPSTREAM pStream;
        OLECHAR pwcsval[MAX_PATH];

        std::memset(pwcsval, 0, MAX_PATH);
        mbstowcs(pwcsval, stmName.c_str(), stmName.length());

        DWORD grfMode = STGM_WRITE | STGM_SHARE_EXCLUSIVE;
        IResourceLock locker(&m_mutex);
        if (pStorage->OpenStream(pwcsval, nullptr, grfMode, 0, &pStream) == S_OK) {
         // Encpypt the information
         if (m_bEncrypt)
             encrypt((BYTE*)pData, argLen);
         // Write the Data and close the stream
         ULONG wSize = 0;
         pStream->Write(pData, argLen, &wSize);
         pStream->Release();
         m_pRootStg->Commit(STGC_ONLYIFCURRENT);
         return true;
        }
    } catch(...) {
    }
    return false;
}

// Remove Stream Into Current Storage
bool OleFileManager::removeStream(LPSTORAGE pStorage, const std::string& stmName)
{
    assert(pStorage != nullptr);
    try {
         OLECHAR pwcsval[MAX_PATH];
         std::memset(pwcsval, 0, MAX_PATH);
         mbstowcs(pwcsval, stmName.c_str(), stmName.length());
        IResourceLock locker(&m_mutex);
         HRESULT hResult = pStorage->DestroyElement(pwcsval);
         if (hResult == S_OK) {
             pStorage->Release();
             m_pRootStg->Commit(STGC_ONLYIFCURRENT);
         }
         return hResult == S_OK;
    } catch(...) {
    }
   return false;
}

/*############################################################################*
 *                             LOADING FUNCTIONS                              *
 *############################################################################*/
std::list<OleStream*>* OleFileManager::loadStreams(const char* argStgName)
{
   assert(m_pRootStg != nullptr);
   IResourceLock locker(&m_mutex);
   auto pLesStreams = new std::list<OleStream*>();
   try {
      LPSTORAGE pStorage = openStorage(argStgName);
      if (pStorage) {
         LPENUMSTATSTG lpEnum;
         if (pStorage->EnumElements(0, nullptr, 0, &lpEnum) == S_OK) {
            STATSTG ss;
            ULONG   ulcount;
            // Go through this once - finding all stream in this storage enumerate one element
            while (lpEnum->Next(1, &ss, &ulcount) == S_OK) {
               // Possible values for ss.type are: STGTY_...STREAM|STORAGE|LOCKBYTES|PROPERTY
               if (ss.type == STGTY_STREAM) {
                  LPSTREAM leStream;
                  DWORD grfMode = STGM_READ | STGM_SHARE_EXCLUSIVE;
                  if (pStorage->OpenStream(ss.pwcsName, nullptr, grfMode, 0, &leStream) == S_OK) {
                     //Add Stream data in map
                     try {
                         ULONG taille = ss.cbSize.LowPart + 1;
                         char* pBuffer = new char[taille];
                         ULONG rLength = 0;
                         memset(pBuffer, 0, taille);
                         if (leStream->Read(pBuffer, taille, &rLength) == S_OK) {
                             if (m_bEncrypt)
                                 encrypt((BYTE*)pBuffer, rLength); // Decrypt data with the same algorithm
                             // Push data in list
                             char ssName[MAX_OLECHAR];
                             memset(ssName, 0, MAX_OLECHAR);
                             wcstombs(ssName, ss.pwcsName, MAX_OLECHAR);
                             pLesStreams->push_back(new OleStream(ssName, pBuffer));
                         }
                         delete [] pBuffer;
                     } catch(...) {
                     }
                     leStream->Release();
                  }
               }
               // Free up the storage name
               LPMALLOC pIMalloc;
               CoGetMalloc(MEMCTX_TASK, &pIMalloc );
               pIMalloc->Free((LPVOID)ss.pwcsName);
               pIMalloc->Release();
            } // while
            lpEnum->Release();
         }
         pStorage->Release();
      }
   } catch(...) {
   }
   return pLesStreams;
}

ULONG OleFileManager::retrieveStream(const char* argStgName, const char* argStmName, char* argData, UINT argDataLen)
{
   ULONG DataSize = 0;
   try {
       OLECHAR pwcsval[MAX_PATH];
       LPSTORAGE pStorage = openStorage(argStgName);
       if (pStorage) {
           LPSTREAM pStream;
           mbstowcs(pwcsval, argStmName, MAX_PATH);
           DWORD grfMode = STGM_READ | STGM_SHARE_EXCLUSIVE;
           IResourceLock locker(&m_mutex);
           HRESULT hResult = pStorage->OpenStream(pwcsval, nullptr, grfMode, 0, &pStream);
           if (hResult == S_OK) {
               if (pStream->Read(argData, argDataLen, &DataSize) == S_OK) {
                  if (m_bEncrypt)
                      encrypt((BYTE*)argData, DataSize);
               }
               pStream->Release();
           }
           pStorage->Release();
       }
   } catch(...) {
   }
   return DataSize;
}

/*############################################################################*
 *                COUMPOUND DOCUMENT FILE DEFRAGMENTE FUNCTIONS               *
 *############################################################################*/
// Free up the Storage Disk Space
bool OleFileManager::defragmentFile()
{
   IResourceLock locker(&m_mutex);
   // Create a temporary file.  We don't use DELETEONRELEASE in case we have to save it when copying over the old file fails.
   LPSTORAGE pStorageNew;
   DWORD dwMode = STGM_CREATE | STGM_READWRITE | STGM_TRANSACTED | STGM_SHARE_EXCLUSIVE;
   //DWORD dwMode = STGM_DIRECT_SWMR | STGM_READWRITE | STGM_SHARE_DENY_WRITE;
   HRESULT hResult = StgCreateDocfile(nullptr, dwMode, 0, &pStorageNew);
   if ((hResult != NOERROR) || (pStorageNew == nullptr))
      return false;

   // Copy from one to the other
   if (m_pRootStg->CopyTo(0, nullptr, nullptr, pStorageNew) != S_OK) {
      pStorageNew->Release();
      return false;
   } else
       pStorageNew->Commit(STGC_ONLYIFCURRENT);

   // Get the name of the temp file
   STATSTG st;
   m_pRootStg->Release();
   m_pRootStg = nullptr;
   pStorageNew->Stat(&st, 0);
   pStorageNew->Release();

   // Delete the old file and copy the new one to the correct location
   if (DeleteFile(m_docFile) == 0)
      return false;

   TCHAR szTemp[MAX_PATH];
   WideCharToMultiByte(CP_ACP, 0, st.pwcsName, -1, szTemp, MAX_PATH, nullptr, nullptr);

   // Move this file to the old location
   if (MoveFile(szTemp, m_docFile) == 0)
      return false;

   // Reopen this new storage file
   if (!openDatabase())
      return false;

    return true;
}

void OleFileManager::initDb(const std::string& dbPathName, std::list<std::string>& listStorages)
{
    auto pOLeMgr = new OleFileManager(dbPathName.c_str());
    if (!pOLeMgr->openDatabase())
        pOLeMgr->createDatabase();
    for (const std::string& stgName : listStorages) {
        LPSTORAGE pStorage = pOLeMgr->openOrCreateStorage(stgName.c_str());
        pStorage->Release();
    }
    delete pOLeMgr;
}