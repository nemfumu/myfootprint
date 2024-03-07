////////////////////////////////////////////////////////////////////////////////
//                            LOGGING SERVICE                                 //
////////////////////////////////////////////////////////////////////////////////
//    DATE      PROG.                                                         //
// DD-MMM-YYYY INIT. SIR    MODIFICATION DESCRIPTION                          //
// ---------- ----- ------ -------------------------------------------------- //
// 07/02/2024  E.MF  Creation.                                                //
////////////////////////////////////////////////////////////////////////////////

#ifndef FOOTPRINTAGENT_SERVICELOGGER_H
#define FOOTPRINTAGENT_SERVICELOGGER_H

#include <windows.h>
#include <string>
#include <mutex>
#include "ServiceConfig.h"

class ServiceLogger
{
public:
    virtual ~ServiceLogger();

    static ServiceLogger* getInstance();
    void WINAPI initialize(ServiceCfg* pCfg);

    bool WINAPI write(const std::string& msgLog);

private:
    ServiceLogger();
    std::string getTime();

    std::string m_filename;
    HANDLE m_handle;
    static std::mutex m_mutex;
    static ServiceLogger* m_instance;
};


#endif //FOOTPRINTAGENT_SERVICELOGGER_H
