//
// Created by admin on 05/02/2024.
//

#ifndef FOOTPRINTAGENT_MESSAGEDATA_H
#define FOOTPRINTAGENT_MESSAGEDATA_H

#include <minwindef.h>
#include <string>
#include "EcogesteKpi.h"

class MessageData
{
public:
    explicit MessageData(kpi::Code code) : m_pkiCode(code)
             {};
    virtual ~MessageData()
    {}

    kpi::Code getKpiCode() const { return m_pkiCode; }

protected:
    kpi::Code m_pkiCode;
};

#endif //FOOTPRINTAGENT_MESSAGEDATA_H
