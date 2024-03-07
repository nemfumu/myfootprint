cmake_minimum_required(VERSION 3.26)
project(FootPrintAgent)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/sources")

set(SOURCES_FILES sources/main.cpp
        sources/AppQueue.cpp
        sources/Common.cpp
        sources/DataStore.cpp
        sources/EcogesteKpi.cpp
        sources/FootPrintService.cpp
        sources/Message.cpp
        sources/MessageData.cpp
        sources/OleFileManager.cpp
        sources/OleFileStore.cpp
        sources/ParserObject.cpp
        sources/PersistanceManager.cpp
        sources/PersistentData.cpp
        sources/ResourceLock.cpp
        sources/Resources.cpp
        sources/ServiceConfig.cpp
        sources/ServiceInformationFactory.cpp
        sources/ServiceLogger.cpp
        sources/ServiceTask.cpp
        sources/ServiceTimer.cpp
        sources/SystemInfoHelper.cpp
        sources/SystemInformation.cpp
        sources/TaskManager.cpp
        sources/ConfigParser.cpp
        sources/ConfigParser.cpp
)

add_executable(FootPrintAgent ${SOURCES_FILES}
)

target_link_libraries(FootPrintAgent PRIVATE dxva2)
target_link_libraries(FootPrintAgent PRIVATE ole32)
target_link_libraries(FootPrintAgent PRIVATE Ws2_32)