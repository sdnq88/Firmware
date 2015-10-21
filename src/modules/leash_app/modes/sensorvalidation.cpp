#include "sensorvalidation.h"

#include <stdio.h>

#include <sensor_validation/sensor_validation.hpp>

#include "../displayhelper.h"
#include "../uorb_functions.h"
#include "main.h"
#include "menu.h"

namespace modes
{

SensorValidation::SensorValidation(bool forceStart)
{
    if (forceStart)
    {
        DisplayHelper::showInfo(INFO_SENSOR_VALIDATION_OK_TO_START, 0);
        status = Status::OkToStart;
    }
    else
    {
        status = Status::GetStatus;
        DisplayHelper::showInfo(INFO_SENSOR_VALIDATION_SHOW_STATUS, 0);
    }
}

int SensorValidation::getTimeout()
{
    int timeout = -1;

    switch (status)
    {
        case Status::Countdown:
            timeout = 1000;
            break;

        default:
            break;
    }

    return timeout;
}

void SensorValidation::listenForEvents(bool awaitMask[])
{
    awaitMask[FD_KbdHandler] = 1;

    switch (status)
    {
        case Status::Started:
        case Status::GetStatus:
            awaitMask[FD_AirdogStatus] = 1;
            awaitMask[FD_SensorStatus] = 1;
            break;

        default:
            break;
    }
}

Base* SensorValidation::doEvent(int orbId)
{
    Base *next = nullptr;

    switch (status)
    {
        case Status::GetStatus:
        {
            SENSOR_STATUS dogStatus = DataManager::instance()->airdog_status.sensor_status;
            SENSOR_STATUS leashStatus = DataManager::instance()->sensor_status.combined_status;
            int showStatus = 0;

            if (leashStatus == SENSOR_STATUS_OK)
            {
                showStatus |= 1;
            }
            else if (leashStatus < SENSOR_STATUS_CHECK_NEEDED)
            {
                // error
                showStatus |= 2;
            }

            if (dogStatus == SENSOR_STATUS_OK)
            {
                showStatus |= 1 << 4;
            }
            else if (dogStatus < SENSOR_STATUS_CHECK_NEEDED)
            {
                // error
                showStatus |= 2 << 4;
            }

            if (dogStatus == SENSOR_STATUS_OK && leashStatus == SENSOR_STATUS_OK)
            {
                next = new Main();
            }
            else if (dogStatus != SENSOR_STATUS_UNKNOWN && leashStatus != SENSOR_STATUS_UNKNOWN)
            {
                DisplayHelper::showInfo(INFO_SENSOR_VALIDATION_REQUIRED, 0);
                status = Status::Required;
            }
            else
            {
                DisplayHelper::showInfo(INFO_SENSOR_VALIDATION_SHOW_STATUS, showStatus);
            }

            break;
        }

        case Status::Required:
        {
            if (orbId == FD_KbdHandler)
            {
                if (key_pressed(BTN_OK))
                {
                    DisplayHelper::showInfo(INFO_SENSOR_VALIDATION_OK_TO_START, 0);
                    status = Status::OkToStart;
                }
            }
            break;
        }

        case Status::OkToStart:
        {
            if (orbId == FD_KbdHandler)
            {
                if (key_pressed(BTN_OK))
                {
                    time(&timeStarted);
                    status = Status::Countdown;
                    DisplayHelper::showInfo(INFO_SENSOR_VALIDATION_COUNTDOWN, 5);
                }
            }
            break;
        }

        case Status::Countdown:
        {
            time_t now;
            int gone;

            time(&now);

            gone = (int)(now - timeStarted);

            if (gone >= 5)
            {
                status = Status::Started;
                DisplayHelper::showInfo(INFO_SENSOR_VALIDATION_SHOW_STATUS, 0);
                request_sensor_check();
                sendAirDogCommnad(VEHICLE_CMD_PERFORM_SENSOR_VALIDATION, 1);
            }
            else
            {
                DisplayHelper::showInfo(INFO_SENSOR_VALIDATION_COUNTDOWN, 5 - gone);
            }

            break;
        }

        case Status::Started:
        {
            int showStatus = 0;
            bool isLeashOk;
            bool isDogOk;
            bool buttonOkAvailable = true;

            { // check airdog sensor status
                SENSOR_STATUS sensorStatus = DataManager::instance()->airdog_status.sensor_status;
                isDogOk = sensorStatus == SENSOR_STATUS_OK;

                if (sensorStatus == SENSOR_STATUS_OK)
                {
                    showStatus |= 1 << 4;
                }
                else if (sensorStatus == SENSOR_STATUS_WRONG)
                {
                	// incorrect tilt
                	showStatus |= 4 << 4;
                }
                else if (sensorStatus < SENSOR_STATUS_CHECK_NEEDED)
                {
                    // error
                    showStatus |= 2 << 4;
                }
                else
                {
                    buttonOkAvailable = false;
                }
            }

            { // check airleash sensor status
                SENSOR_STATUS sensorStatus = DataManager::instance()->sensor_status.combined_status;
                isLeashOk = sensorStatus == SENSOR_STATUS_OK;;

                if (sensorStatus == SENSOR_STATUS_OK)
                {
                    showStatus |= 1;
                }
                else if (sensorStatus == SENSOR_STATUS_WRONG)
                {
                	// incorrect tilt
                	showStatus |= 4;
                }
                else if (sensorStatus < SENSOR_STATUS_CHECK_NEEDED)
                {
                    // error
                    showStatus |= 2;
                }
                else
                {
                    buttonOkAvailable = false;
                }
            }

            if (isDogOk && isLeashOk)
            {
                next = new Main();
            }
            else
            {
                if (orbId == FD_KbdHandler)
                {
                    if (buttonOkAvailable && key_pressed(BTN_OK))
                    {
                        next = new SensorValidation();
                    }

                }

                if (next == nullptr)
                {
                    DisplayHelper::showInfo(INFO_SENSOR_VALIDATION_SHOW_STATUS, showStatus);
                }
            }
            break;
        }

        default:
            break;
    }

    if (orbId == FD_KbdHandler && key_pressed(BTN_MODE))
    {
        next = new Menu();
    }

    return next;
}

}

