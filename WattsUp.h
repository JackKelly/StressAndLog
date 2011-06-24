/*
 * WattsUp.h
 *
 *  Created on: 13 Jun 2011
 *      Author: jack
 */

#ifndef WATTSUP_H_
#define WATTSUP_H_

// Maximum number of parameters returned by meter
#define MAX_PARAMS 18

#include <SerialStream.h>
#include "Singleton.h"

using namespace std;

class WattsUp {
public:
    WattsUp();
    ~WattsUp();

    int getWatts();

private:

    /**
     * for /dev/ttyUSB0
     */
    LibSerial::SerialStream wattsUpSerialPort;

    void openDevice();

    struct pair {
        string name;
        int    value;
    };

    pair response[MAX_PARAMS+1];

    void initialiseResponse();

    void getResponse();

    void sendInitCommand();
};

typedef Singleton<WattsUp> WattsUpSingleton;

#endif /* WATTSUP_H_ */
