#pragma once

#include <time.h>

#include "base.h"

namespace modes
{

class SensorValidation : public Base
{
public:
    SensorValidation();

    virtual int getTimeout();
    virtual void listenForEvents(bool awaitMask[]);
    virtual Base* doEvent(int orbId);

protected:
    enum class Status
    {
        Unknown,
        GetStatus,
        Required,
        OkToStart,
        Countdown,
        Started
    };

    Status status;
    time_t timeStarted;
};

}
