//---------------------------------------------------------------------------
//  Mutex, semaphores, event et thread safe.
//---------------------------------------------------------------------------
#include "Resources.h"

Semaphore::Semaphore(unsigned int initialCount, int maxCount)
: IResource()
, m_semaphore()
{
    m_semaphore = CreateSemaphore(nullptr,            // default security attributes
                                  initialCount,       // initial count
                                  maxCount,           // maximum count
                                  nullptr);
}

long Semaphore::getSize()
{
    long currentCount = 0;
    ReleaseSemaphore(m_semaphore, 0, &currentCount);
    return currentCount;
}

void Semaphore::wait(unsigned int timeout)
{
    WaitForSingleObject(m_semaphore, timeout);
}

void Semaphore::post(long count)
{
    ReleaseSemaphore(m_semaphore, count, nullptr);
}
