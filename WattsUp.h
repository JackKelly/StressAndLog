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

using namespace std;

class WattsUp {
public:
    WattsUp();
    ~WattsUp();

    /**
     * @returns Current number of Watts*10
     */
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

    struct pair {
        string name;
        int    value;
    };

    pair response[MAX_PARAMS+1];

    void initialiseResponse();

    void getResponse();
};

#endif /* WATTSUP_H_ */
