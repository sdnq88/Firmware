#include "bluetoothhelper.h"

#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#define _BLUETOOTH21_BASE       0x2d00

#define PAIRING_ON          _IOC(_BLUETOOTH21_BASE, 0)
#define PAIRING_OFF         _IOC(_BLUETOOTH21_BASE, 1)
#define DISCONNECT         _IOC(_BLUETOOTH21_BASE, 3)

int BluetoothHelper::pairing(bool start)
{
    int result = 0;
    int fd = open("/dev/btctl", 0);

    if (fd == -1) {
        result = false;
    }

    if (result == 0)
    {
        if (start)
        {
            ioctl(fd, PAIRING_ON, 0);
        }
        else
        {
            ioctl(fd, PAIRING_OFF, 0);
        }
    }

    close(fd);
    return result;
}

int BluetoothHelper::disconnect()
{
    int result = 0;
    int fd = open("/dev/btctl", 0);

    if (fd == -1) {
        result = false;
    }

    if (result == 0)
    {
        ioctl(fd, DISCONNECT, 0);
    }

    close(fd);
    return result;
}

