#pragma once

#include "list.h"

namespace modes
{

class FactoryTest : public List
{
public:
    FactoryTest();
    ~FactoryTest();

    virtual int getTimeout();
    virtual void listenForEvents(bool awaitMask[]);
    virtual Base* doEvent(int orbId);

protected:
    int pressedButtonMask;

};

} // end of namespace modes

