////////////////////////////////////////////////////////////////////////////////
//                         DATA STORE - IMPLEMENTATION                        //
////////////////////////////////////////////////////////////////////////////////
//============================================================================//
// MODIFICATIONS:                                                             //
//------------  --------------------------------------------------------------//
// 17/11/2003: Ajout ExeTransactionId dans CDataStore::GetDataAsString et dans//
//             CDataStore::CreateTransaction.                                 //
//                                                                            //
// 13/02/2004: Ajout Transaction Type.                                        //
// 18/02/2004: Ajout Transaction Index.                                       //
////////////////////////////////////////////////////////////////////////////////

#include <process.h>
#include <cstdlib>
#include <io.h>

#include<iostream>
#include<fstream>
#include<ostream>
#include <cstdio>
#include<istream>
#include<sstream>
//#include "FileCtrl.hpp"

#include "Common.h"
#include "ParserObject.h"
#include "DataStore.h"

using namespace std;

//============================================================================//
// STRUCTURE   :                 DataStore                                    //
// DESCRIPTION : Classe Generique pilotant la persistence des donnees. Elle   //
//               assure aussi la sauvegarde globale des transaction.          //
//============================================================================//
// Constructor
DataStore::DataStore(const std::string& dbName, const std::string& dbPath)
: m_dbName(dbName)
, m_dbPath(dbPath)
, m_swapDataBuffer(new char[MAX_SWAP_BUFFER])
{
   memset(m_swapDataBuffer, 0, MAX_SWAP_BUFFER);
};

// Destructor
DataStore::~DataStore()
{
    delete [] m_swapDataBuffer;
    m_swapDataBuffer = NULL;
};

