////////////////////////////////////////////////////////////////////////////////
//                         CONFIGURATION OBJECTS                              //
////////////////////////////////////////////////////////////////////////////////
//    DATE      PROG.                                                         //
// DD-MMM-YYYY INIT. SIR    MODIFICATION DESCRIPTION                          //
// ---------- ----- ------ -------------------------------------------------- //
// 08/02/2024  E.MF  Creation.                                                //
////////////////////////////////////////////////////////////////////////////////

#ifndef FOOTPRINTAGENT_SERVICECONFIG_H
#define FOOTPRINTAGENT_SERVICECONFIG_H

#include <string>

/*
 * Class ServiceCfg
 */
class ServiceCfg
{
public:
    ServiceCfg();
    virtual ~ServiceCfg() {};

    std::string getDbPath() const { return m_dbPath; }
    std::string getDbName() const { return m_dbName; }
    std::string getLogPath() const { return m_logPath; }
    std::string getLogName() const { return m_logName; }
    std::string getServerPort() const { return m_serverPort; }
    std::string getServerName() const { return m_serverName; }

    void setDbPath(const std::string& path) { m_dbPath = path; }
    void setDbName(const std::string& name) { m_dbName = name; }
    void setLogPath(const std::string& path) { m_logPath = path; }
    void setLogName(const std::string& name) { m_logName = name; }

    void setServerPort(const std::string& port) { m_serverPort = port; }
    void setServerName(const std::string& name) { m_serverName = name; }

private:
    std::string m_dbPath;
    std::string m_dbName;
    std::string m_logPath;
    std::string m_logName;
    std::string m_serverPort;
    std::string m_serverName;
};

#endif //FOOTPRINTAGENT_SERVICECONFIG_H
