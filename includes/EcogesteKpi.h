//
// Created by admin on 07/02/2024.
//

#ifndef FOOTPRINTAGENT_ECOGESTEKPI_H
#define FOOTPRINTAGENT_ECOGESTEKPI_H

#include <string>
#include <minwindef.h>

namespace kpi {

    enum Code {
        SCREEN_BRIGHTNESS = 0,
        SCREEN_STANDBY,
        USE_SCREENSAVER,
        WIFI_CONNEXION_TIME,
        POWER_SAVING_AND_SLEEP,
        PRINT_NUMBER,
        USED_SCREEN_NUMBER,
        STARTUP_APP_NUMBER,
        OPEN_WINDOWS_NUMBER,
        STORED_FILES_OVER_10MB,
        STORED_FILES_OVER_5MB_10MB,
        PC_TIME_WITHOUT_ACTION,
        TRASH_EMPTIED,
        EMAIL_TRASH_EMPTIED,
        FUW_BUT_NOT_IN_FAVORITES,
        UNUSED_APP_MORE_THAN_YEAR,
        BATTERY_OPTIMIZATION,
        TURN_OFF_UNUSED_PC,
        UNUSED_PLUGGED_DEVICES,
        EXCHANGED_DATA,
        BROWSERS_INSTANCES
    };

    typedef struct {
        Code code;
        char name[MAX_PATH / 2];
        bool isActif;
    } KPI;

    const kpi::KPI kpiTable[] = {
            {SCREEN_BRIGHTNESS,               "SCREEN_BRIGHTNESS", true},
            {SCREEN_STANDBY,                  "SCREEN_STANDBY", false},
            {USE_SCREENSAVER,                 "USE_SCREENSAVER", false},
            {WIFI_CONNEXION_TIME,             "WIFI_CONNEXION_TIME", false},
            {POWER_SAVING_AND_SLEEP,          "POWER_SAVING_AND_SLEEP", false},
            {PRINT_NUMBER,                    "PRINT_NUMBER", false},
            {USED_SCREEN_NUMBER,              "USED_SCREEN_NUMBER", false},
            {STARTUP_APP_NUMBER,              "STARTUP_APP_NUMBER", false},
            {OPEN_WINDOWS_NUMBER,             "OPEN_WINDOWS_NUMBER", false},
            {STORED_FILES_OVER_10MB,          "STORED_FILES_OVER_10MB", false},
            {STORED_FILES_OVER_5MB_10MB,      "STORED_FILES_OVER_5MB_10MB", false},
            {PC_TIME_WITHOUT_ACTION,          "PC_TIME_WITHOUT_ACTION", false},
            {TRASH_EMPTIED,                   "TRASH_EMPTIED", false},
            {EMAIL_TRASH_EMPTIED,             "EMAIL_TRASH_EMPTIED", false},
            {FUW_BUT_NOT_IN_FAVORITES,        "FUW_BUT_NOT_IN_FAVORITES", false},
            {UNUSED_APP_MORE_THAN_YEAR,       "UNUSED_APP_MORE_THAN_YEAR", false},
            {BATTERY_OPTIMIZATION,            "BATTERY_OPTIMIZATION", true},
            {TURN_OFF_UNUSED_PC,              "TURN_OFF_UNUSED_PC", false},
            {UNUSED_PLUGGED_DEVICES,          "UNUSED_PLUGGED_DEVICES", false},
            {EXCHANGED_DATA,                  "EXCHANGED_DATA", false},
            {BROWSERS_INSTANCES,              "BROWSERS_INSTANCES", true}
    };
}

#endif //FOOTPRINTAGENT_ECOGESTEKPI_H
