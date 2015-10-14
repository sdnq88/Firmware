#include "factorytest.h"

#include <stdio.h>
#include <string.h>
#include "../displayhelper.h"
#include "../datamanager.h"

namespace modes
{

FactoryTest::FactoryTest() :
    pressedButtonMask(0)
{
    DisplayHelper::showTest();
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
    const int lineCount = 2;
    const int line1Size = 100;
    char line1[line1Size];
    const int line2Size = 100;
    char line2[line2Size];
    const char *list[] = {
        line1,
        line2,
    };

    bool needButtons = false;
    memset(line1, 0, sizeof(line1));
    memset(line2, 0, sizeof(line2));

    snprintf(line1, line1Size, "buttons to press:");

    if (key_pressed(BTN_POWER))
    {
        pressedButtonMask |= BTN_POWER;
    }
    else if (key_pressed(BTN_PLAY))
    {
        pressedButtonMask |= BTN_PLAY;
    }
    else if (key_pressed(BTN_UP))
    {
        pressedButtonMask |= BTN_UP;
    }
    else if (key_pressed(BTN_DOWN))
    {
        pressedButtonMask |= BTN_DOWN;
    }
    else if (key_pressed(BTN_LEFT))
    {
        pressedButtonMask |= BTN_LEFT;
    }
    else if (key_pressed(BTN_RIGHT))
    {
        pressedButtonMask |= BTN_RIGHT;
    }
    else if (key_pressed(BTN_TO_ME))
    {
        pressedButtonMask |= BTN_TO_ME;
    }
    else if (key_pressed(BTN_TO_H))
    {
        pressedButtonMask |= BTN_TO_H;
    }
    else if (key_pressed(BTN_MODE))
    {
        pressedButtonMask |= BTN_MODE;
    }
    else if (key_pressed(BTN_FUTURE))
    {
        pressedButtonMask |= BTN_FUTURE;
    }

    int length = 0;

    if (!(pressedButtonMask & BTN_POWER))
    {
        needButtons = true;
        length += snprintf(line2 + length, line2Size - length, "POWER ");
    }
    if (!(pressedButtonMask & BTN_PLAY))
    {
        needButtons = true;
        length += snprintf(line2 + length, line2Size - length, "PLAY ");
    }
    if (!(pressedButtonMask & BTN_UP))
    {
        needButtons = true;
        length += snprintf(line2 + length, line2Size - length, "UP ");
    }
    if (!(pressedButtonMask & BTN_DOWN))
    {
        needButtons = true;
        length += snprintf(line2 + length, line2Size - length, "DOWN ");
    }
    if (!(pressedButtonMask & BTN_LEFT))
    {
        needButtons = true;
        length += snprintf(line2 + length, line2Size - length, "LEFT ");
    }
    if (!(pressedButtonMask & BTN_RIGHT))
    {
        needButtons = true;
        length += snprintf(line2 + length, line2Size - length, "RIGHT ");
    }
    if (!(pressedButtonMask & BTN_TO_ME))
    {
        needButtons = true;
        length += snprintf(line2 + length, line2Size - length, "TO_ME ");
    }
    if (!(pressedButtonMask & BTN_TO_H))
    {
        needButtons = true;
        length += snprintf(line2 + length, line2Size - length, "TO_HOME ");
    }
    if (!(pressedButtonMask & BTN_MODE))
    {
        needButtons = true;
        length += snprintf(line2 + length, line2Size - length, "MODE ");
    }
    if (!(pressedButtonMask & BTN_FUTURE))
    {
        needButtons = true;
        length += snprintf(line2 + length, line2Size - length, "FUTURE ");
    }

    if(!needButtons)
    {
        snprintf(line2, line2Size, "All buttons pressed");
    }

    if (orbId == FD_KbdHandler)
    {
        setList(list, lineCount);
    }
    else
    {
        DisplayHelper::showTest();
    }

    return nextMode;
}

}
