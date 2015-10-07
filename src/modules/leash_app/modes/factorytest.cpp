#include "factorytest.h"

#include <stdio.h>
#include <string.h>
#include "../displayhelper.h"
#include "../datamanager.h"

namespace modes
{

FactoryTest::FactoryTest()
{
    DisplayHelper::showLogo();
}

FactoryTest::~FactoryTest()
{
}

void FactoryTest::listenForEvents(bool awaitMask[])
{
    awaitMask[FD_KbdHandler] = 1;
}

int FactoryTest::getTimeout()
{
    return 5000;
}

Base* FactoryTest::doEvent(int orbId)
{
    Base *nextMode = nullptr;
    const int lineCount = 1;
    char line1[100];
    const char *list[] = {
        line1
    };

    memset(line1, 0, sizeof(line1));

    if (key_pressed(BTN_POWER))
    {
        snprintf(line1, sizeof(line1) - 1, "POWER pressed");
    }
    else if (key_pressed(BTN_PLAY))
    {
        snprintf(line1, sizeof(line1) - 1, "PLAY pressed");
    }
    else if (key_pressed(BTN_UP))
    {
        snprintf(line1, sizeof(line1) - 1, "UP pressed");
    }
    else if (key_pressed(BTN_DOWN))
    {
        snprintf(line1, sizeof(line1) - 1, "DOWN pressed");
    }
    else if (key_pressed(BTN_LEFT))
    {
        snprintf(line1, sizeof(line1) - 1, "LEFT pressed");
    }
    else if (key_pressed(BTN_RIGHT))
    {
        snprintf(line1, sizeof(line1) - 1, "RIGHT pressed");
    }
    else if (key_pressed(BTN_TO_ME))
    {
        snprintf(line1, sizeof(line1) - 1, "TO ME pressed");
    }
    else if (key_pressed(BTN_TO_H))
    {
        snprintf(line1, sizeof(line1) - 1, "TO HOME pressed");
    }
    else if (key_pressed(BTN_MODE))
    {
        snprintf(line1, sizeof(line1) - 1, "MODE pressed");
    }
    else if (key_pressed(BTN_FUTURE))
    {
        snprintf(line1, sizeof(line1) - 1, "FUTURE pressed");
    }

    if (orbId == FD_KbdHandler)
    {
        setList(list, lineCount);
    }
    else
    {
        DisplayHelper::showLogo();
    }

    return nextMode;
}

}
