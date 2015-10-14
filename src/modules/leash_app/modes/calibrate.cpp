#include "calibrate.h"

#include <stdio.h>
#include <unistd.h>

#include <systemlib/systemlib.h>

#include <uORB/uORB.h>
#include <uORB/topics/vehicle_command.h>

#include "menu.h"
#include "../datamanager.h"
#include "../displayhelper.h"
#include "../../airdog/calibrator/calibrator.hpp"
#include "../../airdog/calibrator/calibration_commons.hpp"
#include "../uorb_functions.h"

namespace modes
{

Calibrate::Calibrate(CalibrationDevice device, int returnEntry, int returnParam) :
    device(device),
    returnEntry(returnEntry),
    returnParam(returnParam)
{
    switch (device)
    {
        case CalibrationDevice::LEASH_ACCEL:
            DisplayHelper::showInfo(INFO_NEXT_SIDE_UP, 0);
            calibration::calibrate_in_new_task(calibration::CALIBRATE_ACCELEROMETER);
            break;

        case CalibrationDevice::LEASH_GYRO:
            DisplayHelper::showInfo(INFO_CALIBRATING_HOLD_STILL, 0);
            calibration::calibrate_in_new_task(calibration::CALIBRATE_GYROSCOPE);
            break;

        case CalibrationDevice::LEASH_MAGNETOMETER:
            DisplayHelper::showInfo(INFO_CALIBRATING_HOLD_STILL, 0);
            calibration::calibrate_in_new_task(calibration::CALIBRATE_MAGNETOMETER);
            break;

        case CalibrationDevice::AIRDOG_ACCEL:
            DisplayHelper::showInfo(INFO_CALIBRATING_AIRDOG, 0);
            sendAirDogCommnad(VEHICLE_CMD_PREFLIGHT_CALIBRATION, 0, 0, 0, 0, 1);
            break;

        case CalibrationDevice::AIRDOG_GYRO:
            DisplayHelper::showInfo(INFO_CALIBRATING_AIRDOG, 0);
            sendAirDogCommnad(VEHICLE_CMD_PREFLIGHT_CALIBRATION, 1);
            break;

        case CalibrationDevice::AIRDOG_MAGNETOMETER:
            DisplayHelper::showInfo(INFO_CALIBRATING_AIRDOG, 0);
            sendAirDogCommnad(VEHICLE_CMD_PREFLIGHT_CALIBRATION, 0, 1);
            break;

        default:
            break;
    }
}

int Calibrate::getTimeout()
{
    return Error::getTimeout();
}

void Calibrate::listenForEvents(bool awaitMask[])
{
    Error::listenForEvents(awaitMask);

    awaitMask[FD_KbdHandler] = 1;
    awaitMask[FD_Calibrator] = 1;
}

Base* Calibrate::doEvent(int orbId)
{
    Base *nextMode = nullptr;

    Error::doEvent(orbId);

    if (!isErrorShowed && lastErrorTime != 0)
    {
        // error screen is closed
        // go back
        nextMode = new Menu(returnEntry, returnParam);
    }
    else if (orbId == FD_KbdHandler)
    {
        if (key_pressed(BTN_BACK))
        {
            switch (device)
            {
                case CalibrationDevice::LEASH_ACCEL:
                case CalibrationDevice::LEASH_GYRO:
                case CalibrationDevice::LEASH_MAGNETOMETER:
                    calibration::calibrate_stop();
                    break;

                case CalibrationDevice::AIRDOG_ACCEL:
                case CalibrationDevice::AIRDOG_GYRO:
                case CalibrationDevice::AIRDOG_MAGNETOMETER:
                    sendAirDogCommnad(VEHICLE_CMD_PREFLIGHT_CALIBRATION, 0, 0, 0, 0, 0, 0, 1);
                    break;
            }
            nextMode = new Menu(returnEntry, returnParam);
        }
        else if (key_pressed(BTN_OK) &&
                 CALIBRATOR_FINISH == DataManager::instance()->calibrator.status)
        {
            nextMode = new Menu(returnEntry, returnParam);
        }
    }
    else if (orbId == FD_Calibrator)
    {
        int status = DataManager::instance()->calibrator.status;
        calibration::CALIBRATION_RESULT result = DataManager::instance()->calibrator.result;

        switch (status)
        {
            case CALIBRATOR_DETECTING_SIDE:
                DisplayHelper::showInfo(INFO_NEXT_SIDE_UP, 0);
                break;

            case CALIBRATOR_CALIBRATING:
                DisplayHelper::showInfo(INFO_CALIBRATING_HOLD_STILL, 0);
                break;

            case CALIBRATOR_DANCE:
                DisplayHelper::showInfo(INFO_CALIBRATING_DANCE, 0);
                break;

            case CALIBRATOR_FINISH:
                if (result == calibration::CALIBRATION_RESULT::SUCCESS)
                {
                    DisplayHelper::showInfo(INFO_SUCCESS, 0);
                }
                else
                {
                    DisplayHelper::showInfo(INFO_FAILED, 0);
                }
                break;
        }
    }

    return nextMode;
}

}
