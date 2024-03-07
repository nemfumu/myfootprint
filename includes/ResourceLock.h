////////////////////////////////////////////////////////////////////////////////
//                       RESOURCE_LOCK OBJECT - INTERFACE                     //
////////////////////////////////////////////////////////////////////////////////

#ifndef FOOTPRINTAGENT_RESOURCELOCK_H
#define FOOTPRINTAGENT_RESOURCELOCK_H

#include "Resources.h"

/*============================================================================//
// STRUCTURE   :                     IResourceLock                            //
// DESCRIPTION : Class for locking and unlocking resource.                    //
//============================================================================*/
class IResourceLock
{
public:
            IResourceLock(Mutex* pMutex);
            IResourceLock(Semaphore* pSem);
   virtual ~IResourceLock();

private:    //------------------ CLASS PRIVATE ATTRIBUTES -------------------
   IResource* m_resource;
};

/*
 * Class StdResourceLock
 */
/*
class StdResourceLock
{
    enum Type {
        MUTEX     = 0,
        SEMAPHORE = 1
    };

public:
    StdResourceLock(std::mutex* pMutex);
    StdResourceLock(HANDLE hSem);
    virtual ~StdResourceLock();

private:    //------------------ CLASS PRIVATE ATTRIBUTES -------------------
    std::mutex* m_mutex;
    HANDLE m_semaphore;
    StdResourceLock::Type m_type;
};
*/

#endif
