/*
 * WattsUp.h
 *
 *  Created on: 13 Jun 2011
 *      Author: jack
 */

#ifndef WATTSUP_H_
#define WATTSUP_H_

#include <SerialStream.h>

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

    /**
     * Open serial connection
     */
    void openDevice();

    struct response {
        char    command;
        int     params,
                watts,  // Tenths of Watts
                volts,  // Tenths of Volts
                mAmps,  // miliamps
                wh,     // Tenths of Watt hours
                cost,   // Tenths of cents or other currency
                mokWh,
                moCost,
                maxWatts,
                maxVolts,
                maxAmps,
                minWatts,
                minVolts,
                minAmps,
                powerFactor,
                dutyCycle,
                powerCycle,
                lineFrequency, // Tenths of Hz
                va;     // Tenths of volta-amps
    } wattsUpResponse;

    void getResponse();
};

#endif /* WATTSUP_H_ */
