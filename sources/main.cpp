#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <cstdio>
#include <iostream>

#include "../includes/FootPrintService.h"
#include "../includes/Common.h"

#pragma comment(lib, "advapi32.lib")

#define SVCNAME TEXT("FootPrintAgent")

// MessageId: SVC_ERROR
// MessageText:
//  An error has occurred (%2).
#define SVC_ERROR                        ((DWORD)0xC0020001L)

SERVICE_STATUS          gSvcStatus;
SERVICE_STATUS_HANDLE   gSvcStatusHandle;
HANDLE                  ghSvcStopEvent = nullptr;

VOID SvcInstall(VOID);
VOID WINAPI SvcCtrlHandler(DWORD);
VOID WINAPI SvcMain(DWORD, LPTSTR*);

VOID ReportSvcStatus(DWORD, DWORD, DWORD);
VOID SvcInit(DWORD, LPTSTR*);
VOID SvcReportEvent(LPTSTR);

FootPrintService* myServiceWorker = nullptr;
/*
// Purpose:
//   Entry point for the process
//
// Parameters:
//   None
//
// Return value:
//   None, defaults to 0 (zero)
*/
int __cdecl _tmain(int argc, TCHAR *argv[])
{
    // If command-line parameter is "install", install the service.
    // Otherwise, the service is probably being started by the SCM.

    if( lstrcmpi( argv[1], TEXT("install")) == 0 ) {
        SvcInstall();
        return 0;
    }

    // TO_DO: Add any additional services for the process to this table.
    SERVICE_TABLE_ENTRY DispatchTable[] = {
                            { SVCNAME, (LPSERVICE_MAIN_FUNCTION)SvcMain },
                            { nullptr, nullptr }
    };

    // This call returns when the service has stopped.
    // The process should simply terminate when the call returns.

    if (!StartServiceCtrlDispatcher( DispatchTable )) {
        SvcReportEvent(TEXT("StartServiceCtrlDispatcher"));
        std::cout << Common::getLastErrorAsString("StartServiceCtrlDispatcher") << std::endl;
    }
}

//
// Purpose:
//   Installs a service in the SCM database
//
// Parameters:
//   None
//
// Return value:
//   None
//
VOID SvcInstall()
{
    SC_HANDLE schSCManager;
    SC_HANDLE schService;
    TCHAR szUnquotedPath[MAX_PATH];

    if (!GetModuleFileName(nullptr, szUnquotedPath, MAX_PATH)) {
        std::cout << "Cannot install service ! \n"
                  << Common::getLastErrorAsString("GetModuleFileName").c_str() << std::endl;
        return;
    }

    // In case the path contains a space, it must be quoted so that
    // it is correctly interpreted. For example,
    // "d:\my share\myservice.exe" should be specified as
    // ""d:\my share\myservice.exe"".
    TCHAR szPath[MAX_PATH];
    StringCbPrintf(szPath, MAX_PATH, TEXT("\"%s\""), szUnquotedPath);

    // Get a handle to the SCM database.
    schSCManager = OpenSCManager(nullptr,                   // local computer
                                 nullptr,                   // ServicesActive database
                                 SC_MANAGER_ALL_ACCESS);    // full access rights
    if (nullptr == schSCManager) {
        std::cout << Common::getLastErrorAsString("OpenSCManager") << std::endl;
        return;
    }

    // Create the service
    schService = CreateService(schSCManager,              // SCM database
                               SVCNAME,                   // name of service
                               SVCNAME,                   // service name to display
                               SERVICE_ALL_ACCESS,        // desired access
                               SERVICE_WIN32_OWN_PROCESS, // service type
                               SERVICE_DEMAND_START,      // start type
                               SERVICE_ERROR_NORMAL,      // error control type
                               szPath,                    // path to service's binary
                               nullptr,                   // no load ordering group
                               nullptr,                   // no tag identifier
                               nullptr,                   // no dependencies
                               nullptr,                   // LocalSystem account
                               nullptr);                  // no password

    if (schService == nullptr) {
        std::cout << Common::getLastErrorAsString("CreateService") << std::endl;
        CloseServiceHandle(schSCManager);
        return;
    }
    else
        printf("Service installed successfully\n");

    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);
}

//
// Purpose:
//   Entry point for the service
//
// Parameters:
//   dwArgc - Number of arguments in the lpszArgv array
//   lpszArgv - Array of strings. The first string is the name of
//     the service and subsequent strings are passed by the process
//     that called the StartService function to start the service.
//
// Return value:
//   None.
//
VOID WINAPI SvcMain( DWORD dwArgc, LPTSTR* lpszArgv )
{
    // Register the handler function for the service
    gSvcStatusHandle = RegisterServiceCtrlHandler(SVCNAME, SvcCtrlHandler);
    if (!gSvcStatusHandle) {
        SvcReportEvent(TEXT("RegisterServiceCtrlHandler"));
        return;
    }

    // These SERVICE_STATUS members remain as set here
    gSvcStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    gSvcStatus.dwServiceSpecificExitCode = 0;

    // Report initial status to the SCM
    ReportSvcStatus( SERVICE_START_PENDING, NO_ERROR, 3000 );

    // Perform service-specific initialization and work.
    SvcInit( dwArgc, lpszArgv );
}

