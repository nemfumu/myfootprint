////////////////////////////////////////////////////////////////////////////////
//                          FOOTPRINTAGENT SERVICE                            //
////////////////////////////////////////////////////////////////////////////////
//    DATE      PROG.                                                         //
// DD-MMM-YYYY INIT. SIR    MODIFICATION DESCRIPTION                          //
// ---------- ----- ------ -------------------------------------------------- //
// 26/01/2024  E.MF  Creation File                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef FOOTPRINTAGENT_FOOTPRINTSERVICE_H
#define FOOTPRINTAGENT_FOOTPRINTSERVICE_H

#include <winsock2.h>
#include <windows.h>
#include <mutex>
#include <string>
#include <list>
#include <map>

#include "ServiceLogger.h"
#include "EcogesteKpi.h"
#include "ServiceTimer.h"
#include "TaskManager.h"
#include "ConfigParser.h"

class FootPrintService;
class ServiceLogger;

/*
 * Class UserInfo
 */
struct UserInfo
{
    SOCKET socket;
    struct sockaddr saddr;
    HANDLE hThread;
};

struct ServiceInfo
{
    FootPrintService* service;
    UserInfo* userInfo;
};

/*
 * Class FootPrintService
 */
class FootPrintService {
public:
    virtual ~FootPrintService();

    static FootPrintService* getInstance();

    bool initialize();
    void stop();
    bool isValid() const { return m_svcStopEvent == FALSE; }

    SOCKET getListenSocket() const { return m_listenSocket; }

protected:
    FootPrintService();
    // FootPrintService should not be cloneable nor assignable.
    FootPrintService(FootPrintService &other) = delete;
    void operator=(const FootPrintService &) = delete;

    static DWORD WINAPI serverThread(void* lpParam);
    static DWORD WINAPI serviceThread(void* lpParam);

    void createTimers();
    void initServer();
    void releaseServer();

    void onNewClient(SOCKET socket, const sockaddr& sockAddr);
    void onDisconnectClient(SOCKET socket);

    PersistanceManager* getPersistanceManager() const { return m_taskManager->getPersistanceManager(); };

private:
    void loadConfig();
    void initDb();

    bool m_svcStopEvent;
    static std::mutex m_mutex;
    static FootPrintService* m_instance;
    TaskManager* m_taskManager;
    std::map<kpi::Code, ServiceTimer*> m_timersMap;
    ServiceCfg* m_config;
    ConfigParser* m_CfgParser;

    SOCKET m_listenSocket;
    std::map<SOCKET, UserInfo*> m_users;
    HANDLE m_serverThread;
    HANDLE m_serviceThread;
};

#endif //FOOTPRINTAGENT_FOOTPRINTSERVICE_H
