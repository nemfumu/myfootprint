//
// Created by admin on 26/01/2024.
//

#include "FootPrintService.h"

#include <ws2tcpip.h>
#include <iostream>
#include <map>
#include <random>
#include <fstream>
#include <cassert>
#include <cstring>
#include "Shlwapi.h"

#include "Common.h"
#include "ServiceTimer.h"
#include "ServiceConfig.h"
#include "ConfigParser.h"
#include "ServiceLogger.h"
#include "SystemInformation.h"
#include "FootPrintException.h"

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
//#pragma comment (lib, "Mswsock.lib")

#undef UNICODE

#define MAX_STG_NAME        		32
#define DEFAULT_BUFLEN      		4096
#define DEFAULT_PORT        		"19001"

#define FOOTPRINTAGENT_INIFILE  	"D:\\Ecogestes\\FootPrintAgent\\cmake-build-debug\\FootPrintAgent.ini"

#define NONE_T                 	"none"
#define HEARTHBEAT_REQUEST_T		"HEARTHBEAT_REQUEST"
#define HEARTHBEAT_REPLY_T     	"HEARTHBEAT_REPLY"
#define STORAGES_T					"STORAGES"
#define STREAMS_T						"STREAMS"

std::mutex FootPrintService::m_mutex = std::mutex();
FootPrintService *FootPrintService::m_instance = nullptr;

FootPrintService *FootPrintService::getInstance() {
	if (m_instance == nullptr) {
		m_mutex.lock();
		if (m_instance == nullptr)
			m_instance = new FootPrintService();
		m_mutex.unlock();
	}
	return m_instance;
}

FootPrintService::FootPrintService()
		  : m_svcStopEvent(FALSE), m_taskManager(nullptr), m_timersMap(), m_config(new ServiceCfg()),
			 m_CfgParser(nullptr), m_listenSocket(INVALID_SOCKET), m_users(), m_serverThread(INVALID_HANDLE_VALUE),
			 m_serviceThread(INVALID_HANDLE_VALUE) {}

FootPrintService::~FootPrintService() {
	for (auto &it: m_timersMap)
		delete it.second;

	releaseServer();

	delete m_taskManager;
	delete m_config;
	delete m_CfgParser;
	delete ServiceLogger::getInstance();
}

void FootPrintService::releaseServer() {
	closesocket(m_listenSocket);
	for (auto user: m_users) {
		closesocket(user.first);
		CloseHandle(user.second->hThread);
	}
	WSACleanup();
}

void FootPrintService::loadConfig() {
	try {
		// Update m_config from FootPrintAgent.ini file
		m_CfgParser = new ConfigParser(FOOTPRINTAGENT_INIFILE);
		m_config->setDbName(m_CfgParser->get("db", "name"));
		m_config->setDbPath(m_CfgParser->get("db", "path"));

		m_config->setLogName(m_CfgParser->get("logger", "name"));
		m_config->setLogPath(m_CfgParser->get("logger", "path"));

		m_config->setServerPort(m_CfgParser->get("server", "listenPort"));
		m_config->setServerName(m_CfgParser->get("server", "serverName"));
	} catch (std::exception &e) {
		std::cout << Common::getLastErrorAsString("FootPrintService::loadConfig()") << std::endl;
		//See windows log events
	}
}

bool FootPrintService::initialize() {
	loadConfig();
	ServiceLogger::getInstance()->initialize(m_config);

	initDb();

	m_taskManager = new TaskManager();
	m_taskManager->initialize(m_config);

	createTimers();

	initServer();
	return (m_serverThread != INVALID_HANDLE_VALUE);
}

void FootPrintService::initDb() {
	char buffer[MAX_STG_NAME * 2];
	char stgName[MAX_STG_NAME];
	// Create or update database for new kpi
	std::string dbPathName(m_config->getDbPath());
	dbPathName.append("\\").append(m_config->getDbName());
	std::list<std::string> StgNames;
	size_t size = sizeof(kpi::kpiTable) / sizeof(kpi::KPI);
	for (int i = 0; i < size; i++) {
		std::memset(buffer, 0, MAX_STG_NAME * 2);
		std::memset(stgName, 0, MAX_STG_NAME);
		sprintf(buffer, "%02d-%s", (int) kpi::kpiTable[i].code, kpi::kpiTable[i].name);
		std::strncpy(stgName, buffer, std::min(MAX_STG_NAME - 1, (int) std::strlen(buffer)));
		StgNames.push_back(buffer);
	}
	PersistanceManager::initDb(dbPathName, StgNames);
	// Check database created
	std::ifstream dbf;
	dbf.open(dbPathName, std::ifstream::in);
	assert(dbf.is_open());
	dbf.close();
}

