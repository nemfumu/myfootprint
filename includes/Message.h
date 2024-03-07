////////////////////////////////////////////////////////////////////////////////
//                      APPLICATION MESSAGE  - INTERFACE                      //
////////////////////////////////////////////////////////////////////////////////
//    DATE      PROG.                                                         //
// DD-MMM-YYYY INIT. SIR    MODIFICATION DESCRIPTION                          //
// ---------- ----- ------ -------------------------------------------------- //
// 07/02/2024  E.MF  Creation.                                                //
////////////////////////////////////////////////////////////////////////////////

#ifndef FOOTPRINTAGENT_APPMESSAGE_H
#define FOOTPRINTAGENT_APPMESSAGE_H

#include <cstdio>
#include <cstdlib>
#include <stdexcept>

#include "MessageData.h"

#define AGENT_MESSAGE_BASE                        0
#define AGENT_MESSAGE_END                         99

#define AGENT_TICK_MESSAGE                        (AGENT_MESSAGE_BASE + 0)
#define AGENT_TASK_MESSAGE                        (AGENT_MESSAGE_BASE + 1)
#define AGENT_HEARTBEAT_REQUEST                   (AGENT_MESSAGE_BASE + 2)
#define AGENT_HEARTBEAT_REPLY                     (AGENT_MESSAGE_BASE + 3)
#define AGENT_RAW_DATA                            (AGENT_MESSAGE_BASE + 4)
#define AGENT_THREAD_TERMINATE                    (AGENT_MESSAGE_BASE + 5)

/******************************************************************************
 * CLASSE      :                 APPLICATION MESSAGE                          *
 * DESCRIPTION : Cette classe encapsule les donnees necessaires a l'envoi     *
 *               d'un message a un destinataire ayant une queue.              *
 ******************************************************************************/
class IMessage
{
public:
             IMessage(MessageData* pData, int idMsg) : m_data(pData), m_msgId(idMsg) {};
    explicit IMessage(const IMessage& pMsg) : m_data(pMsg.m_data), m_msgId(pMsg.m_msgId) {};
    virtual ~IMessage() noexcept
    {
        discardData();
    };

    MessageData* getData() const { return m_data; };
    void detachData()  { m_data = nullptr; };

    void discardData() noexcept {
        if (m_data)
            delete m_data;
        m_data = nullptr;
    };

    int getMessageId() const { return m_msgId; }

protected:
    MessageData* m_data;
    int m_msgId;
};

#endif
