////////////////////////////////////////////////////////////////////////////////
//                       RESOURCE_LOCK OBJECT - INTERFACE                     //
////////////////////////////////////////////////////////////////////////////////

//#include <cassert>
#include "../includes/ResourceLock.h"

//============================================================================//
// STRUCTURE   :                     IResourceLock                            //
// DESCRIPTION : Class for locking and unlocking resource.                    //
//============================================================================//
// Constructor
IResourceLock::IResourceLock(Mutex* pRes)
: m_resource(pRes)
{
    pRes->lock();
}

// Constructor
IResourceLock::IResourceLock(Semaphore* pRes)
: m_resource(pRes)
{
    pRes->acquire();
}

// Destructor
IResourceLock::~IResourceLock()
{
    m_resource->release();
}

/*
 * Class StdResourceLock
 */
/*
StdResourceLock::StdResourceLock(std::mutex* pMutex)
: m_mutex(pMutex)
, m_semaphore(INVALID_HANDLE_VALUE)
, m_type(StdResourceLock::Type::MUTEX)
{
    assert(m_mutex != nullptr);
    m_mutex->lock();
}

StdResourceLock::StdResourceLock(HANDLE hSem)
: m_mutex(nullptr)
, m_semaphore(hSem)
, m_type(StdResourceLock::Type::SEMAPHORE)
{
    WaitForSingleObject(m_semaphore, INFINITE);
}

StdResourceLock::~StdResourceLock()
{
    if (m_type == StdResourceLock::Type::MUTEX)
        m_mutex->unlock();
    else
        ReleaseSemaphore(m_semaphore, 1, nullptr);
};
*/