void FootPrintService::initServer() {
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		ServiceLogger::getInstance()->write(Common::getLastErrorAsString("FootPrintServer::initialize()"));
		throw footprint_exception("FootPrintServer::initialize() - WSAStartup failed with error");
	}

	struct addrinfo *pAddrInfo = nullptr;
	struct addrinfo hints = {0};
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(m_config->getServerName().c_str(), m_config->getServerPort().c_str(), &hints, &pAddrInfo);
	if (iResult != 0) {
		WSACleanup();
		Common::getLastErrorAsString("FootPrintServer::initialize()");
		throw footprint_exception("FootPrintServer::initialize() - getaddrinfo failed with error");
	}

	m_listenSocket = socket(pAddrInfo->ai_family, pAddrInfo->ai_socktype, pAddrInfo->ai_protocol);
	if (m_listenSocket == INVALID_SOCKET) {
		freeaddrinfo(pAddrInfo);
		WSACleanup();
		Common::getLastErrorAsString("FootPrintServer::initialize()", WSAGetLastError());
		throw footprint_exception("FootPrintServer::initialize() - Socket failed with error");
	}

	// Set up the TCP listening socket
	iResult = bind(m_listenSocket, pAddrInfo->ai_addr, (int) pAddrInfo->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		freeaddrinfo(pAddrInfo);
		closesocket(m_listenSocket);
		WSACleanup();
		Common::getLastErrorAsString("FootPrintServer::initialize()", WSAGetLastError());
		throw footprint_exception("FootPrintServer::initialize() - Bind failed with error");
	}

	freeaddrinfo(pAddrInfo);

	iResult = listen(m_listenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		closesocket(m_listenSocket);
		WSACleanup();
		Common::getLastErrorAsString("FootPrintServer::initialize()", WSAGetLastError());
		throw footprint_exception("FootPrintServer::initialize() - Listen failed with error");
	}
	m_serverThread = CreateThread(nullptr, 0, FootPrintService::serverThread, this, 0, nullptr);
}

void FootPrintService::onNewClient(SOCKET socket, const sockaddr &sockAddr)
{
	char buffer[MAX_PATH];
	char value = 1;
	int iResult = setsockopt(socket, SOL_SOCKET, SO_KEEPALIVE, &value, sizeof(value));
	if (iResult != 0) {
		sprintf(buffer, "TCP keep alive failure (error %d)\n", iResult);
		ServiceLogger::getInstance()->write(buffer);
	}

	int dontLinger = 0;
	iResult = setsockopt(socket, SOL_SOCKET, SO_DONTLINGER, (LPSTR)&dontLinger, sizeof(int));
	if (iResult != 0) {
		sprintf(buffer, "DONTLINGER option setup failure (error %d)\n", iResult);
		ServiceLogger::getInstance()->write(buffer);
	}

	auto pUser = new UserInfo();
	pUser->saddr = {0};
	pUser->socket = socket;
	std::memcpy((void *) &pUser->saddr, (void *) &sockAddr, sizeof(sockaddr));

	auto sInfo = new ServiceInfo();
	sInfo->service = this;
	sInfo->userInfo = pUser;

	pUser->hThread = CreateThread(nullptr, 0, FootPrintService::serviceThread, sInfo, 0, nullptr);
	m_users.insert(std::pair<SOCKET, UserInfo *>(socket, pUser));
}

void FootPrintService::onDisconnectClient(SOCKET socket) {
	auto it = m_users.find(socket);
	if (it != m_users.end()) {
		closesocket(socket);
		m_users.erase(it);
	}
}

void FootPrintService::stop()
{
	ServiceLogger::getInstance()->write("FootPrintService::stop() - Stopping...");
	for (auto &it: m_timersMap) {
		ServiceTimer *pTimer = it.second;
		pTimer->stop();
	}
	m_taskManager->stop();

	// Disconnect all clients
	for (auto it: m_users) {
		int iResult = shutdown(it.first, SD_SEND);
		if (iResult == SOCKET_ERROR) {
			ServiceLogger::getInstance()->write(Common::getLastErrorAsString("FootPrintService::stop()", WSAGetLastError()));
		}
	}
	m_svcStopEvent = TRUE;
}

void FootPrintService::createTimers() {
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<int> distributor(30 * 60, 60 * 60);  // Between 30mn et 1h

	size_t size = sizeof(kpi::kpiTable) / sizeof(kpi::KPI);
	for (int i = 0; i < size; i++) {
		if (!kpi::kpiTable[i].isActif)
			continue;
		int interval;
		try {
			std::string timing = m_CfgParser->get("timers", kpi::kpiTable[i].name);
			interval = std::strtol(timing.c_str(), nullptr, 10);
		} catch (...) {
			interval = distributor(mt);
		}
		kpi::Code code = kpi::kpiTable[i].code;
		auto pTimer = new ServiceTimer(code, interval, m_taskManager->getQueue());
		m_timersMap.insert(std::make_pair(code, pTimer));
	}
	ServiceLogger::getInstance()->write("FootPrintService::createTimers() - Timers created !\n");
}

