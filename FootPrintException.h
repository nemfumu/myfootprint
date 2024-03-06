//
// Created by admin on 23/02/2024.
//

#ifndef FOOTPRINTAGENT_EXCEPTION_H
#define FOOTPRINTAGENT_EXCEPTION_H

#include <minwindef.h>
#include <exception>
#include <algorithm>

struct footprint_exception : std::exception
{
    explicit footprint_exception(const char* wmsg)
    {
        memset(_msg, 0, MAX_PATH);
        memcpy(_msg, wmsg, std::min(MAX_PATH-1, (int)strlen(wmsg)));
    }

    [[nodiscard]] const char* what() const noexcept override { return _msg; }

    char _msg[MAX_PATH]{};
};

#endif //FOOTPRINTAGENT_EXCEPTION_H
