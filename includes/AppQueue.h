/*
 * Created by admin on 07/02/2024.
 */

#ifndef FOOTPRINTAGENT_APPQUEUE_H
#define FOOTPRINTAGENT_APPQUEUE_H

#include <list>
#include <queue>

#include "Resources.h"
#include "Message.h"

class AppQueue
{
public:
    explicit AppQueue();
    virtual ~AppQueue() throw();

    long getCount();
    bool post(IMessage* argMsg);
    IMessage* pull();

private:
    Semaphore m_semaphore;
    Mutex     m_mutex;
    std::queue<IMessage*>* m_fifo;
};

#endif //FOOTPRINTAGENT_APPQUEUE_H
