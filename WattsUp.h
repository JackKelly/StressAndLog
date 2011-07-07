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

class WattsUp : public Sensor {
public:
    WattsUp();
    ~WattsUp();

    virtual void log();
    int get_deci_watts();

private:

    /***************************
     *   Member functions      *
     ***************************/
    void open_device();

    void initialise_response();

    void get_response();

    void send_init_command();

    /***************************
     *   Member variables      *
     ***************************/

    /**
     * for /dev/ttyUSB0
     */
    LibSerial::SerialStream wattsUpSerialPort;

    struct pair {
        std::string name;
        int    value;
    };

    pair response[MAX_PARAMS+1];
};

typedef Singleton<WattsUp> WattsUpSingleton;

#endif /* WATTSUP_H_ */
