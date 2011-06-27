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
#include "Sensor.h"

using namespace std;

class WattsUp : public Sensor {
public:
    WattsUp();
    ~WattsUp();

    virtual void log();
    int get_deci_watts();

private:

    /**
     * for /dev/ttyUSB0
     */
    LibSerial::SerialStream wattsUpSerialPort;

    void open_device();

    struct pair {
        string name;
        int    value;
    };

    pair response[MAX_PARAMS+1];

    void initialise_response();

    void get_response();

    void send_init_command();
};

typedef Singleton<WattsUp> WattsUpSingleton;

#endif /* WATTSUP_H_ */