//
// Purpose:
//   The service code
//
// Parameters:
//   dwArgc - Number of arguments in the lpszArgv array
//   lpszArgv - Array of strings. The first string is the name of
//     the service and subsequent strings are passed by the process
//     that called the StartService function to start the service.
//
// Return value:
//   None
//
VOID SvcInit( DWORD dwArgc, LPTSTR *lpszArgv)
{
    // TO_DO: Declare and set any required variables.
    //   Be sure to periodically call ReportSvcStatus() with
    //   SERVICE_START_PENDING. If initialization fails, call
    //   ReportSvcStatus with SERVICE_STOPPED.

    // Create an event. The control handler function, SvcCtrlHandler,
    // signals this event when it receives the stop control code.
    ghSvcStopEvent = CreateEvent(nullptr,    // default security attributes
                                 TRUE,    // manual reset event
                                 FALSE,   // not signaled
                                 nullptr);   // no name
    if ( ghSvcStopEvent == nullptr) {
        ReportSvcStatus(SERVICE_STOPPED, GetLastError(), 0);
        return;
    }

    // Report running status when initialization is complete.
    ReportSvcStatus(SERVICE_RUNNING, NO_ERROR, 0);

    // TO_DO: Perform work until service stops.
    myServiceWorker = FootPrintService::getInstance();
    if (!myServiceWorker->initialize()) {
        ReportSvcStatus(SERVICE_STOPPED, GetLastError(), 0);
        delete myServiceWorker;
        return;
    }
    while (true) {
        // Check whether to stop the service.
        WaitForSingleObject(ghSvcStopEvent, INFINITE);
        ReportSvcStatus( SERVICE_STOPPED, NO_ERROR, 0 );
        return;
    }
}

//
// Purpose:
//   Sets the current service status and reports it to the SCM.
//
// Parameters:
//   dwCurrentState - The current state (see SERVICE_STATUS)
//   dwWin32ExitCode - The system error code
//   dwWaitHint - Estimated time for pending operation,
//     in milliseconds
//
// Return value:
//   None
//
VOID ReportSvcStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint)
{
    static DWORD dwCheckPoint = 1;

    // Fill in the SERVICE_STATUS structure.
    gSvcStatus.dwCurrentState = dwCurrentState;
    gSvcStatus.dwWin32ExitCode = dwWin32ExitCode;
    gSvcStatus.dwWaitHint = dwWaitHint;

    if (dwCurrentState == SERVICE_START_PENDING)
        gSvcStatus.dwControlsAccepted = 0;
    else
        gSvcStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;

    if ((dwCurrentState == SERVICE_RUNNING) || (dwCurrentState == SERVICE_STOPPED))
        gSvcStatus.dwCheckPoint = 0;
    else
        gSvcStatus.dwCheckPoint = dwCheckPoint++;

    // Report the status of the service to the SCM.
    SetServiceStatus( gSvcStatusHandle, &gSvcStatus );
}

//
// Purpose:
//   Called by SCM whenever a control code is sent to the service
//   using the ControlService function.
//
// Parameters:
//   dwCtrl - control code
//
// Return value:
//   None
//
VOID WINAPI SvcCtrlHandler( DWORD dwCtrl )
{
    // Handle the requested control code.
    switch (dwCtrl) {
        case SERVICE_CONTROL_STOP:
            ReportSvcStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);

            // Signal the service to stop.
            SetEvent(ghSvcStopEvent);
            ReportSvcStatus(gSvcStatus.dwCurrentState, NO_ERROR, 0);
            if (myServiceWorker)
                myServiceWorker->stop();
            return;
        case SERVICE_CONTROL_INTERROGATE:
            break;
        default:
            break;
    }
}

//
// Purpose:
//   Logs messages to the event log
//
// Parameters:
//   szFunction - name of function that failed
//
// Return value:
//   None
//
// Remarks:
//   The service must have an entry in the Application event log.
//
VOID SvcReportEvent(LPTSTR szFunction)
{
    HANDLE hEventSource;
    LPCTSTR lpszStrings[2];
    TCHAR Buffer[255];

    hEventSource = RegisterEventSource(nullptr, SVCNAME);

    if (nullptr != hEventSource ) {
        StringCchPrintf(Buffer, 80, TEXT("%s failed with %s"), szFunction, Common::getLastErrorAsString("").c_str());

        lpszStrings[0] = SVCNAME;
        lpszStrings[1] = Buffer;
        ReportEvent(hEventSource,        // event log handle
                    EVENTLOG_ERROR_TYPE, // event type
                    0,                   // event category
                    SVC_ERROR,           // event identifier
                    nullptr,                // no security identifier
                    2,                   // size of lpszStrings array
                    0,                   // no binary data
                    lpszStrings,         // array of strings
                    nullptr);               // no binary data

        DeregisterEventSource(hEventSource);
    }
}
