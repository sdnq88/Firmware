#pragma once

class BluetoothHelper
{
public:
    static int pairing(bool start = true);
    static int disconnect();
};