DWORD WINAPI FootPrintService::serverThread(void *lpParam) {
	auto myService = static_cast<FootPrintService *>(lpParam);
	ServiceLogger::getInstance()->write("FootPrintService::serverThread() : service running ...\n");

	fd_set fdReadSet;
	while (myService->isValid()) {
		try {
			FD_ZERO(&fdReadSet);
			FD_SET(myService->getListenSocket(), &fdReadSet);
			if (FD_ISSET(myService->getListenSocket(), &fdReadSet)) {
				sockaddr sockAddr = {0};
				socklen_t addrLen = sizeof(sockaddr);
				// Accept a client socket
				SOCKET clientSocket = accept(myService->getListenSocket(), &sockAddr, &addrLen);
				if (clientSocket == INVALID_SOCKET) {
					ServiceLogger::getInstance()->write(Common::getLastErrorAsString(
							  "FootPrintService::serverThread() - Accept failed with error: ", WSAGetLastError()));
					continue;
				}
				myService->onNewClient(clientSocket, sockAddr);
				Sleep(3000);
			} //else
			   //bServerStop = true;
		} catch (...) {
			ServiceLogger::getInstance()->write(Common::getLastErrorAsString("FootPrintService::serverThread"));
		}
	}
	ServiceLogger::getInstance()->write("FootPrintService::serverThread() : service stopped !\n");
	return EXIT_SUCCESS;
}

DWORD WINAPI FootPrintService::serviceThread(void *lpParam) {
	auto pSvcInfo = static_cast<ServiceInfo *>(lpParam);
	FootPrintService *myService = pSvcInfo->service;
	std::string header("FootPrintService::serviceThread() : service");
	{
		std::ostringstream oss;
		oss << header << " OK for client " << pSvcInfo->userInfo->socket << "\n";
		ServiceLogger::getInstance()->write(oss.str());
	}
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	bool bServiceStop = false;
	while (!bServiceStop && myService->isValid()) {
		try {
			std::memset(recvbuf, 0, DEFAULT_BUFLEN);
			int iResult = recv(pSvcInfo->userInfo->socket, recvbuf, recvbuflen, 0);
			if (iResult > 0) {
				std::string replystr;
				std::list<std::string> request = Common::split(recvbuf, ':');
				if (request.front() == HEARTHBEAT_REQUEST_T) {
					replystr.append(HEARTHBEAT_REPLY_T);
				} else if (request.front() == STORAGES_T) {
					std::list<std::string> *storages = myService->getPersistanceManager()->selectStorages();
					for (const std::string &data: *storages)
						replystr.append(data).append(";");
				} else if (request.front() == STREAMS_T) {
					std::string stgName = request.back();
					std::list<std::string> *streams = myService->getPersistanceManager()->loadStreams(stgName);
					//for (auto  data : *storages)
					//  replystr.append(data).append(";");
					int count = 1;
					auto it = streams->begin();
					while (streams->size() - count++ > 10)
						it++;
					for (; it != streams->end(); it++)
						replystr.append(*it).append(";");
				}
				if (replystr.length() > 0) {
					size_t datalen = replystr.length() + 1;
					char *replybuf = new char[datalen];
					std::memset(replybuf, 0, datalen);
					std::memcpy(replybuf, replystr.c_str(), replystr.length());

					// Echo the buffer back to the sender
					size_t iSendResult = send(pSvcInfo->userInfo->socket, replybuf, (int) datalen, 0);
					if (iSendResult == SOCKET_ERROR) {
						ServiceLogger::getInstance()->write(
								  Common::getLastErrorAsString("FootPrintService::serviceThread() - Send failed with error.",
																		 WSAGetLastError()));
					}
					delete[] replybuf;
				} else {
					char *replybuf = new char[INT32_LEN];
					std::memset(replybuf, 0, INT32_LEN);
					std::strncpy(replybuf, NONE_T, strlen(NONE_T));
					send(pSvcInfo->userInfo->socket, replybuf, std::strlen(replybuf), 0);
					delete[] replybuf;
				}
			} else if (iResult == 0) {
				myService->onDisconnectClient(pSvcInfo->userInfo->socket);
				bServiceStop = true;
			} else {
				int errorId = WSAGetLastError();
				ServiceLogger::getInstance()->write(
						  Common::getLastErrorAsString("FootPrintService::serviceThread() - recv failed with error. ",
																 errorId));
				myService->onDisconnectClient(pSvcInfo->userInfo->socket);
				bServiceStop = true; //errorId == WSAEINTR;
			}
			Sleep(1000);
		} catch (...) {
			ServiceLogger::getInstance()->write(Common::getLastErrorAsString("FootPrintService::ServiceWorkerThread"));
		}
	}
	{
		std::ostringstream oss;
		oss << header << "terminated for " << pSvcInfo->userInfo->socket << "\n";
		ServiceLogger::getInstance()->write(oss.str());
	}
	return EXIT_SUCCESS;
}