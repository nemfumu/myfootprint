//
// Created by admin on 07/02/2024.
//

#include "../includes/AppQueue.h"
#include "../includes/ResourceLock.h"

//---------------------------------------------------------------------------
// Fifo Chainee
//---------------------------------------------------------------------------
// Constructeurs
AppQueue::AppQueue()
: m_semaphore()
, m_mutex()
, m_fifo(new std::queue<IMessage*>())
{};

AppQueue::~AppQueue() noexcept
{
    try {
        IResourceLock locker(&m_mutex);
        while (!m_fifo->empty()) {
            IMessage* pMsg = m_fifo->front();
            m_fifo->pop();
            delete pMsg;
        }
    } catch(const std::exception& e) {
    }
};

//---------------------------------------------------------------------------
bool AppQueue::post(IMessage* pMsg)
{
    {
        IResourceLock locker(&m_mutex);
        m_fifo->push(pMsg);
    }
    m_semaphore.post();
    return true;
};

IMessage* AppQueue::pull()
{
    IMessage* pMsg = nullptr;
    m_semaphore.wait();            // wait for something to show up
    {
        IResourceLock locker(&m_mutex);
        pMsg = m_fifo->front();
        m_fifo->pop();
    }
    return pMsg;
};

long AppQueue::getCount()
{
    IResourceLock locker(&m_mutex);
    return (long)m_fifo->size();
}
//---------------------------------------------------------------------------

