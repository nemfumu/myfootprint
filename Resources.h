////////////////////////////////////////////////////////////////////////////////
//                        SYNCHRONIZATION OBJECTS                             //
////////////////////////////////////////////////////////////////////////////////
//    DATE      PROG.                                                         //
// DD-MMM-YYYY INIT. SIR    MODIFICATION DESCRIPTION                          //
// ---------- ----- ------ -------------------------------------------------- //
// 04/02/2024  E.MF  Creation.                                                //
////////////////////////////////////////////////////////////////////////////////

#ifndef FOOTSPRINTAGENT_RESOURCES_H
#define FOOTSPRINTAGENT_RESOURCES_H

#include <windows.h>
#include <cstdio>
#include <cstdlib>

#include <sys/time.h>
#include <mutex>
#include <semaphore>

class IResource
{
public:
             IResource() {};
    virtual ~IResource()
    {}

    virtual void release() = 0;
};

/*
 * Class Mutex
 */
class Mutex : public IResource
{
public:
             Mutex() : IResource(), m_mutex() {};
    virtual ~Mutex() {};

    virtual void lock() { m_mutex.lock(); };
    virtual void release() { m_mutex.unlock(); };

private:
    std::mutex m_mutex;
};

/*
 * Class Semaphore
 */
class Semaphore : public IResource
{
public:
             Semaphore(unsigned int initialCount=0, int maxCount=INT32_MAX);
    virtual ~Semaphore() {
        CloseHandle(m_semaphore);
    };

    long getSize();
    void wait(unsigned int timeout=INFINITE);
    void post(long count=1);

    void acquire() { wait(); }
    virtual void release() { post(); }

private:
    HANDLE m_semaphore;
};

#endif

