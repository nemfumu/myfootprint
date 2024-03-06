cmake_minimum_required(VERSION 3.26)
project(FootPrintAgent)

set(CMAKE_CXX_STANDARD 17)

add_executable(FootPrintAgent main.cpp
        AppQueue.cpp
        Common.cpp
        DataStore.cpp
        EcogesteKpi.cpp
        FootPrintService.cpp
        Message.cpp
        MessageData.cpp
        OleFileManager.cpp
        OleFileStore.cpp
        ParserObject.cpp
        PersistanceManager.cpp
        PersistentData.cpp
        ResourceLock.cpp
        Resources.cpp
        ServiceConfig.cpp
        ServiceInformationFactory.cpp
        ServiceLogger.cpp
        ServiceTask.cpp
        ServiceTimer.cpp
        SystemInfoHelper.cpp
        SystemInformation.cpp
        TaskManager.cpp
        ConfigParser.cpp
        ConfigParser.cpp
        FootPrintException.h
)

target_link_libraries(FootPrintAgent PRIVATE dxva2)
target_link_libraries(FootPrintAgent PRIVATE ole32)
target_link_libraries(FootPrintAgent PRIVATE Ws2_32)